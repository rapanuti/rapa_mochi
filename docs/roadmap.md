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

## Fase 4 — Página web local (post-MVP)
- Estado (IP, heap, uptime, versión), prueba de animaciones, emoción por defecto, configuración básica de secuencias.

## Fase 5 — Secuencias de emociones
- Guardar secuencias con nombre; ejecutarlas desde web, botones o eventos.

## Fase 6 — Botones
- 2–3 botones configurables (GPIO 25/26/27); asignar acciones desde la web.

## Fase 7 — Reglas / eventos
- Asociar eventos con emociones; reglas para temperatura, MQTT, touch/piezo, batería.

## Fase 8 — MQTT
- Integración con **n8n / Home Assistant / Node-RED**; recibir comandos y publicar estado.

## Fase 9 — Vibración
- Motor vibrador vía transistor/MOSFET; asociar vibración a eventos.

## Fase 10 — Sonido
- Parlante con amplificador (MAX98357A I2S / PAM8403); sonidos por emoción.

## Fase 11 — Batería
- 18650 segura (TP4056 con protección), medición y alertas de batería baja.

## Fase 12 — Inteligencia simulada
- Estados internos, reacciones automáticas, personalidad básica, rutinas por horario.

## Fase 13 — Inteligencia real
- Integración con n8n / API externa; mensajes y respuestas generadas por IA fuera del ESP32.

---

### Emociones previstas (placeholder en `emotion_manager`)
`idle, happy, sad, surprised, angry, sleepy, hot, cold, thinking, notification, error, wifi_ok, wifi_fail`

### Eventos previstos (placeholder en `event_manager`)
`boot, wifi_connected, wifi_failed, button_1/2/3_pressed, web_command, mqtt_message, piezo_tap, temperature_hot, temperature_cold, low_battery`
