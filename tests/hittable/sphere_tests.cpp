#include <catch2/catch_test_macros.hpp>

#include "hittable.h"
#include "ray.h"

TEST_CASE("Ray hits sphere in front", "[Hittable_Sphere]") {
    Sphere sphere(vec3(0.0), 1.0, nullptr);
    Ray ray(vec3(0.0, 0.0, -2.0), vec3(0.0, 0.0, 1.0));

    const auto record = sphere.hits(ray, interval(0.0, interval::infinity));
    REQUIRE(record.has_value());
    REQUIRE(record->ts == 1.0);
    REQUIRE(record->normal == vec3(0.0, 0.0, -1.0));
    REQUIRE(record->front_face);
}

TEST_CASE("Ray hits sphere from inside", "[Hittable_Sphere]") {
    Sphere sphere(vec3(0.0), 1.0, nullptr);
    Ray ray(vec3(0.0), vec3(0.0, 0.0, 1.0));

    const auto record = sphere.hits(ray, interval(0.0, interval::infinity));
    REQUIRE(record.has_value());
    REQUIRE(record->ts == 1.0);
    REQUIRE(record->normal == vec3(0.0, 0.0, -1.0));
    REQUIRE(!record->front_face);
}

TEST_CASE("Ray does not hit sphere", "[Hittable_Sphere]") {
    Sphere sphere(vec3(0.0), 1.0, nullptr);
    Ray ray(vec3(0.0, 0.0, -2.0), vec3(0.0, 1.0, 0.0));

    const auto record = sphere.hits(ray, interval(0.0, interval::infinity));
    REQUIRE(!record.has_value());
}

TEST_CASE("Ray misses sphere diagonally", "[Hittable_Sphere]") {
    Sphere sphere(vec3(0.0), 1.0, nullptr);
    Ray ray(vec3(0.0, 1.5, 0.0), vec3(1.0, -1.0, 0.0));

    const auto record = sphere.hits(ray, interval(0.0, interval::infinity));
    REQUIRE(!record.has_value());
}

TEST_CASE("Ray hits sphere diagonally", "[Hittable_Sphere]") {
    Sphere sphere(vec3(0.0), 1.0, nullptr);
    Ray ray(vec3(0.0, 1.4, 0.0), vec3(1.0, -1.0, 0.0));

    const auto record = sphere.hits(ray, interval(0.0, interval::infinity));
    REQUIRE(record.has_value());
}

TEST_CASE("Ray misses sphere parallel", "[Hittable_Sphere]") {
    Sphere sphere(vec3(0.0), 1.0, nullptr);
    Ray ray(vec3(0.0, -5.0, -1.1), vec3(0.0, 1.0, 0.0));

    const auto record = sphere.hits(ray, interval(0.0, interval::infinity));
    REQUIRE(!record.has_value());
}

TEST_CASE("Ray hits sphere interval", "[Hittable_Sphere]") {
    Sphere sphere(vec3(0.0), 1.0, nullptr);
    Ray ray(vec3(0.0, 0.0, -2.0), vec3(0.0, 0.0, 1.0));

    const auto record = sphere.hits(ray, interval(0.0, 1.1));
    REQUIRE(record.has_value());
    REQUIRE(record->ts == 1.0);
}

TEST_CASE("Ray misses sphere because of interval", "[Hittable_Sphere]") {
    Sphere sphere(vec3(0.0), 1.0, nullptr);
    Ray ray(vec3(0.0, 0.0, -2.0), vec3(0.0, 0.0, 1.0));

    const auto record = sphere.hits(ray, interval(0.0, 0.9));
    REQUIRE(!record.has_value());
}
