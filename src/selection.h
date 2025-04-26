#ifndef _SELECTION
#define _SELECTION

// Keep track which entity is selected.

#include "scene.h"
#include <stddef.h>

typedef struct {
    int is_entity_selected;
    EntityHandle handle;
} EntitySelection;

extern EntitySelection entity_selection_state;

void selection_select_entity(EntityHandle handle);
void selection_deselect_all(void);
Entity *selection_get_selected_entity(void);

#endif
