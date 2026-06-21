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

- `CHANGELOG.md` con el historial del proyecto y el flujo de trabajo por ramas.
- `rapa_mochi/README.md`: guía dedicada con la **configuración probada en
  ESP32-WROOM-32** (Board: ESP32 Dev Module, Port: `/dev/cu.usbserial-10`,
  Upload Speed: 115200), instalación paso a paso en Arduino IDE, sección
  **Conexión WiFi** (variables editables + ejemplo mínimo + integración sin romper
  las animaciones) y **roadmap** de 8 fases.

### Cambiado

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
