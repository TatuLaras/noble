#ifndef _SETTINGS
#define _SETTINGS

#include "common.h"
#include "handles.h"

#define SETTINGS_ASSET_SLOTS_AVAILABLE 10
#define SETTINGS_DEFAULT_GRID_DENSITY 1.0

typedef enum {
    MODE_NORMAL,
    MODE_LIGHTING,
    MODE_TERRAIN,
} Mode;

typedef struct {
    int quantize_to_grid_enabled;
    float grid_density;
    float grid_height;
    int mouse_movements_vertical;
    int grid_enabled;
    int fps_controls_enabled;
    int gizmos_enabled;
    int properties_menu_enabled;
    int adding_raycast_include_objects;
    int lighting_enabled;
    Mode mode;
    int debug_info_enabled;
    float gizmo_size;
    char project_directory[MAX_PATH_LENGTH];
    char asset_directory[MAX_PATH_LENGTH];
    char skybox_directory[MAX_PATH_LENGTH];
    char terrain_texture_directory[MAX_PATH_LENGTH];
    char *scene_filepath;

    uint8_t current_asset_slot;
    AssetHandle selected_assets[SETTINGS_ASSET_SLOTS_AVAILABLE];
} Settings;

extern Settings settings;

// Quantize `vec` to grid depending on the current grid settings.
Vector3 settings_quantize_to_grid(Vector3 vec, int ignore_enabled);

#endif
