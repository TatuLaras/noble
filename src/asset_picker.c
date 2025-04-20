#include "asset_picker.h"
#include "common.h"
#include "string_vector.h"
#include <raylib.h>
#include <stdio.h>
#include <string.h>

static inline void update_matches(AssetPickerState *picker) {
    picker->selected_match = 0;
    if (!picker->candidates.data)
        return;

    stringvec_free(&picker->matches);

    if (picker->search_query_used == 0) {
        picker->matches = stringvec_clone(&picker->candidates);
        return;
    }

    picker->matches = stringvec_init();

    char *candidate = 0;
    size_t i = 0;
    while ((candidate = stringvec_get(&picker->candidates, i++))) {
        int is_match = !strncmp(candidate, picker->search_query,
                                picker->search_query_used);
        if (is_match) {
            stringvec_append(&picker->matches, candidate, strlen(candidate));
            if (stringvec_count(&picker->matches) >=
                ASSET_PICKER_MATCHES_MAX_COUNT)
                break;
        }
    }
}

static inline void append_character(AssetPickerState *picker, char character) {
    if (picker->search_query_used >= ARRAY_LENGTH(picker->search_query))
        return;
    picker->search_query[picker->search_query_used++] = character;
    update_matches(picker);
}

static inline void erase_character(AssetPickerState *picker) {
    if (picker->search_query_used == 0)
        return;
    picker->search_query_used--;
    update_matches(picker);
}

void asset_picker_start_search(AssetPickerState *picker) {
    picker->search_query_used = 0;
    picker->picking_asset = 1;
    update_matches(picker);
}

void asset_picker_stop_search(AssetPickerState *picker) {
    picker->search_query_used = 0;
    picker->picking_asset = 0;
}

void asset_picker_input_key(AssetPickerState *picker, KeyboardKey key,
                            int ctrl_down) {
    if (key == KEY_NULL)
        return;

    if (key == KEY_N && ctrl_down) {
        asset_picker_next_match(picker);
        return;
    }
    if (key == KEY_P && ctrl_down) {
        asset_picker_previous_match(picker);
        return;
    }

    if (key >= KEY_A && key <= KEY_Z) {
        append_character(picker, 'a' + (key - KEY_A));
        return;
    }

    if (key == KEY_BACKSPACE)
        erase_character(picker);

    if (key == KEY_ESCAPE || key == KEY_CAPS_LOCK) {
        asset_picker_stop_search(picker);
    }
}

void asset_picker_next_match(AssetPickerState *picker) {
    if (picker->selected_match < min(ASSET_PICKER_MATCHES_MAX_COUNT,
                                     stringvec_count(&picker->matches) - 1))
        picker->selected_match++;
}

void asset_picker_previous_match(AssetPickerState *picker) {
    if (picker->selected_match > 0)
        picker->selected_match--;
}
