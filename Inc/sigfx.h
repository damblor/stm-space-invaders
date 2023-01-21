#ifndef __SIGFX_H
#define __SIGFX_H

#include "space_invaders.h"

#ifdef __cplusplus
extern "C"
{
#endif

  void SIGFX_DrawGameScreen(enemy_t *enemies, player_t player, bullet_t *bullets);
  void SIGFX_DrawWinScreen();
  void SIGFX_DrawGameOverScreen();
  void SIGFX_DrawStartScreen();
  void SIGFX_DrawKonamiCodeScreen();
  void SIGFX_DrawInit();

#ifdef __cplusplus
}
#endif

#endif /* __SIGFX_H */