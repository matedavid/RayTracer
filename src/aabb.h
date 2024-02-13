#pragma once

#include "interval.h"
#include "vec.h"

// Forward declarations
class Ray;

class AABB {
  public:
    AABB() = default;
    AABB(interval x, interval y, interval z); // a and b are extrema of the bounding box
    AABB(vec3 a, vec3 b);
    AABB(const AABB& a, const AABB& b);

    [[nodiscard]] vec3 max() const { return vec3(m_x.max, m_y.max, m_z.max); }
    [[nodiscard]] vec3 min() const { return vec3(m_x.min, m_y.min, m_z.min); }

    [[nodiscard]] const interval& axis(uint32_t n) const;
    [[nodiscard]] bool hit(const Ray& ray, const interval& ray_t) const;

  private:
    interval m_x, m_y, m_z;
};
