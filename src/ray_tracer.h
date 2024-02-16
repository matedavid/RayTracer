#pragma once

#include <cstdint>

#include "vec.h"

// Forward declarations
class Camera;
class Ray;
class IHittable;
class IImageDumper;

class RayTracer {
  public:
    struct Description {
        // Rendering params
        uint32_t samples_per_pixel = 10;
        uint32_t max_depth = 10;
        uint32_t num_threads = 1;

        // Log params
        double percentage_update_progress = 0.2; // Displays progress every time it reaches the specified percentage
    };


    explicit RayTracer(Description description);

    [[nodiscard]] static uint32_t max_num_threads();

    void render(const Camera& camera, const IHittable& scene, IImageDumper& image) const;


  private:
    Description m_desc;

    struct RenderingInfo {
        vec3 camera_center;
        vec3 pixel00_loc;
        vec3 delta_u, delta_v;
        double scale;
        IImageDumper& image;
    };

    using Position = std::pair<std::size_t, std::size_t>;
    void render_pixel(Position pixel, const IHittable& scene, const RenderingInfo& info) const;

    [[nodiscard]] static vec3 ray_color_r(const Ray& ray, const IHittable& scene, uint32_t depth);
    [[nodiscard]] static vec3 pixel_sample_square(const vec3& delta_u, const vec3& delta_v);
    [[nodiscard]] static double linear_to_gamma(double val);
};
