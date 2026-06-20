# minimal_esp32_wroom32_oled — Dasai Mochi (FASE 1: solo OLED)

Versión mínima del proyecto **Dasai Mochi** de [upir](https://github.com/upiir/esp32s3_oled_dasai_mochi),
adaptada de la placa original **Seeed Xiao ESP32-S3** a un **ESP32-WROOM-32 / ESP32 Dev Module**.

> Adaptación por **Oswaldo José Anzola ("Jose")** — fork: https://github.com/rapanuti/rapa_mochi

En esta fase **solo se enciende la OLED y se reproduce la animación de la cara Mochi**
(90 cuadros de 128×64). Sin audio, sin touch, sin batería, sin sensores, sin WiFi/BT.

---

## Hardware

| Componente | Detalle |
|------------|---------|
| Placa | ESP32 DevKit / ESP32-WROOM-32 (chip ESP32-D0WD-V3 rev v3.1) |
| Pantalla | OLED SSD1306 **128×64** I2C, dirección **0x3C** |

### Cableado

| OLED | ESP32-WROOM-32 |
|------|----------------|
| VCC / VDD | **3V3** |
| GND | **GND** |
| SDA | **GPIO21** |
| SCL / SCK | **GPIO22** |

---

## Opción A — Arduino IDE (recomendada)

1. **Soporte de placas ESP32** (una sola vez):
   - `Archivo ▸ Preferencias ▸ URLs adicionales de gestor de tarjetas`, añade:
     `https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json`
   - `Herramientas ▸ Placa ▸ Gestor de tarjetas…` → instala **esp32 (Espressif Systems)**.
2. **Librería** (`Herramientas ▸ Administrar librerías…`):
   - Instala **U8g2** de *oliver* (olikraus). Es la única dependencia.
3. Abre el archivo **`minimal_esp32_wroom32_oled.ino`** (esta carpeta).
4. `Herramientas ▸ Placa` → **ESP32 Dev Module**.
5. Selecciona el **Puerto** correcto.
6. Pulsa **Subir (Upload)** ➜ al reiniciar verás el splash `Mochi / ESP32-WROOM-32` y luego la animación.

> Si la animación va muy rápida/lenta, edita `#define ANIM_FPS 24` al inicio del `.ino`.

## Opción B — PlatformIO

```bash
cd minimal_esp32_wroom32_oled
pio run -t upload      # compila + sube
pio device monitor     # 115200 baud
```

---

## Qué deberías ver en la OLED

1. ~1.2 s de splash: **“Mochi / ESP32-WROOM-32”**.
2. Luego, la **cara Mochi animada** en bucle (parpadeos / sonrisa), a pantalla completa.
3. En el **Monitor Serie (115200)**: `[Mochi] Display OK -> mostrando splash 1.2s`.

## Errores típicos y solución

| Síntoma | Causa probable | Solución |
|---------|----------------|----------|
| Pantalla en negro, sin splash | Dirección/cableado I2C | Confirma 0x3C (si tu módulo es 0x3D, cambia `OLED_I2C_ADDR`). Revisa SDA=21 / SCL=22 y 3V3. |
| Compila pero nada en pantalla | Librería equivocada | Asegúrate de usar **U8g2** (no Adafruit_SSD1306 aquí). |
| `A fatal error occurred: Failed to connect` | Modo descarga | Mantén **BOOT** pulsado al iniciar la subida, o baja `upload_speed`. |
| Imagen espejada / al revés | Rotación | Cambia `U8G2_R0` por `U8G2_R2` (180°) en el constructor. |
| Animación entrecortada | Bus I2C lento | Sube `OLED_BUS_HZ` a `400000` (ya por defecto). |

---

## Fases futuras (no incluidas)

Los *stubs* comentados al inicio del `.ino` (`PIN_SPEAKER`, `PIN_VIBRATOR`,
`PIN_TOUCH`, `PIN_BATTERY_ADC`) marcan dónde integrar **parlante/piezo, motor
vibrador, sensor táctil y batería 18650** cuando llegue la FASE 2/3.
