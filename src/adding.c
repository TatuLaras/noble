#include "adding.h"

void adding_asset_instantiate(EntityAddingState *state, Scene *scene,
                              Settings *settings, Ray ray) {
    state->adding = 1;

    Entity entity = {
        .transform = raycast_get_desired_model_transform(settings, scene, ray),
    };
    strncpy(entity.asset_identifier, settings->selected_asset,
            ARRAY_LENGTH(entity.asset_identifier));

    scene_add(scene, entity, &state->entity_id);
}

void adding_entity_update(EntityAddingState *state, Scene *scene,
                          Settings *settings, Ray ray, float rotation_angle) {
    if (!state->adding)
        return;

    LiveEntity *entity = scene_get(scene, state->entity_id);
    if (!entity)
        return;

    state->rotation_angle_y += rotation_angle;

    entity->entity.transform = MatrixMultiply(
        MatrixRotateY(state->rotation_angle_y),
        raycast_get_desired_model_transform(settings, scene, ray));
}

void adding_stop(EntityAddingState *state) {
    state->adding = 0;
}
