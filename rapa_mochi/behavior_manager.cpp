// ===========================================================================
//  behavior_manager.cpp  -  Reacciones espontaneas estando idle (no bloqueante).
// ===========================================================================
#include "config.h"
#include "behavior_manager.h"
#include "emotion_manager.h"
#include "storage_manager.h"

static bool     enabled  = true;
static bool     demo     = false;
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

void behaviorSetDemo(bool on) { demo = on; demoIdx = 1; lastTick = 0; }
bool behaviorDemo()           { return demo; }

void behaviorUpdate(uint32_t now) {
  // Modo demo: recorre TODAS las caras (1..COUNT-1), ~3 s cada una.
  if (demo) {
    if (now - lastTick >= 3000) {
      lastTick = now;
      emotionRequestFor((Emotion)demoIdx, 3000, /*force=*/true);
      if (++demoIdx >= (uint8_t)Emotion::COUNT) demoIdx = 1;
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
