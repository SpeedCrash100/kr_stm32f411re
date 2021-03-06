#ifndef UART_H_
#define UART_H_

#include "types.h"

/// @defgroup UART Подсистема UART
/// @{

typedef struct UART UART;

/// Тип функции, вызываемой при приеме первой половины посылки
typedef void (*UARTReadHalfCallback)(void);
/// Тип функции, вызываемой при приеме второй половины посылки или ошибке
typedef void (*UARTReadFullCallback)(Boolean);

/**
 * @brief UART_Init инициализация %UART
 * @return дескриптор UART или NULL в случае ошибки
 */
UART* UART_Init();

/**
 * @brief UART_StartReceive принимает данные по интерфейсу %UART
 *
 * @param halfCallback функция, вызываемая при приеме первой половины посылки
 * @param fullCallback функция, вызываемая при приеме второй половины посылки
 * или ошибке
 * @param data указатель на место, где будут находиться данные
 * @param size размер посылки
 * @return TRUE если запуск приема успешен
 *
 * @remark функция fullCallback имеет аргумент типа Boolean. Если он равен TRUE,
 * то не следует считывать данные так как в процессе передачи произошла ошибка.
 */
Boolean UART_StartReceive(UART*, UARTReadHalfCallback halfCallback,
                          UARTReadFullCallback fullCallback, uint8_t* data,
                          uint32_t size);

/**
 * @brief UART_Acquire получение доступа к работе с интерфейсом
 * @return TRUE если доступ удалось получить и FALSE иначе
 */
Boolean UART_Acquire(UART*);

/**
 * @brief UART_Free освобождает интерфейс для других процессов
 *
 * @warning Необходимо быть аккуратным. Эта функция освободит интерфейс, даже
 * если вызываемый никогда не запрашивал доступ к нему.
 */
void UART_Free(UART*);

/// @}

#endif /* UART_H_ */
