#ifndef _SCENE
#define _SCENE

#include "handles.h"
#include "model_vector.h"
#include <raylib.h>
#include <raymath.h>
#include <stddef.h>
#include <stdint.h>

#define MAX_PATH_LENGTH 4096

// Base entity data
typedef struct {
    AssetHandle asset_handle;
    LightingGroupHandle lighting_group_handle;
    ModelHandle model_handle;
    Matrix transform;
    int is_destroyed;
    int ignore_raycast;
    int is_unlit;
} Entity;

typedef struct {
    Entity *entities;
    size_t entities_used;
    size_t entities_allocated;
    ModelVector models;
} Scene;

extern Scene scene;

void scene_init(void);
// Adds a new entity to the scene. Assumes a raylib context is already
// initialized. Returns 1 on error.
int scene_add(Entity entity, EntityHandle *out_entity_handle);
// Removes an entity from the scene by `handle`.
void scene_remove(EntityHandle handle);
// Gets entity of `scene` by `id`, returns 0 when no entity for that index
// exists.
Entity *scene_get_entity(EntityHandle handle);
// Unlinks an entity's model into it's own private instance of that model.
int scene_entity_model_unlink(Entity *entity);
void scene_free(void);

#endif
