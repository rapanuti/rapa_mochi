// ===========================================================================
//  emotion_manager.h  -  (STUB) Catalogo de emociones del Mochi. Inerte por ahora.
// ===========================================================================
#pragma once
#include <Arduino.h>

enum class Emotion : uint8_t {
  IDLE, HAPPY, SAD, SURPRISED, ANGRY, SLEEPY, HOT, COLD,
  THINKING, NOTIFICATION, ERROR, WIFI_OK, WIFI_FAIL, COUNT
};

// Definicion de una emocion (campos futuros: animacion/secuencia, sonido, vibracion).
struct EmotionDef {
  const char* name;
  uint16_t    durationMs;
  uint8_t     priority;
};

void    emotionBegin();
void    emotionUpdate(uint32_t now);
bool    emotionRequest(Emotion e);     // pide mostrar una emocion (stub: solo guarda)
Emotion emotionCurrent();
