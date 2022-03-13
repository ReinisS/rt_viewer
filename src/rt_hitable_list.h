#pragma once

#include "rt_raytracing.h"
#include "rt_hitable.h"
#include "rt_weekend.h"
#include "rt_ray.h"

#include <memory>
#include <vector>

using std::shared_ptr;
using std::make_shared;

namespace rt {

class HitableList : public Hitable {
    public:
        HitableList() {}
        HitableList(shared_ptr<Hitable> object) { add(object); }

        void clear() { objects.clear(); }
        void add(shared_ptr<Hitable> object) { objects.push_back(object); }

        virtual bool hit(
            RTContext &rtx, const Ray& r, float t_min, float t_max, HitRecord& rec) const override;
        
        virtual bool bounding_box(
            double time0, double time1, AABB& output_box) const override;

    public:
        std::vector<shared_ptr<Hitable>> objects;
};

bool HitableList::hit(RTContext &rtx, const Ray& r, float t_min, float t_max, HitRecord& rec) const {
    HitRecord temp_rec;
    bool hit_anything = false;
    auto closest_so_far = t_max;

    for (const auto& object : objects) {
        if (object->hit(rtx, r, t_min, closest_so_far, temp_rec)) {
            hit_anything = true;
            closest_so_far = temp_rec.t;
            rec = temp_rec;
        }
    }

    return hit_anything;
}

bool HitableList::bounding_box(double time0, double time1, AABB& output_box) const {
    if (objects.empty()) return false;

    AABB temp_box;
    bool first_box = true;

    for (const auto& object : objects) {
        if (!object->bounding_box(time0, time1, temp_box)) return false;
        output_box = first_box ? temp_box : surrounding_box(output_box, temp_box);
        first_box = false;
    }

    return true;
}

} // namespace rt
