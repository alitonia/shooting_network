import pygame
from Character.Bullet import Bullet

pygame.init()

GRAVITY = 0.75


class Player(pygame.sprite.Sprite):
    def __init__(self, x, y, scale, speed, armo):
        pygame.sprite.Sprite.__init__(self)

        self.animation_list = []
        self.update_time = pygame.time.get_ticks()
        self.frame_index = 0
        self.action = 0
        self.offset_x = 0
        self.offset_y = 0

        # Load animation for each action (E.x: idle, run, shoot, ...etc)
        # idle animation
        temp_list = []
        for i in range(5):
            img = pygame.image.load(f"Character/images_player/s_jojo_idle_{i}.png")
            # img = pygame.image.load(f"./images_player/s_jojo_idle_{i}.png")
            img = pygame.transform.scale(img, (int(img.get_width() * scale), int(img.get_height() * scale)))
            temp_list.append(img)
        self.animation_list.append(temp_list)

        # run animation
        temp_list = []
        for i in range(10):
            img = pygame.image.load(f"Character/images_player/s_jojo_run_{i}.png")
            img = pygame.transform.scale(img, (int(img.get_width() * scale), int(img.get_height() * scale)))
            temp_list.append(img)
        self.animation_list.append(temp_list)

        # jump animation
        temp_list = []
        for i in range(9):
            img = pygame.image.load(f"Character/images_player/s_jojo_jump_{i}.png")
            img = pygame.transform.scale(img, (int(img.get_width() * scale), int(img.get_height() * scale)))
            temp_list.append(img)
        self.animation_list.append(temp_list)

        # crouch animation
        temp_list = []
        for i in range(2):
            img = pygame.image.load(f"Character/images_player/s_jojo_crouch_shooting_{i}.png")
            img = pygame.transform.scale(img, (int(img.get_width() * scale), int(img.get_height() * scale)))
            temp_list.append(img)
        self.animation_list.append(temp_list)

        self.image = self.animation_list[self.action][self.frame_index]

        self.rect = self.image.get_rect()
        self.rect.center = (x, y)

        self.alive = True
        self.in_air = True
        self.vel_y = 0

        self.jump = False
        self.move_left = False
        self.move_right = False
        self.crouch = False

        self.speed = speed

        self.flip = False
        self.direction = 1

        self.shoot = False
        self.shoot_cooldown = 0

        self.armo = armo  # will decrease if a bullet come through
        self.start_armo = armo
        self.health = 100
        self.max_health = self.health

    def update(self):
        self.update_animation()
        # self.check_death()
        if self.shoot_cooldown > 0:
            self.shoot_cooldown -= 1

    def update_animation(self):
        ANIMATION_COOLDOWN = 100
        self.image = self.animation_list[self.action][self.frame_index]
        if pygame.time.get_ticks() - self.update_time > ANIMATION_COOLDOWN:
            self.update_time = pygame.time.get_ticks()
            self.frame_index += 1

        if self.frame_index >= len(self.animation_list[self.action]):
            if self.action == 4:
                self.frame_index = len(self.animation_list[self.action]) - 1
            else:
                self.frame_index = 0

    def update_action(self, new_action):
        if new_action != self.action:
            self.action = new_action
            # update the animation settings
            self.frame_index = 0
            self.update_time = pygame.time.get_ticks()
            if new_action == 3:
                self.offset_x = 10
                self.offset_y = 10
            else:
                self.offset_x = 0
                self.offset_y = 0

    def shooting(self):
        if self.shoot_cooldown == 0:
            self.shoot_cooldown = 20
            bullet = Bullet(self.rect.centerx + (0.5 * (self.rect.size[0] + self.offset_x) * self.direction),
                            self.offset_y + self.rect.centery + (0.25 * self.rect.size[1]), self.direction)

            # reduce armo
            self.armo -= 1

    def move(self):
        dx = 0
        dy = 0

        if self.move_left:
            dx = -self.speed
            self.flip = True
            self.direction = -1
        if self.move_right:
            dx = self.speed
            self.flip = False
            self.direction = 1
        if self.jump and not self.in_air:
            self.vel_y = -11
            self.jump = False
            self.in_air = True

        self.vel_y += GRAVITY
        if self.vel_y > 10:
            self.vel_y

        dy += self.vel_y

        if self.rect.bottom + dy > 300:
            dy = 300 - self.rect.bottom
            self.in_air = False

        self.rect.x += dx
        self.rect.y += dy

    def check_death(self):
        if self.health <= 0:
            self.health = 0
            self.speed = 0
            self.alive = False
            self.update_action(3)

    def draw(self, screen):
        screen.blit(pygame.transform.flip(self.image, self.flip, False), self.rect)
