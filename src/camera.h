#pragma once

#include <cstdint>
#include "vec3.h"

// Forward declarations
class Ray;
class IHittable;

class Camera {
  public:
    struct Description {
        uint32_t width = 1920;
        uint32_t height = 1080;
        uint32_t samples_per_pixel = 10;
        uint32_t max_depth = 10;

        // Camera positioning
        double vertical_fov = glm::radians(90.0f); // in radians
        vec3 look_from = vec3(0.0, 0.0, -1.0);
        vec3 look_at =  vec3(0.0);
        vec3 up = vec3(0.0, 1.0, 0.0);
    };

    explicit Camera(Description description);

    void render(const IHittable& scene) const;

  private:
    Description m_desc;

    vec3 m_delta_u{}, m_delta_v{};
    vec3 m_pixel00_loc{};

    [[nodiscard]] static vec3 ray_color_r(const Ray& ray, const IHittable& scene, uint32_t depth);
    [[nodiscard]] vec3 pixel_sample_square() const;
    [[nodiscard]] static double linear_to_gamma(double val);
};
