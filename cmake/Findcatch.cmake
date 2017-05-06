find_path( catch_INCLUDE catch.hpp HINTS "${CMAKE_SOURCE_DIR}/dependency/catch/include" )

if ( catch_INCLUDE )
    set( CATCH_FOUND TRUE )
    message( STATUS "Located Catch include at: ${catch_INCLUDE}" )
else ( )
    message( FATAL_ERROR "Failed to locate Catch dependency." )
endif ( )