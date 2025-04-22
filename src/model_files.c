#include "model_files.h"

#include "aseprite_texture.h"
#include "common.h"
#include "string_vector.h"
#include <raylib.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

static uint64_t file_last_modified(const char *filepath) {
    struct stat attr;
    if (stat(filepath, &attr))
        return 0;
    return attr.st_mtim.tv_sec;
}

// Replaces last three characters with "aseprite".
static char *path_get_corresponding_texture_file(const char *src) {
    size_t length = strlen(src);
    if (length < 3)
        return 0;
    size_t target_length = length + 6;

    char *destination = (char *)malloc(target_length);
    memcpy(destination, src, length);
    destination[target_length - 9] = 0;
    strcat(destination, "aseprite");

    return destination;
}

void try_load_corresponding_texture(const char *filepath, Model *model) {
    char *texture_file = path_get_corresponding_texture_file(filepath);

    Image *image = aseprite_load_as_image(texture_file);
    free(texture_file);
    if (!image)
        return;

    Texture2D texture = LoadTextureFromImage(*image);
    UnloadImage(*image);
    free(image);
    if (!texture.id)
        return;

    model->materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = texture;
}

uint64_t get_most_recent_file_modification(StringVector *model_filepaths) {
    size_t i = 0;
    uint64_t last_modification = 0;

    while (1) {
        char *model_filepath = stringvec_get(model_filepaths, i++);
        if (!model_filepath)
            break;

        char *texture_filepath =
            path_get_corresponding_texture_file(model_filepath);

        last_modification =
            max(last_modification, file_last_modified(model_filepath));

        if (!texture_filepath)
            continue;
        last_modification =
            max(last_modification, file_last_modified(texture_filepath));

        free(texture_filepath);
    }

    return last_modification;
}

// Loads model data from `model_filepaths` supplied and possible aseprite
// texture files of the same name.
// ModelVector load_model_data_from_files(StringVector *model_filepaths) {
//     ModelVector vec = modelvec_init();
//
//     size_t i = 0;
//     while (1) {
//         char *model_filepath = stringvec_get(model_filepaths, i++);
//         if (!model_filepath)
//             break;
//         Model model = LoadModel(model_filepath);
//
//         char *texture_filepath =
//             path_get_corresponding_texture_file(model_filepath);
//         try_load_corresponding_texture(texture_filepath, &model);
//         free(texture_filepath);
//
//         modelvec_append(&vec, model);
//     }
//     return vec;
// }
