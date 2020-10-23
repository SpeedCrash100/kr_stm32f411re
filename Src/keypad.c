#include "keypad.h"

struct Keypad {
  KeyState keystates[1];
};

Keypad g_Keypad;

Keypad* Keypad_Init() {
  // TODO! Init
  g_Keypad.keystates[0] = Clicked;
  return &g_Keypad;
}

KeyState Keypad_GetState(Keypad* keypad, KeyButtons key) {
  // TODO! grab real value!
  (void)key;
  if (keypad->keystates[0] == Clicked) {
    keypad->keystates[0] = Unpressed;
    return Clicked;
  }
  return Unpressed;
}
