#include "editor.h"

#include "adding.h"
#include "asset_picker.h"
#include "lighting.h"
#include "lighting_edit.h"
#include "model_vector.h"
#include "raycast.h"
#include "scene.h"
#include "scene_file.h"
#include "selection.h"
#include "settings.h"
#include "transform.h"
#include <assert.h>
#include <raylib.h>
#include <stdio.h>

#define TRANSFORM_BASE_SENSITIVITY 0.006
#define TRANSFORM_SLOW_MODE_MULTIPLIER 0.1
#define MIN_GRID_DENSITY (1.0 / 64.0)
#define MAX_GRID_DENSITY 32
#define GIZMO_SIZE_ADJUST_SENSITIVITY 0.08

static inline void start_transform(TransformMode mode, Axis axis) {
    if (settings.lighting_edit_mode_enabled) {
        if (mode == TRANSFORM_TRANSLATE &&
            lighting_edit_state.is_light_selected) {
            lighting_edit_transform_start(axis);
            DisableCursor();
        }
        return;
    }

    Entity *selected_entity = selection_get_selected_entity();
    if (!selected_entity)
        return;

    transform_start(mode, axis, selected_entity);
    DisableCursor();
}

static inline void toggle_selected_entity_lighting(void) {
    Entity *selected_entity = selection_get_selected_entity();
    if (!selected_entity)
        return;

    ModelData *model_data =
        modelvec_get(&scene.models, selected_entity->model_handle);
    assert(model_data);

    if (!model_data->is_private_instance) {
        if (scene_entity_model_unlink(selected_entity))
            return;
    }

    model_data = modelvec_get(&scene.models, selected_entity->model_handle);
    assert(model_data);

    selected_entity->is_unlit = !selected_entity->is_unlit;
    if (selected_entity->is_unlit) {
        model_data->model.materials[0].shader = unlit_shader;
    } else {
        LightingGroup *group =
            lighting_scene_get_group(selected_entity->lighting_group_handle);
        assert(group);
        model_data->model.materials[0].shader = group->shader;
    }
}

static inline void pick_selected_entity_asset(void) {
    if (!entity_selection_state.is_entity_selected)
        return;

    Entity *entity = scene_get_entity(entity_selection_state.handle);
    assert(entity);
    settings.selected_asset[settings.current_asset_slot] = entity->asset_handle;
}

// In light edit mode this toggles the enabled status of the light.
static inline void delete_selected_object(void) {
    if (settings.lighting_edit_mode_enabled) {
        lighting_edit_selected_light_toggle_enabled();
    } else if (entity_selection_state.is_entity_selected) {
        scene_remove(entity_selection_state.handle);
        selection_deselect_all();
    }
}

void editor_stop_transform(void) {
    if (settings.lighting_edit_mode_enabled) {
        lighting_edit_transform_stop();
        EnableCursor();
        return;
    }

    Entity *selected_entity = selection_get_selected_entity();
    if (selected_entity)
        transform_stop(selected_entity);
    EnableCursor();
}

void editor_cancel_transform(void) {
    if (settings.lighting_edit_mode_enabled) {
        lighting_edit_transform_cancel();
    } else
        transform_cancel();

    EnableCursor();
}

void editor_execute_action(ShortcutAction action) {
    switch (action) {
    case ACTION_TOGGLE_FPS_CONTROLS:
        editor_set_fps_controls_enabled(!settings.fps_controls_enabled);
        break;
    case ACTION_TOGGLE_GRID:
        settings.grid_enabled = !settings.grid_enabled;
        break;
    case ACTION_TOGGLE_GIZMOS:
        settings.gizmos_enabled = !settings.gizmos_enabled;
        break;
    case ACTION_TOGGLE_QUANTIZE:
        settings.quantize_to_grid_enabled = !settings.quantize_to_grid_enabled;
        break;
    case ACTION_TOGGLE_ADDING_RAYCAST_INCLUDE_OBJECTS:
        settings.adding_raycast_include_objects =
            !settings.adding_raycast_include_objects;
        break;
    case ACTION_TOGGLE_LIGHTING_EDIT_MODE:
        if (transform_operation.mode != TRANSFORM_NONE)
            editor_cancel_transform();

        settings.lighting_edit_mode_enabled =
            !settings.lighting_edit_mode_enabled;
        break;
    case ACTION_TOGGLE_LIGHTING:
        settings.lighting_enabled = !settings.lighting_enabled;
        lighting_scene_set_enabled(settings.lighting_enabled);
        break;
    case ACTION_TOGGLE_SELECTED_ENTITY_LIGHTING:
        toggle_selected_entity_lighting();
        break;

    case ACTION_OBJECT_DELETE:
        delete_selected_object();
        break;

    case ACTION_START_PICKING_ASSET:
        asset_picker_start_search();
        break;

    case ACTION_OBJECT_START_ROTATE_X:
        start_transform(TRANSFORM_ROTATE, AXIS_X);
        break;
    case ACTION_OBJECT_START_ROTATE_Y:
        start_transform(TRANSFORM_ROTATE, AXIS_Y);
        break;
    case ACTION_OBJECT_START_ROTATE_Z:
        start_transform(TRANSFORM_ROTATE, AXIS_Z);
        break;

    case ACTION_OBJECT_START_TRANSLATE_X:
        start_transform(TRANSFORM_TRANSLATE, AXIS_X);
        break;
    case ACTION_OBJECT_START_TRANSLATE_Y:
        start_transform(TRANSFORM_TRANSLATE, AXIS_Y);
        break;
    case ACTION_OBJECT_START_TRANSLATE_Z:
        start_transform(TRANSFORM_TRANSLATE, AXIS_Z);
        break;
    case ACTION_GRID_RESET:
        settings.grid_density = 1;
        break;
    case ACTION_CHANGE_AXIS_X:
        if (transform_operation.mode != TRANSFORM_NONE)
            start_transform(transform_operation.mode, AXIS_X);
        break;
    case ACTION_CHANGE_AXIS_Y:
        if (transform_operation.mode != TRANSFORM_NONE)
            start_transform(transform_operation.mode, AXIS_Y);
        break;
    case ACTION_CHANGE_AXIS_Z:
        if (transform_operation.mode != TRANSFORM_NONE)
            start_transform(transform_operation.mode, AXIS_Z);
        break;

    case ACTION_ASSET_SLOT_1:
        settings.current_asset_slot = 0;
        break;
    case ACTION_ASSET_SLOT_2:
        settings.current_asset_slot = 1;
        break;
    case ACTION_ASSET_SLOT_3:
        settings.current_asset_slot = 2;
        break;
    case ACTION_ASSET_SLOT_4:
        settings.current_asset_slot = 3;
        break;
    case ACTION_ASSET_SLOT_5:
        settings.current_asset_slot = 4;
        break;
    case ACTION_ASSET_SLOT_6:
        settings.current_asset_slot = 5;
        break;
    case ACTION_ASSET_SLOT_7:
        settings.current_asset_slot = 6;
        break;
    case ACTION_ASSET_SLOT_8:
        settings.current_asset_slot = 7;
        break;
    case ACTION_ASSET_SLOT_9:
        settings.current_asset_slot = 8;
        break;
    case ACTION_ASSET_SLOT_10:
        settings.current_asset_slot = 9;
        break;

    case ACTION_PICK_ASSET_FROM_SELECTED_ENTITY:
        pick_selected_entity_asset();
        break;

    case ACTION_SAVE_SCENE: {
        FILE *fp = fopen(settings.scene_filepath, "w");
        scene_file_store(fp);
        fclose(fp);
    } break;

    case ACTION_NONE:
        break;
    case ACTION_TOGGLE_TILE_MODE:
        printf("Unimplemented\n");
        break;
    }
}

void editor_mouse_select_object(Ray ray) {
    ObjectRaycastResult hit_result = raycast_scene_objects(ray);
    selection_deselect_all();
    if (hit_result.hit_something)
        selection_select_entity(hit_result.entity_id);
}

void editor_instantiate_object(Ray ray, LightingGroupHandle handle) {
    selection_deselect_all();
    if (adding_asset_instantiate(ray))
        return;

    Entity *entity = scene_get_entity(entity_adding_state.entity_handle);
    lighting_group_add_entity(handle, entity);
}

void editor_transform_adjust(float amount, int slow_mode) {
    float additional_multiplier = 1;
    if (slow_mode)
        additional_multiplier = TRANSFORM_SLOW_MODE_MULTIPLIER;

    transform_operation.amount +=
        amount * TRANSFORM_BASE_SENSITIVITY * additional_multiplier;

    if (settings.lighting_edit_mode_enabled &&
        lighting_edit_state.is_light_selected) {
        light_source_update(lighting_edit_state.current_group,
                            lighting_edit_state.currently_selected_light);
    }
}

void editor_added_light_adjust(float amount, int slow_mode) {
    float additional_multiplier = 1;
    if (slow_mode)
        additional_multiplier = TRANSFORM_SLOW_MODE_MULTIPLIER;
    lighting_edit_adding_light_update(amount * TRANSFORM_BASE_SENSITIVITY *
                                      additional_multiplier);
}

void editor_adjust_grid_density(float amount) {
    if (amount > 0)
        settings.grid_density /= 2;
    else if (amount < 0)
        settings.grid_density *= 2;

    if (settings.grid_density < MIN_GRID_DENSITY)
        settings.grid_density = MIN_GRID_DENSITY;
    if (settings.grid_density > MAX_GRID_DENSITY)
        settings.grid_density = MAX_GRID_DENSITY;
}

void editor_adjust_gizmo_size(float amount) {
    settings.gizmo_size += amount * GIZMO_SIZE_ADJUST_SENSITIVITY;
    if (settings.gizmo_size < 0.1)
        settings.gizmo_size = 0.1;
}

void editor_set_fps_controls_enabled(int enabled) {
    settings.fps_controls_enabled = enabled;

    if (enabled)
        DisableCursor();
    else
        EnableCursor();
}
