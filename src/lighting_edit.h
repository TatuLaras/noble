#ifndef _LIGHTING_EDIT
#define _LIGHTING_EDIT

// Functions for editing the lighting of a scene.

#include "transform.h"
#include <raylib.h>

#define LIGHT_SELECT_RADIUS 20

typedef struct {
    LightSourceHandle currently_selected_light;
    LightSourceHandle currently_added_light;
    int is_light_selected;
    int is_light_added;
} LightingEditState;

extern LightingEditState lighting_edit_state;

// Adds a light source to the scene.
int lighting_edit_add_light(Ray ray);
// Toggles the enabled status of the selected light source.
void lighting_edit_selected_light_toggle_enabled(void);
// Updates the y position of the currently added light (mouse button held down).
void lighting_edit_adding_light_update(float delta_y);
// Stops the light adding process.
void lighting_edit_adding_stop(void);

// Selects the closest light at `screen_position`.
void lighting_edit_select_light_at(Vector2 screen_position, Camera3D camera);

// Starts the transforming process for the currently selected light source. The
// to-be position can be adjusted through the `transform_operation` struct in
// "transform.h" and finally applied using `lighting_edit_transform_apply`.
void lighting_edit_transform_start(Axis axis);
// Stops the transform process and applies the transformation.
void lighting_edit_transform_stop(void);
// Stops the transform process discarding the transformation.
void lighting_edit_transform_cancel(void);
// Applies the information in the `transform_operation` struct in "transform.h"
// to the actual light position, committing changes.
void lighting_edit_transform_apply(void);
// Gets the position change vector from the current transform operation.
Vector3 lighting_edit_transform_get_delta_vector(void);

#endif
