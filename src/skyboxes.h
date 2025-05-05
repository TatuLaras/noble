#ifndef _SKYBOXES
#define _SKYBOXES

// Functions for handling skyboxes.

#include "handles.h"
#include "string_vector.h"
#include <stddef.h>

extern StringVector skybox_list;

// Gets the list of all skyboxes from the skybox directory to the skybox list.
void skyboxes_fetch_all(void);
char *skyboxes_get_name(SkyboxHandle handle);
// Writes handle of skybox `name` into `out_handle`. Returns 1 if there is no
// such skybox.
int skyboxes_get_handle(const char *name, SkyboxHandle *out_handle);
size_t skyboxes_get_count(void);

#endif
