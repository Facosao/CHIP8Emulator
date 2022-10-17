import pygame


class InputHandler:

    PRESSED = True
    RELEASED = False

    def __init__(self, key_array) -> None:
        self.keys = key_array

    def set_key(self, pressed_key: bool, key) -> None:

        match key:

            case pygame.K_x:
                index = 0

            case pygame.K_1:
                index = 1

            case pygame.K_2 | pygame.K_UP:
                index = 2

            case pygame.K_3:
                index = 3

            case pygame.K_q | pygame.K_LEFT:
                index = 4

            case pygame.K_w:
                index = 5

            case pygame.K_e | pygame.K_RIGHT:
                index = 6

            case pygame.K_a:
                index = 7

            case pygame.K_s | pygame.K_DOWN:
                index = 8

            case pygame.K_d:
                index = 9

            case pygame.K_z:
                index = 0xA

            case pygame.K_c:
                index = 0xB

            case pygame.K_4:
                index = 0xC

            case pygame.K_r:
                index = 0xD

            case pygame.K_f:
                index = 0xE

            case pygame.K_v:
                index = 0xF

            case _:
                return

        if pressed_key:
            self.keys[index] = 1
        else:
            self.keys[index] = 0
