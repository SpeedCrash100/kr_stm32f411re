#ifndef PWM_H_
#define PWM_H_

#include "types.h"

/**
 * @defgroup PWM генерация ШИМ
 * @{
 */

typedef struct PWM PWM;

/**
 * @brief PWM_Init инициализирует подсистему PWM
 * @return дескриптор на подсистему или NULL в случае ошибки
 */
PWM* PWM_Init();

/**
 * @brief PWM_SetFreq устанавливает целевую частоту генерации ШИМ
 * @param freq целевая частота
 * @return частоту, которая была установлена
 */
int32_t PWM_SetFreq(PWM*, int32_t freq);

/**
 * @brief PWM_AddWidth добавляет модулирующее значение для последующего
 * воспроизведения
 * @param width модулирующее значение
 */
void PWM_AddWidth(PWM*, uint8_t width);

/**
 * @brief PWM_Start запускает генерацию ШИМ
 */
void PWM_Start(PWM*);

/**
 * @brief PWM_Stop останавливает генерацию ШИМ
 */
void PWM_Stop(PWM*);

/**
 * @brief PWM_GetBufferUsage получает наполненность буфера
 * @param usage указатель на место, куда будет записан результат от 0 до 100 в
 * процентах
 */
void PWM_GetBufferUsage(PWM*, int32_t* usage);

/// @}

#endif /* PWM_H_ */
