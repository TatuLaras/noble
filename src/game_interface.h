#ifndef _GAME_INTERFACE
#define _GAME_INTERFACE

#include "scene.h"
#include <stddef.h>

typedef struct {
    int quantize_to_grid_enabled;
    float grid_density;
    int grid_enabled;
    int gizmos_enabled;
    int adding_raycast_include_objects;
    float gizmo_size;
    char asset_directory[MAX_PATH_LENGTH];
    char selected_asset[200];
} Settings;

// Takes ownership of `filepaths`.
void game_init(void);
void game_main(void);
void game_deinit(void);

#endif
