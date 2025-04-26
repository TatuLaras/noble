#ifndef _TRANSFORM
#define _TRANSFORM

// Provides functions for providing entity transform controls to the user.

#include "scene.h"
#include <raylib.h>
#define ROTATION_SNAP_INCREMENT (PI / 8.0)

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

extern TransformOperation transform_operation;

// Gets an equivalent transformation matrix from current transform operation.
Matrix transform_get_matrix(void);

// Starts the transforming process for an entity.
void transform_start(TransformMode mode, Axis axis, Entity *entity);
// Stops the transforming process while committing new transform into the
// entitys transform.
void transform_stop(Entity *entity);

// Stops the transforming process without committing to the entity transform.
void transform_cancel(void);

#endif
