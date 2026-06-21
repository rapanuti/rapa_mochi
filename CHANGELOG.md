# Changelog

Todos los cambios notables de **rapa_mochi** se documentan en este archivo.

El formato sigue [Keep a Changelog](https://keepachangelog.com/es-ES/1.1.0/)
y el proyecto usa [Versionado Semántico](https://semver.org/lang/es/).

## Flujo de trabajo

Cada modificación se desarrolla en una **rama nueva** (`feat/…`, `fix/…`, `chore/…`,
`docs/…`), se prueba en hardware y luego se integra a `main`. Cada entrada de este
changelog corresponde a una o varias de esas ramas ya integradas.

## [No publicado]

### Añadido

- **Bloque D — Integración con n8n / IA externa (Fase 13).**
  - **Mensajes en pantalla** (`notice_manager`): muestra texto arbitrario en la OLED con
    ajuste de línea, con prioridad sobre emoción/animación. Pensado para respuestas
    generadas por IA fuera del ESP32.
  - **Comandos MQTT**: `text:...` (mensaje), `seq:...` (secuencia), nombre de emoción, o
    texto libre → se muestra. Disparables desde n8n / Home Assistant / Node-RED.
  - **Estado y eventos hacia n8n**: `online` al conectar, **heartbeat** JSON cada 30 s
    (uptime/emoción/IP/batería) y publicación de **eventos** (`event:button_1`,
    `event:low_battery`, ...) en `MQTT_TOPIC_OUT`.
  - Panel web: campo **"Mensaje en pantalla"**. Doc: [n8n_integration.md](docs/n8n_integration.md).
- **Bloque C — Hardware opcional: MQTT, vibrador, sonido, batería (Fases 8–11).**
  Cada manager trae **código real completo** detrás de su flag (`MQTT_ENABLED`,
  `VIBRATION_ENABLED`, `SOUND_ENABLED`, `BATTERY_ENABLED`), **desactivado por defecto**:
  el build no requiere librerías extra hasta que actives uno.
  - **MQTT** (`mqtt_manager`, PubSubClient): conecta al broker, se suscribe a
    `MQTT_TOPIC_IN`, interpreta comandos (`happy`, `seq:happy:1000;sad:800`, o texto→
    notificación) y publica estado en `MQTT_TOPIC_OUT`. Reconexión no bloqueante.
  - **Vibrador** (`vibration_manager`): pulsos no bloqueantes (vía MOSFET/transistor).
  - **Sonido** (`sound_manager`): tonos no bloqueantes en buzzer pasivo (ledc, core 3.x).
  - **Batería** (`battery_manager`): mide 18650 por ADC1 con divisor, % con histéresis y
    emite el evento `LOW_BATTERY` (→ cara triste) bajo umbral.
  - Sus `update()` ya se llaman en el loop (no-op mientras estén desactivados).
- **Bloque B — Botones + Eventos + Personalidad (Fases 6, 7 y 12).**
  - **Botones físicos** (`input_manager`, GPIO 25/26/27, `INPUT_PULLUP` + debounce +
    detección de flanco). Activado por defecto; es seguro aunque no haya botones
    conectados (leen HIGH). Cada botón tiene una **acción configurable** desde la web
    (emoción / secuencia / estado), persistida en NVS.
  - **Motor de eventos** (`event_manager`): despacha eventos (botones, wifi, piezo,
    temperatura, batería baja, mqtt) a acciones. Reglas por defecto: wifi_connected→
    wifi_ok, low_battery→sad, piezo_tap→surprised, etc.
  - **Personalidad autónoma** (`behavior_manager`, Fase 12): estando tranquilo, el Mochi
    de vez en cuando "siente" una emoción aleatoria (configurable ON/OFF desde la web).
  - Panel web ampliado: configurar acción de cada botón y activar/desactivar la personalidad.
- **Bloque A — Emociones + Panel web (Fases 4 y 5).**
  - **Sistema de emociones con caras procedurales** (`emotion_manager` + `face_renderer`):
    13 emociones (idle/happy/sad/surprised/angry/sleepy/hot/cold/thinking/notification/
    error/wifi_ok/wifi_fail) dibujadas con primitivas U8g2 (ojos/boca/iconos, con
    parpadeo). El **idle** sigue siendo la animación Mochi de 90 frames. Cada emoción
    tiene duración y prioridad; al expirar vuelve a la emoción por defecto.
  - **Secuencias de emociones** (`sequence_manager`): crear/nombrar/guardar (en NVS) y
    reproducir cadenas tipo `happy:1000;surprised:600;sad:800`.
  - **Panel web local** (`web_config_server`, puerto 80, al conectar): estado del
    dispositivo (IP, heap, uptime, versión, emoción actual), **probar cada emoción**,
    fijar **emoción por defecto**, activar/desactivar el **saludo inicial**, y
    crear/guardar/probar **secuencias**.
  - Primitivas de dibujo añadidas a `display_manager`; KV genérico en `storage_manager`.
- **Arquitectura modular (refactor mayor).** El sketch de un solo archivo se dividió en
  módulos (pestañas de Arduino IDE): `config.h`, `mochi_frames.{h,cpp}` (los 90 frames
  movidos **verbatim**, hash idéntico), `display_manager`, `animation_manager`,
  `led_status`, `storage_manager` (NVS), `wifi_manager` (con **portal cautivo propio**),
  y stubs preparados y desactivados: `emotion_manager`, `event_manager`, `input_manager`,
  `mqtt_manager`, `sound_manager`, `vibration_manager`, `battery_manager`,
  `web_config_server`. El `.ino` quedó como orquestador (135 líneas) con una **máquina de
  estados de arranque** no bloqueante.
- **Saludo inicial** "Hola / Soy Rapa Mochi" al encender, antes de la animación.
- **Portal cautivo WiFi propio** (sin librerías externas: `WebServer` + `DNSServer` +
  `Preferences`): AP **`RapaMochi_Setup`**, lista de redes + SSID manual, guarda en NVS y
  reinicia. Si no hay/falla el WiFi, no bloquea la animación.
- **Credenciales en NVS** (namespace `rapamochi`); `secrets.h` queda solo como *fallback*
  del primer arranque.
- Documentación en `docs/`: `hardware.md`, `arduino_setup.md`, `wifi_portal.md`, `roadmap.md`
  (roadmap completo de 13 fases).
- Soporte preparado (desactivado) para **3 botones** (GPIO 25/26/27) y managers de
  MQTT/sonido/vibración/batería con flags `*_ENABLED`.
- **Fase 1 del roadmap — WiFi + IP en OLED.** `rapa_mochi.ino` ahora se conecta al WiFi
  al arrancar (función `conectarWiFi()` acotada a ~10 s para no congelar la animación) y
  muestra en la OLED `WiFi OK` + la IP, o `WiFi FAIL`, durante ~2 s antes de la animación.
  Variables editables `ssid` / `password` al inicio del sketch.
  - **LED azul integrado (GPIO2)**: parpadea con un destello breve **cada 5 s solo
    cuando hay WiFi conectado**, de forma no bloqueante (con `millis()`, sin frenar la
    animación). Indicador visual de "conectado".
- `CHANGELOG.md` con el historial del proyecto y el flujo de trabajo por ramas.
- `rapa_mochi/README.md`: guía dedicada con la **configuración probada en
  ESP32-WROOM-32** (Board: ESP32 Dev Module, Port: `/dev/cu.usbserial-10`,
  Upload Speed: 115200), instalación paso a paso en Arduino IDE, sección
  **Conexión WiFi** (variables editables + ejemplo mínimo + integración sin romper
  las animaciones) y **roadmap** de 8 fases.

### Seguridad

- Las credenciales WiFi se movieron a un archivo **`secrets.h` ignorado por git**
  (con plantilla `secrets.h.example`), para que la contraseña **nunca se suba** al repo.
  Se añadió `.gitignore` (incluye `secrets.h` y artefactos de compilación).

### Cambiado

- **Modos exclusivos**: secuencia, modo aleatorio, demo y personalidad ahora se excluyen
  (solo uno activo a la vez). Iniciar una secuencia apaga aleatorio/demo, y activar
  aleatorio/demo detiene la secuencia. Esto arregla que el **modo aleatorio no disparara**
  cuando había una secuencia en loop manteniendo una emoción activa.
- La emoción de prueba del panel ahora dura **9 s** (`WEB_TEST_MS`, antes 3 s).
- **Fix de condición de carrera en secuencias**: durante una secuencia la emoción se
  pide **permanente** (sin auto-expiración) y la secuencia controla el tiempo con su
  propio reloj, evitando el doble temporizador que hacía que los tiempos no se
  respetaran. El modo demo usa el mismo principio.
- **Modo aleatorio**: duraciones ahora **5/10/15/20 s** (igual que el constructor de
  secuencias), en vez de 3–8 s.
- **Constructor visual de secuencias**: en vez de escribir, eliges cada emoción de un menú
  y su duración (5/10/15/20 s). Las secuencias ahora se reproducen en **loop**.
- **Sección "Aleatorio"**: el Mochi elige emociones al azar con duración variable (3–8 s),
  en bucle (toggle ON/OFF en el panel).
- **Panel web rediseñado estilo macOS**: tarjetas blancas redondeadas, fuente del sistema,
  acento azul, chips y toggles ON/OFF resaltados. Se añadió `<meta charset="utf-8">` (el
  emoji del título ya se ve bien).
- **Fix:** al guardar la *emoción por defecto* desde la web, ahora se refleja **al
  instante** en la OLED (antes esperaba a la siguiente emoción).
- La **emoción de prueba** del panel dura **3 s** (`WEB_TEST_MS`) y se muestra siempre.
- **20 caras nuevas del catálogo Dasai Mochi** (total 33): smile, laugh, uwu, raspberry,
  sakura, rainbow, dumb_love, love, distracted, sneeze, squint, look_down, shift, bee,
  gtr_rain, neon_tube, **pong** (juego animado), **rotation** (ojos girando), gforce, splat.
  Varias **animadas** (pétalos, lluvia, abeja, pelota de pong, pupilas que giran).
- **Transiciones entre emociones**: un breve **parpadeo** (párpados que cierran/abren)
  suaviza el cambio de una cara a otra.
- **Caras procedurales rediseñadas al estilo oficial Dasai Mochi (Gen 3)**: ojos como
  rectángulos verticales redondeados, ojos `^ ^` (happy), medio cerrados (sleepy), en `X`
  (error), lágrima (sad), gota de sudor (hot), y detalles `z`/`?`/`!`/`WiFi`. Basado en la
  *face gallery* de referencia.
- El estado del WiFi (IP) ahora se muestra en la OLED **~10 s** al arrancar
  (antes 2 s), configurable con `#define WIFI_INFO_MS`. La IP también sale por el
  Monitor Serie (115200).
- **README principal reescrito con identidad propia** (distinto al original de upir):
  bilingüe **ES + EN**, enfocado a **makers principiantes**, con el gancho de
  "**compañero de escritorio Mochi**", portada con GIF, tabla de hardware, estructura
  del repo, inicio rápido, roadmap y una **sección de créditos** aparte (atribución a
  upir + recursos). Se quitaron los links heredados de AliExpress/WOKWI.
- `LICENSE`: se añadió la línea de copyright de la adaptación
  (`Copyright (c) 2026 Oswaldo José Anzola`) conservando la original de upir,
  como exige la licencia MIT.
- El sketch `rapa_mochi/` ahora apunta al **ESP32-WROOM-32 clásico** en lugar de
  Seeed Xiao ESP32-S3 (los pines I2C por defecto SDA=21 / SCL=22 ya son los de la
  WROOM-32; el código no requirió cambios funcionales).
- Carpetas y sketches renombrados:
  - `ARDUINO_xiao_oled_dasai_mochi/` → `rapa_mochi/`
  - `minimal_esp32_wroom32_oled/` → `minimal_rapa_mochi/`
  - Los `.ino` se renombraron para coincidir con sus carpetas (requisito de Arduino).

### Eliminado

- Todas las referencias y enlaces a **YouTube** en los README y en las cabeceras de
  los `.ino` (se conserva la atribución a upir y el enlace al repositorio fuente).

## Créditos

Basado en el proyecto original **"Dasai Mochi"** de
[upir](https://github.com/upiir/esp32s3_oled_dasai_mochi) (2025).
Adaptación a ESP32-WROOM-32 por **Oswaldo José Anzola ("Jose")**.
