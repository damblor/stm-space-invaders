#ifndef __STJOYSTICK_H
#define __STJOYSTICK_H

#include <stdint.h>

typedef enum
{
  NONE,
  LEFT,
  RIGHT,
  UP,
  DOWN,
  UP_LEFT,
  UP_RIGHT,
  DOWN_LEFT,
  DOWN_RIGHT,
} direction_t;

extern uint32_t XY[2];

#ifdef __cplusplus
extern "C"
{
#endif

  direction_t STJ_ReadJoystick();

#ifdef __cplusplus
}
#endif

#endif /* __STJOYSTICK_H */
