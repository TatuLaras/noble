#include "general_buffer.h"
#include <stdlib.h>

#define STARTING_SIZE 16
#define GROWTH_FACTOR 2

GeneralBuffer genbuf_init(void) {
    return (GeneralBuffer){
        .data = malloc(STARTING_SIZE),
        .data_allocated = STARTING_SIZE,
        .data_size = 0,
    };
}

void genbuf_append(GeneralBuffer *buf, void *data, size_t data_size);

void genbuf_get(GeneralBuffer *buf, void *out_data, size_t out_data_size);
