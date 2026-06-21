// ===========================================================================
//  vibration_manager.h  -  (STUB) Activar con VIBRATION_ENABLED 1.
//  El motor NO va directo al GPIO: usar MOSFET/transistor + diodo flyback.
// ===========================================================================
#pragma once
#include <Arduino.h>

void vibrationBegin();
void vibrationUpdate();
void vibrate(uint16_t ms, uint8_t pct = 100);
