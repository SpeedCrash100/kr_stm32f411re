#include "pwm.h"

#include <limits.h>
#include <math.h>
#include <stdlib.h>

#include "queue.h"
#include "stm32f4xx_hal.h"

/// @ingroup PWM
/// @{

/// Размер очереди в байтах
#define BUFFER_SIZE 102400

struct PWM {
  /// Дескриптор HAL таймера, который создает ШИМ сигнал
  TIM_HandleTypeDef tim1;
  /// Дескриптор HAL канала, на котором генерируется ШИМ сигнал
  TIM_OC_InitTypeDef chanConfig;

  /// При переполнении буфера указывает было ли взято хотя бы 1 значение
  Boolean valueCaptured;
  /// Текущее значение фильтра значений
  uint16_t filter;
  /// Буфер, в котором размещается очередь
  uint8_t buffer[BUFFER_SIZE];
  /// Надстройка над буфером - очередь
  Queue queue;
};

/// Указывает инициализирована ли система
Boolean g_PWM_initialized = FALSE;
/// Глобальный объект подсистемы PWM
PWM g_PWM = {0};

PWM* PWM_Init() {
  if (g_PWM_initialized) return &g_PWM;

  TIM_HandleTypeDef* tim1 = &g_PWM.tim1;
  TIM_OC_InitTypeDef* chanConfig = &g_PWM.chanConfig;

  /// - Инициализация TIM1
  ///   - Пределитель: 38, меняется в процессе работы программы согласно
  ///   запросам пользователя.
  ///   - Таймер настроен на увеличение счетчика
  ///   - Сброс счетчика происходит раз в 256 тактов

  tim1->Instance = TIM1;
  tim1->Init.Prescaler = 38;
  tim1->Init.CounterMode = TIM_COUNTERMODE_UP;
  tim1->Init.Period = 256;
  tim1->Init.ClockDivision = TIM_CLOCKPRESCALER_DIV1;
  tim1->Init.RepetitionCounter = 0;
  if (HAL_TIM_Base_Init(tim1) != HAL_OK) return NULL;

  if (HAL_TIM_PWM_Init(tim1) != HAL_OK) return NULL;

  /// - Включение Capture-Compare прерывания

  HAL_NVIC_SetPriority(TIM1_CC_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(TIM1_CC_IRQn);

  /// Настройка 1 канала на режим работы ШИМ1. Полярность высокая.

  chanConfig->OCMode = TIM_OCMODE_PWM1;
  chanConfig->OCPolarity = TIM_OCPOLARITY_HIGH;
  chanConfig->Pulse = 0;
  if (HAL_TIM_PWM_ConfigChannel(tim1, chanConfig, TIM_CHANNEL_1) != HAL_OK)
    return NULL;

  /// - Инициализация очереди и фильтра

  g_PWM.valueCaptured = TRUE;
  g_PWM.filter = 0;
  g_PWM.queue = Queue_Create(g_PWM.buffer, sizeof(g_PWM.buffer));

  g_PWM_initialized = TRUE;

  return &g_PWM;
}

int32_t PWM_SetFreq(PWM* hnd, int32_t freq) {
  /// - Поиск такого пределителя, чтобы отклонение от целевой частоты было
  /// минимальным
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

  /// Установка пределителя в настройки таймера
  __HAL_TIM_SET_PRESCALER(&hnd->tim1, prescal);
  return realFreq = SystemCoreClock / ((256 + 1) * (prescal + 1));
}

void PWM_AddWidth(PWM* hnd, uint8_t width) {
  /// Если в очереди есть место, то просто добавить полученный байт в неё. В
  /// противном случае использовать бегущее среднее с коэффициентом 0.1

  if (Queue_Overflowed(&hnd->queue)) {
    // Overflow filter

    uint16_t scaledWidth = (uint16_t)(width)*UINT16_MAX / 256;
    if (hnd->valueCaptured == TRUE) {  // Reset filter
      hnd->valueCaptured = FALSE;
      hnd->filter = scaledWidth;
    }

    uint32_t kKoff = 10;
    uint32_t nKoff = 100;

    //                   (nKoff - kKoff)             kKoff
    // f_(i+1) = f_(i) * ---------------  + new_f * -------
    //                        nKoff                  nKoffs

    hnd->filter = (uint16_t)((uint32_t)(hnd->filter) * (nKoff - kKoff) / nKoff +
                             (uint32_t)(scaledWidth)*kKoff / nKoff);

  } else {
    Queue_Push(&hnd->queue, width);
  }
}

void PWM_Start(PWM* hnd) { HAL_TIMEx_OCN_Start_IT(&hnd->tim1, TIM_CHANNEL_1); }

void PWM_Stop(PWM* hnd) { HAL_TIMEx_OCN_Stop_IT(&hnd->tim1, TIM_CHANNEL_1); }

void PWM_GetBufferUsage(PWM* hnd, int32_t* usage) {
  if (usage == NULL) return;

  uint32_t size = Queue_Size(&hnd->queue);
  uint32_t capacity = Queue_Capacity(&hnd->queue);

  *usage = size * 100 / capacity;
}

/**
 * @brief HAL_TIM_Base_MspInit HAL процедура, используемая для инициализации
 * таймеров
 * @details Запускает тактирование TIM1 и GPIOA, а также переводит контакт PA7 в
 * режим работы с таймером
 *
 * @param htim_base дескриптор таймера
 */
void HAL_TIM_Base_MspInit(TIM_HandleTypeDef* htim_base) {
  if (htim_base->Instance == TIM1) {
    __HAL_RCC_TIM1_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    GPIO_InitStruct.Pin = GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF1_TIM1;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
  }
}

/**
 * @brief TIM1_CC_IRQHandler прерывание CC таймера TIM1
 * @details Устанавливает новое модулирующее значение из очереди. При
 * переполнении очереди вставляет отфильтрованное значение в очередь так как
 * прерывание освобождает 1 позицию.
 */
void TIM1_CC_IRQHandler() {
  __HAL_TIM_CLEAR_IT(&g_PWM.tim1, TIM_IT_CC1);

  uint8_t nextWidth = 0;
  if (Queue_Pop(&g_PWM.queue, &nextWidth)) {
    __HAL_TIM_SET_COMPARE(&g_PWM.tim1, TIM_CHANNEL_1, nextWidth);
  }

  if (g_PWM.valueCaptured == FALSE) {
    uint8_t filteredWidth =
        (uint8_t)((uint32_t)(g_PWM.filter) * 256 / UINT16_MAX);

    if (Queue_Push(&g_PWM.queue, filteredWidth)) {
      g_PWM.valueCaptured = TRUE;
    }
  }
}

/// @}
