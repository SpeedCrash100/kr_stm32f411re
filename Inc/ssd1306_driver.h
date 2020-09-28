#ifndef SSD1306_DRIVER_H_
#define SSD1306_DRIVER_H_

#include "types.h"
#include "i2c.h"

typedef struct SSD1306_Driver SSD1306_Driver;

SSD1306_Driver* SSD1306_Init(I2C*);
void SSD1306_Clear(SSD1306_Driver*, Point p1, Point p2);
void SSD1306_DrawRectangle(SSD1306_Driver*, Point p1, Point p2);
void SSD1306_DrawRectangleFilled(SSD1306_Driver*, Point p1, Point p2);
void SSD1306_DrawText(SSD1306_Driver*, Point pos, char* text);
void SSD1306_Swap(SSD1306_Driver*);


#endif /* SSD1306_DRIVER_H_ */
