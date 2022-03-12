#pragma once

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "glm/ext.hpp"

#include <cstdlib>

namespace rt {

inline double random_double() {
    // Returns a random real in [0,1).
    return rand() / (RAND_MAX + 1.0);
}

inline double random_double(double min, double max) {
    // Returns a random real in [min,max).
    return min + (max-min)*random_double();
}

inline glm::vec3 random_vec3() {
    return glm::vec3(float(random_double()), float(random_double()), float(random_double()));
}

inline static glm::vec3 random_vec3(double min, double max) {
    return glm::vec3(float(random_double(min,max)), float(random_double(min,max)), float(random_double(min,max)));
}

// Rejection method for generating a random vec3 in a unit radius sphere
glm::vec3 random_in_unit_sphere() {
    while (true) {
        auto p = random_vec3(-1,1);
        if (glm::length2(p) >= 1) continue;
        return p;
    }
}

// Normalize to get True Lambertian Reflection
glm::vec3 random_unit_vector() {
    return glm::normalize(random_in_unit_sphere());
}

}  // namespace rt
