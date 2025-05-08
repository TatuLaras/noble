#include "properties_menu.h"

#include "lighting.h"
#include "lighting_edit.h"
#include "raygui.h"
#include "ui.h"
#include <stdint.h>

static inline void render_scene_properties(void) {

    uint16_t title_height = 20;
    uint16_t picker_height = 120;
    uint16_t spacing_height = 10;

    LightingGroup *lighting_group =
        lighting_scene_get_group(lighting_edit_state.current_group);

    Rectangle title_rect = ui_properties_menu_reserve_height(title_height);
    GuiLabel(title_rect, "Scene");

    // Color picker
    Color before = lighting_group->ambient_color;
    Rectangle color_picker_rect =
        ui_properties_menu_reserve_height(picker_height);
    color_picker_rect.width -= 32;
    color_picker_rect.x += 2;
    color_picker_rect.y += 4;
    GuiColorPicker(color_picker_rect, 0, &lighting_group->ambient_color);
    Color after = lighting_group->ambient_color;

    if (before.r != after.r || before.b != after.b || before.g != after.g ||
        before.a != after.a)
        update_shader_data(lighting_edit_state.current_group);

    ui_properties_menu_reserve_height(spacing_height);

    ui_properties_menu_reserve_section_end();
}

void properties_menu_render(void) {
    render_scene_properties();
}
