#pragma once

#include <cstdint>
#include <filesystem>
#include <vector>

#include "vec.h"

class Texture {
  public:
    enum class Filtering {
        Nearest,
        Bilinear,
        Trilinear,
        Anisotropic,
    };

    Texture(const std::filesystem::path& path, Filtering filtering);
    ~Texture() = default;

    [[nodiscard]] vec3 access(double u, double v) const;

    [[nodiscard]] uint32_t width() const { return m_width; }
    [[nodiscard]] uint32_t height() const { return m_height; }
    [[nodiscard]] uint32_t channels() const { return m_channels; }

  private:
    uint32_t m_width, m_height, m_channels;
    Filtering m_filtering;

    std::vector<std::byte> m_data;

    [[nodiscard]] vec3 get_color_at(uint32_t u, uint32_t v) const;
};
