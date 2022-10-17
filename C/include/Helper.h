#ifndef INIT_HEADER
#define INIT_HEADER

#include <SDL2/SDL.h>

void call_SDL_Init(void);
void init_all_video(SDL_Window** new_window, SDL_Renderer** new_renderer);
SDL_Renderer *get_current_renderer(SDL_Window *current_window);

#endif