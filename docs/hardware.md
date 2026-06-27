# Hardware

> 🔌 ¿Buscas cómo conectar todo? Mira el **[diagrama de conexiones](wiring.md)**.

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
| Táctil/Botón 1/2/3 | 25 / 26 / 27 | **TTP223** (tocar = ALTO, `INPUT_PULLDOWN`) o botón a GND |
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

## Activar managers opcionales (Bloque C)

Cada uno se activa cambiando su flag en `config.h` (de `0` a `1`). Por defecto están en `0`
y el firmware compila sin librerías extra.

| Manager | Flag | Pin | Hardware / lib necesarios |
|---------|------|-----|---------------------------|
| MQTT | `MQTT_ENABLED` | WiFi | Librería **PubSubClient** + broker. Configura `MQTT_SERVER/PORT/USER/PASSWORD/TOPIC_IN/TOPIC_OUT`. |
| Vibrador | `VIBRATION_ENABLED` | 13 | Motor + **MOSFET/transistor** (NO directo al GPIO) + **diodo flyback** + R en base/gate. |
| Sonido | `SOUND_ENABLED` | 14 | **Buzzer pasivo** directo, o **PAM8403/MAX98357A** + parlante 4–8 Ω + fuente estable. |
| Batería | `BATTERY_ENABLED` | 34 | 18650 + **TP4056 con protección** + **divisor 2×100k** de Vbat a GPIO34. Ajusta `BATTERY_DIVIDER/MIN/MAX`. |

> Tras activar un flag, su `update()` (ya llamado en el loop) empieza a funcionar.
> El evento `LOW_BATTERY` muestra cara triste; los comandos MQTT pueden disparar emociones
> o secuencias (ver [comandos en mqtt_manager](../rapa_mochi/mqtt_manager.cpp)).
