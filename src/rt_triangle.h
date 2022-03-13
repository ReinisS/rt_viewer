#pragma once

#include "rt_hitable.h"

namespace rt {

class Triangle : public Hitable {
  public:
    Triangle() {}
    Triangle(const glm::vec3 &a, const glm::vec3 &b, const glm::vec3 &c, shared_ptr<Material> m) : v0(a), v1(b), v2(c), mat_ptr(m){};

    virtual bool hit(RTContext &rtx, const Ray &r, float t_min, float t_max, HitRecord &rec) const;

    virtual bool bounding_box(double time0, double time1, AABB& output_box) const override;

    glm::vec3 v0;
    glm::vec3 v1;
    glm::vec3 v2;
    shared_ptr<Material> mat_ptr;
};

// Ray-triangle test adapted from "Real-Time Collision Detection" book (pages 191--192)
bool Triangle::hit(RTContext &rtx, const Ray &r, float t_min, float t_max, HitRecord &rec) const
{
    glm::vec3 n = glm::cross(v1 - v0, v2 - v0);
    float d = glm::dot(-r.direction(), n);
    if (d > 0.0f) {
        float temp = glm::dot(r.origin() - v0, n);
        if (temp >= 0.0f) {
            glm::vec3 e = glm::cross(-r.direction(), r.origin() - v0);
            float v = glm::dot(v2 - v0, e);
            float w = -glm::dot(v1 - v0, e);
            if (v >= 0.0f && v <= d && w >= 0.0f && v + w <= d) {
                temp /= d;
                if (temp < t_max && temp > t_min) {
                    rec.t = temp;
                    rec.p = r.point_at_parameter(rec.t);
                    rec.normal = n;
                    rec.set_face_normal(r, rec.normal);
                    rec.mat_ptr = mat_ptr;
                    return true;
                }
            }
        }
    }
    return false;
}

// "Finding the bounding box for a triangle is a matter of finding the smallest and largest x, y, and z components from its three points."
// http://raytracerchallenge.com/bonus/bounding-boxes.html
bool Triangle::bounding_box(double time0, double time1, AABB& output_box) const {
    output_box = AABB(
        glm::vec3(glm::min(v0.x, v1.x, v2.x), glm::min(v0.y, v1.y, v2.y), glm::min(v0.z, v1.z, v2.z)),
        glm::vec3(glm::max(v0.x, v1.x, v2.x), glm::max(v0.y, v1.y, v2.y), glm::max(v0.z, v1.z, v2.z)));
    return true;
}

}  // namespace rt
