#ifndef _UI
#define _UI

// User interface implementation

#include <raylib.h>
#include <stdint.h>

void ui_init(void);

// Renders the user interface. Uses raylib drawing functions, must be used
// within raylib drawing code.
void ui_render(uint16_t screen_width, uint16_t screen_height);
// Reserves a space within the properties edit panel which the caller can then
// draw into.
Rectangle ui_properties_menu_reserve_height(uint16_t height);
void ui_properties_menu_reserve_spacer(void);
void ui_properties_menu_reserve_section_end(void);

// Resets the properties menu height allocations. Meant to be called at
// the end of every frame after all UI has been drawn (including property
// menus).
void ui_properties_menu_reset(void);
// Returns the rectangle of screen space the properties menu takes up in total.
Rectangle ui_properties_menu_get_rect(void);

#endif
