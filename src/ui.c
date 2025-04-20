#include "ui.h"
#include "asset_picker.h"
#include "common.h"
#include "string_vector.h"
#include <stdint.h>
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

void ui_init(void) {
    GuiLoadStyleCherry();
}

void ui_render(uint16_t screen_width, AssetPickerState *picker) {
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
