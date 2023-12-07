#include "camera.h"

#include <fstream>
#include <iostream>

#include "ray.h"
#include "rand.h"
#include "interval.h"
#include "hittable.h"
#include "material.h"

Camera::Camera(Description description) : m_desc(description) {
    const auto aspect_ratio = static_cast<double>(m_desc.width) / static_cast<double>(m_desc.height);

    const auto viewport_height = 2.0 * description.focal_length * glm::tan(m_desc.vertical_fov / 2.0);
    const auto viewport_width = viewport_height * aspect_ratio;

    const auto viewport_u = vec3(viewport_width, 0, 0);
    const auto viewport_v = vec3(0, -viewport_height, 0);

    m_delta_u = viewport_u / static_cast<double>(m_desc.width);
    m_delta_v = viewport_v / static_cast<double>(m_desc.height);

    const auto viewport_upper_left =
        m_desc.center - vec3(0, 0, description.focal_length) - viewport_u / 2.0 - viewport_v / 2.0;
    m_pixel00_loc = viewport_upper_left + 0.5 * (m_delta_u + m_delta_v);
}

void Camera::render(const IHittable& scene) const {
    std::ofstream file("output.ppm");

    file << "P3\n" << m_desc.width << " " << m_desc.height << "\n255\n";

    const auto scale = 1.0 / m_desc.samples_per_pixel;

    for (std::size_t col = 0; col < m_desc.height; ++col) {
        if (col % 100 == 0)
            std::cout << "Progress: " << uint32_t((float(col + 1) / float(m_desc.height)) * 100.0f) << "%\n";

        for (std::size_t row = 0; row < m_desc.width; ++row) {
            const auto drow = static_cast<double>(row);
            const auto dcol = static_cast<double>(col);

            const auto pixel_center = m_pixel00_loc + m_delta_u * drow + m_delta_v * dcol;

            vec3 color{0.0};
            for (std::size_t s = 0; s < m_desc.samples_per_pixel; ++s) {
                const auto pixel_sample = pixel_center + pixel_sample_square();
                const auto direction = pixel_sample - m_desc.center;

                const auto ray = Ray(m_desc.center, direction);
                color += ray_color_r(ray, scene, m_desc.max_depth);
            }

            color *= scale;

            auto r = linear_to_gamma(color.r);
            auto g = linear_to_gamma(color.g);
            auto b = linear_to_gamma(color.b);

            constexpr interval intensity(0.0, 0.999);
            file << static_cast<int>(intensity.clamp(r) * 256.0) << " " << static_cast<int>(intensity.clamp(g) * 256.0)
                 << " " << static_cast<int>(intensity.clamp(b) * 256.0) << "\n";
        }
    }
}

vec3 Camera::ray_color_r(const Ray& ray, const IHittable& scene, uint32_t depth) {
    if (depth == 0)
        return vec3{0.0};

    const auto record = scene.hits(ray, interval(0.001, interval::infinity));
    if (record) {
        const auto material_hit = record->material->scatter(ray, *record);
        if (material_hit) {
            return material_hit->attenuation * ray_color_r(material_hit->scatter, scene, depth - 1);
        }

        return vec3{0.0};
    }

    // Sky
    const vec3 unit_direction = glm::normalize(ray.direction());
    const auto a = 0.5 * (unit_direction.y + 1.0);
    return (1.0 - a) * vec3(1.0) + a * vec3(0.5f, 0.7f, 1.0f);
}

vec3 Camera::pixel_sample_square() const {
    const auto px = -0.5 + random_double();
    const auto py = -0.5 + random_double();
    return (px * m_delta_u) + (py * m_delta_v);
}

double Camera::linear_to_gamma(double val) {
    return glm::sqrt(val);
}
