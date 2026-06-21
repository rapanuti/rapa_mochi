// ===========================================================================
//  mqtt_manager.cpp  -  Cliente MQTT. Compile-safe (MQTT_ENABLED). PubSubClient
//  SOLO se incluye dentro del #if, asi que con MQTT_ENABLED 0 no requiere la lib.
//  Comandos entrantes (MQTT_TOPIC_IN):
//    - nombre de emocion ("happy", "sad", ...) -> muestra esa emocion
//    - "seq:happy:1000;sad:800"                -> reproduce una secuencia
//    - cualquier otro texto                    -> emocion "notification"
// ===========================================================================
#include "config.h"
#include "mqtt_manager.h"

#if MQTT_ENABLED
  #include <WiFi.h>
  #include <PubSubClient.h>      // Library Manager: "PubSubClient" by Nick O'Leary
  #include "emotion_manager.h"
  #include "sequence_manager.h"
  #include "event_manager.h"
  #include "notice_manager.h"
  #include "battery_manager.h"

  static WiFiClient   net;
  static PubSubClient client(net);
  static uint32_t     lastTry  = 0;
  static uint32_t     lastBeat = 0;

  static void onMessage(char* topic, byte* payload, unsigned int len) {
    (void)topic;
    String msg;
    for (unsigned int i = 0; i < len; i++) msg += (char)payload[i];
    msg.trim();
    if (msg.startsWith("text:")) {               // mensaje (IA/n8n) -> se muestra en la OLED
      noticeShow(msg.substring(5));
    } else if (msg.startsWith("seq:")) {         // secuencia de emociones
      seqStop();
      seqPlayData(msg.substring(4));
    } else {
      Emotion e = emotionFromName(msg);
      if (e != Emotion::IDLE || msg.equalsIgnoreCase("idle")) { seqStop(); emotionRequest(e); }
      else noticeShow(msg);                       // texto libre -> se muestra como mensaje
    }
  }

  void mqttBegin() {
    client.setServer(MQTT_SERVER, MQTT_PORT);
    client.setCallback(onMessage);
  }

  static void reconnect() {
    if (client.connected()) return;
    if (WiFi.status() != WL_CONNECTED) return;
    if (millis() - lastTry < 5000) return;       // reintento acotado, no bloquea
    lastTry = millis();
    String id = "rapamochi-" + String((uint32_t)ESP.getEfuseMac(), HEX);
    bool ok = (strlen(MQTT_USER) > 0)
                ? client.connect(id.c_str(), MQTT_USER, MQTT_PASSWORD)
                : client.connect(id.c_str());
    if (ok) {
      client.subscribe(MQTT_TOPIC_IN);
      client.publish(MQTT_TOPIC_OUT, "online");
    }
  }

  void mqttUpdate() {
    reconnect();
    client.loop();

    // Heartbeat de estado hacia n8n cada 30 s (JSON simple).
    if (client.connected() && millis() - lastBeat >= 30000) {
      lastBeat = millis();
      String s = String("{\"uptime\":") + (millis() / 1000)
               + ",\"emotion\":\"" + emotionName(emotionCurrent()) + "\""
               + ",\"ip\":\"" + WiFi.localIP().toString() + "\""
               + ",\"battery\":" + batteryPercent() + "}";
      client.publish(MQTT_TOPIC_OUT, s.c_str());
    }
  }

  bool mqttPublish(const char* payload) {
    return client.connected() ? client.publish(MQTT_TOPIC_OUT, payload) : false;
  }

#else  // ---- desactivado: no-op, no requiere PubSubClient ----
  void mqttBegin() {}
  void mqttUpdate() {}
  bool mqttPublish(const char* payload) { (void)payload; return false; }
#endif
