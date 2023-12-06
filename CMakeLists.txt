cmake_minimum_required(VERSION 3.26)

project(RayTracer LANGUAGES CXX)

# Project settings
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Create executable
add_executable(${PROJECT_NAME})

target_compile_options(${PROJECT_NAME} BEFORE PRIVATE -Wall -Wpedantic -Wextra -Wshadow -Wconversion -Werror)
set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN YES)

# Sources and include directories
target_sources(${PROJECT_NAME} PRIVATE
        src/main.cpp
)

target_include_directories(${PROJECT_NAME} PUBLIC src/)
