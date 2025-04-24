#ifndef _LIGHTING
#define _LIGHTING

#include "scene.h"
#include <raylib.h>
#include <stdalign.h>
#include <stddef.h>
#include <stdint.h>

#define LIGHTING_MAX_LIGHTS_PER_GROUP 16

typedef size_t LightingGroupHandle;
typedef size_t LightSourceHandle;

typedef enum {
    LIGHT_NULL,
    LIGHT_POINT,
    LIGHT_DIRECTIONAL,
} LightType;

typedef struct {
    int disabled;
    float intensity;
    float intensity_cap;
    LightType type;
    Vector3 position;
    Vector3 target;
    Color color;

    uint32_t enabled_location;
    uint32_t intensity_location;
    uint32_t intensity_cap_location;
    uint32_t type_location;
    uint32_t position_location;
    uint32_t target_location;
    uint32_t color_location;
} LightSource;

// A group of lights handled by one shader instance
typedef struct {
    LightSource lights[LIGHTING_MAX_LIGHTS_PER_GROUP];
    size_t lights_size;
    int is_deleted;
    int is_shading_disabled;
    Shader shader;
    Color ambient_color;

    uint32_t is_shading_disabled_location;
    uint32_t ambient_color_location;

} LightingGroup;

typedef struct {
    LightingGroup *groups;
    size_t groups_size;
    size_t groups_allocated;
} LightingScene;

extern LightingScene lighting_scene;
extern Shader unlit_shader;

void lighting_scene_init(void);
void lighting_scene_free(void);

// Creates new empty `LightingGroup` inside lighting scene.
LightingGroupHandle lighting_group_create(Color ambient_color);
// Adds a new entity to the lighting group so it will be affected by the light
// sources in that group.
void lighting_group_add_entity(LightingGroupHandle handle, LiveEntity *entity);
// Adds a new light source to the lighting group, allowing it to illuminate
// entities in that group.
int lighting_group_add_light(LightingGroupHandle handle, LightSource source,
                             LightSourceHandle *out_light_source_handle);
// Gets a pointer to a lightsource in `group` by `handle`. Returns 1 on
// out-of-bound `handle` s.
LightSource *lighting_group_get_light(LightingGroupHandle group_handle,
                                      LightSourceHandle source_handle);

// Disables or enables all shading in a lighting group.
void lighting_scene_set_enabled(uint32_t enabled);
// Disables or enables all shading in a lighting scene.
void lighting_group_set_enabled(LightingGroupHandle handle, uint32_t enabled);

// Updates a single light source's data in the lighting group shader. Returns 1
// on out-of-bound `handle` s.
int light_source_update(LightingGroupHandle group_handle,
                        LightSourceHandle light_handle);

// Get lighting group by handle.
LightingGroup *lighting_scene_get_group(LightingGroupHandle handle);

#endif
