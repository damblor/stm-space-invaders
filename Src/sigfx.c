#include "sigfx.h"
#include "st7735.h"
#include "space_invaders.h"
#include <stdio.h>

/**
 * @brief Draws the player on the screen
 * @param player player_t struct to draw
 */
void SIGFX_DrawPlayer(player_t player)
{
  if (player.invincible)
  {
    ST7735_DrawRectangle(player.posX, player.posY, PLAYER_SIZE, PLAYER_SIZE, ST7735_RGB565(255, 126, 0));
  }
  else
  {
    ST7735_DrawRectangle(player.posX, player.posY, PLAYER_SIZE, PLAYER_SIZE, ST7735_GREEN);
  }
}

/**
 * @brief Draws the bullets on the screen
 * @param bullets bullet_t array to draw
 */
void SIGFX_DrawBullets(bullet_t *bullets)
{
  for (size_t i = 0; i < BULLET_ARRAY_SIZE; i++)
  {
    if (!bullets[i].active)
      continue;
    ST7735_DrawRectangle(bullets[i].posX, bullets[i].posY, BULLET_SIZE, BULLET_SIZE, ST7735_RED);
  }
}

/**
 * @brief Draws the enemies on the screen
 * @param enemies enemy_t array to draw
 */
void SIGFX_DrawEnemies(enemy_t* enemies)
{
  for (size_t i = 0; i < ENEMY_ARRAY_SIZE_Y * ENEMY_ARRAY_SIZE_X; i++)
  {
    if (!enemies[i].alive)
      continue;
    if (enemies[i].type == 1)
      ST7735_DrawRectangle(enemies[i].posX + 1, enemies[i].posY, ENEMY_SIZE, ENEMY_SIZE, ST7735_CYAN);
    else
      ST7735_DrawRectangle(enemies[i].posX + 1, enemies[i].posY, ENEMY_SIZE, ENEMY_SIZE, ST7735_MAGENTA);
  }
}

/**
 * @brief Draws the game screen
 * @param enemies enemy_t array to draw
 * @param player player_t struct to draw
 * @param bullets bullet_t array to draw
 */
void SIGFX_DrawGameScreen(enemy_t *enemies, player_t player, bullet_t *bullets)
{
  ST7735_ClearScreen();

  SIGFX_DrawBullets(bullets);
  SIGFX_DrawEnemies(enemies);
  SIGFX_DrawPlayer(player);
  
  ST7735_WriteString(0, 0, "Score:", Font_7x10, ST7735_WHITE);
  char s[7];
  sprintf(s, "%d", score);
  ST7735_WriteString(40, 0, s, Font_7x10, ST7735_WHITE);
  sprintf(s, "%d", player.lives);
  ST7735_WriteString(ST7735_WIDTH - 10, 0, s, Font_7x10, ST7735_WHITE);

  ST7735_FlushBuffer();
}

/**
 * @brief Draws the win screen
 */
void SIGFX_DrawWinScreen()
{
  ST7735_ClearScreen();

  ST7735_WriteString(10, 30, "CONGRATULATIONS!", Font_7x10, ST7735_WHITE);
  ST7735_WriteString(20, 50, "Score:", Font_7x10, ST7735_WHITE);
  char s[7];
  sprintf(s, "%d", score);
  ST7735_WriteString(60, 50, s, Font_7x10, ST7735_WHITE);
  ST7735_WriteString(35, 95, "Well done", Font_7x10, ST7735_WHITE);
  ST7735_WriteString(50, 110, "Hero.", Font_7x10, ST7735_WHITE);

  ST7735_FlushBuffer();
}

/**
 * @brief Draws the game over screen
 */
void SIGFX_DrawGameOverScreen()
{
  ST7735_ClearScreen();

  ST7735_WriteString(10, 30, "You have failed.", Font_7x10, ST7735_WHITE);
  ST7735_WriteString(30, 50, "Our planet", Font_7x10, ST7735_WHITE);
  ST7735_WriteString(30, 70, "is doomed.", Font_7x10, ST7735_WHITE);
  ST7735_WriteString(20, 90, "Score:", Font_7x10, ST7735_WHITE);
  char s[7];
  sprintf(s, "%d", score);
  ST7735_WriteString(60, 90, s, Font_7x10, ST7735_WHITE);

  ST7735_FlushBuffer();
}

/**
 * @brief Draws the start screen
 */
void SIGFX_DrawStartScreen()
{
  ST7735_ClearScreen();

  ST7735_WriteString(15, 30, "Welcome pilot!", Font_7x10, ST7735_WHITE);
  ST7735_WriteString(0, 45, "Brave the invaders", Font_7x10, ST7735_WHITE);
  ST7735_WriteString(50, 60, "and", Font_7x10, ST7735_WHITE);
  ST7735_WriteString(5, 75, "come back a hero.", Font_7x10, ST7735_WHITE);
  ST7735_WriteString(30, 100, "Good luck.", Font_7x10, ST7735_WHITE);
  ST7735_WriteString(7, 130, "Press A to start.", Font_7x10, ST7735_GREEN);

  ST7735_FlushBuffer();
}

/**
 * @brief Draws the konami code screen
 */
void SIGFX_DrawKonamiCodeScreen()
{
  ST7735_ClearScreen();

  ST7735_WriteString(0, 0, "KONAMI CODE", Font_7x10, ST7735_RGB565(255, 0, 0));
  ST7735_WriteString(0, 10, "UP UP DOWN DOWN", Font_7x10, ST7735_RGB565(0, 255, 0));
  ST7735_WriteString(0, 20, "LEFT RIGHT", Font_7x10, ST7735_RGB565(0, 0, 255));
  ST7735_WriteString(0, 30, "LEFT RIGHT", Font_7x10, ST7735_RGB565(255, 255, 255));

  ST7735_FlushBuffer();
}

/**
 * @brief Draws the init screen
 */
void SIGFX_DrawInit()
{
  ST7735_ClearScreen();
  ST7735_WriteString(20, 40, "SPACE", Font_16x26, ST7735_WHITE);
  ST7735_WriteString(17, 70, "INVADERS", Font_11x18, ST7735_WHITE);
  ST7735_FlushBuffer();
}
