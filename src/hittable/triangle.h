#pragma once

#include "hittable/hittable.h"

class Triangle : public IHittable {
  public:
    struct Vertex {
        vec3 pos{};
        vec2 uv{};
        vec3 normal{};
    };

    Triangle(Vertex a, Vertex b, Vertex c, std::shared_ptr<IMaterial> material);
    ~Triangle() override = default;

    [[nodiscard]] std::optional<HitRecord> hits(const Ray& ray, const interval& ray_t) const override;
    [[nodiscard]] AABB bounding_box() const override;

  private:
    Vertex m_a, m_b, m_c;
    std::shared_ptr<IMaterial> m_material;
    AABB m_bounding_box{};
};
