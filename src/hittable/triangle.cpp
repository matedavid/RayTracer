#include "triangle.h"

Triangle::Triangle(Vertex a, Vertex b, Vertex c, std::shared_ptr<IMaterial> material)
      : m_a(a), m_b(b), m_c(c), m_material(std::move(material)) {
    const auto min = glm::min(glm::min(m_a.pos, m_b.pos), m_c.pos);
    const auto max = glm::max(glm::max(m_a.pos, m_b.pos), m_c.pos);

    m_bounding_box = AABB(min, max);
    // m_normal = glm::normalize(glm::cross(m_b.pos - m_a.pos, m_c.pos - m_a.pos));
}

std::optional<HitRecord> Triangle::hits(const Ray& ray, const interval& ray_t) const {
    // Möller–Trumbore intersection algorithm:
    // https://en.wikipedia.org/wiki/M%C3%B6ller%E2%80%93Trumbore_intersection_algorithm

    constexpr auto epsilon = std::numeric_limits<double>::epsilon();

    const auto edge_1 = m_b.pos - m_a.pos;
    const auto edge_2 = m_c.pos - m_a.pos;

    const auto ray_cross_e2 = glm::cross(ray.direction(), edge_2);
    const auto det = glm::dot(edge_1, ray_cross_e2);

    if (det > -epsilon && det < epsilon) {
        // Ray is parallel to triangle
        return {};
    }

    const auto inv_det = 1.0 / det;
    const auto s = ray.origin() - m_a.pos;
    const auto u = inv_det * glm::dot(s, ray_cross_e2);

    if (u < 0 || u > 1)
        return {};

    const auto s_cross_e1 = glm::cross(s, edge_1);
    const auto v = inv_det * glm::dot(ray.direction(), s_cross_e1);

    if (v < 0 || u + v > 1)
        return {};

    // Find intersection point in triangle
    const auto t = inv_det * glm::dot(edge_2, s_cross_e1);

    if (t <= epsilon || !ray_t.surrounds(t)) {
        // Line intersection but not ray intersection, or t is not inside ray_t limits
        return {};
    }

    // Compute texture uv
    const auto w = 1.0 - u - v;
    const auto texture_uv = w * m_a.uv + u * m_b.uv + v * m_c.uv;

    // Compute normal
    const auto outward_normal = w * m_a.normal + u * m_b.normal + v * m_c.normal;

    HitRecord record{};
    record.ts = t;
    record.point = ray.at(record.ts);
    record.uv = texture_uv;
    record.material = m_material;

    record.set_front_face(ray, outward_normal);

    return record;
}

AABB Triangle::bounding_box() const {
    return m_bounding_box;
}
