cmake_minimum_required(VERSION 3.26)

project(RayTracerLib LANGUAGES CXX)

# Project settings
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Create library
add_library(${PROJECT_NAME})

# target_compile_options(${PROJECT_NAME} BEFORE PRIVATE -Wall -Wpedantic -Wextra -Wshadow -Wconversion -Werror -fopenmp)
target_compile_options(${PROJECT_NAME} BEFORE PRIVATE -Wall -Wpedantic -Wextra -Wshadow -Wconversion -fopenmp)

# Sources and include directories
target_sources(${PROJECT_NAME} PRIVATE
        aabb.cpp
        camera.cpp
        hittable.cpp
        image_dumper.cpp
        material.cpp
        rand.cpp
        ray.cpp
        ray_tracer.cpp
        vec.cpp
        texture.cpp
)

target_include_directories(${PROJECT_NAME} PUBLIC src/)

#
# Dependencies
#
include(FetchContent)

# glm
FetchContent_Declare(
        glm
        GIT_REPOSITORY https://github.com/g-truc/glm.git
        GIT_TAG 0.9.9.8
)
FetchContent_MakeAvailable(glm)
target_link_libraries(${PROJECT_NAME} PUBLIC glm::glm)

# stb
FetchContent_Declare(
        stb
        GIT_REPOSITORY https://github.com/nothings/stb.git
)
FetchContent_MakeAvailable(stb)
target_include_directories(${PROJECT_NAME} PRIVATE ${stb_SOURCE_DIR})

# OpenMP
find_package(OpenMP REQUIRED)

if (OpenMP_FOUND)
    target_compile_options(${PROJECT_NAME} PRIVATE ${OpenMP_CXX_FLAGS})
    target_link_libraries(${PROJECT_NAME} PRIVATE OpenMP::OpenMP_CXX)
endif ()
