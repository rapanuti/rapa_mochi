// ===========================================================================
//  storage_manager.h  -  Persistencia en NVS (Preferences), namespace "rapamochi".
// ===========================================================================
#pragma once
#include <Arduino.h>

void storageBegin();
bool storageLoadWiFi(String& ssid, String& pass);   // false si no hay credenciales
void storageSaveWiFi(const String& ssid, const String& pass);
void storageClearWiFi();
