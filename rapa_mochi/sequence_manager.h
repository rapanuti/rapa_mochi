// ===========================================================================
//  sequence_manager.h  -  Secuencias de emociones con nombre (Fase 5).
//  Formato de datos: "emo:ms;emo:ms;..."  ej: "happy:1000;surprised:600;sad:800"
// ===========================================================================
#pragma once
#include <Arduino.h>

#define SEQ_MAX 4    // numero de secuencias guardables

void   seqBegin();                                   // carga las guardadas de NVS
int    seqSlots();                                   // SEQ_MAX
String seqName(int i);                               // nombre del slot i ("" si vacio)
String seqData(int i);                               // datos del slot i
bool   seqSave(int i, const String& name, const String& data);

void   seqPlay(int i, bool loop = false);            // reproduce el slot i guardado
void   seqPlayData(const String& data, bool loop = false); // reproduce datos (test)
void   seqUpdate(uint32_t now);                      // avanza pasos (llamar en loop)
bool   seqPlaying();
void   seqStop();
