#include "camera.h"

#include "hittable.h"
#include "material.h"

constexpr uint32_t IMAGE_WIDTH = 400;
constexpr uint32_t IMAGE_HEIGHT = static_cast<uint32_t>(IMAGE_WIDTH / (16.0f / 9.0f));

int main() {
    const Camera camera({
        .width = IMAGE_WIDTH,
        .height = IMAGE_HEIGHT,
        .samples_per_pixel = 10,
        .max_depth = 50,

        .vertical_fov = glm::radians(90.0f),
        .look_from = vec3(-2.0, 2.0, 1.0),
        .look_at = vec3(0.0, 0.0, -1.0),
        .up = vec3(0.0, 1.0, 0.0),
    });

    auto material_ground = std::make_shared<Lambertian>(vec3(0.8, 0.8, 0.0));
    auto material_center = std::make_shared<Lambertian>(vec3(0.1, 0.2, 0.5));
    auto material_left = std::make_shared<Dielectric>(1.5);
    auto material_right = std::make_shared<Metal>(vec3(0.8, 0.6, 0.2), 0.0);

    HittableList scene;

    scene.add_hittable<Sphere>(vec3(0.0, -100.5, -1.0), 100.0, material_ground);
    scene.add_hittable<Sphere>(vec3(0.0, 0.0, -1.0), 0.5, material_center);
    scene.add_hittable<Sphere>(vec3(-1.0, 0.0, -1.0), 0.5, material_left);
    scene.add_hittable<Sphere>(vec3(1.0, 0.0, -1.0), 0.5, material_right);

    camera.render(scene);

    return 0;
}