#include "material.h"

#include "rand.h"
#include "texture.h"
#include "onb.h"
#include "hittable/hittable.h"

//
// Lambertian
//

Lambertian::Lambertian(vec3 albedo) : m_albedo(albedo) {}

Lambertian::Lambertian(std::shared_ptr<Texture> texture) : m_texture(std::move(texture)) {}

std::optional<MaterialHit> Lambertian::scatter([[maybe_unused]] const Ray& ray, const HitRecord& record) const {
    ONB uvw{};
    uvw.build_from_w(record.normal);

    const auto scatter_direction = uvw.local(random_cosine_direction());

    return MaterialHit{
        .scatter = Ray(record.point, scatter_direction),
        .attenuation = m_texture != nullptr ? m_texture->sample(record.uv.x, record.uv.y) : m_albedo,
        .pdf = glm::dot(record.normal, scatter_direction) / glm::pi<double>(),
    };
}

double Lambertian::scattering_prob([[maybe_unused]] const Ray& incoming,
                                  const HitRecord& record,
                                  const Ray& outgoing) const {
    const auto cosine = glm::dot(record.normal, outgoing.direction());
    return cosine < 0.0 ? 0.0 : cosine / glm::pi<double>();
}

//
// Metal
//

Metal::Metal(vec3 albedo, double fuzz) : m_albedo(albedo), m_fuzz(std::min(fuzz, 1.0)) {}

std::optional<MaterialHit> Metal::scatter(const Ray& ray, const HitRecord& record) const {
    const auto reflected = glm::reflect(glm::normalize(ray.direction()), record.normal);

    const auto reflected_ray = Ray(record.point, reflected + m_fuzz * vec3_random_unit());
    const auto material_hit = MaterialHit{
        .scatter = reflected_ray,
        .attenuation = m_albedo,
        .pdf = 1.0,
    };

    return glm::dot(reflected_ray.direction(), record.normal) > 0.0 ? material_hit : std::optional<MaterialHit>{};
}

double Metal::scattering_prob([[maybe_unused]] const Ray& incoming, const HitRecord& record, const Ray& outgoing) const {
    // Just make sure the outgoing ray is in the same hemisphere as the normal
    return glm::dot(record.normal, outgoing.direction()) < 0.0 ? 0.0 : 1.0;
}

//
// Dielectric
//

Dielectric::Dielectric(double refraction_index) : m_refraction_index(refraction_index) {}

std::optional<MaterialHit> Dielectric::scatter(const Ray& ray, const HitRecord& record) const {
    const double refraction_ratio = record.front_face ? 1.0 / m_refraction_index : m_refraction_index;

    // const auto refracted = vec3_refract(glm::normalize(ray.direction()), record.normal, refraction_ratio);

    const auto direction_normalized = glm::normalize(ray.direction());

    const double cos_theta = fmin(glm::dot(-direction_normalized, record.normal), 1.0);
    const double sin_theta = glm::sqrt(1.0 - cos_theta * cos_theta);

    const bool cannot_refract = refraction_ratio * sin_theta > 1.0;

    vec3 scatter_direction;
    if (cannot_refract || reflectance(cos_theta, refraction_ratio) > random_double())
        scatter_direction = glm::reflect(direction_normalized, record.normal);
    else
        scatter_direction = glm::refract(direction_normalized, record.normal, refraction_ratio);

    return MaterialHit{
        .scatter = Ray(record.point, scatter_direction),
        .attenuation = vec3(1.0),
        .pdf = 1.0,
    };
}

double Dielectric::scattering_prob([[maybe_unused]] const Ray& incoming,
                                  [[maybe_unused]] const HitRecord& record,
                                  [[maybe_unused]] const Ray& outgoing) const {
    return 1.0;
}

double Dielectric::reflectance(double cosine, double ref_idx) {
    // Use Schlick's approximation for reflectance.
    auto r0 = (1 - ref_idx) / (1 + ref_idx);
    r0 = r0 * r0;
    return r0 + (1 - r0) * glm::pow((1 - cosine), 5);
}

//
// DiffuseEmissive
//

DiffuseEmissive::DiffuseEmissive(vec3 emission_color, double intensity) : m_color(emission_color * intensity) {}

std::optional<MaterialHit> DiffuseEmissive::scatter([[maybe_unused]] const Ray& ray,
                                                    [[maybe_unused]] const HitRecord& record) const {
    return {};
}

std::optional<vec3> DiffuseEmissive::emitted([[maybe_unused]] double u, [[maybe_unused]] double v) const {
    return m_color;
}

double DiffuseEmissive::scattering_prob([[maybe_unused]] const Ray& incoming,
                                       [[maybe_unused]] const HitRecord& record,
                                       [[maybe_unused]] const Ray& outgoing) const {
    return 0.0;
}