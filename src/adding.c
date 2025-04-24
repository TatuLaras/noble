#include "adding.h"

#include "common.h"
#include "raycast.h"
#include "selection.h"
#include "settings.h"

EntityAddingState entity_adding_state = {0};

int adding_asset_instantiate(Ray ray) {
    entity_adding_state.adding = 1;

    Entity entity = {
        .transform = raycast_get_desired_model_transform(ray),
        .ignore_raycast = 1,
    };
    strncpy(entity.asset_identifier, settings.selected_asset,
            ARRAY_LENGTH(entity.asset_identifier));

    if (scene_add(entity, &entity_adding_state.entity_handle))
        return 1;

    return 0;
}

void adding_entity_update(Ray ray, float rotation_angle) {
    if (!entity_adding_state.adding)
        return;

    LiveEntity *entity = scene_get_entity(entity_adding_state.entity_handle);
    if (!entity)
        return;

    entity_adding_state.rotation_angle_y += rotation_angle;

    entity->entity.transform =
        MatrixMultiply(MatrixRotateY(entity_adding_state.rotation_angle_y),
                       raycast_get_desired_model_transform(ray));
}

void adding_stop(void) {
    LiveEntity *entity = scene_get_entity(entity_adding_state.entity_handle);
    if (entity)
        entity->entity.ignore_raycast = 0;
    entity_adding_state.adding = 0;
    selection_select_entity(entity_adding_state.entity_handle);
}
