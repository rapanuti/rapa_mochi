// ===========================================================================
//  display_manager.h  -  Encapsula la OLED SSD1306 (U8g2). Nadie mas toca u8g2.
// ===========================================================================
#pragma once
#include <Arduino.h>

void dispBegin();                       // Wire 21/22, addr 0x3C, 400kHz, R0
void dispGreeting();                    // "Hola" / "Soy Rapa Mochi"
void dispConnecting();                  // "Conectando WiFi..."
void dispPortal(const String& apIp);    // "Config WiFi" + RapaMochi_Setup + IP
void dispWifiOk(const String& ip);      // "WiFi OK" + "IP: x.x.x.x"
void dispWifiFail();                    // "WiFi FAIL"
void dispSaved();                       // "Guardado / Reiniciando..."
void dispFrame(const unsigned char* xbm); // dibuja un frame 128x64 y refresca
