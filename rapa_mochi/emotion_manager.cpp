// ===========================================================================
//  emotion_manager.cpp  -  Estado + prioridades + render de la emocion actual.
// ===========================================================================
#include "emotion_manager.h"
#include "face_renderer.h"

// Indexada por el enum Emotion (mismo orden). COUNT entradas.
static const EmotionDef TABLE[] = {
  {"idle",         0,    0},
  {"happy",        2000, 1},
  {"sad",          2000, 1},
  {"surprised",    1500, 2},
  {"angry",        2000, 2},
  {"sleepy",       4000, 0},
  {"hot",          2000, 1},
  {"cold",         2000, 1},
  {"thinking",     2000, 1},
  {"notification", 1500, 3},
  {"error",        2500, 4},
  {"wifi_ok",      2000, 2},
  {"wifi_fail",    2000, 2},
};

static Emotion  current = Emotion::IDLE;
static Emotion  baseEmo = Emotion::IDLE;   // emocion por defecto al expirar
static uint32_t tStart  = 0;
static uint16_t curDur  = 0;
static uint8_t  curPrio = 0;

static const EmotionDef& def(Emotion e) { return TABLE[(uint8_t)e]; }

void emotionBegin() {
  current = Emotion::IDLE;
  baseEmo = Emotion::IDLE;
  curDur = 0; curPrio = 0;
}

bool emotionRequestFor(Emotion e, uint16_t ms, bool force) {
  uint8_t p = def(e).priority;
  // force=true (secuencias) ignora la prioridad; force=false (peticiones puntuales) la respeta.
  if (!force && emotionActive() && p < curPrio) return false;
  current = e;
  curDur  = ms;
  curPrio = p;
  tStart  = millis();
  return true;
}

bool emotionRequest(Emotion e) { return emotionRequestFor(e, def(e).durationMs); }

void emotionUpdate(uint32_t now) {
  if (current == Emotion::IDLE) return;
  if (curDur != 0 && (now - tStart) >= curDur) {
    current = baseEmo;     // vuelve a la emocion por defecto
    curDur  = 0;           // la base persiste (no vuelve a expirar)
    curPrio = 0;
  }
}

bool        emotionActive()           { return current != Emotion::IDLE; }
void        emotionRender(uint32_t now){ faceRender(current, now); }
Emotion     emotionCurrent()          { return current; }
const char* emotionName(Emotion e)    { return def(e).name; }

Emotion emotionFromName(const String& s) {
  for (uint8_t i = 0; i < (uint8_t)Emotion::COUNT; i++)
    if (s.equalsIgnoreCase(TABLE[i].name)) return (Emotion)i;
  return Emotion::IDLE;
}

void    emotionSetDefault(Emotion e) {
  baseEmo = e;
  if (!emotionActive()) { current = e; curDur = 0; curPrio = 0; }
}
Emotion emotionDefault() { return baseEmo; }

void emotionForceDefault() {   // retorno explicito al estado base (fin de secuencia)
  current = baseEmo;
  curDur  = 0;
  curPrio = 0;
}
