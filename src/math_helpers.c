#include "math_helpers.h"
#include "common.h"
#include <raymath.h>
#include <stddef.h>

Vector3 math_ray_ground_intersection(Ray ray) {
    float t = (-1 * ray.position.y) / ray.direction.y;
    return Vector3Add(ray.position, Vector3Scale(ray.direction, t));
}

BoundingBox math_transform_box(BoundingBox box, Matrix transform) {

    Vector3 bounding_box_points[] = {
        box.min,
        box.max,
        {
            box.min.x,
            box.min.y,
            box.max.z,
        },
        {
            box.max.x,
            box.min.y,
            box.max.z,
        },
        {
            box.max.x,
            box.min.y,
            box.min.z,
        },
        {
            box.min.x,
            box.max.y,
            box.max.z,
        },
        {
            box.min.x,
            box.max.y,
            box.min.z,
        },
        {
            box.max.x,
            box.max.y,
            box.min.z,
        },
    };

    for (size_t i = 0; i < ARRAY_LENGTH(bounding_box_points); i++) {
        Vector3 transformed_point =
            Vector3Transform(bounding_box_points[i], transform);

        if (i == 0) {
            box.min = transformed_point;
            box.max = transformed_point;
        }

        box.min.x = minf(box.min.x, transformed_point.x);
        box.min.y = minf(box.min.y, transformed_point.y);
        box.min.z = minf(box.min.z, transformed_point.z);

        box.max.x = maxf(box.max.x, transformed_point.x);
        box.max.y = maxf(box.max.y, transformed_point.y);
        box.max.z = maxf(box.max.z, transformed_point.z);
    }

    return box;
}
