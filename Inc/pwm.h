#ifndef PWM_H_
#define PWM_H_

#include "types.h"

Boolean PWM_Init();
void PWM_SetFreq(int32_t freq);
void PWM_AddWidth(uint8_t width);
void PWM_Start();
void PWM_Stop();
void PWM_GetBufferUsage(int32_t* usage);


#endif /* PWM_H_ */
