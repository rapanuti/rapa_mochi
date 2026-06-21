// ===========================================================================
//  behavior_manager.h  -  Personalidad autonoma (Fase 12).
//  Cuando el Mochi esta tranquilo (idle), de vez en cuando "siente" algo.
// ===========================================================================
#pragma once
#include <Arduino.h>

void behaviorBegin();
void behaviorUpdate(uint32_t now);
void behaviorSetEnabled(bool on);
bool behaviorEnabled();
