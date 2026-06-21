// ===========================================================================
//  led_status.h  -  LED azul (GPIO2): destello cada 5 s SOLO si hay WiFi.
// ===========================================================================
#pragma once

void ledInit();
void ledUpdate();   // no bloqueante; llamar en cada loop()
