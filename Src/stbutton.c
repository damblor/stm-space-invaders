#include "st7735.h"
#include "stdio.h"
#include "stdlib.h"
#include "main.h"
#include "stm32f4xx_hal.h"
#include "stbutton.h"

#define STB_PRESS_THRESHOLD 500

/**
 * @brief Get the state of a button
 * @param GPIOx GPIO family
 * @param GPIO_Pin GPIO pin
 * @return true if set, false if reset
 */
bool buttonState(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin)
{
  return HAL_GPIO_ReadPin(GPIOx, GPIO_Pin) == GPIO_PIN_SET;
}

/**
 * @brief Initialize a button
 * @param button button_t struct
 * @param GPIOx GPIO family
 * @param GPIO_Pin GPIO pin
 */
void STB_InitButton(button_t *button, GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin)
{
  button->GPIOx = GPIOx;
  button->GPIO_Pin = GPIO_Pin;
  button->buttonDownTs = 0;
  button->buttonUpTs = 0;
  button->buttonDown = false;
}

/**
 * @brief Get the button event
 * @param button button_t struct
 * @return button event
 */
buttonEvent_t STB_GetButtonEvent(button_t *button)
{
  buttonEvent_t buttonEvent = NO_PRESS;
  uint32_t now = HAL_GetTick();

  if (buttonState(button->GPIOx, button->GPIO_Pin))
  {
    if (!button->buttonDown)
    {
      button->buttonDown = true;
      button->buttonDownTs = now;
    }
  }
  else
  {
    if (button->buttonDown)
    {
      button->buttonDown = false;
      button->buttonUpTs = now;
      if (button->buttonUpTs - button->buttonDownTs < STB_PRESS_THRESHOLD)
      {
        buttonEvent = SINGLE_PRESS;
      }
    }
  }

  return buttonEvent;
}
