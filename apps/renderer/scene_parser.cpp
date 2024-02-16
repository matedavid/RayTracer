#include "scene_parser.h"

#include <iostream>
#include <fstream>
#include <cassert>

#include <nlohmann/json.hpp>
using json = nlohmann::json;

#include "RayTracer/hittable.h"
#include "RayTracer/material.h"

std::optional<SceneParser> SceneParser::parse(const std::filesystem::path& path) {
    if (!std::filesystem::exists(path)) {
        std::cout << "Could not open scene file in path: " << path << "\n";
        return std::nullopt;
    }

    return SceneParser(path);
}

SceneParser::SceneParser(const std::filesystem::path& path) {
    std::ifstream file(path);
    assert(file.is_open() && ("Error opening file in path: " + path.string()).c_str());

    const auto data = json::parse(file);

    m_camera_description = {};
    m_scene = std::make_shared<HittableList>();

    if (data.contains("camera"))
        parse_camera(data["camera"]);

    if (data.contains("scene") && data["scene"].is_array())
        parse_scene(data["scene"]);
}

template <typename T>
T parse_value(const json& data, const std::string& key, const T& default_value = {}) {
    return data.contains(key) ? data[key].get<T>() : default_value;
}

vec3 parse_value(const json& data, const std::string& key, const vec3& default_value = vec3(0.0)) {
    return data.contains(key) ? vec3(data[key][0], data[key][1], data[key][2]) : default_value;
}

void SceneParser::parse_camera(const auto& data) {
    m_camera_description.width = parse_value(data, "width", m_camera_description.width);
    m_camera_description.height = parse_value(data, "height", m_camera_description.height);

    m_camera_description.vertical_fov = parse_value(data, "fov", m_camera_description.vertical_fov);
    m_camera_description.look_from = parse_value(data, "lookFrom", m_camera_description.look_from);
    m_camera_description.look_at = parse_value(data, "lookAt", m_camera_description.look_at);
    m_camera_description.up = parse_value(data, "up", m_camera_description.up);
}

std::shared_ptr<IMaterial> parse_material(const json& data) {
    const auto& type = data["type"];

    std::shared_ptr<IMaterial> mat;
    if (type == "lambertian") {
        const auto albedo = parse_value(data, "albedo", vec3(1.0));
        mat = std::make_shared<Lambertian>(albedo);
    } else if (type == "metal") {
        const auto albedo = parse_value(data, "albedo", vec3(1.0));
        const auto fuzz = parse_value(data, "fuzz", 0.0);
        mat = std::make_shared<Metal>(albedo, fuzz);
    } else if (type == "dielectric") {
        const auto refraction_index = parse_value(data, "index", 0.0);
        mat = std::make_shared<Dielectric>(refraction_index);
    } else if (type == "emissive") {
        const auto color = parse_value(data, "color", vec3(1.0));
        const auto intensity = parse_value(data, "intensity", 1.0);
        mat = std::make_shared<DiffuseEmissive>(color, intensity);
    } else {
        std::cout << "Material with type: '" << type << "' not supported\n";
        return nullptr;
    }

    return mat;
}

void SceneParser::parse_scene(const auto& data) {
    assert(data.is_array() && "Scene description must be an array");

    for (const auto& obj : data) {
        const auto type = obj["type"];

        if (type == "sphere") {
            const auto center = parse_value(obj, "center", vec3(0.0));
            const auto radius = parse_value(obj, "radius", 0.5);

            const auto material = parse_material(obj["material"]);
            assert(material != nullptr);

            m_scene->add_hittable<Sphere>(center, radius, material);
        }
    }
}