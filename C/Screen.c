#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_video.h>
#include <stdbool.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include "./include/CHIP8.h"
#include "./include/Helper.h"

void screen_draw(SDL_Renderer* renderer, SDL_Rect* pixels, struct CHIP8* self) {
    
    /*
    for (int i = 0; i < 2048; i++) {

        if (self->display[i] == 1)
            mvprintw(i / 64, i % 64, "#");
        else
            mvprintw(i / 64, i % 64, " ");
    }
    */

    //SDL_Renderer* renderer = get_current_renderer(window);

    SDL_SetRenderDrawColor(renderer, 97, 134, 169, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, 33, 41, 70, SDL_ALPHA_OPAQUE);

    for (int i = 0; i < 2048; i++) {

        if (self->display[i] == 1) {
            SDL_RenderFillRect(renderer, &pixels[i]);
        }
    }

    SDL_RenderPresent(renderer);
}
