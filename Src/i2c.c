#include "i2c.h"
#include "stm32f4xx_hal.h"

I2C_HandleTypeDef i2c;
Boolean i2c_acquired;
Boolean i2c_in_progress;
uint16_t address;

Boolean I2C_Init()
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	__HAL_RCC_GPIOB_CLK_ENABLE();
	GPIO_InitStruct.Pin = GPIO_PIN_8|GPIO_PIN_9;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.Alternate = GPIO_AF4_I2C1;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	__HAL_RCC_I2C1_CLK_ENABLE();
	HAL_NVIC_SetPriority(I2C1_EV_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(I2C1_EV_IRQn);


	i2c.Instance = I2C1;
	i2c.Init.ClockSpeed = 400000;
	i2c.Init.DutyCycle = I2C_DUTYCYCLE_2;
	i2c.Init.OwnAddress1 = 0;
	i2c.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
	i2c.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
	i2c.Init.OwnAddress2 = 0;
	i2c.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
	i2c.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
	if (HAL_I2C_Init(&i2c) != HAL_OK)
	{
		return FALSE;
	}


	i2c_acquired = FALSE;
	i2c_in_progress = FALSE;
	address = 0x3C << 1;

	return TRUE;
}

Boolean I2C_Acquire()
{
	if(i2c_acquired)
		return FALSE;
	i2c_acquired = TRUE;
	return TRUE;
}
void I2C_Free()
{
	i2c_acquired = FALSE;
}

Boolean I2C_SendCommand(uint8_t cmd)
{
	if (!i2c_acquired)
		return FALSE;

	HAL_StatusTypeDef status = HAL_I2C_Mem_Write(&i2c, address, 0x00, I2C_MEMADD_SIZE_8BIT, &cmd, 1, 1000);
	if(status != HAL_OK)
	{
		return FALSE;
	}
	return TRUE;
}
Boolean I2C_SendData(uint8_t* data, uint16_t size)
{
	if (!i2c_acquired)
		return FALSE;

	HAL_StatusTypeDef status = HAL_I2C_Mem_Write(&i2c, address, 0x40, I2C_MEMADD_SIZE_8BIT, data, size, 1000);
	if(status != HAL_OK)
	{
		return FALSE;
	}
	return TRUE;
}
