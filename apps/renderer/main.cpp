#include <iostream>

#include "scene_parser.h"

#include "hittable.h"
#include "camera.h"
#include "ray_tracer.h"
#include "image_dumper.h"

int main(int32_t argc, const char* argv[]) {
    if (argc < 2) {
        std::cout << "Usage: ./RayTracerRenderer <scene_file>.json\n";
        return 1;
    }

    const std::string scene_file = argv[1];
    const auto parser = SceneParser::parse(scene_file);
    if (!parser)
        return 1;

    const auto desc = parser->camera_description();

    Camera camera(parser->camera_description());
    PPMImageDumper image(camera.width(), camera.height());

    const auto bvh_scene = BVHNode(*parser->scene());

    const RayTracer ray_tracer({
        .samples_per_pixel = 600,
        .max_depth = 200,
        .num_threads = 12,
    });

    ray_tracer.render(camera, bvh_scene, image);

    image.dump("output.ppm");

    return 0;
}