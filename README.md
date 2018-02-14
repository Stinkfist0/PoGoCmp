# PoGoCmp

## Prerequisites
- CMake (3.0.2 or newer)
- C++14 capable compiler

## Configuration
See the beginning of `CMakeLists.txt` for the build options.

## Dependencies
 - None currently

## Building 

```
mkdir build && cd build
cmake ..
```

## Installing
 Use `CMAKE_INSTALL_PREFIX` (defaults to `<project>/installed`) to control the installation location.
Delete `build/CMakeCache.txt` if changing this between builds.
```
cd build
cmake --build . --target INSTALL
```
