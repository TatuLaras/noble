#ifndef _ASSETS
#define _ASSETS

// Functions for handling assets.

#include "string_vector.h"
#include <stddef.h>

// Gets a list of .obj files as asset names (without .obj extension) from a
// directory. Non-recursive.
StringVector assets_get_list(const char *directory);

#endif
