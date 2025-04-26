#ifndef _SCENE_FILE
#define _SCENE_FILE

// Functions for storing a scene as a file

#include "common.h"
#include "lighting.h"
#include <stddef.h>
#include <stdio.h>

#define SCENE_FILE_MAGIC 0x1273
#define SCENE_FILE_VERSION 0

#include "scene.h"
#include <stdint.h>

/*
 * Scene file structure:
 * - Header, as described by `SceneFileHeader`
 * - Any amount of struct SceneFileEntity, count specified in header
 */
typedef struct {
    uint16_t reserved : 16;
} SceneFileFlags;

typedef struct {
    uint16_t magic;
    SceneFileFlags flags;
    uint16_t version;
    uint16_t lighting_group_table_entries;
    uint32_t asset_table_entries;

    // Empty space 4 bytes

    size_t light_source_table_entries;
    size_t entity_table_entries;
} SceneFileHeader;

typedef struct {
    char name[NAME_MAX_LENGTH];
} SceneFileAsset;

typedef struct {
    char name[NAME_MAX_LENGTH];
    Color ambient_color;
} SceneFileLightGroup;

typedef struct {
    Color color;
    uint8_t is_disabled;
    LightType type : 8;
    uint16_t light_group_index;
    float intensity;
    float intensity_granular;
    float intensity_cap;
    Vector3 position;
} SceneFileLightSource;

typedef struct {
    char name[NAME_MAX_LENGTH];
    Matrix transform;
    uint8_t ignore_raycast;
    uint8_t is_unlit;
} SceneFileEntity;

void scene_file_store(FILE *fp);
int scene_file_load(FILE *fp);

#endif
