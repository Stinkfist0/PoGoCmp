# PoGoCmp

[![Build Status](https://travis-ci.org/Stinkfist0/PoGoCmp.svg?branch=master)](https://travis-ci.org/Stinkfist0/PoGoCmp)
[![Build status](https://ci.appveyor.com/api/projects/status/0w5ik8sbxfp95soy/branch/master?svg=true)](https://ci.appveyor.com/project/Stinkfist0/pogocmp/branch/master)
[![codecov](https://codecov.io/gh/Stinkfist0/PoGoCmp/branch/master/graph/badge.svg)](https://codecov.io/gh/Stinkfist0/PoGoCmp)

A simple command-line tool for Pokémon GO. Motivation for this project was mainly to keep my fingers warm with 
modern C++ and its ecosystem (build system, testing, CI, package manager, acknowledged libraries) even though 
it might not be the easiest nor the fastest approach. The aim is to have a proper cross-platform (incl. mobile OSs) 
GUI application at some point (GUI probably implemented using Cocos2d-x).

## Supported platforms
- Windows & MSVC
- *nix & GCC/Clang (macOS untested currently)

## Prerequisites
- CMake (3.0.2 or newer)
- C++17-capable compiler

## Dependencies
 - [JSON for Modern C++](https://github.com/nlohmann/json)
 - [Range-v3](https://github.com/ericniebler/range-v3)

The dependencies for the project are obtained using [vcpkg](https://vcpkg.readthedocs.io). Obtain vcpkg
if necessary:
```
> git clone https://github.com/Microsoft/vcpkg.git
> cd vcpkg
> .\bootstrap-vcpkg.bat (or ./bootstrap-vcpkg.sh on *nix)
```
and install the dependencies:
```
> set VCPKG_DEFAULT_TRIPLET=x64-windows (x86 by default)
> .\vcpkg.exe install nlohmann-json range-v3
```

## Configuration
See the beginning of the root `CMakeLists.txt` for the build options.
```
mkdir build && cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=/path/to/vcpkg/scripts/buildsystems/vcpkg.cmake -DEXAMPLE_OPTION=1
```

## Building

Either
```
cd build
cmake --build . --config RelWithDebInfo
```
or simply perform *Installing* as described below, if you are simply interested in the executables.

## Installing
 Use `CMAKE_INSTALL_PREFIX` (defaults to `<projectDir>/installed`) to control the installation location.
Delete `build/CMakeCache.txt` if changing this between builds.
```
cd build
cmake --build . --target INSTALL --config RelWithDebInfo
```

## Testing
If building of tests was enabled (`BUILD_TESTS=1`, on by default), simply run `ctest` in the build directory:
```
cd build
ctest --verbose
```

## Running
Assuming current directory `<projectDir>/build` and `CMAKE_INSTALL_PREFIX` was not modified:
```
../installed/bin/PoGoCmpCli --help
```

