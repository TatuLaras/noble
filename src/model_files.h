#ifndef _MODEL_FILES
#define _MODEL_FILES

#include "model_vector.h"
#include "string_vector.h"
#include <raylib.h>
#include <stdint.h>

void try_load_corresponding_texture(const char *filepath, Model *model);
uint64_t get_most_recent_file_modification(StringVector *model_filepaths);

#endif
