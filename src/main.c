/*
 *   noble - a scene editor for retro game development
 */
#include "game_interface.h"
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/stat.h>

int main(int argc, char **argv) {

    char *scene_filepath = 0;
    for (int i = 1; i < argc; i++) {
        // CLI args..

        if (*argv[i] == '-') {
            fprintf(stderr, "Unsupported command-line option \"%s\"", argv[i]);
            return 1;
        }

        scene_filepath = argv[i];
        break;
    }

    if (!scene_filepath) {
        fprintf(stderr, "ERROR: No scene filepath was provided as "
                        "argument.\nUsage: noble [SCENE FILEPATH] [OPTIONS]\n");
        return 1;
    }

    if (game_init(scene_filepath))
        return 1;

    game_main();
    game_deinit();

    return 0;
}
