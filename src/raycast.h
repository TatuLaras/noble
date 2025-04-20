#ifndef _MATH
#define _MATH

#include "game_interface.h"
#include <raylib.h>
#include <raymath.h>
#include <stddef.h>

typedef struct {
    int hit_something;
    size_t entity_id;
    Vector3 point;
} ObjectRaycastResult;

// Gets a transform matrix from the `ray` s intersection with the ground plane
// that can be used with model placement into the scene.
Matrix raycast_get_desired_model_transform(Settings *settings, Scene *scene,
                                           Ray ray);
// Casts a `ray` into the `scene` and returns information about which object was
// hit and the hit point.
ObjectRaycastResult raycast_scene_objects(Ray ray, Scene *scene);

#endif
