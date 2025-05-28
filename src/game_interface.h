#ifndef _GAME_INTERFACE
#define _GAME_INTERFACE

#include <raylib.h>
#include <stddef.h>

int game_init(char *scene_filepath, const char *vertex_shader,
              const char *entity_frag_shader, const char *terrain_frag_shader,
              const char *skybox_model);
void game_main(void);
void game_deinit(void);

#endif
