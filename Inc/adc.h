#ifndef ADC_H_
#define ADC_H_

#include "types.h"

/// @defgroup ADC подсистема АЦП
/// @{

typedef struct STM32ADC STM32ADC;

/**
 * @brief ADC_Init инициализирует АЦП
 * @details АЦП запускается в постоянном режиме и использует DMA для передачи
 * результатов в память программы. Сканирование идет только на 0 канале АЦП.
 *
 * @return дескриптор АЦП
 */
STM32ADC* ADC_Init();

/**
 * @brief ADC_Get получает последнее значение с АЦП
 * @return результат измерения
 */
uint16_t ADC_Get(STM32ADC*);

/// @}

#endif /* ADC_H_ */
