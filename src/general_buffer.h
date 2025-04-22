#ifndef _FILE_BUFFER
#define _FILE_BUFFER

#include <stddef.h>
#include <stdint.h>

typedef struct {
    uint8_t *data;
    size_t data_allocated;
    size_t data_size;
} GeneralBuffer;

GeneralBuffer genbuf_init(void);
void genbuf_append(GeneralBuffer *buf, void *data, size_t data_size);
void genbuf_get(GeneralBuffer *buf, void *out_data, size_t out_data_size);

#endif
