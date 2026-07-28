#include <WiFi.h>
#include <WebServer.h>
#include <WebSocketsServer.h>

// WIFI
const char* ssid = "ESP32-Car";
const char* password = "12345678";

WebServer server(80);
WebSocketsServer webSocket = WebSocketsServer(81);

// PINES
const int motorAPin = 18;
const int motorBPin = 19;
const int servoPin  = 21;

// SERVO
const int SERVO_CENTER = 90;
const int SERVO_MIN = 60;
const int SERVO_MAX = 120;

// =====================================================
// PWM (CORE 3.x)
// =====================================================
void setupPWM() {
    ledcAttach(motorAPin, 8000, 10);
    ledcAttach(motorBPin, 8000, 10);
    ledcAttach(servoPin, 50, 16);

    ledcWrite(motorAPin, 0);
    ledcWrite(motorBPin, 0);
}

// MOTOR
void motorStop() {
    ledcWrite(motorAPin, 0);
    ledcWrite(motorBPin, 0);
}

void writeMotor(int speed, bool forward) {
    speed = constrain(speed, 0, 1023);

    if (forward) {
        ledcWrite(motorBPin, 0);
        ledcWrite(motorAPin, speed);
    } else {
        ledcWrite(motorAPin, 0);
        ledcWrite(motorBPin, speed);
    }
}

// SERVO
void writeServo(int angle) {
    angle = constrain(angle, SERVO_MIN, SERVO_MAX);
    int pulse = map(angle, 0, 180, 500, 2500);
    int duty  = (pulse * 65535) / 20000;
    ledcWrite(servoPin, duty);
}

// =====================================================
// HTML + JOYSTICK PRO
// =====================================================
String webpage =
"<!DOCTYPE html>"
"<html>"
"<head>"
"<meta name='viewport' content='width=device-width, initial-scale=1.0, user-scalable=no'>"
"<title>ESP32 CAR</title>"
"</head>"
"<body style='margin:0;text-align:center;background:#111;color:white;'>"

"<h2>Joystick</h2>"
"<canvas id='joy' width='300' height='300' style='background:#222;border-radius:50%;touch-action:none;'></canvas>"

"<script>"
"let canvas = document.getElementById('joy');"
"let ctx = canvas.getContext('2d');"

"let center = 150;"
"let posX = center;"
"let posY = center;"
"let active = false;"

"let socket = new WebSocket('ws://' + window.location.hostname + ':81');"

"let lastSend = 0;"

// 🔥 BLOQUEAR SCROLL Y RECARGA
"document.body.style.overflow='hidden';"
"document.body.style.touchAction='none';"
"document.addEventListener('touchmove', function(e){e.preventDefault();}, {passive:false});"

// 🔥 DIBUJO
"function draw(){"
"ctx.clearRect(0,0,300,300);"
"ctx.beginPath();ctx.arc(center,center,140,0,Math.PI*2);ctx.stroke();"
"ctx.beginPath();ctx.arc(posX,posY,30,0,Math.PI*2);ctx.fill();"
"}"

// 🔥 ENVÍO
"function sendData(){"
"let x=(posX-center)/100;"
"let y=(center-posY)/100;"
"socket.send(x.toFixed(2)+'|'+y.toFixed(2));"
"}"

// 🔥 LIMITAR MOVIMIENTO
"function limitStick(x,y){"
"let dx=x-center;"
"let dy=y-center;"
"let dist=Math.sqrt(dx*dx+dy*dy);"
"if(dist>100){dx=dx/dist*100;dy=dy/dist*100;}"
"return {x:center+dx,y:center+dy};"
"}"

// 🔥 ACTUALIZAR
"function update(x,y){"
"let now=Date.now();"
"if(now-lastSend<30)return;"
"lastSend=now;"
"let p=limitStick(x,y);"
"posX=p.x;"
"posY=p.y;"
"draw();"
"sendData();"
"}"

// 🔥 TOUCH EVENTS
"canvas.addEventListener('touchstart', function(){active=true;});"

"canvas.addEventListener('touchmove', function(e){"
"if(!active)return;"
"let rect=canvas.getBoundingClientRect();"
"let t=e.touches[0];"
"update(t.clientX-rect.left, t.clientY-rect.top);"
"});"

"canvas.addEventListener('touchend', function(){"
"active=false;"
"posX=center;"
"posY=center;"
"draw();"
"sendData();"
"});"

"draw();"
"</script>"

"</body>"
"</html>";

// =====================================================
// WEBSOCKET
// =====================================================
void onWebSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {

    if (type == WStype_TEXT) {

        String msg = String((char*)payload);
        int sep = msg.indexOf("|");

        float x = msg.substring(0, sep).toFloat();
        float y = msg.substring(sep + 1).toFloat();

        int speed = abs(y) * 1023;
        bool forward = (y > 0);

        if (abs(y) < 0.1) motorStop();
        else writeMotor(speed, forward);

        int servoAngle = map(x * 100, -100, 100, SERVO_MIN, SERVO_MAX);
        writeServo(servoAngle);
    }
}

// =====================================================
// SETUP
// =====================================================
void setup() {
    Serial.begin(115200);

    setupPWM();
    writeServo(SERVO_CENTER);
    motorStop();

    WiFi.softAP(ssid, password);

    server.on("/", []() {
        server.send(200, "text/html", webpage);
    });

    server.begin();

    webSocket.begin();
    webSocket.onEvent(onWebSocketEvent);

    Serial.println("Listo!");
    Serial.println(WiFi.softAPIP());
}

// =====================================================
// LOOP
// =====================================================
void loop() {
    server.handleClient();
    webSocket.loop();
}