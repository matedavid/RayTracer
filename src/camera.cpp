#include "camera.h"

Camera::Camera(Description description) : m_desc(description) {
    // Change value of fov from degrees to radians
    m_desc.vertical_fov = glm::radians(m_desc.vertical_fov);

    const auto aspect_ratio = static_cast<double>(m_desc.width) / static_cast<double>(m_desc.height);

    const auto focal_length = glm::length(m_desc.look_from - m_desc.look_at);

    const auto viewport_height = 2.0 * focal_length * glm::tan(m_desc.vertical_fov / 2.0);
    const auto viewport_width = viewport_height * aspect_ratio;

    const auto w = glm::normalize(m_desc.look_from - m_desc.look_at);
    const auto u = glm::normalize(glm::cross(m_desc.up, w));
    const auto v = glm::cross(w, u);

    const auto viewport_u = viewport_width * u;
    const auto viewport_v = viewport_height * (-v);

    m_delta_u = viewport_u / static_cast<double>(m_desc.width);
    m_delta_v = viewport_v / static_cast<double>(m_desc.height);

    const auto viewport_upper_left = m_desc.look_from - (focal_length * w) - viewport_u / 2.0 - viewport_v / 2.0;
    m_pixel00_loc = viewport_upper_left + 0.5 * (m_delta_u + m_delta_v);
}
