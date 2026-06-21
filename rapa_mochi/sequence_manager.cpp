// ===========================================================================
//  sequence_manager.cpp  -  Parser + reproductor de secuencias de emociones.
// ===========================================================================
#include "sequence_manager.h"
#include "emotion_manager.h"
#include "storage_manager.h"

static String   names[SEQ_MAX];
static String   datas[SEQ_MAX];

static bool     playing   = false;
static bool     loopMode  = false;   // repetir la secuencia al terminar
static String   playData;        // datos en reproduccion
static int      stepIdx   = 0;
static uint32_t stepStart = 0;
static uint16_t stepDur   = 0;

void seqBegin() {
  for (int i = 0; i < SEQ_MAX; i++) {
    names[i] = storageGetString((String("s") + i + "n").c_str(), "");
    datas[i] = storageGetString((String("s") + i + "d").c_str(), "");
  }
}

int    seqSlots()        { return SEQ_MAX; }
String seqName(int i)    { return (i >= 0 && i < SEQ_MAX) ? names[i] : String(""); }
String seqData(int i)    { return (i >= 0 && i < SEQ_MAX) ? datas[i] : String(""); }

bool seqSave(int i, const String& name, const String& data) {
  if (i < 0 || i >= SEQ_MAX) return false;
  names[i] = name;
  datas[i] = data;
  storagePutString((String("s") + i + "n").c_str(), name);
  storagePutString((String("s") + i + "d").c_str(), data);
  return true;
}

// Lee el paso 'stepIdx' de playData ("emo:ms"). Devuelve false si no hay mas pasos.
static bool startStep() {
  // Saltar hasta el paso stepIdx (separados por ';')
  int count = 0, start = 0;
  for (int i = 0; i <= (int)playData.length(); i++) {
    bool end = (i == (int)playData.length()) || (playData[i] == ';');
    if (end) {
      if (count == stepIdx) {
        String tok = playData.substring(start, i);   // "emo:ms"
        tok.trim();
        if (tok.length() == 0) return false;
        int c = tok.indexOf(':');
        String emo = (c >= 0) ? tok.substring(0, c) : tok;
        uint16_t ms = (c >= 0) ? (uint16_t)tok.substring(c + 1).toInt() : 1000;
        if (ms == 0) ms = 1000;
        // La emocion se pide PERMANENTE (dur=0): la SECUENCIA controla el tiempo
        // con stepDur. Asi se evita la carrera (la emocion ya no expira sola).
        emotionRequestFor(emotionFromName(emo), 0, /*force=*/true);
        stepStart = millis();
        stepDur   = ms;
        return true;
      }
      count++;
      start = i + 1;
    }
  }
  return false;   // stepIdx fuera de rango -> fin
}

void seqPlayData(const String& data, bool loop) {
  playData = data;
  stepIdx  = 0;
  loopMode = loop;
  playing  = startStep();
}

void seqPlay(int i, bool loop) {
  if (i >= 0 && i < SEQ_MAX && datas[i].length() > 0) seqPlayData(datas[i], loop);
}

void seqUpdate(uint32_t now) {
  if (!playing) return;
  if (now - stepStart >= stepDur) {
    stepIdx++;
    if (!startStep()) {                 // no hay mas pasos
      if (loopMode) {                   // loop: reinicia desde el primero
        stepIdx = 0;
        if (!startStep()) { playing = false; emotionForceDefault(); }
      } else {
        playing = false;
        emotionForceDefault();          // vuelve YA al estado base
      }
    }
  }
}

bool seqPlaying() { return playing; }

void seqStop() {
  if (playing) {
    playing = false;
    emotionForceDefault();   // la ultima emocion era permanente: vuelve a la base
  }
}
