#include "settings.h"
#include "common.h"

Settings settings = {
    .quantize_to_grid_enabled = 1,
    .grid_density = 2.0,
    .grid_enabled = 1,
    .gizmos_enabled = 1,
    .properties_menu_enabled = 0,
    .lighting_enabled = 1,
    .adding_raycast_include_objects = 1,
    .gizmo_size = 1.0,
    .project_directory = "/home/tatu/_repos/ebb/",
};

Vector3 settings_quantize_to_grid(Vector3 vec, int ignore_enabled) {
    if (settings.quantize_to_grid_enabled || ignore_enabled)
        return (Vector3){
            .x = quantize(vec.x, settings.grid_density),
            .y = quantize(vec.y, settings.grid_density),
            .z = quantize(vec.z, settings.grid_density),
        };

    return vec;
}
