#include "lighting_edit.h"
#include "lighting.h"
#include "raycast.h"
#include "raygui.h"
#include "settings.h"
#include "transform.h"
#include "ui.h"
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
        .position = raycast_ground_intersection(ray),
        .color = (Color){0xff, 0xcc, 0x99, 0xff},
        .intensity = 6.0,
        .intensity_cap = 10.0,
    };

    light.position = settings_quantize_to_grid(light.position, 0);

    if (!lighting_group_add_light(lighting_edit_state.current_group, light,
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
        lighting_group_get_light(lighting_edit_state.current_group,
                                 lighting_edit_state.currently_selected_light);
    assert(light);
    light->is_disabled = !light->is_disabled;

    light_source_update(lighting_edit_state.current_group,
                        lighting_edit_state.currently_selected_light,
                        Vector3Zero());
}

void lighting_edit_adding_light_update(float delta_y) {
    LightSource *light =
        lighting_group_get_light(lighting_edit_state.current_group,
                                 lighting_edit_state.currently_added_light);
    if (!light)
        return;

    light->position.y += delta_y;

    light_source_update(lighting_edit_state.current_group,
                        lighting_edit_state.currently_added_light,
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
    while ((light = lighting_group_get_light(lighting_edit_state.current_group,
                                             i++))) {
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
        lighting_group_get_light(lighting_edit_state.current_group,
                                 lighting_edit_state.currently_selected_light);
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

static inline void slider_input(const char *label, float *property, float min,
                                float max, uint16_t height) {
    Rectangle slider_rect = ui_properties_menu_reserve_height(height);
    Rectangle label_rect = slider_rect;
    label_rect.height = 20;
    label_rect.x += 2;
    label_rect.width -= 4;
    slider_rect.height = 20;
    slider_rect.y += 20;
    slider_rect.x += 2;
    slider_rect.width -= 6;

    GuiLabel(label_rect, label);
    GuiSlider(slider_rect, 0, 0, property, min, max);
}

void lighting_edit_render_properties_menu(void) {
    if (!lighting_edit_state.is_light_selected || settings.mode == MODE_NORMAL)
        return;

    LightSource *light =
        lighting_group_get_light(lighting_edit_state.current_group,
                                 lighting_edit_state.currently_selected_light);
    if (!light)
        return;

    uint16_t title_height = 20;
    uint16_t picker_height = 120;
    uint16_t spacing_height = 0;
    uint16_t slider_height = 40;

    Rectangle title_rect = ui_properties_menu_reserve_height(title_height);
    GuiLabel(title_rect, "Light");

    // Color picker
    Rectangle color_picker_rect =
        ui_properties_menu_reserve_height(picker_height);
    color_picker_rect.width -= 32;
    color_picker_rect.x += 2;
    color_picker_rect.y += 4;
    GuiColorPicker(color_picker_rect, 0, &light->color);

    ui_properties_menu_reserve_height(spacing_height);

    slider_input("Intensity", &light->intensity, 0.0, 30.0, slider_height);
    slider_input("Intensity granular", &light->intensity_granular, 0.0, 2.0,
                 slider_height);
    slider_input("Intensity cap", &light->intensity_cap, 0.0, 10.0,
                 slider_height);

    ui_properties_menu_reserve_section_end();
}
