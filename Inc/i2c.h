#ifndef I2C_H_
#define I2C_H_

#include "types.h"

typedef struct I2C I2C;
typedef void (*I2CCallback)(void);

I2C* I2C_Init();

Boolean I2C_Acquire(I2C*);
void I2C_Free(I2C*);

Boolean I2C_SendCommand(I2C*, uint8_t cmd);
Boolean I2C_SendData(I2C*, I2CCallback callback, uint8_t* data, uint16_t size);


#endif /* I2C_H_ */
