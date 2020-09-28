#include "uart.h"
#include "stm32f4xx_hal.h"

Boolean acquired;
Boolean in_progress;
UART_HandleTypeDef uart = {0};
DMA_HandleTypeDef uart_dma = {0};

UARTReadCallback currentHandler;

Boolean UART_Init()
{
	uart.Instance = USART2;
	uart.Init.BaudRate = 115200;
	uart.Init.WordLength = USART_WORDLENGTH_8B;
	uart.Init.StopBits = USART_STOPBITS_2;
	uart.Init.Parity = USART_PARITY_NONE;
	uart.Init.Mode = USART_MODE_TX_RX;
	uart.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	uart.Init.OverSampling = UART_OVERSAMPLING_16;
	if (HAL_UART_Init(&uart) != HAL_OK)
		return FALSE;

	HAL_NVIC_SetPriority(USART2_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(USART2_IRQn);

	USART2->CR3 |= USART_CR3_ONEBIT; //Ignore noises!

	return TRUE;
}

Boolean UART_StartReceive(UARTReadCallback callback, uint8_t* buffer, uint32_t size)
{
	if(in_progress || !acquired)
		return FALSE;

	currentHandler = callback;
	in_progress = TRUE;
	if(HAL_UART_Receive_DMA(&uart, buffer, size) != HAL_OK)
		return FALSE;

	return TRUE;
}

Boolean UART_Acquire()
{
	if(acquired)
		return FALSE;
	acquired = TRUE;
	return TRUE;
}

void UART_Free()
{
	acquired = FALSE;
}

void HAL_UART_MspInit(UART_HandleTypeDef* huart)
{
	if (huart->Instance == USART2)
	{
		__HAL_RCC_USART2_CLK_ENABLE();
		__HAL_RCC_GPIOA_CLK_ENABLE();

		GPIO_InitTypeDef GPIO_InitStruct = {0};
		GPIO_InitStruct.Pin = GPIO_PIN_2|GPIO_PIN_3;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
		GPIO_InitStruct.Alternate = GPIO_AF7_USART2;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

		__HAL_RCC_DMA1_CLK_ENABLE();
		uart_dma.Instance = DMA1_Stream5;
		uart_dma.Init.Channel = DMA_CHANNEL_4;
		uart_dma.Init.Direction = DMA_PERIPH_TO_MEMORY;
		uart_dma.Init.PeriphInc = DMA_PINC_DISABLE;
		uart_dma.Init.MemInc = DMA_MINC_ENABLE;
		uart_dma.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
		uart_dma.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
		uart_dma.Init.Priority = DMA_PRIORITY_MEDIUM;
		uart_dma.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
		if(HAL_DMA_Init(&uart_dma) != HAL_OK)
			return;

		__HAL_LINKDMA(huart, hdmarx, uart_dma);

		HAL_NVIC_SetPriority(DMA1_Stream5_IRQn, 0, 0);
		HAL_NVIC_EnableIRQ(DMA1_Stream5_IRQn);
	}
}

void DMA1_Stream5_IRQHandler(void)
{
	HAL_DMA_IRQHandler(&uart_dma);
}

void USART2_IRQHandler(void)
{
	HAL_UART_IRQHandler(&uart);
}

void HAL_UART_RxHalfCpltCallback(UART_HandleTypeDef *huart)
{
	in_progress = TRUE;
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if(currentHandler)
		currentHandler();

	in_progress = FALSE;
}

void UART_DMAReceiveCplt(DMA_HandleTypeDef *huart_dma)
{


}
