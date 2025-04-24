#include "selection.h"

#include "scene.h"
#include <assert.h>

EntitySelection entity_selection_state = {0};

void selection_select_entity(EntityHandle handle) {
    entity_selection_state.handle = handle;
    entity_selection_state.is_entity_selected = 1;
}

void selection_deselect_all(void) {
    entity_selection_state.is_entity_selected = 0;
}

LiveEntity *selection_get_selected_entity(void) {
    if (!entity_selection_state.is_entity_selected)
        return 0;
    return scene_get_entity(entity_selection_state.handle);
}
