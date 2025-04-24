#ifndef _SCENE
#define _SCENE

#include "model_vector.h"
#include <raylib.h>
#include <raymath.h>
#include <stddef.h>
#include <stdint.h>

#define MAX_PATH_LENGTH 4096

// Can't have a circular include with lighting.h so we define these here too...
typedef size_t EntityHandle;
typedef size_t LightingGroupHandle;

// Base entity data, can be stored in a scene file.
typedef struct {
    // Typically the name of the .obj file in the assets folder
    char asset_identifier[200];
    Matrix transform;
    int ignore_raycast;
    int is_unlit;
    LightingGroupHandle lighting_group_handle;
} Entity;

// Entity data of a spawned in-game entity.
typedef struct {
    Entity entity;
    ModelHandle model_handle;
    int is_destroyed;
} LiveEntity;

typedef struct {
    LiveEntity *entities;
    char assets_base_path[MAX_PATH_LENGTH];
    size_t entities_used;
    size_t entities_allocated;
    ModelVector models;
} Scene;

extern Scene scene;

// Initializes the scene.
void scene_init(const char *assets_base_path);
// Adds a new entity to the scene. Assumes a raylib context is already
// initialized. Returns 1 on error.
int scene_add(Entity entity, EntityHandle *out_entity_handle);
// Removes an entity from the scene by `handle`.
void scene_remove(EntityHandle handle);
// Gets entity of `scene` by `id`, returns 0 when no entity for that index
// exists.
LiveEntity *scene_get_entity(EntityHandle handle);
// Unlinks an entity's model into it's own private instance of that model.
int scene_entity_model_unlink(LiveEntity *entity);
void scene_free(void);

#endif
