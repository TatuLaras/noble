#ifndef _ADDING
#define _ADDING

// Handles adding objects to the scene

#include "handles.h"
#include <raylib.h>
#include <stddef.h>

typedef struct {
    int adding;
    EntityHandle entity_handle;
} EntityAddingState;

extern EntityAddingState entity_adding_state;

// Instantiates an instance of the currently selected asset in `settings` at
// `ray` hit point into the `scene`. Returns 1 on error.
int adding_asset_instantiate(Ray ray, int copy_rotation);

// Updates position and rotation of the current entity being added to the scene
// (mouse button held down) according to the `ray` hit and rotation_angle.
void adding_entity_update(Ray ray, float rotation_angle);

// Stops the entity adding process
void adding_stop(void);

#endif
