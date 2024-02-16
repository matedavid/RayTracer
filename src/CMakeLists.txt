project(RayTracerLib LANGUAGES CXX)

# Create library
add_library(${PROJECT_NAME})

# target_compile_options(${PROJECT_NAME} BEFORE PRIVATE -Wall -Wpedantic -Wextra -Wshadow -Wconversion -Werror -fopenmp)
target_compile_options(${PROJECT_NAME} BEFORE PRIVATE -Wall -Wpedantic -Wextra -Wshadow -Wconversion -fopenmp)

# Sources and include directories
target_sources(${PROJECT_NAME} PRIVATE
        aabb.cpp
        camera.cpp
        image_dumper.cpp
        material.cpp
        rand.cpp
        ray.cpp
        ray_tracer.cpp
        vec.cpp
        texture.cpp

        # hittable
        hittable/sphere.cpp
        hittable/triangle.cpp
        hittable/model.cpp
        hittable/hittable_list.cpp
        hittable/bvh_node.cpp
)

# Include directory for lib
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

#
# Dependencies
#
include(FetchContent)

# glm
FetchContent_Declare(
        glm
        GIT_REPOSITORY https://github.com/g-truc/glm.git
        GIT_TAG 0.9.9.8
        GIT_PROGRESS TRUE
)
FetchContent_MakeAvailable(glm)
target_link_libraries(${PROJECT_NAME} PUBLIC glm::glm)

# stb
FetchContent_Declare(
        stb
        GIT_REPOSITORY https://github.com/nothings/stb.git
        GIT_PROGRESS TRUE
)
FetchContent_MakeAvailable(stb)
target_include_directories(${PROJECT_NAME} PRIVATE ${stb_SOURCE_DIR})

# assimp
FetchContent_Declare(
        assimp
        GIT_REPOSITORY https://github.com/assimp/assimp
        GIT_TAG v5.3.1
        GIT_PROGRESS TRUE
)

set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(ASSIMP_INJECT_DEBUG_POSTFIX OFF CACHE BOOL "" FORCE)
set(ASSIMP_INSTALL OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(assimp)
target_link_libraries(${PROJECT_NAME} PRIVATE assimp::assimp)

# OpenMP
find_package(OpenMP REQUIRED)

if (OpenMP_FOUND)
    target_compile_options(${PROJECT_NAME} PRIVATE ${OpenMP_CXX_FLAGS})
    target_link_libraries(${PROJECT_NAME} PRIVATE OpenMP::OpenMP_CXX)
endif ()
