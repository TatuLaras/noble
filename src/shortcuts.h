#ifndef _SHORTCUTS
#define _SHORTCUTS

// A shortcut handling module to handle multi-key sequential key combinations
// easier.

#include <raylib.h>
#include <stdint.h>

typedef enum {
    ACTION_NONE,
    ACTION_TOGGLE_FPS_CONTROLS,
    ACTION_TOGGLE_GRID,
    ACTION_TOGGLE_GIZMOS,
    ACTION_TOGGLE_QUANTIZE,
    ACTION_TOGGLE_ADDING_RAYCAST_INCLUDE_OBJECTS,
    ACTION_TOGGLE_TILE_MODE,
    ACTION_TOGGLE_LIGHTING_EDIT_MODE,
    ACTION_TOGGLE_LIGHTING,
    ACTION_TOGGLE_SELECTED_ENTITY_LIGHTING,
    ACTION_GRID_RESET,
    ACTION_OBJECT_DELETE,
    ACTION_START_PICKING_ASSET,
    ACTION_OBJECT_START_TRANSLATE_X,
    ACTION_OBJECT_START_TRANSLATE_Y,
    ACTION_OBJECT_START_TRANSLATE_Z,
    ACTION_OBJECT_START_ROTATE_X,
    ACTION_OBJECT_START_ROTATE_Y,
    ACTION_OBJECT_START_ROTATE_Z,
    ACTION_CHANGE_AXIS_X,
    ACTION_CHANGE_AXIS_Y,
    ACTION_CHANGE_AXIS_Z,
} ShortcutAction;

typedef enum {
    MATCH_NONE,
    MATCH_FULL,
    MATCH_PARTIAL,
} ShortcutMatchType;

typedef struct {
    ShortcutAction action;
    KeyboardKey keypresses[4];
    int require_shift_down;
    int require_ctrl_down;
    int require_alt_down;
} Shortcut;

typedef struct {
    KeyboardKey keypresses[4];
    uint8_t keypresses_stored;
} ShortcutBuffer;

extern ShortcutBuffer shortcut_buffer;

// Registers the currently pressed key and if an action has taken place it
// returns that, otherwise returns `ACTION_NONE`.
ShortcutAction shortcutbuf_get_action(KeyboardKey key, int shift_down,
                                      int ctrl_down, int alt_down);

#endif
