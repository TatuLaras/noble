/*
 *   noble - a scene editor for retro game development
 */
#include "game_interface.h"
#include "settings.h"
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv) {

    char *scene_filepath = 0;
    for (int i = 1; i < argc; i++) {
        // CLI args..
        if (!strcmp(argv[i], "-vertical")) {
            settings.mouse_movements_vertical = 1;
            printf("OPTION: Using vertical mouse movements\n");
            continue;
        }

        if (*argv[i] == '-') {
            fprintf(stderr, "Unsupported command-line option \"%s\"", argv[i]);
            return 1;
        }

        scene_filepath = argv[i];
        break;
    }

// Resource file paths
#ifdef DEBUG
    const char *vertex_shader = "resources/shaders/vertex.vert";
    const char *entity_frag_shader = "resources/shaders/entity.frag";
    const char *terrain_frag_shader = "resources/shaders/terrain.frag";
    const char *skybox_model = "resources/skybox.obj";
#else
    const char *vertex_shader = "/usr/share/noble/shaders/vertex.vert";
    const char *entity_frag_shader = "/usr/share/noble/shaders/entity.frag";
    const char *terrain_frag_shader = "/usr/share/noble/shaders/terrain.frag";
    const char *skybox_model = "/usr/share/noble/skybox.obj";
#endif

    if (!scene_filepath) {
        fprintf(stderr, "ERROR: No scene filepath was provided as "
                        "argument.\nUsage: noble [SCENE FILEPATH] [OPTIONS]\n");
        return 1;
    }

    if (game_init(scene_filepath, vertex_shader, entity_frag_shader,
                  terrain_frag_shader, skybox_model))
        return 1;

    game_main();
    game_deinit();

    return 0;
}
