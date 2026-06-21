// ===========================================================================
//  web_config_server.h  -  UI web local (Fase 4): estado, prueba de emociones,
//  emocion por defecto, saludo on/off y secuencias. WebServer:80, solo en STA.
//  Se arranca cuando el dispositivo ya esta conectado (nunca a la vez que el portal).
// ===========================================================================
#pragma once

void webBegin();    // registra rutas y arranca el WebServer:80
void webUpdate();   // server.handleClient() (llamar en loop)
void webStop();
