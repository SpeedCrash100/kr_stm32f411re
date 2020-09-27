#include "adc.h"
#include "stm32f4xx_hal.h"

ADC_HandleTypeDef adc;
DMA_HandleTypeDef dma;
uint16_t result;

Boolean ADC_Init()
{
	__HAL_RCC_GPIOA_CLK_ENABLE();
	// Configuring for PA0 (ADC1 Channel 0)
	GPIO_InitTypeDef gpioInitStruct = {0};

	gpioInitStruct.Pin = GPIO_PIN_0;
	gpioInitStruct.Mode = GPIO_MODE_ANALOG;
	gpioInitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOA, &gpioInitStruct);

	adc.Instance = ADC1;
	adc.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV8;
	adc.Init.ScanConvMode = ENABLE;
	adc.Init.ContinuousConvMode = ENABLE;
	adc.Init.DiscontinuousConvMode = DISABLE;
	adc.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
	adc.Init.ExternalTrigConv = ADC_SOFTWARE_START;
	adc.Init.DataAlign = ADC_DATAALIGN_RIGHT;
	adc.Init.NbrOfConversion = 1;
	adc.Init.DMAContinuousRequests = ENABLE;
	adc.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
	adc.Init.Resolution = ADC_RESOLUTION_8B;
	if(HAL_ADC_Init(&adc) != HAL_OK)
	{
		return FALSE;
	}

	ADC_ChannelConfTypeDef sConfig = {0};
	sConfig.Channel = ADC_CHANNEL_0;
	sConfig.Rank = 1;
	sConfig.SamplingTime = ADC_SAMPLETIME_480CYCLES;
	if(HAL_ADC_ConfigChannel(&adc, &sConfig) != HAL_OK)
	{
		return FALSE;
	}

	HAL_ADC_Start_DMA(&adc, (uint32_t*)&result, sizeof(result));

	return TRUE;
}

uint16_t ADC_Get()
{
	return (uint16_t)result;
}

void HAL_ADC_MspInit(ADC_HandleTypeDef* hadc)
{
	if (hadc->Instance == ADC1)
	{
		__HAL_RCC_ADC1_CLK_ENABLE();
		__HAL_RCC_DMA2_CLK_ENABLE();

		dma.Instance = DMA2_Stream0;
		dma.Init.Channel = DMA_CHANNEL_0;
		dma.Init.Direction = DMA_PERIPH_TO_MEMORY;
		dma.Init.PeriphInc = DMA_PINC_DISABLE;
		dma.Init.MemInc = DMA_MINC_ENABLE;
		dma.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
		dma.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
		dma.Init.Mode = DMA_CIRCULAR;
		dma.Init.Priority = DMA_PRIORITY_LOW;
		dma.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
		if (HAL_DMA_Init(&dma) != HAL_OK)
		{
			return;
		}

		__HAL_LINKDMA(&adc, DMA_Handle, dma);
	}

}

