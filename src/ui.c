#include "ui.h"

#include "asset_picker.h"
#include "common.h"
#include "settings.h"
#include "string_vector.h"
#include <raylib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

// Ignore some warnings from external UI library
#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#endif

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#include "raygui_style_cherry.h"

#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif

#define PROPERTIES_PANEL_WIDTH 140

static uint16_t properties_menu_height = 0;

static inline void render_status_bar(Rectangle rect) {
    char status_string[256] = {0};

    char *asset = "no asset";
    if (settings.lighting_edit_mode_enabled)
        asset = "LIGHTING EDIT";
    else if (*settings.selected_asset)
        asset = settings.selected_asset;

    char *gizmos = "";
    if (settings.gizmos_enabled)
        gizmos = "gizmos";

    char *grid = "";
    if (settings.grid_enabled)
        grid = "grid";

    char *snap = "";
    if (settings.quantize_to_grid_enabled)
        snap = "snap";

    char *object_ignore = "";
    if (!settings.adding_raycast_include_objects)
        object_ignore = "objects ignore";

    char *lighting = "";
    if (settings.lighting_enabled)
        lighting = "shading";

    snprintf(status_string, ARRAY_LENGTH(status_string) - 1,
             "%s  |  o [%s]  p [%s]  q [%s]  i [%s]  s [%s]", asset, gizmos,
             grid, snap, object_ignore, lighting);

    GuiStatusBar(rect, status_string);
}

void ui_init(void) {
    GuiLoadStyleCherry();
}

void ui_render(uint16_t screen_width, uint16_t screen_height,
               AssetPickerState *picker) {
    render_status_bar((Rectangle){0, screen_height - 20, screen_width, 20});

    if (!picker->picking_asset)
        return;

    char current_search_query[ARRAY_LENGTH(picker->search_query)] = {0};
    strncpy(current_search_query, picker->search_query,
            picker->search_query_used);

    uint32_t y = 0;
    GuiStatusBar((Rectangle){0, y, screen_width, 20}, current_search_query);
    y += 20;

    static char newline_separated_matches[4096] = {0};
    stringvec_as_newline_separated(&picker->matches, newline_separated_matches,
                                   ARRAY_LENGTH(newline_separated_matches),
                                   ASSET_PICKER_MATCHES_MAX_COUNT);

    GuiListView((Rectangle){0, y, screen_width,
                            30 * ASSET_PICKER_MATCHES_MAX_COUNT + 4},
                newline_separated_matches, 0, (int *)&picker->selected_match);
    y += 200;
}

Rectangle ui_properties_menu_reserve_height(uint16_t height) {
    Rectangle rect = {0, properties_menu_height, PROPERTIES_PANEL_WIDTH,
                      height};
    properties_menu_height += height;
    return rect;
}

void ui_properties_menu_reset(void) {
    properties_menu_height = 0;
}

Rectangle ui_properties_menu_get_rect(void) {
    return (Rectangle){0, 0, PROPERTIES_PANEL_WIDTH, properties_menu_height};
}
