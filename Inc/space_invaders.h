#ifndef __SPACEINVADERS_H
#define __SPACEINVADERS_H

#include <stdint.h>
#include <stdbool.h>

#define ENEMY_ARRAY_SIZE_X 7
#define ENEMY_ARRAY_SIZE_Y 3
#define ENEMY_SIZE 8
#define ENEMY_SPEED 5

#define PLAYER_SIZE 10
#define PLAYER_SPEED 5
#define PLAYER_INVINCIBLE_TIME 30

#define BULLET_ARRAY_SIZE 30
#define BULLET_SIZE 3
#define BULLET_SPEED 2

#define ENEMY_MOVE_TIME 60
#define PLAYER_MOVE_TIME 5
#define PLAYER_SHOOT_TIME 5
#define BULLET_MOVE_TIME 5

#define RESET_TIME 60

typedef struct
{
  uint16_t posX;
  uint16_t posY;
  uint8_t type;
  bool alive;
} enemy_t;

typedef struct
{
  uint16_t posX;
  uint16_t posY;
  bool alive;
  uint8_t lives;
  bool invincible;
} player_t;

typedef struct
{
  int16_t posX;
  int16_t posY;
  bool active;
} bullet_t;

typedef enum
{
  INITIALIZING,
  START,
  RUNNING,
  GAME_OVER,
  WIN,
  KONAMICODE,
} game_state_t;

extern game_state_t gameState;
extern int16_t score;

#ifdef __cplusplus
extern "C"
{
#endif

  void SPACEINV_Init();
  void SPACEINV_Reset();
  void SPACEINV_PlayerShoot();
  void SPACEINV_Run();

#ifdef __cplusplus
}
#endif

#endif /* __SPACEINVADERS_H */
