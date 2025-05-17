#include "adding.h"

#include "raycast.h"
#include "selection.h"
#include "settings.h"

EntityAddingState entity_adding_state = {0};

Matrix adding_get_desired_model_transform(Ray ray) {
    Vector3 targetted_position =
        raycast_ground_intersection(ray, settings.grid_height);

    if (settings.adding_raycast_include_objects) {
        ObjectRaycastResult object_result = raycast_scene_objects(ray);
        if (object_result.result.hit &&
            (Vector3DistanceSqr(ray.position, object_result.result.point) <
             Vector3DistanceSqr(ray.position, targetted_position)))
            targetted_position = object_result.result.point;
    }

    targetted_position = settings_quantize_to_grid(targetted_position, 0);

    Matrix translation = MatrixTranslate(
        targetted_position.x, targetted_position.y, targetted_position.z);

    return translation;
}

int adding_asset_instantiate(Ray ray) {
    entity_adding_state.adding = 1;

    Entity entity = {
        .transform = adding_get_desired_model_transform(ray),
        .ignore_raycast = 1,
        .asset_handle = settings.selected_assets[settings.current_asset_slot],
    };

    if (scene_add(entity, &entity_adding_state.entity_handle,
                  settings.asset_directory))
        return 1;

    return 0;
}

void adding_entity_update(Ray ray, float rotation_angle) {
    if (!entity_adding_state.adding)
        return;

    Entity *entity = scene_get_entity(entity_adding_state.entity_handle);
    if (!entity)
        return;

    entity_adding_state.rotation_angle_y += rotation_angle;

    entity->transform =
        MatrixMultiply(MatrixRotateY(entity_adding_state.rotation_angle_y),
                       adding_get_desired_model_transform(ray));
}

void adding_stop(void) {
    Entity *entity = scene_get_entity(entity_adding_state.entity_handle);
    if (entity)
        entity->ignore_raycast = 0;
    entity_adding_state.adding = 0;
    selection_select_entity(entity_adding_state.entity_handle);
}
