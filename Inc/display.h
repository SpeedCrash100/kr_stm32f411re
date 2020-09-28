#ifndef DISPLAY_H_
#define DISPLAY_H_

#include "types.h"

typedef struct Display Display;

Display* Display_Init();
void Display_SetFreq(Display*, int32_t freq);
void Display_SetBufferUsage(Display*, int32_t usage);
void Display_SetState(Display*, MainStates state);

#endif /* DISPLAY_H_ */
