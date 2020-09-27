#include "maincontoller.h"
#include "stm32f4xx_hal.h"
#include "adc.h"
#include "display.h"
#include "pwm.h"

struct {
	MainStates state;


} mainContoller;

Boolean Init_RCC();
Boolean Init_IRQ();

Boolean MainContoller_Init()
{
	if(!Init_RCC())
		return FALSE;
	if(!Init_IRQ())
		return FALSE;

	if(!ADC_Init())
		return FALSE;

	if(!Display_Init())
		return FALSE;

	if(!PWM_Init())
		return FALSE;


	return TRUE;
}

void MainContoller_Loop()
{
	uint16_t result = 0;

	PWM_Start();

	while(TRUE)
	{
		result = ADC_Get();
		Display_SetFreq(result);
		Display_SetBufferUsage(result);
		PWM_AddWidth(result);

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

	// Init clocks
	RCC_ClkInitTypeDef clkInitStruct = {0};
	RCC_OscInitTypeDef oscInitStruct = {0};

	oscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
	oscInitStruct.HSEState = RCC_HSE_OFF;
	oscInitStruct.HSIState = RCC_HSI_ON;
	oscInitStruct.PLL.PLLState = RCC_PLL_ON;
	oscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
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
	HAL_RCC_ClockConfig(&clkInitStruct, FLASH_LATENCY_4);
	if (status != HAL_OK)
	{
		return FALSE;
	}

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


