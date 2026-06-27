// ===========================================================================
//  input_manager.cpp  -  Debounce + flanco de bajada (presion). No-op si INPUT_ENABLED 0.
// ===========================================================================
#include "config.h"
#include "input_manager.h"

#if INPUT_ENABLED
static const uint8_t PINS[3]       = { BUTTON_1, BUTTON_2, BUTTON_3 };
static bool          pressed[3]    = { false, false, false };
static bool          justP[3]      = { false, false, false };
static uint32_t      lastChange[3] = { 0, 0, 0 };

void inputBegin() {
  for (int i = 0; i < 3; i++) {
    if (BUTTON_USE_PULLUP)        pinMode(PINS[i], INPUT_PULLUP);    // boton a GND
    else if (BUTTON_ACTIVE_HIGH)  pinMode(PINS[i], INPUT_PULLDOWN);  // TTP223: flotante/idle = BAJO
    else                          pinMode(PINS[i], INPUT);
  }
}

void inputUpdate(uint32_t now) {
  const int active = BUTTON_ACTIVE_HIGH ? HIGH : LOW;   // nivel que cuenta como "presionado"
  for (int i = 0; i < 3; i++) {
    bool down = (digitalRead(PINS[i]) == active);
    if (down != pressed[i] && (now - lastChange[i]) >= BTN_DEBOUNCE_MS) {
      pressed[i]    = down;
      lastChange[i] = now;
      if (down) justP[i] = true;          // flanco de presion
    }
  }
}

bool inputPressed(uint8_t idx) { return (idx < 3) ? pressed[idx] : false; }

bool inputJustPressed(uint8_t idx) {
  if (idx >= 3) return false;
  bool v = justP[idx];
  justP[idx] = false;                     // se consume
  return v;
}

#else  // ---- desactivado: no-op ----
void inputBegin() {}
void inputUpdate(uint32_t now) { (void)now; }
bool inputPressed(uint8_t idx) { (void)idx; return false; }
bool inputJustPressed(uint8_t idx) { (void)idx; return false; }
#endif
