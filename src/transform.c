#include "transform.h"

#include "common.h"
#include "scene.h"
#include <assert.h>
#include <raymath.h>

#define ROTATION_SPEED_MULTIPLIER 1

void transform_operation_apply(Settings *settings, LiveEntity *live_entity,
                               TransformOperation *operation) {
    assert(operation);
    assert(settings);
    assert(live_entity);

    live_entity->entity.transform =
        MatrixMultiply(transform_get_matrix(settings, operation),
                       live_entity->entity.transform);
}

Matrix transform_get_matrix(Settings *settings, TransformOperation *operation) {
    assert(operation);
    assert(settings);

    Vector3 axis_transform;

    switch (operation->axis) {
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

    axis_transform = Vector3Scale(axis_transform, operation->amount);

    switch (operation->mode) {
    case TRANSFORM_TRANSLATE:
        if (settings->quantize_to_grid_enabled)
            axis_transform =
                vector3_quantize(axis_transform, settings->grid_density);
        return MatrixTranslate(axis_transform.x, axis_transform.y,
                               axis_transform.z);
    case TRANSFORM_ROTATE:
        if (settings->quantize_to_grid_enabled)
            axis_transform =
                vector3_quantize(axis_transform, ROTATION_SNAP_INCREMENT);
        return MatrixRotateXYZ(axis_transform);
    case TRANSFORM_NONE:
        return MatrixIdentity();
    }

    return MatrixIdentity();
}

void transform_start(Settings *settings, TransformOperation *operation,
                     TransformMode mode, Axis axis, LiveEntity *live_entity) {
    assert(live_entity);
    assert(operation);
    assert(settings);

    // Apply previous if needed
    if (operation->mode != TRANSFORM_NONE) {
        transform_operation_apply(settings, live_entity, operation);
    }

    operation->mode = mode;
    operation->axis = axis;
    operation->amount = 0;
}

void transform_stop(Settings *settings, TransformOperation *operation,
                    LiveEntity *live_entity) {
    assert(live_entity);
    assert(operation);
    assert(settings);

    if (operation->mode == TRANSFORM_NONE)
        return;

    transform_operation_apply(settings, live_entity, operation);
    operation->mode = TRANSFORM_NONE;
}

void transform_cancel(TransformOperation *operation) {
    operation->mode = TRANSFORM_NONE;
}
