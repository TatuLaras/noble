#include "scene_file.h"
#include <stdio.h>

void scene_file_store(FILE *fp) {
    SceneFileHeader header = {.magic = SCENE_FILE_MAGIC,
                              .version = SCENE_FILE_VERSION};
    fwrite(&header, (sizeof header), 1, fp);
}

Scene scene_file_load(FILE *fp);
