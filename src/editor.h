#ifndef _EDITOR
#define _EDITOR

// User-facing editor logic.

#include "shortcuts.h"
#include <raylib.h>

// Execute ShortcutAction `action`.
void editor_execute_action(ShortcutAction action, Camera *camera);
// Makes the closest object to the camera that intersects with `ray` the
// currently selected object.
void editor_mouse_select_object(Ray ray);
// Instantiates an entity with the current asset at `ray` intersection point
// and adds it to the lighting scene.
//
// `copy_rotation`: Whether or not the rotation of the previously added entity
// will be preserved.
void editor_instantiate_object(Ray ray, int copy_rotation);
// Cancels currently active entity or light source transform operation.
void editor_cancel_transform(void);
// Stops and applies currently active entity or light source transform
// operation.
void editor_stop_transform(void);
// Adjust the amount of the currently active transform operation.
void editor_transform_adjust(float amount, int slow_mode);
// Adjust the position of the current light source being added (mouse button
// held down after clicking to add).
void editor_added_light_adjust(float amount, int slow_mode);
// Doubles grid density if `amount` < 0, halves it if `amount` > 0.
void editor_adjust_grid_density(float amount);
// Adjusts the ground plane / grid height, also used as the ground when
// adding lights or entities.
void editor_adjust_grid_height(float amount);
// Adjusts transform gizmo lenght by `amount`.
void editor_adjust_gizmo_size(float amount);
// Enters first-person-view mode.
void editor_set_fpv_controls_enabled(Camera *camera, int enabled);
// Gets a transform matrix from the `ray` s intersection with the ground plane
// that can be used with model placement into the scene.
Vector3 editor_general_scene_raycast(Ray ray, int no_quantize);

#endif
