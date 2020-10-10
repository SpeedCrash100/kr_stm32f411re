#include "maincontoller.h"
#include "stm32f4xx_hal.h"
#include "adc.h"
#include "display.h"
#include "pwm.h"
#include "uart.h"
#include <stdlib.h>

struct {
	MainStates state;

	STM32ADC* adc;
	Display* display;
	PWM* pwm;
	UART* uart;


	Boolean uart_working;
	Boolean uart_half;
	Boolean uart_full;

} mainContoller;

Boolean Init_RCC();
Boolean Init_IRQ();

Boolean MainContoller_Init()
{
	if(!Init_RCC())
		return FALSE;
	if(!Init_IRQ())
		return FALSE;

	mainContoller.adc = ADC_Init();
	if(mainContoller.adc == NULL)
		return FALSE;

	mainContoller.display = Display_Init();
	if(mainContoller.display == NULL)
		return FALSE;

	mainContoller.pwm = PWM_Init();
	if(!mainContoller.pwm)
		return FALSE;

	mainContoller.uart = UART_Init();
	if(!mainContoller.uart)
		return FALSE;

	mainContoller.state = Stopped;
	mainContoller.uart_working = FALSE;
	mainContoller.uart_half = FALSE;
	mainContoller.uart_full = FALSE;

	return TRUE;
}

void UartHalf() {
	mainContoller.uart_half = TRUE;
}

void UartFull(Boolean errored) {
	if (errored) {
		mainContoller.uart_working = FALSE;
		mainContoller.uart_full = FALSE;
		mainContoller.uart_half = FALSE;

		while(1) {
			__NOP();
		}
	}

	mainContoller.uart_full = TRUE;
	mainContoller.uart_working = FALSE;
}

void MainContoller_Loop()
{
	uint16_t result = 0;



	uint8_t uartPackage[10*1024];
	int uartPackageSize = sizeof(uartPackage);
	int uartPackageElCount = uartPackageSize/sizeof(uartPackage[0]);

	while(TRUE)
	{
		result = ADC_Get(mainContoller.adc) * 100 / 8192;


		int32_t realF = PWM_SetFreq(mainContoller.pwm, 2500 + 150 * result);

		Display_SetFreq(mainContoller.display, realF);
		Display_SetBufferUsage(mainContoller.display, result);

		if (!mainContoller.uart_working) {
			if (UART_Acquire(mainContoller.uart)) {
				if(UART_StartReceive(mainContoller.uart, UartHalf, UartFull, uartPackage, sizeof(uartPackage))) {
					mainContoller.uart_working = TRUE;
					Display_SetState(mainContoller.display, Waiting);
				}
			}
		}

		if (mainContoller.uart_half) {
			mainContoller.uart_half = FALSE;
			Display_SetState(mainContoller.display, Started);
			for (int i = 0; i < uartPackageElCount / 2; i++) {
				PWM_AddWidth(mainContoller.pwm, uartPackage[i]);
			}
		}

		if (mainContoller.uart_full) {
			mainContoller.uart_full = FALSE;
			for (int i = uartPackageElCount / 2; i < uartPackageElCount; i++) {
				PWM_AddWidth(mainContoller.pwm, uartPackage[i]);
			}
			UART_Free(mainContoller.uart);
			PWM_Start(mainContoller.pwm);
		}


		//TODO! Loop
	}
}

Boolean Init_RCC()
{
	// Init HAL
	HAL_StatusTypeDef status = HAL_Init();
	if (status != HAL_OK)
	{
		return FALSE;
	}

	__HAL_RCC_GPIOH_CLK_ENABLE(); // For HSE

	// Init clocks
	RCC_ClkInitTypeDef clkInitStruct = {0};
	RCC_OscInitTypeDef oscInitStruct = {0};

	oscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
	oscInitStruct.HSEState = RCC_HSE_ON;
	oscInitStruct.PLL.PLLState = RCC_PLL_ON;
	oscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	oscInitStruct.PLL.PLLM = 8;
	oscInitStruct.PLL.PLLN = 100;
	oscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
	oscInitStruct.PLL.PLLQ = 4; // NC
	status = HAL_RCC_OscConfig(&oscInitStruct);
	if (status != HAL_OK)
	{
		return FALSE;
	}

	clkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
	clkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	clkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	clkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
	clkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
	HAL_RCC_ClockConfig(&clkInitStruct, FLASH_LATENCY_1);
	if (status != HAL_OK)
	{
		return FALSE;
	}

	HAL_RCC_MCOConfig(RCC_MCO2, RCC_MCO2SOURCE_SYSCLK, RCC_MCODIV_5);

	SystemCoreClockUpdate();

	return TRUE;
}

Boolean Init_IRQ()
{
	return TRUE;
}

void HAL_MspInit(void)
{
	__HAL_RCC_SYSCFG_CLK_ENABLE();
	__HAL_RCC_PWR_CLK_ENABLE();
}

void SysTick_Handler(void) {
	HAL_IncTick();
}


