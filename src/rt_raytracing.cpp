#include "rt_raytracing.h"
#include "rt_ray.h"
#include "rt_hitable.h"
#include "rt_hitable_list.h"
#include "rt_sphere.h"
#include "rt_triangle.h"
#include "rt_box.h"
#include "rt_weekend.h"
#include "rt_material.h"
#include "rt_bvh_node.h"

#include "cg_utils2.h"  // Used for OBJ-mesh loading

namespace rt {

// Store scene (world) in a global variable for convenience
struct Scene {
    Sphere ground;
    // Replaced by generic HitableList world
    // std::vector<Sphere> spheres;
    // std::vector<Box> boxes;
    // std::vector<Triangle> mesh;
    // Box mesh_bbox;
    HitableList world;
} g_scene;

bool hit_world(RTContext &rtx, const Ray &r, float t_min, float t_max, HitRecord &rec)
{
    HitRecord temp_rec;
    bool hit_anything = false;
    float closest_so_far = t_max;

    if (g_scene.world.hit(rtx, r, t_min, closest_so_far, temp_rec)) {
        hit_anything = true;
        closest_so_far = temp_rec.t;
        rec = temp_rec;
    }

    // Replaced by generic HitableList world hit checking
    // if (g_scene.ground.hit(rtx, r, t_min, closest_so_far, temp_rec)) {
    //     hit_anything = true;
    //     closest_so_far = temp_rec.t;
    //     rec = temp_rec;
    // }
    // for (int i = 0; i < g_scene.spheres.size(); ++i) {
    //     if (g_scene.spheres[i].hit(rtx, r, t_min, closest_so_far, temp_rec)) {
    //         hit_anything = true;
    //         closest_so_far = temp_rec.t;
    //         rec = temp_rec;
    //     }
    // }
    // for (int i = 0; i < g_scene.boxes.size(); ++i) {
    //     if (g_scene.boxes[i].hit(rtx, r, t_min, closest_so_far, temp_rec)) {
    //         hit_anything = true;
    //         closest_so_far = temp_rec.t;
    //         rec = temp_rec;
    //     }
    // }
    // for (int i = 0; i < g_scene.mesh.size(); ++i) {
    //     if (g_scene.mesh[i].hit(rtx, r, t_min, closest_so_far, temp_rec)) {
    //         hit_anything = true;
    //         closest_so_far = temp_rec.t;
    //         rec = temp_rec;
    //     }
    // }
    return hit_anything;
}

// This function should be called recursively (inside the function) for
// bouncing rays when you compute the lighting for materials, like this
//
// if (hit_world(...)) {
//     ...
//     return color(rtx, r_bounce, max_bounces - 1);
// }
//
// See Chapter 7 in the "Ray Tracing in a Weekend" book
glm::vec3 color(RTContext &rtx, const Ray &r, int max_bounces)
{
    if (max_bounces < 0) return glm::vec3(0.0f);

    HitRecord rec;
    if (hit_world(rtx, r, 0.001f, 9999.0f, rec)) {  // Set min to avoid "shadow acne" (floating point approximation error)
        rec.normal = glm::normalize(rec.normal);    // Always normalise before use!
        if (rtx.show_normals) { return rec.normal * 0.5f + 0.5f; }

        // Implement lighting for materials here
        // ...
        Ray scattered;
        glm::vec3 attenuation;
        if (rec.mat_ptr->scatter(rtx, r, rec, attenuation, scattered))
            return attenuation * color(rtx, scattered, max_bounces-1);
        return glm::vec3(0.0f);
    }

    // If no hit, return sky color
    glm::vec3 unit_direction = glm::normalize(r.direction());
    float t = 0.5f * (unit_direction.y + 1.0f);
    return (1.0f - t) * rtx.ground_color + t * rtx.sky_color;
}

// Old way of adding objects to the scene
// Need to change back hit_world() and uncomment Scene container objects for it to work
// void custom_scene_old(const char *filename) {
//     auto material_ground = make_shared<Lambertian>(glm::vec3(0.2f, 0.6f, 0.2f));
    
//     auto material_center = make_shared<Lambertian>(glm::vec3(0.7f, 0.3f, 0.3f));
//     auto material_left   = make_shared<Metal>(glm::vec3(0.8f, 0.8f, 0.8f), 0.1f);
//     auto material_right  = make_shared<Metal>(glm::vec3(0.8f, 0.6f, 0.2f), 0.5f);

//     auto material_blue_metal = make_shared<Metal>(glm::vec3(0.0f, 0.0f, 1.0f), 0.1f);
//     auto material_orange_metal = make_shared<Metal>(glm::vec3(1.0f, 0.6f, 0.0f), 0.6f);
//     auto material_red_matte = make_shared<Lambertian>(glm::vec3(1.0f, 0.0f, 0.0f));

//     auto material_glass = make_shared<Dielectric>(1.5);

    // Ground sphere
    // g_scene.ground = Sphere(glm::vec3(0.0f, -1000.5f, 0.0f), 1000.0f, material_ground);

    // Other spheres
    // g_scene.spheres = {
    //     Sphere(glm::vec3(0.0f, 0.0f, 0.0f), 0.5f, material_center),
    //     Sphere(glm::vec3(1.0f, 0.0f, 0.0f), 0.5f, material_right),
    //     Sphere(glm::vec3(-1.0f, 0.0f, 0.0f), 0.5f, material_left),

    //     Sphere(glm::vec3(-1.0f, 0.0f, -1.0f), 0.5f, material_blue_metal),
    //     Sphere(glm::vec3(0.0f, 0.0f, -1.0f), 0.5f, material_orange_metal),
    //     Sphere(glm::vec3(1.0f, 0.0f, -1.0f), 0.5f, material_red_matte),
    // };

    // Boxes
    // g_scene.boxes = {
    //    Box(glm::vec3(0.0f, -0.25f, 0.0f), glm::vec3(0.25f), material_center),
    //    Box(glm::vec3(1.0f, -0.25f, 0.0f), glm::vec3(0.25f), material_right),
    //    Box(glm::vec3(-1.0f, -0.25f, 0.0f), glm::vec3(0.25f), material_left),
    // };

    // Triangle mesh
    // cg::OBJMesh mesh;
    // cg::objMeshLoad(mesh, filename);
    // g_scene.mesh.clear();
    // for (int i = 0; i < mesh.indices.size(); i += 3) {
    //    int i0 = mesh.indices[i + 0];
    //    int i1 = mesh.indices[i + 1];
    //    int i2 = mesh.indices[i + 2];
    //    glm::vec3 v0 = mesh.vertices[i0] + glm::vec3(0.0f, 0.135f, 0.0f);
    //    glm::vec3 v1 = mesh.vertices[i1] + glm::vec3(0.0f, 0.135f, 0.0f);
    //    glm::vec3 v2 = mesh.vertices[i2] + glm::vec3(0.0f, 0.135f, 0.0f);
    //    g_scene.mesh.push_back(Triangle(v0, v1, v2, material_center));
    // }
// }

HitableList custom_scene(const char *filename) {
    // New way of adding objects to g_scene.world object
    HitableList world;

    auto material_ground = make_shared<Lambertian>(glm::vec3(0.2f, 0.6f, 0.2f));
    
    auto material_center = make_shared<Lambertian>(glm::vec3(0.7f, 0.3f, 0.3f));
    auto material_left   = make_shared<Metal>(glm::vec3(0.8f, 0.8f, 0.8f), 0.1f);
    auto material_right  = make_shared<Metal>(glm::vec3(0.8f, 0.6f, 0.2f), 0.5f);

    auto material_blue_metal = make_shared<Metal>(glm::vec3(0.0f, 0.0f, 1.0f), 0.1f);
    auto material_orange_metal = make_shared<Metal>(glm::vec3(1.0f, 0.6f, 0.0f), 0.6f);
    auto material_red_matte = make_shared<Lambertian>(glm::vec3(1.0f, 0.0f, 0.0f));

    auto material_glass = make_shared<Dielectric>(1.5);

    // Ground sphere
    world.add(make_shared<Sphere>(glm::vec3(0.0f, -1000.5f, 0.0f), 1000.0f, material_ground));

    // Other spheres
    // world.add(make_shared<Sphere>(glm::vec3(0.0f, 0.0f, 0.0f), 0.5f, material_center));
    world.add(make_shared<Sphere>(glm::vec3(1.0f, 0.0f, 0.0f), 0.5f, material_right));
    world.add(make_shared<Sphere>(glm::vec3(-1.0f, 0.0f, 0.0f), 0.5f, material_glass));

    world.add(make_shared<Sphere>(glm::vec3(-1.0f, 0.0f, -1.0f), 0.5f, material_blue_metal));
    world.add(make_shared<Sphere>(glm::vec3(0.0f, 0.0f, -1.0f), 0.5f, material_orange_metal));
    world.add(make_shared<Sphere>(glm::vec3(1.0f, 0.0f, -1.0f), 0.5f, material_red_matte));

    // Triangle mesh
    cg::OBJMesh mesh;
    cg::objMeshLoad(mesh, filename);
    for (int i = 0; i < mesh.indices.size(); i += 3) {
       int i0 = mesh.indices[i + 0];
       int i1 = mesh.indices[i + 1];
       int i2 = mesh.indices[i + 2];
       glm::vec3 v0 = mesh.vertices[i0] + glm::vec3(0.0f, 0.135f, 0.0f);
       glm::vec3 v1 = mesh.vertices[i1] + glm::vec3(0.0f, 0.135f, 0.0f);
       glm::vec3 v2 = mesh.vertices[i2] + glm::vec3(0.0f, 0.135f, 0.0f);
       world.add(make_shared<Triangle>(Triangle(v0, v1, v2, material_left)));
    }

    return world;
}

HitableList semi_random_scene(const char *filename) {
    HitableList world;

    auto ground_material = make_shared<Lambertian>(glm::vec3(0.5f, 0.5f, 0.5f));
    world.add(make_shared<Sphere>(glm::vec3(0,-1000,0), 1000, ground_material));

    for (int a = -5; a < 5; a++) {
        for (int b = -5; b < 5; b++) {
            auto choose_mat = random_double();
            glm::vec3 center(a + 0.3*random_double(), 0.2, b + 0.3*random_double());

            if ((center - glm::vec3(4, 0.2, 0)).length() > 0.9) {
                shared_ptr<Material> sphere_material;

                if (choose_mat < 0.8) {
                    // diffuse
                    auto albedo = random_vec3() * random_vec3();
                    sphere_material = make_shared<Lambertian>(albedo);
                    world.add(make_shared<Sphere>(center, 0.2, sphere_material));
                } else if (choose_mat < 0.95) {
                    // metal
                    auto albedo = random_vec3(0.5, 1);
                    auto fuzz = random_double(0, 0.5);
                    sphere_material = make_shared<Metal>(albedo, fuzz);
                    world.add(make_shared<Sphere>(center, 0.2, sphere_material));
                } else {
                    // glass
                    sphere_material = make_shared<Dielectric>(1.5);
                    world.add(make_shared<Sphere>(center, 0.2, sphere_material));
                }
            }
        }
    }

    auto material1 = make_shared<Dielectric>(1.5);
    world.add(make_shared<Sphere>(glm::vec3(-1.25, 0.5, 0), 0.5, material1));

    auto material2 = make_shared<Metal>(glm::vec3(0.8, 0.8, 0.8), 0.1);
    // Triangle mesh
    cg::OBJMesh mesh;
    cg::objMeshLoad(mesh, filename);
    for (int i = 0; i < mesh.indices.size(); i += 3) {
       int i0 = mesh.indices[i + 0];
       int i1 = mesh.indices[i + 1];
       int i2 = mesh.indices[i + 2];
       glm::vec3 v0 = mesh.vertices[i0] + glm::vec3(0.0f, 0.5f, 0.0f);
       glm::vec3 v1 = mesh.vertices[i1] + glm::vec3(0.0f, 0.5f, 0.0f);
       glm::vec3 v2 = mesh.vertices[i2] + glm::vec3(0.0f, 0.5f, 0.0f);
       world.add(make_shared<Triangle>(Triangle(v0, v1, v2, material2)));
    }

    auto material3 = make_shared<Metal>(glm::vec3(0.7, 0.6, 0.5), 0.0);
    world.add(make_shared<Sphere>(glm::vec3(1.25, 0.5, 0), 0.5, material3));

    return world;
}

HitableList random_scene() {
    HitableList world;

    auto ground_material = make_shared<Lambertian>(glm::vec3(0.5f, 0.5f, 0.5f));
    world.add(make_shared<Sphere>(glm::vec3(0,-1000,0), 1000, ground_material));

    for (int a = -11; a < 11; a++) {
        for (int b = -11; b < 11; b++) {
            auto choose_mat = random_double();
            glm::vec3 center(a + 0.9*random_double(), 0.2, b + 0.9*random_double());

            if ((center - glm::vec3(4, 0.2, 0)).length() > 0.9) {
                shared_ptr<Material> sphere_material;

                if (choose_mat < 0.8) {
                    // diffuse
                    auto albedo = random_vec3() * random_vec3();
                    sphere_material = make_shared<Lambertian>(albedo);
                    world.add(make_shared<Sphere>(center, 0.2, sphere_material));
                } else if (choose_mat < 0.95) {
                    // metal
                    auto albedo = random_vec3(0.5, 1);
                    auto fuzz = random_double(0, 0.5);
                    sphere_material = make_shared<Metal>(albedo, fuzz);
                    world.add(make_shared<Sphere>(center, 0.2, sphere_material));
                } else {
                    // glass
                    sphere_material = make_shared<Dielectric>(1.5);
                    world.add(make_shared<Sphere>(center, 0.2, sphere_material));
                }
            }
        }
    }

    auto material1 = make_shared<Dielectric>(1.5);
    world.add(make_shared<Sphere>(glm::vec3(0, 1, 0), 1.0, material1));

    auto material2 = make_shared<Lambertian>(glm::vec3(0.4, 0.2, 0.1));
    world.add(make_shared<Sphere>(glm::vec3(-4, 1, 0), 1.0, material2));

    auto material3 = make_shared<Metal>(glm::vec3(0.7, 0.6, 0.5), 0.0);
    world.add(make_shared<Sphere>(glm::vec3(4, 1, 0), 1.0, material3));

    return world;
}

// MODIFY THIS FUNCTION!
void setupScene(RTContext &rtx, const char *filename)
{
    g_scene.world.clear();

    // custom_scene_old(filename);
    // HitableList world = custom_scene(filename);
    HitableList world = semi_random_scene(filename);
    // HitableList world = random_scene();

    g_scene.world = HitableList(make_shared<BvhNode>(world, 0.0, 1.0));
}

// MODIFY THIS FUNCTION!
void updateLine(RTContext &rtx, int y)
{
    int nx = rtx.width;
    int ny = rtx.height;
    float aspect = float(nx) / float(ny);

    float theta = glm::radians(rtx.vfov);
    float h = tan(theta/2.0f);
    float viewport_height = 2.0f * h;
    float viewport_width = aspect * viewport_height;

    float focal_length = 1.0f;

    glm::vec3 origin(0.0f, 0.0f, 0.0f);
    glm::vec3 horizontal(viewport_width, 0.0f, 0.0f);
    glm::vec3 vertical(0.0f, viewport_height, 0.0f);
    // glm::vec3 lower_left_corner(-1.0f * aspect, -1.0f, -1.0f);
    glm::vec3 lower_left_corner = origin - horizontal/2.0f - vertical/2.0f - glm::vec3(0, 0, focal_length);
    glm::mat4 world_from_view = glm::inverse(rtx.view);

    // You can try parallelising this loop by uncommenting this line:
    #pragma omp parallel for schedule(dynamic)
    for (int x = 0; x < nx; ++x) {
        float u, v;
        if (rtx.perform_antialiasing) {
            // Add random jitter to u, v so that we get antialiasing from averaging color values between multiple frames
            u = (float(x) + float(random_double())) / float(nx);
            v = (float(y) + float(random_double())) / float(ny);
        }
        else {
            u = (float(x) + 0.5f) / float(nx);
            v = (float(y) + 0.5f) / float(ny);
        }

        Ray r(origin, lower_left_corner + u * horizontal + v * vertical);
        r.A = glm::vec3(world_from_view * glm::vec4(r.A, 1.0f));
        r.B = glm::vec3(world_from_view * glm::vec4(r.B, 0.0f));

        // Note: in the RTOW book, they have an inner loop for the number of
        // samples per pixel. Here, you do not need this loop, because we want
        // some interactivity and accumulate samples over multiple frames
        // instead (until the camera moves or the rendering is reset).

        if (rtx.current_frame <= 0) {
            // Here we make the first frame blend with the old image,
            // to smoothen the transition when resetting the accumulation
            glm::vec4 old = rtx.image[y * nx + x];
            rtx.image[y * nx + x] = glm::clamp(old / glm::max(1.0f, old.a), 0.0f, 1.0f);
        }
        glm::vec3 c = color(rtx, r, rtx.max_bounces);
        rtx.image[y * nx + x] += glm::vec4(c, 1.0f);
    }
}

void updateImage(RTContext &rtx)
{
    if (rtx.freeze) return;                    // Skip update
    rtx.image.resize(rtx.width * rtx.height);  // Just in case...

    updateLine(rtx, rtx.current_line % rtx.height);

    if (rtx.current_frame < rtx.max_frames) {
        rtx.current_line += 1;
        if (rtx.current_line >= rtx.height) {
            rtx.current_frame += 1;
            rtx.current_line = rtx.current_line % rtx.height;
        }
    }
}

void resetImage(RTContext &rtx)
{
    rtx.image.clear();
    rtx.image.resize(rtx.width * rtx.height);
    rtx.current_frame = 0;
    rtx.current_line = 0;
    rtx.freeze = false;
}

void resetAccumulation(RTContext &rtx)
{
    rtx.current_frame = -1;
}

}  // namespace rt
