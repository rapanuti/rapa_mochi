// ===========================================================================
//  wifi_manager.cpp  -  STA no bloqueante + portal cautivo propio.
// ===========================================================================
#include <WiFi.h>
#include <WebServer.h>
#include <DNSServer.h>
#include "config.h"
#include "secrets.h"          // WIFI_SSID / WIFI_PASSWORD (fallback 1er arranque)
#include "wifi_manager.h"
#include "storage_manager.h"

static WebServer     server(80);
static DNSServer     dns;
static const byte    DNS_PORT = 53;

static unsigned long tStart       = 0;
static bool          portalActive = false;
static bool          gotCreds     = false;
static String        scanCache;            // <option>s de redes, cacheadas

// ---------------------------------------------------------------------------
//  STA
// ---------------------------------------------------------------------------
void wifiBeginTry() {
  String ssid, pass;
  if (!storageLoadWiFi(ssid, pass)) {       // sin credenciales guardadas:
    ssid = WIFI_SSID;                        // usa el fallback de secrets.h
    pass = WIFI_PASSWORD;
  }
  WiFi.persistent(false);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid.c_str(), pass.c_str());
  tStart = millis();
  Serial.print(F("[WIFI] intentando SSID: ")); Serial.println(ssid);
}

WifiTry wifiPoll() {
  if (WiFi.status() == WL_CONNECTED)          return WT_CONNECTED;
  if (millis() - tStart >= WIFI_TRY_MS)       return WT_TIMEOUT;
  return WT_CONNECTING;
}

bool   wifiIsConnected() { return WiFi.status() == WL_CONNECTED; }
String wifiIP()          { return WiFi.localIP().toString(); }

// ---------------------------------------------------------------------------
//  Portal cautivo
// ---------------------------------------------------------------------------
static const char PORTAL_CSS[] PROGMEM =
  "<!DOCTYPE html><html lang='es'><head><meta charset='utf-8'>"
  "<meta name='viewport' content='width=device-width,initial-scale=1'>"
  "<title>Rapa Mochi - WiFi</title><style>"
  "body{font-family:-apple-system,BlinkMacSystemFont,'Helvetica Neue',Arial,sans-serif;"
  "background:#f5f5f7;color:#1d1d1f;margin:0;padding:22px 14px}"
  ".card{background:#fff;border-radius:16px;padding:18px;max-width:420px;margin:0 auto;"
  "box-shadow:0 1px 4px rgba(0,0,0,.1)}h2{margin:0 0 2px;font-size:22px}"
  ".muted{color:#86868b;font-size:13px;margin:0 0 14px}label{font-size:13px;color:#86868b}"
  "select,input{width:100%;box-sizing:border-box;padding:10px;margin:4px 0 12px;"
  "border:1px solid #d2d2d7;border-radius:10px;font:inherit}"
  "button{width:100%;padding:13px;border:none;border-radius:10px;background:#0071e3;"
  "color:#fff;font:inherit;font-size:15px;cursor:pointer}"
  ".btn2{display:block;text-align:center;background:#e8e8ed;color:#1d1d1f;text-decoration:none;"
  "border-radius:10px;padding:9px;margin:0 0 14px;font-size:14px}"
  "</style></head><body><div class='card'>";

static String htmlPage() {
  String h = FPSTR(PORTAL_CSS);
  h += F("<h2>🐱 Rapa Mochi</h2><p class='muted'>Configura tu red WiFi (2.4 GHz)</p>"
         "<form method='POST' action='/save'>"
         "<label>Redes detectadas</label>"
         "<select name='ssid'>");
  h += scanCache;
  h += F("</select>"
         "<a class='btn2' href='/rescan'>&#8635; Volver a escanear</a>"
         "<label>O escribe el nombre (SSID) manual</label>"
         "<input name='ssid_manual' placeholder='Nombre de la red'>"
         "<label>Contrasena</label>"
         "<input name='pass' type='password' placeholder='Contrasena'>"
         "<button type='submit'>Guardar y conectar</button>"
         "</form></div></body></html>");
  return h;
}

static void doScan() {
  int n = WiFi.scanNetworks();
  scanCache = "";
  for (int i = 0; i < n; i++) {
    scanCache += "<option value='" + WiFi.SSID(i) + "'>" + WiFi.SSID(i)
               + " (" + String(WiFi.RSSI(i)) + " dBm)</option>";
  }
  if (n <= 0) scanCache = F("<option value=''>(no se encontraron redes)</option>");
  WiFi.scanDelete();
}

static void handleRoot() { server.send(200, "text/html", htmlPage()); }

// Volver a escanear redes y recargar el formulario.
static void handleRescan() {
  doScan();
  server.sendHeader("Location", "/", true);
  server.send(303, "text/plain", "");
}

static void handleSave() {
  String ssid = server.arg("ssid_manual");
  if (ssid.length() == 0) ssid = server.arg("ssid");   // del <select> si no hay manual
  String pass = server.arg("pass");
  if (ssid.length() > 0) {
    storageSaveWiFi(ssid, pass);
    storagePutInt("newcfg", 1);                         // marca: red nueva configurada
    gotCreds = true;
    server.send(200, "text/html",
                F("<!DOCTYPE html><html><head><meta charset='utf-8'>"
                  "<meta name='viewport' content='width=device-width,initial-scale=1'>"
                  "<style>body{font-family:-apple-system,sans-serif;background:#f5f5f7;text-align:center;"
                  "padding:40px 16px;color:#1d1d1f}</style></head><body>"
                  "<h2>&#10003; Guardado</h2><p>Conectando a tu red... el Mochi se reinicia.</p>"
                  "</body></html>"));
  } else {
    server.send(200, "text/html",
                F("<html><body style='font-family:sans-serif;text-align:center;padding:30px'>"
                  "<h3>Falta el nombre de la red. <a href='/'>volver</a></h3></body></html>"));
  }
}

// Cualquier otra URL -> redirige al portal (captive portal).
static void handleNotFound() {
  server.sendHeader("Location", String("http://") + WiFi.softAPIP().toString(), true);
  server.send(302, "text/plain", "");
}

void wifiStartPortal() {
  // Detiene el intento STA fallido y su auto-reconexion para que no compita con el
  // softAP / el escaneo (evita que el portal RapaMochi_Setup salga inestable).
  WiFi.disconnect(true);
  WiFi.setAutoReconnect(false);
  WiFi.mode(WIFI_AP_STA);          // STA para escanear, AP para servir el portal
  WiFi.softAP(AP_SSID);            // red abierta "RapaMochi_Setup"
  delay(100);                       // settle del AP (una sola vez, no en el loop)
  doScan();                         // escanear UNA vez y cachear
  dns.start(DNS_PORT, "*", WiFi.softAPIP());
  server.on("/", handleRoot);
  server.on("/save", HTTP_POST, handleSave);
  server.on("/rescan", handleRescan);
  server.onNotFound(handleNotFound);
  server.begin();
  portalActive = true;
  gotCreds     = false;
  Serial.print(F("[WIFI] portal activo en http://")); Serial.println(WiFi.softAPIP());
}

void wifiHandlePortal() {
  if (!portalActive) return;
  dns.processNextRequest();
  server.handleClient();
}

bool   wifiPortalGotCreds() { return gotCreds; }
String wifiPortalApIP()     { return WiFi.softAPIP().toString(); }

// ---------------------------------------------------------------------------
//  Utilidades
// ---------------------------------------------------------------------------
void wifiReboot() {
  delay(300);
  ESP.restart();
}

void wifiClearAndReboot() {
  storageClearWiFi();
  wifiReboot();
}
