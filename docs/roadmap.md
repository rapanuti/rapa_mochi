# Roadmap — Rapa Mochi

De un sketch visual a un companion modular, configurable e interactivo.

## Fase 0 — Estado base ✅
- ESP32-WROOM-32 + OLED SSD1306 funcionando.
- Animación tipo Mochi (90 frames) funcionando.

## Fase 1 — Refactorización ✅ (este entregable)
- Arquitectura modular: display, animación, emociones, eventos, WiFi, almacenamiento, entradas, y stubs (mqtt/sound/vibration/battery/web).
- `.ino` reducido a orquestador (setup/loop + máquina de estados).

## Fase 2 — Saludo + WiFi no bloqueante ✅ (este entregable)
- Saludo inicial "Hola / Soy Rapa Mochi".
- Intento de WiFi guardado sin bloquear la animación.

## Fase 3 — Portal cautivo WiFi ✅ (este entregable)
- AP **RapaMochi_Setup**, página de configuración (lista de redes + manual), guardar en NVS, mostrar IP.

## Fase 4 — Página web local ✅
- Estado (IP, heap, uptime, versión), prueba de emociones, emoción por defecto, saludo on/off, secuencias. Panel en `http://<IP>` (ver [web_panel.md](web_panel.md)).

## Fase 5 — Secuencias de emociones ✅
- Crear/nombrar/guardar secuencias (NVS) y ejecutarlas desde la web (`emo:ms;emo:ms;...`). Pendiente: dispararlas desde botones/eventos (Fases 6–7).

## Fase 6 — Botones ✅
- 3 botones configurables (GPIO 25/26/27) con acción asignable desde la web (emoción/secuencia/estado). Pines sin botón leen HIGH (seguro).

## Fase 7 — Reglas / eventos ✅
- `event_manager` asocia eventos con emociones/secuencias. Reglas por defecto para wifi, piezo, temperatura, batería y MQTT (estas últimas se activarán al integrar su hardware/infra).

## Fase 8 — MQTT ✅ (código; activar con `MQTT_ENABLED 1`)
- Cliente MQTT (PubSubClient): recibe comandos en `MQTT_TOPIC_IN`, publica estado en `MQTT_TOPIC_OUT`. Base para n8n / Home Assistant / Node-RED (ver Fase 13).

## Fase 9 — Vibración ✅ (código; activar con `VIBRATION_ENABLED 1`)
- Pulsos no bloqueantes vía transistor/MOSFET + diodo flyback. Asociado a eventos.

## Fase 10 — Sonido ✅ (código; activar con `SOUND_ENABLED 1`)
- Tonos en buzzer pasivo (ledc). Para audio real: PAM8403 / MAX98357A I2S + parlante.

## Fase 11 — Batería ✅ (código; activar con `BATTERY_ENABLED 1`)
- 18650 con TP4056 protegido + divisor a ADC1; medición, histéresis y evento de batería baja.

## Fase 12 — Inteligencia simulada
- Estados internos, reacciones automáticas, personalidad básica, rutinas por horario.

## Fase 13 — Inteligencia real ✅ (código; activar con `MQTT_ENABLED 1`)
- Integración con n8n / API externa vía MQTT: comandos (emoción/secuencia/texto), mensajes de IA mostrados en la OLED, y publicación de estado/eventos. Ver [n8n_integration.md](n8n_integration.md).

---

### Emociones previstas (placeholder en `emotion_manager`)
`idle, happy, sad, surprised, angry, sleepy, hot, cold, thinking, notification, error, wifi_ok, wifi_fail`

### Eventos previstos (placeholder en `event_manager`)
`boot, wifi_connected, wifi_failed, button_1/2/3_pressed, web_command, mqtt_message, piezo_tap, temperature_hot, temperature_cold, low_battery`
