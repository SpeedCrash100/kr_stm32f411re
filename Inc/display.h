#ifndef DISPLAY_H_
#define DISPLAY_H_

#include "types.h"

/// @defgroup Display отображение характеристик ШИМ
/// @{

typedef struct Display Display;

/**
 * @brief Display_Init инициализирует дисплей и рисует начальное состояние
 * системы
 * @return дескриптор дисплея
 */
Display* Display_Init();

/**
 * @brief Display_SetFreq устанавливает новую отображаемую частоту
 * @param freq новая частота
 */
void Display_SetFreq(Display*, int32_t freq);

/**
 * @brief Display_SetBufferUsage устанавливает новое значение использования
 * буфера
 * @param usage новое использование буфера
 */
void Display_SetBufferUsage(Display*, int32_t usage);

/**
 * @brief Display_SetState устанавливает новое состояние
 * @param state новое состояние
 */
void Display_SetState(Display*, MainStates state);

/// @}

#endif /* DISPLAY_H_ */
