# rapa_mochi — Dasai Mochi en ESP32-WROOM-32 clásico

Animación de la cara **Dasai Mochi** (90 cuadros de 128×64) en una OLED SSD1306, portada
del proyecto original de [upir](https://github.com/upiir/esp32s3_oled_dasai_mochi) (Seeed
Xiao ESP32-S3) al **ESP32-WROOM-32 clásico / ESP32 Dev Module**.

> Adaptación por **Oswaldo José Anzola ("Jose")** — fork: <https://github.com/rapanuti/rapa_mochi>

---

## Configuración probada en ESP32-WROOM-32

El proyecto fue **probado correctamente** con esta configuración en **Arduino IDE**:

| Ajuste | Valor |
|--------|-------|
| Board | **ESP32 Dev Module** |
| Port | **`/dev/cu.usbserial-10`** |
| Upload Speed | **115200** |

### Hardware usado

| Componente | Detalle |
|------------|---------|
| Placa | ESP32 DevKit / **ESP32-WROOM-32** |
| Chip detectado | **ESP32-D0WD-V3** |
| Pantalla | OLED **SSD1306 128×64** I2C |
| Dirección I2C | **0x3C** |
| SDA | **GPIO21** |
| SCL / SCK | **GPIO22** |
| Rotación correcta | **0** (`U8G2_R0`) |

### Cableado

| OLED | ESP32-WROOM-32 |
|------|----------------|
| VCC / VDD | **3V3** |
| GND | **GND** |
| SDA | **GPIO21** |
| SCL / SCK | **GPIO22** |

---

## Instalación paso a paso (Arduino IDE)

1. **Instalar Arduino IDE.**
   Descárgalo desde <https://www.arduino.cc/en/software> e instálalo (Windows / macOS / Linux).

2. **Instalar el soporte para ESP32.**
   - Abre `Arduino IDE ▸ Preferencias` (en macOS: `Arduino IDE ▸ Settings…`).
   - En **"URLs adicionales de gestor de tarjetas"** pega:
     `https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json`
   - Ve a `Herramientas ▸ Placa ▸ Gestor de tarjetas…`, busca **esp32** e instala
     **"esp32 by Espressif Systems"**.

3. **Instalar la librería U8g2.**
   - Abre `Herramientas ▸ Administrar librerías…` (Gestor de librerías).
   - Busca **U8g2** (de *oliver / olikraus*) e instálala. Es la **única dependencia**.

4. **Seleccionar la placa "ESP32 Dev Module".**
   - `Herramientas ▸ Placa ▸ ESP32 Arduino ▸ ESP32 Dev Module`.

5. **Seleccionar el puerto `/dev/cu.usbserial-10`.**
   - `Herramientas ▸ Puerto ▸ /dev/cu.usbserial-10`.
   - Si no aparece, revisa el cable USB (que sea de datos) y los drivers
     **CP210x / CH340** según el chip USB-serial de tu placa.

6. **Configurar Upload Speed en `115200`.**
   - `Herramientas ▸ Upload Speed ▸ 115200`.

7. **Subir el sketch.**
   - Abre `rapa_mochi.ino` y pulsa **Subir (Upload)** (botón ➜).
   - Al reiniciar, la OLED muestra la cara Mochi animada en bucle.

8. **Si falla la carga, usar el botón BOOT.**
   - Cuando en el monitor aparezca **`Connecting........_____`**, **mantén pulsado el
     botón `BOOT`** de la placa hasta que comience la escritura (`Writing...`).
   - Suéltalo cuando empiece a subir. En algunas placas ayuda pulsar `BOOT`, tocar `EN/RST`
     una vez y soltar `BOOT` al ver `Connecting...`.

---

## Conexión WiFi

> ✅ **Fase 1 ya implementada.** El sketch `rapa_mochi.ino` **ya incluye el WiFi**: al
> arrancar intenta conectarse y muestra en la OLED `WiFi OK` + la IP (o `WiFi FAIL`)
> durante ~2 s, y luego empieza la animación. **Solo tienes que editar tu red.**
>
> 💡 Además, el **LED azul integrado (GPIO2)** parpadea un destello cada **5 s** mientras
> haya WiFi conectado, como indicador visual. Si en tu placa el LED está en otro pin,
> cambia `#define LED_WIFI 2` al inicio del sketch.

Tus credenciales **no van dentro del `.ino`**, sino en un archivo aparte llamado
**`secrets.h`** que **git ignora** (así tu contraseña nunca se sube a GitHub):

1. En la carpeta `rapa_mochi/`, copia `secrets.h.example` y renómbralo a **`secrets.h`**.
2. Edita `secrets.h` con los datos de tu red **(WiFi de 2.4 GHz)**:
   ```cpp
   #define WIFI_SSID      "TU_WIFI"
   #define WIFI_PASSWORD  "TU_PASSWORD"
   ```
3. Compila y sube. Al arrancar verás el estado del WiFi en la OLED durante **~10 s**
   (configurable con `#define WIFI_INFO_MS` en el sketch) y luego la animación.

> 💡 La IP también se imprime en el **Monitor Serie** (115200): ábrelo en Arduino IDE y
> verás `IP: 192.168.x.x` al arrancar, por si no alcanzas a leerla en la pantalla.

El resto de esta sección explica **cómo funciona** y el ejemplo mínimo de referencia.

### Ejemplo mínimo compatible con ESP32

Este sketch independiente solo conecta al WiFi e imprime la IP por el monitor serie (115200):

```cpp
#include <WiFi.h>

const char* ssid     = "TU_WIFI";
const char* password = "TU_PASSWORD";

void conectarWiFi() {
  WiFi.begin(ssid, password);

  Serial.print("Conectando a WiFi");

  int intentos = 0;
  while (WiFi.status() != WL_CONNECTED && intentos < 20) {
    delay(500);
    Serial.print(".");
    intentos++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("");
    Serial.println("WiFi conectado");
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("");
    Serial.println("No se pudo conectar al WiFi");
  }
}

void setup() {
  Serial.begin(115200);
  conectarWiFi();
}

void loop() {}
```

### Integrarlo en el sketch principal sin romper las animaciones

1. **Agregar el include** al inicio del archivo, junto a los demás:
   ```cpp
   #include <WiFi.h>
   ```

2. **Definir las credenciales** (cerca del inicio, antes de `setup()`):
   ```cpp
   const char* ssid     = "TU_WIFI";
   const char* password = "TU_PASSWORD";
   ```

3. **Crear la función `conectarWiFi()`** (la del ejemplo de arriba).

4. **Llamarla dentro de `setup()`**, después de inicializar `Serial` y antes o después de
   iniciar la pantalla:
   ```cpp
   void setup() {
     Serial.begin(115200);
     u8g2.begin();        // inicia la OLED
     conectarWiFi();      // intenta conectar (máx. ~10 s por el límite de 20 intentos)
   }
   ```

5. **No bloquear permanentemente el `loop` de animaciones si el WiFi falla.**
   El bucle de `conectarWiFi()` ya está **acotado a 20 intentos** (~10 s) y sale aunque no
   conecte, de modo que la animación arranca igual. **No** uses un `while (WiFi.status() !=
   WL_CONNECTED) {}` sin límite, porque congelaría el Mochi para siempre.

6. **Mostrar en la OLED un mensaje temporal** con el resultado, antes de empezar la animación:
   ```cpp
   u8g2.clearBuffer();
   u8g2.setFont(u8g2_font_6x10_tr);
   if (WiFi.status() == WL_CONNECTED) {
     u8g2.drawStr(0, 20, "WiFi OK");
     u8g2.drawStr(0, 40, ("IP: " + WiFi.localIP().toString()).c_str());
   } else {
     u8g2.drawStr(0, 20, "WiFi FAIL");
   }
   u8g2.sendBuffer();
   delay(1500);           // muestra el estado ~1.5 s y luego sigue la animacion
   ```
   Mensajes posibles:
   - `WiFi OK` + `IP: xxx.xxx.xxx.xxx`
   - `WiFi FAIL`

---

## Roadmap (crecer el proyecto)

- **Fase 1 — WiFi + mostrar IP en OLED.** Conectar a la red y mostrar la IP en pantalla.
- **Fase 2 — Hora por NTP.** Sincronizar la hora con un servidor NTP y mostrarla.
- **Fase 3 — Expresiones según eventos.** Cambiar la cara/animación del Mochi según eventos.
- **Fase 4 — Mensajes desde n8n.** Recibir mensajes desde **n8n** por **HTTP** o **MQTT**.
- **Fase 5 — Sensor touch / piezo.** Añadir entrada táctil capacitiva o un piezo.
- **Fase 6 — Motor vibrador.** Añadir vibración (motor vía transistor) para feedback háptico.
- **Fase 7 — Batería 18650.** Alimentación con celda 18650 y **carga segura** (TP4056/protección).
- **Fase 8 — Carcasa tipo Mochi.** Diseñar e imprimir una carcasa con forma de Mochi.
