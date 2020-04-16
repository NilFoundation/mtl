# Nil Foundation's MultiThreading Library: C++ Actor Library

ACTOR is an open source C++ actor model implementation featuring lightweight & fast actor implementations, pattern matching for messages, network transparent messaging, and more.

## Building

ACTOR uses CMake [CMake](http://www.cmake.org/) build system and follows standard procedure for building:

```
git clone --recurse-submodules https://github.com/nilfoundation/actor.git actor
cd actor && mkdir build && cd build && cmake .. && make -j4 all
```

ACTOR also can be included as CMake submodule or added as dependency to other
CMake-based projects using the file `cmake/FindACTOR.cmake`.

### CMake build options available

* ```BUILD_TESTS``` - enables building unit tests
* ```BUILD_SHARED_LIBS``` - enables building shared libraries (CMake default variable)
* ```BUILD_WITH_CCACHE``` - enables building with CCache usage.

## Dependencies

* CMake
* Boost ( >= 1.58)
* Pthread (until C++11 compilers support the new `thread_local` keyword)

## Supported Compilers

* GCC >= 4.8.3
* Clang >= 3.2
* MSVC >= 2015, update 3

## Supported Operating Systems

* Linux
* Mac OS X
* FreeBSD 10
* Windows >= 7 (currently static builds only)

## Optional Dependencies

* Doxygen (for the `doxygen` target)
* LaTeX (for the `manual` target)
* Pandoc and Python with pandocfilters (for the `rst` target)
* CCache (for caching the build)
