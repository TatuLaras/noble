#ifndef _COMMON
#define _COMMON

#include <raylib.h>
#include <stdint.h>

uint64_t max(uint64_t a, uint64_t b);
uint64_t min(uint64_t a, uint64_t b);
float maxf(float a, float b);
float minf(float a, float b);

#define ARRAY_LENGTH(x) ((sizeof x) / sizeof(*x))
#define NAME_MAX_LENGTH 128

float quantize(float value, float interval);
// Quantizes a vector depending on the current snap settings.
Vector3 vector3_quantize(Vector3 value);
// Same as `vector3_quantize` but with a custom `increment`.
Vector3 vector3_quantize_custom(Vector3 value, float increment);
// Quantizes a scalar value depending on the current snap settings.
float scalar_quantize(float value);

#endif
