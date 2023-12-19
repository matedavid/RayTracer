cmake_minimum_required(VERSION 3.26)

project(Tests LANGUAGES CXX)

#
# Dependencies
#
Include(FetchContent)

FetchContent_Declare(
        Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG v3.4.0
)

FetchContent_MakeAvailable(Catch2)

add_executable(${PROJECT_NAME})
target_sources(${PROJECT_NAME} PRIVATE
        hittable/sphere_tests.cpp
        hittable/triangle_tests.cpp
)
target_include_directories(${PROJECT_NAME} PRIVATE ../src/)
target_link_libraries(${PROJECT_NAME} PRIVATE Catch2::Catch2WithMain)

target_compile_options(${PROJECT_NAME} BEFORE PRIVATE -Wall -Wpedantic -Wextra -Wshadow -Wconversion -fopenmp)

# Link RayTracer library
target_link_libraries(${PROJECT_NAME} PRIVATE RayTracerLib)

