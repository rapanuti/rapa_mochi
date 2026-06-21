// ===========================================================================
//  storage_manager.cpp  -  Wrapper de Preferences (NVS). Abre/cierra por operacion.
// ===========================================================================
#include <Preferences.h>
#include "config.h"
#include "storage_manager.h"

static Preferences prefs;

void storageBegin() {
  // NVS se inicializa solo; nada que hacer aqui por ahora.
}

bool storageLoadWiFi(String& ssid, String& pass) {
  prefs.begin(NVS_NAMESPACE, /*readOnly=*/true);
  ssid = prefs.getString("ssid", "");
  pass = prefs.getString("pass", "");
  prefs.end();
  return ssid.length() > 0;
}

void storageSaveWiFi(const String& ssid, const String& pass) {
  prefs.begin(NVS_NAMESPACE, /*readOnly=*/false);
  prefs.putString("ssid", ssid);
  prefs.putString("pass", pass);
  prefs.end();
}

void storageClearWiFi() {
  prefs.begin(NVS_NAMESPACE, /*readOnly=*/false);
  prefs.remove("ssid");
  prefs.remove("pass");
  prefs.end();
}
