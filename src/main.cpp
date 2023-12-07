#include "camera.h"

#include "hittable.h"
#include "material.h"

constexpr uint32_t IMAGE_WIDTH = 1920;
constexpr uint32_t IMAGE_HEIGHT = static_cast<uint32_t>(IMAGE_WIDTH / (16.0f / 9.0f));

int main() {
    const Camera camera({
        .width = IMAGE_WIDTH,
        .height = IMAGE_HEIGHT,
        .focal_length = 1.0,
        .vertical_fov = glm::radians(90.0f),
        .center = glm::vec3(0.0f),
        .samples_per_pixel = 40,
        .max_depth = 50,
    });

    auto material_ground = std::make_shared<Lambertian>(vec3(0.8, 0.8, 0.0));
    auto material_center = std::make_shared<Lambertian>(vec3(0.7, 0.3, 0.3));
    auto material_left = std::make_shared<Metal>(vec3(0.8, 0.8, 0.8), 0.3);
    auto material_right = std::make_shared<Metal>(vec3(0.8, 0.6, 0.2), 1.0);

    HittableList scene;

    scene.add_hittable<Sphere>(vec3(0.0, -100.5, -1.0), 100.0, material_ground);
    scene.add_hittable<Sphere>(vec3(0.0, 0.0, -1.0), 0.5, material_center);
    scene.add_hittable<Sphere>(vec3(-1.0, 0.0, -1.0), 0.5, material_left);
    scene.add_hittable<Sphere>(vec3(1.0, 0.0, -1.0), 0.5, material_right);

    camera.render(scene);

    return 0;
}