#include "ui.h"

#include "asset_picker.h"
#include "assets.h"
#include "terrain_textures.h"
#include "common.h"
#include "settings.h"
#include "shortcuts.h"
#include "terrain_edit.h"
#include <assert.h>
#include <raylib.h>
#include <stdint.h>
#include <stdio.h>

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
#define PROPERTIES_PANEL_PADDING 8
#define PROPERTIES_PANEL_SPACING 2
static const Color PROPERTIES_BACKGROUND_COLOR = {0x75, 0x32, 0x33, 0xff};
static const Color PROPERTIES_BORDER_COLOR = {0xda, 0x57, 0x57, 0xff};

static uint16_t properties_menu_height = 0;

static inline void render_status_bar(Rectangle rect) {
    char status_string[256] = {0};

    char *item_name = "";
    char *submode = "";
    char *mode = "LIGHTING";

    if (settings.mode == MODE_NORMAL) {
        mode = "NORMAL";
        item_name = assets_get_name(
            settings.selected_assets[settings.current_asset_slot]);
    }

    if (settings.mode == MODE_TERRAIN) {
        mode = "TERRAIN";
        switch (terrain_edit_state.tool) {
        case TERRAIN_TOOL_RAISE:
            submode = "raise";
            break;
        case TERRAIN_TOOL_RAISE_SMOOTH:
            submode = "raise smooth";
            break;
        case TERRAIN_TOOL_SET:
            submode = "set";
            break;
        }

        item_name = terrain_textures_get_name(
            settings.selected_terrain_textures[settings.current_asset_slot]);
    }

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
             "%s %s  %u %s  |  o [%s]  p [%s]  q [%s]  i [%s]  s [%s]", mode,
             submode, settings.current_asset_slot + 1, item_name, gizmos, grid,
             snap, object_ignore, lighting);

    if (!shortcuts_waiting_for_keypress())
        GuiStatusBar(rect, status_string);
}

void ui_init(void) {
    GuiLoadStyleCherry();
}

void ui_render(uint16_t screen_width, uint16_t screen_height) {
    render_status_bar((Rectangle){0, screen_height - 20, screen_width, 20});

    if (!asset_picker.picking_asset)
        return;

    asset_picker.search_query[asset_picker.search_query_used] = '_';
    asset_picker.search_query[asset_picker.search_query_used + 1] = 0;

    uint32_t y = 0;
    GuiStatusBar((Rectangle){0, y, screen_width, 20},
                 asset_picker.search_query);
    y += 20;

    GuiListView((Rectangle){0, y, screen_width,
                            30 * ASSET_PICKER_MATCHES_MAX_COUNT + 4},
                asset_picker_get_newline_separated_matches(), 0,
                (int *)&asset_picker.selected_match);
    y += 200;
}

static inline void draw_vertical_border(Rectangle rect) {
    DrawLine(rect.x, rect.y, rect.x, rect.y + rect.height,
             PROPERTIES_BORDER_COLOR);
    DrawLine(rect.x + rect.width, rect.y, rect.x + rect.width,
             rect.y + rect.height, PROPERTIES_BORDER_COLOR);
}

Rectangle ui_properties_menu_reserve_height(uint16_t height) {
    if (properties_menu_height == 0)
        ui_properties_menu_reserve_spacer();

    Rectangle outer_rect = {
        1,
        properties_menu_height,
        PROPERTIES_PANEL_WIDTH + PROPERTIES_PANEL_PADDING * 2,
        height + PROPERTIES_PANEL_SPACING * 2,
    };
    DrawRectangleRec(outer_rect, PROPERTIES_BACKGROUND_COLOR);
    draw_vertical_border(outer_rect);

    Rectangle inner_rect = {1 + PROPERTIES_PANEL_PADDING,
                            properties_menu_height + PROPERTIES_PANEL_SPACING,
                            PROPERTIES_PANEL_WIDTH, height};
    properties_menu_height += outer_rect.height;
    return inner_rect;
}

void ui_properties_menu_reserve_section_end(void) {
    Rectangle padding = {1, properties_menu_height,
                         PROPERTIES_PANEL_WIDTH + PROPERTIES_PANEL_PADDING * 2,
                         PROPERTIES_PANEL_PADDING};
    DrawRectangleRec(padding, PROPERTIES_BACKGROUND_COLOR);
    draw_vertical_border(padding);
    properties_menu_height += padding.height;
    ui_properties_menu_reserve_spacer();
}

void ui_properties_menu_reserve_spacer(void) {
    DrawLine(0, properties_menu_height,
             PROPERTIES_PANEL_WIDTH + PROPERTIES_PANEL_PADDING * 2 + 1,
             properties_menu_height, PROPERTIES_BORDER_COLOR);
    properties_menu_height += 1;
}

void ui_properties_menu_reset(void) {
    properties_menu_height = 0;
}

Rectangle ui_properties_menu_get_rect(void) {
    return (Rectangle){1, 0, PROPERTIES_PANEL_WIDTH, properties_menu_height};
}
