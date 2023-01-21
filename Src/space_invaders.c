#include "space_invaders.h"
#include "st7735.h"
#include "stbutton.h"
#include "stjoystick.h"
#include "sigfx.h"

static int timer = 0;
static int enemyMoveTimer = 0;
static int playerMoveTimer = 0;
static int playerShootTimer = 0;
static int bulletMoveTimer = 0;
static int playerInvincibleTimer = 0;
static int bulletsFired = 0;

game_state_t gameState;
enemy_t enemies[ENEMY_ARRAY_SIZE_Y][ENEMY_ARRAY_SIZE_X];
bullet_t bullets[BULLET_ARRAY_SIZE];
player_t player;
uint16_t totalEnemies = 0;
int16_t score = 0;
direction_t lastMoves[8];

/**
 * @brief Initialize the game
 */
void SPACEINV_Init()
{
  gameState = INITIALIZING;
  SIGFX_DrawInit();

  for (size_t i = 0; i < BULLET_ARRAY_SIZE; i++)
  {
    bullets[i].posX = 0;
    bullets[i].posY = 0;
    bullets[i].active = false;
  }

  for (size_t i = 0; i < ENEMY_ARRAY_SIZE_Y; i++)
  {
    for (size_t j = 0; j < ENEMY_ARRAY_SIZE_X; j++)
    {
      enemies[i][j].posX = 15 + j * 15;
      enemies[i][j].posY = 15 + i * 10;
      enemies[i][j].alive = true;
      if (i % 2 == 0 && j % 2 == 0)
      {
        enemies[i][j].type = 2;
        totalEnemies += 2;
      }
      else
      {
        enemies[i][j].type = 1;
        totalEnemies++;
      }
    }
  }

  for (size_t i = 0; i < 8; i++)
  {
    lastMoves[i] = NONE;
  }
  
  player.posX = ST7735_WIDTH / 2;
  player.posY = ST7735_HEIGHT - 20;
  player.lives = 3;
  player.invincible = false;

  gameState = START;
}

/**
 * @brief Reset the game
 */
void SPACEINV_Reset()
{
  if (timer < RESET_TIME)
    return;
  gameState = INITIALIZING;
  totalEnemies = 0;
  score = 0;
  timer = 0;
  enemyMoveTimer = 0;
  playerMoveTimer = 0;
  playerShootTimer = 0;
  bulletMoveTimer = 0;
  playerInvincibleTimer = 0;
  bulletsFired = 0;

  for (size_t i = 0; i < BULLET_ARRAY_SIZE; i++)
  {
    bullets[i].posX = 0;
    bullets[i].posY = 0;
    bullets[i].active = false;
  }

  for (size_t i = 0; i < ENEMY_ARRAY_SIZE_Y; i++)
  {
    for (size_t j = 0; j < ENEMY_ARRAY_SIZE_X; j++)
    {
      enemies[i][j].posX = 15 + j * 15;
      enemies[i][j].posY = 15 + i * 10;
      enemies[i][j].alive = true;
      if (i % 2 == 0 && j % 2 == 0)
      {
        enemies[i][j].type = 2;
        totalEnemies += 2;
      }
      else
      {
        enemies[i][j].type = 1;
        totalEnemies++;
      }
    }
  }
  player.posX = ST7735_WIDTH / 2;
  player.posY = ST7735_HEIGHT - 20;
  player.lives = 3;
  player.invincible = false;

  for (size_t i = 0; i < 8; i++)
  {
    lastMoves[i] = NONE;
  }
  gameState = START;
  timer = 0;
}

/**
 * @brief Invoke when player shoots
 */
void SPACEINV_PlayerShoot()
{
  if (playerShootTimer < PLAYER_SHOOT_TIME)
    return;
  if (bulletsFired >= BULLET_ARRAY_SIZE)
    return;
  TIM2->CCR1 = 5;
  for (size_t i = 0; i < BULLET_ARRAY_SIZE; i++)
  {
    if (bullets[i].active)
      continue;
    bullets[i].active = true;
    bullets[i].posX = player.posX + PLAYER_SIZE / 2 - BULLET_SIZE / 2;
    bullets[i].posY = player.posY - BULLET_SIZE;
    bulletsFired++;
    break;
  }
  playerShootTimer = 0;
}

/**
 * @brief Move the bullets
 */
void SPACEINV_MoveBullets()
{
  for (size_t i = 0; i < BULLET_ARRAY_SIZE; i++)
  {
    if (!bullets[i].active)
      continue;
    bullets[i].posY += -BULLET_SPEED;
    if (bullets[i].posY < 0)
    {
      bullets[i].active = false;
      bulletsFired--;
      score -= 2;
    }
  }
}

/**
 * @brief Move the enemies
 */
void SPACEINV_MoveEnemies()
{
  if (enemyMoveTimer < (ENEMY_MOVE_TIME))
    return;
  for (size_t i = 0; i < ENEMY_ARRAY_SIZE_Y; i++)
  {
    for (size_t j = 0; j < ENEMY_ARRAY_SIZE_X; j++)
    {
      if (!enemies[i][j].alive)
        continue;
      enemies[i][j].posY += ENEMY_SPEED;
    }
  }
}

/**
 * @brief Change the player positiob by x and y
 * @param x horizontal position
 * @param y vertical position
 */
void SPACEINV_ChangePlayerPos(int8_t x, int8_t y)
{
  if (player.posX + x < 0)
  {
    player.posX += x - (player.posX + x);
  }
  else if (player.posX + x > ST7735_WIDTH - PLAYER_SIZE)
  {
    player.posX += ST7735_WIDTH - PLAYER_SIZE - player.posX;
  }
  else
  {
    player.posX += x;
  }
  if (player.posY + y < 0)
  {
    player.posY += y - (player.posY + y);
  }
  else if (player.posY + y > ST7735_HEIGHT - PLAYER_SIZE)
  {
    player.posY += ST7735_HEIGHT - PLAYER_SIZE - player.posY;
  }
  else
  {
    player.posY += y;
  }
}

/**
 * @brief Move the player
 */
void SPACEINV_MovePlayer()
{
  if (playerMoveTimer < PLAYER_MOVE_TIME)
    return;
  direction_t dir = STJ_ReadJoystick();
  if (dir == NONE)
    return;
  if (dir == UP_LEFT)
  {
    SPACEINV_ChangePlayerPos(-5, -5);
  }
  else if (dir == UP_RIGHT)
  {
    SPACEINV_ChangePlayerPos(5, -5);
  }
  else if (dir == DOWN_LEFT)
  {
    SPACEINV_ChangePlayerPos(-5, 5);
  }
  else if (dir == DOWN_RIGHT)
  {
    SPACEINV_ChangePlayerPos(5, 5);
  }
  else if (dir == LEFT)
  {
    SPACEINV_ChangePlayerPos(-5, 0);
  }
  else if (dir == RIGHT)
  {
    SPACEINV_ChangePlayerPos(5, 0);
  }
  else if (dir == UP)
  {
    SPACEINV_ChangePlayerPos(0, -5);
  }
  else if (dir == DOWN)
  {
    SPACEINV_ChangePlayerPos(0, 5);
  }
  for (size_t i = 0; i < 7; i++)
  {
    lastMoves[i] = lastMoves[i + 1];
  }
  lastMoves[7] = dir;
}

/**
 * @brief Check if the player entered the konami code
 */
void SPACEINV_CheckKonamiCode()
{
  if (lastMoves[0] == UP && lastMoves[1] == UP && lastMoves[2] == DOWN && lastMoves[3] == DOWN && lastMoves[4] == LEFT && lastMoves[5] == RIGHT && lastMoves[6] == LEFT && lastMoves[7] == RIGHT)
  {
    gameState = KONAMICODE;
    score = 0;
  }
}

/**
 * @brief Check collisions between bullets, enemies and player
 */
void SPACEINV_CheckCollisions()
{
  for (size_t i = 0; i < ENEMY_ARRAY_SIZE_Y; i++)
  {
    for (size_t j = 0; j < ENEMY_ARRAY_SIZE_X; j++)
    {
      if (!enemies[i][j].alive)
        continue;
      for (size_t k = 0; k < BULLET_ARRAY_SIZE; k++)
      {
        if (!bullets[k].active)
          continue;
        if ((bullets[k].posX >= enemies[i][j].posX && bullets[k].posX <= enemies[i][j].posX + ENEMY_SIZE && bullets[k].posY >= enemies[i][j].posY && bullets[k].posY <= enemies[i][j].posY + ENEMY_SIZE) ||
            (bullets[k].posX + BULLET_SIZE >= enemies[i][j].posX && bullets[k].posX + BULLET_SIZE <= enemies[i][j].posX + ENEMY_SIZE && bullets[k].posY >= enemies[i][j].posY && bullets[k].posY <= enemies[i][j].posY + ENEMY_SIZE))
        {
          if (enemies[i][j].type == 2)
          {
            enemies[i][j].type = 1;
            score += 5;
          }
          else
          {
            enemies[i][j].alive = false;
            score += 10;
          }
          bullets[k].active = false;
          bulletsFired--;
          totalEnemies--;
        }
      }
      if (player.invincible)
        continue;
      if ((player.posX >= enemies[i][j].posX && player.posX <= enemies[i][j].posX + ENEMY_SIZE && player.posY >= enemies[i][j].posY && player.posY <= enemies[i][j].posY + ENEMY_SIZE) ||
          (player.posX + PLAYER_SIZE >= enemies[i][j].posX && player.posX + PLAYER_SIZE <= enemies[i][j].posX + ENEMY_SIZE && player.posY >= enemies[i][j].posY && player.posY <= enemies[i][j].posY + ENEMY_SIZE) ||
          (player.posX >= enemies[i][j].posX && player.posX <= enemies[i][j].posX + ENEMY_SIZE && player.posY + PLAYER_SIZE >= enemies[i][j].posY && player.posY + PLAYER_SIZE <= enemies[i][j].posY + ENEMY_SIZE) ||
          (player.posX + PLAYER_SIZE >= enemies[i][j].posX && player.posX + PLAYER_SIZE <= enemies[i][j].posX + ENEMY_SIZE && player.posY + PLAYER_SIZE >= enemies[i][j].posY && player.posY + PLAYER_SIZE <= enemies[i][j].posY + ENEMY_SIZE))
      {
        player.lives--;
        if (player.lives == 0)
        {
          gameState = GAME_OVER;
        }
        player.invincible = true;
        playerInvincibleTimer = PLAYER_INVINCIBLE_TIME;
      }
    }
  }
}

/**
 * @brief Check if enemies have reached the bottom of the screen
 */
void SPACEINV_CheckEnemyBorderCollisions()
{
  for (size_t i = 0; i < ENEMY_ARRAY_SIZE_Y; i++)
  {
    for (size_t j = 0; j < ENEMY_ARRAY_SIZE_X; j++)
    {
      if (!enemies[i][j].alive)
        continue;
      if (enemies[i][j].posY + ENEMY_SIZE >= ST7735_HEIGHT)
      {
        gameState = GAME_OVER;
      }
    }
  }
}

/**
 * @brief Check if the player has won
 */
void SPACEINV_CheckWin()
{
  if (totalEnemies == 0)
    gameState = WIN;
}

/**
 * @brief Update game logic and draw game screen
 */
void SPACEINV_Update()
{
  enemyMoveTimer++;
  playerMoveTimer++;
  playerShootTimer++;
  bulletMoveTimer++;

  SPACEINV_MoveEnemies();
  SPACEINV_MovePlayer();
  SPACEINV_MoveBullets();
  SPACEINV_CheckCollisions();
  SPACEINV_CheckWin();
  SPACEINV_CheckEnemyBorderCollisions();

  SIGFX_DrawGameScreen((enemy_t *)enemies, player, (bullet_t *)bullets);

  if (enemyMoveTimer >= (ENEMY_MOVE_TIME))
    enemyMoveTimer = 0;
  if (playerMoveTimer >= PLAYER_MOVE_TIME)
    playerMoveTimer = 0;
  if (bulletMoveTimer >= BULLET_MOVE_TIME)
    bulletMoveTimer = 0;
  if (playerInvincibleTimer > 0)
    playerInvincibleTimer--;
  else
    player.invincible = false;
  SPACEINV_CheckKonamiCode();
  TIM2->CCR1 = 0;
}

/**
 * @brief Run the game
 */
void SPACEINV_Run()
{
  switch (gameState)
  {
  case INITIALIZING:
    break;
  case START:
    SIGFX_DrawStartScreen();
    break;
  case RUNNING:
    SPACEINV_Update();
    break;
  case WIN:
    timer++;
    SIGFX_DrawWinScreen();
    break;
  case GAME_OVER:
    timer++;
    SIGFX_DrawGameOverScreen();
    break;
  case KONAMICODE:
    SIGFX_DrawKonamiCodeScreen();
    break;
  default:
    break;
  }
}
