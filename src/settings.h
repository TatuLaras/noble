#ifndef _SETTINGS
#define _SETTINGS

#include "handles.h"
#include "scene.h"

#define SETTINGS_ASSET_SLOTS_AVAILABLE 10
typedef struct {
    int quantize_to_grid_enabled;
    float grid_density;
    int fps_controls_enabled;
    int grid_enabled;
    int gizmos_enabled;
    int adding_raycast_include_objects;
    int lighting_enabled;
    int lighting_edit_mode_enabled;
    float gizmo_size;
    char asset_directory[MAX_PATH_LENGTH];
    char *scene_filepath;
    AssetHandle selected_asset[SETTINGS_ASSET_SLOTS_AVAILABLE];
    uint8_t current_asset_slot;
} Settings;

extern Settings settings;

#endif
