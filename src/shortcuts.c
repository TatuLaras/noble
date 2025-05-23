#include "shortcuts.h"

#include "common.h"
#include "settings.h"
#include <raylib.h>
#include <stddef.h>

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
    int is_global;
    Mode mode;
} Shortcut;

typedef struct {
    KeyboardKey keypresses[4];
    uint8_t keypresses_stored;
} ShortcutBuffer;
ShortcutBuffer shortcut_buffer = {0};

// Exclusive to default mode (MODE_DEFAULT) if not specified otherwise
static Shortcut shortcuts[] = {
    {
        .action = ACTION_TOGGLE_GRID,
        .keypresses = {KEY_P},
        .is_global = 1,
    },
    {
        .action = ACTION_TOGGLE_GIZMOS,
        .keypresses = {KEY_O},
        .is_global = 1,
    },
    {
        .action = ACTION_TOGGLE_QUANTIZE,
        .keypresses = {KEY_Q},
        .is_global = 1,
    },
    {
        .action = ACTION_TOGGLE_LIGHTING,
        .keypresses = {KEY_U},
        .is_global = 1,
    },
    {.action = ACTION_OBJECT_DELETE, .keypresses = {KEY_DELETE}},
    {.action = ACTION_OBJECT_DELETE, .keypresses = {KEY_BACKSPACE}},
    {
        .action = ACTION_LIGHT_TOGGLE_ENABLED,
        .keypresses = {KEY_DELETE},
        .mode = MODE_LIGHTING,
    },
    {
        .action = ACTION_LIGHT_TOGGLE_ENABLED,
        .keypresses = {KEY_BACKSPACE},
        .mode = MODE_LIGHTING,
    },
    {.action = ACTION_START_PICKING_ASSET, .keypresses = {KEY_SPACE}},
    {
        .action = ACTION_START_PICKING_TERRAIN_TEXTURE,
        .keypresses = {KEY_SPACE},
        .mode = MODE_TERRAIN,
    },
    {
        .action = ACTION_START_PICKING_SKYBOX,
        .keypresses = {KEY_F10},
        .is_global = 1,
    },
    {
        .action = ACTION_OBJECT_START_TRANSLATE_X,
        .keypresses = {KEY_G, KEY_X},
        .is_global = 1,
    },
    {
        .action = ACTION_OBJECT_START_TRANSLATE_Y,
        .keypresses = {KEY_G, KEY_Y},
        .is_global = 1,
    },
    {
        .action = ACTION_OBJECT_START_TRANSLATE_Z,
        .keypresses = {KEY_G, KEY_Z},
        .is_global = 1,
    },
    {
        .action = ACTION_OBJECT_START_ROTATE_X,
        .keypresses = {KEY_R, KEY_X},
        .is_global = 1,
    },
    {
        .action = ACTION_OBJECT_START_ROTATE_Y,
        .keypresses = {KEY_R, KEY_Y},
        .is_global = 1,
    },
    {
        .action = ACTION_OBJECT_START_ROTATE_Z,
        .keypresses = {KEY_R, KEY_Z},
        .is_global = 1,
    },
    {
        .action = ACTION_CHANGE_AXIS_X,
        .keypresses = {KEY_X},
        .is_global = 1,
    },
    {
        .action = ACTION_CHANGE_AXIS_Y,
        .keypresses = {KEY_Y},
        .is_global = 1,
    },
    {
        .action = ACTION_CHANGE_AXIS_Z,
        .keypresses = {KEY_Z},
        .is_global = 1,
    },
    {.action = ACTION_ASSET_SLOT_1, .keypresses = {KEY_ONE}},
    {.action = ACTION_ASSET_SLOT_2, .keypresses = {KEY_TWO}},
    {.action = ACTION_ASSET_SLOT_3, .keypresses = {KEY_THREE}},
    {.action = ACTION_ASSET_SLOT_4, .keypresses = {KEY_FOUR}},
    {.action = ACTION_ASSET_SLOT_5, .keypresses = {KEY_FIVE}},
    {.action = ACTION_ASSET_SLOT_6, .keypresses = {KEY_SIX}},
    {.action = ACTION_ASSET_SLOT_7, .keypresses = {KEY_SEVEN}},
    {.action = ACTION_ASSET_SLOT_8, .keypresses = {KEY_EIGHT}},
    {.action = ACTION_ASSET_SLOT_9, .keypresses = {KEY_NINE}},
    {.action = ACTION_ASSET_SLOT_10, .keypresses = {KEY_ZERO}},
    {
        .action = ACTION_TERRAIN_SLOT_1,
        .keypresses = {KEY_ONE},
        .mode = MODE_TERRAIN,
    },
    {
        .action = ACTION_TERRAIN_SLOT_2,
        .keypresses = {KEY_TWO},
        .mode = MODE_TERRAIN,
    },
    {
        .action = ACTION_TERRAIN_SLOT_3,
        .keypresses = {KEY_THREE},
        .mode = MODE_TERRAIN,
    },
    {
        .action = ACTION_TERRAIN_SLOT_4,
        .keypresses = {KEY_FOUR},
        .mode = MODE_TERRAIN,
    },
    {
        .action = ACTION_TERRAIN_SLOT_5,
        .keypresses = {KEY_FIVE},
        .mode = MODE_TERRAIN,
    },
    {
        .action = ACTION_TERRAIN_SLOT_6,
        .keypresses = {KEY_SIX},
        .mode = MODE_TERRAIN,
    },
    {
        .action = ACTION_TERRAIN_SLOT_7,
        .keypresses = {KEY_SEVEN},
        .mode = MODE_TERRAIN,
    },
    {
        .action = ACTION_TERRAIN_TOOL_RAISE,
        .keypresses = {KEY_J},
        .mode = MODE_TERRAIN,
    },
    {
        .action = ACTION_TERRAIN_TOOL_RAISE_SMOOTH,
        .keypresses = {KEY_K},
        .mode = MODE_TERRAIN,
    },
    {
        .action = ACTION_TERRAIN_TOOL_SET,
        .keypresses = {KEY_L},
        .mode = MODE_TERRAIN,
    },
    {
        .action = ACTION_TOGGLE_DEBUG_INFO,
        .keypresses = {KEY_F11},
        .is_global = 1,
    },
    {
        .action = ACTION_TOGGLE_PROPERTIES_MENU,
        .keypresses = {KEY_N},
        .is_global = 1,
    },
    {
        .action = ACTION_CAMERA_FOCUS_SELECTED,
        .keypresses = {KEY_F},
        .is_global = 1,
    },
    {.action = ACTION_CAMERA_RESET, .keypresses = {KEY_B}, .is_global = 1},
    {.action = ACTION_SET_MODE_NORMAL, .keypresses = {KEY_A}, .is_global = 1},
    {.action = ACTION_SET_MODE_LIGHTING, .keypresses = {KEY_S}, .is_global = 1},
    {.action = ACTION_SET_MODE_TERRAIN, .keypresses = {KEY_D}, .is_global = 1},
    {
        .action = ACTION_TOGGLE_FPS_CONTROLS,
        .keypresses = {KEY_F},
        .require_shift_down = 1,
        .is_global = 1,
    },
    {
        .action = ACTION_TOGGLE_RAYCAST_OBJECT_IGNORE,
        .keypresses = {KEY_I},
    },
    {
        .action = ACTION_GRID_RESET,
        .keypresses = {KEY_C},
        .require_alt_down = 1,
        .is_global = 1,
    },
    {
        .action = ACTION_PICK_ASSET_FROM_SELECTED_ENTITY,
        .keypresses = {KEY_Q},
        .require_ctrl_down = 1,
    },
    {
        .action = ACTION_SAVE_SCENE,
        .keypresses = {KEY_S},
        .require_ctrl_down = 1,
        .is_global = 1,
    },
};

// Resets the list of keypresses.
static void flush_keypress_buffer(void) {
    shortcut_buffer.keypresses_stored = 0;
}

// Search the list of shortcuts and return match if found, flush keypress buffer
// if not even a partial (starts with) match is found.
static Shortcut get_matching_shortcut(int shift_down, int ctrl_down,
                                      int alt_down) {
    int partial_match_exists = 0;
    for (size_t i = 0; i < ARRAY_LENGTH(shortcuts); i++) {

        if ((shift_down != shortcuts[i].require_shift_down) ||
            (ctrl_down != shortcuts[i].require_ctrl_down) ||
            (alt_down != shortcuts[i].require_alt_down) ||
            (!shortcuts[i].is_global && shortcuts[i].mode != settings.mode))
            continue;

        ShortcutMatchType match_type = MATCH_FULL;
        size_t j = 0;
        while (j < 4 && shortcuts[i].keypresses[j]) {
            if (j >= shortcut_buffer.keypresses_stored) {
                match_type = MATCH_PARTIAL;
                break;
            }

            if (shortcuts[i].keypresses[j] != shortcut_buffer.keypresses[j]) {
                match_type = MATCH_NONE;
                break;
            }

            j++;
        }

        if (match_type == MATCH_FULL)
            return shortcuts[i];
        if (match_type == MATCH_PARTIAL)
            partial_match_exists = 1;
    }

    if (!partial_match_exists)
        flush_keypress_buffer();

    return (Shortcut){0};
}

// Adds a key to the list of keypresses.
static void append_key(KeyboardKey key) {
    if (shortcut_buffer.keypresses_stored >= 4)
        return;

    shortcut_buffer.keypresses[shortcut_buffer.keypresses_stored++] = key;
}

ShortcutAction shortcuts_get_action(KeyboardKey key, int shift_down,
                                    int ctrl_down, int alt_down) {
    if (key == KEY_NULL)
        return ACTION_NONE;

    if (key == KEY_ESCAPE) {
        flush_keypress_buffer();
        return ACTION_NONE;
    }

    append_key(key);

    Shortcut active_shortcut =
        get_matching_shortcut(shift_down, ctrl_down, alt_down);

    if (active_shortcut.action == ACTION_NONE)
        return ACTION_NONE;

    flush_keypress_buffer();

    return active_shortcut.action;
}

int shortcuts_waiting_for_keypress(void) {
    return shortcut_buffer.keypresses_stored > 0;
}
