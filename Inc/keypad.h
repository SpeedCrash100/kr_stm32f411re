#ifndef KEYPAD_H_
#define KEYPAD_H_

#include "types.h"

/// @defgroup Keypad обработка нажатия кнопок
/// @{

typedef struct Keypad Keypad;
/// Состояния кнопок, допустимые системой
typedef enum {
  Unpressed = 0,  ///< Кнопка не нажата или нажата, но более 1 такта
  Clicked         ///< Кнопка только что нажата
} KeyState;

/// Все кнопки в системе
typedef enum {
  KeyStartStop = 0,  ///< Кнопка Старт/Стоп

  KeyButtons_SIZE  ///< Количество кнопок
} KeyButtons;

/**
 * @brief Keypad_Init инициализирует обработку нажатия кнопок
 * @return дескриптор клавиатуры или NULL в случае ошибки
 */
Keypad* Keypad_Init();

/**
 * @brief Keypad_GetState получает текущее состояние кнопки
 * @details если кнопка в момент запроса была в состоянии Clicked, то
 * возвращается это же состояние, но кнопке назначается состояние Unpressed
 *
 * @param keynumber кнопка из перечисления KeyButtons
 * @return текущее состояние указанной кнопки
 */
KeyState Keypad_GetState(Keypad*, KeyButtons keynumber);

/// @}

#endif /* KEYPAD_H_ */
