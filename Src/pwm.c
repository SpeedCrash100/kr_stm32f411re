#include "pwm.h"

#include <limits.h>
#include <math.h>
#include <stdlib.h>

#include "stm32f4xx_hal.h"

#define BUFFER_SIZE 102400

struct PWM {
  TIM_HandleTypeDef tim1;
  TIM_OC_InitTypeDef chanConfig;

  uint8_t width;
  uint32_t freq;

  Boolean valueCaptured;
  uint16_t filter;
  uint8_t buffer[BUFFER_SIZE];

  // TMP
  uint32_t bufferPos;
  uint32_t bufferWritePos;
};

Boolean g_PWM_initialized = FALSE;
PWM g_PWM = {0};

PWM* PWM_Init() {
  if (g_PWM_initialized) return &g_PWM;

  TIM_HandleTypeDef* tim1 = &g_PWM.tim1;
  TIM_OC_InitTypeDef* chanConfig = &g_PWM.chanConfig;

  tim1->Instance = TIM1;
  tim1->Init.Prescaler = 38;
  tim1->Init.CounterMode = TIM_COUNTERMODE_UP;
  tim1->Init.Period = 256;
  tim1->Init.ClockDivision = TIM_CLOCKPRESCALER_DIV1;
  tim1->Init.RepetitionCounter = 0;
  if (HAL_TIM_Base_Init(tim1) != HAL_OK) return NULL;

  if (HAL_TIM_PWM_Init(tim1) != HAL_OK) return NULL;

  HAL_NVIC_SetPriority(TIM1_CC_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(TIM1_CC_IRQn);

  chanConfig->OCMode = TIM_OCMODE_PWM1;
  chanConfig->OCPolarity = TIM_OCPOLARITY_HIGH;
  chanConfig->Pulse = 0;
  if (HAL_TIM_PWM_ConfigChannel(tim1, chanConfig, TIM_CHANNEL_1) != HAL_OK)
    return NULL;

  g_PWM.freq = 1000;
  g_PWM.width = 0;

  // TODO! Queue
  g_PWM.valueCaptured = TRUE;
  g_PWM.filter = 0;
  g_PWM.bufferPos = 0;
  g_PWM.bufferWritePos = 0;

  g_PWM_initialized = TRUE;

  return &g_PWM;
}

int32_t PWM_SetFreq(PWM* hnd, int32_t freq) {
  uint32_t prescal = 1;
  uint32_t realFreq = SystemCoreClock / ((256 + 1) * (prescal + 1));
  uint32_t delta = abs(freq - (int)realFreq);
  while (prescal < 65535) {
    prescal++;
    realFreq = SystemCoreClock / ((256 + 1) * (prescal + 1));
    uint32_t newDelta = abs(freq - (int)realFreq);
    if (newDelta < delta) {
      delta = newDelta;
    } else {
      prescal--;  // Revert
      break;
    }
  }

  __HAL_TIM_SET_PRESCALER(&hnd->tim1, prescal);
  return realFreq = SystemCoreClock / ((256 + 1) * (prescal + 1));
}

void PWM_AddWidth(PWM* hnd, uint8_t width) {
  // TODO! Check queue overflow
  int32_t usage = 0;
  PWM_GetBufferUsage(hnd, &usage);
  if (usage == 100) {
    // Overflow filter

    uint16_t scaledWidth = (uint16_t)(width)*UINT16_MAX / 256;
    if (hnd->valueCaptured == TRUE) {  // Reset filter
      hnd->valueCaptured = FALSE;
      hnd->filter = scaledWidth;
    }

    uint32_t kKoff = 1;
    uint32_t nKoff = 100;

    //                   (nKoff - kKoff)             kKoff
    // f_(i+1) = f_(i) * ---------------  + new_f * -------
    //                        nKoff                  nKoffs

    hnd->filter = (uint16_t)((uint32_t)(hnd->filter) * (nKoff - kKoff) / nKoff +
                             (uint32_t)(scaledWidth)*kKoff / nKoff);

  } else {
    // TODO! Add to queue
    hnd->buffer[hnd->bufferWritePos] = width;
    hnd->bufferWritePos++;
    hnd->bufferWritePos %= BUFFER_SIZE;
  }
}

void PWM_Start(PWM* hnd) { HAL_TIMEx_OCN_Start_IT(&hnd->tim1, TIM_CHANNEL_1); }

void PWM_Stop(PWM* hnd) { HAL_TIMEx_OCN_Stop_IT(&hnd->tim1, TIM_CHANNEL_1); }

void PWM_GetBufferUsage(PWM* hnd, int32_t* usage) {
  if (usage == NULL) return;

  // TODO! Use queue size
  if (hnd->bufferPos <= hnd->bufferWritePos) {
    *usage = (hnd->bufferWritePos - hnd->bufferPos) * 100 / BUFFER_SIZE;
  } else {
    *usage = (BUFFER_SIZE - (hnd->bufferPos - hnd->bufferWritePos)) * 100 /
             BUFFER_SIZE;
  }
}

void HAL_TIM_Base_MspInit(TIM_HandleTypeDef* htim_base) {
  if (htim_base->Instance == TIM1) {
    __HAL_RCC_TIM1_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    /**TIM1 GPIO Configuration
    PA7     ------> TIM1_CH1N
    */
    GPIO_InitStruct.Pin = GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF1_TIM1;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
  }
}

void TIM1_CC_IRQHandler() {
  __HAL_TIM_CLEAR_IT(&g_PWM.tim1, TIM_IT_CC1);

  __HAL_TIM_SET_COMPARE(&g_PWM.tim1, TIM_CHANNEL_1,
                        g_PWM.buffer[g_PWM.bufferPos]);

  if (g_PWM.valueCaptured == FALSE) {
    g_PWM.valueCaptured = TRUE;
    // TODO! Insert in queue filtered value
    g_PWM.buffer[g_PWM.bufferWritePos] =
        (uint16_t)((uint32_t)(g_PWM.filter) * 256 / UINT16_MAX);
    g_PWM.bufferWritePos++;
    g_PWM.bufferWritePos %= BUFFER_SIZE;
  }

  g_PWM.bufferPos++;
  g_PWM.bufferPos %= BUFFER_SIZE;
}
