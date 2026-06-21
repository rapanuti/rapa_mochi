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

static String pageStatus() {
  String h = F("<!DOCTYPE html><html><head>"
               "<meta name='viewport' content='width=device-width,initial-scale=1'>"
               "<title>Rapa Mochi</title>"
               "<style>body{font-family:sans-serif;max-width:480px;margin:auto;padding:12px}"
               "a.btn,button{display:inline-block;margin:3px;padding:8px 10px;border:1px solid #888;"
               "border-radius:6px;background:#f4f4f4;text-decoration:none;color:#111;cursor:pointer}"
               "fieldset{margin:12px 0}input,select{padding:6px}</style></head><body>");
  h += "<h2>🐱 Rapa Mochi</h2>";

  // --- Estado ---
  h += "<fieldset><legend><b>Estado</b></legend>";
  h += "IP: <b>" + WiFi.localIP().toString() + "</b><br>";
  h += "WiFi: <b>" + String(WiFi.status() == WL_CONNECTED ? "conectado" : "desconectado") + "</b><br>";
  h += "Heap libre: <b>" + String(ESP.getFreeHeap()) + "</b> bytes<br>";
  h += "Uptime: <b>" + uptimeStr() + "</b><br>";
  h += "Firmware: <b>" FW_VERSION "</b><br>";
  h += "Emocion actual: <b>" + String(emotionName(emotionCurrent())) + "</b>";
  h += "</fieldset>";

  // --- Probar emociones ---
  h += "<fieldset><legend><b>Probar emocion</b></legend>";
  for (uint8_t i = 0; i < (uint8_t)Emotion::COUNT; i++) {
    String n = emotionName((Emotion)i);
    h += "<a class='btn' href='/emo?e=" + n + "'>" + n + "</a>";
  }
  h += "</fieldset>";

  // --- Emocion por defecto + saludo ---
  int greet = storageGetInt("greet", 1);
  h += "<fieldset><legend><b>Configuracion</b></legend>";
  h += "<form action='/setdefault' method='GET'>Emocion por defecto: "
       "<select name='e'>" + defaultEmotionOptions(emotionDefault()) + "</select> "
       "<button type='submit'>Guardar</button></form>";
  h += "<p>Saludo inicial: <b>" + String(greet ? "ON" : "OFF") + "</b> &nbsp;"
       "<a class='btn' href='/greeting?on=1'>ON</a>"
       "<a class='btn' href='/greeting?on=0'>OFF</a></p>";
  h += "</fieldset>";

  // --- Secuencias ---
  h += "<fieldset><legend><b>Secuencias</b></legend>";
  for (int i = 0; i < seqSlots(); i++) {
    String nm = seqName(i);
    h += String(i) + ": <b>" + (nm.length() ? nm : String("(vacio)")) + "</b> ";
    h += "<small>" + seqData(i) + "</small> ";
    if (seqData(i).length()) h += "<a class='btn' href='/seqplay?i=" + String(i) + "'>play</a>";
    h += "<br>";
  }
  h += "<form action='/seqsave' method='GET'>Guardar en slot "
       "<select name='i'>";
  for (int i = 0; i < seqSlots(); i++) h += "<option value='" + String(i) + "'>" + String(i) + "</option>";
  h += "</select><br>nombre <input name='name' placeholder='mi_secuencia'><br>"
       "datos <input name='data' size='30' placeholder='happy:1000;surprised:600;sad:800'><br>"
       "<button type='submit'>Guardar</button></form>";
  h += "<form action='/seqtest' method='GET'>Probar sin guardar: "
       "<input name='data' size='30' placeholder='happy:800;angry:800'>"
       "<button type='submit'>Probar</button></form>";
  h += "</fieldset>";

  // --- Botones + personalidad ---
  h += "<fieldset><legend><b>Botones y personalidad</b></legend>";
  for (int b = 0; b < 3; b++) {
    ActionType a = eventButtonActionType(b);
    int p = eventButtonActionParam(b);
    String cur = String(actionTypeName(a));
    if (a == ActionType::EMOTION)       cur += " (" + String(emotionName((Emotion)p)) + ")";
    else if (a == ActionType::SEQUENCE) cur += " (slot " + String(p) + ")";
    h += "Boton " + String(b + 1) + ": <b>" + cur + "</b><br>";
    h += "<form action='/btnset' method='GET'>"
         "<input type='hidden' name='b' value='" + String(b) + "'>"
         "accion <select name='a'>"
         "<option value='none'>none</option><option value='emotion'>emotion</option>"
         "<option value='sequence'>sequence</option><option value='status'>status</option></select> "
         "emo <select name='emo'>" + allEmotionOptions() + "</select> "
         "seq <select name='seq'>";
    for (int s = 0; s < seqSlots(); s++) h += "<option value='" + String(s) + "'>" + String(s) + "</option>";
    h += "</select> <button type='submit'>Set</button></form>";
  }
  h += "<p>Personalidad autonoma: <b>" + String(behaviorEnabled() ? "ON" : "OFF") + "</b> "
       "<a class='btn' href='/behavior?on=1'>ON</a>"
       "<a class='btn' href='/behavior?on=0'>OFF</a></p>";
  h += "</fieldset>";

  // --- Mensaje en la OLED (texto / IA externa) ---
  h += "<fieldset><legend><b>Mensaje en pantalla</b></legend>";
  h += "<form action='/notice' method='GET'>"
       "<input name='text' size='28' placeholder='Hola, soy Rapa Mochi'> "
       "<button type='submit'>Mostrar</button></form>";
  h += "<small>Tambien por MQTT: publica <code>text:tu mensaje</code> en el topic de entrada.</small>";
  h += "</fieldset>";

  h += "</body></html>";
  return h;
}

static void redirectHome() {
  web.sendHeader("Location", "/", true);
  web.send(303, "text/plain", "");
}

static void handleRoot()    { web.send(200, "text/html", pageStatus()); }

static void handleEmo() {
  if (web.hasArg("e")) {
    seqStop();                                   // corta una secuencia en curso
    emotionRequest(emotionFromName(web.arg("e")));
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

static void handleSeqPlay() {
  if (web.hasArg("i")) seqPlay(web.arg("i").toInt());
  redirectHome();
}

static void handleSeqTest() {
  if (web.hasArg("data")) seqPlayData(web.arg("data"));
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

static void handleNotice() {
  if (web.hasArg("text")) noticeShow(web.arg("text"));
  redirectHome();
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
  web.on("/btnset", handleBtnSet);
  web.on("/behavior", handleBehavior);
  web.on("/notice", handleNotice);
  web.begin();
  started = true;
  Serial.print(F("[WEB] panel en http://")); Serial.println(WiFi.localIP());
}

void webUpdate() { if (started) web.handleClient(); }

void webStop()   { if (started) { web.stop(); started = false; } }
