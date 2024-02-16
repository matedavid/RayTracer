#include "sphere.h"

#include "material.h"
#include "interval.h"

Sphere::Sphere(vec3 position, double radius, std::shared_ptr<IMaterial> material)
      : m_position(position), m_radius(radius), m_material(std::move(material)) {
    const auto rvec3 = vec3(radius);
    m_bounding_box = AABB(m_position - rvec3, m_position + rvec3);
}

std::optional<HitRecord> Sphere::hits(const Ray& ray, const interval& ray_t) const {
    const auto oc = ray.origin() - m_position;

    const auto a = glm::dot(ray.direction(), ray.direction());
    const auto b = 2.0f * glm::dot(ray.direction(), oc);
    const auto c = glm::dot(oc, oc) - m_radius * m_radius;

    const auto discriminant = b * b - 4.0f * a * c;
    if (discriminant < 0.0f)
        return {};

    auto root = (-b - glm::sqrt(discriminant)) / (2.0f * a);
    if (!ray_t.surrounds(root)) {
        root = (-b + glm::sqrt(discriminant)) / (2.0f * a);

        if (!ray_t.surrounds(root)) {
            return {};
        }
    }

    // Compute texture uv
    const auto uv_point = ray.at(root);
    const auto uv_direction = glm::normalize(m_position - uv_point);

    const auto longitude = 0.5 + atan2(uv_direction.z, uv_direction.x) / (2.0 * M_PI);
    const auto latitude = 0.5 + asin(uv_direction.y) / M_PI;

    HitRecord record{};
    record.ts = root;
    record.point = ray.at(record.ts);
    record.uv = vec2(longitude, latitude);
    record.material = m_material;

    const auto outward_normal = (record.point - m_position) / m_radius;
    record.set_front_face(ray, outward_normal);

    return record;
}

AABB Sphere::bounding_box() const {
    return m_bounding_box;
}
