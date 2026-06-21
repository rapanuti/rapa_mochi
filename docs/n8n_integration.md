# Integración con n8n / IA externa (Fase 13)

Rapa Mochi se controla y reporta su estado por **MQTT**. Así, herramientas como **n8n**,
**Home Assistant** o **Node-RED** (y cualquier IA que corra fuera del ESP32) pueden hacerlo
reaccionar y leer su estado.

> Requiere activar MQTT: `MQTT_ENABLED 1` en `config.h` + librería **PubSubClient** +
> configurar `MQTT_SERVER/PORT/USER/PASSWORD/TOPIC_IN/TOPIC_OUT`.

## Topics

| Topic | Dirección | Uso |
|-------|-----------|-----|
| `MQTT_TOPIC_IN` (def. `rapamochi/in`) | n8n → Mochi | **comandos** |
| `MQTT_TOPIC_OUT` (def. `rapamochi/out`) | Mochi → n8n | **estado y eventos** |

## Comandos (publicar en `rapamochi/in`)

| Payload | Efecto |
|---------|--------|
| `happy` (o `sad`, `surprised`, `angry`, `sleepy`, `hot`, `cold`, `thinking`, `notification`, `error`, `idle`, ...) | Muestra esa **emoción** |
| `seq:happy:1000;surprised:600;sad:800` | Reproduce una **secuencia** de emociones |
| `text:Hola, soy una respuesta de IA` | Muestra ese **texto** en la OLED unos segundos |
| cualquier otro texto | Se muestra como **mensaje** en la OLED |

## Estado y eventos (suscribirse a `rapamochi/out`)

- Al conectar publica `online`.
- **Heartbeat** cada 30 s (JSON):
  ```json
  {"uptime":1234,"emotion":"idle","ip":"192.168.1.50","battery":-1}
  ```
  (`battery` es `-1` si `BATTERY_ENABLED 0`.)
- **Eventos** en tiempo real, p. ej. `event:button_1`, `event:wifi_connected`,
  `event:low_battery`, `event:piezo_tap`, etc.

## Ejemplos de flujo n8n

**1) IA responde y el Mochi lo muestra:**
`Webhook/Chat → (modelo de IA) → MQTT Publish` a `rapamochi/in` con
`text:{{ $json.respuesta }}`.

**2) Reaccionar a un botón:**
`MQTT Trigger` en `rapamochi/out` → filtra `event:button_1` → ejecuta tu automatización
(encender una luz, enviar un mensaje, etc.).

**3) Estado de ánimo programado:**
`Schedule (cron)` → `MQTT Publish` a `rapamochi/in` con `sleepy` por la noche y `happy`
por la mañana.

**4) Alerta de batería baja:**
`MQTT Trigger` filtrando `event:low_battery` → notificación a tu teléfono.

## Notas

- El ESP32 **no** ejecuta IA; solo recibe/muestra mensajes y publica su estado. La
  "inteligencia" vive en n8n / tu API / tu modelo.
- Los comandos también se pueden probar desde el **panel web** (campo *Mensaje en pantalla*).
