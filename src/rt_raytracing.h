#pragma once

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

namespace rt {

struct RTContext {
    int width = 500;
    int height = 500;
    std::vector<glm::vec4> image;
    bool freeze = false;
    int current_frame = 0;
    int current_line = 0;
    int max_frames = 1000;
    int max_bounces = 3;
    float epsilon = 2e-4f;
    glm::mat4 view = glm::mat4(1.0f);
    glm::vec3 ground_color = glm::vec3(0.5f, 0.5f, 0.5f);
    glm::vec3 sky_color = glm::vec3(0.5f, 0.7f, 1.0f);
    bool show_normals = false;
    bool perform_antialiasing = true;
    bool perform_gamma_correction = true;
    int diffuse_method = 2; // 0 - Random in unit sphere, 1 - Normalized random in unit sphere, 2 - Random in unit hemisphere
    float vfov = 90.0f;     // Vertical field-of-view in degrees
    float vfov_step = 1.0f;
    // Add more settings and parameters here
    // ...
};

void setupScene(RTContext &rtx, const char *mesh_filename);
void updateImage(RTContext &rtx);
void resetImage(RTContext &rtx);
void resetAccumulation(RTContext &rtx);

}  // namespace rt
