#pragma once

#include <filesystem>
#include <optional>
#include <memory>

#include "camera.h"

class HittableList;

class SceneParser {
  public:
    static std::optional<SceneParser> parse(const std::filesystem::path& path);

    [[nodiscard]] Camera::Description camera_description() const { return m_camera_description; }
    [[nodiscard]] std::shared_ptr<HittableList> scene() const { return m_scene; }

  private:
    Camera::Description m_camera_description{};
    std::shared_ptr<HittableList> m_scene{};

    SceneParser(const std::filesystem::path& path);

    void parse_camera(const auto& data);
    void parse_scene(const auto& data);
};