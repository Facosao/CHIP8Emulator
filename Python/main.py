import builtins
import sys
import pygame
import Screen
import ROM

from CHIP8 import CHIP8
from Input import InputHandler
from Timer import Timer

FRAMERATE = 60
EMULATOR_FREQUENCY = 1200

if __name__ == "__main__":

    pygame.init()
    pygame.mixer.init()
    pygame.display.set_caption("CHIP-8 Emulator")
    pygame.display.set_mode((64, 32), (pygame.SCALED | pygame.RESIZABLE))
    pygame.event.set_allowed(pygame.QUIT | pygame.KEYDOWN | pygame.KEYUP)

    if len(sys.argv) == 1:
        print("No ROM file to load!")
        pygame.quit()

    # Required objects
    clock = pygame.time.Clock()
    interpreter = CHIP8()
    input = InputHandler(interpreter.keys)
    output = Screen.Output(interpreter.display)
    timer = Timer(interpreter.timers)

    ROM.init_memory(interpreter.memory)
    ROM.read_rom(interpreter.memory)

    ticks = 60

    while True:

        event = pygame.event.poll()

        match event.type:

            case pygame.QUIT:
                break

            case pygame.KEYDOWN:
                input.set_key(input.PRESSED, event.key)

            case pygame.KEYUP:
                input.set_key(input.RELEASED, event.key)

        for _ in range(EMULATOR_FREQUENCY // FRAMERATE):
            interpreter.run()

        timer.update()

        output.draw()

        if ticks > 0:
            ticks -= 1
        else:
            ticks = 60
            caption = "CHIP-8 Emulator - " + sys.argv[1] + " - FPS: " + str(clock.get_fps())[0:5:]
            pygame.display.set_caption(caption)

        clock.tick(FRAMERATE)

    pygame.quit()
