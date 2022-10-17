#ifndef SCREEN_HEADER
#define SCREEN_HEADER

#include "CHIP8.h"
#include <SDL2/SDL.h>

void screen_draw(SDL_Renderer* renderer, SDL_Rect* pixels, struct CHIP8* self);

#endif