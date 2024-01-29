#include "texture.h"

#include <cassert>
#include <cstring>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

Texture::Texture(const std::filesystem::path& path, Filtering filtering) : m_filtering(filtering) {
    int32_t width, height, channels;
    auto* data = stbi_load(path.c_str(), &width, &height, &channels, 0);
    assert(data != nullptr);

    assert(channels == 4 || channels == 3);

    m_width = static_cast<uint32_t>(width);
    m_height = static_cast<uint32_t>(height);
    m_channels = static_cast<uint32_t>(channels);

    const auto data_size = m_width * m_height * channels * sizeof(std::byte);
    m_data = std::vector<std::byte>(data_size);
    memcpy(m_data.data(), data, data_size);

    stbi_image_free(data);
}

vec3 Texture::sample(double u, double v) const {
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
    const auto pos = v * m_width * m_channels + u * m_channels;
    assert(pos < m_data.size());

    return {static_cast<double>(m_data[pos]) / 255.0,
            static_cast<double>(m_data[pos + 1]) / 255.0,
            static_cast<double>(m_data[pos + 2]) / 255.0};
}
