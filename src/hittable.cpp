#include "hittable.h"

#include <algorithm>

#include <glm/gtc/matrix_transform.hpp>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include "ray.h"
#include "interval.h"
#include "rand.h"
#include "material.h"

// HitRecord
void HitRecord::set_front_face(const Ray& ray, const vec3& outward_normal) {
    front_face = glm::dot(ray.direction(), outward_normal) < 0.0;
    normal = front_face ? outward_normal : -outward_normal;
}

//
// Sphere
//

Sphere::Sphere(vec3 position, double radius, std::shared_ptr<IMaterial> material)
      : m_position(position), m_radius(radius), m_material(std::move(material)) {
    const auto rvec3 = vec3(radius);
    m_bounding_box = AABB(m_position - rvec3, m_position + rvec3);
}

std::optional<HitRecord> Sphere::hits(const Ray& ray, const interval& ray_t) const {
    const auto oc = ray.origin() - m_position;

    const auto a = glm::dot(ray.direction(), ray.direction());
    const auto b = 2.0f * glm::dot(ray.direction(), oc);
    const auto c = glm::dot(oc, oc) - m_radius * m_radius;

    const auto discriminant = b * b - 4.0f * a * c;
    if (discriminant < 0.0f)
        return {};

    auto root = (-b - glm::sqrt(discriminant)) / (2.0f * a);
    if (!ray_t.surrounds(root)) {
        root = (-b + glm::sqrt(discriminant)) / (2.0f * a);

        if (!ray_t.surrounds(root)) {
            return {};
        }
    }

    // Compute texture uv
    const auto uv_point = ray.at(root);
    const auto uv_direction = glm::normalize(m_position - uv_point);

    const auto longitude = 0.5 + atan2(uv_direction.z, uv_direction.x) / (2.0 * M_PI);
    const auto latitude = 0.5 + asin(uv_direction.y) / M_PI;

    HitRecord record{};
    record.ts = root;
    record.point = ray.at(record.ts);
    record.uv = vec2(longitude, latitude);
    record.material = m_material;

    const auto outward_normal = (record.point - m_position) / m_radius;
    record.set_front_face(ray, outward_normal);

    return record;
}

AABB Sphere::bounding_box() const {
    return m_bounding_box;
}

//
// Triangle
//

Triangle::Triangle(Vertex a, Vertex b, Vertex c, std::shared_ptr<IMaterial> material)
      : m_a(a), m_b(b), m_c(c), m_material(std::move(material)) {
    const auto min = glm::min(glm::min(m_a.pos, m_b.pos), m_c.pos);
    const auto max = glm::max(glm::max(m_a.pos, m_b.pos), m_c.pos);

    m_bounding_box = AABB(min, max);
    // m_normal = glm::normalize(glm::cross(m_b.pos - m_a.pos, m_c.pos - m_a.pos));
}

std::optional<HitRecord> Triangle::hits(const Ray& ray, const interval& ray_t) const {
    // Möller–Trumbore intersection algorithm:
    // https://en.wikipedia.org/wiki/M%C3%B6ller%E2%80%93Trumbore_intersection_algorithm

    constexpr auto epsilon = std::numeric_limits<double>::epsilon();

    const auto edge_1 = m_b.pos - m_a.pos;
    const auto edge_2 = m_c.pos - m_a.pos;

    const auto ray_cross_e2 = glm::cross(ray.direction(), edge_2);
    const auto det = glm::dot(edge_1, ray_cross_e2);

    if (det > -epsilon && det < epsilon) {
        // Ray is parallel to triangle
        return {};
    }

    const auto inv_det = 1.0 / det;
    const auto s = ray.origin() - m_a.pos;
    const auto u = inv_det * glm::dot(s, ray_cross_e2);

    if (u < 0 || u > 1)
        return {};

    const auto s_cross_e1 = glm::cross(s, edge_1);
    const auto v = inv_det * glm::dot(ray.direction(), s_cross_e1);

    if (v < 0 || u + v > 1)
        return {};

    // Find intersection point in triangle
    const auto t = inv_det * glm::dot(edge_2, s_cross_e1);

    if (t <= epsilon || !ray_t.surrounds(t)) {
        // Line intersection but not ray intersection, or t is not inside ray_t limits
        return {};
    }

    // Compute texture uv
    const auto w = 1.0 - u - v;
    const auto texture_uv = w * m_a.uv + u * m_b.uv + v * m_c.uv;

    // Compute normal
    const auto outward_normal = w * m_a.normal + u * m_b.normal + v * m_c.normal;

    HitRecord record{};
    record.ts = t;
    record.point = ray.at(record.ts);
    record.uv = texture_uv;
    record.material = m_material;

    record.set_front_face(ray, outward_normal);

    return record;
}

AABB Triangle::bounding_box() const {
    return m_bounding_box;
}

//
// Mesh
//

Mesh::Mesh(const std::vector<Triangle::Vertex>& vertices,
           const std::vector<uvec3>& faces,
           const std::shared_ptr<IMaterial>& material) {
    auto max = vec3(std::numeric_limits<double>::min());
    auto min = vec3(std::numeric_limits<double>::max());

    for (const auto f : faces) {
        const auto v1 = vertices[f.x];
        const auto v2 = vertices[f.y];
        const auto v3 = vertices[f.z];

        max = glm::max(max, glm::max(glm::max(v1.pos, v2.pos), v3.pos));
        min = glm::min(min, glm::min(glm::min(v1.pos, v2.pos), v3.pos));

        m_faces.emplace_back(v1, v2, v3, material);
    }

    m_bounding_box = AABB(min, max);
}

std::optional<HitRecord> Mesh::hits(const Ray& ray, const interval& ray_t) const {
    std::optional<HitRecord> record;
    auto closest_max_t = ray_t.max;

    for (const auto& face : m_faces) {
        const auto r = face.hits(ray, interval(ray_t.min, closest_max_t));
        if (r.has_value()) {
            record = r;
            closest_max_t = record->ts;
        }
    }

    return record;
}

AABB Mesh::bounding_box() const {
    return m_bounding_box;
}

//
// Model
//

Model::Model(const std::filesystem::path& path) : Model(path, vec3(0.0), vec3(1.0), vec3(0.0)) {}

Model::Model(const std::filesystem::path& path, vec3 translation, vec3 scale, vec3 rotation) {
    auto transform = glm::dmat4(1.0);
    transform = glm::translate(transform, translation);
    transform = glm::scale(transform, scale);
    transform = glm::rotate(transform, rotation.x, vec3(1.0, 0.0, 0.0));
    transform = glm::rotate(transform, rotation.y, vec3(0.0, 1.0, 0.0));
    transform = glm::rotate(transform, rotation.z, vec3(0.0, 0.0, 1.0));

    Assimp::Importer importer;

    constexpr uint32_t flags = aiProcess_Triangulate | aiProcess_OptimizeMeshes | aiProcess_OptimizeGraph;
    const auto scene = importer.ReadFile(path.c_str(), flags);
    assert(scene); // TODO: UGLY

    auto max = vec3(std::numeric_limits<double>::min());
    auto min = vec3(std::numeric_limits<double>::max());

    for (std::size_t i = 0; i < scene->mNumMeshes; ++i) {
        const auto mesh = scene->mMeshes[i];
        if (mesh->mNumVertices == 0)
            continue;

        load_mesh(mesh, transform);

        max = glm::max(m_meshes.back()->bounding_box().max(), max);
        min = glm::min(m_meshes.back()->bounding_box().min(), min);
    }

    m_meshes.clear();

    // Normalize
    auto size = max - min;
    auto center = (max + min) * 0.5;

    transform = glm::scale(glm::dmat4(1.0), vec3(2.0 / glm::max(size.x, glm::max(size.y, size.z)))) *
                glm::translate(glm::dmat4(1.0), -center);

    for (std::size_t i = 0; i < scene->mNumMeshes; ++i) {
        const auto mesh = scene->mMeshes[i];
        if (mesh->mNumVertices == 0)
            continue;

        load_mesh(mesh, transform);
    }

    // Create BVH from meshes
    m_root = std::make_unique<BVHNode>(m_meshes, 0, m_meshes.size());
}

std::optional<HitRecord> Model::hits(const Ray& ray, const interval& ray_t) const {
    return m_root->hits(ray, ray_t);
}

AABB Model::bounding_box() const {
    return m_root->bounding_box();
}

static std::shared_ptr<IMaterial> s_sample_material = std::make_shared<Lambertian>(vec3(0.18));

void Model::load_mesh(const aiMesh* mesh, const glm::dmat4& transform) {
    assert(mesh->HasTextureCoords(0));

    // Load mesh info
    std::vector<Triangle::Vertex> vertices;
    std::vector<uvec3> face_indices;

    for (std::size_t v = 0; v < mesh->mNumVertices; ++v) {
        const auto& vertex = mesh->mVertices[v];
        const auto& uv = mesh->mTextureCoords[0][v];

        const auto tpos = transform * vec4(vertex.x, vertex.y, vertex.z, 1.0);
        const auto pos = vec3(tpos.x, tpos.y, tpos.z) / tpos.w;

        const auto normal = mesh->mNormals[v].Normalize();

        vertices.push_back({
            .pos = pos,
            .uv = vec2(uv.x, 1.0 - uv.y),
            .normal = vec3(normal.x, normal.y, normal.z),
        });
    }

    for (std::size_t f = 0; f < mesh->mNumFaces; ++f) {
        const auto& face = mesh->mFaces[f];
        assert(face.mNumIndices == 3);

        face_indices.emplace_back(face.mIndices[0], face.mIndices[1], face.mIndices[2]);
    }

    m_meshes.push_back(std::make_shared<Mesh>(vertices, face_indices, s_sample_material));
}

//
// HittableList
//

std::optional<HitRecord> HittableList::hits(const Ray& ray, const interval& ray_t) const {
    std::optional<HitRecord> record;
    auto closest_max_t = ray_t.max;

    for (const auto& object : m_objects) {
        const auto r = object->hits(ray, interval(ray_t.min, closest_max_t));
        if (r.has_value()) {
            record = r;
            closest_max_t = record->ts;
        }
    }

    return record;
}

AABB HittableList::bounding_box() const {
    return m_bounding_box;
}

//
// BVHNode
//

BVHNode::BVHNode(const HittableList& list) : BVHNode(list.objects(), 0, list.objects().size()) {}

BVHNode::BVHNode(const std::vector<std::shared_ptr<IHittable>>& objects, std::size_t start, std::size_t end) {
    const auto axis = random_int(0, 2);
    const auto comparator = (axis == 0) ? box_x_compare : (axis == 1) ? box_y_compare : box_z_compare;

    const auto span = end - start;

    if (span == 1) {
        m_left = m_right = objects[start];
    } else if (span == 2) {
        if (comparator(objects[start], objects[start + 1])) {
            m_left = objects[start];
            m_right = objects[start + 1];
        } else {
            m_left = objects[start + 1];
            m_right = objects[start];
        }
    } else {
        auto objs = objects;
        std::sort(objs.begin() + start, objs.begin() + end, comparator);

        const auto middle = start + span / 2;
        m_left = std::make_shared<BVHNode>(objs, start, middle);
        m_right = std::make_shared<BVHNode>(objs, middle, end);
    }

    m_bounding_box = AABB(m_left->bounding_box(), m_right->bounding_box());
}

std::optional<HitRecord> BVHNode::hits(const Ray& ray, const interval& ray_t) const {
    if (!m_bounding_box.hit(ray, ray_t))
        return {};

    auto hit_left = m_left->hits(ray, ray_t);
    auto hit_right = m_right->hits(ray, interval(ray_t.min, hit_left.has_value() ? hit_left->ts : ray_t.max));

    if (hit_right.has_value())
        return hit_right;
    return hit_left;
}

AABB BVHNode::bounding_box() const {
    return m_bounding_box;
}

bool BVHNode::box_compare(const std::shared_ptr<IHittable>& a, const std::shared_ptr<IHittable>& b, uint32_t axis) {
    return a->bounding_box().axis(axis).min < b->bounding_box().axis(axis).min;
}

bool BVHNode::box_x_compare(const std::shared_ptr<IHittable>& a, const std::shared_ptr<IHittable>& b) {
    return box_compare(a, b, 0);
}

bool BVHNode::box_y_compare(const std::shared_ptr<IHittable>& a, const std::shared_ptr<IHittable>& b) {
    return box_compare(a, b, 1);
}

bool BVHNode::box_z_compare(const std::shared_ptr<IHittable>& a, const std::shared_ptr<IHittable>& b) {
    return box_compare(a, b, 2);
}
