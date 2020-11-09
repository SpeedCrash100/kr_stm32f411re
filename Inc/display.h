#ifndef DISPLAY_H_
#define DISPLAY_H_

#include "types.h"

/// @defgroup Display отображение характеристик ШИМ
/// @{

typedef struct Display Display;

/**
 * @brief Display_Init инициализирует дисплей и рисует начальное состояние
 * системы
 * @return дескриптор дисплея или NULL в случае ошибки
 */
Display* Display_Init();

/**
 * @brief Display_SetFreq устанавливает новую отображаемую частоту
 * @details Рисует справа от надписи "Freq: " установленную частоту в формате
 * "10000 Hz", если она изменилась с предыдущего вызова
 *
 * @param freq новая частота
 */
void Display_SetFreq(Display*, int32_t freq);

/**
 * @brief Display_SetBufferUsage устанавливает новое значение использования
 * буфера
 * @details Рисует справа от надписи "Buff: " установленную наполненость буфера
 * в формате "100%", если она изменилась с предыдущего вызова. А также обновляет
 * визуальное отображение заполнености буфера.
 * @param usage новое использование буфера
 */
void Display_SetBufferUsage(Display*, int32_t usage);

/**
 * @brief Display_SetState устанавливает новое состояние
 * @details Рисует справа внизу наименование нового состояния, если оно
 * изменилось с предыдущего вызова
 *
 * @param state новое состояние
 */
void Display_SetState(Display*, MainStates state);

/// @}

#endif /* DISPLAY_H_ */
