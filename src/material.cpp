#include "material.h"

#include "hittable.h"
#include "rand.h"

//
// Lambertian
//

Lambertian::Lambertian(vec3 color) : m_color(color) {}

std::optional<MaterialHit> Lambertian::scatter([[maybe_unused]] const Ray& ray, const HitRecord& record) const {
    auto scatter_direction = record.normal + vec3_random_unit();
    if (vec3_near_zero(scatter_direction))
        scatter_direction = record.normal;

    const auto scatter = Ray(record.point, scatter_direction);
    return MaterialHit{
        .scatter = scatter,
        .attenuation = m_color,
    };
}
