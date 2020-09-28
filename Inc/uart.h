#ifndef UART_H_
#define UART_H_

#include "types.h"

typedef struct UART UART;
typedef void (*UARTReadCallback)(void);

UART* UART_Init();
Boolean UART_StartReceive(UART*, UARTReadCallback, UARTReadCallback, uint8_t*, uint32_t);
Boolean UART_Acquire(UART*);
void UART_Free(UART*);

#endif /* UART_H_ */
