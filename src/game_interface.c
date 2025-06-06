#include "game_interface.h"

#include "adding.h"
#include "asset_picker.h"
#include "assets.h"
#include "editor.h"
#include "gizmos.h"
#include "lighting.h"
#include "lighting_edit.h"
#include "orbital_controls.h"
#include "properties_menu.h"
#include "raymath.h"
#include "rlgl.h"
#include "scene.h"
#include "scene_file.h"
#include "selection.h"
#include "settings.h"
#include "shortcuts.h"
#include "skyboxes.h"
#include "terrain.h"
#include "terrain_edit.h"
#include "terrain_textures.h"
#include "transform.h"
#include "ui.h"
#include <raylib.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define VERTICAL_MOVEMENT_SPEED 3.0

static Camera camera = {
    .position = {0.0f, 6.0f, 6.0f},
    .target = {0.0f, 0.0f, 0.0f},
    .up = {0.0f, 1.0f, 0.0f},
    .fovy = 45.0f,
};

static RenderTexture scene_render_target = {0};

static void render(void) {
    int render_gizmos =
        settings.gizmos_enabled && (!settings.fps_controls_enabled);

    if (!scene_render_target.id || IsWindowResized()) {
        if (scene_render_target.id)
            UnloadRenderTexture(scene_render_target);
        scene_render_target =
            LoadRenderTexture(GetScreenWidth(), GetScreenHeight());
        printf("Reload render texture\n");
    }

    BeginTextureMode(scene_render_target);
    ClearBackground((Color){0});

    if (shortcuts_waiting_for_keypress())
        ClearBackground((Color){.r = 0x5d, .g = 0x52, .b = 0x52});

    BeginMode3D(camera);

    size_t i = 0;
    Entity *entity = {0};

    while ((entity = scene_get_entity(i++))) {
        if (entity->is_destroyed)
            continue;

        Model *model = scene_entity_get_model(entity);

        Matrix transform = entity->transform;

        int is_selected = (entity_selection_state.is_entity_selected &&
                           entity_selection_state.handle == i - 1);
        int is_current_entity_being_transformed =
            is_selected && transform_operation.mode != TRANSFORM_NONE &&
            settings.mode == MODE_NORMAL;

        if (is_current_entity_being_transformed) {
            Matrix preview_transform = transform_get_matrix();
            transform = MatrixMultiply(preview_transform, entity->transform);
        }

        if (settings.mode == MODE_TERRAIN)
            rlEnableWireMode();

        DrawMesh(model->meshes[0], model->materials[0], transform);

        if (settings.mode == MODE_TERRAIN)
            rlDisableWireMode();

        int is_being_added = entity_adding_state.adding &&
                             entity_adding_state.entity_handle == i - 1;

        if (render_gizmos && settings.mode == MODE_NORMAL &&
            (is_selected || is_being_added))
            gizmos_render_transform_gizmo(transform);
    }

    if (settings.grid_enabled && render_gizmos) {
        Vector3 origin = settings_quantize_to_grid(camera.target, 1);
        gizmos_draw_grid(
            80 / settings.grid_density, settings.grid_density,
            (Vector3){origin.x, settings.grid_height + 0.003, origin.z});
    }

    terrain_draw();

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

    if (render_gizmos) {
        switch (settings.mode) {
        case MODE_NORMAL:
            break;
        case MODE_LIGHTING:
            gizmos_render_light_gizmos(camera);
            break;
        case MODE_TERRAIN:
            gizmos_render_terrain_gizmos(camera);
            break;
        }
    }

    if (settings.properties_menu_enabled) {
        PropertiesMenuEvent event = properties_menu_render();
        switch (event) {
        case PROPERTIES_EVENT_NONE:
            break;
        case PROPERTIES_EVENT_TERRAIN_RESIZE:
            terrain_generate_mesh();
            break;
        }
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

static inline float get_mouse_delta(void) {
    if (settings.mouse_movements_vertical)
        return -GetMouseDelta().y;
    return GetMouseDelta().x;
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

// Adjust grid with c + scrollwheel, returns 1 if grid was adjusted.
static inline int grid_adjust(void) {
    if (IsKeyDown(KEY_C)) {
        if (IsKeyDown(KEY_LEFT_SHIFT))
            editor_adjust_grid_height(GetMouseWheelMove());
        else
            editor_adjust_grid_density(GetMouseWheelMove());
        return 1;
    }
    return 0;
}

static inline void handle_inputs_normal(void) {
    float scroll = GetMouseWheelMove();

    if (IsKeyDown(KEY_V)) {
        editor_adjust_gizmo_size(scroll);
        return;
    }

    // Object instantiation
    if (mouse_button_pressed(MOUSE_BUTTON_LEFT)) {
        Ray ray = GetScreenToWorldRay(GetMousePosition(), camera);
        editor_instantiate_object(ray, IsKeyDown(KEY_LEFT_SHIFT));
        return;
    }

    // Update added object while mouse down
    if (mouse_button_down(MOUSE_BUTTON_LEFT)) {
        float rotate_by_angle = scroll * ROTATION_SNAP_INCREMENT;
        Ray ray = GetScreenToWorldRay(GetMousePosition(), camera);
        adding_entity_update(ray, rotate_by_angle);
        return;
    }

    // Scroll not used for anything else
    orbital_adjust_camera_zoom(&camera, scroll);

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

static inline void handle_inputs_lighting(void) {

    if (mouse_button_pressed(MOUSE_BUTTON_RIGHT)) {
        lighting_edit_select_light_at(GetMousePosition(), camera);
        return;
    }

    if (mouse_button_pressed(MOUSE_BUTTON_LEFT)) {
        Ray ray = GetScreenToWorldRay(GetMousePosition(), camera);
        lighting_edit_add_light(ray);
        DisableCursor();
        return;
    }

    if (mouse_button_down(MOUSE_BUTTON_LEFT) &&
        lighting_edit_state.is_light_added) {
        editor_added_light_adjust(get_mouse_delta(), IsKeyDown(KEY_LEFT_SHIFT));
        return;
    }

    if (mouse_button_released(MOUSE_BUTTON_LEFT) &&
        lighting_edit_state.is_light_added) {
        lighting_edit_adding_stop();
        EnableCursor();
        return;
    }

    orbital_adjust_camera_zoom(&camera, GetMouseWheelMove());
}

static inline void handle_inputs_terrain(void) {
    if (IsKeyPressed(KEY_LEFT_ALT)) {
        terrain_edit_state.backup_screen_pos = GetMousePosition();
        DisableCursor();
    }
    if (IsKeyReleased(KEY_LEFT_ALT))
        EnableCursor();
    if (IsKeyDown(KEY_LEFT_ALT)) {
        terrain_edit_adjust_tool_radius(get_mouse_delta());
        return;
    }

    if (mouse_button_down(MOUSE_BUTTON_LEFT)) {
        ToolMode mode = TOOL_MODE_NORMAL;
        if (IsKeyDown(KEY_LEFT_SHIFT))
            mode = TOOL_MODE_TEXTURE;

        if (terrain_edit_use_tool(GetMousePosition(), camera, mode))
            terrain_generate_mesh();
        return;
    }
    if (mouse_button_down(MOUSE_BUTTON_RIGHT)) {
        if (terrain_edit_use_tool(GetMousePosition(), camera,
                                  TOOL_MODE_ALTERNATIVE_FUNCTION))
            terrain_generate_mesh();

        return;
    }

    if (mouse_button_released(MOUSE_BUTTON_LEFT) ||
        mouse_button_released(MOUSE_BUTTON_RIGHT)) {
        terrain_edit_finish_brush_stroke();
        terrain_generate_mesh();
        return;
    }

    orbital_adjust_camera_zoom(&camera, GetMouseWheelMove());
}

static inline void handle_inputs(void) {
    // Asset picker UI captures all inputs when active
    if (asset_picker.picking_asset) {
        // Select asset
        if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)) {
            asset_picker_select_current_option();
            return;
        }

        asset_picker_input_key(GetKeyPressed(), IsKeyDown(KEY_LEFT_CONTROL));
        return;
    }

    if (IsMouseButtonPressed(MOUSE_BUTTON_MIDDLE) || IsKeyPressed(KEY_H)) {
        DisableCursor();
        terrain_edit_state.backup_screen_pos =
            (Vector2){GetScreenWidth() / 2.0, GetScreenHeight() / 2.0};
    }
    if ((IsMouseButtonReleased(MOUSE_BUTTON_MIDDLE) || IsKeyReleased(KEY_H)) &&
        transform_operation.mode == TRANSFORM_NONE)
        EnableCursor();

    if (IsMouseButtonDown(MOUSE_BUTTON_MIDDLE) || IsKeyDown(KEY_H)) {
        orbital_camera_update(&camera, settings.mouse_movements_vertical);
        return;
    }

    if (settings.fps_controls_enabled) {
        if (IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_CAPS_LOCK) ||
            IsMouseButtonPressed(MOUSE_BUTTON_LEFT) ||
            IsMouseButtonPressed(MOUSE_BUTTON_RIGHT))
            editor_set_fpv_controls_enabled(&camera, 0);

        float delta_time = GetFrameTime();
        float vertical_movement = 0;

        if (IsKeyDown(KEY_SPACE))
            vertical_movement = delta_time * VERTICAL_MOVEMENT_SPEED;
        else if (IsKeyDown(KEY_LEFT_CONTROL))
            vertical_movement = delta_time * VERTICAL_MOVEMENT_SPEED * -1;

        camera.position.y += vertical_movement;
        camera.target.y += vertical_movement;

        UpdateCamera(&camera, CAMERA_FIRST_PERSON);
        return;
    }

    // Shortcuts
    ShortcutAction action = shortcuts_get_action(
        GetKeyPressed(), IsKeyDown(KEY_LEFT_SHIFT), IsKeyDown(KEY_LEFT_CONTROL),
        IsKeyDown(KEY_LEFT_ALT));
    editor_execute_action(action, &camera);

    if (grid_adjust())
        return;

    if (transform_operation.mode != TRANSFORM_NONE) {
        if (mouse_button_pressed(MOUSE_BUTTON_LEFT))
            editor_stop_transform();
        if (mouse_button_pressed(MOUSE_BUTTON_RIGHT))
            editor_cancel_transform();

        editor_transform_adjust(get_mouse_delta(), IsKeyDown(KEY_LEFT_SHIFT));
        return;
    }

    switch (settings.mode) {
    case MODE_NORMAL:
        handle_inputs_normal();
        break;
    case MODE_LIGHTING:
        handle_inputs_lighting();
        break;
    case MODE_TERRAIN:
        handle_inputs_terrain();
        break;
    }
}

static inline void load_scene(void) {
    FILE *fp = fopen(settings.scene_filepath, "r");
    if (!fp) {
        perror("WARNING: Could not open scene file");
        return;
    }
    if (scene_file_load(fp, settings.skybox_directory,
                        settings.asset_directory)) {
        printf("WARNING: Did not load scene from file, either it doesn't exist "
               "or is in the wrong format.\n");
    }
    fclose(fp);
}

int game_init(char *scene_filepath, const char *vertex_shader,
              const char *entity_frag_shader, const char *terrain_frag_shader,
              const char *skybox_model) {
    settings.scene_filepath = scene_filepath;
    SetTraceLogLevel(LOG_WARNING);

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(800, 450, "Noble");
    SetTargetFPS(60);

    ui_init();
    scene_init();
    lighting_scene_init(BLACK, vertex_shader, entity_frag_shader,
                        terrain_frag_shader);
    scene_skybox_init(skybox_model);

    strcpy(settings.asset_directory, settings.project_directory);
    strcat(settings.asset_directory, "assets/");

    strcpy(settings.skybox_directory, settings.project_directory);
    strcat(settings.skybox_directory, "skyboxes/");

    strcpy(settings.terrain_texture_directory, settings.project_directory);
    strcat(settings.terrain_texture_directory, "terrain_textures/");

    assets_fetch_all(settings.asset_directory);
    skyboxes_fetch_all(settings.skybox_directory);
    terrain_textures_fetch_all(settings.terrain_texture_directory);

    lighting_scene_set_enabled(settings.lighting_enabled);

    terrain_init(50);
    load_scene();
    scene_load_selected_skybox(settings.skybox_directory);
    terrain_generate_mesh();
    terrain_textures_load_all_selected(settings.terrain_texture_directory);
    return 0;
}

void game_main(void) {
    while (!WindowShouldClose()) {

        if (settings.mode == MODE_LIGHTING &&
            lighting_edit_state.is_light_selected) {
            lighting_light_update(lighting_edit_state.currently_selected_light,
                                  lighting_edit_transform_get_delta_vector());
        }

        scene_check_for_model_file_updates();
        render();
        handle_inputs();
        ui_properties_menu_reset();
    }
}

void game_deinit(void) {
    scene_free();
    terrain_free();

    CloseWindow();
}
