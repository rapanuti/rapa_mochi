# Hardware

## Placa y pantalla (probado)

| Componente | Detalle |
|------------|---------|
| Placa | ESP32 DevKit / **ESP32-WROOM-32** (chip **ESP32-D0WD-V3**) |
| Pantalla | OLED **SSD1306 128×64** I2C |
| Dirección I2C | **0x3C** |
| Rotación | **0** (`U8G2_R0`) |

### Cableado OLED ↔ ESP32

| OLED | ESP32 |
|------|-------|
| VCC / VDD | **3V3** |
| GND | **GND** |
| SDA | **GPIO21** |
| SCL / SCK | **GPIO22** |

## Mapa de pines (config.h)

| Función | Pin | Nota |
|---------|-----|------|
| I2C SDA / SCL | 21 / 22 | OLED (no cambiar) |
| LED estado | 2 | LED azul integrado |
| Botón 1 / 2 / 3 | 25 / 26 / 27 | `INPUT_PULLUP`, LOW = presionado (futuro) |
| Vibrador | 13 | vía MOSFET/transistor (futuro) |
| Sonido PWM | 14 | buzzer pasivo / PAM8403 (futuro) |
| Batería ADC | 34 | ADC1, input-only (futuro) |

### Pines a EVITAR en el ESP32-WROOM-32

- **0, 2, 12, 15** — strapping (afectan el arranque).
- **6–11** — conectados a la flash SPI (no usar).
- **34–39** — solo entrada, **sin** pull-up interno (no sirven para `INPUT_PULLUP`).
- **1, 3** — Serial (TX/RX).
- **ADC2 (incluye 25/26/27)** — no usar `analogRead` con WiFi encendido. Como **botones digitales** (`digitalRead`) sí funcionan con WiFi.

## Piezas para fases futuras (con seguridad)

> ⚠️ Nada de litio ni motores conectados directo al GPIO.

- **Vibrador (motor de vape):** driver con **transistor NPN o MOSFET**, **diodo flyback** en paralelo al motor, resistencia en base/gate, alimentación adecuada. Nunca directo al pin.
- **Sonido:** parlante 4–8 Ω con amplificador **MAX98357A (I2S)** o **PAM8403**, fuente estable. El GPIO solo no mueve un parlante real.
- **Batería 18650 / LiPo:** módulo de carga con **protección** (**TP4056** con protección), interruptor físico, medición por **divisor resistivo** a ADC1, y caja segura. No cargar litio sin protección.
- **Sensor/botón táctil o piezo:** entrada táctil capacitiva (`touchRead`) o piezo con acondicionamiento; pendiente de fase.
