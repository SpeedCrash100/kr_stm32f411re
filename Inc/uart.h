#ifndef UART_H_
#define UART_H_

#include "types.h"

typedef void (*UARTReadCallback)(void);

Boolean UART_Init();
Boolean UART_StartReceive(UARTReadCallback callback, uint8_t* buffer, uint32_t size);
Boolean UART_Acquire();
void UART_Free();

#endif /* UART_H_ */
