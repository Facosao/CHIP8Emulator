//#include <ncurses.h>
//#include <SDL2/SDL_events.h>
//#include <SDL2/SDL_error.h>
//#include <SDL2/SDL_log.h>
//#include <SDL2/SDL_render.h>
//#include <SDL2/SDL_video.h>
//#include <SDL2/SDL_render.h>
//#include <SDL2/SDL_timer.h>
//#include <SDL2/SDL_video.h>
//#include <SDL2/SDL_timer.h>
//#include <SDL2/SDL_video.h>
//#include <SDL2/SDL_rect.h>

#include <time.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include "./include/CHIP8.h"
#include "./include/Timer.h"
#include "./include/Screen.h"
#include "./include/ROM.h"
#include "./include/Helper.h"
#include "./include/Input.h"
#include "./include/Font.h"

const uint64_t FRAMERATE = 60;
const uint64_t EMULATOR_FREQUENCY = 114000000;
// Max. frequency on my laptop (Ivy Bridge i5) -> 114 MHz -> 114000000

int main(int argc, char **argv) {

    if ((argv[1] == NULL) || (argc > 2)) {
        //printf("HEX[%X] = %X -> \n", 2, HEX_SPRITE_ADDRESS[2]);
        perror("No ROM file to load!\n");
        exit(EXIT_FAILURE);
    }

    call_SDL_Init();
    
    SDL_Window* window = NULL;
    SDL_Renderer* renderer = NULL;
    SDL_Event event;

    init_all_video(&window, &renderer);
    
    struct CHIP8 interpreter = chip8_new();

    init_memory(&interpreter);
    read_rom(&interpreter, argv[1]);

    SDL_Rect pixels[2048];

    for (int i = 0; i < 2048; i++) {
        pixels[i].x = (i % 64);
        pixels[i].y = (i / 64);
        pixels[i].w = 1;
        pixels[i].h = 1;
    }

    //for (int i = 0; i < 1024; i++) {
    //    interpreter.display[i] = 1;
    //}

    //int ticks = 60;

    /*
    printf("basic test = ");
    for (int i = 0; i < 5; i++) {
        printf("%d ", HEX_SPRITES[i]);
    }
    printf("\n");

    for (int i = 0x100; i < 0x150; i += 0x5) {
        printf("sprite[%X] = ", i);
        for (int j = 0; j < 5; j++) {
            printf("%d ", interpreter.memory[i + j]);
        }
        printf("\n");
    }
    */

    time_t start_delay = time(NULL);
    time_t total_delay = 0;

    while (true) {

        //start_delay = time(NULL);
        uint64_t frame_start = SDL_GetTicks64();

        while (SDL_PollEvent(&event) == 1) {

            switch (event.type) {

                case SDL_QUIT: {
                    SDL_Quit();
                    return 0;
                }

                case SDL_KEYUP:
                case SDL_KEYDOWN: {
                    set_key(&interpreter, &event.key);
                    break;
                }
            }
        }

        ///*
        for (int i = 0; i < (int)(EMULATOR_FREQUENCY / FRAMERATE); i++) {
            chip8_run(&interpreter);
        }
        //*/

        timer_update(&interpreter);
        //screen_draw(renderer, pixels, &interpreter);

        SDL_SetRenderDrawColor(renderer, 97, 134, 169, SDL_ALPHA_OPAQUE);
        SDL_RenderClear(renderer);
        SDL_SetRenderDrawColor(renderer, 33, 41, 70, SDL_ALPHA_OPAQUE);

        for (int i = 0; i < 2048; i++) {

            if (interpreter.display[i] == 1) {
                SDL_RenderFillRect(renderer, &pixels[i]);
            }
        }

        SDL_RenderPresent(renderer);

        total_delay = time(NULL) - start_delay;
        uint64_t frame_end = SDL_GetTicks64();

        if (total_delay > 0) {
            char window_title[32];
            sprintf(window_title, "CHIP-8 Emulator - Frametime: %02lu", frame_end - frame_start);
            SDL_SetWindowTitle(window, window_title);
            start_delay = time(NULL);
        }
    }

    return 0;
}