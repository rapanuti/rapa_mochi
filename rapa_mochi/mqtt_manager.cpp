// ===========================================================================
//  mqtt_manager.cpp  -  (STUB) Compile-safe. PubSubClient SOLO dentro del #if.
// ===========================================================================
#include "config.h"
#include "mqtt_manager.h"

#if MQTT_ENABLED
  #include <WiFi.h>
  #include <PubSubClient.h>   // Library Manager: "PubSubClient" by Nick O'Leary

  static WiFiClient   net;
  static PubSubClient client(net);

  void mqttBegin() {
    client.setServer(MQTT_SERVER, MQTT_PORT);
    // TODO fase MQTT: conectar, suscribir MQTT_TOPIC_IN, callback -> eventos.
  }
  void mqttUpdate() { client.loop(); }
  bool mqttPublish(const char* payload) {
    return client.connected() ? client.publish(MQTT_TOPIC_OUT, payload) : false;
  }

#else  // ---- desactivado: no-op, no requiere PubSubClient ----
  void mqttBegin() {}
  void mqttUpdate() {}
  bool mqttPublish(const char* payload) { (void)payload; return false; }
#endif
