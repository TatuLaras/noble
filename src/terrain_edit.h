#ifndef _TERRAIN_EDIT
#define _TERRAIN_EDIT

#include "settings.h"
#include <raylib.h>
#include <stdint.h>

typedef struct {
    Vector2 top_left;
    Vector2 bottom_right;
    Vector2 top_right;
    Vector2 bottom_left;
} TerrainScreenPoints;

typedef enum {
    TERRAIN_TOOL_RAISE,
    TERRAIN_TOOL_RAISE_SMOOTH,
    TERRAIN_TOOL_SET,
} TerrainEditTool;

typedef enum {
    TOOL_MODE_NORMAL,
    TOOL_MODE_ALTERNATIVE_FUNCTION,
    TOOL_MODE_TEXTURE,
} ToolMode;

typedef struct {
    TerrainEditTool tool;
    float tool_radius;
} TerrainEditState;

extern TerrainEditState terrain_edit_state;

// Get screen positions of terrain boundary corners.
TerrainScreenPoints terrain_edit_get_screen_points(Camera *camera);
// Adjusts tool radius depending on a sensitivity constant and `amount`.
void terrain_edit_adjust_tool_radius(float amount);
// Applies currently selected terrain edit tool to points within tool affect
// radius of `screen_pos`.
// Returns 1 if terrain data was changed.
int terrain_edit_use_tool(Vector2 screen_pos, Camera camera,
                          ToolMode tool_mode);

void terrain_edit_finish_brush_stroke(void);

// Returns 1 if `terrain_point` is within the radius of points which will be
// affected by a tool if used.
int terrain_is_point_within_tool_radius(Vector3 tool_origin,
                                        Vector3 terrain_point);

#endif
