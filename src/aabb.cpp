#include "aabb.h"

#include "ray.h"

AABB::AABB(interval x, interval y, interval z) : m_x(x), m_y(y), m_z(z) {}

AABB::AABB(vec3 a, vec3 b) {
    m_x = interval(fmin(a.x, b.x), fmax(a.x, b.x));
    m_y = interval(fmin(a.y, b.y), fmax(a.y, b.y));
    m_z = interval(fmin(a.z, b.z), fmax(a.z, b.z));
}

AABB::AABB(const AABB& a, const AABB& b) {
    m_x = interval(a.m_x, b.m_x);
    m_y = interval(a.m_y, b.m_y);
    m_z = interval(a.m_z, b.m_z);
}

const interval& AABB::axis(uint32_t n) const {
    if (n == 1)
        return m_y;
    else if (n == 2)
        return m_z;
    return m_x;
}

bool AABB::hit(const Ray& ray, const interval& ray_t) const {
    auto ray_t_min = ray_t.min;
    auto ray_t_max = ray_t.max;

    for (uint32_t a = 0; a < 3; ++a) {
        const auto inv_direction = 1.0 / ray.direction()[static_cast<int32_t>(a)];
        const auto origin = ray.origin()[static_cast<int32_t>(a)];

        auto t0 = (axis(a).min - origin) * inv_direction;
        auto t1 = (axis(a).max - origin) * inv_direction;

        if (inv_direction < 0)
            std::swap(t0, t1);

        if (t0 > ray_t.min)
            ray_t_min = t0;
        if (t1 < ray_t.max)
            ray_t_max = t1;

        if (ray_t_max <= ray_t_min)
            return false;
    }

    return true;
}
