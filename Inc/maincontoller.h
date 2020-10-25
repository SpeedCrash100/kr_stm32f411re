#ifndef MAINCONTOLLER_H_
#define MAINCONTOLLER_H_

#include "types.h"

/// @defgroup MC Главный контроллер
/// @{

/**
 * @brief MainContoller_Init инициализирует программу и устройство для работы
 * @return TRUE - если, инициализация успешна. FALSE - в других случаях
 */
Boolean MainContoller_Init();

/**
 * @brief MainContoller_Loop Главный цикл приложения
 */
void MainContoller_Loop();

/// @}

#endif /* MAINCONTOLLER_H_ */
