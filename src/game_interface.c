#include "game_interface.h"

#include "common.h"
#include "math_helpers.h"
#include "model_vector.h"
#include "raymath.h"
#include "scene.h"
#include "shortcuts.h"
#include <math.h>
#include <raylib.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#define VERTICAL_MOVEMENT_SPEED 3.0
#define GIZMO_SIZE_ADJUST_SENSITIVITY 0.004
#define SCROLL_ROTATION_INCREMENT 0.0625 * PI;

typedef enum {
    TRANSFORM_NONE,
    TRANSFORM_TRANSLATE,
    TRANSFORM_ROTATE,
} TransformMode;

typedef enum { AXIS_X, AXIS_Y, AXIS_Z } Axis;

typedef struct {
    TransformMode mode;
    Axis axis;
} TransformOperation;

static TransformOperation current_transform_operation = {0};

static Scene scene = {0};
static ShortcutBuffer shortcuts = {0};

static Camera3D camera = {0};

static int quantize_to_grid_enabled = 1;
static float grid_density = 1.0;
static int grid_enabled = 1;
static int gizmos_enabled = 1;
static float gizmo_size = 1.0;

static size_t currently_added_entity_id = 0;
static float currently_added_entity_rotation_angle = 0;
static int currently_adding_an_entity = 0;

static size_t currently_selected_entity_id = 0;
static int entity_has_been_selected = 0;

static inline float quantize(float value, float interval) {
    return roundf(value / interval) * interval;
}

static inline Matrix get_targetted_position_translation_matrix(void) {
    Ray ray = GetScreenToWorldRay(GetMousePosition(), camera);
    Vector3 targetted_position = math_ray_ground_intersection(ray);

    if (quantize_to_grid_enabled) {
        targetted_position.x = quantize(targetted_position.x, grid_density);
        targetted_position.y = quantize(targetted_position.y, grid_density);
        targetted_position.z = quantize(targetted_position.z, grid_density);
    }

    Matrix translation = MatrixTranslate(
        targetted_position.x, targetted_position.y, targetted_position.z);

    return translation;
}

static void render(void) {
    BeginDrawing();

    // Have gray background if waiting for another keystroke for a multi-key
    // shortcut
    if (shortcuts.keypresses_stored == 0)
        ClearBackground((Color){.r = 0x14, .g = 0x26, .b = 0x32});
    else
        ClearBackground(GRAY);

    BeginMode3D(camera);

    size_t i = 0;
    LiveEntity *live_entity = {0};

    while ((live_entity = scene_get(&scene, i++))) {
        if (live_entity->is_destroyed)
            continue;

        Model *model = modelvec_get(&scene.models, live_entity->model_index);

        DrawMesh(model->meshes[0], model->materials[0],
                 live_entity->entity.transform);

        int is_active =
            (entity_has_been_selected && currently_selected_entity_id == i - 1);

        if (gizmos_enabled && is_active) {
            Vector3 origin =
                Vector3Transform(Vector3Zero(), live_entity->entity.transform);
            origin.y += 0.01;
            Vector3 x_axis = Vector3Transform((Vector3){gizmo_size, 0, 0},
                                              live_entity->entity.transform);
            Vector3 y_axis = Vector3Transform((Vector3){0, gizmo_size, 0},
                                              live_entity->entity.transform);
            Vector3 z_axis = Vector3Transform((Vector3){0, 0, gizmo_size},
                                              live_entity->entity.transform);
            DrawSphere(origin, 0.03, WHITE);

            DrawLine3D(origin, x_axis, RED);
            DrawSphere(x_axis, 0.05, RED);

            DrawLine3D(origin, y_axis, GREEN);
            DrawSphere(y_axis, 0.05, GREEN);

            DrawLine3D(origin, z_axis, BLUE);
            DrawSphere(z_axis, 0.05, BLUE);
        }
    }

    if (grid_enabled)
        DrawGrid(100 / grid_density, grid_density);

    EndMode3D();
    EndDrawing();
}

static void handle_shortcuts(void) {
    ShortcutAction action = shortcutbuf_get_action(
        &shortcuts, GetKeyPressed(), IsKeyDown(KEY_LEFT_SHIFT),
        IsKeyDown(KEY_LEFT_CONTROL), IsKeyDown(KEY_LEFT_ALT));

    switch (action) {

    case ACTION_TOGGLE_GRID:
        grid_enabled = !grid_enabled;
        break;
    case ACTION_TOGGLE_GIZMOS:
        gizmos_enabled = !gizmos_enabled;
        break;
    case ACTION_TOGGLE_QUANTIZE:
        quantize_to_grid_enabled = !quantize_to_grid_enabled;
        break;
    case ACTION_GRID_DENSITY_INCREASE:
        grid_density /= 2;
        break;
    case ACTION_GRID_DENSITY_DECREASE:
        grid_density *= 2;
        break;
    case ACTION_OBJECT_DELETE: {
        if (currently_adding_an_entity) {
            scene_remove(&scene, currently_added_entity_id);
            currently_adding_an_entity = 0;
        }
    } break;

    case ACTION_OBJECT_START_ROTATE_X:
        current_transform_operation.mode = TRANSFORM_ROTATE;
        current_transform_operation.axis = AXIS_X;
        DisableCursor();
        break;

    case ACTION_OBJECT_START_TRANSLATE_X:
    case ACTION_OBJECT_START_TRANSLATE_Y:
    case ACTION_OBJECT_START_TRANSLATE_Z:
    case ACTION_OBJECT_START_ROTATE_Y:
    case ACTION_OBJECT_START_ROTATE_Z:
        printf("Warning: Shortcut action %d not yet implemented.\n", action);
        break;
    case ACTION_NONE:
        break;
    }
}

void game_init(void) {
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(800, 450, "Noble");
    SetTargetFPS(60);

    scene = scene_init("/home/tatu/_repos/ebb/assets/");

    camera.position = (Vector3){0.0f, 6.0f, 6.0f};
    camera.target = (Vector3){0.0f, 0.0f, 0.0f};
    camera.up = (Vector3){0.0f, 1.0f, 0.0f};
    camera.fovy = 45.0f;
}

void game_main(void) {
    while (!WindowShouldClose()) {
        Vector2 mouse_delta = GetMouseDelta();

        handle_shortcuts();

        // FPS style movement
        //  TODO: some easier controls as well as an option
        if (IsMouseButtonDown(MOUSE_BUTTON_MIDDLE)) {
            float delta_time = GetFrameTime();
            float vertical_movement = 0;

            if (IsKeyDown(KEY_SPACE))
                vertical_movement = delta_time * VERTICAL_MOVEMENT_SPEED;
            else if (IsKeyDown(KEY_C))
                vertical_movement = delta_time * VERTICAL_MOVEMENT_SPEED * -1;

            camera.position.y += vertical_movement;
            camera.target.y += vertical_movement;

            UpdateCamera(&camera, CAMERA_FIRST_PERSON);
        }

        if (IsKeyDown(KEY_L)) {
            gizmo_size -= mouse_delta.y * GIZMO_SIZE_ADJUST_SENSITIVITY;
        }

        // Clamp values
        if (gizmo_size < 0.1)
            gizmo_size = 0.1;

        // Add objects to scene on left click
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            currently_adding_an_entity = 1;

            Entity entity = {
                .asset_identifier = "barrel",
                .transform = get_targetted_position_translation_matrix(),
            };
            currently_added_entity_id = scene_add(&scene, entity);
        }

        // Update position while mouse left held down
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) &&
            currently_adding_an_entity) {

            LiveEntity *entity = scene_get(&scene, currently_added_entity_id);

            if (entity) {
                currently_added_entity_rotation_angle +=
                    GetMouseWheelMove() * SCROLL_ROTATION_INCREMENT;

                entity->entity.transform = MatrixMultiply(
                    MatrixRotateY(currently_added_entity_rotation_angle),
                    get_targetted_position_translation_matrix());
            }
        }

        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
            currently_adding_an_entity = 0;
            currently_selected_entity_id = currently_added_entity_id;
            entity_has_been_selected = 1;
        }

        // Object seletion
        if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
            Ray ray = GetScreenToWorldRay(GetMousePosition(), camera);
            RayCollision collision = {0};

            entity_has_been_selected = 0;

            LiveEntity *live_entity = {0};
            size_t i = 0;
            while ((live_entity = scene_get(&scene, i++))) {
                if (live_entity->is_destroyed)
                    continue;

                Model *model =
                    modelvec_get(&scene.models, live_entity->model_index);

                collision = GetRayCollisionMesh(ray, model->meshes[0],
                                                live_entity->entity.transform);

                if (collision.hit) {
                    currently_selected_entity_id = i - 1;
                    entity_has_been_selected = 1;
                    break;
                }
            }
        }

        if (current_transform_operation.mode != TRANSFORM_NONE &&
            entity_has_been_selected) {
            LiveEntity *entity =
                scene_get(&scene, currently_selected_entity_id);

            if (entity) {
                if (current_transform_operation.mode == TRANSFORM_ROTATE) {
                    entity->entity.transform =
                        MatrixMultiply(MatrixRotateX(mouse_delta.x * 0.004),
                                       entity->entity.transform);
                }
            }
        }

        render();
    }
}

void game_deinit(void) {
    scene_free(&scene);
    CloseWindow();
}
