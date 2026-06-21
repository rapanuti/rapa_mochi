// ===========================================================================
//  led_status.cpp  -  Extraido del sketch original (parpadeoWiFiLED).
// ===========================================================================
#include <Arduino.h>
#include <WiFi.h>
#include "config.h"
#include "led_status.h"

static unsigned long ultimoBlink    = 0;
static unsigned long encendidoDesde = 0;
static bool          encendido      = false;

void ledInit() {
  pinMode(LED_WIFI, OUTPUT);
  digitalWrite(LED_WIFI, LOW);
}

void ledUpdate() {
  unsigned long ahora = millis();

  // Sin WiFi: LED apagado.
  if (WiFi.status() != WL_CONNECTED) {
    if (encendido) { digitalWrite(LED_WIFI, LOW); encendido = false; }
    return;
  }

  // Con WiFi: destello breve cada LED_BLINK_CADA_MS.
  if (!encendido && (ahora - ultimoBlink >= LED_BLINK_CADA_MS)) {
    digitalWrite(LED_WIFI, HIGH);
    encendido      = true;
    encendidoDesde = ahora;
    ultimoBlink    = ahora;
  }
  if (encendido && (ahora - encendidoDesde >= LED_BLINK_DUR_MS)) {
    digitalWrite(LED_WIFI, LOW);
    encendido = false;
  }
}
