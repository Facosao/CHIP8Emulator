#include <SDL2/SDL_log.h>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_video.h>
#include <ncurses.h>
#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>

void call_SDL_Init(void) {
    int init_flags = SDL_INIT_TIMER | SDL_INIT_AUDIO | SDL_INIT_VIDEO;

    if (SDL_Init(init_flags) != 0) {
        SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
        SDL_Quit();
    }
}

void init_all_video(SDL_Window** new_window, SDL_Renderer** new_renderer) {

    const int window_flags = SDL_WINDOW_RESIZABLE;
    const int renderer_flags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC;
    const int window_pos = SDL_WINDOWPOS_CENTERED;

    *new_window = SDL_CreateWindow("CHIP-8 Emulator", window_pos, window_pos, 1280, 640, window_flags);

    if (new_window == NULL) {
        printf("Couldn't create window: %s\n", SDL_GetError());
        scanf("%*c");
        //SDL_Quit();
    }

    *new_renderer = SDL_CreateRenderer(*new_window, -1, renderer_flags);

    if (new_renderer == NULL) {
        printf("Couldn't create renderer: %s\n", SDL_GetError());
        scanf("%*c");
        //SDL_Quit();
    }

    ///*
    if (SDL_RenderSetLogicalSize(*new_renderer, 64, 32) < 0) {
        printf("Couldn't set render logical size: %s\n", SDL_GetError());
        scanf("%*c");
        //SDL_Quit();
    }

    //SDL_SetRenderDrawColor(new_renderer, 97, 134, 169, SDL_ALPHA_OPAQUE);
    //SDL_RenderClear(new_renderer);
    //SDL_RenderPresent(new_renderer);

    SDL_RenderSetIntegerScale(*new_renderer, true);
    //*/

    //SDL_SetWindowSize(new_window, 1280, 640);
    //SDL_UpdateWindowSurface(new_window);
}

SDL_Renderer* get_current_renderer(SDL_Window* current_window) {
    
    SDL_Renderer* current_renderer = SDL_GetRenderer(current_window);

    if (current_renderer == NULL) {
        printf("Couldn't get current renderer: %s\n", SDL_GetError());
        scanf("%*c");
        //SDL_Quit();
    }

    return current_renderer;
}
