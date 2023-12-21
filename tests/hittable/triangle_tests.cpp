#include <catch2/catch_all.hpp>

#include "hittable.h"
#include "ray.h"

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

TEST_CASE("Ray misses sphere because of interval", "[Hittable_Triangle]") {
    Triangle triangle(Triangle::Vertex{.pos = vec3(-1.0, -1.0, 0.0)},
                      Triangle::Vertex{.pos = vec3(1.0, -1.0, 0.0)},
                      Triangle::Vertex{.pos = vec3(0.0, 1.0, 0.0)},
                      nullptr);
    Ray ray(vec3(0.0, 0.0, -1.0), vec3(0.0, 0.0, 1.0));

    const auto record = triangle.hits(ray, interval(0.0, 0.9));
    REQUIRE(!record.has_value());
}
