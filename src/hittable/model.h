#pragma once

#include <vector>
#include <filesystem>

#include "hittable/hittable.h"
#include "hittable/triangle.h"

// Forward declarations
struct aiMesh;
struct aiScene;

class Mesh : public IHittable {
  public:
    Mesh(const std::vector<Triangle::Vertex>& vertices,
         const std::vector<uvec3>& faces,
         const std::shared_ptr<IMaterial>& material);
    ~Mesh() override = default;

    [[nodiscard]] std::optional<HitRecord> hits(const Ray& ray, const interval& ray_t) const override;
    [[nodiscard]] AABB bounding_box() const override;

  private:
    std::vector<Triangle> m_faces;
    AABB m_bounding_box;
};

class Model : public IHittable {
  public:
    explicit Model(const std::filesystem::path& path);
    Model(const std::filesystem::path& path, vec3 translation, vec3 scale, vec3 rotation);

    ~Model() override = default;

    [[nodiscard]] std::optional<HitRecord> hits(const Ray& ray, const interval& ray_t) const override;
    [[nodiscard]] AABB bounding_box() const override;

  private:
    std::vector<std::shared_ptr<IHittable>> m_meshes;
    std::unique_ptr<IHittable> m_root;

    void load_mesh(const aiMesh* mesh, const glm::dmat4& transform);
};