// ===========================================================================
//  notice_manager.h  -  Muestra texto arbitrario en la OLED unos segundos.
//  Pensado para mensajes generados por IA/n8n (via MQTT) o desde el panel web.
//  Tiene prioridad sobre emocion/animacion mientras esta activo.
// ===========================================================================
#pragma once
#include <Arduino.h>

void noticeShow(const String& text, uint16_t ms = 5000);
bool noticeActive(uint32_t now);
void noticeRender();
void noticeClear();
