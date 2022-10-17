//#include <SDL2/SDL_events.h>
//#include <SDL2/SDL_keycode.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include "./include/CHIP8.h"

const bool KEY_PRESSED = true;
const bool KEY_RELEASED = false;

void set_key(struct CHIP8* self, SDL_KeyboardEvent* key) {

    bool pressed_key = false;

    if (key->state == SDL_PRESSED) {
        pressed_key = true;
    }

    int index = -1;

    switch (key->keysym.sym) {
 
        case SDLK_x: {
            index = 0;
            break;
        }

        case SDLK_1: {
            index = 1;
            break;
        }

        case SDLK_UP:
        case SDLK_2: {
            index = 2;
            break;
        }

        case SDLK_3: {
            index = 3;
            break;
        }

        case SDLK_LEFT:
        case SDLK_q: {
            index = 4;
            break;
        }

        case SDLK_w: {
            index = 5;
            break;
        }

        case SDLK_RIGHT:
        case SDLK_e: {
            index = 6;
            break;
        }

        case SDLK_a: {
            index = 7;
            break;
        }

        case SDLK_DOWN:
        case SDLK_s: {
            index = 8;
            break;
        }

        case SDLK_d: {
            index = 9;
            break;
        }

        case SDLK_z: {
            index = 0xA;
            break;
        }

        case SDLK_c: {
            index = 0xB;
            break;
        }

        case SDLK_4: {
            index = 0xC;
            break;
        }

        case SDLK_r: {
            index = 0xD;
            break;
        }

        case SDLK_f: {
            index = 0xE;
            break;
        }

        case SDLK_v: {
            index = 0xF;
            break;
        }

        default:
            return;
    }

    if (pressed_key)
        self->keys[index] = 1;
    else
        self->keys[index] = 0;
}