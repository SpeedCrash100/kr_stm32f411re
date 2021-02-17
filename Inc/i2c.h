#ifndef I2C_H_
#define I2C_H_

#include "types.h"

/// @defgroup I2C связь с дисплеем
/// @{

typedef struct I2C I2C;

/// Тип функции которая вызывается при завершении передачи данных
typedef void (*I2CCallback)(void);

/**
 * @brief I2C_Init инициализирует шину %I2C
 * @details Подготавливает подсистему I2C1 к работе на пинах PB8, PB9 на
 * скорости 400кГц
 *
 * @return дескриптор шины I2C или NULL в случае ошибки
 */
I2C* I2C_Init();

/**
 * @brief I2C_Acquire получение доступа к работе с шиной
 * @return TRUE если доступ удалось получить и FALSE иначе
 */
Boolean I2C_Acquire(I2C*);

/**
 * @brief I2C_Free освобождает шины для других процессов
 *
 * @warning Необходимо быть аккуратным. Эта функция освободит шину, даже если
 * вызываемый никогда не запрашивал доступ к ней.
 */
void I2C_Free(I2C*);

/**
 * @brief I2C_SendCommand отправляет команду дисплею
 * @param cmd байтовое представление команды
 * @return TRUE если передача успешна
 */
Boolean I2C_SendCommand(I2C*, uint8_t cmd);

/**
 * @brief I2C_SendData отправляет данные дисплею
 * @param callback функция, которая будет вызвана при завершении операции
 * @param data указатель на данные для передачи
 * @param size количество данных в байтах
 * @return TRUE если запуск асинхронной операции успешен
 */
Boolean I2C_SendData(I2C*, I2CCallback callback, uint8_t* data, uint16_t size);

/// @}

#endif /* I2C_H_ */
