// ===========================================================================
//  mqtt_manager.h  -  (STUB) Activar con MQTT_ENABLED 1 (requiere PubSubClient).
//  Futuro: integracion con n8n, Home Assistant, Node-RED, alertas.
// ===========================================================================
#pragma once
#include <Arduino.h>

void mqttBegin();
void mqttUpdate();
bool mqttPublish(const char* payload);
