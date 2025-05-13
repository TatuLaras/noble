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
    ACTION_TOGGLE_RAYCAST_OBJECT_IGNORE,
    ACTION_TOGGLE_TILE_MODE,
    ACTION_TOGGLE_LIGHTING,
    ACTION_GRID_RESET,
    ACTION_OBJECT_DELETE,
    ACTION_LIGHT_TOGGLE_ENABLED,
    ACTION_START_PICKING_ASSET,
    ACTION_START_PICKING_SKYBOX,
    ACTION_OBJECT_START_TRANSLATE_X,
    ACTION_OBJECT_START_TRANSLATE_Y,
    ACTION_OBJECT_START_TRANSLATE_Z,
    ACTION_OBJECT_START_ROTATE_X,
    ACTION_OBJECT_START_ROTATE_Y,
    ACTION_OBJECT_START_ROTATE_Z,
    ACTION_CHANGE_AXIS_X,
    ACTION_CHANGE_AXIS_Y,
    ACTION_CHANGE_AXIS_Z,
    ACTION_ASSET_SLOT_1,
    ACTION_ASSET_SLOT_2,
    ACTION_ASSET_SLOT_3,
    ACTION_ASSET_SLOT_4,
    ACTION_ASSET_SLOT_5,
    ACTION_ASSET_SLOT_6,
    ACTION_ASSET_SLOT_7,
    ACTION_ASSET_SLOT_8,
    ACTION_ASSET_SLOT_9,
    ACTION_ASSET_SLOT_10,
    ACTION_TERRAIN_TOOL_1_RAISE,
    ACTION_TERRAIN_TOOL_2_RAISE_SMOOTH,
    ACTION_TERRAIN_TOOL_3_SET,
    ACTION_PICK_ASSET_FROM_SELECTED_ENTITY,
    ACTION_SAVE_SCENE,
    ACTION_TOGGLE_DEBUG_INFO,
    ACTION_TOGGLE_PROPERTIES_MENU,
    ACTION_CAMERA_RESET,
    ACTION_CAMERA_FOCUS_SELECTED,
    ACTION_SET_MODE_NORMAL,
    ACTION_SET_MODE_LIGHTING,
    ACTION_SET_MODE_TERRAIN,
} ShortcutAction;

// Registers the currently pressed key and if an action has taken place it
// returns that, otherwise returns `ACTION_NONE`.
ShortcutAction shortcuts_get_action(KeyboardKey key, int shift_down,
                                    int ctrl_down, int alt_down);
// Returns true if we are waiting for another keypress (for example 'g' has been
// pressed and either an axis or a cancellation is required).
int shortcuts_waiting_for_keypress(void);

#endif
