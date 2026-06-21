// ===========================================================================
//  web_config_server.cpp  -  UI web local (Fase 4) cuando hay WiFi conectado.
//  Estado del dispositivo, prueba de emociones, emocion por defecto y secuencias.
// ===========================================================================
#include <WiFi.h>
#include <WebServer.h>
#include "config.h"
#include "web_config_server.h"
#include "emotion_manager.h"
#include "sequence_manager.h"
#include "storage_manager.h"
#include "event_manager.h"
#include "behavior_manager.h"
#include "notice_manager.h"
#include "face_renderer.h"
#include "display_manager.h"

static WebServer web(80);
static bool      started = false;

// ---- helpers ----
static String uptimeStr() {
  uint32_t s = millis() / 1000UL;
  uint32_t h = s / 3600; s %= 3600;
  uint32_t m = s / 60;   s %= 60;
  char buf[24];
  snprintf(buf, sizeof(buf), "%luh %02lum %02lus", (unsigned long)h, (unsigned long)m, (unsigned long)s);
  return String(buf);
}

// Emociones validas como "por defecto" (estables; las efimeras quedan fuera para no
// dejar una cara rara fija y para no desactivar permanentemente la animacion idle).
static bool isStableDefault(Emotion e) {
  switch (e) {
    case Emotion::IDLE:  case Emotion::HAPPY:    case Emotion::SAD:
    case Emotion::SLEEPY:case Emotion::THINKING: case Emotion::HOT:
    case Emotion::COLD:  case Emotion::ANGRY:    return true;
    default: return false;   // surprised/notification/error/wifi_ok/wifi_fail
  }
}

static String defaultEmotionOptions(Emotion sel) {
  String o;
  for (uint8_t i = 0; i < (uint8_t)Emotion::COUNT; i++) {
    Emotion e = (Emotion)i;
    if (!isStableDefault(e)) continue;
    o += "<option value='" + String(emotionName(e)) + "'";
    if (e == sel) o += " selected";
    o += ">" + String(emotionName(e)) + "</option>";
  }
  return o;
}

static String allEmotionOptions() {
  String o;
  for (uint8_t i = 0; i < (uint8_t)Emotion::COUNT; i++)
    o += "<option value='" + String(emotionName((Emotion)i)) + "'>" + String(emotionName((Emotion)i)) + "</option>";
  return o;
}

// Opciones para el constructor de secuencias (con un "—" = paso vacio).
static String stepEmotionOptions() {
  String o = "<option value=''>—</option>";
  for (uint8_t i = 0; i < (uint8_t)Emotion::COUNT; i++)
    o += "<option value='" + String(emotionName((Emotion)i)) + "'>" + String(emotionName((Emotion)i)) + "</option>";
  return o;
}

static String stepTimeOptions() {
  return "<option value='5000'>5s</option><option value='10000'>10s</option>"
         "<option value='15000'>15s</option><option value='20000'>20s</option>";
}

// Fila de toggle ON/OFF estilo macOS (resalta el estado activo).
static String pill(const char* label, const char* path, bool on) {
  return "<div class='row'><span class='muted'>" + String(label) + "</span><span>"
       + "<a class='chip" + String(on ? " on" : "") + "' href='" + path + "?on=1'>ON</a>"
       + "<a class='chip" + String(!on ? " on" : "") + "' href='" + path + "?on=0'>OFF</a>"
       + "</span></div>";
}

static const char PAGE_CSS[] PROGMEM =
  "<!DOCTYPE html><html lang='es'><head><meta charset='utf-8'>"
  "<meta name='viewport' content='width=device-width,initial-scale=1'><title>Rapa Mochi</title>"
  "<style>"
  "*{box-sizing:border-box}"
  "body{font-family:-apple-system,BlinkMacSystemFont,'Helvetica Neue',Arial,sans-serif;"
  "background:#f5f5f7;color:#1d1d1f;margin:0;padding:22px 14px;-webkit-font-smoothing:antialiased}"
  ".wrap{max-width:540px;margin:0 auto}"
  "h1{font-size:26px;font-weight:600;text-align:center;margin:0 0 18px}"
  ".card{background:#fff;border-radius:16px;padding:16px 18px;margin:14px 0;box-shadow:0 1px 4px rgba(0,0,0,.08)}"
  ".card h2{font-size:12px;font-weight:600;color:#86868b;text-transform:uppercase;letter-spacing:.06em;margin:0 0 12px}"
  ".row{display:flex;justify-content:space-between;align-items:center;font-size:15px;padding:4px 0}"
  ".muted{color:#86868b}"
  ".chip{display:inline-block;margin:4px 5px 0 0;padding:7px 13px;border:1px solid #d2d2d7;border-radius:980px;"
  "background:#fff;color:#1d1d1f;text-decoration:none;font-size:13px;cursor:pointer}"
  ".chip:active{background:#e8e8ed}.chip.on{background:#0071e3;color:#fff;border-color:#0071e3}"
  ".pvchip{display:inline-flex;align-items:center;padding-left:6px}"
  ".pv{width:34px;height:17px;margin-right:7px;image-rendering:pixelated;background:#000;border-radius:3px}"
  "button{font:inherit;background:#0071e3;color:#fff;border:none;border-radius:10px;padding:9px 16px;cursor:pointer;font-size:14px}"
  "button:active{opacity:.85}"
  "input,select{font:inherit;border:1px solid #d2d2d7;border-radius:10px;padding:8px 10px;background:#fff;color:#1d1d1f;margin:3px 0}"
  "input{width:100%}form{margin:8px 0}label,small{color:#86868b;font-size:13px}"
  ".bgrp{padding:10px 0;border-top:1px solid #f0f0f2}code{background:#f0f0f2;padding:1px 5px;border-radius:5px}"
  "</style></head><body><div class='wrap'>";

static String pageStatus() {
  String h = FPSTR(PAGE_CSS);
  h += "<h1>🐱 Rapa Mochi</h1>";

  // --- Estado ---
  h += "<div class='card'><h2>Estado</h2>";
  h += "<div class='row'><span class='muted'>IP</span><b>" + WiFi.localIP().toString() + "</b></div>";
  h += "<div class='row'><span class='muted'>WiFi</span><b>" + String(WiFi.status() == WL_CONNECTED ? "conectado" : "desconectado") + "</b></div>";
  h += "<div class='row'><span class='muted'>Heap libre</span><b>" + String(ESP.getFreeHeap()) + " B</b></div>";
  h += "<div class='row'><span class='muted'>Uptime</span><b>" + uptimeStr() + "</b></div>";
  h += "<div class='row'><span class='muted'>Firmware</span><b>" FW_VERSION "</b></div>";
  h += "<div class='row'><span class='muted'>Emocion actual</span><b>" + String(emotionName(emotionCurrent())) + "</b></div>";
  h += "</div>";

  // --- Probar emociones (con miniatura de cada cara) ---
  h += "<div class='card'><h2>Probar emocion</h2>";
  for (uint8_t i = 0; i < (uint8_t)Emotion::COUNT; i++) {
    String n = emotionName((Emotion)i);
    h += "<a class='chip pvchip' href='/emo?e=" + n + "'>"
         "<img class='pv' src='/face?e=" + n + "' loading='lazy'>" + n + "</a>";
  }
  h += "</div>";

  // --- Configuracion ---
  h += "<div class='card'><h2>Configuracion</h2>";
  h += "<form action='/setdefault' method='GET'><label>Emocion por defecto</label><br>"
       "<select name='e'>" + defaultEmotionOptions(emotionDefault()) + "</select> "
       "<button type='submit'>Guardar</button></form>";
  h += pill("Saludo inicial", "/greeting", storageGetInt("greet", 1));
  h += "</div>";

  // --- Secuencias ---
  h += "<div class='card'><h2>Secuencias</h2>";
  for (int i = 0; i < seqSlots(); i++) {
    String nm = seqName(i);
    h += "<div class='row'><span><b>" + String(i) + "</b> " + (nm.length() ? nm : String("(vacio)"))
       + " <span class='muted'>" + seqData(i) + "</span></span>";
    if (seqData(i).length()) h += "<a class='chip' href='/seqplay?i=" + String(i) + "'>play (loop)</a>";
    h += "</div>";
  }

  // Constructor visual: elige emocion + tiempo por paso (se reproduce en loop).
  h += "<form action='/seqbuild' method='GET'><label>Constructor (elige emocion y tiempo)</label>";
  for (int s = 0; s < 5; s++) {
    h += "<div class='row'><select name='e" + String(s) + "'>" + stepEmotionOptions() + "</select>"
         "<select name='t" + String(s) + "'>" + stepTimeOptions() + "</select></div>";
  }
  h += "<label>Guardar en slot</label> <select name='i'>";
  for (int i = 0; i < seqSlots(); i++) h += "<option value='" + String(i) + "'>" + String(i) + "</option>";
  h += "</select><input name='name' placeholder='nombre de la secuencia'>"
       "<button type='submit'>Guardar y reproducir (loop)</button></form>";

  h += "<form action='/seqtest' method='GET'><label>Avanzado: texto (loop)</label>"
       "<input name='data' placeholder='happy:5000;angry:5000'>"
       "<button type='submit'>Probar</button></form>";
  h += "</div>";

  // --- Botones + personalidad ---
  h += "<div class='card'><h2>Botones y personalidad</h2>";
  for (int b = 0; b < 3; b++) {
    ActionType a = eventButtonActionType(b);
    int p = eventButtonActionParam(b);
    String cur = String(actionTypeName(a));
    if (a == ActionType::EMOTION)       cur += " (" + String(emotionName((Emotion)p)) + ")";
    else if (a == ActionType::SEQUENCE) cur += " (slot " + String(p) + ")";
    h += "<div class='bgrp'><div class='row'><span class='muted'>Boton " + String(b + 1) + "</span><b>" + cur + "</b></div>";
    h += "<form action='/btnset' method='GET'>"
         "<input type='hidden' name='b' value='" + String(b) + "'>"
         "<select name='a'><option value='none'>none</option><option value='emotion'>emotion</option>"
         "<option value='sequence'>sequence</option><option value='status'>status</option></select> "
         "<select name='emo'>" + allEmotionOptions() + "</select> "
         "<select name='seq'>";
    for (int s = 0; s < seqSlots(); s++) h += "<option value='" + String(s) + "'>" + String(s) + "</option>";
    h += "</select> <button type='submit'>Set</button></form></div>";
  }
  h += pill("Personalidad autonoma", "/behavior", behaviorEnabled());
  h += pill("Modo demo (todas las caras)", "/demo", behaviorDemo());
  h += "</div>";

  // --- Aleatorio ---
  h += "<div class='card'><h2>Aleatorio</h2>";
  h += pill("Emociones al azar (duracion variable, loop)", "/random", behaviorRandom());
  h += "<small>El Mochi va eligiendo una emocion al azar (3-8 s cada una) sin parar.</small>";
  h += "</div>";

  // --- Mensaje en pantalla ---
  h += "<div class='card'><h2>Mensaje en pantalla</h2>";
  h += "<form action='/notice' method='GET'>"
       "<input name='text' placeholder='Hola, soy Rapa Mochi'>"
       "<button type='submit'>Mostrar</button></form>";
  h += "<small>Tambien por MQTT: publica <code>text:tu mensaje</code> en el topic de entrada.</small>";
  h += "</div>";

  h += "</div></body></html>";
  return h;
}

static void redirectHome() {
  web.sendHeader("Location", "/", true);
  web.send(303, "text/plain", "");
}

static void handleRoot()    { web.send(200, "text/html", pageStatus()); }

static void handleEmo() {
  if (web.hasArg("e")) {
    // El test toma el control total: apaga modos automaticos y secuencia para que
    // la emocion no la corten, y la muestra (WEB_TEST_MS=0 -> permanente).
    behaviorSetDemo(false);
    behaviorSetRandom(false);
    seqStop();
    emotionRequestFor(emotionFromName(web.arg("e")), WEB_TEST_MS, /*force=*/true);
  }
  redirectHome();
}

static void handleSetDefault() {
  if (web.hasArg("e")) {
    Emotion e = emotionFromName(web.arg("e"));
    if (isStableDefault(e)) {                    // solo emociones estables como base
      emotionSetDefault(e);
      storagePutString("defemo", emotionName(e));
    }
  }
  redirectHome();
}

static void handleGreeting() {
  if (web.hasArg("on")) storagePutInt("greet", web.arg("on").toInt() ? 1 : 0);
  redirectHome();
}

// Antes de reproducir una secuencia, apaga los modos automaticos (solo uno activo).
static void stopAutoModes() { behaviorSetDemo(false); behaviorSetRandom(false); }

static void handleSeqPlay() {
  if (web.hasArg("i")) { stopAutoModes(); seqPlay(web.arg("i").toInt(), /*loop=*/true); }
  redirectHome();
}

static void handleSeqTest() {
  if (web.hasArg("data")) { stopAutoModes(); seqPlayData(web.arg("data"), /*loop=*/true); }
  redirectHome();
}

// Constructor visual: arma "emo:ms;..." con los pasos elegidos y lo guarda + reproduce en loop.
static void handleSeqBuild() {
  String data = "";
  for (int s = 0; s < 5; s++) {
    String e = web.arg(String("e") + s);
    if (e.length() == 0) continue;                 // paso vacio ("—")
    String t = web.arg(String("t") + s);
    if (t.length() == 0) t = "5000";
    if (data.length()) data += ";";
    data += e + ":" + t;
  }
  if (data.length()) {
    int slot  = web.hasArg("i") ? web.arg("i").toInt() : 0;
    String nm = web.arg("name"); if (nm.length() == 0) nm = "secuencia";
    seqSave(slot, nm, data);
    stopAutoModes();
    seqStop();
    seqPlayData(data, /*loop=*/true);
  }
  redirectHome();
}

static void handleSeqSave() {
  if (web.hasArg("i") && web.hasArg("data"))
    seqSave(web.arg("i").toInt(), web.arg("name"), web.arg("data"));
  redirectHome();
}

static void handleBtnSet() {
  if (web.hasArg("b") && web.hasArg("a")) {
    int b = web.arg("b").toInt();
    ActionType a = actionTypeFromName(web.arg("a"));
    int p = 0;
    if (a == ActionType::EMOTION)       p = (int)emotionFromName(web.arg("emo"));
    else if (a == ActionType::SEQUENCE) p = web.arg("seq").toInt();
    eventSetButtonAction(b, a, p);
  }
  redirectHome();
}

static void handleBehavior() {
  if (web.hasArg("on")) behaviorSetEnabled(web.arg("on").toInt() != 0);
  redirectHome();
}

static void handleDemo() {
  if (web.hasArg("on")) { seqStop(); behaviorSetDemo(web.arg("on").toInt() != 0); }
  redirectHome();
}

static void handleRandom() {
  if (web.hasArg("on")) { seqStop(); behaviorSetRandom(web.arg("on").toInt() != 0); }
  redirectHome();
}

static void handleNotice() {
  if (web.hasArg("text")) noticeShow(web.arg("text"));
  redirectHome();
}

// Miniatura de una emocion como BMP 1-bit 128x64 (blanco sobre negro).
// Se dibuja en el buffer de la OLED; como esto corre ANTES del refresco del loop,
// el siguiente render lo sobreescribe y la pantalla real no se altera.
static void handleFace() {
  if (!web.hasArg("e")) { web.send(404, "text/plain", ""); return; }
  Emotion e = emotionFromName(web.arg("e"));
  faceRender(e, millis());                 // dibuja la cara en el buffer (sin flush)
  const uint8_t* buf = dispBuffer();        // 1024 B, layout vertical de U8g2

  static uint8_t bmp[62 + 1024];
  memset(bmp, 0, sizeof(bmp));
  // --- Cabecera BMP (1 bpp, 128x64, bottom-up) ---
  bmp[0] = 'B'; bmp[1] = 'M';
  uint32_t fsize = sizeof(bmp);
  bmp[2] = fsize; bmp[3] = fsize >> 8; bmp[4] = fsize >> 16; bmp[5] = fsize >> 24;
  bmp[10] = 62;                 // offset a los datos
  bmp[14] = 40;                 // tamano DIB header
  bmp[18] = 128;                // ancho
  bmp[22] = 64;                 // alto
  bmp[26] = 1;                  // planos
  bmp[28] = 1;                  // bits por pixel
  bmp[34] = 0x00; bmp[35] = 0x04;  // tamano imagen = 1024
  bmp[46] = 2; bmp[50] = 2;     // colores usados / importantes
  // Paleta: 0 = negro (ya en 0), 1 = blanco
  bmp[58] = 0xFF; bmp[59] = 0xFF; bmp[60] = 0xFF;
  // --- Pixeles: filas de abajo hacia arriba, 16 bytes/fila, MSB = izquierda ---
  for (int row = 0; row < 64; row++) {
    int y = 63 - row;
    uint8_t* dst = &bmp[62 + row * 16];
    for (int x = 0; x < 128; x++) {
      if ((buf[(y >> 3) * 128 + x] >> (y & 7)) & 1) dst[x >> 3] |= (0x80 >> (x & 7));
    }
  }
  web.sendHeader("Cache-Control", "max-age=600");
  web.send_P(200, PSTR("image/bmp"), (PGM_P)bmp, sizeof(bmp));
}

void webBegin() {
  if (started) return;
  web.on("/", handleRoot);
  web.on("/emo", handleEmo);
  web.on("/setdefault", handleSetDefault);
  web.on("/greeting", handleGreeting);
  web.on("/seqplay", handleSeqPlay);
  web.on("/seqtest", handleSeqTest);
  web.on("/seqsave", handleSeqSave);
  web.on("/seqbuild", handleSeqBuild);
  web.on("/btnset", handleBtnSet);
  web.on("/behavior", handleBehavior);
  web.on("/demo", handleDemo);
  web.on("/random", handleRandom);
  web.on("/notice", handleNotice);
  web.on("/face", handleFace);
  web.begin();
  started = true;
  Serial.print(F("[WEB] panel en http://")); Serial.println(WiFi.localIP());
}

void webUpdate() { if (started) web.handleClient(); }

void webStop()   { if (started) { web.stop(); started = false; } }
