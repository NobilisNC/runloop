/*
 * Copyright 2013-2018, Corvusoft Ltd, All Rights Reserved.
 */

#ifndef _CORVUSOFT_MOCK_RUN_LOOP_H
#define _CORVUSOFT_MOCK_RUN_LOOP_H 1

//System Includes
#include <regex>
#include <chrono>
#include <memory>
#include <string>
#include <functional>
#include <system_error>

//Project Includes
#include <corvusoft/core/run_loop.hpp>

//External Includes

//System Namespaces

//Project Namespaces

//External Namespaces

namespace corvusoft
{
    //Forward Declarations
    namespace core
    {
        class RunLoop;
    }
    
    namespace mock
    {
        //Forward Declarations
        
        class RunLoop final : public core::RunLoop
        {
            public:
                //Friends
                
                //Definitions
                
                //Constructors
                RunLoop( void ) : core::RunLoop( )
                {
                    return;
                }
                
                virtual ~RunLoop( void )
                {
                    return;
                }
                
                //Functionality
                bool is_stopped( void ) const
                {
                    return is_mock_stopped;
                }
                
                bool is_suspended( void ) const
                {
                    return is_mock_suspended;
                }
                
                void resume( void )
                {
                    is_mock_suspended = false;
                }
                
                void suspend( void )
                {
                    is_mock_suspended = true;
                }
                
                void stop( void )
                {
                    is_mock_stopped = true;
                }
                
                void start( void )
                {
                    is_mock_stopped = false;
                }
                
                void cancel( const std::string& )
                {
                    return;
                }
                
                void cancel( const std::regex& )
                {
                    return;
                }
                
                void wait( const std::string& )
                {
                    return;
                }
                
                void wait( const std::regex& )
                {
                    return;
                }
                
                void wait( const std::chrono::milliseconds& )
                {
                    return;
                }
                
                void launch( const std::function< std::error_code ( void ) >& task, const std::string& )
                {
                    task( );
                }
                
                void launch_if( const bool condition, const std::function< std::error_code ( void ) >& task, const std::string& )
                {
                    if ( condition ) task( );
                }
                
                void launch_on( const signal_t, const std::function< std::error_code ( void ) >& task, const std::string& )
                {
                    task( );
                }
                
                void launch_in( const std::chrono::milliseconds&, const std::function< std::error_code ( void ) >& task, const std::string& )
                {
                    task( );
                }
                
                void launch_at( const std::chrono::time_point< std::chrono::system_clock >&, const std::function< std::error_code ( void ) >& task, const std::string& )
                {
                    task( );
                }
                
                //Getters
                
                //Setters
                void set_worker_limit( const unsigned int )
                {
                    return;
                }
                
                void set_ready_handler( const std::function< std::error_code ( void ) >& )
                {
                    return;
                }
                
                void set_error_handler( const std::function< void ( const std::string&, const std::error_code&, const std::string& ) >& )
                {
                    return;
                }
                
                void set_log_handler( const std::function< std::error_code ( const std::string&, const std::error_code&, const std::string& ) >& )
                {
                    return;
                }
                
                //Operators
                
                //Properties
                
            protected:
                //Friends
                
                //Definitions
                
                //Constructors
                
                //Functionality
                
                //Getters
                
                //Setters
                
                //Operators
                
                //Properties
                
            private:
                //Friends
                
                //Definitions
                
                //Constructors
                RunLoop( const RunLoop& original ) = delete;
                
                //Functionality
                
                //Getters
                
                //Setters
                
                //Operators
                RunLoop& operator =( const RunLoop& value ) = delete;
                
                //Properties
                bool is_mock_stopped = true;
                
                bool is_mock_suspended = false;
        };
    }
}

#endif  /* _CORVUSOFT_MOCK_RUN_LOOP_H */