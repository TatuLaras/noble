#include "assets.h"

#include "common.h"
#include "string_vector.h"
#include <assert.h>
#include <dirent.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

// Checks if file ends in .obj
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

StringVector assets_get_list(const char *directory) {
    StringVector vec = stringvec_init();

    DIR *d;
    struct dirent *dir;
    d = opendir(directory);
    if (d) {
        while ((dir = readdir(d)) != NULL) {
            if (dir->d_type == DT_REG && is_obj(dir->d_name))
                // The "- 4" strips the .obj extension
                stringvec_append(&vec, dir->d_name, strlen(dir->d_name) - 4);
        }
        closedir(d);
    }

    return vec;
}
