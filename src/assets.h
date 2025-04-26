#ifndef _ASSETS
#define _ASSETS

// Functions for handling assets.

#include "handles.h"
#include "string_vector.h"
#include <stddef.h>

extern StringVector asset_list;

// Gets the list of all assets from the asset directory (see settings.h) to the
// asset list.
void assets_fetch_all(void);
char *assets_get_name(AssetHandle handle);

#endif
