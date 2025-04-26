#include "scene_file.h"

#include "assets.h"
#include "common.h"
#include "general_buffer.h"
#include "handles.h"
#include "lighting.h"
#include "scene.h"
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static inline void
serialize_asset_data_into_buf(GeneralBuffer *buf,
                              size_t *out_asset_table_entries) {
    size_t asset_table_entries = asset_list.indices_used;
    char *asset_table =
        genbuf_allocate(buf, NAME_MAX_LENGTH * asset_table_entries);

    AssetHandle asset_handle = 0;
    char *asset_name = 0;
    while ((asset_name = assets_get_name(asset_handle)) &&
           asset_handle < asset_table_entries) {
        strncpy(asset_table + asset_handle * NAME_MAX_LENGTH, asset_name,
                NAME_MAX_LENGTH);
        asset_handle++;
    }

    if (out_asset_table_entries)
        *out_asset_table_entries = asset_table_entries;
}

static inline void
serialize_lighting_data_into_buf(GeneralBuffer *buf,
                                 uint16_t *out_light_group_table_entries,
                                 size_t *out_light_source_table_entries) {
    GeneralBuffer light_sources = genbuf_init();
    LightingGroup *group = 0;
    LightSource *light = 0;
    LightingGroupHandle group_handle = 0;
    LightSourceHandle light_handle = 0;
    uint16_t lighting_group_table_entries = 0;
    size_t light_source_table_entries = 0;
    while ((group = lighting_scene_get_group(group_handle++))) {
        if (group->is_deleted)
            continue;

        SceneFileLightGroup scene_file_group = {.ambient_color =
                                                    group->ambient_color};
        genbuf_append(buf, &scene_file_group, sizeof(SceneFileLightGroup));

        // Light sources
        light_handle = 0;
        while (
            (light = lighting_group_get_light(group_handle, light_handle++))) {
            SceneFileLightSource scene_file_light = {
                .light_group_index = lighting_group_table_entries,
                .color = light->color,
                .is_disabled = light->is_disabled,
                .type = light->type,
                .intensity = light->intensity,
                .intensity_granular = light->intensity_granular,
                .intensity_cap = light->intensity_cap,
                .position = light->position,
            };
            genbuf_append(&light_sources, &scene_file_light,
                          sizeof(SceneFileLightSource));
            light_source_table_entries++;
        }

        lighting_group_table_entries++;
    }

    genbuf_append(buf, light_sources.data, light_sources.data_size);
    genbuf_free(&light_sources);

    if (out_light_group_table_entries)
        *out_light_group_table_entries = lighting_group_table_entries;
    if (out_light_source_table_entries)
        *out_light_source_table_entries = light_source_table_entries;
}

static inline void
serialize_entity_data_into_buf(GeneralBuffer *buf,
                               size_t *out_entity_table_entries) {
    EntityHandle entity_handle = 0;
    Entity *entity = 0;
    size_t entity_table_entries = 0;
    while ((entity = scene_get_entity(entity_handle++))) {
        if (entity->is_destroyed)
            continue;
        SceneFileEntity scene_file_entity = {
            .transform = entity->transform,
            .ignore_raycast = entity->ignore_raycast,
            .is_unlit = entity->is_unlit,
        };
        genbuf_append(buf, &scene_file_entity, sizeof(SceneFileEntity));
        entity_table_entries++;
    }

    if (out_entity_table_entries)
        *out_entity_table_entries = entity_table_entries;
}

void scene_file_store(FILE *fp) {
    GeneralBuffer content = genbuf_init();

    size_t asset_table_entries = 0;
    serialize_asset_data_into_buf(&content, &asset_table_entries);

    uint16_t lighting_group_table_entries = 0;
    size_t light_source_table_entries = 0;
    serialize_lighting_data_into_buf(&content, &lighting_group_table_entries,
                                     &light_source_table_entries);

    size_t entity_table_entries = 0;
    serialize_entity_data_into_buf(&content, &entity_table_entries);

    SceneFileHeader header = {
        .magic = SCENE_FILE_MAGIC,
        .version = SCENE_FILE_VERSION,
        .asset_table_entries = asset_table_entries,
        .lighting_group_table_entries = lighting_group_table_entries,
        .light_source_table_entries = light_source_table_entries,
        .entity_table_entries = entity_table_entries,
    };

    fwrite(&header, (sizeof header), 1, fp);
    fwrite(content.data, content.data_size, 1, fp);
    genbuf_free(&content);
}

int scene_file_load(FILE *fp) {
    printf("INFO: loading scene file.\n");

    fseek(fp, 0, SEEK_END);
    size_t file_size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    uint8_t *file_buffer = malloc(file_size + 1);
    assert(file_buffer);

    SceneFileHeader *header = (SceneFileHeader *)file_buffer;
    if (header->magic != SCENE_FILE_MAGIC) {
        printf("ERROR: Not a noble scene file\n");
        return 1;
    };
    return 0;
}
