// ===========================================================================
//  wifi_manager.h  -  Conexion STA no bloqueante + portal cautivo propio.
//  Portal = WebServer + DNSServer + Preferences (todo del core ESP32, sin libs).
// ===========================================================================
#pragma once
#include <Arduino.h>

enum WifiTry { WT_CONNECTING, WT_CONNECTED, WT_TIMEOUT };

// --- Modo estacion (STA) ---
void    wifiBeginTry();        // carga creds (NVS -> fallback secrets.h) y WiFi.begin()
WifiTry wifiPoll();            // estado de la conexion, sin bloquear
bool    wifiIsConnected();
String  wifiIP();             // IP local como texto

// --- Portal cautivo (AP RapaMochi_Setup) ---
void    wifiStartPortal();     // levanta AP + DNS + WebServer y escanea redes 1 vez
void    wifiHandlePortal();    // procesa DNS + HTTP (llamar en loop; no bloquea)
bool    wifiPortalGotCreds();  // true cuando el usuario guardo credenciales
String  wifiPortalApIP();      // IP del AP (192.168.4.1)

// --- Utilidades ---
void    wifiReboot();          // ESP.restart()
void    wifiClearAndReboot();  // borra credenciales y reinicia (vuelve a portal)
