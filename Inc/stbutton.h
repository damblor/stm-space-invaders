#ifndef __STBUTTON_H
#define __STBUTTON_H

#include "main.h"
#include <stdint.h>
#include <stdbool.h>

typedef enum
{
  NO_PRESS,
  SINGLE_PRESS,
} buttonEvent_t;

typedef struct
{
  GPIO_TypeDef *GPIOx;
  uint16_t GPIO_Pin;
  uint32_t buttonDownTs;
  uint32_t buttonUpTs;
  bool buttonDown;
} button_t;

#ifdef __cplusplus
extern "C"
{
#endif

  void STB_InitButton(button_t *button, GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);
  buttonEvent_t STB_GetButtonEvent(button_t *button);

#ifdef __cplusplus
}
#endif

#endif /* __STBUTTON_H */
