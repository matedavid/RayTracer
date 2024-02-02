#pragma once

#include "vec.h"

class ONB {
  public:
    ONB() = default;

    [[nodiscard]] vec3 operator[](int i) const { return axis[i]; }
    [[nodiscard]] vec3& operator[](int i) { return axis[i]; }

    [[nodiscard]] vec3 u() const { return axis[0]; }
    [[nodiscard]] vec3 v() const { return axis[1]; }
    [[nodiscard]] vec3 w() const { return axis[2]; }

    [[nodiscard]] vec3 local(double a, double b, double c) const { return a * u() + b * v() + c * w(); }

    [[nodiscard]] vec3 local(const vec3& a) const { return a.x * u() + a.y * v() + a.z * w(); }

    void build_from_w(const vec3& w) {
        const auto unit_w = glm::normalize(w);
        const auto a = fabs(unit_w.x) > 0.9 ? vec3(0, 1, 0) : vec3(1, 0, 0);
        const auto v = glm::normalize(cross(unit_w, a));
        const auto u = cross(unit_w, v);
        axis[0] = u;
        axis[1] = v;
        axis[2] = unit_w;
    }

  private:
    vec3 axis[3];
};
