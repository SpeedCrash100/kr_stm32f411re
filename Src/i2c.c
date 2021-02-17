#include "i2c.h"

#include "stm32f4xx_hal.h"

/// @ingroup I2C
/// @{

/// Дескриптор шины %I2C
struct I2C {
  /// Получен ли доступ к шине кем-то
  Boolean acquired;
  /// Есть ли операции в процессе работы
  Boolean in_progress;

  /// HAL дескриптор I2C шины
  I2C_HandleTypeDef hal_i2c;
  /// Адрес дисплея
  uint16_t address;

  /// Функция окончания операции передачи
  I2CCallback callback;
};

/// Инициализирована ли шина
Boolean g_I2C_initialized = FALSE;
/// Глобальный дескриптор шины
I2C g_I2C = {0};

I2C* I2C_Init() {
  if (g_I2C_initialized) return &g_I2C;

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  __HAL_RCC_GPIOB_CLK_ENABLE();
  GPIO_InitStruct.Pin = GPIO_PIN_8 | GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF4_I2C1;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  __HAL_RCC_I2C1_CLK_ENABLE();
  HAL_NVIC_SetPriority(I2C1_EV_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(I2C1_EV_IRQn);

  I2C_HandleTypeDef* i2c = &g_I2C.hal_i2c;
  i2c->Instance = I2C1;
  i2c->Init.ClockSpeed = 400000;
  i2c->Init.DutyCycle = I2C_DUTYCYCLE_2;
  i2c->Init.OwnAddress1 = 0;
  i2c->Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  i2c->Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  i2c->Init.OwnAddress2 = 0;
  i2c->Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  i2c->Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(i2c) != HAL_OK) {
    return NULL;
  }

  g_I2C.acquired = FALSE;
  g_I2C.in_progress = FALSE;
  g_I2C.address = 0x3C << 1;
  g_I2C.callback = NULL;

  g_I2C_initialized = TRUE;
  return &g_I2C;
}

Boolean I2C_Acquire(I2C* hnd) {
  /// Если кто-то уже получил доступ, то вернуть FALSE; иначе получить доступ и
  /// вернуть TRUE
  if (hnd->acquired) return FALSE;
  hnd->acquired = TRUE;
  return TRUE;
}

void I2C_Free(I2C* hnd) {
  /// Всегда освобождать
  hnd->acquired = FALSE;
}

Boolean I2C_SendCommand(I2C* hnd, uint8_t cmd) {
  /// Проверка того, что доступ был получен и сейчас не выполняется другой
  /// операции
  if (!hnd->acquired || hnd->in_progress) return FALSE;

  hnd->in_progress = TRUE;

  /// Отправить команду
  HAL_StatusTypeDef status = HAL_I2C_Mem_Write(
      &g_I2C.hal_i2c, g_I2C.address, 0x00, I2C_MEMADD_SIZE_8BIT, &cmd, 1, 1000);
  hnd->in_progress = FALSE;
  if (status != HAL_OK) {
    return FALSE;
  }

  return TRUE;
}

Boolean I2C_SendData(I2C* hnd, I2CCallback callback, uint8_t* data,
                     uint16_t size) {
  /// Проверка того, что доступ был получен и сейчас не выполняется другой
  /// операции
  if (!hnd->acquired || hnd->in_progress) return FALSE;

  hnd->in_progress = TRUE;
  hnd->callback = callback;

  /// Отправка команды используя модель прерываний
  HAL_StatusTypeDef status = HAL_I2C_Mem_Write_IT(
      &g_I2C.hal_i2c, g_I2C.address, 0x40, I2C_MEMADD_SIZE_8BIT, data, size);
  if (status != HAL_OK) {
    return FALSE;
  }
  return TRUE;
}

/**
 * @brief I2C1_EV_IRQHandler прерывание по событию от I2C1 подсистемы
 * @details передает управление обработчику HAL
 */
void I2C1_EV_IRQHandler() { HAL_I2C_EV_IRQHandler(&g_I2C.hal_i2c); }

/**
 * @brief HAL_I2C_MemTxCpltCallback вызывается при окончании передачи данных
 * @details вызывает функцию обратной связи о завершении передчи данных
 */
void HAL_I2C_MemTxCpltCallback(I2C_HandleTypeDef* hi2c) {
  if (hi2c->Instance == I2C1) {
    if (g_I2C.callback) g_I2C.callback();

    g_I2C.in_progress = FALSE;
  }
}

/// @}
