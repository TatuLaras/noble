#ifndef _GIZMOS
#define _GIZMOS

#include "handles.h"
#include <raylib.h>

void gizmos_render_transform_gizmo(Matrix transform);
void gizmos_render_light_gizmos(LightingGroupHandle handle, Camera *camera);
void gizmos_render_terrain_gizmos(void);
void gizmos_draw_grid(int slices, float spacing, Vector3 origin);

#endif
