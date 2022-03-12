#pragma once

#include "rt_ray.h"
#include "rt_weekend.h"

namespace rt {

struct HitRecord;

class Material {
    public:
        virtual bool scatter(
            RTContext &rtx, const Ray& r_in, const HitRecord& rec, glm::vec3& attenuation, Ray& scattered
        ) const = 0;
};

class Lambertian : public Material {
    public:
        Lambertian(const glm::vec3& a) : albedo(a) {}

        virtual bool scatter(
            RTContext &rtx, const Ray& r_in, const HitRecord& rec, glm::vec3& attenuation, Ray& scattered
        ) const override {
            // Get random diffuse based on selected method
            glm::vec3 random_diffuse;
            if (rtx.diffuse_method == 0) {
                random_diffuse = random_in_unit_sphere();
            }
            else if (rtx.diffuse_method == 1) {
                random_diffuse = random_unit_vector();
            }
            else if (rtx.diffuse_method == 2) {
                random_diffuse = random_in_hemisphere(rec.normal);
            }
            else {
                random_diffuse = glm::vec3(0.0f);
            }

            glm::vec3 target = rec.p + rec.normal + random_diffuse;
            auto scatter_direction = rec.normal + random_diffuse;

            // Catch degenerate scatter direction
            if (near_zero_vec3(scatter_direction))
                scatter_direction = rec.normal;

            scattered = Ray(rec.p, scatter_direction);
            attenuation = albedo;
            return true;
        }

    public:
        glm::vec3 albedo;
};

class Metal : public Material {
    public:
        Metal(const glm::vec3& a, float f) : albedo(a), fuzz(f < 1 ? f : 1) {}

        virtual bool scatter(
            RTContext &rtx, const Ray& r_in, const HitRecord& rec, glm::vec3& attenuation, Ray& scattered
        ) const override {
            glm::vec3 reflected = glm::reflect(glm::normalize(r_in.direction()), rec.normal);
            scattered = Ray(rec.p, reflected + fuzz*random_in_unit_sphere());
            attenuation = albedo;
            return (glm::dot(scattered.direction(), rec.normal) > 0);
        }

    public:
        glm::vec3 albedo;
        float fuzz;
};

}  // namespace rt
