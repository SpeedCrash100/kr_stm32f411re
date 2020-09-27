#include "ssd1306_driver.h"
#include "stm32f4xx_hal.h"
#include "i2c.h"
#include <stdlib.h>
#include "fonts.h"

uint8_t renderBuffer[128*64/8];


void SendCmd(uint8_t cmd)
{
	while(!I2C_Acquire())
	{
		__NOP();
	}

	if(!I2C_SendCommand(cmd))
	{
		while(TRUE);
	}

	I2C_Free();
}

void SendData(uint8_t* data, uint16_t size)
{
	while(!I2C_Acquire())
	{
		__NOP();
	}

	if(!I2C_SendData(data, size))
	{
		while(TRUE);
	}

	I2C_Free();
}


Boolean SSD1306_Init()
{
	I2C_Init();

	HAL_Delay(100); // Recommended delay for display starting up
	//Set Power Reset for normal screen operation
	__HAL_RCC_GPIOA_CLK_ENABLE();
	GPIO_InitTypeDef init = {0};
	init.Pin = GPIO_PIN_8;
	init.Mode = GPIO_MODE_OUTPUT_PP;

	HAL_GPIO_Init(GPIOA, &init);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET);
	HAL_Delay(100);

	SendCmd(0xAE); /*display off*/
	SendCmd(0x20);
	SendCmd(0x00);

	SendCmd(0xC8); /*Com scan direction*/


	SendCmd(0x00); /*set lower column address*/
	SendCmd(0x10); /*set higher column address*/
	SendCmd(0x40); /*set display start line*/

	SendCmd(0xB0); /*set page address*/
	SendCmd(0x81); /*contract control*/
	SendCmd(0xcf); /*128*/

	SendCmd(0xA1); /*set segment remap*/

	SendCmd(0xA6); /*normal / reverse*/

	SendCmd(0xA8); /*multiplex ratio*/
	SendCmd(0x3F); /*duty = 1/64*/

	SendCmd(0xA4); /* Display RAM content*/

	SendCmd(0xD3); /*set display offset*/
	SendCmd(0x00);
	SendCmd(0xD5); /*set osc division*/
	SendCmd(0x80);

	SendCmd(0xD9); /*set pre-charge period*/
	SendCmd(0x22);

	SendCmd(0xDA); /*set COM pins*/
	SendCmd(0x12);

	SendCmd(0xdb); /*set vcomh*/
	SendCmd(0x20);
	SendCmd(0x8D); /*set charge pump disable*/
	SendCmd(0x14);

	SendCmd(0xAF); /*display ON*/


	memset(renderBuffer, 0, sizeof(renderBuffer));
	SendData(renderBuffer, sizeof(renderBuffer));

	return TRUE;
}


void SSD1306_Clear(Point p1, Point p2)
{
	if (p1.y > p2.y)
	{
		int32_t tmp = p1.y;
		p1.y = p2.y;
		p2.y = tmp;
	}

	if (p1.x > p2.x)
	{
		int32_t tmp = p1.x;
		p1.x = p2.x;
		p2.x = tmp;
	}

	for (int32_t x = p1.x; x < p2.x; x++) {
		for (int32_t y = p1.y; y < p2.y; y++) {
			int32_t page = y / 8;
			uint8_t* byte = &renderBuffer[page*128+x];
			(*byte) &= ~(1 << (y % 8));
		}
	}

}

void SSD1306_DrawRectangle(Point p1, Point p2)
{
	if (p1.y > p2.y)
	{
		int32_t tmp = p1.y;
		p1.y = p2.y;
		p2.y = tmp;
	}

	if (p1.x > p2.x)
	{
		int32_t tmp = p1.x;
		p1.x = p2.x;
		p2.x = tmp;
	}

	//Draw horizontal

	for (int32_t x = p1.x; x < p2.x; x++) {
		int32_t page1 = p1.y / 8;
		uint8_t* byte1 = &renderBuffer[page1*128+x];
		(*byte1) |= 1 << (p1.y % 8);

		int32_t page2 = p2.y / 8;
		uint8_t* byte2 = &renderBuffer[page2*128+x];
		(*byte2) |= 1 << (p2.y % 8);
	}

	//Vertical
	for (int32_t y = p1.y; y < p2.y; y++) {
		int32_t page = y / 8;
		uint8_t* byte1 = &renderBuffer[page*128+p1.x];
		(*byte1) |= 1 << (y % 8);

		uint8_t* byte2 = &renderBuffer[page*128+p2.x];
		(*byte2) |= 1 << (y % 8);
	}

}
void SSD1306_DrawRectangleFilled(Point p1, Point p2)
{
	if (p1.y > p2.y)
	{
		int32_t tmp = p1.y;
		p1.y = p2.y;
		p2.y = tmp;
	}

	if (p1.x > p2.x)
	{
		int32_t tmp = p1.x;
		p1.x = p2.x;
		p2.x = tmp;
	}

	for (int32_t x = p1.x; x < p2.x; x++) {
		for (int32_t y = p1.y; y < p2.y; y++) {
			int32_t page = y / 8;
			uint8_t* byte = &renderBuffer[page*128+x];
			(*byte) |= 1 << (y % 8);
		}
	}

}

char convertChar(char ch)
{
	if (ch < 32 || ch > 126)
		return 0;

	return ch - 32;
}

void SSD1306_DrawText(Point pos, char* text)
{
	sFONT font = Font12;
	int32_t bytesForWidth = 1;

	for(char* ptr = text; *ptr != 0; ptr++)
	{
		char ch = convertChar(*ptr);
		const uint8_t* base = &font.table[ch*bytesForWidth*font.Height];

		for (int y = 0; y < font.Height; y++)
		{
			const uint8_t* lane = &base[y*bytesForWidth];

			uint32_t page = (pos.y + y) / 8;
			uint8_t* byte = &renderBuffer[page*128+pos.x];

			for (int x = 0; x < font.Width; x++)
			{
				uint32_t posInLane = x / (8*bytesForWidth);
				uint32_t needDraw = lane[posInLane] & (0b10000000 >> (x % 8));

				if(needDraw)
				{
					byte[x] |= 1 << ((pos.y + y) % 8);
				}

			}
		}


		pos.x += font.Width + 1;
	}

}

void SSD1306_Swap()
{
	SendData(renderBuffer, sizeof(renderBuffer));
}
