// ===========================================================================
//  sound_manager.cpp  -  (STUB) Compile-safe. Usa solo APIs del core ESP32.
// ===========================================================================
#include "config.h"
#include "sound_manager.h"

#if SOUND_ENABLED
  // TODO fase sonido: ledcAttach(SOUND_PWM_PIN, ...) para buzzer pasivo,
  // o I2S para MAX98357A. NO conectar un parlante directo al GPIO.
  void soundBegin() {}
  void soundUpdate() {}
  void soundPlay(int fx) { (void)fx; }
#else
  void soundBegin() {}
  void soundUpdate() {}
  void soundPlay(int fx) { (void)fx; }
#endif
