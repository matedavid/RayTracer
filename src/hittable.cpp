#include "hittable.h"

#include "ray.h"
#include "interval.h"

// HitRecord
void HitRecord::set_front_face(const Ray& ray, const vec3& outward_normal) {
    front_face = glm::dot(ray.direction(), outward_normal) < 0.0;
    normal = front_face ? outward_normal : -outward_normal;
}

//
// Sphere
//
Sphere::Sphere(vec3 position, double radius, std::shared_ptr<IMaterial> material)
      : m_position(position), m_radius(radius), m_material(std::move(material)) {}

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

    HitRecord record{};
    record.ts = root;
    record.point = ray.at(record.ts);
    record.material = m_material;

    const auto outward_normal = (record.point - m_position) / m_radius;
    record.set_front_face(ray, outward_normal);

    return record;
}

//
// HittableList
//

std::optional<HitRecord> HittableList::hits(const Ray& ray, const interval& ray_t) const {
    std::optional<HitRecord> record;
    auto closest_max_t = ray_t.max;

    for (const auto& object : m_objects) {
        const auto r = object->hits(ray, interval(ray_t.min, closest_max_t));
        if (r.has_value()) {
            record = r;
            closest_max_t = record->ts;
        }
    }

    return record;
}