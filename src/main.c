#include "game_interface.h"
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/stat.h>

int main(int argc, char **argv) {

    for (int i = 1; i < argc; i++) {
        // CLI args..

        if (*argv[i] == '-') {
            fprintf(stderr, "Unsupported command-line option \"%s\"", argv[i]);
            return 1;
        }
    }

    game_init();
    game_main();
    game_deinit();

    return 0;
}
