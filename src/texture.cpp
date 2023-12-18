#include "texture.h"

#include <cassert>
#include <cstring>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

Texture::Texture(const std::filesystem::path& path, Filtering filtering) : m_filtering(filtering) {
    int32_t width, height, channels;
    auto* data = stbi_load(path.c_str(), &width, &height, &channels, STBI_rgb);

    assert(channels == 3);

    m_width = static_cast<uint32_t>(width);
    m_height = static_cast<uint32_t>(height);
    m_channels = static_cast<uint32_t>(channels);

    m_data = std::vector<std::byte>(m_width * m_height * m_channels * sizeof(std::byte));
    memcpy(m_data.data(), data, m_data.size());

    stbi_image_free(data);
}

vec3 Texture::access(double u, double v) const {
    const auto u_pos = static_cast<double>(m_width - 1) * u;
    const auto v_pos = static_cast<double>(m_height - 1) * v;

    switch (m_filtering) {
    default:
    case Filtering::Nearest: {
        const auto u_nearest = static_cast<uint32_t>(round(u_pos));
        const auto v_nearest = static_cast<uint32_t>(round(v_pos));

        return get_color_at(u_nearest, v_nearest);
    }
    case Filtering::Bilinear:
        assert(false && "Not implemented");
        break;
    case Filtering::Trilinear:
        assert(false && "Not implemented");
        break;
    case Filtering::Anisotropic:
        assert(false && "Not implemented");
        break;
    }
}

vec3 Texture::get_color_at(uint32_t u, uint32_t v) const {
    const auto pos = u * m_width * m_channels + v * m_channels;
    assert(pos < m_data.size());

    return {m_data[pos], m_data[pos + 1], m_data[pos + 2]};
}
