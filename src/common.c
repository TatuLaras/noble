#include "common.h"

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
