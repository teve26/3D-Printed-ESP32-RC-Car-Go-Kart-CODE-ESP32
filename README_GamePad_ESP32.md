<div align="center">

# 🕹️ GamePad Digital — ESP32 Go-Kart RC

### Control remoto vía WiFi con joystick táctil, directo desde el navegador

![ESP32](https://img.shields.io/badge/MCU-ESP32-e7352c?style=for-the-badge&logo=espressif&logoColor=white)
![WiFi Control](https://img.shields.io/badge/Control-WiFi%20AP-00b0ff?style=for-the-badge&logo=wifi&logoColor=white)
![WebSocket](https://img.shields.io/badge/Protocolo-WebSocket-black?style=for-the-badge&logo=socketdotio&logoColor=white)
![Status](https://img.shields.io/badge/Estado-Funcional-brightgreen?style=for-the-badge)

</div>

---

## 🏎️ ¿Qué hace este código?

Convierte una **ESP32** en el cerebro de un Go-Kart RC impreso en 3D, sin necesidad de control físico ni app externa. El ESP32 crea su propio punto de acceso WiFi y sirve una **página web con un joystick táctil**. Cualquier celular que se conecte puede manejar el carrito desde el navegador — sin instalar nada.

Todo el movimiento (aceleración, reversa y dirección) se envía en tiempo real mediante **WebSockets**, logrando una respuesta rápida y fluida, ideal para carreras 1 vs 1 🏁.

<div align="center">

```
   📱 Celular (Joystick táctil)
          │  WebSocket
          ▼
   📡 ESP32 (Access Point "ESP32-Car")
          │
   ┌──────┴──────┐
   ▼             ▼
 🔧 Motor DC   🕹️ Servo SG90
 (tracción)     (dirección)
```

</div>

---

## ⚙️ Cómo funciona

1. El ESP32 arranca en modo **Access Point** (`WiFi.softAP`) — no necesita router ni internet.
2. Te conectas desde el celular a la red que crea el ESP32.
3. Al abrir la IP del ESP32 en el navegador, se carga un **joystick circular** dibujado en `<canvas>`.
4. Cada movimiento del dedo se traduce en coordenadas `x, y` y se envía por WebSocket cada ~30ms.
5. El ESP32 interpreta esas coordenadas:
   - **Eje Y** → velocidad y sentido del motor (adelante/reversa)
   - **Eje X** → ángulo del servo de dirección

---

## 📶 Datos de conexión

| Parámetro | Valor |
|---|---|
| **SSID** | `ESP32-Car` |
| **Contraseña** | `12345678` |
| **Puerto Web** | `80` |
| **Puerto WebSocket** | `81` |

> 💡 Una vez conectado a la red, abre `http://<IP-del-ESP32>` en el navegador (la IP se imprime en el Monitor Serie al iniciar).

---

## 🔌 Mapa de pines

| Pin ESP32 | Función |
|---|---|
| `GPIO 18` | Motor DC — dirección A (avance) |
| `GPIO 19` | Motor DC — dirección B (reversa) |
| `GPIO 21` | Señal PWM del servo SG90 (dirección) |

---

## 🎛️ Parámetros ajustables en el código

| Variable | Descripción | Valor actual |
|---|---|---|
| `SERVO_CENTER` | Ángulo neutro del servo | 90° |
| `SERVO_MIN` / `SERVO_MAX` | Límites de giro para no forzar la dirección | 60° – 120° |
| PWM Motor | Resolución 10 bits @ 8kHz | — |
| PWM Servo | Resolución 16 bits @ 50Hz | — |

---

## 📚 Librerías necesarias

Instala desde el **Arduino Library Manager**:

- `WiFi.h` — incluida en el core de ESP32
- `WebServer.h` — incluida en el core de ESP32
- `WebSocketsServer.h` — librería [WebSockets de Links2004](https://github.com/Links2004/arduinoWebSockets)

---

## 🚀 Cómo flashear

1. Instala el **core de ESP32** en el Arduino IDE (Board Manager).
2. Selecciona tu modelo de placa ESP32.
3. Instala la librería `WebSockets` de Links2004.
4. Sube `GamePad_Digital_ESP32.ino` a tu placa.
5. Abre el Monitor Serie a `115200 baudios` para ver la IP asignada.
6. Conéctate a la red `ESP32-Car` desde tu celular y abre esa IP en el navegador. 🎮

---

## 🏁 Los pilotos

<div align="center">

**#1 Verde** vs **#2 Rojo** — el mismo firmware corriendo en dos chasis, listo para carreras cabeza a cabeza.

</div>

---

## 🔧 Ideas para expandir

- [ ] Control multijugador (varios ESP32 en modo estación conectados a un mismo router)
- [ ] Indicador de batería en la interfaz web
- [ ] Modo carrera con temporizador
- [ ] Telemetría en vivo (velocidad, ángulo de dirección)
- [ ] Control por gamepad físico (Bluetooth)

---

<div align="center">

### 🤖 Hecho por makers, para makers

*Si este proyecto te sirvió, considera dejar una ⭐ en el repositorio.*

</div>
