#include "model.h"

#include <vector>
#include <glm/gtc/matrix_transform.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "material.h"
#include "hittable/bvh_node.h"

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