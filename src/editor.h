#ifndef _EDITOR
#define _EDITOR

// Editor logic and functionality

#include "lighting.h"
#include "shortcuts.h"
#include <raylib.h>

void editor_execute_action(ShortcutAction action);
void editor_cancel_transform(void);
void editor_stop_transform(void);
void editor_mouse_select_object(Ray ray);
void editor_instantiate_object(Ray ray, LightingGroupHandle handle);
void editor_transform_adjust(float amount, int slow_mode);
void editor_added_light_adjust(float amount, int slow_mode);
void editor_adjust_grid_density(float amount);
void editor_adjust_grid_height(float amount);
void editor_adjust_gizmo_size(float amount);
void editor_set_fps_controls_enabled(int enabled);

#endif
