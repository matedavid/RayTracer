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
    const auto reflected = glm::reflect(glm::normalize(ray.direction()), record.normal);

    const auto reflected_ray = Ray(record.point, reflected + m_fuzz * vec3_random_unit());
    auto material_hit = MaterialHit{
        .scatter = reflected_ray,
        .attenuation = m_albedo,
    };

    return glm::dot(reflected_ray.direction(), record.normal) > 0.0 ? material_hit : std::optional<MaterialHit>{};
}

Dielectric::Dielectric(double refraction_index) : m_refraction_index(refraction_index) {}

std::optional<MaterialHit> Dielectric::scatter(const Ray& ray, const HitRecord& record) const {
    double refraction_ratio = record.front_face ? (1.0 / m_refraction_index) : m_refraction_index;

    // const auto refracted = vec3_refract(glm::normalize(ray.direction()), record.normal, refraction_ratio);

    const auto direction_normalized = glm::normalize(ray.direction());

    double cos_theta = fmin(glm::dot(-direction_normalized, record.normal), 1.0);
    double sin_theta = glm::sqrt(1.0 - cos_theta * cos_theta);

    bool cannot_refract = refraction_ratio * sin_theta > 1.0;

    vec3 scatter_direction;
    if (cannot_refract || reflectance(cos_theta, refraction_ratio) > random_double())
        scatter_direction = glm::reflect(direction_normalized, record.normal);
    else
        scatter_direction = glm::refract(direction_normalized, record.normal, refraction_ratio);

    return MaterialHit{
        .scatter = Ray(record.point, scatter_direction),
        .attenuation = vec3(1.0),
    };
}

double Dielectric::reflectance(double cosine, double ref_idx) {
    // Use Schlick's approximation for reflectance.
    auto r0 = (1 - ref_idx) / (1 + ref_idx);
    r0 = r0 * r0;
    return r0 + (1 - r0) * glm::pow((1 - cosine), 5);
}
