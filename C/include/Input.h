#ifndef INPUT_HEADER
#define INPUT_HEADER

#include "CHIP8.h"
#include <SDL2/SDL.h>

void set_key(struct CHIP8* self, SDL_KeyboardEvent* key_event);

#endif