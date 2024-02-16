#pragma once

#include <vector>

#include "hittable/hittable.h"

// Forward declarations
class HittableList;

class BVHNode : public IHittable {
  public:
    explicit BVHNode(const HittableList& list);
    BVHNode(const std::vector<std::shared_ptr<IHittable>>& objects, std::size_t start, std::size_t end);

    [[nodiscard]] std::optional<HitRecord> hits(const Ray& ray, const interval& ray_t) const override;
    [[nodiscard]] AABB bounding_box() const override;

  private:
    std::shared_ptr<IHittable> m_left;
    std::shared_ptr<IHittable> m_right;

    AABB m_bounding_box;

    [[nodiscard]] static bool box_compare(const std::shared_ptr<IHittable>& a,
                                          const std::shared_ptr<IHittable>& b,
                                          uint32_t axis);
    [[nodiscard]] static bool box_x_compare(const std::shared_ptr<IHittable>& a, const std::shared_ptr<IHittable>& b);
    [[nodiscard]] static bool box_y_compare(const std::shared_ptr<IHittable>& a, const std::shared_ptr<IHittable>& b);
    [[nodiscard]] static bool box_z_compare(const std::shared_ptr<IHittable>& a, const std::shared_ptr<IHittable>& b);
};
