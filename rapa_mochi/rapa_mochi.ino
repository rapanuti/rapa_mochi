// ===========================================================================
//  Rapa Mochi  -  Companion animado en OLED sobre ESP32-WROOM-32
//  Arquitectura modular (ver docs/). Este .ino es solo el orquestador:
//  setup() + loop() + maquina de estados de arranque (BootState).
//
//  Adaptacion/evolucion por Oswaldo Jose Anzola ("Jose").
//  Basado en el "Dasai Mochi" de upir (https://github.com/upiir/esp32s3_oled_dasai_mochi).
//
//  Hardware probado: ESP32 Dev Module / ESP32-WROOM-32, OLED SSD1306 128x64 I2C
//  @0x3C, SDA=21, SCL=22, rotacion 0. Upload 115200. Particion: Minimal SPIFFS.
// ===========================================================================
#include <Arduino.h>
#include "config.h"

// --- Modulos implementados (MVP) ---
#include "display_manager.h"
#include "animation_manager.h"
#include "led_status.h"
#include "storage_manager.h"
#include "wifi_manager.h"

// --- Emociones / secuencias / web (Bloque A) ---
#include "emotion_manager.h"
#include "sequence_manager.h"
#include "notice_manager.h"

// --- Botones / eventos / personalidad (Bloque B) ---
#include "event_manager.h"
#include "input_manager.h"
#include "behavior_manager.h"

// --- Arquitectura preparada (stubs desactivados) ---
#include "mqtt_manager.h"
#include "sound_manager.h"
#include "vibration_manager.h"
#include "battery_manager.h"
#include "web_config_server.h"

// ---------------------------------------------------------------------------
//  Maquina de estados de arranque (todo no bloqueante, via millis())
// ---------------------------------------------------------------------------
enum BootState { BOOT_GREETING, BOOT_WIFI_TRY, BOOT_PORTAL, BOOT_WIFI_INFO, RUN_IDLE };

static BootState    bootState;
static BootState    drawnState = (BootState)255;   // fuerza el primer dibujo
static unsigned long tEstado;

// Dibuja la pantalla estatica del estado actual UNA sola vez (al entrar).
static void drawStateOnce() {
  if (drawnState == bootState) return;
  drawnState = bootState;
  switch (bootState) {
    case BOOT_GREETING:  dispGreeting();               break;
    case BOOT_WIFI_TRY:  dispConnecting();             break;
    case BOOT_PORTAL:    dispPortal(wifiPortalApIP()); break;
    case BOOT_WIFI_INFO:
      if (storageGetInt("newcfg", 0)) {        // recien configurada desde el portal
        storagePutInt("newcfg", 0);            // limpia la marca (solo una vez)
        dispNewNetwork(wifiIP());
      } else {
        dispWifiOk(wifiIP());
      }
      break;
    case RUN_IDLE:       /* la animacion se dibuja en loop */ break;
  }
}

// ---------------------------------------------------------------------------
void setup() {
  Serial.begin(115200);
  delay(150);
  Serial.println();
  Serial.println(F("[BOOT] Rapa Mochi - arquitectura modular"));
  Serial.print  (F("[BOOT] firmware ")); Serial.println(F(FW_VERSION));

  dispBegin();         // OLED lista (Wire 21/22, 0x3C, 400kHz, R0)
  ledInit();           // LED azul GPIO2
  storageBegin();      // NVS
  animBegin();         // animacion idle

  // Emociones + secuencias (Bloque A):
  emotionBegin();
  seqBegin();
  emotionSetDefault(emotionFromName(storageGetString("defemo", "idle")));

  // Botones + eventos + personalidad (Bloque B):
  eventBegin();
  inputBegin();
  behaviorBegin();

  // Managers de hardware (no-op mientras esten desactivados):
  mqttBegin();
  soundBegin();
  vibrationBegin();
  batteryBegin();

  // Saludo inicial configurable desde la web: si esta OFF, va directo a conectar.
  if (storageGetInt("greet", 1)) {
    bootState = BOOT_GREETING;
  } else {
    wifiBeginTry();
    bootState = BOOT_WIFI_TRY;
  }
  tEstado = millis();
  eventPost(EventType::BOOT);
}

// ---------------------------------------------------------------------------
void loop() {
  unsigned long now = millis();

  ledUpdate();          // destello LED si hay WiFi (no bloquea)
  inputUpdate(now);     // botones (no-op si INPUT_ENABLED 0)
  drawStateOnce();      // pinta la pantalla del estado al entrar

  switch (bootState) {

    case BOOT_GREETING:                       // "Hola" / "Soy Rapa Mochi"
      if (now - tEstado >= GREETING_MS) {
        wifiBeginTry();                        // NVS -> fallback secrets.h
        bootState = BOOT_WIFI_TRY;
        tEstado   = now;
      }
      break;

    case BOOT_WIFI_TRY:                        // intentando conectar
      switch (wifiPoll()) {
        case WT_CONNECTED:
          eventPost(EventType::WIFI_CONNECTED);
          bootState = BOOT_WIFI_INFO; tEstado = now;
          break;
        case WT_TIMEOUT:
          eventPost(EventType::WIFI_FAILED);
          dispCreatingAP();                    // "Sin WiFi / Creando red" durante el escaneo
          wifiStartPortal();                   // abre AP RapaMochi_Setup (bloquea ~2s)
          bootState = BOOT_PORTAL; tEstado = now;
          break;
        default:                               // WT_CONNECTING: seguir esperando
          break;
      }
      break;

    case BOOT_PORTAL:                          // portal cautivo activo
      wifiHandlePortal();                       // DNS + HTTP (no bloquea)
      if (wifiPortalGotCreds()) {               // el usuario guardo su red
        dispSaved();
        wifiReboot();                           // reinicia y conecta solo
      }
      break;

    case BOOT_WIFI_INFO:                       // "WiFi OK" + IP unos segundos
      if (now - tEstado >= WIFI_INFO_MS) {
        if (wifiIsConnected()) webBegin();     // arranca el panel web (puerto 80)
        inputJustPressed(0); inputJustPressed(1); inputJustPressed(2);  // descarta flancos del arranque
        bootState = RUN_IDLE;
      }
      break;

    case RUN_IDLE:                             // estado normal
      // Botones fisicos -> eventos -> acciones (emocion/secuencia)
      if (inputJustPressed(0)) eventPost(EventType::BUTTON_1_PRESSED);
      if (inputJustPressed(1)) eventPost(EventType::BUTTON_2_PRESSED);
      if (inputJustPressed(2)) eventPost(EventType::BUTTON_3_PRESSED);

      if (wifiIsConnected()) webUpdate();      // atiende el panel web
      seqUpdate(now);                          // avanza la secuencia si hay una
      eventUpdate(now);                        // reglas temporizadas (reservado)
      behaviorUpdate(now);                     // personalidad autonoma (Fase 12)
      emotionUpdate(now);                      // expira la emocion -> base
      if      (noticeActive(now))                          noticeRender();    // mensaje (IA/n8n/web)
      else if (emotionActive() || emotionTransitioning(now)) emotionRender(now); // cara + transicion
      else                                                 animRenderNext();  // animacion Mochi idle

      // Managers de hardware (no-op si su *_ENABLED es 0):
      mqttUpdate();
      soundUpdate();
      vibrationUpdate();
      batteryUpdate();
      break;
  }
}
