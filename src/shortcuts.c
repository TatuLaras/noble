#include "shortcuts.h"
#include "common.h"
#include <raylib.h>
#include <stddef.h>
#include <stdio.h>

static Shortcut shortcuts[] = {
    {
        .action = ACTION_TOGGLE_GRID,
        .keypresses = {KEY_P},
    },
    {
        .action = ACTION_TOGGLE_GIZMOS,
        .keypresses = {KEY_O},
    },
    {
        .action = ACTION_TOGGLE_QUANTIZE,
        .keypresses = {KEY_Q},
    },
    {
        .action = ACTION_TOGGLE_ADDING_RAYCAST_INCLUDE_OBJECTS,
        .keypresses = {KEY_I},
    },
    {
        .action = ACTION_GRID_RESET,
        .keypresses = {KEY_Z},
        .require_shift_down = 1,
    },
    {
        .action = ACTION_OBJECT_DELETE,
        .keypresses = {KEY_X},
    },
    {
        .action = ACTION_START_PICKING_ASSET,
        .keypresses = {KEY_F},
    },
    {
        .action = ACTION_OBJECT_START_TRANSLATE_X,
        .keypresses = {KEY_G, KEY_X},
    },
    {
        .action = ACTION_OBJECT_START_TRANSLATE_Y,
        .keypresses = {KEY_G, KEY_Y},
    },
    {
        .action = ACTION_OBJECT_START_TRANSLATE_Z,
        .keypresses = {KEY_G, KEY_Z},
    },
    {
        .action = ACTION_OBJECT_START_ROTATE_X,
        .keypresses = {KEY_R, KEY_X},
    },
    {
        .action = ACTION_OBJECT_START_ROTATE_Y,
        .keypresses = {KEY_R, KEY_Y},
    },
    {
        .action = ACTION_OBJECT_START_ROTATE_Z,
        .keypresses = {KEY_R, KEY_Z},
    },
};

// Resets the list of keypresses.
static void flush_keypress_buffer(ShortcutBuffer *buf) {
    buf->keypresses_stored = 0;
}

// Search the list of shortcuts and return match if found, flush keypress buffer
// if not even a partial (starts with) match is found.
static Shortcut get_matching_shortcut(ShortcutBuffer *buf, int shift_down,
                                      int ctrl_down, int alt_down) {
    int partial_match_exists = 0;
    for (size_t i = 0; i < ARRAY_LENGTH(shortcuts); i++) {

        if ((shift_down != shortcuts[i].require_shift_down) ||
            (ctrl_down != shortcuts[i].require_ctrl_down) ||
            (alt_down != shortcuts[i].require_alt_down))
            continue;

        ShortcutMatchType match_type = MATCH_FULL;
        size_t j = 0;
        while (j < 4 && shortcuts[i].keypresses[j]) {
            if (j >= buf->keypresses_stored) {
                match_type = MATCH_PARTIAL;
                break;
            }

            if (shortcuts[i].keypresses[j] != buf->keypresses[j]) {
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
        flush_keypress_buffer(buf);

    return (Shortcut){0};
}

// Adds a key to the list of keypresses.
static void append_key(ShortcutBuffer *buf, KeyboardKey key) {
    if (buf->keypresses_stored >= 4)
        return;

    buf->keypresses[buf->keypresses_stored++] = key;
}

ShortcutAction shortcutbuf_get_action(ShortcutBuffer *buf, KeyboardKey key,
                                      int shift_down, int ctrl_down,
                                      int alt_down) {
    if (key == KEY_NULL)
        return ACTION_NONE;

    if (key == KEY_ESCAPE) {
        flush_keypress_buffer(buf);
        return ACTION_NONE;
    }

    append_key(buf, key);

    Shortcut active_shortcut =
        get_matching_shortcut(buf, shift_down, ctrl_down, alt_down);

    if (active_shortcut.action == ACTION_NONE)
        return ACTION_NONE;

    flush_keypress_buffer(buf);

    // return ACTION_NONE;

    return active_shortcut.action;
}
