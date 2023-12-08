#pragma once

#include <cstdint>
#include <filesystem>
#include <vector>

#include "vec3.h"

class IImageDumper {
  public:
    virtual ~IImageDumper() = default;

    [[nodiscard]] virtual uint32_t width() const = 0;
    [[nodiscard]] virtual uint32_t height() const = 0;

    [[nodiscard]] virtual std::vector<vec3>& operator[](std::size_t row) = 0;
    [[nodiscard]] virtual const std::vector<vec3>& operator[](std::size_t row) const = 0;

    virtual void dump(const std::filesystem::path& path) const = 0;
};

class PPMImageDumper : public IImageDumper {
  public:
    PPMImageDumper(uint32_t width, uint32_t height);
    ~PPMImageDumper() override = default;

    [[nodiscard]] uint32_t width() const override { return m_width; }
    [[nodiscard]] uint32_t height() const override { return m_height; }

    [[nodiscard]] std::vector<vec3>& operator[](std::size_t row) override;
    [[nodiscard]] const std::vector<vec3>& operator[](std::size_t row) const override;

    void dump(const std::filesystem::path& path) const override;

  private:
    std::vector<std::vector<vec3>> m_data;
    uint32_t m_width, m_height;
};