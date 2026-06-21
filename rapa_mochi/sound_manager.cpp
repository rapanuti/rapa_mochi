// ===========================================================================
//  sound_manager.cpp  -  Tonos no bloqueantes en buzzer pasivo. Compile-safe.
//  HARDWARE: buzzer PASIVO en SOUND_PWM_PIN, o amplificador (PAM8403 / MAX98357A I2S)
//  + parlante 4/8 ohm con fuente estable. NO conectar un parlante directo al GPIO.
//  API ledc para arduino-esp32 core 3.x (ledcAttach / ledcWriteTone).
// ===========================================================================
#include "config.h"
#include "sound_manager.h"

#if SOUND_ENABLED
static uint32_t offAt   = 0;
static bool     playing = false;

void soundBegin() {
  ledcAttach(SOUND_PWM_PIN, 2000, 8);   // core 3.x: (pin, freqBase, resolucion)
}

void soundPlay(int fx) {                 // fx: 0..n -> tono simple distinto
  uint32_t freq = 800 + (uint32_t)(fx < 0 ? 0 : fx) * 220;
  ledcWriteTone(SOUND_PWM_PIN, freq);
  playing = true;
  offAt   = millis() + 150;             // beep corto
}

void soundUpdate() {
  if (playing && (int32_t)(millis() - offAt) >= 0) {
    ledcWriteTone(SOUND_PWM_PIN, 0);     // silencio
    playing = false;
  }
}

#else  // ---- desactivado: no-op ----
void soundBegin() {}
void soundUpdate() {}
void soundPlay(int fx) { (void)fx; }
#endif
