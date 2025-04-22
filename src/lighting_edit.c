#include "lighting_edit.h"
#include "common.h"
#include "lighting.h"
#include "raycast.h"

int lighting_edit_add_light(LightingEditState *state,
                            LightingGroupHandle group_handle, Ray ray) {
    LightSource light = {
        .type = LIGHT_POINT,
        .position = raycast_ground_intersection(ray),
        .color = WHITE,
        .intensity = 4.0,
    };

    light.position = vector3_quantize(light.position);

    if (!lighting_group_add_light(group_handle, light,
                                  &state->currently_added_light)) {
        state->is_light_added = 1;
        return 0;
    }
    return 1;
}
