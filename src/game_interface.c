#include "game_interface.h"

#include "adding.h"
#include "asset_picker.h"
#include "assets.h"
#include "common.h"
#include "model_vector.h"
#include "raycast.h"
#include "raymath.h"
#include "scene.h"
#include "selection.h"
#include "shortcuts.h"
#include "string_vector.h"
#include "transform.h"
#include "ui.h"
#include <raylib.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define VERTICAL_MOVEMENT_SPEED 3.0
#define GIZMO_SIZE_ADJUST_SENSITIVITY 0.08
#define TRANSFORM_BASE_SENSITIVITY 0.006
#define TRANSFORM_SLOW_MODE_MULTIPLIER 0.1
#define MIN_GRID_DENSITY 0.015625
#define MAX_GRID_DENSITY 32

static TransformOperation transform_operation = {0};

static Scene scene = {0};
static ShortcutBuffer shortcuts = {0};

static Camera3D camera = {0};

static Settings settings = {
    .quantize_to_grid_enabled = 1,
    .grid_density = 1.0,
    .grid_enabled = 1,
    .gizmos_enabled = 1,
    .gizmo_size = 1.0,
    .asset_directory = "/home/tatu/_repos/ebb/assets/",
    .adding_raycast_include_objects = 1,
};

static EntityAddingState entity_adding_state = {0};
static EntitySelection entity_selection_state = {0};
static AssetPickerState asset_picker = {0};

static inline void refresh_asset_list(Settings *sett) {
    stringvec_free(&asset_picker.candidates);
    asset_picker.candidates = assets_get_list(sett->asset_directory);
}

static inline void render_gizmos(Matrix transform) {
    Vector3 origin = Vector3Transform(Vector3Zero(), transform);

    Vector3 x_axis =
        Vector3Transform((Vector3){settings.gizmo_size, 0, 0}, transform);
    Vector3 y_axis =
        Vector3Transform((Vector3){0, settings.gizmo_size, 0}, transform);
    Vector3 z_axis =
        Vector3Transform((Vector3){0, 0, settings.gizmo_size}, transform);

    DrawSphere(origin, 0.03, WHITE);
    DrawLine3D(origin, x_axis, RED);
    DrawLine3D(origin, y_axis, GREEN);
    DrawLine3D(origin, z_axis, BLUE);

    // Transform mode gizmos

    if (transform_operation.mode == TRANSFORM_TRANSLATE) {
        Vector3 direction;
        Color color;
        switch (transform_operation.axis) {
        case AXIS_X:
            direction = Vector3Subtract(x_axis, origin);
            color = RED;
            break;
        case AXIS_Y:
            direction = Vector3Subtract(y_axis, origin);
            color = GREEN;
            break;
        case AXIS_Z:
            direction = Vector3Subtract(z_axis, origin);
            color = BLUE;
            break;
        }

        direction = Vector3Normalize(direction);

        if (settings.quantize_to_grid_enabled)
            for (int i = -6; i <= 6; i++)
                DrawSphere(
                    Vector3Add(origin,
                               Vector3Scale(direction,
                                            (float)i * settings.grid_density)),
                    0.03, color);
        DrawLine3D(Vector3Add(origin, Vector3Scale(direction, -100.0)),
                   Vector3Add(origin, Vector3Scale(direction, 100.0)), color);
        return;
    }

    if (transform_operation.mode == TRANSFORM_ROTATE) {

        switch (transform_operation.axis) {
        case AXIS_X:
            DrawSphere(x_axis, 0.05, RED);
            break;
        case AXIS_Y:
            DrawSphere(y_axis, 0.05, GREEN);
            break;
        case AXIS_Z:
            DrawSphere(z_axis, 0.05, BLUE);
            break;
        }
        return;
    }

    // Normal mode gizmos

    DrawLine3D(origin, x_axis, RED);
    DrawSphere(x_axis, 0.05, RED);

    DrawLine3D(origin, y_axis, GREEN);
    DrawSphere(y_axis, 0.05, GREEN);

    DrawLine3D(origin, z_axis, BLUE);
    DrawSphere(z_axis, 0.05, BLUE);
}

static void render(void) {
    BeginDrawing();

    // Have gray background if waiting for another keystroke for a multi-key
    // shortcut
    if (shortcuts.keypresses_stored == 0)
        ClearBackground((Color){.r = 0x32, .g = 0x15, .b = 0x15});
    else
        ClearBackground((Color){.r = 0x5d, .g = 0x52, .b = 0x52});

    BeginMode3D(camera);

    size_t i = 0;
    LiveEntity *live_entity = {0};

    while ((live_entity = scene_get(&scene, i++))) {
        if (live_entity->is_destroyed)
            continue;

        Model *model = modelvec_get(&scene.models, live_entity->model_index);

        Matrix transform = live_entity->entity.transform;

        int is_active = (entity_selection_state.selected &&
                         entity_selection_state.entity_id == i - 1);
        int current_entity_being_transformed =
            is_active && transform_operation.mode != TRANSFORM_NONE;

        if (current_entity_being_transformed) {
            Matrix preview_transform =
                transform_get_matrix(&settings, &transform_operation);
            transform = MatrixMultiply(preview_transform,
                                       live_entity->entity.transform);
        }

        DrawMesh(model->meshes[0], model->materials[0], transform);

        int is_being_added = entity_adding_state.adding &&
                             entity_adding_state.entity_id == i - 1;

        if (settings.gizmos_enabled && (is_active || is_being_added))
            render_gizmos(transform);
    }

    if (settings.grid_enabled)
        DrawGrid(100 / settings.grid_density, settings.grid_density);

    EndMode3D();

    ui_render(GetScreenWidth(), &asset_picker);
    EndDrawing();
}

static inline void start_translating(TransformMode mode, Axis axis) {
    LiveEntity *selected_entity =
        selection_get_selected_entity(&scene, &entity_selection_state);
    if (!selected_entity)
        return;

    transform_start(&settings, &transform_operation, mode, axis,
                    selected_entity);
    DisableCursor();
}

static inline void stop_translating(void) {
    LiveEntity *selected_entity =
        selection_get_selected_entity(&scene, &entity_selection_state);
    if (selected_entity)
        transform_stop(&settings, &transform_operation, selected_entity);
    EnableCursor();
}

static inline void cancel_translate(void) {
    transform_cancel(&transform_operation);
    EnableCursor();
}

static void handle_shortcuts(void) {
    ShortcutAction action = shortcutbuf_get_action(
        &shortcuts, GetKeyPressed(), IsKeyDown(KEY_LEFT_SHIFT),
        IsKeyDown(KEY_LEFT_CONTROL), IsKeyDown(KEY_LEFT_ALT));

    switch (action) {

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
    case ACTION_OBJECT_DELETE: {
        if (entity_adding_state.adding) {
            scene_remove(&scene, entity_adding_state.entity_id);
            entity_adding_state.adding = 0;
        } else if (entity_selection_state.selected) {
            scene_remove(&scene, entity_selection_state.entity_id);
            selection_deselect_all(&entity_selection_state);
        }
    } break;

    case ACTION_START_PICKING_ASSET:
        asset_picker_start_search(&asset_picker);
        break;

    case ACTION_OBJECT_START_ROTATE_X:
        start_translating(TRANSFORM_ROTATE, AXIS_X);
        break;
    case ACTION_OBJECT_START_ROTATE_Y:
        start_translating(TRANSFORM_ROTATE, AXIS_Y);
        break;
    case ACTION_OBJECT_START_ROTATE_Z:
        start_translating(TRANSFORM_ROTATE, AXIS_Z);
        break;

    case ACTION_OBJECT_START_TRANSLATE_X:
        start_translating(TRANSFORM_TRANSLATE, AXIS_X);
        break;
    case ACTION_OBJECT_START_TRANSLATE_Y:
        start_translating(TRANSFORM_TRANSLATE, AXIS_Y);
        break;
    case ACTION_OBJECT_START_TRANSLATE_Z:
        start_translating(TRANSFORM_TRANSLATE, AXIS_Z);
        break;
    case ACTION_GRID_RESET:
        settings.grid_density = 1;
        break;
    case ACTION_CHANGE_AXIS_X:
        if (transform_operation.mode != TRANSFORM_NONE)
            start_translating(transform_operation.mode, AXIS_X);
        break;
    case ACTION_CHANGE_AXIS_Y:
        if (transform_operation.mode != TRANSFORM_NONE)
            start_translating(transform_operation.mode, AXIS_Y);
        break;
    case ACTION_CHANGE_AXIS_Z:
        if (transform_operation.mode != TRANSFORM_NONE)
            start_translating(transform_operation.mode, AXIS_Z);
        break;

    case ACTION_NONE:
        break;
    }
}

static inline void handle_inputs(void) {
    // Asset picker UI captures all inputs when active
    if (asset_picker.picking_asset) {
        // Select asset
        if (IsKeyPressed(KEY_ENTER)) {
            char *asset_identifier = stringvec_get(&asset_picker.matches,
                                                   asset_picker.selected_match);
            if (asset_identifier) {
                strncpy(settings.selected_asset, asset_identifier,
                        ARRAY_LENGTH(settings.selected_asset) - 1);
                asset_picker_stop_search(&asset_picker);
            }
        }

        asset_picker_input_key(&asset_picker, GetKeyPressed(),
                               IsKeyDown(KEY_LEFT_CONTROL));
        return;
    }

    handle_shortcuts();

    Vector2 mouse_delta = GetMouseDelta();
    float scroll = GetMouseWheelMove();

    // FPS style movement
    //  TODO: some easier controls as well as an option
    if (IsMouseButtonDown(MOUSE_BUTTON_MIDDLE)) {
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

    if (IsKeyDown(KEY_L)) {
        settings.gizmo_size -= scroll * GIZMO_SIZE_ADJUST_SENSITIVITY;
        if (settings.gizmo_size < 0.1)
            settings.gizmo_size = 0.1;
    }

    if (IsKeyDown(KEY_C)) {
        if (scroll > 0)
            settings.grid_density /= 2;
        else if (scroll < 0)
            settings.grid_density *= 2;

        if (settings.grid_density < MIN_GRID_DENSITY)
            settings.grid_density = MIN_GRID_DENSITY;
        if (settings.grid_density > MAX_GRID_DENSITY)
            settings.grid_density = MAX_GRID_DENSITY;
        return;
    }

    if (transform_operation.mode != TRANSFORM_NONE) {

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
            stop_translating();
        if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT))
            cancel_translate();

        float additional_multiplier = 1;
        if (IsKeyDown(KEY_LEFT_SHIFT))
            additional_multiplier = TRANSFORM_SLOW_MODE_MULTIPLIER;

        transform_operation.amount +=
            mouse_delta.x * TRANSFORM_BASE_SENSITIVITY * additional_multiplier;

        return;
    }

    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        selection_deselect_all(&entity_selection_state);

        Ray ray = GetScreenToWorldRay(GetMousePosition(), camera);
        adding_asset_instantiate(&entity_adding_state, &scene, &settings, ray);
        return;
    }

    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
        float rotate_by_angle = scroll * ROTATION_SNAP_INCREMENT;
        Ray ray = GetScreenToWorldRay(GetMousePosition(), camera);
        adding_entity_update(&entity_adding_state, &scene, &settings, ray,
                             rotate_by_angle);
        return;
    }

    if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT) &&
        entity_adding_state.adding) {
        selection_entity_select(&entity_selection_state,
                                entity_adding_state.entity_id);
        adding_stop(&entity_adding_state, &scene);
        return;
    }

    // Object seletion
    if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
        Ray ray = GetScreenToWorldRay(GetMousePosition(), camera);
        ObjectRaycastResult hit_result = raycast_scene_objects(ray, &scene);
        selection_deselect_all(&entity_selection_state);
        if (hit_result.hit_something)
            selection_entity_select(&entity_selection_state,
                                    hit_result.entity_id);
        return;
    }
}

void game_init(void) {

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(800, 450, "Noble");
    SetTargetFPS(60);

    ui_init();
    refresh_asset_list(&settings);

    scene = scene_init("/home/tatu/_repos/ebb/assets/");

    camera.position = (Vector3){0.0f, 6.0f, 6.0f};
    camera.target = (Vector3){0.0f, 0.0f, 0.0f};
    camera.up = (Vector3){0.0f, 1.0f, 0.0f};
    camera.fovy = 45.0f;
}

void game_main(void) {
    while (!WindowShouldClose()) {
        handle_inputs();
        render();
    }
}

void game_deinit(void) {
    scene_free(&scene);
    CloseWindow();
}
