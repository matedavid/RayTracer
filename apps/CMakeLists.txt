cmake_minimum_required(VERSION 3.26)

project(RayTracerPlayground LANGUAGES CXX)

# Create library
add_executable(${PROJECT_NAME} main.cpp)

target_include_directories(${PROJECT_NAME} PRIVATE ../src/)
target_link_libraries(${PROJECT_NAME} PRIVATE RayTracerLib)
