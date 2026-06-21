// ===========================================================================
//  vibration_manager.cpp  -  (STUB) Compile-safe. Driver via transistor/MOSFET.
// ===========================================================================
#include "config.h"
#include "vibration_manager.h"

#if VIBRATION_ENABLED
  void vibrationBegin() {
    pinMode(VIBRATION_PIN, OUTPUT);
    digitalWrite(VIBRATION_PIN, LOW);
  }
  void vibrationUpdate() {}
  void vibrate(uint16_t ms, uint8_t pct) { (void)ms; (void)pct; /* TODO PWM por ledc */ }
#else
  void vibrationBegin() {}
  void vibrationUpdate() {}
  void vibrate(uint16_t ms, uint8_t pct) { (void)ms; (void)pct; }
#endif
