#include "stjoystick.h"

uint32_t XY[2];

/**
 * @brief Get the joystick direction
 * @return direction_t direction
 */
direction_t STJ_ReadJoystick()
{
  if (XY[0] < 1500 && XY[1] < 1500)
    return UP_LEFT;
  else if (XY[0] > 2500 && XY[1] < 1500)
    return UP_RIGHT;
  else if (XY[0] < 1500 && XY[1] > 2500)
    return DOWN_LEFT;
  else if (XY[0] > 2500 && XY[1] > 2500)
    return DOWN_RIGHT;
  else if (XY[0] < 1500)
    return LEFT;
  else if (XY[0] > 2500)
    return RIGHT;
  else if (XY[1] < 1500)
    return UP;
  else if (XY[1] > 2500)
    return DOWN;
  return NONE;
}
