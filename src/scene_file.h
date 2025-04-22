#ifndef _SCENE_FILE
#define _SCENE_FILE

// Functions for storing a scene as a file

#include "common.h"
#include <stdio.h>
#define SCENE_FILE_MAGIC 5325
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
    uint32_t version;
    uint64_t entities_count;
} SceneFileHeader;

typedef struct {
    char asset_identifier[NAME_MAX_LENGTH];
    int ignore_raycast;
    Matrix transform;
} SceneFileEntity;

void scene_file_store(FILE *fp);
Scene scene_file_load(FILE *fp);

#endif
