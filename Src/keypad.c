#include "keypad.h"

#include "stm32f4xx_hal.h"

/// @ingroup Keypad
/// @{

/// Состояние клавиатуры
struct Keypad {
  KeyState keystates[KeyButtons_SIZE];
  uint32_t timePressed[KeyButtons_SIZE];
};

Keypad g_Keypad;

Keypad* Keypad_Init() {
  __HAL_RCC_GPIOC_CLK_ENABLE();
  GPIO_InitTypeDef gpio = {0};
  gpio.Pin = GPIO_PIN_1;
  gpio.Mode = GPIO_MODE_IT_FALLING;
  gpio.Pull = GPIO_PULLUP;
  gpio.Speed = GPIO_SPEED_LOW;
  HAL_GPIO_Init(GPIOC, &gpio);
  HAL_NVIC_EnableIRQ(EXTI1_IRQn);

  return &g_Keypad;
}

void EXTI1_IRQHandler(void) {
  EXTI->PR = EXTI_PR_PR1;
  if (HAL_GetTick() - g_Keypad.timePressed[KeyStartStop] >= 250) {
    g_Keypad.keystates[KeyStartStop] = Clicked;
    g_Keypad.timePressed[KeyStartStop] = HAL_GetTick();
  }
}

KeyState Keypad_GetState(Keypad* keypad, KeyButtons key) {
  if (keypad->keystates[key] == Clicked) {
    keypad->keystates[key] = Unpressed;
    return Clicked;
  }
  return Unpressed;
}

/// @}
