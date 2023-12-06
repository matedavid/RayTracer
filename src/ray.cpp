#include "ray.h"

Ray::Ray(vec3 origin, vec3 direction) : m_origin(origin), m_direction(direction) {}

vec3 Ray::at(double ts) const {
    return m_origin + m_direction * ts;
}
