#include "display.h"
#include "ssd1306_driver.h"
#include "i2c.h"
#include <stdio.h>

struct Display {
	SSD1306_Driver* drv;

	int32_t displayFreq;
	int32_t bufferUsage;
	MainStates displayState;
};

Boolean g_Display_initialized = FALSE;
Display g_Display;


Display* Display_Init()
{
	if(g_Display_initialized)
		return &g_Display;

	I2C* i2c_bus = I2C_Init();
	if(!i2c_bus)
		return NULL;

	g_Display.drv = SSD1306_Init(i2c_bus);
	if(!g_Display.drv)
		return NULL;

	g_Display.bufferUsage = 0;
	g_Display.displayFreq = 10000;
	g_Display.displayState = Stopped;

	Point p = {0, 0};
	SSD1306_DrawText(g_Display.drv, p, "Freq: ");
	Display_SetFreq(&g_Display, 10000);

	p.y = 32;
	SSD1306_DrawText(g_Display.drv, p, "Buff: ");
	Point p1 = {0, 45}, p2 = {127, 50};
	SSD1306_DrawRectangle(g_Display.drv, p1, p2);
	Display_SetBufferUsage(&g_Display, 10);


	Display_SetState(&g_Display, Stopped);

	g_Display_initialized = TRUE;

	return &g_Display;
}


void Display_SetFreq(Display* hnd, int32_t freq)
{
	if(freq == g_Display.displayFreq)
		return;
	 g_Display.displayFreq = freq;

	Point p1, p2; // Area of text;
	p1.x = 63;
	p1.y = 0;
	p2.x = 127;
	p2.y = 12;
	SSD1306_Clear(g_Display.drv, p1, p2);

	char buf[9] = {0};
	char output[9] = {0};
	sprintf(buf, "%ld Hz",  g_Display.displayFreq);
	sprintf(output, "%8s", buf);
	SSD1306_DrawText(g_Display.drv, p1, output);

	SSD1306_Swap(g_Display.drv);
}

void Display_SetBufferUsage(Display* hnd, int32_t usage)
{
	if (usage ==  g_Display.bufferUsage)
		return;

	 g_Display.bufferUsage = usage;

	Point p1, p2; // Area of text;
	p1.x = 63;
	p1.y = 32;
	p2.x = 127;
	p2.y = 44;
	SSD1306_Clear(g_Display.drv, p1, p2);

	char buf[9] = {0};
	char output[9] = {0};
	sprintf(buf, "%ld %%",  g_Display.bufferUsage);
	sprintf(output, "%8s", buf);
	SSD1306_DrawText(g_Display.drv, p1, output);

	p1.x = 1;
	p1.y = 46;
	p2.x = 126;
	p2.y = 50;
	SSD1306_Clear(g_Display.drv, p1, p2);
	int32_t screenSpace = 125*(usage % 101) / 100;
	p2.x = 1 + screenSpace;

	SSD1306_DrawRectangleFilled(g_Display.drv, p1, p2);

	SSD1306_Swap(g_Display.drv);
}
void Display_SetState(Display* hnd, MainStates state)
{
	if(state ==  g_Display.displayState)
		return;

	g_Display.displayState = state;
	Point p1, p2; // Area of text;
	p1.x = 0;
	p1.y = 51;
	p2.x = 127;
	p2.y = 63;
	SSD1306_Clear(g_Display.drv, p1, p2);

	char buf[17] = {0};

	switch(state)
	{
	case Stopped:
		sprintf(buf, "%16s", "Stopped");
		break;
	case Waiting:
		sprintf(buf, "%16s", "Waiting");
		break;
	case Started:
		sprintf(buf, "%16s", "Started");
		break;
	case Stopping:
		sprintf(buf, "%16s", "Stopped");
		break;
	default:
		sprintf(buf, "%16s", "Unknown");
		break;
	}

	SSD1306_DrawText(g_Display.drv, p1, buf);

	SSD1306_Swap(g_Display.drv);
}
