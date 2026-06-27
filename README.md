# 🐱 rapa_mochi

**Tu compañero de escritorio Mochi, hecho con un ESP32 y una pantalla OLED.**
_Your little Mochi desk companion, built with an ESP32 and an OLED display._

<p align="center">
  <img src="RIVE_big_smile_animation_4x.gif" alt="Animacion Mochi" width="320">
</p>

---

## 🇪🇸 ¿Qué es esto?

**rapa_mochi** es un compañero **Dasai Mochi** que vive en una pantallita OLED. Tiene
una **arquitectura modular** y un montón de funciones: **33 caras** (animación Mochi +
emociones procedurales con transiciones), **portal cautivo WiFi**, un **panel web** para
controlarlo, **botones** configurables, **personalidad autónoma**, e integración con
**MQTT / n8n** para mostrar mensajes de IA. Además trae preparado (desactivado) el código
de **vibrador, sonido y batería 18650**.

Está pensado para **makers principiantes**: si nunca has programado un ESP32, aquí
encuentras todo explicado paso a paso.

## 🇬🇧 What is this?

**rapa_mochi** is a **Dasai Mochi** companion living on a small OLED screen. It has a
**modular architecture** and lots of features: **33 faces** (Mochi animation + procedural
emotions with blink transitions), a **WiFi captive portal**, a **web panel** to control it,
configurable **buttons**, an **autonomous personality**, and **MQTT / n8n** integration to
show AI messages. It also ships ready-but-disabled code for **vibration, sound and an
18650 battery**.

It's aimed at **beginner makers**: if you've never flashed an ESP32 before, everything
here is explained step by step.

---

## ✨ Características / Features

- 🐱 **Animación Mochi** (90 cuadros, 128×64) + **33 caras/emociones** procedurales con
  **transiciones tipo parpadeo** (varias animadas: pong, ojos girando, pétalos, lluvia…).
- 🖥️ **Panel web estilo macOS**: estado del dispositivo, **probar las 33 emociones con
  miniatura** de cada cara, **constructor visual de secuencias** (en bucle), emoción por
  defecto, **modo demo**, botones, mensajes en pantalla y **gestión de WiFi**.
- 📶 **WiFi**: portal cautivo propio (`RapaMochi_Setup`) con **escáner de redes**, y también
  cambiar de red **desde el panel**. Al arrancar, **cara de WiFi + IP** en la OLED.
- 🎬 **Secuencias** de emociones con nombre, guardadas en NVS, reproducibles en **bucle**.
- 🔘 **3 botones** configurables + **personalidad autónoma** (reacciones espontáneas).
- 🔗 **MQTT / n8n / Home Assistant**: comandos y mensajes de IA, publicación de estado/eventos.
- 🔌 **Vibrador, sonido y batería 18650**: código listo, activable por flags.
- 🧩 **Arquitectura modular** (managers) — fácil de extender. 🆓 Software libre (MIT).
- 📖 Documentación **paso a paso** para principiantes. Probado en **ESP32-WROOM-32**.

---

## 🧰 Hardware

| Componente / Component | Detalle / Detail |
|------------------------|------------------|
| Placa / Board | ESP32 DevKit · **ESP32-WROOM-32** (chip ESP32-D0WD-V3) |
| Pantalla / Display | OLED **SSD1306 128×64** I2C |
| Dirección I2C / I2C address | **0x3C** |
| SDA | **GPIO21** |
| SCL / SCK | **GPIO22** |
| Alimentación / Power | **3V3** y **GND** |

---

## 📂 Estructura del repo / Repo layout

| Carpeta / Folder | Qué es / What it is |
|------------------|---------------------|
| [`rapa_mochi/`](rapa_mochi/) | Sketch principal **modular** para ESP32-WROOM-32 (managers + portal cautivo WiFi). |
| [`docs/`](docs/) | [hardware](docs/hardware.md) · [conexiones](docs/wiring.md) · [setup Arduino](docs/arduino_setup.md) · [portal WiFi](docs/wifi_portal.md) · [panel web](docs/web_panel.md) · [n8n / IA](docs/n8n_integration.md) · [roadmap](docs/roadmap.md). |
| [`minimal_rapa_mochi/`](minimal_rapa_mochi/) | Versión mínima de referencia (solo OLED) con control de FPS y splash. |
| `RIVE_*`, `PHOTOPEA_*` | Recursos de la animación (Rive / Photopea). |

### 🧩 Arquitectura modular / Modular architecture

El sketch está dividido en módulos (pestañas en Arduino IDE); el `.ino` es solo el
orquestador (boot + loop):

- **Núcleo:** `display_manager`, `animation_manager`, `face_renderer` (caras procedurales),
  `emotion_manager` (+ transiciones), `sequence_manager`, `notice_manager` (mensajes),
  `led_status`.
- **Conectividad:** `wifi_manager` (+ portal cautivo), `web_config_server`, `mqtt_manager`,
  `storage_manager` (NVS).
- **Interacción:** `input_manager` (botones), `event_manager` (reglas), `behavior_manager`
  (personalidad).
- **Hardware opcional (gated):** `vibration_manager`, `sound_manager`, `battery_manager`.

---

## 🚀 Inicio rápido / Quick start

**ES — La guía detallada paso a paso** (instalar Arduino IDE, soporte ESP32, librería
U8g2, placa, puerto, subir, botón BOOT) está en **[`rapa_mochi/README.md`](rapa_mochi/README.md)**.

Resumen:

1. Instala **Arduino IDE** y el **soporte para ESP32** (Espressif).
2. Instala la librería **U8g2** (olikraus) — única dependencia.
3. Conecta la OLED: **SDA→GPIO21**, **SCL→GPIO22**, **VCC→3V3**, **GND→GND**.
4. Placa **ESP32 Dev Module**, Upload Speed **115200**, selecciona el puerto.
5. Abre `rapa_mochi/rapa_mochi.ino` y pulsa **Subir**. ¡Verás la cara Mochi! 🐱

**EN —** The full step-by-step guide lives in **[`rapa_mochi/README.md`](rapa_mochi/README.md)**.
Short version: install Arduino IDE + ESP32 support + the **U8g2** library, wire the OLED
(SDA→21, SCL→22, 3V3, GND), pick **ESP32 Dev Module** at **115200**, open the sketch and
hit **Upload**.

---

## 🖥️ Panel web / Web panel

Cuando el Mochi está conectado a tu WiFi, sirve un panel en **`http://<IP>`** (la IP sale
en la OLED al arrancar). Desde ahí controlas casi todo:

- **Estado**: IP, WiFi, memoria libre, uptime, versión, emoción actual.
- **Probar emoción**: las 33 caras, cada una con su **miniatura** (vista previa real de la
  OLED). Al pulsar, se queda fija hasta que elijas otra.
- **Configuración**: emoción por defecto y saludo inicial on/off.
- **WiFi**: red actual, **cambiar de red** (SSID + contraseña) u **olvidar red** (abre el portal).
- **Secuencias**: **constructor visual** (eliges emoción + tiempo 5/10/15/20 s) que se guarda
  y reproduce en **bucle**.
- **Botones y personalidad**: acción de cada botón, **personalidad autónoma** y **modo demo**.
- **Mensaje en pantalla**: texto centrado en la OLED (también por MQTT con `text:...`).

Guía completa en [`docs/web_panel.md`](docs/web_panel.md).

---

## 📶 WiFi (portal cautivo)

Al arrancar, el Mochi intenta conectarse a la red guardada; si no hay o falla, muestra
**"Creando red"** y abre un Access Point **`RapaMochi_Setup`** para configurarla desde el
navegador (**escáner de redes** + contraseña). También puedes cambiar de red **desde el
panel web**. Guía completa en [`docs/wifi_portal.md`](docs/wifi_portal.md).

_On boot the Mochi tries the saved network; if none/fails it shows **"Creating network"**
and opens a **`RapaMochi_Setup`** access point (with a **network scanner**). You can also
change networks **from the web panel**. Full guide in [`docs/wifi_portal.md`](docs/wifi_portal.md)._

---

## 🗺️ Estado / Status

El roadmap (Fases 0–13) está **implementado**. Guía completa: [`docs/roadmap.md`](docs/roadmap.md).
_The roadmap (Phases 0–13) is **implemented**. Full guide: [`docs/roadmap.md`](docs/roadmap.md)._

| Área / Area | Estado |
|-------------|--------|
| Arquitectura modular + portal cautivo WiFi | ✅ |
| 33 caras/emociones + transiciones + secuencias | ✅ |
| Panel web (estado, test, config, mensajes) | ✅ |
| Botones + eventos + personalidad autónoma | ✅ |
| MQTT / n8n (comandos, mensajes IA, estado) | ✅ código (`MQTT_ENABLED 1`) |
| Vibrador / sonido / batería 18650 | ✅ código (flags `*_ENABLED`) |
| Carcasa física tipo Mochi | ⏳ pendiente (impresión 3D) |

> Las funciones de hardware traen el **código completo desactivado por flag**; se activan
> en `config.h` cuando cableas el módulo correspondiente (ver [`docs/hardware.md`](docs/hardware.md)).

---

## 📜 Créditos / Credits

Basado en el proyecto original **"Dasai Mochi"** de
**[upir](https://github.com/upiir/esp32s3_oled_dasai_mochi)** (2025), creado para la
placa Seeed Xiao ESP32-S3. _Based on the original **"Dasai Mochi"** project by upir._

**rapa_mochi** es la adaptación al **ESP32-WROOM-32 clásico** y la base para hacerlo
crecer, por **Oswaldo José Anzola ("Jose")** · ✉️ oswaldojanzolag@gmail.com

Herramientas y recursos usados / Tools & resources:

- [U8g2](https://github.com/olikraus/u8g2) — librería para la OLED.
- [image2cpp](https://javl.github.io/image2cpp/) — convertir imágenes a arrays C.
- [Photopea](https://www.photopea.com/) y [Rive](https://rive.app/) — edición y animación.

---

## ⚖️ Licencia / License

**MIT** — software libre. Copyright (c) 2025 upir · Copyright (c) 2026 Oswaldo José Anzola.
Consulta el archivo [`LICENSE`](LICENSE). _See the [`LICENSE`](LICENSE) file._
