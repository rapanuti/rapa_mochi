// ===========================================================================
//  event_manager.h  -  Reglas evento -> accion (emocion / secuencia / estado).
//  Las acciones de los 3 botones son configurables desde la web y se guardan en NVS.
// ===========================================================================
#pragma once
#include <Arduino.h>

enum class EventType : uint8_t {
  BOOT, WIFI_CONNECTED, WIFI_FAILED,
  BUTTON_1_PRESSED, BUTTON_2_PRESSED, BUTTON_3_PRESSED,
  WEB_COMMAND, MQTT_MESSAGE, PIEZO_TAP,
  TEMPERATURE_HOT, TEMPERATURE_COLD, LOW_BATTERY, COUNT
};

enum class ActionType : uint8_t { NONE, EMOTION, SEQUENCE, STATUS };

void        eventBegin();
bool        eventPost(EventType t, int32_t payload = -1);   // ejecuta la accion asociada
void        eventUpdate(uint32_t now);

// --- Config de acciones de botones (para la web) ---
void        eventSetButtonAction(int btn, ActionType a, int param);
ActionType  eventButtonActionType(int btn);
int         eventButtonActionParam(int btn);
const char* actionTypeName(ActionType a);
ActionType  actionTypeFromName(const String& s);
