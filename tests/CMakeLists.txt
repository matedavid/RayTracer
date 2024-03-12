project(Tests LANGUAGES CXX)

add_executable(${PROJECT_NAME})
target_sources(${PROJECT_NAME} PRIVATE
        hittable/sphere_tests.cpp
        hittable/triangle_tests.cpp
)

target_compile_options(${PROJECT_NAME} BEFORE PRIVATE -Wall -Wpedantic -Wextra -Wshadow -Wconversion)

#
# Dependencies
#
Include(FetchContent)

FetchContent_Declare(
        Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG v3.4.0
        GIT_PROGRESS TRUE
)
FetchContent_MakeAvailable(Catch2)
target_link_libraries(${PROJECT_NAME} PRIVATE Catch2::Catch2WithMain)

# Link RayTracer library
target_link_libraries(${PROJECT_NAME} PRIVATE RayTracerLib)

add_test(NAME Tests COMMAND ${PROJECT_NAME})
