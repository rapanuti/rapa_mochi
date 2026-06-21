// ===========================================================================
//  web_config_server.h  -  (POST-MVP) UI web local de estado/test/config.
//  Esqueleto inerte: NO instancia WebServer:80 todavia para no chocar con el portal.
//  Se implementa cuando el dispositivo ya esta conectado a WiFi (modo STA).
// ===========================================================================
#pragma once

void webBegin();    // arranca el WebServer:80 (futuro)
void webUpdate();   // server.handleClient() (futuro)
void webStop();
