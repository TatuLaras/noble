#ifndef _ASSET_PICKER
#define _ASSET_PICKER

// Logic for a searchable menu of available assets.

#include "string_vector.h"
#include <raylib.h>
#include <stddef.h>

#define ASSET_PICKER_MATCHES_MAX_COUNT 6

typedef struct {
    int picking_asset;
    char search_query[100];
    size_t search_query_used;
    StringVector matches;
    StringVector candidates;
    size_t selected_match;
} AssetPickerState;

extern AssetPickerState asset_picker;

// Start a new search.
void asset_picker_start_search(void);
// Stop the current search.
void asset_picker_stop_search(void);
// Input a new key to the search, can be a normal letter in which case it gets
// appended to the search query, or it can be a special hotkey.
void asset_picker_input_key(KeyboardKey key, int ctrl_down);

#endif
