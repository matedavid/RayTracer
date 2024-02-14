#include "ray_tracer.h"

#include <cassert>
#include <iostream>
#include <chrono>
#include <omp.h>

#include "camera.h"
#include "hittable.h"
#include "image_dumper.h"
#include "ray.h"
#include "rand.h"
#include "interval.h"
#include "material.h"

RayTracer::RayTracer(Description description) : m_desc(description) {
    if (m_desc.num_threads == 0)
        m_desc.num_threads = 1;

    m_desc.num_threads = std::min(m_desc.num_threads, static_cast<uint32_t>(omp_get_max_threads()));
    m_desc.percentage_update_progress = interval(0.01, 1.0).clamp(m_desc.percentage_update_progress);
}

void RayTracer::render(const Camera& camera, const IHittable& scene, IImageDumper& image) const {
    assert(camera.width() == image.width() && camera.height() == image.height());

    const auto scale = 1.0 / m_desc.samples_per_pixel;
    const auto& [delta_u, delta_v] = camera.deltas();
    const auto& pixel00_loc = camera.pixel00_location();

    // Log information
    std::cout << "RayTracer information:\n";
    std::cout << "    Width: " << camera.width() << "\n";
    std::cout << "    Height: " << camera.height() << "\n";
    std::cout << "    Samples per pixel: " << m_desc.samples_per_pixel << "\n";
    std::cout << "    Max Depth: " << m_desc.max_depth << "\n";
    std::cout << "    Num Threads: " << m_desc.num_threads << "\n";
    std::cout << "\n";

    omp_set_num_threads(static_cast<int>(m_desc.num_threads));

    const auto start = std::chrono::high_resolution_clock::now();
    std::size_t progress = 0;

    const auto update_progress_every = static_cast<std::size_t>(camera.height() * m_desc.percentage_update_progress);

#pragma omp parallel for
    for (std::size_t row = 0; row < camera.height(); ++row) {
#pragma omp atomic
        progress++;

        if (progress % update_progress_every == 0)
            std::cout << "[Progress]: " << int(progress / float(camera.height()) * 100.0f) << "%\n";

        for (std::size_t col = 0; col < camera.width(); ++col) {
            const auto drow = static_cast<double>(row);
            const auto dcol = static_cast<double>(col);

            const auto pixel_center = pixel00_loc + delta_u * dcol + delta_v * drow;

            vec3 color{0.0};
            for (std::size_t s = 0; s < m_desc.samples_per_pixel; ++s) {
                const auto pixel_sample = pixel_center + pixel_sample_square(delta_u, delta_v);
                const auto direction = pixel_sample - camera.center();

                const auto ray = Ray(camera.center(), direction);
                color += ray_color_r(ray, scene, m_desc.max_depth);
            }

            color *= scale;

            auto r = linear_to_gamma(color.r);
            auto g = linear_to_gamma(color.g);
            auto b = linear_to_gamma(color.b);

            assert(!std::isnan(r) && !std::isnan(g) && !std::isnan(b));

            image[row][col] = vec3(r, g, b);
        }
    }

    const auto end = std::chrono::high_resolution_clock::now();
    const auto duration = std::chrono::duration_cast<std::chrono::seconds>(end - start);

    std::cout << "\n";
    std::cout << "Execution time: " << duration.count() << "s\n";
}

vec3 RayTracer::ray_color_r(const Ray& ray, const IHittable& scene, uint32_t depth) {
    if (depth == 0)
        return vec3{0.0};

    const auto record = scene.hits(ray, interval(0.001, interval::infinity));
    if (record) {
        auto color = vec3{0.0};

        const auto material_hit = record->material->scatter(ray, *record);
        if (material_hit) {
            // const auto scattering_pdf = record->material->scattering_pdf(ray, *record, material_hit->scatter);
            // const auto c =
            //     material_hit->attenuation * scattering_pdf * ray_color_r(material_hit->scatter, scene, depth - 1);
            // color += c / material_hit->pdf;

            const auto c = material_hit->attenuation * ray_color_r(material_hit->scatter, scene, depth - 1);
            color += c;
        }

        const auto emission_color = record->material->emitted(record->uv.x, record->uv.y);
        if (emission_color) {
            color += *emission_color;
        }

        return color;
    }

    // Sky
    // const vec3 unit_direction = glm::normalize(ray.direction());
    // const auto a = 0.5 * (unit_direction.y + 1.0);
    // return (1.0 - a) * vec3(1.0) + a * vec3(0.5f, 0.7f, 1.0f);
    return vec3{0.0};
}

vec3 RayTracer::pixel_sample_square(const vec3& delta_u, const vec3& delta_v) {
    const auto px = -0.5 + random_double();
    const auto py = -0.5 + random_double();
    return (px * delta_u) + (py * delta_v);
}

double RayTracer::linear_to_gamma(double val) {
    return glm::sqrt(val);
}
