#ifndef _TRANSFORM
#define _TRANSFORM

#include "game_interface.h"
#include "scene.h"

#define ROTATION_SNAP_INCREMENT 0.0625 * PI

typedef enum {
    TRANSFORM_NONE,
    TRANSFORM_TRANSLATE,
    TRANSFORM_ROTATE,
} TransformMode;

typedef enum { AXIS_X, AXIS_Y, AXIS_Z } Axis;

typedef struct {
    TransformMode mode;
    Axis axis;
    float amount;
} TransformOperation;

void transform_operation_apply(Settings *settings, LiveEntity *live_entity,
                               TransformOperation *operation);

Matrix transform_get_matrix(Settings *settings, TransformOperation *operation);

void transform_start(Settings *settings, TransformOperation *operation,
                     TransformMode mode, Axis axis, LiveEntity *live_entity);
void transform_stop(Settings *settings, TransformOperation *operation,
                    LiveEntity *live_entity);

void transform_cancel(TransformOperation *operation);

#endif
