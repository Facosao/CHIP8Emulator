import pygame


class Timer:
    def __init__(self, timers) -> None:
        self.timer = timers
        self.DT = 0
        self.ST = 1
        self.sound_is_playing = False
        self.tone = pygame.mixer.Sound("tone.ogg")

    def update(self):

        if self.timer[self.DT] > 0:
            self.timer[self.DT] -= 1

        if self.timer[self.ST] > 0 and self.sound_is_playing is False:
            self.timer[self.ST] -= 1
            self.tone.play(-1, 0, 0)
            self.sound_is_playing = True

        elif self.timer[self.ST] > 0 and self.sound_is_playing is True:
            self.timer[self.ST] -= 1

        elif self.timer[self.ST] <= 0:
            self.tone.fadeout(100)
            self.sound_is_playing = False
