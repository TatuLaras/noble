#include "terrain_edit.h"

#include "raygui.h"
#include "settings.h"
#include "terrain.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#define RADIUS_EDIT_SENSITIVITY 10.0

TerrainEditState terrain_edit_state = {.tool_radius = 30};
static uint8_t *tool_applied_to_point = 0;
static size_t tool_applied_to_point_size = 0;

TerrainScreenPoints terrain_edit_get_screen_points(Camera *camera) {
    BoundingBox bounds = terrain_get_bounds();
    bounds.max.x -= 1;
    bounds.max.z -= 1;

    return (TerrainScreenPoints){
        .top_left = GetWorldToScreen(bounds.min, *camera),
        .bottom_right = GetWorldToScreen(bounds.max, *camera),

        .top_right =
            GetWorldToScreen((Vector3){bounds.max.x, bounds.max.y,
                                       bounds.max.z - terrain.width + 1},
                             *camera),
        .bottom_left =
            GetWorldToScreen((Vector3){bounds.max.x - terrain.width + 1,
                                       bounds.max.y, bounds.max.z},
                             *camera),
    };
}

void terrain_edit_adjust_tool_radius(float amount) {
    terrain_edit_state.tool_radius += amount * RADIUS_EDIT_SENSITIVITY;
    if (terrain_edit_state.tool_radius < 10)
        terrain_edit_state.tool_radius = 10;
}

// Returns effect of the selected tool on `height_value`.
static inline float tool_effect(float height_value, int alternative_mode,
                                float distance_percentage) {
    switch (terrain_edit_state.tool) {
    case TERRAIN_TOOL_RAISE: {
        float multiplier = 1;
        if (alternative_mode)
            multiplier = -1;
        return height_value + settings.grid_density * multiplier;
    }
    case TERRAIN_TOOL_RAISE_SMOOTH: {
        float multiplier = 1;
        if (alternative_mode)
            multiplier = -1;

        multiplier *= 1 - distance_percentage;
        return height_value + settings.grid_density * multiplier;
    }
    case TERRAIN_TOOL_SET:
        return settings.grid_height;
    }
}

int terrain_edit_use_tool(Vector2 screen_pos, Camera camera,
                          int alternative_mode) {
    if (!tool_applied_to_point || tool_applied_to_point_size != terrain.size) {
        if (tool_applied_to_point)
            free(tool_applied_to_point);
        tool_applied_to_point_size = terrain.size;
        tool_applied_to_point = calloc(terrain.size, 1);
        assert(tool_applied_to_point);
    }

    int changes = 0;

    Vector3 camera_forward =
        Vector3Normalize(Vector3Subtract(camera.target, camera.position));

    // Iterate through points inside tool effect radius
    for (size_t i = 0; i < terrain.size; i++) {
        if (tool_applied_to_point[i])
            continue;

        size_t x = i % terrain.width;
        size_t y = i / terrain.width;
        Vector3 terrain_point_world_pos =
            Vector3Add((Vector3){terrain.top_left_world_pos.x, 0,
                                 terrain.top_left_world_pos.y},
                       (Vector3){x, terrain.heights[i], y});

        Vector3 direction_from_camera = Vector3Normalize(
            Vector3Subtract(terrain_point_world_pos, camera.position));

        if (Vector3DotProduct(direction_from_camera, camera_forward) <= 0)
            continue;

        Vector2 terrain_point_screen_pos =
            GetWorldToScreen(terrain_point_world_pos, camera);

        float distance_from_screen_pos =
            Vector2Distance(terrain_point_screen_pos, screen_pos);
        if (distance_from_screen_pos < terrain_edit_state.tool_radius) {
            terrain.heights[i] = tool_effect(
                terrain.heights[i], alternative_mode,
                distance_from_screen_pos / terrain_edit_state.tool_radius);

            tool_applied_to_point[i] = 1;
            changes = 1;
        }
    }
    return changes;
}

void terrain_edit_finish_brush_stroke(void) {
    memset(tool_applied_to_point, 0, tool_applied_to_point_size);
}
