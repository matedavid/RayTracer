#include "camera.h"

#include "hittable.h"

constexpr uint32_t IMAGE_WIDTH = 400;
constexpr uint32_t IMAGE_HEIGHT = static_cast<uint32_t>(IMAGE_WIDTH / (16.0f / 9.0f));

int main() {
    const Camera camera({
        .width = IMAGE_WIDTH,
        .height = IMAGE_HEIGHT,
        .focal_length = 1.f,
        .vertical_fov = glm::radians(90.0f),
        .center = glm::vec3(0.0f),
        .samples_per_pixel = 10,
        .max_depth = 50,
    });

    HittableList scene;

    scene.add_hittable<Sphere>(vec3(0.0, -100.5, -1.0), 100.0);
    scene.add_hittable<Sphere>(vec3(0.0, 0.0, -1.0), 0.5);

    camera.render(scene);

    return 0;
}