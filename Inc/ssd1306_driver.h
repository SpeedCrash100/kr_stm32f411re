#ifndef SSD1306_DRIVER_H_
#define SSD1306_DRIVER_H_

#include "types.h"

Boolean SSD1306_Init();
void SSD1306_Clear(Point p1, Point p2);
void SSD1306_DrawRectangle(Point p1, Point p2);
void SSD1306_DrawRectangleFilled(Point p1, Point p2);
void SSD1306_DrawText(Point pos, char* text);
void SSD1306_Swap();


#endif /* SSD1306_DRIVER_H_ */
