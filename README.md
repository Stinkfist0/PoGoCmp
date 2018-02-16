# PoGoCmp

[![Build Status](https://travis-ci.org/Stinkfist0/PoGoCmp.svg?branch=master)](https://travis-ci.org/Stinkfist0/PoGoCmp)
[![Build status](https://ci.appveyor.com/api/projects/status/0w5ik8sbxfp95soy/branch/master?svg=true)](https://ci.appveyor.com/project/Stinkfist0/pogocmp/branch/master)

## Prerequisites
- CMake (3.0.2 or newer)
- C++14 capable compiler

## Configuration
See the beginning of the root `CMakeLists.txt` for the build options.

## Dependencies
 - [JSON for Modern C++ 3.1.1](https://github.com/nlohmann/json), embedded in the repository

## Building 

```
mkdir build && cd build
cmake ..
```

## Installing
 Use `CMAKE_INSTALL_PREFIX` (defaults to `<projectDir>/installed`) to control the installation location.
Delete `build/CMakeCache.txt` if changing this between builds.
```
cd build
cmake --build . --target INSTALL
```

## Testing
If buildin of tests was enable, simply run `ctest` in the build directory:
```
cd build
ctest
```