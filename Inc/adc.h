#ifndef ADC_H_
#define ADC_H_

#include "types.h"

typedef struct STM32ADC STM32ADC;

STM32ADC* ADC_Init();
uint16_t ADC_Get(STM32ADC*);

#endif /* ADC_H_ */
