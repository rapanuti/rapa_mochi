// ===========================================================================
//  battery_manager.cpp  -  (STUB) Compile-safe. ADC1 seguro con WiFi ON.
// ===========================================================================
#include "config.h"
#include "battery_manager.h"

#if BATTERY_ENABLED
  // TODO fase bateria: analogReadMilliVolts(BATTERY_ADC_PIN) * factor_divisor.
  void batteryBegin() {}
  void batteryUpdate() {}
  int  batteryPercent() { return -1; }
  bool batteryLow() { return false; }
#else
  void batteryBegin() {}
  void batteryUpdate() {}
  int  batteryPercent() { return -1; }
  bool batteryLow() { return false; }
#endif
