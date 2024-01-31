#include "camera.h"
#include "hittable.h"
#include "material.h"
#include "image_dumper.h"
#include "rand.h"
#include "ray_tracer.h"

constexpr uint32_t IMAGE_WIDTH = 1920;
constexpr uint32_t IMAGE_HEIGHT = static_cast<uint32_t>(IMAGE_WIDTH / (16.0f / 9.0f));

void create_scene(HittableList& scene);

int main() {
    // Define camera
    const Camera camera({
        .width = IMAGE_WIDTH,
        .height = IMAGE_HEIGHT,
        .vertical_fov = glm::radians(20.0f),
        .look_from = vec3(13.0, 2.0, 3.0),
        .look_at = vec3(0.0, 0.0, 0.0),
        .up = vec3(0.0, 1.0, 0.0),
    });

    // Image dumper
    PPMImageDumper image(IMAGE_WIDTH, IMAGE_HEIGHT);

    // Create scene
    HittableList scene;
    // create_scene(scene);

    auto material1 = std::make_shared<Dielectric>(1.5);
    scene.add_hittable<Sphere>(vec3(0, 1, 0), 1.0, material1);

    auto material2 = std::make_shared<Lambertian>(vec3(1.0, 0.0, 0.0));
    scene.add_hittable<Sphere>(vec3(4, 1, 0), 1.0, material2);

    auto material3 = std::make_shared<Metal>(vec3(0.7, 0.6, 0.5), 0.0);
    scene.add_hittable<Sphere>(vec3(-4, 1, 0), 1.0, material3);

    auto material4 = std::make_shared<DiffuseEmissive>(vec3(1.0), 4.0);
    scene.add_hittable<Sphere>(vec3(2, 4, 0), 1.0, material4);

    auto ground_material = std::make_shared<Lambertian>(vec3(0.5, 0.5, 0.5));
    scene.add_hittable<Sphere>(vec3(0, -1000, 0), 1000, ground_material);

    // Render
    const RayTracer ray_tracer({
        .samples_per_pixel = 100,
        .max_depth = 50,
        .num_threads = 8,
    });

    const auto bvh_scene = BVHNode(scene);

    ray_tracer.render(camera, bvh_scene, image);

    // Save image
    image.dump("output.ppm");

    return 0;
}

void create_scene(HittableList& scene) {
    auto ground_material = std::make_shared<Lambertian>(vec3(0.5, 0.5, 0.5));
    scene.add_hittable<Sphere>(vec3(0, -1000, 0), 1000, ground_material);

    for (int32_t a = -11; a < 11; a++) {
        for (int32_t b = -11; b < 11; b++) {
            auto choose_mat = random_double();
            vec3 center(a + 0.9 * random_double(), 0.2, b + 0.9 * random_double());

            if (glm::length(center - vec3(4, 0.2, 0)) > 0.9) {
                std::shared_ptr<IMaterial> sphere_material;

                if (choose_mat < 0.8) {
                    // diffuse
                    auto albedo = vec3_random() * vec3_random();
                    sphere_material = std::make_shared<Lambertian>(albedo);
                    scene.add_hittable<Sphere>(center, 0.2, sphere_material);
                } else if (choose_mat < 0.95) {
                    // metal
                    auto albedo = vec3_random(0.5, 1);
                    auto fuzz = random_double(0, 0.5);
                    sphere_material = std::make_shared<Metal>(albedo, fuzz);
                    scene.add_hittable<Sphere>(center, 0.2, sphere_material);
                } else {
                    // glass
                    sphere_material = std::make_shared<Dielectric>(1.5);
                    scene.add_hittable<Sphere>(center, 0.2, sphere_material);
                }
            }
        }
    }

    auto material1 = std::make_shared<Dielectric>(1.5);
    scene.add_hittable<Sphere>(vec3(0, 1, 0), 1.0, material1);

    auto material2 = std::make_shared<Lambertian>(vec3(0.4, 0.2, 0.1));
    scene.add_hittable<Sphere>(vec3(-4, 1, 0), 1.0, material2);

    auto material3 = std::make_shared<Metal>(vec3(0.7, 0.6, 0.5), 0.0);
    scene.add_hittable<Sphere>(vec3(4, 1, 0), 1.0, material3);
}
