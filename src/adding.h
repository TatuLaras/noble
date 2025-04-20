#ifndef _ADDING
#define _ADDING

// Handles adding objects to the scene

#include "common.h"
#include "game_interface.h"
#include "raycast.h"
#include "scene.h"
#include <raylib.h>
#include <stddef.h>
#include <string.h>

typedef struct {
    int adding;
    size_t entity_id;
    float rotation_angle_y;
} EntityAddingState;

// Instantiates an instance of the currently selected asset in `settings` at
// `ray` hit point into the `scene`.
void adding_asset_instantiate(EntityAddingState *state, Scene *scene,
                              Settings *settings, Ray ray);

// Updates position and rotation of the current entity being added to the scene
// (mouse button held down) according to the `ray` hit and rotation_angle.
void adding_entity_update(EntityAddingState *state, Scene *scene,
                          Settings *settings, Ray ray, float rotation_angle);

// Stops the entity adding process
void adding_stop(EntityAddingState *state, Scene *scene);

#endif
