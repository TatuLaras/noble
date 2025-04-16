#include "unity.h"

#include "shortcuts.h"
#include "unity_internals.h"
#include <raylib.h>

ShortcutBuffer buf = {0};

// In case they change
#define _KEY_GIZMOS KEY_O
#define _KEY_GRID KEY_P
#define _KEY_TRANSLATE KEY_G
#define _KEY_ROTATE KEY_R
#define _KEY_GRID_SIZE KEY_Z

void setUp(void) {}
void tearDown(void) {}

void test_single_key(void) {
    TEST_ASSERT_EQUAL(ACTION_TOGGLE_GIZMOS,
                      shortcutbuf_get_action(&buf, _KEY_GIZMOS, 0, 0, 0));
    TEST_ASSERT_EQUAL(ACTION_NONE,
                      shortcutbuf_get_action(&buf, _KEY_GIZMOS, 1, 0, 0));
    TEST_ASSERT_EQUAL(ACTION_NONE,
                      shortcutbuf_get_action(&buf, _KEY_GIZMOS, 0, 1, 1));
    TEST_ASSERT_EQUAL(ACTION_NONE,
                      shortcutbuf_get_action(&buf, _KEY_GIZMOS, 0, 0, 1));
    TEST_ASSERT_EQUAL(ACTION_NONE,
                      shortcutbuf_get_action(&buf, _KEY_GIZMOS, 0, 1, 0));
    TEST_ASSERT_EQUAL(ACTION_TOGGLE_GRID,
                      shortcutbuf_get_action(&buf, _KEY_GRID, 0, 0, 0));
    TEST_ASSERT_EQUAL(ACTION_TOGGLE_GRID,
                      shortcutbuf_get_action(&buf, _KEY_GRID, 0, 0, 0));
    TEST_ASSERT_EQUAL(ACTION_NONE,
                      shortcutbuf_get_action(&buf, KEY_W, 0, 0, 0));
}

void test_multi_key(void) {
    TEST_ASSERT_EQUAL(ACTION_NONE,
                      shortcutbuf_get_action(&buf, _KEY_TRANSLATE, 0, 0, 0));
    TEST_ASSERT_EQUAL(ACTION_OBJECT_START_TRANSLATE_X,
                      shortcutbuf_get_action(&buf, KEY_X, 0, 0, 0));
    TEST_ASSERT_EQUAL(ACTION_TOGGLE_GRID,
                      shortcutbuf_get_action(&buf, _KEY_GRID, 0, 0, 0));
    TEST_ASSERT_EQUAL(ACTION_NONE,
                      shortcutbuf_get_action(&buf, _KEY_TRANSLATE, 0, 0, 0));
    TEST_ASSERT_EQUAL(ACTION_NONE,
                      shortcutbuf_get_action(&buf, KEY_X, 1, 0, 0));
    TEST_ASSERT_EQUAL(ACTION_TOGGLE_GRID,
                      shortcutbuf_get_action(&buf, _KEY_GRID, 0, 0, 0));
    TEST_ASSERT_EQUAL(ACTION_NONE,
                      shortcutbuf_get_action(&buf, _KEY_TRANSLATE, 0, 0, 0));
    TEST_ASSERT_EQUAL(ACTION_OBJECT_START_TRANSLATE_Y,
                      shortcutbuf_get_action(&buf, KEY_Y, 0, 0, 0));
    TEST_ASSERT_EQUAL(ACTION_NONE,
                      shortcutbuf_get_action(&buf, _KEY_ROTATE, 0, 0, 0));
    TEST_ASSERT_EQUAL(ACTION_OBJECT_START_ROTATE_Z,
                      shortcutbuf_get_action(&buf, KEY_Z, 0, 0, 0));
    TEST_ASSERT_EQUAL(ACTION_TOGGLE_GRID,
                      shortcutbuf_get_action(&buf, _KEY_GRID, 0, 0, 0));
}

void test_non_matching(void) {
    TEST_ASSERT_EQUAL(ACTION_NONE,
                      shortcutbuf_get_action(&buf, _KEY_TRANSLATE, 0, 0, 0));
    TEST_ASSERT_EQUAL(ACTION_NONE,
                      shortcutbuf_get_action(&buf, _KEY_GRID, 0, 0, 0));
    TEST_ASSERT_EQUAL(ACTION_TOGGLE_GRID,
                      shortcutbuf_get_action(&buf, _KEY_GRID, 0, 0, 0));
}

void test_escape_cancel(void) {
    TEST_ASSERT_EQUAL(ACTION_NONE,
                      shortcutbuf_get_action(&buf, _KEY_TRANSLATE, 0, 0, 0));
    TEST_ASSERT_EQUAL(ACTION_NONE,
                      shortcutbuf_get_action(&buf, KEY_ESCAPE, 0, 0, 0));
    TEST_ASSERT_EQUAL(ACTION_TOGGLE_GRID,
                      shortcutbuf_get_action(&buf, _KEY_GRID, 0, 0, 0));
}

void test_modifier_keys(void) {
    TEST_ASSERT_EQUAL(ACTION_GRID_DENSITY_DECREASE,
                      shortcutbuf_get_action(&buf, _KEY_GRID_SIZE, 0, 0, 0));
    TEST_ASSERT_EQUAL(ACTION_NONE,
                      shortcutbuf_get_action(&buf, _KEY_GRID_SIZE, 0, 1, 0));
    TEST_ASSERT_EQUAL(ACTION_GRID_DENSITY_INCREASE,
                      shortcutbuf_get_action(&buf, _KEY_GRID_SIZE, 1, 0, 0));
    TEST_ASSERT_EQUAL(ACTION_TOGGLE_GRID,
                      shortcutbuf_get_action(&buf, _KEY_GRID, 0, 0, 0));
}

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_single_key);
    RUN_TEST(test_multi_key);
    RUN_TEST(test_non_matching);
    RUN_TEST(test_escape_cancel);
    RUN_TEST(test_modifier_keys);

    return UNITY_END();
}
