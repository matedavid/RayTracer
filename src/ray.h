#pragma once

#include "vec.h"

class Ray {
  public:
    Ray(vec3 origin, vec3 direction);

    [[nodiscard]] vec3 at(double ts) const;

    [[nodiscard]] vec3 origin() const { return m_origin; }
    [[nodiscard]] vec3 direction() const { return m_direction; }

  private:
    vec3 m_origin, m_direction;
};
