include(FetchContent)

# Playground
add_executable(RayTracerPlayground playground/main.cpp)
target_link_libraries(RayTracerPlayground PRIVATE RayTracerLib)

# Renderer
add_executable(RayTracerRenderer
        renderer/main.cpp
        renderer/scene_parser.cpp
)
target_link_libraries(RayTracerRenderer PRIVATE RayTracerLib)

FetchContent_Declare(
        json
        GIT_REPOSITORY https://github.com/nlohmann/json.git
        GIT_TAG v3.11.3
        GIT_PROGRESS TRUE
)
FetchContent_MakeAvailable(json)
target_link_libraries(RayTracerRenderer PRIVATE nlohmann_json::nlohmann_json)
