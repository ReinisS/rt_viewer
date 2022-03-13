#pragma once

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "glm/ext.hpp"

#include <cstdlib>
#include <memory>
#include <iostream>

using std::shared_ptr;
using std::make_shared;

namespace rt {

// Returns a random real in [0,1).
inline double random_double() {
    return rand() / (RAND_MAX + 1.0);
}

// Returns a random real in [min,max).
inline double random_double(double min, double max) {
    return min + (max-min)*random_double();
}

// Returns a random glm::vec3
inline glm::vec3 random_vec3() {
    return glm::vec3(float(random_double()), float(random_double()), float(random_double()));
}

// Returns a random glm::vec3 where each component is in [min,max)
inline static glm::vec3 random_vec3(double min, double max) {
    return glm::vec3(float(random_double(min,max)), float(random_double(min,max)), float(random_double(min,max)));
}

// Returns a random glm::vec3 in a unit radius sphere using rejection method
glm::vec3 random_in_unit_sphere() {
    while (true) {
        auto p = random_vec3(-1,1);
        if (glm::length2(p) >= 1) continue;
        return p;
    }
}

// Returns a normalized random glm::vec3 in a unit radius sphere using rejection method (used for True Lambertian Reflection)
glm::vec3 random_unit_vector() {
    return glm::normalize(random_in_unit_sphere());
}

// Returns a random glm::vec3 in a unit hemisphere
glm::vec3 random_in_hemisphere(const glm::vec3& normal) {
    glm::vec3 in_unit_sphere = random_in_unit_sphere();
    if (glm::dot(in_unit_sphere, normal) > 0.0) // In the same hemisphere as the normal
        return in_unit_sphere;
    else
        return -in_unit_sphere;
}

// Return true if the vector is close to zero in all dimensions.
bool near_zero_vec3(glm::vec3 e) {
    const auto s = 1e-8;
    return (fabs(e[0]) < s) && (fabs(e[1]) < s) && (fabs(e[2]) < s);
}

// Returns a random integer in [min,max].
inline int random_int(int min, int max) {
    return static_cast<int>(random_double(min, max+1));
}

}  // namespace rt
