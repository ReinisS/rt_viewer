#pragma once

#include "rt_weekend.h"

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
};

} // namespace rt
