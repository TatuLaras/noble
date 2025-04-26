#include "scene.h"

#include "assets.h"
#include "handles.h"
#include "model_files.h"
#include "settings.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ENTITIES_STARTING_SIZE 4
#define ENTITIES_GROWTH_FACTOR 2

Scene scene = {0};

// Loads a model of asset name `asset_name` from the asset directory (see
// settings.h).
static inline Model load_asset_model(AssetHandle handle) {
    char *asset_name = assets_get_name(handle);
    assert(asset_name);

    char filepath[MAX_PATH_LENGTH + 1] = {0};
    assert(strlen(settings.asset_directory) + strlen(asset_name) + 4 <=
           MAX_PATH_LENGTH);

    strcpy(filepath, settings.asset_directory);
    strcat(filepath, asset_name);
    strcat(filepath, ".obj");
    Model model = LoadModel(filepath);
    if (!model.meshCount)
        return (Model){0};
    try_load_corresponding_texture(filepath, &model);
    return model;
}

void scene_init(void) {
    scene = (Scene){
        .models = modelvec_init(),
        .entities = malloc(ENTITIES_STARTING_SIZE * sizeof(Entity)),
        .entities_allocated = ENTITIES_STARTING_SIZE,
    };
}

int scene_add(Entity entity, EntityHandle *out_entity_handle) {

    if (scene.entities_used >= scene.entities_allocated) {
        // Grow buffer
        scene.entities_allocated *= ENTITIES_GROWTH_FACTOR;
        scene.entities =
            realloc(scene.entities, scene.entities_allocated * sizeof(Entity));
        if (!scene.entities)
            return 1;
    }

    char *asset_name = assets_get_name(entity.asset_handle);
    assert(asset_name);

    // Check if model of entity already loaded, connect index
    size_t i = 0;
    Entity *scene_entity = 0;
    int match_found = 0;
    while ((scene_entity = scene_get_entity(i++))) {
        char *scene_entity_asset_name =
            assets_get_name(scene_entity->asset_handle);
        assert(scene_entity_asset_name);

        if (!strcmp(asset_name, scene_entity_asset_name)) {
            // Needs to not be a private instance of a model
            ModelData *model_data =
                modelvec_get(&scene.models, scene_entity->model_handle);
            assert(model_data);
            if (model_data->is_private_instance)
                continue;

            entity.model_handle = scene_entity->model_handle;
            match_found = 1;
            break;
        }
    }

    // If not, load model
    if (!match_found) {
        ModelData model_data = {.model = load_asset_model(entity.asset_handle)};
        if (!model_data.model.meshCount)
            return 1;

        entity.model_handle = modelvec_append(&scene.models, model_data);
    }

    scene.entities[scene.entities_used++] = entity;

    if (out_entity_handle)
        *out_entity_handle = scene.entities_used - 1;

    return 0;
}

void scene_remove(EntityHandle handle) {
    if (handle < scene.entities_used)
        scene.entities[handle].is_destroyed = 1;
}

Entity *scene_get_entity(EntityHandle handle) {
    if (handle >= scene.entities_used)
        return 0;
    return scene.entities + handle;
}

int scene_entity_model_unlink(Entity *entity) {
    assert(entity);

    ModelData *model_data = modelvec_get(&scene.models, entity->model_handle);
    assert(model_data);

    ModelData new_instance = {
        .model = load_asset_model(entity->asset_handle),
        .is_private_instance = 1,
    };
    assert(new_instance.model.meshCount);

    // Copy texture
    new_instance.model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture =
        model_data->model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture;

    entity->model_handle = modelvec_append(&scene.models, new_instance);
    return 0;
}

void scene_free(void) {
    size_t i = 0;
    ModelData *model_data = 0;
    while ((model_data = modelvec_get(&scene.models, i++)))
        UnloadModel(model_data->model);

    modelvec_free(&scene.models);
    if (scene.entities)
        free(scene.entities);
}
