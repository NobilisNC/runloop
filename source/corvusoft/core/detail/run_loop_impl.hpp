/*
 * Copyright 2016-2017, Corvusoft Ltd, All Rights Reserved.
 */

#ifndef _CORVUSOFT_CORE_DETAIL_RUN_LOOP_IMPL_H
#define _CORVUSOFT_CORE_DETAIL_RUN_LOOP_IMPL_H 1

//System Includes
#include <map>
#include <tuple>
#include <mutex>
#include <string>
#include <chrono>
#include <atomic>
#include <vector>
#include <thread>
#include <memory>
#include <ciso646>
#include <csignal>
#include <stdexcept>
#include <functional>
#include <system_error>
#include <algorithm>
#include <condition_variable>

//Project Includes
#include "corvusoft/core/detail/task_impl.hpp"

//External Includes

//System Namespaces

//Project Namespaces

//External Namespaces

namespace corvusoft
{
    //Forward Declarations
    
    namespace core
    {
        //Forward Declarations
        
        namespace detail
        {
            //Forward Declarations
            
            static std::atomic< std::sig_atomic_t > raised_signal( 0 );
            
            struct RunLoopImpl
            {
                static void signal_handler( const int value )
                {
                    raised_signal = value;
                }
                
                std::mutex task_lock { };
                
                std::condition_variable pending_work { };
                
                std::atomic< bool > is_stopped { true };
                
                std::atomic< bool > is_suspended { false };
                
                std::vector< TaskImpl > tasks { };
                
                std::vector< std::thread > workers { };
                
                unsigned int worker_limit = std::thread::hardware_concurrency( );
                
                std::function< std::error_code ( void ) > ready_handler = nullptr;
                
                std::function< std::error_code ( const std::error_code&, const std::string& ) > log_handler = nullptr;
                
                std::function< std::error_code ( const std::string&, const std::error_code&, const std::string& ) > error_handler = nullptr;
                
                const std::function< bool ( void ) > until_work_available = [ this ]( void )
                {
                    return ( is_stopped or not tasks.empty( ) or raised_signal ) and not is_suspended;
                };
                
                const std::function< void ( void ) > dispatch = [ this ]( void )
                {
                    TaskImpl task { };
                    std::vector< TaskImpl >::iterator position { };
                    std::unique_lock< std::mutex > guard( task_lock );
                    
                    while ( is_stopped == false )
                    {
                        pending_work.wait( guard, until_work_available );
                        
                        if ( is_stopped ) break;
                        if ( guard.owns_lock( ) and not is_suspended )
                        {
                            position = std::find_if( tasks.begin( ), tasks.end( ), is_ready );
                            if ( position not_eq tasks.end( ) )
                            {
                                std::swap( task, *position );
                                tasks.erase( position );
                                
                                guard.unlock( );
                                pending_work.notify_one( );
                                
                                if ( task.error.code == std::error_code( ) ) launch( task );
                                if ( task.error.code not_eq std::error_code( ) ) error( task );
                            }
                            else
                            {
                                guard.unlock( );
                                pending_work.notify_one( );
                            }
                            
                            guard.lock( );
                        }
                    }
                    
                    if ( guard.owns_lock( ) ) guard.unlock( );
                    pending_work.notify_one( );
                };
                
                static bool is_ready( TaskImpl& task )
                try
                {
                    if ( task.condition == nullptr ) return true;
                    
                    task.error.code = task.condition( );
                    
                    if ( task.error.code == std::error_code( ) )                 return true;
                    if ( task.error.code == std::errc::operation_canceled )      return true;
                    if ( task.error.code == std::errc::operation_not_permitted ) return false;
                    
                    task.error.message = "task precondition failed with an unsuccessful error state.";
                    return true;
                }
                catch ( const std::exception& ex )
                {
                    task.error.code = std::make_error_code( std::errc::operation_canceled );
                    task.error.message = "std::exception raised when calling task precondition: " + std::string( ex.what( ) );
                    return true;
                }
                catch ( ... )
                {
                    task.error.code = std::make_error_code( std::errc::operation_canceled );
                    task.error.message = "non-std::exception raised when calling task precondition.";
                    return true;
                }
                
                void launch( TaskImpl& task )
                try
                {
                    task.error.code = task.operation( );
                    if ( task.error.code not_eq std::error_code( ) )
                        task.error.message = "task execution failed with an unsuccessful error state.";
                }
                catch ( const std::exception& ex )
                {
                    task.error.message = ex.what( );
                    task.error.code = std::make_error_code( std::errc::operation_canceled );
                    error( task );
                }
                catch ( ... )
                {
                    task.error.code = std::make_error_code( std::errc::operation_canceled );
                    task.error.message = "non-std::exception raised when calling task execution.";
                    error( task );
                }
                
                void error( const TaskImpl& task )
                {
                    error( task.key, task.error.code, task.error.message );
                }
                
                void error( const std::error_code& code, const std::string& message )
                {
                    error( "", code, message );
                }
                
                void error( const std::string& key, const std::error_code& code, const std::string& message )
                try
                {
                    if ( error_handler not_eq nullptr )
                    {
                        const std::error_code status = error_handler( key, code, message );
                        if ( status not_eq std::error_code( ) )
                        {
                            log( "task '" + key + "' failed with '" + message + "'.", code );
                            log( "error handler failed with unsuccessful error state.", status );
                        }
                    }
                }
                catch ( const std::exception& ex )
                {
                    log( "task '" + key + "' failed with '" + message + "'.", code );
                    log( "std::exception raised when calling error handler: " + std::string( ex.what( ) ) );
                }
                catch ( ... )
                {
                    log( "task '" + key + "' failed with '" + message + "'.", code );
                    log( "non-std::exception raised when calling error handler." );
                }
                
                void log( const std::string& message, const std::error_code& code = std::error_code( ) )
                try
                {
                    if ( log_handler not_eq nullptr )
                    {
                        const auto status = log_handler( code, message );
                        if ( status not_eq std::error_code( ) )
                        {
                            fprintf( stderr, "[%i %s] %s", code.value( ), code.message( ).data( ), message.data( ) );
                            fprintf( stderr, "[%i %s] log handler failed with unsuccessful error state.", status.value( ), status.message( ).data( ) );
                        }
                    }
                }
                catch ( const std::exception& ex )
                {
                    fprintf( stderr, "[%i %s] %s", code.value( ), code.message( ).data( ), message.data( ) );
                    fprintf( stderr, "std::exception raised when attempting to log failure: %s", ex.what( ) );
                }
                catch ( ... )
                {
                    fprintf( stderr, "[%i %s] %s", code.value( ), code.message( ).data( ), message.data( ) );
                    fprintf( stderr, "non-std::exception raised when attempting to log failure." );
                }
                
                void log( const TaskImpl& task, const std::string& message, const std::error_code& code )
                {
                    log( message, code );
                    log( task.error.message, task.error.code );
                }
            };
        }
    }
}

#endif  /* _CORVUSOFT_CORE_DETAIL_RUN_LOOP_IMPL_H */
