#include "terrain_edit.h"

#include "raygui.h"
#include "settings.h"
#include "terrain.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define RADIUS_EDIT_SENSITIVITY 0.0008

TerrainEditState terrain_edit_state = {.tool_radius = 0.5};
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
    terrain_edit_state.tool_radius +=
        amount * RADIUS_EDIT_SENSITIVITY * terrain_edit_state.tool_radius;
    if (terrain_edit_state.tool_radius < 0.1)
        terrain_edit_state.tool_radius = 0.1;
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
    return 0;
}

int terrain_is_point_within_tool_radius(Vector3 tool_origin,
                                        Vector3 terrain_point) {
    return Vector3DistanceSqr(terrain_point, tool_origin) <=
           terrain_edit_state.tool_radius * terrain_edit_state.tool_radius;
}

int terrain_edit_use_tool(Vector2 screen_pos, Camera camera,
                          ToolMode tool_mode) {
    RayCollision terrain_ray_collision =
        terrain_raycast(GetScreenToWorldRay(screen_pos, camera));
    if (!terrain_ray_collision.hit)
        return 0;

    if (!tool_applied_to_point || tool_applied_to_point_size != terrain.size) {
        if (tool_applied_to_point)
            free(tool_applied_to_point);
        tool_applied_to_point_size = terrain.size;
        tool_applied_to_point = calloc(terrain.size, 1);
        assert(tool_applied_to_point);
    }

    int changes = 0;

    for (size_t i = 0; i < terrain.size; i++) {
        if (tool_applied_to_point[i])
            continue;

        size_t x = i % terrain.width;
        size_t y = i / terrain.width;
        Vector3 terrain_point_world_pos =
            Vector3Add((Vector3){terrain.top_left_world_pos.x, 0,
                                 terrain.top_left_world_pos.y},
                       (Vector3){x, terrain.heights[i], y});

        if (terrain_is_point_within_tool_radius(terrain_ray_collision.point,
                                                terrain_point_world_pos)) {
            changes = 1;
            tool_applied_to_point[i] = 1;
            printf("%u\n", tool_mode);

            if (tool_mode == TOOL_MODE_TEXTURE) {
                // if (settings.current_asset_slot <= TERRAIN_MAX_TEXTURES)
                terrain.texture_indices[i] = settings.current_asset_slot;
            } else {
                //  TODO: smooth mode
                terrain.heights[i] =
                    tool_effect(terrain.heights[i],
                                tool_mode == TOOL_MODE_ALTERNATIVE_FUNCTION, 1);
            }
        }
    }

    return changes;
}

void terrain_edit_finish_brush_stroke(void) {
    memset(tool_applied_to_point, 0, tool_applied_to_point_size);
}
