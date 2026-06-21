# Panel web local

Cuando el Mochi está **conectado a WiFi**, sirve un panel en `http://<IP>` (la IP que
muestra la OLED al arrancar). Ábrelo desde un dispositivo en la misma red.

## Secciones

1. **Estado** — IP, estado WiFi, heap libre, uptime, versión de firmware y emoción actual.
2. **Probar emoción** — un botón por emoción (happy, sad, surprised, angry, sleepy, hot,
   cold, thinking, notification, error, wifi_ok, wifi_fail). Al pulsar, el Mochi muestra
   esa cara durante su duración y luego vuelve a la emoción por defecto.
3. **Configuración**
   - **Emoción por defecto:** la cara base (por defecto `idle` = animación Mochi). Se guarda en NVS.
   - **Saludo inicial:** ON/OFF del "Hola / Soy Rapa Mochi" al arrancar. Se guarda en NVS.
4. **Secuencias** — cadenas de emociones con tiempos:
   - Formato: `emo:ms;emo:ms;...` — ej. `happy:1000;surprised:600;sad:800`.
   - **Guardar** en uno de los slots (con nombre), **reproducir** un slot guardado, o
     **probar** una secuencia sin guardarla.

## Emociones disponibles (33)

Base: `idle` (animación Mochi de 90 frames) · `happy` · `sad` · `surprised` · `angry` ·
`sleepy` · `hot` · `cold` · `thinking` · `notification` · `error` · `wifi_ok` · `wifi_fail`

Catálogo Dasai (Gen 3): `smile` · `laugh` · `uwu` · `raspberry` · `sakura` · `rainbow` ·
`dumb_love` · `love` · `distracted` · `sneeze` · `squint` · `look_down` · `shift` · `bee` ·
`gtr_rain` · `neon_tube` · `pong` · `rotation` · `gforce` · `splat`

Las caras (excepto `idle`) se dibujan de forma **procedural** con primitivas gráficas
(no usan imágenes). Varias son **animadas** (`sakura`, `bee`, `gtr_rain`, `pong`,
`rotation`, `distracted`…). Al cambiar de emoción hay una **transición tipo parpadeo**.

## Notas

- El panel solo está activo en modo **estación** (conectado a tu WiFi); durante el
  **portal cautivo** (`RapaMochi_Setup`) no está disponible.
- En fases siguientes, las emociones y secuencias también se dispararán desde **botones**,
  **eventos** y **MQTT**.
