#ifndef _MODEL_VECTOR
#define _MODEL_VECTOR

#include <raylib.h>
#include <stddef.h>

typedef size_t ModelHandle;

typedef struct {
    Model model;
    int is_private_instance;
} ModelData;

typedef struct {
    ModelData *data;
    size_t data_allocated;
    size_t data_used;
} ModelVector;

ModelVector modelvec_init(void);
// Appends a `ModelData` to the `ModelVector`. Returns handle / index of newly
// added element.
ModelHandle modelvec_append(ModelVector *vec, ModelData model);
ModelData *modelvec_get(ModelVector *vec, size_t index);
void modelvec_free(ModelVector *vec);

#endif
