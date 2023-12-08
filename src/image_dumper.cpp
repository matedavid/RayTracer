#include "image_dumper.h"

#include <fstream>

#include "interval.h"

PPMImageDumper::PPMImageDumper(uint32_t width, uint32_t height) : m_width(width), m_height(height) {
    m_data = {m_height, std::vector<vec3>(m_width, vec3(0.0))};
}

std::vector<vec3>& PPMImageDumper::operator[](std::size_t row) {
    assert(row >= 0 && row < m_height);
    return m_data[row];
}

const std::vector<vec3>& PPMImageDumper::operator[](std::size_t row) const {
    assert(row >= 0 && row < m_height);
    return m_data[row];
}

void PPMImageDumper::dump(const std::filesystem::path& path) const {
    std::ofstream file(path);

    file << "P3\n" << m_width << " " << m_height << "\n255\n";

    constexpr interval intensity(0.0, 0.999);

    for (std::size_t row = 0; row < m_height; ++row) {
        for (std::size_t col = 0; col < m_width; ++col) {
            const auto& color = m_data[row][col];

            const auto r = color.r;
            const auto g = color.g;
            const auto b = color.b;

            file << static_cast<int>(intensity.clamp(r) * 256.0) << " ";
            file << static_cast<int>(intensity.clamp(g) * 256.0) << " ";
            file << static_cast<int>(intensity.clamp(b) * 256.0) << "\n";
        }
    }

    file.close();
}
