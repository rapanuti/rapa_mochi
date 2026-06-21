// ===========================================================================
//  animation_manager.h  -  Reproduce la animacion idle (90 frames) sin bloquear.
// ===========================================================================
#pragma once

void animBegin();        // reinicia el contador de frames
void animRenderNext();   // avanza/dibuja el siguiente frame (gate FRAME_INTERVAL_MS)
