#include "adding.h"

#include "raycast.h"
#include "selection.h"
#include "settings.h"

EntityAddingState entity_adding_state = {0};

int adding_asset_instantiate(Ray ray) {
    entity_adding_state.adding = 1;

    Entity entity = {
        .transform = raycast_get_desired_model_transform(ray),
        .ignore_raycast = 1,
        .asset_handle = settings.selected_asset[settings.current_asset_slot],
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
                       raycast_get_desired_model_transform(ray));
}

void adding_stop(void) {
    Entity *entity = scene_get_entity(entity_adding_state.entity_handle);
    if (entity)
        entity->ignore_raycast = 0;
    entity_adding_state.adding = 0;
    selection_select_entity(entity_adding_state.entity_handle);
}
