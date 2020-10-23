#ifndef KEYPAD_H_
#define KEYPAD_H_

#include "types.h"

typedef struct Keypad Keypad;

typedef enum { Unpressed = 0, Clicked } KeyState;

typedef enum { KeyStartStop = 0 } KeyButtons;

Keypad* Keypad_Init();
KeyState Keypad_GetState(Keypad*, KeyButtons keynumber);

#endif /* KEYPAD_H_ */
