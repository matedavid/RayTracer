#pragma once

#include "hittable/hittable.h"

class Sphere : public IHittable {
  public:
    Sphere(vec3 position, double radius, std::shared_ptr<IMaterial> material);
    ~Sphere() override = default;

    [[nodiscard]] std::optional<HitRecord> hits(const Ray& ray, const interval& ray_t) const override;
    [[nodiscard]] AABB bounding_box() const override;

  private:
    vec3 m_position;
    double m_radius;
    std::shared_ptr<IMaterial> m_material;
    AABB m_bounding_box;
};