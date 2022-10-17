#include <stdbool.h>
#include "./include/Timer.h"

void timer_update(struct CHIP8* self) {
    static bool sound_is_playing = false;
    //static tone

    if (self->DT > 0) {
        self->DT -= 1;
    }

    if ((self->ST > 0) && (sound_is_playing == false)) {
        self->ST -= 1;
        // tone play
        sound_is_playing = true;
    }
    else {
        if ((self->ST > 0) && (sound_is_playing == true)) {
            self->ST -= 1;
        }
        else {
            if ((self->ST <= 0) && (sound_is_playing == true)) {
                // tone fadeout
                sound_is_playing = false;
            }
        }
    }
}
