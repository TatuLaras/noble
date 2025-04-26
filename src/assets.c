#include "assets.h"

#include "settings.h"
#include "string_vector.h"
#include <assert.h>
#include <dirent.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

StringVector asset_list = {0};

// Returns 1 if file ends in .obj.
static inline int is_obj(char *filename) {
    size_t length = strlen(filename);
    if (length < 4)
        return 0;

    int holds_true = 1;
    holds_true = holds_true && filename[length - 4] == '.';
    holds_true = holds_true && filename[length - 3] == 'o';
    holds_true = holds_true && filename[length - 2] == 'b';
    holds_true = holds_true && filename[length - 1] == 'j';

    return holds_true;
}

// Inserts a list of filenames (without extension) of all the .obj files in
// `directory` into the StringVector `destination`.
static inline void get_obj_basenames(const char *directory,
                                     StringVector *destination) {

    DIR *d;
    struct dirent *dir;
    d = opendir(directory);
    if (!d)
        return;

    while ((dir = readdir(d)) != NULL) {
        if (dir->d_type == DT_REG && is_obj(dir->d_name))
            // The "- 4" strips the .obj extension
            stringvec_append(destination, dir->d_name, strlen(dir->d_name) - 4);
    }

    closedir(d);

    return;
}

void assets_fetch_all(void) {
    if (!asset_list.data)
        asset_list = stringvec_init();

    if (*settings.asset_directory == 0)
        return;

    stringvec_truncate(&asset_list);
    get_obj_basenames(settings.asset_directory, &asset_list);
}

char *assets_get_name(AssetHandle handle) {
    return stringvec_get(&asset_list, handle);
}
