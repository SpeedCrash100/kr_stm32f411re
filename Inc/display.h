#ifndef DISPLAY_H_
#define DISPLAY_H_

#include "types.h"

Boolean Display_Init();
void Display_SetFreq(int32_t freq);
void Display_SetBufferUsage(int32_t usage);
void Display_SetState(MainStates state);

#endif /* DISPLAY_H_ */
