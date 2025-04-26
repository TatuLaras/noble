#include "game_interface.h"

#include "adding.h"
#include "asset_picker.h"
#include "assets.h"
#include "editor.h"
#include "gizmos.h"
#include "lighting.h"
#include "lighting_edit.h"
#include "model_vector.h"
#include "raymath.h"
#include "scene.h"
#include "scene_file.h"
#include "selection.h"
#include "settings.h"
#include "shortcuts.h"
#include "transform.h"
#include "ui.h"
#include <assert.h>
#include <raylib.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define VERTICAL_MOVEMENT_SPEED 3.0

static LightingGroupHandle lighting_group_handle;

static Camera3D camera = {0};

static void render(void) {
    BeginDrawing();

    ClearBackground((Color){.r = 0x32, .g = 0x15, .b = 0x15});

    if (settings.lighting_edit_mode_enabled)
        ClearBackground(BLACK);

    if (shortcuts_waiting_for_keypress())
        ClearBackground((Color){.r = 0x5d, .g = 0x52, .b = 0x52});

    BeginMode3D(camera);

    size_t i = 0;
    Entity *entity = {0};

    while ((entity = scene_get_entity(i++))) {
        if (entity->is_destroyed)
            continue;

        ModelData *model_data =
            modelvec_get(&scene.models, entity->model_handle);

        Matrix transform = entity->transform;

        int is_selected = (entity_selection_state.is_entity_selected &&
                           entity_selection_state.handle == i - 1);
        int is_current_entity_being_transformed =
            is_selected && transform_operation.mode != TRANSFORM_NONE &&
            !settings.lighting_edit_mode_enabled;

        if (is_current_entity_being_transformed) {
            Matrix preview_transform = transform_get_matrix();
            transform = MatrixMultiply(preview_transform, entity->transform);
        }

        DrawMesh(model_data->model.meshes[0], model_data->model.materials[0],
                 transform);

        int is_being_added = entity_adding_state.adding &&
                             entity_adding_state.entity_handle == i - 1;

        if (settings.gizmos_enabled && !settings.lighting_edit_mode_enabled &&
            (is_selected || is_being_added))
            gizmos_render_transform_gizmo(transform);
    }

    if (settings.grid_enabled)
        DrawGrid(100 / settings.grid_density, settings.grid_density);

    EndMode3D();

    if (settings.lighting_edit_mode_enabled && settings.gizmos_enabled)
        gizmos_render_light_gizmos(lighting_group_handle, camera);

    lighting_edit_render_properties_menu();

    // Draw properties menu bounds
    Rectangle properties = ui_properties_menu_get_rect();
    DrawRectangleLines(properties.x, properties.y, properties.width,
                       properties.height, (Color){0xda, 0x57, 0x57, 0xff});

    ui_render(GetScreenWidth(), GetScreenHeight());

    EndDrawing();
}

static inline int mouse_inside_properties_menu(void) {
    Rectangle rect = ui_properties_menu_get_rect();
    Vector2 mouse = GetMousePosition();

    if (mouse.x >= rect.x && mouse.x <= rect.x + rect.width &&
        mouse.y >= rect.y && mouse.y <= rect.y + rect.height)
        return 1;
    return 0;
}

static inline int mouse_button_pressed(MouseButton button) {
    return IsMouseButtonPressed(button) && !mouse_inside_properties_menu();
}
static inline int mouse_button_down(MouseButton button) {
    return IsMouseButtonDown(button) && !mouse_inside_properties_menu();
}
static inline int mouse_button_released(MouseButton button) {
    return IsMouseButtonReleased(button) && !mouse_inside_properties_menu();
}

static inline void handle_inputs(void) {

    // Asset picker UI captures all inputs when active
    if (asset_picker.picking_asset) {
        // Select asset
        if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
            asset_picker_select_current_option();
            entity_adding_state.rotation_angle_y = 0;
            return;
        }

        asset_picker_input_key(GetKeyPressed(), IsKeyDown(KEY_LEFT_CONTROL));
        return;
    }

    Vector2 mouse_delta = GetMouseDelta();
    float scroll = GetMouseWheelMove();

    // FPS style movement
    //  TODO: some easier controls as well as an option
    if (settings.fps_controls_enabled) {
        if (IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_CAPS_LOCK) ||
            IsMouseButtonPressed(MOUSE_BUTTON_LEFT) ||
            IsMouseButtonPressed(MOUSE_BUTTON_RIGHT))
            editor_set_fps_controls_enabled(0);

        float delta_time = GetFrameTime();
        float vertical_movement = 0;

        if (IsKeyDown(KEY_SPACE))
            vertical_movement = delta_time * VERTICAL_MOVEMENT_SPEED;
        else if (IsKeyDown(KEY_LEFT_SHIFT))
            vertical_movement = delta_time * VERTICAL_MOVEMENT_SPEED * -1;

        camera.position.y += vertical_movement;
        camera.target.y += vertical_movement;

        UpdateCamera(&camera, CAMERA_FIRST_PERSON);
        return;
    }

    if (IsKeyDown(KEY_V)) {
        editor_adjust_gizmo_size(scroll);
    }

    if (IsKeyDown(KEY_C)) {
        editor_adjust_grid_density(scroll);
        return;
    }

    // Shortcuts
    ShortcutAction action = shortcuts_get_action(
        GetKeyPressed(), IsKeyDown(KEY_LEFT_SHIFT), IsKeyDown(KEY_LEFT_CONTROL),
        IsKeyDown(KEY_LEFT_ALT));
    editor_execute_action(action);

    if (transform_operation.mode != TRANSFORM_NONE) {

        if (mouse_button_pressed(MOUSE_BUTTON_LEFT))
            editor_stop_transform();
        if (mouse_button_pressed(MOUSE_BUTTON_RIGHT))
            editor_cancel_transform();

        editor_transform_adjust(mouse_delta.x, IsKeyDown(KEY_LEFT_SHIFT));
        return;
    }

    // Lighting edit
    if (settings.lighting_edit_mode_enabled) {
        if (mouse_button_pressed(MOUSE_BUTTON_RIGHT)) {
            lighting_edit_select_light_at(GetMousePosition(), camera);
        }

        if (mouse_button_pressed(MOUSE_BUTTON_LEFT)) {
            Ray ray = GetScreenToWorldRay(GetMousePosition(), camera);
            lighting_edit_add_light(ray);
            DisableCursor();
        }

        if (mouse_button_down(MOUSE_BUTTON_LEFT) &&
            lighting_edit_state.is_light_added) {
            editor_added_light_adjust(mouse_delta.x, IsKeyDown(KEY_LEFT_SHIFT));
        }

        if (mouse_button_released(MOUSE_BUTTON_LEFT) &&
            lighting_edit_state.is_light_added) {
            lighting_edit_adding_stop();
            EnableCursor();
        }

        return;
    } else if (lighting_edit_state.is_light_added) {
        lighting_edit_adding_stop();
        EnableCursor();
    }

    // Object instantiation
    if (mouse_button_pressed(MOUSE_BUTTON_LEFT)) {
        Ray ray = GetScreenToWorldRay(GetMousePosition(), camera);
        editor_instantiate_object(ray, lighting_group_handle);
        return;
    }

    // Update added object while mouse down
    if (mouse_button_down(MOUSE_BUTTON_LEFT)) {
        float rotate_by_angle = scroll * ROTATION_SNAP_INCREMENT;
        Ray ray = GetScreenToWorldRay(GetMousePosition(), camera);
        adding_entity_update(ray, rotate_by_angle);
        return;
    }

    // Stop updating added object
    if (mouse_button_released(MOUSE_BUTTON_LEFT) &&
        entity_adding_state.adding) {
        adding_stop();
        return;
    }

    // Object seletion
    if (mouse_button_pressed(MOUSE_BUTTON_RIGHT)) {
        Ray ray = GetScreenToWorldRay(GetMousePosition(), camera);
        editor_mouse_select_object(ray);
        return;
    }
}

int game_init(char *scene_filepath) {
    settings.scene_filepath = scene_filepath;
    FILE *fp = fopen(scene_filepath, "r");
    if (scene_file_load(fp)) {
        fclose(fp);
        return 1;
    }
    fclose(fp);

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(800, 450, "Noble");
    SetTargetFPS(60);

    ui_init();
    assets_fetch_all();

    scene_init();
    lighting_scene_init();

    lighting_group_handle =
        lighting_group_create((Color){0x21, 0x0d, 0x1f, 255});
    lighting_edit_state.current_group = lighting_group_handle;

    lighting_scene_set_enabled(settings.lighting_enabled);

    camera.position = (Vector3){0.0f, 6.0f, 6.0f};
    camera.target = (Vector3){0.0f, 0.0f, 0.0f};
    camera.up = (Vector3){0.0f, 1.0f, 0.0f};
    camera.fovy = 45.0f;

    return 0;
}

void game_main(void) {
    while (!WindowShouldClose()) {
        if (lighting_edit_state.is_light_selected &&
            settings.lighting_edit_mode_enabled)
            light_source_update(lighting_edit_state.current_group,
                                lighting_edit_state.currently_selected_light);

        render();
        handle_inputs();
        ui_properties_menu_reset();
    }
}

void game_deinit(void) {
    scene_free();
    CloseWindow();
}
