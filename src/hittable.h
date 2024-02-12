#pragma once

#include <optional>
#include <memory>
#include <vector>
#include <filesystem>

#include "vec.h"
#include "aabb.h"

// Forward declarations
class Ray;
class interval;
class IMaterial;
class aiScene;
class aiMesh;

struct HitRecord {
    vec3 point;
    vec3 normal;
    vec2 uv;
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
    [[nodiscard]] virtual AABB bounding_box() const = 0;
};

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
