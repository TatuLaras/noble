#include "gizmos.h"
#include "lighting_edit.h"
#include "rlgl.h"
#include "settings.h"
#include "transform.h"
#include <raylib.h>
#include <raymath.h>

void gizmos_draw_grid(int slices, float spacing, Vector3 origin) {
    int halfSlices = slices / 2;

    rlBegin(RL_LINES);
    for (int i = -halfSlices; i <= halfSlices; i++) {
        rlColor3f(0.75f, 0.75f, 0.75f);

        rlVertex3f(origin.x + (float)i * spacing, origin.y,
                   origin.z - (float)halfSlices * spacing);
        rlVertex3f(origin.x + (float)i * spacing, origin.y,
                   origin.z + (float)halfSlices * spacing);

        rlVertex3f(origin.x - (float)halfSlices * spacing, origin.y,
                   origin.z + (float)i * spacing);
        rlVertex3f(origin.x + (float)halfSlices * spacing, origin.y,
                   origin.z + (float)i * spacing);
    }
    rlEnd();
}

void gizmos_render_transform_gizmo(Matrix transform) {

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

void gizmos_render_light_gizmos(LightingGroupHandle handle, Camera3D camera) {

    LightSourceHandle i = 0;
    LightSource *light = 0;
    while ((light = lighting_group_get_light(handle, i++))) {
        if (light->type == LIGHT_NULL)
            break;
        Vector3 light_3d_pos = light->position;
        int is_selected = lighting_edit_state.is_light_selected &&
                          lighting_edit_state.currently_selected_light == i - 1;

        // Light transform gizmo
        if (is_selected && transform_operation.mode == TRANSFORM_TRANSLATE) {
            BeginMode3D(camera);
            switch (transform_operation.axis) {
            case AXIS_X:
                light_3d_pos.x += transform_operation.amount;
                DrawLine3D((Vector3){-100, light_3d_pos.y, light_3d_pos.z},
                           (Vector3){100, light_3d_pos.y, light_3d_pos.z}, RED);
                break;
            case AXIS_Y:
                light_3d_pos.y += transform_operation.amount;
                DrawLine3D((Vector3){light_3d_pos.x, -100, light_3d_pos.z},
                           (Vector3){light_3d_pos.x, 100, light_3d_pos.z},
                           GREEN);
                break;
            case AXIS_Z:
                light_3d_pos.z += transform_operation.amount;
                DrawLine3D((Vector3){light_3d_pos.x, light_3d_pos.y, -100},
                           (Vector3){light_3d_pos.x, light_3d_pos.y, 100},
                           BLUE);
                break;
            }
            EndMode3D();
        }

        Vector2 light_pos = GetWorldToScreen(light_3d_pos, camera);
        Vector2 light_base_pos = GetWorldToScreen(
            (Vector3){light_3d_pos.x, settings.grid_height, light_3d_pos.z},
            camera);

        if (light_3d_pos.y < settings.grid_height &&
            light_base_pos.y > light_pos.y)
            continue;
        if (light_3d_pos.y > settings.grid_height &&
            light_base_pos.y < light_pos.y)
            continue;

        DrawLineEx(light_pos, light_base_pos, 2.0, light->color);

        DrawCircle(light_pos.x, light_pos.y, 6, light->color);

        Color color = GRAY;
        if (lighting_edit_state.is_light_selected &&
            lighting_edit_state.currently_selected_light == i - 1)
            color = WHITE;

        DrawCircleLines(light_pos.x, light_pos.y, LIGHT_SELECT_RADIUS, color);

        if (light->is_disabled)
            DrawLineEx(
                (Vector2){light_pos.x - LIGHT_SELECT_RADIUS - 4, light_pos.y},
                (Vector2){light_pos.x + LIGHT_SELECT_RADIUS + 4, light_pos.y},
                2, color);
    }
}
