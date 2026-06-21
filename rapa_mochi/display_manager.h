// ===========================================================================
//  display_manager.h  -  Encapsula la OLED SSD1306 (U8g2). Nadie mas toca u8g2.
// ===========================================================================
#pragma once
#include <Arduino.h>

void dispBegin();                       // Wire 21/22, addr 0x3C, 400kHz, R0
void dispGreeting();                    // "Hola" / "Soy Rapa Mochi"
void dispConnecting();                  // "Conectando WiFi..."
void dispCreatingAP();                   // "Sin WiFi / Creando red" mientras levanta el portal
void dispPortal(const String& apIp);    // cara + RapaMochi_Setup + IP del portal
void dispWifiOk(const String& ip);      // cara feliz + "IP: x.x.x.x"
void dispNewNetwork(const String& ip);  // cara feliz + "Nueva red!" + IP (tras configurar)
void dispWifiFail();                    // "WiFi FAIL"
void dispSaved();                       // "Guardado / Reiniciando..."
void dispFrame(const unsigned char* xbm); // dibuja un frame 128x64 y refresca

// --- Primitivas de dibujo (para caras procedurales y pantallas custom) ---
void dispClear();                       // limpia el buffer (no refresca)
void dispFlush();                       // envia el buffer a la OLED
void dispSetColor(uint8_t c);           // 1 = pixel encendido, 0 = borrar
void dispPixel(int x, int y);
void dispLine(int x0, int y0, int x1, int y1);
void dispBox(int x, int y, int w, int h);
void dispRBox(int x, int y, int w, int h, int r);
void dispDisc(int x, int y, int r);                 // circulo relleno
void dispCircle(int x, int y, int r);               // circulo contorno
void dispEllipseFilled(int x, int y, int rx, int ry);
void dispTriangle(int x0, int y0, int x1, int y1, int x2, int y2);
void dispTextSmall(int x, int y, const char* s);    // texto fuente 6x10
const uint8_t* dispBuffer();            // buffer interno U8g2 (1024 B, 128x64) para exportar
