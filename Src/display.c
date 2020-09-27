#include "display.h"
#include "ssd1306_driver.h"
#include <stdio.h>

int32_t displayFreq = 0;
int32_t bufferUsage = 0;
MainStates displayState = Started;

Boolean Display_Init()
{
	if(! SSD1306_Init())
		return FALSE;

	Point p = {0, 0};
	SSD1306_DrawText(p, "Freq: ");
	Display_SetFreq(10000);

	p.y = 32;
	SSD1306_DrawText(p, "Buff: ");
	Point p1 = {0, 45}, p2 = {127, 50};
	SSD1306_DrawRectangle(p1, p2);
	Display_SetBufferUsage(10);


	Display_SetState(Stopped);

	return TRUE;
}


void Display_SetFreq(int32_t freq)
{
	if(freq == displayFreq)
		return;
	displayFreq = freq;

	Point p1, p2; // Area of text;
	p1.x = 63;
	p1.y = 0;
	p2.x = 127;
	p2.y = 12;
	SSD1306_Clear(p1, p2);

	char buf[9] = {0};
	char output[9] = {0};
	sprintf(buf, "%ld Hz", displayFreq);
	sprintf(output, "%8s", buf);
	SSD1306_DrawText(p1, output);

	SSD1306_Swap();
}

void Display_SetBufferUsage(int32_t usage)
{
	if (usage == bufferUsage)
		return;

	bufferUsage = usage;

	Point p1, p2; // Area of text;
	p1.x = 63;
	p1.y = 32;
	p2.x = 127;
	p2.y = 44;
	SSD1306_Clear(p1, p2);

	char buf[9] = {0};
	char output[9] = {0};
	sprintf(buf, "%ld %%", bufferUsage);
	sprintf(output, "%8s", buf);
	SSD1306_DrawText(p1, output);


	p1.x = 1;
	p1.y = 46;
	p2.x = 126;
	p2.y = 50;
	SSD1306_Clear(p1, p2);
	int32_t screenSpace = 125*(usage % 101) / 100;
	p2.x = 1 + screenSpace;

	SSD1306_DrawRectangleFilled(p1, p2);

	SSD1306_Swap();
}
void Display_SetState(MainStates state)
{
	if(state == displayState)
		return;

	displayState = state;
	Point p1, p2; // Area of text;
	p1.x = 0;
	p1.y = 51;
	p2.x = 127;
	p2.y = 63;
	SSD1306_Clear(p1, p2);

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

	SSD1306_DrawText(p1, buf);

	SSD1306_Swap();
}
