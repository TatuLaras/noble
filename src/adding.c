#include "adding.h"

#include "editor.h"
#include "selection.h"
#include "settings.h"

EntityAddingState entity_adding_state = {0};

static Matrix get_preserved_base_transform(void) {
    Entity *entity = scene_get_entity(entity_adding_state.entity_handle);
    if (!entity)
        return MatrixIdentity();

    return matrix_strip_position(entity->transform);
}

//  TODO: Vector3 position as parameter instead.
int adding_asset_instantiate(Ray ray, int copy_rotation) {
    entity_adding_state.adding = 1;

    Matrix transform = MatrixIdentity();
    if (copy_rotation)
        transform = get_preserved_base_transform();

    Vector3 entity_pos = editor_general_scene_raycast(ray, 0);
    Entity entity = {
        .transform = MatrixMultiply(
            transform,
            MatrixTranslate(entity_pos.x, entity_pos.y, entity_pos.z)),
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

    Vector3 entity_pos = editor_general_scene_raycast(ray, 0);
    matrix_set_position(&entity->transform, entity_pos);

    entity->transform =
        MatrixMultiply(MatrixRotateY(rotation_angle), entity->transform);
}

void adding_stop(void) {
    Entity *entity = scene_get_entity(entity_adding_state.entity_handle);
    if (entity)
        entity->ignore_raycast = 0;
    entity_adding_state.adding = 0;
    selection_select_entity(entity_adding_state.entity_handle);
}
