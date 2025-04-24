#include "scene.h"

#include "model_files.h"
#include "model_vector.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ENTITIES_STARTING_SIZE 4
#define ENTITIES_GROWTH_FACTOR 2

Scene scene = {0};

static inline Model
load_model_by_asset_identifier(const char *asset_identifier) {
    char filepath[MAX_PATH_LENGTH + 1] = {0};
    assert(strlen(scene.assets_base_path) + strlen(asset_identifier) + 4 <=
           MAX_PATH_LENGTH);

    strcpy(filepath, scene.assets_base_path);
    strcat(filepath, asset_identifier);
    strcat(filepath, ".obj");
    Model model = LoadModel(filepath);
    if (!model.meshCount)
        return (Model){0};
    try_load_corresponding_texture(filepath, &model);
    return model;
}

void scene_init(const char *assets_base_path) {
    Scene new_scene = {
        .models = modelvec_init(),
        .entities = malloc(ENTITIES_STARTING_SIZE * sizeof(LiveEntity)),
        .entities_allocated = ENTITIES_STARTING_SIZE,
    };
    scene = new_scene;
    strncpy(scene.assets_base_path, assets_base_path, MAX_PATH_LENGTH - 1);
}

int scene_add(Entity entity, EntityHandle *out_entity_handle) {
    if (*entity.asset_identifier == 0)
        return 1;

    if (scene.entities_used >= scene.entities_allocated) {
        // Grow buffer
        scene.entities_allocated *= ENTITIES_GROWTH_FACTOR;
        scene.entities = realloc(scene.entities,
                                 scene.entities_allocated * sizeof(LiveEntity));
        assert(scene.entities);
    }

    LiveEntity live_entity = {
        .entity = entity,
    };

    // Check if model of entity already loaded, connect index
    size_t i = 0;
    LiveEntity *scene_entity = 0;
    int match_found = 0;
    while ((scene_entity = scene_get_entity(i++))) {
        if (!strcmp(entity.asset_identifier,
                    scene_entity->entity.asset_identifier)) {
            // Needs to not be a private instance of a model
            ModelData *model_data =
                modelvec_get(&scene.models, scene_entity->model_handle);
            assert(model_data);
            if (model_data->is_private_instance)
                continue;

            live_entity.model_handle = scene_entity->model_handle;
            match_found = 1;
            break;
        }
    }

    // If not, load model
    if (!match_found) {
        ModelData model_data = {.model = load_model_by_asset_identifier(
                                    live_entity.entity.asset_identifier)};
        if (!model_data.model.meshCount)
            return 1;

        live_entity.model_handle = scene.models.data_used;
        modelvec_append(&scene.models, model_data);
    }

    scene.entities[scene.entities_used++] = live_entity;

    if (out_entity_handle)
        *out_entity_handle = scene.entities_used - 1;

    return 0;
}

void scene_remove(EntityHandle handle) {
    if (handle < scene.entities_used)
        scene.entities[handle].is_destroyed = 1;
}

LiveEntity *scene_get_entity(EntityHandle handle) {
    if (handle >= scene.entities_used)
        return 0;
    return scene.entities + handle;
}

int scene_entity_model_unlink(LiveEntity *entity) {
    assert(entity);

    ModelData *model_data = modelvec_get(&scene.models, entity->model_handle);
    assert(model_data);

    ModelData new_instance = {
        .model =
            load_model_by_asset_identifier(entity->entity.asset_identifier),
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
