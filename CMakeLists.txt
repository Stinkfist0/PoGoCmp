cmake_minimum_required(VERSION 3.0.2)

project(PoGoCmp)

option(BUILD_SHARED_LIBS "Build PoGoCmp as a shared lib." OFF)
option(BUILD_CLI "Build the CLI application" ON)
option(BUILD_DBGEN "Build the database code generator" ON)

if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX "${PROJECT_SOURCE_DIR}/installed"
        CACHE PATH "default install path" FORCE)
endif()
message(STATUS "CMAKE_INSTALL_PREFIX set to ${CMAKE_INSTALL_PREFIX}")

if (MSVC)
    add_compile_options(/W4 /std:c++14)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -std=c++14)
endif()

if (BUILD_SHARED_LIBS)
    add_definitions(-DPOGOCMP_SHARED_BUILD)
endif()

add_subdirectory(src/PoGoCmp)

if (BUILD_CLI)
    add_subdirectory(src/PoGoCmpCli)
endif()

if (BUILD_DBGEN)
    add_subdirectory(src/PoGoCmpDbGen)
endif()
