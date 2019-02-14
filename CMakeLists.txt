cmake_minimum_required(VERSION 3.0.2)

project(PoGoCmp CXX)

find_package(nlohmann_json CONFIG REQUIRED)

# BUILD_SHARED_LIBS is provided by CMake, typed explicitly here for documentation purposes
option(BUILD_SHARED_LIBS "Build PoGoCmp as a shared lib." OFF)
option(BUILD_CLI "Build the CLI application" ON)
option(BUILD_DBGEN "Build the database code generator" ON)
option(BUILD_TESTS "Build the tests" ON)
if(CMAKE_COMPILER_IS_GNUCC)
    option(ENABLE_COVERAGE "Enable coverage reporting for gcc/clang" OFF)
    if (ENABLE_COVERAGE)
        add_compile_options(--coverage -O0)
    endif()
endif()

if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX "${PROJECT_SOURCE_DIR}/installed"
        CACHE PATH "default install path" FORCE)
endif()
message(STATUS "CMAKE_INSTALL_PREFIX set to ${CMAKE_INSTALL_PREFIX}")

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

if (MSVC)
    # With MSVC, VS 2017 or newer assumed
    add_compile_options(/W4 /std:c++17 /utf-8 /permissive-)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -std=c++17 -fvisibility=hidden -Wshadow)
endif()

if (WIN32)
    # Enforce Unicode for Win32 and CRT APIs.
    add_definitions(-DUNICODE -D_UNICODE)
endif()

if (BUILD_SHARED_LIBS)
    add_definitions(-DPOGOCMP_SHARED_BUILD)
endif()

add_subdirectory(src/Lib)

if (BUILD_CLI)
    add_subdirectory(src/Cli)
endif()

if (BUILD_DBGEN)
    add_subdirectory(src/DbGen)
endif()

if (BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
