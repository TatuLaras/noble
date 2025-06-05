#include "gizmos.h"
#include "lighting.h"
#include "lighting_edit.h"
#include "rlgl.h"
#include "settings.h"
#include "terrain.h"
#include "terrain_edit.h"
#include "transform.h"
#include <raylib.h>
#include <raymath.h>

void gizmos_draw_grid(int slices, float spacing, Vector3 origin) {
    int halfSlices = slices / 2;

    rlBegin(RL_LINES);
    for (int i = -halfSlices; i <= halfSlices; i++) {
        rlColor3f(0.5, 0.5, 0.5);

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

void gizmos_render_light_gizmos(Camera camera) {

    LightSourceHandle i = 0;
    LightSource *light = 0;
    while ((light = lighting_scene_get_light(i++))) {
        if (light->type == LIGHT_NULL)
            break;
        Vector3 light_3d_pos = light->position;
        int is_selected = lighting_edit_state.is_light_selected &&
                          lighting_edit_state.currently_selected_light == i - 1;

        // Light transform gizmo
        if (is_selected && transform_operation.mode == TRANSFORM_TRANSLATE) {
            switch (transform_operation.axis) {
            case AXIS_X:
                light_3d_pos.x += transform_operation.amount;
                break;
            case AXIS_Y:
                light_3d_pos.y += transform_operation.amount;
                break;
            case AXIS_Z:
                light_3d_pos.z += transform_operation.amount;
                break;
            }
        }

        Vector2 light_pos = GetWorldToScreen(light_3d_pos, camera);

        Vector2 light_base_pos;
        if (light->type == LIGHT_POINT)
            light_base_pos = GetWorldToScreen(
                (Vector3){light_3d_pos.x, settings.grid_height, light_3d_pos.z},
                camera);
        else
            light_base_pos = GetWorldToScreen(Vector3Zero(), camera);

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

void gizmos_render_terrain_gizmos(Camera camera) {

    Vector2 mouse_pos = GetMousePosition();
    if (IsCursorHidden())
        mouse_pos = terrain_edit_state.backup_screen_pos;

    RayCollision terrain_ray_collision =
        terrain_raycast(GetScreenToWorldRay(mouse_pos, camera));
    if (!terrain_ray_collision.hit)
        return;

    Vector2 terrain_point_screen_pos =
        GetWorldToScreen(terrain_ray_collision.point, camera);

    Vector3 radius_vector =
        Vector3Normalize(Vector3Subtract(camera.target, camera.position));
    radius_vector =
        Vector3Normalize(Vector3CrossProduct(radius_vector, camera.up));
    radius_vector = Vector3Scale(radius_vector, terrain_edit_state.tool_radius);
    radius_vector = Vector3Add(terrain_ray_collision.point, radius_vector);
    float screenspace_radius = Vector2Distance(
        GetWorldToScreen(radius_vector, camera), terrain_point_screen_pos);

    DrawCircleLinesV(terrain_point_screen_pos, screenspace_radius, WHITE);

    // Affected points

    for (size_t i = 0; i < terrain.size; i++) {
        size_t x = i % terrain.width;
        size_t y = i / terrain.width;
        Vector3 terrain_point_world_pos =
            Vector3Add((Vector3){terrain.top_left_world_pos.x, 0,
                                 terrain.top_left_world_pos.y},
                       (Vector3){x, terrain.heights[i], y});

        if (terrain_is_point_within_tool_radius(terrain_ray_collision.point,
                                                terrain_point_world_pos)) {
            Vector2 dot_pos = GetWorldToScreen(terrain_point_world_pos, camera);
            DrawCircleV(dot_pos, 3, RED);
        }
    }
}
