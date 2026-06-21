// ===========================================================================
//  input_manager.cpp  -  Botones con INPUT_PULLUP + debounce. Inerte si INPUT_ENABLED 0.
// ===========================================================================
#include "config.h"
#include "input_manager.h"

#if INPUT_ENABLED
static const uint8_t PINS[3]      = { BUTTON_1, BUTTON_2, BUTTON_3 };
static bool          pressed[3]   = { false, false, false };
static uint32_t      lastChange[3]= { 0, 0, 0 };

void inputBegin() {
  for (int i = 0; i < 3; i++) pinMode(PINS[i], INPUT_PULLUP);  // LOW = presionado
}

void inputUpdate(uint32_t now) {
  for (int i = 0; i < 3; i++) {
    bool down = (digitalRead(PINS[i]) == LOW);
    if (down != pressed[i] && (now - lastChange[i]) >= BTN_DEBOUNCE_MS) {
      pressed[i]    = down;
      lastChange[i] = now;
    }
  }
}

bool inputPressed(uint8_t idx) { return (idx < 3) ? pressed[idx] : false; }

#else  // ---- desactivado: no-op, no toca pines ----
void inputBegin() {}
void inputUpdate(uint32_t now) { (void)now; }
bool inputPressed(uint8_t idx) { (void)idx; return false; }
#endif
