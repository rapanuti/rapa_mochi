// ===========================================================================
//  emotion_manager.cpp  -  (STUB) Tabla placeholder + estado actual. Sin efectos aun.
// ===========================================================================
#include "emotion_manager.h"

static Emotion current = Emotion::IDLE;

// Tabla de referencia para fases futuras (duracion ms, prioridad).
static const EmotionDef TABLE[] = {
  {"idle",         0,    0},
  {"happy",        1500, 1},
  {"sad",          1500, 1},
  {"surprised",    1200, 2},
  {"angry",        1500, 2},
  {"sleepy",       3000, 0},
  {"hot",          1500, 1},
  {"cold",         1500, 1},
  {"thinking",     1500, 1},
  {"notification", 1200, 3},
  {"error",        2000, 4},
  {"wifi_ok",      1500, 2},
  {"wifi_fail",    1500, 2},
};

void emotionBegin() { current = Emotion::IDLE; (void)TABLE; }
void emotionUpdate(uint32_t now) { (void)now; }        // stub: sin transiciones aun
bool emotionRequest(Emotion e) { current = e; return true; }
Emotion emotionCurrent() { return current; }
