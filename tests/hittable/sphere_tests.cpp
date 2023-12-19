#include <catch2/catch_all.hpp>

#include "aabb.h"
#include "hittable.h"
#include "ray.h"

TEST_CASE("Sphere bounding box correct", "[Hittable_Sphere]") {
    const auto x = GENERATE(take(5, random(-5.0, 5.0)));
    const auto y = GENERATE(take(5, random(-5.0, 5.0)));
    const auto z = GENERATE(take(5, random(-5.0, 5.0)));
    const auto r = GENERATE(take(5, random(0.1, 5.0)));

    Sphere sphere(vec3(x, y, z), r, nullptr);
    const auto bbox = sphere.bounding_box();

    REQUIRE(bbox.axis(0).min == x - r);
    REQUIRE(bbox.axis(0).max == x + r);

    REQUIRE(bbox.axis(1).min == y - r);
    REQUIRE(bbox.axis(1).max == y + r);

    REQUIRE(bbox.axis(2).min == z - r);
    REQUIRE(bbox.axis(2).max == z + r);
}

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