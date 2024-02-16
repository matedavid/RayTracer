#include "hittable_list.h"

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

AABB HittableList::bounding_box() const {
    return m_bounding_box;
}
