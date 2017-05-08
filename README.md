Runloop [![Unix Build Status](https://travis-ci.org/Corvusoft/runloop.svg?branch=master)](https://travis-ci.org/corvusoft/runloop)
[![Windows Build Status](https://ci.appveyor.com/api/projects/status/99j5k44bcl2o3e23/branch/master?svg=true)](https://ci.appveyor.com/project/corvusoft/runloop/branch/master)
=============================================================================================================================

Example
-------

```C++
#include <memory>
#include <cstdlib>
#include <corvusoft/core/run_loop.hpp>

using namespace std;
using corvusoft::core::RunLoop;

int main( const int, const char** )
{
    int count = 0;
    const auto counter = [ &count ]( void ) { count++; return success; };
            
    auto runloop = make_shared< RunLoop >( );
    runloop->launch( counter );
    runloop->launch( counter );
    runloop->launch( counter );
    runloop->launch( [ runloop ]( void )
    {
        runloop->wait( );
        runloop->stop( );
        return success;
    } );

    runloop->start( );

    return EXIT_SUCCESS;
}
```

License
-------

&copy; 2013-2017 Corvusoft Limited, United Kingdom. All rights reserved.

The Runloop framework is dual licensed; See [LICENSE](LICENSE) for full details.

Support
-------

Please contact sales@corvusoft.co.uk, for support and licensing options including bespoke software development, testing, design consultation, training, mentoring and code review.

Build
-----

```bash
git clone --recursive https://github.com/corvusoft/runloop.git
mkdir runloop/build
cd runloop/build
cmake ..
make [-j CPU_CORES+1] install
make test
```

You will now find all required components installed in the distribution folder.

Please submit all enhancements, proposals, and defects via the [issue](http://github.com/corvusoft/restbed/issues) tracker.

Documentation
-------------

This codebase is intended to be as self documenting as possible. We have supplied many [test cases](https://github.com/corvusoft/restbed/tree/master/test) to help aid developers.

You can locate the latest API documentation [here](https://github.com/Corvusoft/runloop/tree/master/documentation).

Minimum Requirements
--------------------

| Resource | Requirement                                     |
|:--------:|:-----------------------------------------------:|
| Compiler |            C++14 compliant or above             |
|    OS    |          BSD, Linux, Mac OSX, Windows           |

Contact
-------

| Method                                      | Description                                 |
|:--------------------------------------------|:--------------------------------------------|
| [Twitter](http://www.twitter.com/corvusoft) | Tweet us your questions & feature requests. |
| support@corvusoft.co.uk                     | Support related queries.                    |
| sales@corvusoft.co.uk                       | Sale related queries.                       |
