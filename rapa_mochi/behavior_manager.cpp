// ===========================================================================
//  behavior_manager.cpp  -  Reacciones espontaneas estando idle (no bloqueante).
// ===========================================================================
#include "config.h"
#include "behavior_manager.h"
#include "emotion_manager.h"
#include "storage_manager.h"

static bool     enabled  = true;
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

void behaviorUpdate(uint32_t now) {
  if (!enabled) return;

  // Solo "reacciona" cuando descansa sobre su cara base (sin emocion transitoria).
  // Nota: usar emotionCurrent()!=emotionDefault() (no emotionActive) para que la
  // personalidad siga funcionando aunque la base elegida no sea idle.
  if (emotionCurrent() != emotionDefault()) { lastTick = now; return; }

  if (now - lastTick < BEHAVIOR_TICK_MS) return;
  lastTick = now;

  if ((int)random(0, 100) < BEHAVIOR_CHANCE) {
    static const Emotion moods[] = {
      Emotion::HAPPY, Emotion::THINKING, Emotion::SLEEPY, Emotion::SURPRISED,
      Emotion::SMILE, Emotion::UWU, Emotion::SAKURA, Emotion::LOOK_DOWN,
      Emotion::DISTRACTED, Emotion::LAUGH, Emotion::BEE,
    };
    const int N = sizeof(moods) / sizeof(moods[0]);
    emotionRequest(moods[random(0, N)]);   // respeta prioridad (force=false)
  }
}
