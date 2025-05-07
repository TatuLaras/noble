#include "common.h"
#include "settings.h"

#include <math.h>
#include <raylib.h>
#include <stdint.h>

uint64_t max(uint64_t a, uint64_t b) {
    if (a > b)
        return a;
    return b;
}

uint64_t min(uint64_t a, uint64_t b) {
    if (a < b)
        return a;
    return b;
}

float maxf(float a, float b) {
    if (a > b)
        return a;
    return b;
}

float minf(float a, float b) {
    if (a < b)
        return a;
    return b;
}

float quantize(float value, float interval) {
    return roundf(value / interval) * interval;
}

float scalar_quantize(float value) {
    if (!settings.quantize_to_grid_enabled)
        return value;

    return quantize(value, settings.grid_density);
}

Vector3 vector3_quantize(Vector3 value) {
    return (Vector3){
        .x = quantize(value.x, settings.grid_density),
        .y = quantize(value.y, settings.grid_density),
        .z = quantize(value.z, settings.grid_density),
    };
}

Vector3 vector3_settings_quantize(Vector3 value) {
    if (settings.quantize_to_grid_enabled)
        return vector3_quantize(value);
    return value;
}

Vector3 vector3_quantize_custom(Vector3 value, float increment) {
    if (!settings.quantize_to_grid_enabled)
        return value;

    return (Vector3){
        .x = quantize(value.x, increment),
        .y = quantize(value.y, increment),
        .z = quantize(value.z, increment),
    };
}
