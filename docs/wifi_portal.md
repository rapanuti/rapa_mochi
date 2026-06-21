# Portal cautivo WiFi

Rapa Mochi configura su WiFi con un **portal cautivo propio** (sin librerías externas:
`WebServer` + `DNSServer` + `Preferences`, todo del core ESP32).

## Cómo funciona al arrancar

1. Muestra el saludo **"Hola / Soy Rapa Mochi"** (~2.5 s).
2. Intenta conectarse a la red **guardada en NVS** (o, en el primer arranque, a la de `secrets.h`).
3. Si **no hay credenciales** o **falla** en ~10 s → abre un Access Point.
4. Si **conecta** → muestra **"WiFi OK" + IP** (~10 s) y pasa a la animación.

El intento de WiFi **no bloquea**: si no hay red, tras el timeout el Mochi sigue (portal o animación).

## Configurar el WiFi desde el portal

1. En el móvil/PC, conéctate a la red WiFi abierta **`RapaMochi_Setup`**.
2. Debería abrirse solo el portal; si no, abre el navegador en **`http://192.168.4.1`**.
3. Elige tu red de la **lista** (o escribe el SSID manual), pon la **contraseña** y pulsa **Guardar**.
4. El Mochi muestra **"Guardado / Reiniciando…"**, se reinicia y conecta solo.

> El ESP32-WROOM-32 solo ve redes **2.4 GHz** (no 5 GHz).

## Resetear credenciales / volver a modo configuración

Opciones para borrar la red guardada y reabrir el portal:

- **Re-flashear** con *Tools ▸ Erase All Flash Before Sketch Upload = Enabled* (borra NVS).
- Llamar `wifiClearAndReboot()` (borra credenciales y reinicia) — se conectará a un botón/endpoint en fases siguientes.
- **Plan futuro:** mantener **BUTTON_3 pulsado al arranque** → `wifiClearAndReboot()` → reabre el portal.

## Dónde se guardan las credenciales

En **NVS (Preferences)**, namespace `"rapamochi"`, claves `ssid` / `pass`.
**No** se escriben en el código ni se suben a git. `secrets.h` (gitignored) solo es el *fallback* del primer arranque.
