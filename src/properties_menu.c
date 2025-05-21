#include "properties_menu.h"

#include "lighting.h"
#include "lighting_edit.h"
#include "raygui.h"
#include "settings.h"
#include "terrain.h"
#include "ui.h"
#include <stdint.h>
#include <stdio.h>

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

static inline void render_scene_properties(void) {

    uint16_t title_height = 20;
    uint16_t picker_height = 120;
    uint16_t spacing_height = 10;

    Rectangle title_rect = ui_properties_menu_reserve_height(title_height);
    GuiLabel(title_rect, "Scene");

    // Color picker
    Color before = lighting_scene_get_ambient_color();
    Rectangle color_picker_rect =
        ui_properties_menu_reserve_height(picker_height);
    color_picker_rect.width -= 32;
    color_picker_rect.x += 2;
    color_picker_rect.y += 4;
    Color after = before;
    GuiColorPicker(color_picker_rect, 0, &after);

    if (before.r != after.r || before.b != after.b || before.g != after.g ||
        before.a != after.a) {
        lighting_scene_set_ambient_color(after);
        lighting_shader_data_update();
    }

    ui_properties_menu_reserve_height(spacing_height);

    ui_properties_menu_reserve_section_end();
}

// Returns 1 if terrain was resized.
static inline int render_terrain_properties(void) {
    if (settings.mode != MODE_TERRAIN)
        return 0;

    uint16_t title_height = 20;
    uint16_t slider_height = 40;

    Rectangle title_rect = ui_properties_menu_reserve_height(title_height);
    GuiLabel(title_rect, "Terrain");

    static float width = 0;
    if (width == 0)
        width = terrain.width / 2;

    slider_input("Size", &width, 1.0, 100.0, slider_height);

    Rectangle value_text_rect = ui_properties_menu_reserve_height(20 - 6);
    value_text_rect.width -= 4;
    value_text_rect.y -= 4;
    value_text_rect.x += 2;
    char width_value_str[10] = {0};
    snprintf(width_value_str, ARRAY_LENGTH(width_value_str) - 1, "%u",
             (uint32_t)quantize(width, 1.0) * 2);
    GuiLabel(value_text_rect, width_value_str);

    width = quantize(width, 1.0);
    int resized = 0;
    if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
        terrain_resize((uint32_t)width * 2);
        resized = 1;
    }

    ui_properties_menu_reserve_section_end();
    return resized;
}

static inline void render_light_properties(void) {
    if (!lighting_edit_state.is_light_selected ||
        settings.mode != MODE_LIGHTING)
        return;

    LightSource *light =
        lighting_scene_get_light(lighting_edit_state.currently_selected_light);
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

PropertiesMenuEvent properties_menu_render(void) {
    render_light_properties();
    if (render_terrain_properties())
        return PROPERTIES_EVENT_TERRAIN_RESIZE;
    render_scene_properties();

    return PROPERTIES_EVENT_NONE;
}
