// ===========================================================================
//  input_manager.h  -  Botones 25/26/27 con debounce y deteccion de flanco.
//  Seguro con INPUT_ENABLED aunque no haya botones (INPUT_PULLUP => HIGH).
// ===========================================================================
#pragma once
#include <Arduino.h>

void inputBegin();
void inputUpdate(uint32_t now);
bool inputPressed(uint8_t idx);       // nivel actual (true = presionado)
bool inputJustPressed(uint8_t idx);   // flanco: true UNA vez al presionar (se consume)
