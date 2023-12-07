#include "material.h"

#include "hittable.h"
#include "rand.h"

//
// Lambertian
//

Lambertian::Lambertian(vec3 albedo) : m_albedo(albedo) {}

std::optional<MaterialHit> Lambertian::scatter([[maybe_unused]] const Ray& ray, const HitRecord& record) const {
    auto scatter_direction = record.normal + vec3_random_unit();
    if (vec3_near_zero(scatter_direction))
        scatter_direction = record.normal;

    return MaterialHit{
        .scatter = Ray(record.point, scatter_direction),
        .attenuation = m_albedo,
    };
}

//
// Metal
//

Metal::Metal(vec3 albedo, double fuzz) : m_albedo(albedo), m_fuzz(std::min(fuzz, 1.0)) {}

std::optional<MaterialHit> Metal::scatter(const Ray& ray, const HitRecord& record) const {
    const auto reflected = vec3_reflect(glm::normalize(ray.direction()), record.normal);

    const auto reflected_ray = Ray(record.point, reflected + m_fuzz * vec3_random_unit());
    auto material_hit = MaterialHit{
        .scatter = reflected_ray,
        .attenuation = m_albedo,
    };

    return glm::dot(reflected_ray.direction(), record.normal) > 0.0 ? material_hit : std::optional<MaterialHit>{};
}
