#ifndef I2C_H_
#define I2C_H_

#include "types.h"

Boolean I2C_Init();

Boolean I2C_Acquire();
void I2C_Free();

Boolean I2C_SendCommand(uint8_t cmd);
Boolean I2C_SendData(uint8_t* data, uint16_t size);


#endif /* I2C_H_ */
