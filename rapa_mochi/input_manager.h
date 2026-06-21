// ===========================================================================
//  input_manager.h  -  (STUB) Botones 25/26/27. Activar con INPUT_ENABLED 1.
// ===========================================================================
#pragma once
#include <Arduino.h>

void inputBegin();
void inputUpdate(uint32_t now);
bool inputPressed(uint8_t idx);   // idx 0..2 -> BUTTON_1..BUTTON_3
