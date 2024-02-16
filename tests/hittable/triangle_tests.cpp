#include <catch2/catch_all.hpp>

#include "ray.h"

#include "hittable/triangle.h"
#include "hittable/hittable_list.h"

TEST_CASE("Triangle bounding box correct", "[Hittable_Triangle]") {
    SECTION("Triangle facing z") {
        const auto a = Triangle::Vertex{.pos = vec3(-2.0, -1.0, -1.0)};
        const auto b = Triangle::Vertex{.pos = vec3(2.0, -1.0, -1.0)};
        const auto c = Triangle::Vertex{.pos = vec3(0.0, 3.0, -1.0)};

        Triangle triangle(a, b, c, nullptr);
        const auto bbox = triangle.bounding_box();

        REQUIRE(bbox.axis(0).min == -2.0);
        REQUIRE(bbox.axis(0).max == 2.0);

        REQUIRE(bbox.axis(1).min == -1.0);
        REQUIRE(bbox.axis(1).max == 3.0);

        REQUIRE(bbox.axis(2).min == -1.0);
        REQUIRE(bbox.axis(2).max == -1.0);
    }

    SECTION("Triangle facing x") {
        const auto a = Triangle::Vertex{.pos = vec3(3.0, 4.0, -1.0)};
        const auto b = Triangle::Vertex{.pos = vec3(3.0, 2.0, 1.0)};
        const auto c = Triangle::Vertex{.pos = vec3(3.0, 1.0, 0.0)};

        Triangle triangle(a, b, c, nullptr);
        const auto bbox = triangle.bounding_box();

        REQUIRE(bbox.axis(0).min == 3.0);
        REQUIRE(bbox.axis(0).max == 3.0);

        REQUIRE(bbox.axis(1).min == 1.0);
        REQUIRE(bbox.axis(1).max == 4.0);

        REQUIRE(bbox.axis(2).min == -1.0);
        REQUIRE(bbox.axis(2).max == 1.0);
    }

    SECTION("Triangle facing y") {
        const auto a = Triangle::Vertex{.pos = vec3(-2.0, -3.0, 4.0)};
        const auto b = Triangle::Vertex{.pos = vec3(-3.0, -3.0, -1.0)};
        const auto c = Triangle::Vertex{.pos = vec3(1.0, -3.0, 2.0)};

        Triangle triangle(a, b, c, nullptr);
        const auto bbox = triangle.bounding_box();

        REQUIRE(bbox.axis(0).min == -3.0);
        REQUIRE(bbox.axis(0).max == 1.0);

        REQUIRE(bbox.axis(1).min == -3.0);
        REQUIRE(bbox.axis(1).max == -3.0);

        REQUIRE(bbox.axis(2).min == -1.0);
        REQUIRE(bbox.axis(2).max == 4.0);
    }
}

TEST_CASE("Ray hits triangle in front", "[Hittable_Triangle]") {
    Triangle triangle(Triangle::Vertex{.pos = vec3(-1.0, 0.0, 0.0), .normal = vec3(0.0, 0.0, -1.0)},
                      Triangle::Vertex{.pos = vec3(1.0, 0.0, 0.0), .normal = vec3(0.0, 0.0, -1.0)},
                      Triangle::Vertex{.pos = vec3(0.0, 1.0, 0.0), .normal = vec3(0.0, 0.0, -1.0)},
                      nullptr);
    Ray ray(vec3(0.0, 0.0, -1.0), vec3(0.0, 0.0, 1.0));

    const auto record = triangle.hits(ray, interval(0.0, interval::infinity));
    REQUIRE(record.has_value());
    REQUIRE(record->ts == 1.0);
    REQUIRE(record->normal == vec3(0.0, 0.0, -1.0));
    REQUIRE(record->front_face);
}

TEST_CASE("Ray hits triangle in front from the back", "[Hittable_Triangle]") {
    Triangle triangle(Triangle::Vertex{.pos = vec3(-1.0, 0.0, 0.0), .normal = vec3(0.0, 0.0, -1.0)},
                      Triangle::Vertex{.pos = vec3(1.0, 0.0, 0.0), .normal = vec3(0.0, 0.0, -1.0)},
                      Triangle::Vertex{.pos = vec3(0.0, 1.0, 0.0), .normal = vec3(0.0, 0.0, -1.0)},
                      nullptr);
    Ray ray(vec3(0.0, 0.0, 1.0), vec3(0.0, 0.0, -1.0));

    const auto record = triangle.hits(ray, interval(0.0, interval::infinity));
    REQUIRE(record.has_value());
    REQUIRE(record->ts == 1.0);
    REQUIRE(record->normal == vec3(0.0, 0.0, 1.0));
    REQUIRE(!record->front_face);
}

TEST_CASE("Ray misses triangle diagonally", "[HitTriangle]") {
    Triangle triangle(Triangle::Vertex{.pos = vec3(-1.0, 0.0, 0.0)},
                      Triangle::Vertex{.pos = vec3(1.0, 0.0, 0.0)},
                      Triangle::Vertex{.pos = vec3(0.0, 1.0, 0.0)},
                      nullptr);
    Ray ray(vec3(0.0, 1.0, 0.0), vec3(1.0, -1.0, 0.0));

    const auto record = triangle.hits(ray, interval(0.0, interval::infinity));
    REQUIRE(!record.has_value());
}

TEST_CASE("Ray hits triangle interval", "[Hittable_Triangle]") {
    Triangle triangle(Triangle::Vertex{.pos = vec3(-1.0, -1.0, 0.0)},
                      Triangle::Vertex{.pos = vec3(1.0, -1.0, 0.0)},
                      Triangle::Vertex{.pos = vec3(0.0, 1.0, 0.0)},
                      nullptr);
    Ray ray(vec3(0.0, 0.0, -1.0), vec3(0.0, 0.0, 1.0));

    const auto record = triangle.hits(ray, interval(0.0, 1.1));
    REQUIRE(record.has_value());
    REQUIRE(record->ts == 1.0);
}

TEST_CASE("Ray misses triangle because of interval", "[Hittable_Triangle]") {
    Triangle triangle(Triangle::Vertex{.pos = vec3(-1.0, -1.0, 0.0)},
                      Triangle::Vertex{.pos = vec3(1.0, -1.0, 0.0)},
                      Triangle::Vertex{.pos = vec3(0.0, 1.0, 0.0)},
                      nullptr);
    Ray ray(vec3(0.0, 0.0, -1.0), vec3(0.0, 0.0, 1.0));

    const auto record = triangle.hits(ray, interval(0.0, 0.9));
    REQUIRE(!record.has_value());
}

TEST_CASE("Correct triangle uv coordinates", "[Hittable_Triangle]") {
    HittableList scene;
    scene.add_hittable<Triangle>(Triangle::Vertex{.pos = {0.0, 1.0, 0.0}, .uv = {0.0, 0.0}},
                                 Triangle::Vertex{.pos = {0.0, 0.0, 0.0}, .uv = {0.0, 1.0}},
                                 Triangle::Vertex{.pos = {1.0, 0.0, 0.0}, .uv = {1.0, 1.0}},
                                 nullptr);
    scene.add_hittable<Triangle>(Triangle::Vertex{.pos = {1.0, 1.0, 0.0}, .uv = {1.0, 0.0}},
                                 Triangle::Vertex{.pos = {0.0, 1.0, 0.0}, .uv = {0.0, 0.0}},
                                 Triangle::Vertex{.pos = {1.0, 0.0, 0.0}, .uv = {1.0, 1.0}},
                                 nullptr);

    const auto record = scene.hits(Ray(vec3(0.5, 0.5, -1.0), vec3(0.0, 0.0, 1.0)), interval(0.0, interval::infinity));
    REQUIRE(record.has_value());
    REQUIRE(record->uv == vec2(0.5, 0.5));

    const auto record2 = scene.hits(Ray(vec3(0.7, 0.5, -1.0), vec3(0.0, 0.0, 1.0)), interval(0.0, interval::infinity));
    REQUIRE(record2.has_value());
    REQUIRE(record2->uv == vec2(0.7, 0.5));

    const auto record3 = scene.hits(Ray(vec3(0.5, 0.1, -1.0), vec3(0.0, 0.0, 1.0)), interval(0.0, interval::infinity));
    REQUIRE(record3.has_value());
    REQUIRE(record3->uv == vec2(0.5, 0.9));
}