#ifndef _PROPERTIES_MENU
#define _PROPERTIES_MENU

typedef enum {
    PROPERTIES_EVENT_NONE,
    PROPERTIES_EVENT_TERRAIN_RESIZE,
} PropertiesMenuEvent;

PropertiesMenuEvent properties_menu_render(void);

#endif
