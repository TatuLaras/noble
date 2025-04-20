#include "raycast.h"
#include "common.h"
#include "game_interface.h"
#include "scene.h"
#include <raymath.h>
#include <stddef.h>

static inline Vector3 ray_ground_intersection(Ray ray) {
    float t = (-1 * ray.position.y) / ray.direction.y;
    return Vector3Add(ray.position, Vector3Scale(ray.direction, t));
}

Matrix raycast_get_desired_model_transform(Settings *settings, Scene *scene,
                                           Ray ray) {
    Vector3 targetted_position = ray_ground_intersection(ray);

    if (settings->adding_raycast_include_objects) {
        ObjectRaycastResult result = raycast_scene_objects(ray, scene);
        if (result.hit_something)
            targetted_position = result.point;
    }

    if (settings->quantize_to_grid_enabled) {
        targetted_position.x =
            quantize(targetted_position.x, settings->grid_density);
        targetted_position.y =
            quantize(targetted_position.y, settings->grid_density);
        targetted_position.z =
            quantize(targetted_position.z, settings->grid_density);
    }

    Matrix translation = MatrixTranslate(
        targetted_position.x, targetted_position.y, targetted_position.z);

    return translation;
}

//  TODO: layers, filters
ObjectRaycastResult raycast_scene_objects(Ray ray, Scene *scene) {
    ObjectRaycastResult result = {0};
    RayCollision collision = {0};
    LiveEntity *live_entity = {0};
    size_t i = 0;
    while ((live_entity = scene_get(scene, i++))) {
        if (live_entity->is_destroyed)
            continue;

        Model *model = modelvec_get(&scene->models, live_entity->model_index);

        collision = GetRayCollisionMesh(ray, model->meshes[0],
                                        live_entity->entity.transform);

        if (collision.hit) {
            int is_closer_than_previous =
                !result.hit_something ||
                (Vector3DistanceSqr(ray.position, collision.point) <
                 Vector3DistanceSqr(ray.position, result.point));

            if (is_closer_than_previous)
                result = (ObjectRaycastResult){
                    .hit_something = 1,
                    .entity_id = i - 1,
                    .point = collision.point,
                };
        }
    }

    return result;
}
