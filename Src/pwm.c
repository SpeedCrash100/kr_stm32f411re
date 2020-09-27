#include "pwm.h"
#include "stm32f4xx_hal.h"

TIM_HandleTypeDef tim1 = {0};
TIM_OC_InitTypeDef chanConfig = {0};
uint8_t width;
int32_t freq;

Boolean PWM_Init()
{
	tim1.Instance = TIM1;
	tim1.Init.Prescaler = 9999;
	tim1.Init.CounterMode = TIM_COUNTERMODE_UP;
	tim1.Init.Period = 255;
	tim1.Init.ClockDivision = TIM_CLOCKPRESCALER_DIV1;
	tim1.Init.RepetitionCounter = 0;
	if(HAL_TIM_Base_Init(&tim1) != HAL_OK)
		return FALSE;

	if(HAL_TIM_PWM_Init(&tim1) != HAL_OK)
		return FALSE;


	chanConfig.OCMode = TIM_OCMODE_PWM1;
	chanConfig.OCPolarity = TIM_OCPOLARITY_HIGH;
	chanConfig.Pulse = 0;
	if(HAL_TIM_OC_ConfigChannel(&tim1, &chanConfig, TIM_CHANNEL_1) != HAL_OK)
		return FALSE;

	return TRUE;
}

void PWM_SetFreq(int32_t freq)
{

}

void PWM_AddWidth(uint8_t width)
{
	chanConfig.Pulse = width;
	PWM_Stop();
	HAL_TIM_OC_ConfigChannel(&tim1, &chanConfig, TIM_CHANNEL_1);
	PWM_Start();

}

void PWM_Start()
{
	HAL_TIMEx_OCN_Start(&tim1, TIM_CHANNEL_1);
}

void PWM_Stop()
{
	HAL_TIMEx_OCN_Stop(&tim1, TIM_CHANNEL_1);
}

void PWM_GetBufferUsage(int32_t* usage)
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

