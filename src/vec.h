#pragma once

#include <glm/glm.hpp>

using uvec3 = glm::uvec3;

using vec2 = glm::dvec2;
using vec3 = glm::dvec3;
using vec4 = glm::dvec4;

vec3 vec3_random();
vec3 vec3_random(double min, double max);

vec3 vec3_random_in_unit_sphere();
vec3 vec3_random_unit();

vec3 random_on_hemisphere(const vec3& normal);
vec3 random_cosine_direction();

bool vec3_near_zero(const vec3& v);
