# Arduino IDE — configuración probada

Configuración con la que el proyecto **compila y sube correctamente**.

## Placa y puerto

| Ajuste | Valor |
|--------|-------|
| Board | **ESP32 Dev Module** |
| Partition Scheme | **Minimal SPIFFS (1.9MB APP with OTA / 190KB SPIFFS)** |
| Upload Speed | **115200** |
| Port (Mac) | **`/dev/cu.usbserial-10`** |

> La partición *Minimal SPIFFS* deja margen y conserva la posibilidad de OTA a futuro.
> Si nunca usarás OTA y quieres más espacio: *Huge APP (3MB No OTA)*.

## Soporte ESP32 (una vez)

1. `Arduino IDE ▸ Preferencias ▸ URLs adicionales de gestor de tarjetas`:
   `https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json`
2. `Herramientas ▸ Placa ▸ Gestor de tarjetas…` → instala **esp32 (Espressif Systems)**.

## Librerías

| Librería | Autor | ¿Cuándo? |
|----------|-------|----------|
| **U8g2** | oliver (olikraus) | **Obligatoria** (OLED). Única dependencia del MVP. |
| PubSubClient | Nick O'Leary | Solo si activas `MQTT_ENABLED 1`. |
| ArduinoJson | Benoit Blanchon | Solo en la fase de secuencias por web. |

El portal cautivo usa `WiFi.h`, `WebServer.h`, `DNSServer.h`, `Preferences.h` — **todos del core ESP32**, no se instalan.

## Compilar y subir

1. Copia `rapa_mochi/secrets.h.example` → `rapa_mochi/secrets.h` y pon tu WiFi (queda gitignored).
2. Abre `rapa_mochi/rapa_mochi.ino` (cargará todas las pestañas de módulos).
3. Selecciona Board/Partition/Speed/Port de la tabla de arriba.
4. **Verificar** (compilar) → **Subir**.

## Si falla el upload

- Mantén pulsado **BOOT** en el ESP32 cuando aparezca `Connecting...`; suéltalo al empezar a escribir.
- Confirma **Upload Speed 115200**.
- Cierra el **Monitor Serie** (ocupa el puerto).
- Revisa el **cable USB** (que sea de datos, no solo de carga) y el puerto (`ls /dev/cu.*`).
