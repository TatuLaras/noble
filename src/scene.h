#ifndef _SCENE
#define _SCENE

#include "model_vector.h"
#include <raylib.h>
#include <raymath.h>
#include <stddef.h>
#include <stdint.h>

#define MAX_PATH_LENGTH 4096

// Base entity data, can be stored in a scene file.
typedef struct {
    uint32_t version;
    // Typically the name of the .obj file in the assets folder
    char asset_identifier[200];
    Matrix transform;
    int ignore_raycast;
} Entity;

// Entity data of a spawned in-game entity.
typedef struct {
    Entity entity;
    size_t model_index;
    int is_destroyed;
} LiveEntity;

typedef struct {
    LiveEntity *entities;
    char assets_base_path[MAX_PATH_LENGTH + 1];
    size_t entities_used;
    size_t entities_allocated;
    ModelVector models;
} Scene;

// Stores a serialized scene file to `filepath`.
void scene_store(Scene *scene, const char *filepath);
// Loads a serialized scene file from `filepath`.
Scene scene_load(const char *filepath);
// Initializes an empty scene.
Scene scene_init(const char *assets_base_path);
// Adds a new entity to the scene. Assumes a raylib context is already
// initialized. Returns the id of the added entity.
int scene_add(Scene *scene, Entity entity, size_t *out_id);
// Removes an entity from the scene by `ìd`.
void scene_remove(Scene *scene, size_t id);
// Gets entity of `scene` by `id`, returns 0 when no entity for that index
// exists.
LiveEntity *scene_get(Scene *scene, size_t id);
void scene_free(Scene *scene);

#endif
