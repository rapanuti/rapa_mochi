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
static String htmlPage() {
  String h = F("<!DOCTYPE html><html><head>"
               "<meta name='viewport' content='width=device-width,initial-scale=1'>"
               "<title>Rapa Mochi</title></head>"
               "<body style='font-family:sans-serif;max-width:420px;margin:auto;padding:16px'>"
               "<h2>Rapa Mochi &mdash; Config WiFi</h2>"
               "<form method='POST' action='/save'>"
               "<label>Red WiFi detectada</label><br>"
               "<select name='ssid' style='width:100%;padding:8px'>");
  h += scanCache;
  h += F("</select><br><br>"
         "<label>O escribe el SSID manual</label><br>"
         "<input name='ssid_manual' style='width:100%;padding:8px' placeholder='SSID'><br><br>"
         "<label>Password</label><br>"
         "<input name='pass' type='password' style='width:100%;padding:8px'><br><br>"
         "<button type='submit' style='width:100%;padding:12px'>Guardar y reiniciar</button>"
         "</form></body></html>");
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

static void handleSave() {
  String ssid = server.arg("ssid_manual");
  if (ssid.length() == 0) ssid = server.arg("ssid");   // del <select> si no hay manual
  String pass = server.arg("pass");
  if (ssid.length() > 0) {
    storageSaveWiFi(ssid, pass);
    gotCreds = true;
    server.send(200, "text/html",
                F("<html><body style='font-family:sans-serif'>"
                  "<h3>Guardado. Reiniciando...</h3></body></html>"));
  } else {
    server.send(200, "text/html",
                F("<html><body style='font-family:sans-serif'>"
                  "<h3>SSID vacio. <a href='/'>volver</a></h3></body></html>"));
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
