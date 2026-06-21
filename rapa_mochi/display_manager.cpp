// ===========================================================================
//  display_manager.cpp  -  Unico dueno del objeto U8g2.
// ===========================================================================
#include <U8g2lib.h>
#include <Wire.h>
#include "config.h"
#include "display_manager.h"

// Mismo driver/constructor que el sketch original (HW I2C, full buffer, R0).
static U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

void dispBegin() {
  Wire.begin(OLED_SDA, OLED_SCL);
  u8g2.setI2CAddress(OLED_ADDR << 1);   // U8g2 usa direccion de 8 bits
  u8g2.setBusClock(OLED_BUS_HZ);
  u8g2.begin();
  Wire.setClock(OLED_BUS_HZ);           // asegura el reloj final del bus
}

// Pantalla de 2 lineas (titulo grande + subtitulo).
static void twoLines(const char* l1, const char* l2) {
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_ncenB10_tr);
  u8g2.drawStr(0, 26, l1);
  if (l2) {
    u8g2.setFont(u8g2_font_6x10_tr);
    u8g2.drawStr(0, 46, l2);
  }
  u8g2.sendBuffer();
}

void dispGreeting()   { twoLines("Hola", "Soy Rapa Mochi"); }
void dispConnecting() { twoLines("Conectando", "WiFi..."); }
void dispWifiFail()   { twoLines("WiFi FAIL", "Sigo sin red..."); }
void dispSaved()      { twoLines("Guardado!", "Reiniciando..."); }

void dispWifiOk(const String& ip) {
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_6x10_tr);
  u8g2.drawStr(0, 20, "WiFi OK");
  String s = "IP: " + ip;
  u8g2.drawStr(0, 40, s.c_str());
  u8g2.sendBuffer();
}

void dispPortal(const String& apIp) {
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_6x10_tr);
  u8g2.drawStr(0, 14, "Config WiFi:");
  u8g2.drawStr(0, 30, AP_SSID);
  String s = "http://" + apIp;
  u8g2.drawStr(0, 46, s.c_str());
  u8g2.sendBuffer();
}

void dispFrame(const unsigned char* xbm) {
  u8g2.clearBuffer();
  u8g2.drawXBMP(0, 0, 128, 64, xbm);
  u8g2.sendBuffer();
}

// --- Primitivas de dibujo ---------------------------------------------------
void dispClear()              { u8g2.clearBuffer(); }
void dispFlush()              { u8g2.sendBuffer(); }
void dispSetColor(uint8_t c)  { u8g2.setDrawColor(c); }
void dispPixel(int x, int y)  { u8g2.drawPixel(x, y); }
void dispLine(int x0, int y0, int x1, int y1) { u8g2.drawLine(x0, y0, x1, y1); }
void dispBox(int x, int y, int w, int h)      { u8g2.drawBox(x, y, w, h); }
void dispRBox(int x, int y, int w, int h, int r) { u8g2.drawRBox(x, y, w, h, r); }
void dispDisc(int x, int y, int r)            { u8g2.drawDisc(x, y, r); }
void dispCircle(int x, int y, int r)          { u8g2.drawCircle(x, y, r); }
void dispEllipseFilled(int x, int y, int rx, int ry) { u8g2.drawFilledEllipse(x, y, rx, ry); }
void dispTriangle(int x0, int y0, int x1, int y1, int x2, int y2) {
  u8g2.drawTriangle(x0, y0, x1, y1, x2, y2);
}
void dispTextSmall(int x, int y, const char* s) {
  u8g2.setFont(u8g2_font_6x10_tr);
  u8g2.drawStr(x, y, s);
}
