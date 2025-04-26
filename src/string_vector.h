#ifndef _STRING_VECTOR
#define _STRING_VECTOR

// A vector for storing a list of strings.

#include <stddef.h>

typedef struct {
    char *data;
    size_t data_allocated;
    size_t data_used;
    size_t *indices;
    size_t indices_allocated;
    size_t indices_used;
} StringVector;

StringVector stringvec_init(void);
StringVector stringvec_clone(StringVector *vec);
void stringvec_append(StringVector *vec, char *buffer, size_t buffer_size);
char *stringvec_get(StringVector *vec, size_t index);
void stringvec_free(StringVector *vec);
size_t stringvec_count(StringVector *vec);
// Returns strings in stringvector as a newline-separated string. Ownership of
// the char* belongs to the caller.
void stringvec_as_newline_separated(StringVector *vec, char *out_buffer,
                                    size_t out_buffer_size,
                                    int max_string_count);
// Returns 1 if StringVector `vec` has a string exactly equal to `string`.
int stringvec_has(StringVector *vec, const char *string);
// Truncates all data without deallocating, i.e. still has the same amount of
// space allocated but the vector will be empty.
void stringvec_truncate(StringVector *vec);

#endif
