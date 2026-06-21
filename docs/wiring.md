# Diagrama de conexiones / Wiring

Esquemas de cableado del Rapa Mochi sobre **ESP32-WROOM-32 (ESP32 Dev Module)**.

> ⚡ **Para la PRIMERA prueba solo necesitas la OLED.** Todo lo demás (botones, vibrador,
> sonido, batería) es **opcional** y viene desactivado por flags en `config.h`. Puedes
> alimentar el ESP32 por **USB** mientras pruebas.

---

## 0) Mapa de pines usados

```
                      ESP32-WROOM-32 (DevKit)
                     ┌───────────────────────┐
                3V3 ─┤ 3V3               GND ├─ GND
                     │                       │
        OLED SDA ────┤ GPIO21 (SDA)   GPIO22 ├──── OLED SCL
                     │                       │
        Boton 1 ─────┤ GPIO25         GPIO13 ├──── Vibrador (base/gate driver)
        Boton 2 ─────┤ GPIO26         GPIO14 ├──── Buzzer pasivo / sonido
        Boton 3 ─────┤ GPIO27         GPIO34 ├──── Bateria (ADC, divisor)
                     │                       │
        LED azul ────┤ GPIO2 (integrado)     │
                     │             USB / 5V ─┤ alimentacion (prueba)
                     └───────────────────────┘
```

| Señal | Pin | Notas |
|-------|-----|-------|
| OLED SDA | GPIO21 | I2C |
| OLED SCL | GPIO22 | I2C |
| LED estado | GPIO2 | integrado en la placa |
| Botón 1/2/3 | GPIO25/26/27 | `INPUT_PULLUP` |
| Vibrador | GPIO13 | vía transistor/MOSFET |
| Sonido | GPIO14 | buzzer pasivo / amplificador |
| Batería (medir) | GPIO34 | ADC1, sólo entrada |

---

## 1) OLED SSD1306 128×64 I2C  ✅ (necesaria para la primera prueba)

```
   ESP32                         OLED SSD1306 (I2C, dir 0x3C)
   ┌──────┐                      ┌──────────────────┐
   │  3V3 ├──────────────────────┤ VCC              │
   │  GND ├──────────────────────┤ GND              │
   │ G21  ├──────────────────────┤ SDA              │
   │ G22  ├──────────────────────┤ SCL              │
   └──────┘                      └──────────────────┘
```

Eso es todo lo necesario para ver las caras del Mochi. Si tu módulo trae el pin como
**SCK** en vez de SCL, es el mismo (reloj I2C).

---

## 2) Botones (×3)  — opcional (`INPUT_ENABLED 1`, ya activado)

Cada botón conecta su GPIO a **GND**. El pull-up interno mantiene el pin en ALTO; al
pulsar baja a BAJO (LOW = presionado). **No** hace falta resistencia externa.

```
   G25 ──────────[  BOTÓN 1  ]──────────┐
   G26 ──────────[  BOTÓN 2  ]──────────┤
   G27 ──────────[  BOTÓN 3  ]──────────┤
                                        │
                                       GND
```

> Si NO conectas botones, no pasa nada: los pines quedan en ALTO y nunca se "pulsan".

---

## 3) Vibrador  — opcional (`VIBRATION_ENABLED 1`)

⚠️ El motor **nunca** va directo al GPIO. Usa un **transistor NPN** (p. ej. 2N2222) o un
**MOSFET de nivel lógico** (p. ej. 2N7000 / IRLZ44N) y un **diodo flyback** (1N4001) en
paralelo al motor.

```
                         +V  (3V3 o 5V según el motor)
                          │
                     ┌────┴────┐
                     │  MOTOR  │
                     └────┬────┘
                          ├───────────────┐
                       ▲  │ (diodo 1N4001 │  cátodo = raya hacia +V)
                       │  │  en paralelo  │
                          ▼               │
                          │  Colector     │
   G13 ──[ 1kΩ ]──── Base │   NPN 2N2222  │
                          │  Emisor       │
                          └───────────────┴──── GND
```

- **Transistor**: Base ← (1 kΩ) ← GPIO13 · Colector → motor → +V · Emisor → GND.
- **MOSFET**: Gate ← (100 Ω) ← GPIO13 (+ 100 kΩ Gate→GND) · Drain → motor · Source → GND.
- **Diodo flyback** en paralelo al motor, cátodo (raya) hacia +V. Protege contra picos.

---

## 4) Sonido  — opcional (`SOUND_ENABLED 1`)

**Opción simple — buzzer pasivo** directo (poco volumen):

```
   G14 ──────────[ BUZZER PASIVO ]────────── GND
```

**Opción con amplificador** (parlante real 4–8 Ω): usa **PAM8403** o **MAX98357A (I2S)**
con su propia alimentación estable. El GPIO solo no mueve un parlante.

---

## 5) Batería 18650  — opcional (`BATTERY_ENABLED 1`)

⚠️ Nunca cargues litio sin protección. Usa un **TP4056 con protección** (DW01 + FS8205).

```
   18650 (+) ──── B+ ┌──────────┐ OUT+ ───┬────────────► alimentación regulada
                     │  TP4056  │         │   (boost a 5V → VIN, o LDO a 3V3;
   18650 (-) ──── B- │ (protec.)│ OUT- ───┼──┐   NO conectes la celda al 3V3)
                     └──────────┘         │  │
   carga: USB del TP4056                  │ GND (común con ESP32)
                                          │
   medición de carga (divisor 2×100k):    │
        OUT+ ──[100kΩ]──┬──[100kΩ]── GND  │
                        │                 │
                      G34 (ADC) ──────────┘
```

- **Alimentación**: la celda da ~3.7–4.2 V. **No** la conectes directo a `3V3`. Usa un
  **boost a 5 V → VIN/5V** del DevKit, o un **regulador 3.3 V** adecuado.
- **Medición**: divisor **2×100 kΩ** de OUT+ a GND, punto medio a **GPIO34**. El firmware
  usa `BATTERY_DIVIDER 2.0` (ajústalo si usas otras resistencias).
- **GND común** entre batería, regulador y ESP32.

---

## Resumen rápido

| Quieres probar… | Necesitas cablear | Flag en `config.h` |
|-----------------|-------------------|--------------------|
| Las caras / panel web / WiFi | **Solo la OLED** | (nada que tocar) |
| Botones | OLED + botones a GND | `INPUT_ENABLED 1` (ya) |
| Vibrador | + transistor/MOSFET + diodo | `VIBRATION_ENABLED 1` |
| Sonido | + buzzer/amplificador | `SOUND_ENABLED 1` |
| Batería | + TP4056 + divisor | `BATTERY_ENABLED 1` |

Detalles de componentes y seguridad en [`hardware.md`](hardware.md).
