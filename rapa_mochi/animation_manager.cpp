// ===========================================================================
//  animation_manager.cpp  -  Misma logica de animacion del sketch original,
//  ahora encapsulada. Con FRAME_INTERVAL_MS=0 corre a cadencia libre (igual que hoy).
// ===========================================================================
#include <Arduino.h>
#include "config.h"
#include "animation_manager.h"
#include "display_manager.h"
#include "mochi_frames.h"

static int           frame       = 0;
static unsigned long lastFrameMs = 0;

void animBegin() {
  frame = 0;
  lastFrameMs = 0;
}

void animRenderNext() {
#if FRAME_INTERVAL_MS > 0
  unsigned long now = millis();
  if (now - lastFrameMs < (unsigned long)FRAME_INTERVAL_MS) return;
  lastFrameMs = now;
#endif
  dispFrame(epd_bitmap_allArray[frame]);   // clear + drawXBMP(0,0,128,64) + flush
  frame++;
  if (frame >= ANIM_FRAMES) frame = 0;
}
