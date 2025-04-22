#include "scene.h"

#include "model_files.h"
#include "model_vector.h"
#include <assert.h>
#include <raylib.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ENTITIES_STARTING_SIZE 4
#define ENTITIES_GROWTH_FACTOR 2

Scene scene = {0};

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
    while (1) {
        scene_entity = scene_get_entity(i++);
        if (!scene_entity)
            break;

        if (!strcmp(entity.asset_identifier,
                    scene_entity->entity.asset_identifier)) {
            live_entity.model_id = scene_entity->model_id;
            match_found = 1;
            break;
        }
    }

    // If not, load model
    if (!match_found) {
        char filepath[MAX_PATH_LENGTH + 1] = {0};
        if (strlen(scene.assets_base_path) + strlen(entity.asset_identifier) +
                4 >
            MAX_PATH_LENGTH)
            return 1;

        strcpy(filepath, scene.assets_base_path);
        strcat(filepath, entity.asset_identifier);
        strcat(filepath, ".obj");

        Model model = LoadModel(filepath);
        if (!model.meshCount)
            return 1;

        try_load_corresponding_texture(filepath, &model);
        live_entity.model_id = scene.models.data_used;
        modelvec_append(&scene.models, model);
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

void scene_free(void) {
    size_t i = 0;
    Model *model = 0;
    while ((model = modelvec_get(&scene.models, i++)))
        UnloadModel(*model);

    modelvec_free(&scene.models);
    if (scene.entities)
        free(scene.entities);
}
