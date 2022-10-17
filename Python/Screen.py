import pygame


class Pixel:
    def __init__(self, new_rect) -> None:
        self.color = (0, 0, 0)
        self.rect = new_rect


class Output:
    def __init__(self, screen_array) -> None:

        self.surface = pygame.display.get_surface()
        self.colors = [(0, 0, 0), (255, 255, 255)]
        self.pixels: list[Pixel] = []

        for i in range(2048):
            self.pixels.append(Pixel(pygame.Rect(i % 64, i // 64, 1, 1)))

        self.screen_data = screen_array

    def draw(self):

        for i in range(0, 2048):
            self.pixels[i].color = self.colors[self.screen_data[i]]
            pygame.draw.rect(self.surface, self.pixels[i].color, self.pixels[i].rect)

        pygame.display.flip()
