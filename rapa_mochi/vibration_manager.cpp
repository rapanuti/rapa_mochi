// ===========================================================================
//  vibration_manager.cpp  -  Vibrador no bloqueante. Compile-safe (VIBRATION_ENABLED).
//  HARDWARE: el motor NO va directo al GPIO. Usar MOSFET/transistor NPN + diodo
//  flyback en paralelo al motor + resistencia en base/gate. Alimentacion adecuada.
// ===========================================================================
#include "config.h"
#include "vibration_manager.h"

#if VIBRATION_ENABLED
static uint32_t offAt = 0;
static bool     on    = false;

void vibrationBegin() {
  pinMode(VIBRATION_PIN, OUTPUT);
  digitalWrite(VIBRATION_PIN, LOW);
}

void vibrate(uint16_t ms, uint8_t pct) {
  (void)pct;                       // ON/OFF simple; para intensidad usar ledc (PWM)
  digitalWrite(VIBRATION_PIN, HIGH);
  on    = true;
  offAt = millis() + ms;
}

void vibrationUpdate() {           // apaga al cumplir la duracion (no bloquea)
  if (on && (int32_t)(millis() - offAt) >= 0) {
    digitalWrite(VIBRATION_PIN, LOW);
    on = false;
  }
}

#else  // ---- desactivado: no-op ----
void vibrationBegin() {}
void vibrationUpdate() {}
void vibrate(uint16_t ms, uint8_t pct) { (void)ms; (void)pct; }
#endif
