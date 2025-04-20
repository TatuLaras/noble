#ifndef _ASSET_PICKER
#define _ASSET_PICKER

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

void asset_picker_start_search(AssetPickerState *picker);
void asset_picker_stop_search(AssetPickerState *picker);
void asset_picker_input_key(AssetPickerState *picker, KeyboardKey key,
                            int ctrl_down);
void asset_picker_next_match(AssetPickerState *picker);
void asset_picker_previous_match(AssetPickerState *picker);

#endif
