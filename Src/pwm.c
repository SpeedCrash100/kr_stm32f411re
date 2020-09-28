#include "pwm.h"
#include "stm32f4xx_hal.h"
#include <math.h>

struct PWM {
	TIM_HandleTypeDef tim1;
	TIM_OC_InitTypeDef chanConfig;

	uint8_t width;
	uint32_t freq;

	Boolean valueCaptured;
	uint8_t buffer;
};

Boolean g_PWM_initialized = FALSE;
PWM g_PWM = {0};

PWM* PWM_Init()
{
	if(g_PWM_initialized)
		return &g_PWM;

	TIM_HandleTypeDef* tim1 = &g_PWM.tim1;
	TIM_OC_InitTypeDef* chanConfig = &g_PWM.chanConfig;

	tim1->Instance = TIM1;
	tim1->Init.Prescaler = 38;
	tim1->Init.CounterMode = TIM_COUNTERMODE_UP;
	tim1->Init.Period = 256;
	tim1->Init.ClockDivision = TIM_CLOCKPRESCALER_DIV1;
	tim1->Init.RepetitionCounter = 0;
	if(HAL_TIM_Base_Init(tim1) != HAL_OK)
		return NULL;

	if(HAL_TIM_PWM_Init(tim1) != HAL_OK)
		return NULL;

	HAL_NVIC_SetPriority(TIM1_UP_TIM10_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(TIM1_UP_TIM10_IRQn);
	__HAL_TIM_ENABLE_IT(tim1, TIM_IT_UPDATE);

	chanConfig->OCMode = TIM_OCMODE_PWM1;
	chanConfig->OCPolarity = TIM_OCPOLARITY_HIGH;
	chanConfig->Pulse = 0;
	if(HAL_TIM_PWM_ConfigChannel(tim1, chanConfig, TIM_CHANNEL_1) != HAL_OK)
		return NULL;

	g_PWM.freq = 1000;
	g_PWM.width = 0;
	g_PWM.valueCaptured = TRUE;
	g_PWM_initialized = TRUE;

	return &g_PWM;
}

int32_t PWM_SetFreq(PWM* hnd, int32_t freq)
{
	uint32_t prescal = 38;
	uint32_t realFreq = 100000000 / ( (256+1)*(prescal+1) );
	uint32_t delta = abs(freq - realFreq);
	while(prescal < 65535)
	{
		prescal++;
		realFreq = 100000000 / ( (256+1)*(prescal+1) );
		uint32_t newDelta = abs(freq - realFreq);
		if(newDelta < delta) {
			delta = newDelta;
		} else {
			prescal--; // Revert
			break;
		}
	}

	__HAL_TIM_SET_PRESCALER(&hnd->tim1, prescal);
	return realFreq = 100000000 / ( (256+1)*(prescal+1) );
}

void PWM_AddWidth(PWM* hnd, uint8_t width)
{
	if (hnd->valueCaptured)	{
		hnd->buffer = width;
		hnd->valueCaptured = FALSE;
	} else {
		hnd->buffer += (width - hnd->buffer) * 100 / 1000;
	}

//	__HAL_TIM_SET_COMPARE(&hnd->tim1, TIM_CHANNEL_1, width);
}

void PWM_Start(PWM* hnd)
{
	HAL_TIMEx_OCN_Start_IT(&hnd->tim1, TIM_CHANNEL_1);
}

void PWM_Stop(PWM* hnd)
{
	HAL_TIMEx_OCN_Stop_IT(&hnd->tim1, TIM_CHANNEL_1);
}

void PWM_GetBufferUsage(PWM* hnd, int32_t* usage)
{

}

void HAL_TIM_Base_MspInit(TIM_HandleTypeDef* htim_base)
{
	if(htim_base->Instance == TIM1)
	{
		__HAL_RCC_TIM1_CLK_ENABLE();
		__HAL_RCC_GPIOA_CLK_ENABLE();
		GPIO_InitTypeDef GPIO_InitStruct = {0};
		/**TIM1 GPIO Configuration
		PA7     ------> TIM1_CH1N
		*/
		GPIO_InitStruct.Pin = GPIO_PIN_7;
		GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
		GPIO_InitStruct.Alternate = GPIO_AF1_TIM1;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
	}
}

void TIM1_UP_TIM10_IRQHandler() {
	__HAL_TIM_CLEAR_IT(&g_PWM.tim1, TIM_IT_UPDATE);

	g_PWM.valueCaptured = TRUE;

	__HAL_TIM_SET_COMPARE(&g_PWM.tim1, TIM_CHANNEL_1, g_PWM.buffer);
}

