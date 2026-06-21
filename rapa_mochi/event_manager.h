// ===========================================================================
//  event_manager.h  -  (STUB) Fuentes de eventos -> (futuro) emociones/secuencias.
// ===========================================================================
#pragma once
#include <Arduino.h>

enum class EventType : uint8_t {
  BOOT, WIFI_CONNECTED, WIFI_FAILED,
  BUTTON_1_PRESSED, BUTTON_2_PRESSED, BUTTON_3_PRESSED,
  WEB_COMMAND, MQTT_MESSAGE, PIEZO_TAP,
  TEMPERATURE_HOT, TEMPERATURE_COLD, LOW_BATTERY, COUNT
};

void eventBegin();
bool eventPost(EventType t, int32_t payload = -1);   // encola un evento (stub: no-op)
void eventUpdate(uint32_t now);
