#include "raycast.h"

#include "common.h"
#include "scene.h"
#include "settings.h"
#include <raymath.h>
#include <stddef.h>

Vector3 raycast_ground_intersection(Ray ray) {
    float t = (-1 * ray.position.y) / ray.direction.y;
    return Vector3Add(ray.position, Vector3Scale(ray.direction, t));
}

Matrix raycast_get_desired_model_transform(Ray ray) {
    Vector3 targetted_position = raycast_ground_intersection(ray);

    if (settings.adding_raycast_include_objects) {
        ObjectRaycastResult result = raycast_scene_objects(ray);
        if (result.hit_something)
            targetted_position = result.point;
    }

    targetted_position = vector3_quantize(targetted_position);

    Matrix translation = MatrixTranslate(
        targetted_position.x, targetted_position.y, targetted_position.z);

    return translation;
}

ObjectRaycastResult raycast_scene_objects(Ray ray) {
    ObjectRaycastResult result = {0};
    RayCollision collision = {0};
    Entity *entity = {0};
    size_t i = 0;
    while ((entity = scene_get_entity(i++))) {
        if (entity->is_destroyed)
            continue;

        ModelData *model_data =
            modelvec_get(&scene.models, entity->model_handle);

        collision = GetRayCollisionMesh(ray, model_data->model.meshes[0],
                                        entity->transform);

        if (collision.hit && !entity->ignore_raycast) {
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
