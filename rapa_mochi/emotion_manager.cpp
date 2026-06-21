// ===========================================================================
//  emotion_manager.cpp  -  Estado + prioridades + transicion (parpadeo) + render.
// ===========================================================================
#include "emotion_manager.h"
#include "face_renderer.h"
#include "display_manager.h"     // para la transicion (parpados) y el flush

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
  // --- Catalogo Dasai Mochi (Gen 3) ---
  {"smile",        2000, 1},
  {"laugh",        2200, 1},
  {"uwu",          2500, 1},
  {"raspberry",    2000, 1},
  {"sakura",       5000, 1},
  {"rainbow",      4000, 1},
  {"dumb_love",    2500, 1},
  {"love",         3000, 1},
  {"distracted",   3000, 1},
  {"sneeze",       1500, 2},
  {"squint",       2000, 0},
  {"look_down",    2500, 1},
  {"shift",        2500, 1},
  {"bee",          5000, 1},
  {"gtr_rain",     5000, 1},
  {"neon_tube",    4000, 1},
  {"pong",         8000, 1},
  {"rotation",     6000, 1},
  {"gforce",       3000, 1},
  {"splat",        2500, 2},
};

// Atrapa en compilacion cualquier desajuste futuro entre la tabla y el enum.
static_assert(sizeof(TABLE) / sizeof(TABLE[0]) == (size_t)Emotion::COUNT,
              "TABLE debe tener tantas entradas como Emotion::COUNT");

#define EMO_TRANS_MS 200          // duracion del parpadeo de transicion

static Emotion  current   = Emotion::IDLE;
static Emotion  baseEmo   = Emotion::IDLE;
static Emotion  prevEmo   = Emotion::IDLE;
static uint32_t tStart    = 0;
static uint32_t transStart= 0;
static uint16_t curDur    = 0;
static uint8_t  curPrio   = 0;

static const EmotionDef& def(Emotion e) { return TABLE[(uint8_t)e]; }

static void beginTransition() {     // arranca el parpadeo desde la emocion actual
  prevEmo    = current;
  transStart = millis();
}

void emotionBegin() {
  current = Emotion::IDLE;
  baseEmo = Emotion::IDLE;
  curDur = 0; curPrio = 0;
}

bool emotionRequestFor(Emotion e, uint16_t ms, bool force) {
  uint8_t p = def(e).priority;
  if (!force && emotionActive() && p < curPrio) return false;
  if (e != current) beginTransition();
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
    if (current != baseEmo) beginTransition();
    current = baseEmo;
    curDur  = 0;
    curPrio = 0;
  }
}

bool emotionActive() { return current != Emotion::IDLE; }

bool emotionTransitioning(uint32_t now) { return (now - transStart) < EMO_TRANS_MS; }

void emotionRender(uint32_t now) {
  uint32_t t   = now - transStart;
  Emotion  show = current;
  int      lid  = 0;
  if (t < EMO_TRANS_MS) {                       // estamos en transicion (parpadeo)
    show = (t < EMO_TRANS_MS / 2) ? prevEmo : current;
    lid  = (t <= EMO_TRANS_MS / 2)
             ? (int)(66UL * t / EMO_TRANS_MS)
             : (int)(66UL * (EMO_TRANS_MS - t) / EMO_TRANS_MS);   // 0 -> 33 -> 0
  }
  faceRender(show, now);                         // dibuja la cara (sin flush)
  if (lid > 0) {                                 // parpados negros cerrando/abriendo
    dispSetColor(0);
    dispBox(0, 0, 128, lid);
    dispBox(0, 64 - lid, 128, lid);
    dispSetColor(1);
  }
  dispFlush();
}

Emotion     emotionCurrent()       { return current; }
const char* emotionName(Emotion e) { return def(e).name; }

Emotion emotionFromName(const String& s) {
  for (uint8_t i = 0; i < (uint8_t)Emotion::COUNT; i++)
    if (s.equalsIgnoreCase(TABLE[i].name)) return (Emotion)i;
  return Emotion::IDLE;
}

void emotionSetDefault(Emotion e) {
  baseEmo = e;
  if (!emotionActive()) { if (e != current) beginTransition(); current = e; curDur = 0; curPrio = 0; }
}
Emotion emotionDefault() { return baseEmo; }

void emotionForceDefault() {
  if (current != baseEmo) beginTransition();
  current = baseEmo;
  curDur  = 0;
  curPrio = 0;
}
