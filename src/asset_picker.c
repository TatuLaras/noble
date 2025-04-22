#include "asset_picker.h"
#include "common.h"
#include "string_vector.h"
#include <raylib.h>
#include <stdio.h>
#include <string.h>

AssetPickerState asset_picker = {0};

static inline void update_matches(void) {
    asset_picker.selected_match = 0;
    if (!asset_picker.candidates.data)
        return;

    stringvec_free(&asset_picker.matches);

    if (asset_picker.search_query_used == 0) {
        asset_picker.matches = stringvec_clone(&asset_picker.candidates);
        return;
    }

    asset_picker.matches = stringvec_init();

    char *candidate = 0;
    size_t i = 0;
    while ((candidate = stringvec_get(&asset_picker.candidates, i++))) {
        int is_match = !strncmp(candidate, asset_picker.search_query,
                                asset_picker.search_query_used);
        if (is_match) {
            stringvec_append(&asset_picker.matches, candidate,
                             strlen(candidate));
            if (stringvec_count(&asset_picker.matches) >=
                ASSET_PICKER_MATCHES_MAX_COUNT)
                break;
        }
    }
}

static inline void append_character(char character) {
    if (asset_picker.search_query_used >=
        ARRAY_LENGTH(asset_picker.search_query))
        return;
    asset_picker.search_query[asset_picker.search_query_used++] = character;
    update_matches();
}

static inline void erase_character(void) {
    if (asset_picker.search_query_used == 0)
        return;
    asset_picker.search_query_used--;
    update_matches();
}

static void asset_picker_next_match(void) {
    if (asset_picker.selected_match <
        min(ASSET_PICKER_MATCHES_MAX_COUNT,
            stringvec_count(&asset_picker.matches) - 1))
        asset_picker.selected_match++;
}

static void asset_picker_previous_match(void) {
    if (asset_picker.selected_match > 0)
        asset_picker.selected_match--;
}

void asset_picker_start_search(void) {
    asset_picker.search_query_used = 0;
    asset_picker.picking_asset = 1;
    update_matches();
}

void asset_picker_stop_search(void) {
    asset_picker.search_query_used = 0;
    asset_picker.picking_asset = 0;
}

void asset_picker_input_key(KeyboardKey key, int ctrl_down) {
    if (key == KEY_NULL)
        return;

    if (key == KEY_N && ctrl_down) {
        asset_picker_next_match();
        return;
    }
    if (key == KEY_P && ctrl_down) {
        asset_picker_previous_match();
        return;
    }

    if (key >= KEY_A && key <= KEY_Z) {
        append_character('a' + (key - KEY_A));
        return;
    }

    if (key == KEY_BACKSPACE)
        erase_character();

    if (key == KEY_ESCAPE || key == KEY_CAPS_LOCK) {
        asset_picker_stop_search();
    }
}
