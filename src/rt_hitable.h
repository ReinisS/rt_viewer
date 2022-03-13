#pragma once

#include "rt_weekend.h"
#include "rt_aabb.h"

namespace rt {

class Material;

struct HitRecord {
    float t;
    glm::vec3 p;
    glm::vec3 normal;
    bool front_face;
    shared_ptr<Material> mat_ptr;

    inline void set_face_normal(const Ray& r, const glm::vec3& outward_normal) {
        front_face = glm::dot(r.direction(), outward_normal) < 0;
        normal = front_face ? outward_normal :-outward_normal;
    }
};

class Hitable {
public:
    virtual bool hit(RTContext &rtx, const Ray &r, float t_min, float t_max, HitRecord &rec) const = 0;
    virtual bool bounding_box(double time0, double time1, AABB& output_box) const = 0;
};

} // namespace rt
