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
    explicit Lambertian(vec3 albedo);
    ~Lambertian() override = default;

    [[nodiscard]] std::optional<MaterialHit> scatter(const Ray& ray, const HitRecord& record) const override;

  private:
    vec3 m_albedo;
};

class Metal : public IMaterial {
  public:
    explicit Metal(vec3 albedo, double fuzz);
    ~Metal() override = default;

    [[nodiscard]] std::optional<MaterialHit> scatter(const Ray& ray, const HitRecord& record) const override;

  private:
    vec3 m_albedo;
    double m_fuzz;
};

class Dielectric : public IMaterial {
  public:
    explicit Dielectric(double refraction_index);
    ~Dielectric() override = default;

    [[nodiscard]] std::optional<MaterialHit> scatter(const Ray& ray, const HitRecord& record) const override;

  private:
    double m_refraction_index;

    [[nodiscard]] static double reflectance(double cosine, double ref_idx);
};