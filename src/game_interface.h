#ifndef _GAME_INTERFACE
#define _GAME_INTERFACE

#include <stddef.h>

int game_init(char *scene_filepath);
void game_main(void);
void game_deinit(void);

#endif
