# PoGoCmp

[![Build Status](https://travis-ci.org/Stinkfist0/PoGoCmp.svg?branch=master)](https://travis-ci.org/Stinkfist0/PoGoCmp)
[![Build status](https://ci.appveyor.com/api/projects/status/0w5ik8sbxfp95soy/branch/master?svg=true)](https://ci.appveyor.com/project/Stinkfist0/pogocmp/branch/master)

## Prerequisites
- CMake (3.0.2 or newer)
- C++14-capable compiler

## Dependencies
 - [JSON for Modern C++ 3.1.1](https://github.com/nlohmann/json), embedded in the repository

## Configuration
See the beginning of the root `CMakeLists.txt` for the build options.
Pass in the 
```
mkdir build && cd build
cmake ..
```

## Building

Either
```
cd build
cmake --build . --config RelWithDebInfo
```
or simply perform Installing as described below if you are simlpy interested in the executables.

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
ctest
```

## Running
Assuming current directory 'build' and `CMAKE_INSTALL_PREFIX` was not modified:
```
../installed/bin/PoGoCmpCli --help
```

