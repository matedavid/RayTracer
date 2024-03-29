#include "vec.h"

#include "rand.h"

vec3 vec3_random() {
    return {random_double(), random_double(), random_double()};
}

vec3 vec3_random(double min, double max) {
    return {random_double(min, max), random_double(min, max), random_double(min, max)};
}

vec3 vec3_random_in_unit_sphere() {
    while (true) {
        auto p = vec3_random(-1.0, 1.0);
        const auto length_squared = glm::length(p) * glm::length(p);
        if (length_squared < 1.0)
            return p;
    }
}

vec3 vec3_random_unit() {
    return glm::normalize(vec3_random_in_unit_sphere());
}

vec3 random_on_hemisphere(const vec3& normal) {
    vec3 on_unit_sphere = vec3_random_unit();
    if (glm::dot(on_unit_sphere, normal) > 0.0) // In the same hemisphere as the normal
        return on_unit_sphere;
    else
        return -on_unit_sphere;
}

vec3 random_cosine_direction() {
    const auto r1 = random_double();
    const auto r2 = random_double();

    const auto phi = 2 * M_PI * r1;
    const auto x = glm::cos(phi) * glm::sqrt(r2);
    const auto y = glm::sin(phi) * glm::sqrt(r2);
    const auto z = glm::sqrt(1 - r2);

    return vec3(x, y, z);
}

bool vec3_near_zero(const vec3& v) {
    auto s = 1e-8;
    return (fabs(v.x) < s) && (fabs(v.y) < s) && (fabs(v.z) < s);
}
