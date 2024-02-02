#pragma once

#include <optional>

#include "vec.h"
#include "ray.h"
#include "texture.h"

// Forward declarations
struct HitRecord;

struct MaterialHit {
    Ray scatter;
    vec3 attenuation;
    double pdf;
};

class IMaterial {
  public:
    virtual ~IMaterial() = default;

    [[nodiscard]] virtual std::optional<MaterialHit> scatter(const Ray& ray, const HitRecord& record) const = 0;
    [[nodiscard]] virtual std::optional<vec3> emitted(double u, double v) const { return {}; }
    [[nodiscard]] virtual double scattering_pdf([[maybe_unused]] const Ray& ray,
                                                [[maybe_unused]] const HitRecord& record,
                                                [[maybe_unused]] const Ray& scattered) const {
        return 0.0;
    }
};

class Lambertian : public IMaterial {
  public:
    explicit Lambertian(vec3 albedo);
    explicit Lambertian(std::shared_ptr<Texture> texture);
    ~Lambertian() override = default;

    [[nodiscard]] std::optional<MaterialHit> scatter(const Ray& ray, const HitRecord& record) const override;
    [[nodiscard]] double scattering_pdf(const Ray& ray, const HitRecord& record, const Ray& scattered) const override;

  private:
    vec3 m_albedo{};
    std::shared_ptr<Texture> m_texture = nullptr;
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

class DiffuseEmissive : public IMaterial {
  public:
    explicit DiffuseEmissive(vec3 emission_color, double intensity);

    [[nodiscard]] std::optional<MaterialHit> scatter(const Ray& ray, const HitRecord& record) const override;
    [[nodiscard]] std::optional<vec3> emitted(double u, double v) const override;

  private:
    vec3 m_color;
};