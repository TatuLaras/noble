#include "editor.h"

#include "adding.h"
#include "asset_picker.h"
#include "lighting.h"
#include "lighting_edit.h"
#include "raycast.h"
#include "scene.h"
#include "scene_file.h"
#include "selection.h"
#include "settings.h"
#include "shortcuts.h"
#include "terrain.h"
#include "terrain_edit.h"
#include "transform.h"
#include <assert.h>
#include <raylib.h>
#include <raymath.h>
#include <stdio.h>

#define TRANSFORM_BASE_SENSITIVITY 0.006
#define TRANSFORM_SLOW_MODE_MULTIPLIER 0.1
#define MIN_GRID_DENSITY (1.0 / 64.0)
#define MAX_GRID_DENSITY 32
#define GIZMO_SIZE_ADJUST_SENSITIVITY 0.08
#define FPV_PLAYER_HEIGHT 1.5

static inline void start_transform(TransformMode mode, Axis axis) {
    if (settings.mode == MODE_LIGHTING) {
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

static inline void pick_selected_entity_asset(void) {
    if (!entity_selection_state.is_entity_selected)
        return;

    Entity *entity = scene_get_entity(entity_selection_state.handle);
    assert(entity);
    settings.selected_assets[settings.current_asset_slot] =
        entity->asset_handle;
}

static inline void focus_selected(Camera *camera) {
    switch (settings.mode) {
    case MODE_NORMAL: {
        ObjectRaycastResult hit_result = raycast_scene_objects(
            GetScreenToWorldRay(GetMousePosition(), *camera));

        if (hit_result.result.hit) {
            Entity *entity_under_cursor =
                scene_get_entity(hit_result.entity_id);
            assert(entity_under_cursor);
            camera->target =
                matrix_get_position(entity_under_cursor->transform);
            break;
        }
    }
        __attribute__((fallthrough));
    case MODE_TERRAIN: {
        RayCollision collision =
            terrain_raycast(GetScreenToWorldRay(GetMousePosition(), *camera));
        if (collision.hit)
            camera->target = collision.point;
    } break;
    case MODE_LIGHTING: {
        if (!lighting_edit_state.is_light_selected)
            return;
        LightSource *light = lighting_scene_get_light(
            lighting_edit_state.currently_selected_light);

        if (light)
            camera->target = light->position;
    } break;
    }
}

// In light edit mode this toggles the enabled status of the light.
static inline void delete_selected_object(void) {
    if (entity_selection_state.is_entity_selected) {
        scene_remove(entity_selection_state.handle);
        selection_deselect_all();
    }
}

static inline void delete_selected_light(void) {
    if (!lighting_edit_state.is_light_selected)
        return;

    lighting_scene_remove_light(lighting_edit_state.currently_selected_light);
}

static inline void set_asset_slot(uint8_t slot) {
    settings.current_asset_slot = slot;
}

static inline void set_mode(Mode mode) {
    if (transform_operation.mode != TRANSFORM_NONE)
        editor_cancel_transform();

    settings.mode = mode;
}

void editor_stop_transform(void) {
    if (settings.mode == MODE_LIGHTING) {
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
    if (settings.mode == MODE_LIGHTING) {
        lighting_edit_transform_cancel();
    } else
        transform_cancel();

    EnableCursor();
}

void editor_execute_action(ShortcutAction action, Camera *camera) {
    switch (action) {
    case ACTION_NONE:
        break;

    case ACTION_TOGGLE_FPS_CONTROLS:
        editor_set_fpv_controls_enabled(camera, !settings.fps_controls_enabled);
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
    case ACTION_TOGGLE_LIGHTING:
        settings.lighting_enabled = !settings.lighting_enabled;
        lighting_scene_set_enabled(settings.lighting_enabled);
        break;
    case ACTION_TOGGLE_DEBUG_INFO:
        settings.debug_info_enabled = !settings.debug_info_enabled;
        break;
    case ACTION_TOGGLE_PROPERTIES_MENU:
        settings.properties_menu_enabled = !settings.properties_menu_enabled;
        break;

    case ACTION_OBJECT_DELETE:
        delete_selected_object();
        break;
    case ACTION_LIGHT_DELETE:
        delete_selected_light();
        break;

    case ACTION_START_PICKING_ASSET:
        asset_picker_start_search(PICKER_MODE_ASSET);
        break;
    case ACTION_START_PICKING_TERRAIN_TEXTURE:
        asset_picker_start_search(PICKER_MODE_TERRAIN_TEXTURE);
        break;
    case ACTION_START_PICKING_SKYBOX:
        asset_picker_start_search(PICKER_MODE_SKYBOX);
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
        settings.grid_density = SETTINGS_DEFAULT_GRID_DENSITY;
        settings.grid_height = 0;
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

    case ACTION_TERRAIN_SLOT_1:
    case ACTION_ASSET_SLOT_1:
        set_asset_slot(0);
        break;
    case ACTION_ASSET_SLOT_2:
    case ACTION_TERRAIN_SLOT_2:
        set_asset_slot(1);
        break;
    case ACTION_ASSET_SLOT_3:
    case ACTION_TERRAIN_SLOT_3:
        set_asset_slot(2);
        break;
    case ACTION_ASSET_SLOT_4:
    case ACTION_TERRAIN_SLOT_4:
        set_asset_slot(3);
        break;
    case ACTION_ASSET_SLOT_5:
    case ACTION_TERRAIN_SLOT_5:
        set_asset_slot(4);
        break;
    case ACTION_ASSET_SLOT_6:
    case ACTION_TERRAIN_SLOT_6:
        set_asset_slot(5);
        break;
    case ACTION_ASSET_SLOT_7:
    case ACTION_TERRAIN_SLOT_7:
        set_asset_slot(6);
        break;
    case ACTION_ASSET_SLOT_8:
        set_asset_slot(7);
        break;
    case ACTION_ASSET_SLOT_9:
        set_asset_slot(8);
        break;
    case ACTION_ASSET_SLOT_10:
        set_asset_slot(9);
        break;

    case ACTION_PICK_ASSET_FROM_SELECTED_ENTITY:
        pick_selected_entity_asset();
        break;

    case ACTION_SAVE_SCENE: {
        FILE *fp = fopen(settings.scene_filepath, "w");
        scene_file_store(fp);
        fclose(fp);
    } break;

    case ACTION_CAMERA_RESET:
        camera->position = (Vector3){0.0f, 6.0f, 6.0f};
        camera->target = (Vector3){0.0f, 0.0f, 0.0f};
        break;
    case ACTION_CAMERA_FOCUS_SELECTED: {
        focus_selected(camera);
    } break;

    case ACTION_SET_MODE_NORMAL:
        set_mode(MODE_NORMAL);
        break;
    case ACTION_SET_MODE_LIGHTING:
        set_mode(MODE_LIGHTING);
        break;
    case ACTION_SET_MODE_TERRAIN:
        set_mode(MODE_TERRAIN);
        break;

    case ACTION_TOGGLE_RAYCAST_OBJECT_IGNORE:
        settings.adding_raycast_include_objects =
            !settings.adding_raycast_include_objects;
        break;

    case ACTION_TOGGLE_TILE_MODE:
        printf("Unimplemented\n");
        break;

    case ACTION_TERRAIN_TOOL_RAISE:
        terrain_edit_state.tool = TERRAIN_TOOL_RAISE;
        break;
    case ACTION_TERRAIN_TOOL_RAISE_SMOOTH:
        terrain_edit_state.tool = TERRAIN_TOOL_RAISE_SMOOTH;
        break;
    case ACTION_TERRAIN_TOOL_SET:
        terrain_edit_state.tool = TERRAIN_TOOL_SET;
        break;
    }
}

void editor_mouse_select_object(Ray ray) {
    ObjectRaycastResult hit_result = raycast_scene_objects(ray);
    selection_deselect_all();
    if (hit_result.result.hit)
        selection_select_entity(hit_result.entity_id);
}

void editor_instantiate_object(Ray ray, int copy_rotation) {
    selection_deselect_all();
    if (adding_asset_instantiate(ray, copy_rotation))
        return;

    Entity *entity = scene_get_entity(entity_adding_state.entity_handle);
    Model *model = scene_entity_get_model(entity);
    assert(model->meshCount);
    assert(model->materialCount);
    model->materials[0].shader = lighting_scene_get_base_shader();
}

void editor_transform_adjust(float amount, int slow_mode) {
    float additional_multiplier = 1;
    if (slow_mode)
        additional_multiplier = TRANSFORM_SLOW_MODE_MULTIPLIER;

    transform_operation.amount +=
        amount * TRANSFORM_BASE_SENSITIVITY * additional_multiplier;
}

void editor_added_light_adjust(float amount, int slow_mode) {
    float additional_multiplier = 1;
    if (slow_mode)
        additional_multiplier = TRANSFORM_SLOW_MODE_MULTIPLIER;
    lighting_edit_adding_light_update(amount * TRANSFORM_BASE_SENSITIVITY *
                                      additional_multiplier);
}

void editor_adjust_grid_density(float amount) {
    if (amount < 0)
        settings.grid_density /= 2;
    else if (amount > 0)
        settings.grid_density *= 2;

    if (settings.grid_density < MIN_GRID_DENSITY)
        settings.grid_density = MIN_GRID_DENSITY;
    if (settings.grid_density > MAX_GRID_DENSITY)
        settings.grid_density = MAX_GRID_DENSITY;
}

void editor_adjust_grid_height(float amount) {
    if (amount > 0)
        settings.grid_height += settings.grid_density;
    else if (amount < 0)
        settings.grid_height -= settings.grid_density;
}

void editor_adjust_gizmo_size(float amount) {
    settings.gizmo_size += amount * GIZMO_SIZE_ADJUST_SENSITIVITY;
    if (settings.gizmo_size < 0.1)
        settings.gizmo_size = 0.1;
}

void editor_set_fpv_controls_enabled(Camera *camera, int enabled) {
    settings.fps_controls_enabled = enabled;

    if (enabled) {
        Vector3 direction =
            Vector3Normalize(Vector3Subtract(camera->target, camera->position));
        direction.y = 0;
        camera->position = editor_general_scene_raycast(
            GetScreenToWorldRay(GetMousePosition(), *camera), 0);
        camera->position.y += FPV_PLAYER_HEIGHT;
        camera->target = Vector3Add(camera->position, direction);
        DisableCursor();
    } else
        EnableCursor();
}

Vector3 editor_general_scene_raycast(Ray ray, int no_quantize) {
    Vector3 targetted_position =
        raycast_ground_intersection(ray, settings.grid_height);

    if (settings.adding_raycast_include_objects) {
        ObjectRaycastResult object_result = raycast_scene_objects(ray);
        if (object_result.result.hit &&
            (Vector3DistanceSqr(ray.position, object_result.result.point) <
             Vector3DistanceSqr(ray.position, targetted_position)))
            targetted_position = object_result.result.point;

        RayCollision collision = terrain_raycast(ray);
        if (collision.hit &&
            (Vector3DistanceSqr(ray.position, collision.point) <
             Vector3DistanceSqr(ray.position, targetted_position)))
            targetted_position = collision.point;
    }

    if (!no_quantize)
        targetted_position = settings_quantize_to_grid(targetted_position, 0);

    return targetted_position;
}
