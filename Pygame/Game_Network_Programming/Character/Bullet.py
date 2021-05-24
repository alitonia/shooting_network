import pygame


bullet_group = pygame.sprite.Group()
class Bullet(pygame.sprite.Sprite):
     def __init__(self, x, y, direction):
         self.group = bullet_group
         pygame.sprite.Sprite.__init__(self, self.group)
         self.x = x
         self.y = y
         self.direction = direction
         self.speed = 20
         self.frame_index = 0
         self.update_time = pygame.time.get_ticks()

         self.animation_bullet = []
         for i in range(8):
             # img = pygame.image.load(f"Character/images_player/s_bullet_smg_{i}.png")
             img = pygame.image.load(f"Character/images_player/s_bullet_smg_0.png")

             self.animation_bullet.append(img)

         self.image = self.animation_bullet[self.frame_index]

         self.rect = self.image.get_rect()

         self.rect.center = (x, y)

     def update(self, player):
         # move bullet
         ANIMATION_COOLDOWN = 50
         self.image = self.animation_bullet[self.frame_index]
         if pygame.time.get_ticks() - self.update_time > ANIMATION_COOLDOWN:
             self.update_time = pygame.time.get_ticks()
             self.frame_index += 1
             self.rect.x += (self.direction * self.speed)

         if self.frame_index >= len(self.animation_bullet):
             self.frame_index = 0
             self.rect.x += (self.direction * self.speed)
         # check if bullet is gone off the screen
         if self.rect.right < 0 or self.rect.left > 700:
             self.kill()

         # check collision
         '''
         if pygame.sprite.spritecollide(player, bullet_group, False):
             if player.alive:
                player.health -= 5
                self.kill()
        '''

