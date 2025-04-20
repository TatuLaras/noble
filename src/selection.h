#ifndef _SELECTION
#define _SELECTION

#include "scene.h"
#include <stddef.h>

typedef struct {
    int selected;
    size_t entity_id;
} EntitySelection;

void selection_entity_select(EntitySelection *selection, size_t id);
void selection_deselect_all(EntitySelection *selection);
LiveEntity *selection_get_selected_entity(Scene *scene,
                                          EntitySelection *selection);

#endif
