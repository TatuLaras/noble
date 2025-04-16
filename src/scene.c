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

void scene_store(Scene *scene, const char *filepath);
Scene scene_load(const char *filepath);

Scene scene_init(const char *assets_base_path) {
    Scene scene = {
        .models = modelvec_init(),
        .entities = malloc(ENTITIES_STARTING_SIZE * sizeof(LiveEntity)),
        .entities_allocated = ENTITIES_STARTING_SIZE,
    };
    strncpy(scene.assets_base_path, assets_base_path, MAX_PATH_LENGTH);
    return scene;
}

size_t scene_add(Scene *scene, Entity entity) {
    if (scene->entities_used >= scene->entities_allocated) {
        // Grow buffer
        scene->entities_allocated *= ENTITIES_GROWTH_FACTOR;
        scene->entities = realloc(scene->entities, scene->entities_allocated *
                                                       sizeof(LiveEntity));
        assert(scene->entities);
    }

    LiveEntity live_entity = {
        .entity = entity,
    };

    // Check if model of entity already loaded, connect index
    size_t i = 0;
    LiveEntity *scene_entity = 0;
    int match_found = 0;
    while (1) {
        scene_entity = scene_get(scene, i++);
        if (!scene_entity)
            break;

        if (!strcmp(entity.asset_identifier,
                    scene_entity->entity.asset_identifier)) {
            live_entity.model_index = scene_entity->model_index;
            match_found = 1;
            break;
        }
    }

    // If not, load model
    if (!match_found) {
        printf("LOAD\n");
        // TODO: safety
        char filepath[MAX_PATH_LENGTH + 1] = {0};
        strcpy(filepath, scene->assets_base_path);
        strcat(filepath, entity.asset_identifier);
        strcat(filepath, ".obj");

        Model model = LoadModel(filepath);
        try_load_corresponding_texture(filepath, &model);
        live_entity.model_index = scene->models.data_used;
        modelvec_append(&scene->models, model);
    }

    scene->entities[scene->entities_used++] = live_entity;
    return scene->entities_used - 1;
}

void scene_remove(Scene *scene, size_t id) {
    if (id >= scene->entities_used)
        return;

    //  TODO: Unload models if no others use it?
    scene->entities[id].is_destroyed = 1;
}

LiveEntity *scene_get(Scene *scene, size_t id) {
    if (id >= scene->entities_used)
        return 0;

    return scene->entities + id;
}

void scene_free(Scene *scene) {
    size_t i = 0;
    while (1) {
        Model *model = modelvec_get(&scene->models, i++);
        if (!model)
            break;
        UnloadModel(*model);
    }

    modelvec_free(&scene->models);
    if (scene->entities)
        free(scene->entities);
}
