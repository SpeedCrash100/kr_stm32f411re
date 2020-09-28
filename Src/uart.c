#include "uart.h"
#include "stm32f4xx_hal.h"

struct UART {
	Boolean acquired;
	Boolean in_progress;

	UART_HandleTypeDef hal_uart;
	DMA_HandleTypeDef hal_dma_uart;

	UARTReadCallback currentHandlerHalf;
	UARTReadCallback currentHandlerFull;
};

Boolean g_UART_initialized = FALSE;
UART g_UART = {0};



UART* UART_Init()
{
	if(g_UART_initialized)
		return &g_UART;

	UART_HandleTypeDef* uart = &g_UART.hal_uart;

	uart->Instance = USART2;
	uart->Init.BaudRate = 115200;
	uart->Init.WordLength = USART_WORDLENGTH_8B;
	uart->Init.StopBits = USART_STOPBITS_2;
	uart->Init.Parity = USART_PARITY_NONE;
	uart->Init.Mode = USART_MODE_TX_RX;
	uart->Init.HwFlowCtl = UART_HWCONTROL_NONE;
	uart->Init.OverSampling = UART_OVERSAMPLING_16;
	if (HAL_UART_Init(uart) != HAL_OK)
		return NULL;

	HAL_NVIC_SetPriority(USART2_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(USART2_IRQn);

	USART2->CR3 |= USART_CR3_ONEBIT; //Ignore noises!

	g_UART.acquired = FALSE;
	g_UART.currentHandlerHalf = NULL;
	g_UART.currentHandlerFull = NULL;
	g_UART.in_progress = FALSE;
	g_UART_initialized = TRUE;

	return &g_UART;
}

Boolean UART_StartReceive(UART* hnd, UARTReadCallback callbackHalf, UARTReadCallback callbackFull, uint8_t* buffer, uint32_t size)
{
	if(hnd->in_progress || !hnd->acquired)
		return FALSE;

	hnd->currentHandlerHalf = callbackHalf;
	hnd->currentHandlerFull = callbackFull;
	hnd->in_progress = TRUE;
	if(HAL_UART_Receive_DMA(&hnd->hal_uart, buffer, size) != HAL_OK)
		return FALSE;

	return TRUE;
}

Boolean UART_Acquire(UART* hnd)
{
	if(hnd->acquired)
		return FALSE;
	hnd->acquired = TRUE;
	return TRUE;
}

void UART_Free(UART* hnd)
{
	hnd->acquired = FALSE;
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
		DMA_HandleTypeDef* uart_dma = &g_UART.hal_dma_uart;

		uart_dma->Instance = DMA1_Stream5;
		uart_dma->Init.Channel = DMA_CHANNEL_4;
		uart_dma->Init.Direction = DMA_PERIPH_TO_MEMORY;
		uart_dma->Init.PeriphInc = DMA_PINC_DISABLE;
		uart_dma->Init.MemInc = DMA_MINC_ENABLE;
		uart_dma->Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
		uart_dma->Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
		uart_dma->Init.Priority = DMA_PRIORITY_MEDIUM;
		uart_dma->Init.FIFOMode = DMA_FIFOMODE_DISABLE;
		if(HAL_DMA_Init(uart_dma) != HAL_OK)
			return;

		__HAL_LINKDMA(huart, hdmarx, *uart_dma);

		HAL_NVIC_SetPriority(DMA1_Stream5_IRQn, 0, 0);
		HAL_NVIC_EnableIRQ(DMA1_Stream5_IRQn);
	}
}

void DMA1_Stream5_IRQHandler(void)
{
	HAL_DMA_IRQHandler(&g_UART.hal_dma_uart);
}

void USART2_IRQHandler(void)
{
	HAL_UART_IRQHandler(&g_UART.hal_uart);
}

void HAL_UART_RxHalfCpltCallback(UART_HandleTypeDef *huart) {
	if(g_UART.currentHandlerHalf)
		g_UART.currentHandlerHalf();
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if(g_UART.currentHandlerFull)
		g_UART.currentHandlerFull();

	g_UART.in_progress = FALSE;
}
