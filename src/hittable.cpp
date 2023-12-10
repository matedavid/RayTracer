#include "hittable.h"

#include <algorithm>

#include "ray.h"
#include "interval.h"
#include "rand.h"

// HitRecord
void HitRecord::set_front_face(const Ray& ray, const vec3& outward_normal) {
    front_face = glm::dot(ray.direction(), outward_normal) < 0.0;
    normal = front_face ? outward_normal : -outward_normal;
}

//
// Sphere
//
Sphere::Sphere(vec3 position, double radius, std::shared_ptr<IMaterial> material)
      : m_position(position), m_radius(radius), m_material(std::move(material)) {
    const auto rvec3 = vec3(radius);
    m_bounding_box = AABB(m_position - rvec3, m_position + rvec3);
}

std::optional<HitRecord> Sphere::hits(const Ray& ray, const interval& ray_t) const {
    const auto oc = ray.origin() - m_position;

    const auto a = glm::dot(ray.direction(), ray.direction());
    const auto b = 2.0f * glm::dot(ray.direction(), oc);
    const auto c = glm::dot(oc, oc) - m_radius * m_radius;

    const auto discriminant = b * b - 4.0f * a * c;
    if (discriminant < 0.0f)
        return {};

    auto root = (-b - glm::sqrt(discriminant)) / (2.0f * a);
    if (!ray_t.surrounds(root)) {
        root = (-b + glm::sqrt(discriminant)) / (2.0f * a);

        if (!ray_t.surrounds(root)) {
            return {};
        }
    }

    HitRecord record{};
    record.ts = root;
    record.point = ray.at(record.ts);
    record.material = m_material;

    const auto outward_normal = (record.point - m_position) / m_radius;
    record.set_front_face(ray, outward_normal);

    return record;
}

AABB Sphere::bounding_box() const {
    return m_bounding_box;
}

//
// HittableList
//

std::optional<HitRecord> HittableList::hits(const Ray& ray, const interval& ray_t) const {
    std::optional<HitRecord> record;
    auto closest_max_t = ray_t.max;

    for (const auto& object : m_objects) {
        const auto r = object->hits(ray, interval(ray_t.min, closest_max_t));
        if (r.has_value()) {
            record = r;
            closest_max_t = record->ts;
        }
    }

    return record;
}

AABB HittableList::bounding_box() const {
    return m_bounding_box;
}

//
// BVHNode
//

BVHNode::BVHNode(const HittableList& list) : BVHNode(list.objects(), 0, list.objects().size()) {}

BVHNode::BVHNode(const std::vector<std::shared_ptr<IHittable>>& objects, std::size_t start, std::size_t end) {
    const auto axis = random_int(0, 2);
    const auto comparator = (axis == 0) ? box_x_compare : (axis == 1) ? box_y_compare : box_z_compare;

    const auto span = end - start;

    if (span == 1) {
        m_left = m_right = objects[start];
    } else if (span == 2) {
        if (comparator(objects[start], objects[start + 1])) {
            m_left = objects[start];
            m_right = objects[start + 1];
        } else {
            m_left = objects[start + 1];
            m_right = objects[start];
        }
    } else {
        auto objs = objects;
        std::sort(objs.begin() + start, objs.begin() + end, comparator);

        const auto middle = start + span / 2;
        m_left = std::make_shared<BVHNode>(objs, start, middle);
        m_right = std::make_shared<BVHNode>(objs, middle, end);
    }

    m_bounding_box = AABB(m_left->bounding_box(), m_right->bounding_box());
}

std::optional<HitRecord> BVHNode::hits(const Ray& ray, const interval& ray_t) const {
    if (!m_bounding_box.hit(ray, ray_t))
        return {};

    auto hit_left = m_left->hits(ray, ray_t);
    auto hit_right = m_right->hits(ray, interval(ray_t.min, hit_left.has_value() ? hit_left->ts : ray_t.max));

    if (hit_right.has_value())
        return hit_right;
    return hit_left;
}

AABB BVHNode::bounding_box() const {
    return m_bounding_box;
}

bool BVHNode::box_compare(const std::shared_ptr<IHittable>& a, const std::shared_ptr<IHittable>& b, uint32_t axis) {
    return a->bounding_box().axis(axis).min < b->bounding_box().axis(axis).min;
}

bool BVHNode::box_x_compare(const std::shared_ptr<IHittable>& a, const std::shared_ptr<IHittable>& b) {
    return box_compare(a, b, 0);
}

bool BVHNode::box_y_compare(const std::shared_ptr<IHittable>& a, const std::shared_ptr<IHittable>& b) {
    return box_compare(a, b, 1);
}

bool BVHNode::box_z_compare(const std::shared_ptr<IHittable>& a, const std::shared_ptr<IHittable>& b) {
    return box_compare(a, b, 2);
}
