# Playground
add_executable(RayTracerPlayground playground/main.cpp)
target_link_libraries(RayTracerPlayground PRIVATE RayTracerLib)

# Renderer
add_executable(RayTracerRenderer 
    renderer/main.cpp
)
target_link_libraries(RayTracerRenderer PRIVATE RayTracerLib)