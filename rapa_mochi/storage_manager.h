// ===========================================================================
//  storage_manager.h  -  Persistencia en NVS (Preferences), namespace "rapamochi".
// ===========================================================================
#pragma once
#include <Arduino.h>

void storageBegin();
bool storageLoadWiFi(String& ssid, String& pass);   // false si no hay credenciales
void storageSaveWiFi(const String& ssid, const String& pass);
void storageClearWiFi();

// --- KV generico (ajustes, secuencias, etc.) en el mismo namespace NVS ---
void   storagePutString(const char* key, const String& v);
String storageGetString(const char* key, const String& def);
void   storagePutInt(const char* key, int v);
int    storageGetInt(const char* key, int def);
