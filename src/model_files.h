#ifndef _MODEL_FILES
#define _MODEL_FILES

#include "string_vector.h"
#include <raylib.h>
#include <stdint.h>

// Loads an aseprite file corresponding to the model file `filepath` as a
// texture to `model` if one exists.
void try_load_corresponding_texture(const char *filepath, Model *model);
uint64_t get_most_recent_file_modification(StringVector *model_filepaths);
void load_aseprite_texture(const char *filepath, Model *model);

#endif
