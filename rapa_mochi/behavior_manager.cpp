// ===========================================================================
//  behavior_manager.cpp  -  Reacciones espontaneas estando idle (no bloqueante).
// ===========================================================================
#include "config.h"
#include "behavior_manager.h"
#include "emotion_manager.h"
#include "storage_manager.h"

static bool     enabled  = true;
static bool     demo     = false;
static bool     randomM  = false;
static uint8_t  demoIdx  = 1;
static uint32_t lastTick = 0;

void behaviorBegin() {
  enabled  = storageGetInt("behav", 1) != 0;
  lastTick = 0;
  randomSeed(micros());                 // semilla para random()
}

void behaviorSetEnabled(bool on) {
  enabled = on;
  storagePutInt("behav", on ? 1 : 0);
}

bool behaviorEnabled() { return enabled; }

void behaviorSetDemo(bool on)   { demo = on; if (on) randomM = false; demoIdx = 1; lastTick = 0; }
bool behaviorDemo()             { return demo; }
void behaviorSetRandom(bool on) { randomM = on; if (on) demo = false; lastTick = 0; }
bool behaviorRandom()           { return randomM; }

void behaviorUpdate(uint32_t now) {
  // Tiempos disponibles (igual que el constructor de secuencias).
  static const uint16_t TIMES[] = { 5000, 10000, 15000, 20000 };

  // Modo demo: recorre TODAS las caras (1..COUNT-1), ~3 s cada una.
  // Emocion PERMANENTE (dur=0): el temporizador del demo controla el cambio.
  if (demo) {
    if (now - lastTick >= 3000) {
      lastTick = now;
      emotionRequestFor((Emotion)demoIdx, 0, /*force=*/true);
      if (++demoIdx >= (uint8_t)Emotion::COUNT) demoIdx = 1;
    }
    return;
  }

  // Modo aleatorio: emocion al azar con duracion al azar (5/10/15/20 s), en bucle.
  if (randomM) {
    if (!emotionActive() && (now - lastTick) >= 200) {
      lastTick = now;
      Emotion  e  = (Emotion)(1 + random(0, (long)Emotion::COUNT - 1));   // 1..COUNT-1
      uint16_t ms = TIMES[random(0, 4)];
      emotionRequestFor(e, ms, /*force=*/true);
    }
    return;
  }

  if (!enabled) return;

  // Solo "reacciona" cuando descansa sobre su cara base (sin emocion transitoria).
  if (emotionCurrent() != emotionDefault()) { lastTick = now; return; }

  if (now - lastTick < BEHAVIOR_TICK_MS) return;
  lastTick = now;

  if ((int)random(0, 100) < BEHAVIOR_CHANCE) {
    // Repertorio VARIADO (no solo sonrisas) para que se note el cambio.
    static const Emotion moods[] = {
      Emotion::HAPPY, Emotion::SAD, Emotion::SURPRISED, Emotion::ANGRY, Emotion::SLEEPY,
      Emotion::THINKING, Emotion::HOT, Emotion::COLD, Emotion::UWU, Emotion::SAKURA,
      Emotion::LOVE, Emotion::DISTRACTED, Emotion::SNEEZE, Emotion::LOOK_DOWN,
      Emotion::SHIFT, Emotion::BEE, Emotion::ROTATION, Emotion::GFORCE, Emotion::SQUINT,
    };
    const int N = sizeof(moods) / sizeof(moods[0]);
    emotionRequest(moods[random(0, N)]);   // respeta prioridad (force=false)
  }
}
