// ===========================================================================
//  config.h  -  Configuracion central de Rapa Mochi (pines, timings, flags).
//  Sin codigo: solo #defines. Incluido por casi todos los modulos.
// ===========================================================================
#pragma once
#include <Arduino.h>

// --- Version de firmware ---
#define FW_VERSION          "0.2.0-modular"

// --- Display / I2C  (CONSTRAINTS DUROS: no cambiar) ---
#define OLED_SDA            21
#define OLED_SCL            22
#define OLED_ADDR           0x3C
#define OLED_BUS_HZ         400000UL
// Rotacion U8G2_R0 se aplica en display_manager.cpp (requiere U8g2lib).

// --- Animacion ---
#define ANIM_FRAMES         90
// 0 = cadencia libre (igual que hoy). Sube (p.ej. 33) si la animacion va muy rapida.
#define FRAME_INTERVAL_MS   0

// --- LED de estado (azul integrado) ---
#define LED_WIFI            2          // GPIO2 (strapping, seguro como salida post-boot)
#define LED_BLINK_CADA_MS   5000
#define LED_BLINK_DUR_MS    100

// --- Timings de arranque (TODOS no bloqueantes, via millis()) ---
#define GREETING_MS         2500       // "Hola" / "Soy Rapa Mochi"
#define WIFI_TRY_MS         10000      // ventana para conectar al WiFi guardado
#define WIFI_INFO_MS        10000      // "WiFi OK" + IP en pantalla

// --- Portal cautivo ---
#define AP_SSID             "RapaMochi_Setup"

// --- Botones fisicos (FUTURO; GPIO 25/26/27 limpios, soportan INPUT_PULLUP) ---
#define BUTTON_1            25
#define BUTTON_2            26
#define BUTTON_3            27
#define BTN_DEBOUNCE_MS     40

// --- Almacenamiento NVS ---
#define NVS_NAMESPACE       "rapamochi"

// ===========================================================================
//  Flags de managers (DESACTIVADOS por defecto -> compile-safe, sin libs extra)
//  Cambiar a 1 cuando el hardware/lib correspondiente este listo.
// ===========================================================================
#define INPUT_ENABLED       0
#define MQTT_ENABLED        0
#define SOUND_ENABLED       0
#define VIBRATION_ENABLED   0
#define BATTERY_ENABLED     0

// --- Pines de managers futuros (SIN colision con los botones 25/26/27) ---
#define VIBRATION_PIN       13         // via MOSFET/transistor + diodo flyback
#define SOUND_PWM_PIN       14         // buzzer pasivo / PAM8403 (ledcWriteTone)
#define BATTERY_ADC_PIN     34         // ADC1, input-only, seguro con WiFi ON

// --- MQTT (futuro; solo se usa si MQTT_ENABLED 1) ---
#define MQTT_SERVER         "192.168.1.100"
#define MQTT_PORT           1883
#define MQTT_USER           ""
#define MQTT_PASSWORD       ""
#define MQTT_TOPIC_IN       "rapamochi/in"
#define MQTT_TOPIC_OUT      "rapamochi/out"

// --- Guarda de colision de pines en tiempo de compilacion ---
#if SOUND_ENABLED && INPUT_ENABLED && (SOUND_PWM_PIN==BUTTON_1 || SOUND_PWM_PIN==BUTTON_2 || SOUND_PWM_PIN==BUTTON_3)
  #error "Colision de pines: SOUND_PWM_PIN pisa un boton. Reasigna SOUND_PWM_PIN en config.h"
#endif
#if VIBRATION_ENABLED && INPUT_ENABLED && (VIBRATION_PIN==BUTTON_1 || VIBRATION_PIN==BUTTON_2 || VIBRATION_PIN==BUTTON_3)
  #error "Colision de pines: VIBRATION_PIN pisa un boton. Reasigna VIBRATION_PIN en config.h"
#endif
