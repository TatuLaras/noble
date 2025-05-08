#include "transform.h"

#include "common.h"
#include "settings.h"
#include <assert.h>
#include <raymath.h>

#define ROTATION_SPEED_MULTIPLIER 1

TransformOperation transform_operation = {0};

// Commits the transform operation to the entity transform.
static void transform_apply(Entity *entity) {
    assert(entity);

    entity->transform =
        MatrixMultiply(transform_get_matrix(), entity->transform);
}

Matrix transform_get_matrix(void) {
    Vector3 axis_transform;

    switch (transform_operation.axis) {
    case AXIS_X:
        axis_transform = (Vector3){1.0, 0, 0};
        break;
    case AXIS_Y:
        axis_transform = (Vector3){0, 1.0, 0};
        break;
    case AXIS_Z:
        axis_transform = (Vector3){0, 0, 1.0};
        break;
    }

    axis_transform = Vector3Scale(axis_transform, transform_operation.amount);

    switch (transform_operation.mode) {
    case TRANSFORM_TRANSLATE:
        axis_transform = settings_quantize_to_grid(axis_transform, 0);
        return MatrixTranslate(axis_transform.x, axis_transform.y,
                               axis_transform.z);
    case TRANSFORM_ROTATE:
        if (settings.quantize_to_grid_enabled)
            axis_transform = (Vector3){
                quantize(axis_transform.x, ROTATION_SNAP_INCREMENT),
                quantize(axis_transform.y, ROTATION_SNAP_INCREMENT),
                quantize(axis_transform.z, ROTATION_SNAP_INCREMENT),
            };

        return MatrixRotateXYZ(axis_transform);
    case TRANSFORM_NONE:
        return MatrixIdentity();
    }

    return MatrixIdentity();
}

void transform_start(TransformMode mode, Axis axis, Entity *entity) {
    assert(entity);

    // Apply previous if needed
    if (transform_operation.mode != TRANSFORM_NONE) {
        transform_apply(entity);
    }

    transform_operation.mode = mode;
    transform_operation.axis = axis;
    transform_operation.amount = 0;
}

void transform_stop(Entity *entity) {
    assert(entity);

    if (transform_operation.mode == TRANSFORM_NONE)
        return;

    transform_apply(entity);
    transform_operation.mode = TRANSFORM_NONE;
}

void transform_cancel(void) {
    transform_operation.mode = TRANSFORM_NONE;
}
