#ifndef _MATH
#define _MATH

#include <raylib.h>
#include <raymath.h>

// Gets a `ray` s intersection with the ground plane.
Vector3 math_ray_ground_intersection(Ray ray);
// Takes all corners of the bounding box and calculates a new one after a
// transformation has been applied to the corners.
// If rotation is applied the bounding box will be larger than necessary.
BoundingBox math_transform_box(BoundingBox box, Matrix transform);

#endif
