#ifndef _GAME_INTERFACE
#define _GAME_INTERFACE

#include <raylib.h>
#include <stddef.h>

#define LIGHTING_GROUP_HANDLE_DEFAULT 0

int game_init(char *scene_filepath);
void game_main(void);
void game_deinit(void);

#endif
