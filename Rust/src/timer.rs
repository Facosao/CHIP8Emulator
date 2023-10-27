use super::CHIP8;

const DT: usize = 0;
const ST: usize = 1;

pub fn timer_update(interpreter: &mut CHIP8) {
    
    static mut SOUND_IS_PLAYING: bool = false;
    // static tone
    
    if interpreter.timers[DT] > 0 {
        interpreter.timers[DT] -= 1;
    }

    unsafe {
        if (interpreter.timers[ST] > 0) && !SOUND_IS_PLAYING {
            interpreter.timers[ST] -= 1;
            // tone play
            SOUND_IS_PLAYING = true
        } else if (interpreter.timers[ST] > 0) && SOUND_IS_PLAYING {
            interpreter.timers[ST] -= 1;
        } else if interpreter.timers[ST] <= 0 {
            // tone fadeout
            SOUND_IS_PLAYING = false;
        }
    }
}