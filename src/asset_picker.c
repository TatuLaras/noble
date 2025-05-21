#include "asset_picker.h"

#include "assets.h"
#include "common.h"
#include "handles.h"
#include "scene.h"
#include "settings.h"
#include "skyboxes.h"
#include "string_vector.h"
#include "terrain_textures.h"
#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

AssetPickerState asset_picker = {0};

static char newline_separated_matches[4096] = {0};
static int newline_separated_matches_valid = 0;

static inline StringVector *get_list(void) {
    switch (asset_picker.mode) {
    case PICKER_MODE_ASSET:
        return &asset_list;
    case PICKER_MODE_SKYBOX:
        return &skybox_list;
    case PICKER_MODE_TERRAIN_TEXTURE:
        return &terrain_texture_list;
        break;
    }
    return 0;
}

static inline void update_matches(void) {
    newline_separated_matches_valid = 0;
    asset_picker.selected_match = 0;

    StringVector *list = get_list();

    asset_picker.search_query[asset_picker.search_query_used] = 0;
    asset_picker.matches_used = 0;
    char *candidate = 0;
    AssetHandle i = 0;
    while ((candidate = stringvec_get(list, i++)) &&
           asset_picker.matches_used < ASSET_PICKER_MATCHES_MAX_COUNT) {

        // Not a match
        if (!strstr(candidate, asset_picker.search_query))
            continue;

        asset_picker.matches[asset_picker.matches_used++] = i - 1;
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
        min(ASSET_PICKER_MATCHES_MAX_COUNT, asset_picker.matches_used))
        asset_picker.selected_match++;
}

static void asset_picker_previous_match(void) {
    if (asset_picker.selected_match > 0)
        asset_picker.selected_match--;
}

static void stop_search(void) {
    asset_picker.search_query_used = 0;
    asset_picker.picking_asset = 0;
}

void asset_picker_start_search(AssetPickerMode mode) {
    asset_picker.search_query_used = 0;
    asset_picker.picking_asset = 1;
    asset_picker.mode = mode;
    update_matches();
}

void asset_picker_select_current_option(void) {
    if (asset_picker.selected_match < asset_picker.matches_used) {
        size_t handle = asset_picker.matches[asset_picker.selected_match];

        switch (asset_picker.mode) {
        case PICKER_MODE_ASSET:
            settings.selected_assets[settings.current_asset_slot] = handle;
            break;
        case PICKER_MODE_SKYBOX:
            scene_set_skybox(handle, settings.skybox_directory);
            break;
        case PICKER_MODE_TERRAIN_TEXTURE: {
            terrain_textures_load_into_slot(handle, settings.current_asset_slot,
                                            settings.terrain_texture_directory);
        } break;
        }
    }
    stop_search();
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

    if (key >= KEY_ZERO && key <= KEY_NINE) {
        append_character('0' + (key - KEY_ZERO));
        return;
    }

    if (key == KEY_BACKSPACE)
        erase_character();

    if (key == KEY_ESCAPE || key == KEY_CAPS_LOCK) {
        stop_search();
    }
}

char *asset_picker_get_newline_separated_matches(void) {
    if (newline_separated_matches_valid)
        return newline_separated_matches;

    StringVector *list = get_list();

    size_t pos = 0;
    for (AssetHandle i = 0; i < asset_picker.matches_used; i++) {
        char *name = stringvec_get(list, asset_picker.matches[i]);
        assert(name);

        for (size_t j = 0; name[j]; j++)
            newline_separated_matches[pos++] = name[j];
        newline_separated_matches[pos++] = '\n';
    }

    if (pos > 0)
        newline_separated_matches[pos - 1] = 0;
    else
        newline_separated_matches[pos] = 0;

    newline_separated_matches_valid = 1;
    return newline_separated_matches;
}
