// ===========================================================================
//  emotion_manager.h  -  Motor de emociones del Mochi (caras procedurales).
//  IDLE = animacion Mochi (bitmap). Resto = caras dibujadas (face_renderer).
// ===========================================================================
#pragma once
#include <Arduino.h>

enum class Emotion : uint8_t {
  IDLE, HAPPY, SAD, SURPRISED, ANGRY, SLEEPY, HOT, COLD,
  THINKING, NOTIFICATION, ERROR, WIFI_OK, WIFI_FAIL,
  // --- Catalogo Dasai Mochi (Gen 3) ---
  SMILE, LAUGH, UWU, RASPBERRY, SAKURA, RAINBOW, DUMB_LOVE, LOVE,
  DISTRACTED, SNEEZE, SQUINT, LOOK_DOWN, SHIFT, BEE, GTR_RAIN,
  NEON_TUBE, PONG, ROTATION, GFORCE, SPLAT,
  COUNT
};

struct EmotionDef {
  const char* name;
  uint16_t    durationMs;   // 0 = permanente (hasta otra peticion)
  uint8_t     priority;     // mayor = mas importante (no lo interrumpe uno menor)
};

void        emotionBegin();
void        emotionUpdate(uint32_t now);             // expira -> emocion por defecto
bool        emotionRequest(Emotion e);               // duracion de la tabla (respeta prioridad)
bool        emotionRequestFor(Emotion e, uint16_t ms, bool force = false); // force = ignora prioridad
void        emotionForceDefault();                   // vuelve YA a la emocion base
Emotion     emotionCurrent();
bool        emotionActive();                          // hay cara procedural vigente
bool        emotionTransitioning(uint32_t now);       // hay un parpadeo de transicion en curso
void        emotionRender(uint32_t now);              // dibuja la cara actual
const char* emotionName(Emotion e);
Emotion     emotionFromName(const String& s);         // IDLE si no coincide
void        emotionSetDefault(Emotion e);             // emocion base al expirar
Emotion     emotionDefault();
