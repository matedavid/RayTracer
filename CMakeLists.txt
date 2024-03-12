cmake_minimum_required(VERSION 3.20)

project(RayTracer)

# Project settings
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN YES)

# Options
set(BUILD_TESTS ON CACHE BOOL "Compile tests for the library")
set(BUILD_APPS ON CACHE BOOL "Compile applications")

# Include library
add_subdirectory(src)

# Include apps
if (BUILD_APPS)
    add_subdirectory(apps)
endif ()

# Tests
if (BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif ()
