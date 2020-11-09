#ifndef TYPES_H_
#define TYPES_H_

#include <stdint.h>

/// \defgroup Types Вспомогательные определения
/// @{

/// Логический тип используемый в проекте
typedef enum Boolean {
  FALSE = 0,  ///< Ложь
  TRUE        ///< Истина
} Boolean;

/// Рабочие состояния системы
typedef enum MainStates {
  Stopped,  ///< Генерация ШИМ остановлена
  Waiting,  ///< Ожидание модулирующих значений
  Started,  ///< Генерация ШИМ запущена
  Stopping  ///< Генерация ШИМ воспроизводит остатки буфера и останавливается
} MainStates;

/// Точка в пространстве дисплея
typedef struct {
  int32_t x;  ///< Горизонтальная координата
  int32_t y;  ///< Вертикальная координата
} Point;

/// @}

#endif /* TYPES_H_ */
