#ifndef _SETTINGS
#define _SETTINGS

#include "scene.h"

typedef struct {
    int quantize_to_grid_enabled;
    float grid_density;
    int grid_enabled;
    int gizmos_enabled;
    int adding_raycast_include_objects;
    int lighting_enabled;
    int lighting_edit_mode_enabled;
    float gizmo_size;
    char asset_directory[MAX_PATH_LENGTH];
    char selected_asset[200];
} Settings;

extern Settings settings;

#endif
