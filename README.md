# 🐱 rapa_mochi

**Tu compañero de escritorio Mochi, hecho con un ESP32 y una pantalla OLED.**
_Your little Mochi desk companion, built with an ESP32 and an OLED display._

<p align="center">
  <img src="RIVE_big_smile_animation_4x.gif" alt="Animacion Mochi" width="320">
</p>

---

## 🇪🇸 ¿Qué es esto?

**rapa_mochi** es una carita **Dasai Mochi** que vive en una pantallita OLED y te
acompaña en el escritorio. Por ahora sonríe y parpadea en bucle, pero está pensado
para **ir creciendo**: conectarse al WiFi, saber la hora, reaccionar a eventos y, más
adelante, sentir tu tacto, vibrar y tener su propia carcasa.

Está pensado para **makers principiantes**: si nunca has programado un ESP32, aquí
encuentras todo explicado paso a paso.

## 🇬🇧 What is this?

**rapa_mochi** is a **Dasai Mochi** face living on a small OLED screen — a tiny desk
companion. Right now it smiles and blinks in a loop, but it's designed to **grow**:
connect to WiFi, tell the time, react to events and — later — sense your touch,
vibrate and get its own enclosure.

It's aimed at **beginner makers**: if you've never flashed an ESP32 before, everything
here is explained step by step.

---

## ✨ Características / Features

- 🐱 Animación de la cara Mochi (90 cuadros, 128×64) en OLED SSD1306.
- 🔌 Pensado para el **ESP32-WROOM-32 clásico** (ESP32 Dev Module).
- 📶 Listo para crecer: guía de **WiFi**, hora por NTP y más (ver Roadmap).
- 📖 Documentación **paso a paso** para principiantes.
- 🆓 Software libre (MIT) — úsalo, modifícalo y comparte.

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
| [`rapa_mochi/`](rapa_mochi/) | Sketch principal para ESP32-WROOM-32 + **guía completa** (config probada, WiFi, roadmap). |
| [`minimal_rapa_mochi/`](minimal_rapa_mochi/) | Versión mínima del port (FASE 1: solo OLED) con control de FPS y splash. |
| `RIVE_*`, `PHOTOPEA_*` | Recursos de la animación (Rive / Photopea). |

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

## 📶 WiFi

Para conectar el Mochi a tu red WiFi (variables editables, ejemplo mínimo e integración
sin romper las animaciones), mira la sección **Conexión WiFi** en
[`rapa_mochi/README.md`](rapa_mochi/README.md#conexión-wifi).

_To connect the Mochi to your WiFi (editable variables + minimal example), see the
**Conexión WiFi** section in [`rapa_mochi/README.md`](rapa_mochi/README.md)._

---

## 🗺️ Roadmap

El plan para que el Mochi pase de "carita animada" a compañero conectado:
_The plan to grow the Mochi from "animated face" to a connected companion:_

| Fase / Phase | Objetivo / Goal |
|--------------|-----------------|
| **1** | 📶 WiFi + mostrar la **IP** en la OLED · _WiFi + show IP on the OLED_ |
| **2** | 🕐 Obtener la **hora por NTP** · _Get time over NTP_ |
| **3** | 😀 **Expresiones** según eventos · _Change expressions based on events_ |
| **4** | ✋ Sensor **touch / piezo** · _Touch / piezo input_ |
| **5** | 📳 **Motor vibrador** · _Vibration motor_ |
| **6** | 🔋 **Batería 18650** con carga segura · _18650 battery with safe charging_ |
| **7** | 🧊 **Carcasa tipo Mochi** · _Mochi-shaped enclosure_ |

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
