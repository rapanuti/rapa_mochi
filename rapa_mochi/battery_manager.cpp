// ===========================================================================
//  battery_manager.cpp  -  Medida de bateria 18650 por ADC1. Compile-safe.
//  HARDWARE: 18650 + cargador con PROTECCION (TP4056 con proteccion), interruptor
//  fisico, y divisor resistivo (2x100k) de Vbat a BATTERY_ADC_PIN (GPIO34, ADC1).
//  Nunca cargar litio sin proteccion. ADC1 es seguro con WiFi encendido.
// ===========================================================================
#include "config.h"
#include "battery_manager.h"

#if BATTERY_ENABLED
#include "event_manager.h"               // para emitir LOW_BATTERY (solo si activo)

static uint32_t lastCheck = 0;
static int      pct       = -1;
static bool     lowSent   = false;   // ya se emitio el evento LOW_BATTERY
static bool     low       = false;   // estado "bateria baja" con histeresis

static int readPct() {
  uint32_t mv = (uint32_t)(analogReadMilliVolts(BATTERY_ADC_PIN) * BATTERY_DIVIDER);
  long p = ((long)mv - BATTERY_MIN_MV) * 100 / (BATTERY_MAX_MV - BATTERY_MIN_MV);
  if (p < 0)   p = 0;
  if (p > 100) p = 100;
  return (int)p;
}

void batteryBegin() {
  analogReadResolution(12);
  pct = readPct();                 // lectura inicial inmediata
}

void batteryUpdate() {
  uint32_t now = millis();
  if (now - lastCheck < 10000) return;   // cada 10 s
  lastCheck = now;
  pct = readPct();
  // Histeresis comun para el estado y el evento: baja a <=LOW_PCT, sube a >LOW_PCT+5.
  if (pct <= BATTERY_LOW_PCT) {
    low = true;
    if (!lowSent) { lowSent = true; eventPost(EventType::LOW_BATTERY); }
  } else if (pct > BATTERY_LOW_PCT + 5) {
    low = false;
    lowSent = false;
  }
}

int  batteryPercent() { return pct; }
bool batteryLow()     { return low; }

#else  // ---- desactivado: no-op ----
void batteryBegin() {}
void batteryUpdate() {}
int  batteryPercent() { return -1; }
bool batteryLow() { return false; }
#endif
