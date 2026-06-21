// ===========================================================================
//  event_manager.cpp  -  (STUB) Por ahora solo registra por Serial. Sin reglas aun.
// ===========================================================================
#include <Arduino.h>
#include "event_manager.h"

void eventBegin() {}

bool eventPost(EventType t, int32_t payload) {
  // Stub: en fases futuras esto disparara emociones/secuencias segun reglas.
  Serial.print(F("[EVENT] ")); Serial.print((int)t);
  Serial.print(F(" payload=")); Serial.println(payload);
  return false;
}

void eventUpdate(uint32_t now) { (void)now; }
