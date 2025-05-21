#include "lighting_edit.h"
#include "editor.h"
#include "lighting.h"
#include "raygui.h"
#include "settings.h"
#include "transform.h"
#include <assert.h>
#include <raylib.h>
#include <raymath.h>
#include <stdint.h>

LightingEditState lighting_edit_state = {0};

static inline void select_light(LightSourceHandle handle) {
    lighting_edit_state.is_light_selected = 1;
    lighting_edit_state.currently_selected_light = handle;
}

static inline void deselect_lights(void) {
    lighting_edit_state.is_light_selected = 0;
}

int lighting_edit_add_light(Ray ray) {
    LightSource light = {
        .type = LIGHT_POINT,
        .position = editor_general_scene_raycast(ray, 0),
        .color = (Color){0xff, 0xcc, 0x99, 0xff},
        .intensity = 6.0,
        .intensity_cap = 10.0,
    };

    light.position = settings_quantize_to_grid(light.position, 0);

    if (!lighting_scene_add_light(light,
                                  &lighting_edit_state.currently_added_light)) {
        lighting_edit_state.is_light_added = 1;
        return 0;
    }
    return 1;
}

void lighting_edit_selected_light_toggle_enabled(void) {
    if (!lighting_edit_state.is_light_selected)
        return;

    LightSource *light =
        lighting_scene_get_light(lighting_edit_state.currently_selected_light);
    assert(light);
    light->is_disabled = !light->is_disabled;

    lighting_light_update(lighting_edit_state.currently_selected_light,
                          Vector3Zero());
}

void lighting_edit_adding_light_update(float delta_y) {
    LightSource *light =
        lighting_scene_get_light(lighting_edit_state.currently_added_light);
    if (!light)
        return;

    light->position.y += delta_y;

    lighting_light_update(lighting_edit_state.currently_added_light,
                          Vector3Zero());
}

void lighting_edit_adding_stop(void) {
    lighting_edit_state.is_light_added = 0;
    select_light(lighting_edit_state.currently_added_light);
}

void lighting_edit_select_light_at(Vector2 screen_position, Camera3D camera) {

    deselect_lights();

    LightSourceHandle i = 0;
    LightSource *light = 0;
    LightSource *selected = 0;
    while ((light = lighting_scene_get_light(i++))) {
        Vector2 light_screen_pos = GetWorldToScreen(light->position, camera);
        if (Vector2DistanceSqr(light_screen_pos, screen_position) >
            LIGHT_SELECT_RADIUS * LIGHT_SELECT_RADIUS)
            continue;

        if (!selected ||
            Vector3DistanceSqr(camera.position, selected->position) >
                Vector3DistanceSqr(camera.position, light->position)) {
            selected = light;
            select_light(i - 1);
        }
    }
}

void lighting_edit_transform_start(Axis axis) {
    // Apply previous if needed
    lighting_edit_transform_apply();

    transform_operation.mode = TRANSFORM_TRANSLATE;
    transform_operation.axis = axis;
    transform_operation.amount = 0;
}

void lighting_edit_transform_stop(void) {
    if (transform_operation.mode == TRANSFORM_NONE)
        return;

    lighting_edit_transform_apply();
    transform_operation.mode = TRANSFORM_NONE;
}

void lighting_edit_transform_cancel(void) {
    transform_operation.mode = TRANSFORM_NONE;
}

void lighting_edit_transform_apply(void) {
    if (!lighting_edit_state.is_light_selected)
        return;

    LightSource *light =
        lighting_scene_get_light(lighting_edit_state.currently_selected_light);
    if (!light || transform_operation.mode == TRANSFORM_NONE)
        return;

    light->position =
        Vector3Add(light->position, lighting_edit_transform_get_delta_vector());
}

Vector3 lighting_edit_transform_get_delta_vector(void) {
    if (transform_operation.mode == TRANSFORM_NONE)
        return Vector3Zero();

    switch (transform_operation.axis) {
    case AXIS_X:
        return (Vector3){transform_operation.amount, 0, 0};
    case AXIS_Y:
        return (Vector3){0, transform_operation.amount, 0};
    case AXIS_Z:
        return (Vector3){0, 0, transform_operation.amount};
    }
    return Vector3Zero();
}
