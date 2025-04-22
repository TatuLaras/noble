#ifndef _GAME_INTERFACE
#define _GAME_INTERFACE

#include <stddef.h>

// Takes ownership of `filepaths`.
void game_init(void);
void game_main(void);
void game_deinit(void);

#endif
