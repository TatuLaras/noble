#ifndef _UI
#define _UI

#include "asset_picker.h"
#include <stdint.h>

void ui_init(void);

// Renders the user interface. Uses raylib drawing functions, must be used
// within raylib drawing code.
void ui_render(uint16_t screen_width, uint16_t screen_height,
               AssetPickerState *picker);

#endif
