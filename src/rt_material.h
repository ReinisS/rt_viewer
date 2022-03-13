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

// Matte material
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

// Reflective/glossy material
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

// Glass-like material
class Dielectric : public Material {
    public:
        Dielectric(float index_of_refraction) : ir(index_of_refraction) {}

        virtual bool scatter(
            RTContext &rtx, const Ray& r_in, const HitRecord& rec, glm::vec3& attenuation, Ray& scattered
        ) const override {
            attenuation = glm::vec3(1.0f, 1.0f, 1.0f);
            float refraction_ratio = rec.front_face ? (1.0/ir) : ir;

            glm::vec3 unit_direction = glm::normalize(r_in.direction());
            double cos_theta = fmin(glm::dot(-unit_direction, rec.normal), 1.0);
            double sin_theta = sqrt(1.0 - cos_theta*cos_theta);

            bool cannot_refract = refraction_ratio * sin_theta > 1.0;
            glm::vec3 direction;

            if (cannot_refract || reflectance(cos_theta, refraction_ratio) > random_double())
                direction = glm::reflect(unit_direction, rec.normal);
            else
                direction = glm::refract(unit_direction, rec.normal, refraction_ratio);

            scattered = Ray(rec.p, direction);
            return true;
        }

    public:
        float ir; // Index of Refraction

    private:
        static double reflectance(double cosine, double ref_idx) {
            // Use Schlick's approximation for reflectance.
            auto r0 = (1-ref_idx) / (1+ref_idx);
            r0 = r0*r0;
            return r0 + (1-r0)*pow((1 - cosine),5);
        }
};

}  // namespace rt
