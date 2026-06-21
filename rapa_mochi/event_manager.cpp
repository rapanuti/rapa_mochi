// ===========================================================================
//  event_manager.cpp  -  Despacha eventos a acciones (emocion/secuencia/estado).
// ===========================================================================
#include "event_manager.h"
#include "emotion_manager.h"
#include "sequence_manager.h"
#include "storage_manager.h"
#include "mqtt_manager.h"        // publicar eventos hacia n8n (no-op si MQTT off)

struct Action { ActionType type; int param; };
static Action btn[3];

static const char* ACT_NAMES[] = { "none", "emotion", "sequence", "status" };

// Nombres de eventos (mismo orden que EventType) para publicarlos por MQTT.
static const char* EV_NAMES[] = {
  "boot", "wifi_connected", "wifi_failed",
  "button_1", "button_2", "button_3",
  "web_command", "mqtt_message", "piezo_tap",
  "temp_hot", "temp_cold", "low_battery",
};

const char* actionTypeName(ActionType a) { return ACT_NAMES[(uint8_t)a]; }

ActionType actionTypeFromName(const String& s) {
  for (uint8_t i = 0; i < 4; i++)
    if (s.equalsIgnoreCase(ACT_NAMES[i])) return (ActionType)i;
  return ActionType::NONE;
}

void eventBegin() {
  // Acciones por defecto de los botones (editables desde la web).
  Action def[3] = {
    { ActionType::EMOTION,  (int)Emotion::HAPPY },
    { ActionType::EMOTION,  (int)Emotion::SURPRISED },
    { ActionType::SEQUENCE, 0 },
  };
  for (int i = 0; i < 3; i++) {
    btn[i].type  = (ActionType)storageGetInt((String("b") + i + "t").c_str(), (int)def[i].type);
    btn[i].param = storageGetInt((String("b") + i + "p").c_str(), def[i].param);
  }
}

void eventSetButtonAction(int i, ActionType a, int p) {
  if (i < 0 || i > 2) return;
  btn[i].type = a; btn[i].param = p;
  storagePutInt((String("b") + i + "t").c_str(), (int)a);
  storagePutInt((String("b") + i + "p").c_str(), p);
}

ActionType eventButtonActionType(int i)  { return (i >= 0 && i < 3) ? btn[i].type  : ActionType::NONE; }
int        eventButtonActionParam(int i) { return (i >= 0 && i < 3) ? btn[i].param : 0; }

static bool runAction(const Action& a) {
  switch (a.type) {
    case ActionType::EMOTION:
      if (a.param >= 0 && a.param < (int)Emotion::COUNT) {
        seqStop();                                  // corta secuencia en curso (como la web)
        emotionRequest((Emotion)a.param);
        return true;
      }
      return false;
    case ActionType::SEQUENCE:
      seqStop();
      seqPlay(a.param);
      if (!seqPlaying()) emotionForceDefault();     // slot vacio -> vuelve a la base
      return true;
    case ActionType::STATUS:
      seqStop();
      emotionRequest(Emotion::WIFI_OK);
      return true;
    default:
      return false;
  }
}

bool eventPost(EventType t, int32_t payload) {
  (void)payload;
  // Publica el evento hacia n8n (no-op si MQTT esta desactivado/sin conexion).
  if ((uint8_t)t < (uint8_t)EventType::COUNT)
    mqttPublish((String("event:") + EV_NAMES[(uint8_t)t]).c_str());
  switch (t) {
    case EventType::BUTTON_1_PRESSED: return runAction(btn[0]);
    case EventType::BUTTON_2_PRESSED: return runAction(btn[1]);
    case EventType::BUTTON_3_PRESSED: return runAction(btn[2]);
    case EventType::WIFI_CONNECTED:   emotionRequest(Emotion::WIFI_OK);      return true;
    case EventType::WIFI_FAILED:      emotionRequest(Emotion::WIFI_FAIL);    return true;
    case EventType::PIEZO_TAP:        emotionRequest(Emotion::SURPRISED);    return true;
    case EventType::TEMPERATURE_HOT:  emotionRequest(Emotion::HOT);          return true;
    case EventType::TEMPERATURE_COLD: emotionRequest(Emotion::COLD);         return true;
    case EventType::LOW_BATTERY:      emotionRequest(Emotion::SAD);          return true;
    case EventType::MQTT_MESSAGE:     emotionRequest(Emotion::NOTIFICATION); return true;
    case EventType::WEB_COMMAND:      return true;
    case EventType::BOOT:
    default:                          return false;
  }
}

void eventUpdate(uint32_t now) { (void)now; }   // reservado para reglas temporizadas
