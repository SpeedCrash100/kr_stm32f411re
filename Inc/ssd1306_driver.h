#ifndef SSD1306_DRIVER_H_
#define SSD1306_DRIVER_H_

#include "i2c.h"
#include "types.h"

/// @defgroup SSD1306 Драйвер дисплея SSD1306
/// @{

typedef struct SSD1306_Driver SSD1306_Driver;

/**
 * @brief SSD1306_Init инициализирует драйвер дисплея
 *
 * @param i2c дескриптор шины I2C используемый для связи с дисплеем
 * @return дескриптор драйвера
 */
SSD1306_Driver* SSD1306_Init(I2C* i2c);

/**
 * @brief SSD1306_Clear очищает содержимое в прямоугольнике обозначенном точками
 * p1 и p2
 * @param p1 первая точка прямоугольника
 * @param p2 вторая точка прямоугольника
 */
void SSD1306_Clear(SSD1306_Driver*, Point p1, Point p2);

/**
 * @brief SSD1306_DrawRectangle рисует прямоугольника без заполнения
 * @param p1 первая точка прямоугольника
 * @param p2 вторая точка прямоугольника
 */
void SSD1306_DrawRectangle(SSD1306_Driver*, Point p1, Point p2);

/**
 * @brief SSD1306_DrawRectangleFilled рисует прямоугольника c заполнением
 * @param p1 первая точка прямоугольника
 * @param p2 вторая точка прямоугольника
 */
void SSD1306_DrawRectangleFilled(SSD1306_Driver*, Point p1, Point p2);

/**
 * @brief SSD1306_DrawText рисует текст на экране
 * @param pos позиция начала текста
 * @param text строка для написания
 *
 * @warning Проверки на выход за пределы экрана нет
 *
 */
void SSD1306_DrawText(SSD1306_Driver*, Point pos, char* text);

/**
 * @brief SSD1306_Swap выводит информацию их буфера на экран
 */
void SSD1306_Swap(SSD1306_Driver*);

/// @}

#endif /* SSD1306_DRIVER_H_ */
