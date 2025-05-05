#include "game_interface.h"

#include "adding.h"
#include "asset_picker.h"
#include "assets.h"
#include "common.h"
#include "editor.h"
#include "gizmos.h"
#include "lighting.h"
#include "lighting_edit.h"
#include "model_vector.h"
#include "raymath.h"
#include "rlgl.h"
#include "scene.h"
#include "scene_file.h"
#include "selection.h"
#include "settings.h"
#include "shortcuts.h"
#include "skyboxes.h"
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
static RenderTexture scene_render_target = {0};

static void render(void) {
    if (!scene_render_target.id || IsWindowResized()) {
        if (scene_render_target.id)
            UnloadRenderTexture(scene_render_target);
        scene_render_target =
            LoadRenderTexture(GetScreenWidth(), GetScreenHeight());
        printf("Reload render texture\n");
    }

    BeginTextureMode(scene_render_target);
    ClearBackground((Color){0});

    if (settings.lighting_edit_mode_enabled) {
        LightingGroup *group =
            lighting_scene_get_group(lighting_edit_state.current_group);
        assert(group);
        ClearBackground(group->ambient_color);
    }

    if (shortcuts_waiting_for_keypress())
        ClearBackground((Color){.r = 0x5d, .g = 0x52, .b = 0x52});

    BeginMode3D(camera);

    size_t i = 0;
    Entity *entity = {0};

    while ((entity = scene_get_entity(i++))) {
        if (entity->is_destroyed)
            continue;

        ModelData *model_data = scene_entity_get_model(entity);

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

    if (settings.grid_enabled) {
        Vector3 camera_pos = vector3_quantize(camera.position);
        gizmos_draw_grid(80 / settings.grid_density, settings.grid_density,
                         (Vector3){camera_pos.x, settings.grid_height + 0.002,
                                   camera_pos.z});
    }

    EndMode3D();
    EndTextureMode();

    // Actual draw loop
    BeginDrawing();
    ClearBackground(BLACK);

    // Skybox
    scene_render_skybox(camera);

    // 3D scene
    DrawTextureRec(scene_render_target.texture,
                   (Rectangle){0, 0, (float)scene_render_target.texture.width,
                               (float)-scene_render_target.texture.height},
                   (Vector2){0, 0}, WHITE);

    if (settings.lighting_edit_mode_enabled && settings.gizmos_enabled)
        gizmos_render_light_gizmos(lighting_group_handle, camera);

    if (settings.properties_menu_enabled) {
        lighting_edit_render_properties_menu();
        scene_render_properties_menu();
    }

    if (settings.debug_info_enabled)
        DrawFPS(0, 0);

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
        if (IsKeyDown(KEY_LEFT_SHIFT))
            editor_adjust_grid_height(scroll);
        else
            editor_adjust_grid_density(scroll);
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

static inline void load_scene(void) {
    FILE *fp = fopen(settings.scene_filepath, "r");
    if (!fp) {
        perror("WARNING: Could not open scene file");
        return;
    }
    if (scene_file_load(fp)) {
        printf("WARNING: Did not load scene from file, either it doesn't exist "
               "or is in the wrong format.\n");
    }
    fclose(fp);
}

int game_init(char *scene_filepath) {
    settings.scene_filepath = scene_filepath;

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(800, 450, "Noble");
    SetTargetFPS(60);

    ui_init();
    scene_init();
    lighting_scene_init();

    assets_fetch_all();
    skyboxes_fetch_all();

    lighting_group_handle =
        lighting_group_create((Color){0x21, 0x0d, 0x1f, 255});
    lighting_edit_state.current_group = lighting_group_handle;

    lighting_scene_set_enabled(settings.lighting_enabled);

    camera.position = (Vector3){0.0f, 6.0f, 6.0f};
    camera.target = (Vector3){0.0f, 0.0f, 0.0f};
    camera.up = (Vector3){0.0f, 1.0f, 0.0f};
    camera.fovy = 45.0f;

    load_scene();
    scene_load_skybox();
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
