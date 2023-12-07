#pragma once

#include <glm/glm.hpp>

using vec3 = glm::dvec3;

vec3 vec3_random();
vec3 vec3_random(double min, double max);

vec3 vec3_random_in_unit_sphere();
vec3 vec3_random_unit();

vec3 random_on_hemisphere(const vec3& normal);

bool vec3_near_zero(const vec3& v);
