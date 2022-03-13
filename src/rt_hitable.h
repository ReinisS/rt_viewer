#pragma once

#include "rt_weekend.h"
#include "rt_aabb.h"

namespace rt {

class Material;

struct HitRecord {
    float t;
    glm::vec3 p;
    glm::vec3 normal;
    shared_ptr<Material> mat_ptr;
};

class Hitable {
public:
    virtual bool hit(RTContext &rtx, const Ray &r, float t_min, float t_max, HitRecord &rec) const = 0;
    virtual bool bounding_box(double time0, double time1, AABB& output_box) const = 0;
};

} // namespace rt
