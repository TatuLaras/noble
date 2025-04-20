#ifndef _COMMON
#define _COMMON

#include <raylib.h>
#include <stdint.h>

uint64_t max(uint64_t a, uint64_t b);
uint64_t min(uint64_t a, uint64_t b);
float maxf(float a, float b);
float minf(float a, float b);

#define ARRAY_LENGTH(x) ((sizeof x) / sizeof(*x))

float quantize(float value, float interval);
Vector3 vector3_quantize(Vector3 value, float interval);

#endif
