#include "selection.h"

#include "scene.h"
#include <assert.h>

void selection_entity_select(EntitySelection *selection, size_t id) {
    assert(selection);

    selection->entity_id = id;
    selection->selected = 1;
}

void selection_deselect_all(EntitySelection *selection) {
    assert(selection);

    selection->selected = 0;
}

LiveEntity *selection_get_selected_entity(Scene *scene,
                                          EntitySelection *selection) {
    assert(scene);
    assert(selection);

    if (!selection->selected)
        return 0;
    return scene_get(scene, selection->entity_id);
}
