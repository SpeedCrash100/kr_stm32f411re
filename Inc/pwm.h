#ifndef PWM_H_
#define PWM_H_

#include "types.h"

typedef struct PWM PWM;

PWM* PWM_Init();
int32_t PWM_SetFreq(PWM*, int32_t freq);
void PWM_AddWidth(PWM*, uint8_t width);
void PWM_Start(PWM*);
void PWM_Stop(PWM*);
void PWM_GetBufferUsage(PWM*, int32_t* usage);


#endif /* PWM_H_ */
