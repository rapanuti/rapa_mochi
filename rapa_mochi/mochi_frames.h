// ===========================================================================
//  mochi_frames.h  -  Declaracion de los 90 frames de la animacion idle.
//  Los BYTES viven en mochi_frames.cpp (un solo .cpp -> sin doble copia en flash).
// ===========================================================================
#pragma once
#include <Arduino.h>

// Tabla de 90 punteros a los bitmaps PROGMEM (128x64). Definida en mochi_frames.cpp.
extern const unsigned char* epd_bitmap_allArray[];

// Dimensiones del frame (la cuenta vive en config.h como ANIM_FRAMES).
static const uint8_t MOCHI_FRAME_W = 128;
static const uint8_t MOCHI_FRAME_H = 64;
