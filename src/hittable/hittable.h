#pragma once

#include <optional>
#include <memory>

#include "vec.h"
#include "ray.h"
#include "aabb.h"

// Forward declarations
class IMaterial;
class interval;

struct HitRecord {
    vec3 point;
    vec3 normal;
    vec2 uv;
    double ts;
    bool front_face;
    std::shared_ptr<IMaterial> material;

    // outward_normal assumed to be normalized
    void set_front_face(const Ray& ray, const vec3& outward_normal) {
        front_face = glm::dot(ray.direction(), outward_normal) < 0.0;
        normal = front_face ? outward_normal : -outward_normal;
    }
};

class IHittable {
  public:
    virtual ~IHittable() = default;

    [[nodiscard]] virtual std::optional<HitRecord> hits(const Ray& ray, const interval& ray_t) const = 0;
    [[nodiscard]] virtual AABB bounding_box() const = 0;
};
