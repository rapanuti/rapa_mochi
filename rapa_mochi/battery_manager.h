// ===========================================================================
//  battery_manager.h  -  (STUB) Activar con BATTERY_ENABLED 1.
//  18650 + cargador con proteccion (TP4056); medir via divisor en ADC1 (GPIO34).
// ===========================================================================
#pragma once

void batteryBegin();
void batteryUpdate();
int  batteryPercent();   // -1 = desconocido
bool batteryLow();
