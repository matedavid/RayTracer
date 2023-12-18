#pragma once

#include <cstdint>
#include "vec.h"

class Camera {
  public:
    struct Description {
        uint32_t width = 1920;
        uint32_t height = 1080;

        // Camera positioning
        double vertical_fov = glm::radians(90.0f); // in radians
        vec3 look_from = vec3(0.0, 0.0, -1.0);
        vec3 look_at = vec3(0.0);
        vec3 up = vec3(0.0, 1.0, 0.0);
    };

    explicit Camera(Description description);

    [[nodiscard]] uint32_t width() const { return m_desc.width; }
    [[nodiscard]] uint32_t height() const { return m_desc.height; }

    [[nodiscard]] vec3 center() const { return m_desc.look_from; }
    [[nodiscard]] std::pair<vec3, vec3> deltas() const { return {m_delta_u, m_delta_v}; }
    [[nodiscard]] vec3 pixel00_location() const { return m_pixel00_loc; }

  private:
    Description m_desc;

    vec3 m_delta_u{}, m_delta_v{};
    vec3 m_pixel00_loc{};
};
