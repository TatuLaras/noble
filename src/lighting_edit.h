#ifndef _LIGHTING_EDIT
#define _LIGHTING_EDIT

#include "lighting.h"

typedef struct {
    LightSourceHandle selected_light;
    LightSourceHandle currently_added_light;
    int is_light_selected;
    int is_light_added;
} LightingEditState;

// Adds a light source to the scene.
int lighting_edit_add_light(LightingEditState *state,
                            LightingGroupHandle group_handle, Ray ray);

#endif
