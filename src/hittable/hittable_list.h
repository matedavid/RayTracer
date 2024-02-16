#pragma once

#include <vector>

#include "hittable/hittable.h"
#include "aabb.h"

class HittableList : public IHittable {
  public:
    HittableList() = default;
    ~HittableList() override = default;

    template <typename T, typename... Args>
    void add_hittable(Args&&... args) {
        static_assert(std::is_base_of<IHittable, T>(), "Type must be of type IHittable");
        m_objects.push_back(std::make_shared<T>(args...));

        m_bounding_box = AABB(m_bounding_box, m_objects.back()->bounding_box());
    }

    [[nodiscard]] std::optional<HitRecord> hits(const Ray& ray, const interval& ray_t) const override;
    [[nodiscard]] AABB bounding_box() const override;

    [[nodiscard]] const std::vector<std::shared_ptr<IHittable>>& objects() const { return m_objects; }

  private:
    std::vector<std::shared_ptr<IHittable>> m_objects;
    AABB m_bounding_box;
};
