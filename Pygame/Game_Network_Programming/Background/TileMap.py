import pygame
import os
pygame.init()

SCREEN_WIDTH = 800
SCREEN_HEIGHT = 640
LOWER_MARGIN = 100
SIDE_MARGIN = 300

screen = pygame.display.set_mode((SCREEN_WIDTH + SIDE_MARGIN, SCREEN_HEIGHT + LOWER_MARGIN))
pygame.display.set_caption("Background")


background = pygame.image.load("./images_background/background_color.png")
background = pygame.transform.scale(background, (SCREEN_WIDTH // 2, SCREEN_HEIGHT))
def draw_bg():
    screen.blit(background, (0, 0), )

if __name__ == "__main__":
    run = True

    while run:

        draw_bg()

        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                run = False

    pygame.display.update()
    pygame.quit()