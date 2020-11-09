#include "adc.h"

#include <stdlib.h>

#include "stm32f4xx_hal.h"

/// @ingroup ADC
/// @{

struct STM32ADC {
  /// HAL дескриптор АЦП
  ADC_HandleTypeDef hal_adc_handle;
  /// HAL дескриптор DMA для АЦП
  DMA_HandleTypeDef hal_dma_handle;

  /// Результат с АЦП
  uint16_t adc_result;
};

/// Инициализирована ли АЦП
Boolean g_ADC_initialized = FALSE;
/// Глобальный дескриптор АЦП
STM32ADC g_ADC_struct = {0};

STM32ADC* ADC_Init() {
  if (g_ADC_initialized) return &g_ADC_struct;

  /// - Настройка порта PA0 в аналоговый режим. На этом порту расположен 0 канал
  /// ADC1
  // Configuring for PA0 (ADC1 Channel 0)
  __HAL_RCC_GPIOA_CLK_ENABLE();
  GPIO_InitTypeDef gpioInitStruct = {0};
  gpioInitStruct.Pin = GPIO_PIN_0;
  gpioInitStruct.Mode = GPIO_MODE_ANALOG;
  gpioInitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &gpioInitStruct);

  // Init ADC1 subsystem of MC

  /// - Настройка АЦП:
  ///     - Режим работы: непрерывный
  ///     - Запуск по запросу от программы
  ///     - Данные расположены справа в буфере
  ///     - Количество битов: 12
  ADC_HandleTypeDef* adc = &g_ADC_struct.hal_adc_handle;
  adc->Instance = ADC1;
  adc->Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV8;
  adc->Init.ScanConvMode = ENABLE;
  adc->Init.ContinuousConvMode = ENABLE;
  adc->Init.DiscontinuousConvMode = DISABLE;
  adc->Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  adc->Init.ExternalTrigConv = ADC_SOFTWARE_START;
  adc->Init.DataAlign = ADC_DATAALIGN_RIGHT;
  adc->Init.NbrOfConversion = 1;
  adc->Init.DMAContinuousRequests = ENABLE;
  adc->Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  adc->Init.Resolution = ADC_RESOLUTION_12B;
  if (HAL_ADC_Init(adc) != HAL_OK) {
    return NULL;
  }

  /// Настройка 0 канала АЦП с максимальным размером выборки
  ADC_ChannelConfTypeDef sConfig = {0};
  sConfig.Channel = ADC_CHANNEL_0;
  sConfig.Rank = 1;
  sConfig.SamplingTime = ADC_SAMPLETIME_480CYCLES;
  if (HAL_ADC_ConfigChannel(adc, &sConfig) != HAL_OK) {
    return NULL;
  }

  /// Запуск DMA
  HAL_ADC_Start_DMA(adc, (uint32_t*)(&g_ADC_struct.adc_result),
                    sizeof(g_ADC_struct.adc_result));

  g_ADC_initialized = TRUE;
  return &g_ADC_struct;
}

uint16_t ADC_Get(STM32ADC* handle) {
  if (!handle) return 0;

  return handle->adc_result;
}

void HAL_ADC_MspInit(ADC_HandleTypeDef* hadc) {
  if (hadc->Instance == ADC1) {
    __HAL_RCC_ADC1_CLK_ENABLE();
    __HAL_RCC_DMA2_CLK_ENABLE();

    DMA_HandleTypeDef* dma = &g_ADC_struct.hal_dma_handle;

    dma->Instance = DMA2_Stream0;
    dma->Init.Channel = DMA_CHANNEL_0;
    dma->Init.Direction = DMA_PERIPH_TO_MEMORY;
    dma->Init.PeriphInc = DMA_PINC_DISABLE;
    dma->Init.MemInc = DMA_MINC_ENABLE;
    dma->Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
    dma->Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
    dma->Init.Mode = DMA_CIRCULAR;
    dma->Init.Priority = DMA_PRIORITY_LOW;
    dma->Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    if (HAL_DMA_Init(dma) != HAL_OK) {
      return;
    }

    __HAL_LINKDMA(hadc, DMA_Handle, *dma);
  }
}

/// @}
