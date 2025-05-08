#ifndef _ASSET_PICKER
#define _ASSET_PICKER

// Logic for a searchable menu of available assets.

#include <raylib.h>
#include <stddef.h>
#include <stdint.h>

#define ASSET_PICKER_MATCHES_MAX_COUNT 6

typedef enum {
    PICKER_MODE_ASSET,
    PICKER_MODE_SKYBOX,
} AssetPickerMode;

typedef struct {
    int picking_asset;
    char search_query[100];
    size_t search_query_used;
    size_t matches[ASSET_PICKER_MATCHES_MAX_COUNT];
    uint8_t matches_used;
    size_t selected_match;
    AssetPickerMode mode;
} AssetPickerState;

extern AssetPickerState asset_picker;

// Start a new search.
void asset_picker_start_search(AssetPickerMode mode);
// Input a new key to the search, can be a normal letter in which case it gets
// appended to the search query, or it can be a special hotkey.
void asset_picker_input_key(KeyboardKey key, int ctrl_down);
// Stops the search and sets the currently selected option as the current asset.
void asset_picker_select_current_option(void);

// Gets the matches for the search query as a newline-separated string.
char *asset_picker_get_newline_separated_matches(void);

#endif
