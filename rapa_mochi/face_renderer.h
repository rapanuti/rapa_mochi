// ===========================================================================
//  face_renderer.h  -  Dibuja caras procedurales (U8g2 primitivas) por emocion.
//  No necesita imagenes: ojos/boca/iconos generados con lineas, cajas y circulos.
// ===========================================================================
#pragma once
#include <Arduino.h>
#include "emotion_manager.h"   // enum Emotion

void faceRender(Emotion e, uint32_t now);   // clear + dibuja la cara + flush
