#pragma once

#include <optional>

#include "vec3.h"
#include "ray.h"

// Forward declarations
struct HitRecord;

struct MaterialHit {
    Ray scatter;
    vec3 attenuation;
};

class IMaterial {
  public:
    virtual ~IMaterial() = default;

    [[nodiscard]] virtual std::optional<MaterialHit> scatter(const Ray& ray, const HitRecord& record) const = 0;
};

class Lambertian : public IMaterial {
  public:
    Lambertian(vec3 color);
    ~Lambertian() override = default;

    [[nodiscard]] std::optional<MaterialHit> scatter(const Ray& ray, const HitRecord& record) const override;

  private:
    vec3 m_color;
};