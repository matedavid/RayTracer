#pragma once

#include <optional>
#include <memory>
#include <vector>

#include "vec3.h"

// Forward declarations
class Ray;
class interval;
class IMaterial;

struct HitRecord {
    vec3 point;
    vec3 normal;
    double ts;
    bool front_face;
    std::shared_ptr<IMaterial> material;

    // outward_normal assumed to be normalized
    void set_front_face(const Ray& ray, const vec3& outward_normal);
};

class IHittable {
  public:
    virtual ~IHittable() = default;

    [[nodiscard]] virtual std::optional<HitRecord> hits(const Ray& ray, const interval& ray_t) const = 0;
};

class Sphere : public IHittable {
  public:
    Sphere(vec3 position, double radius, std::shared_ptr<IMaterial> material);
    ~Sphere() override = default;

    [[nodiscard]] std::optional<HitRecord> hits(const Ray& ray, const interval& ray_t) const override;

  private:
    vec3 m_position;
    double m_radius;
    std::shared_ptr<IMaterial> m_material;
};

class HittableList : public IHittable {
  public:
    HittableList() = default;
    ~HittableList() override = default;

    template <typename T, typename... Args>
    void add_hittable(Args&&... args) {
        static_assert(std::is_base_of<IHittable, T>(), "Type must be of type IHittable");
        m_objects.push_back(std::make_unique<T>(args...));
    }

    [[nodiscard]] std::optional<HitRecord> hits(const Ray& ray, const interval& ray_t) const override;

  private:
    std::vector<std::unique_ptr<IHittable>> m_objects;
};
