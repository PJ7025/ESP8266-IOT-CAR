#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

// WiFi Credentials - CHANGE THESE TO YOURS
const char* ssid = "7025";
const char* password = "123456789";

ESP8266WebServer server(80);

// --- PIN MAPPING ---

// STEERING MOTOR (connected to OUT1/OUT2 on L298N)
const int ENA = 14; // D5 - Steering motor enable (kept HIGH = full power)
const int IN1 = 12; // D6 - Steering direction
const int IN2 = 13; // D7 - Steering direction

// DRIVE MOTOR (connected to OUT3/OUT4 on L298N)
const int IN3 = 4;  // D2 - Drive direction
const int IN4 = 5;  // D1 - Drive direction
const int ENB = 2;  // D4 - Drive motor speed (PWM)

// HEADLIGHTS
const int LED_PIN = 16; // D0 - Headlight LEDs

// Default drive speed (ESP8266 PWM range is 0 to 1023)
int currentSpeed = 800;

// Headlight state
bool headlightsOn = false;

// HTML & JavaScript Remote Control Site
const char* remote_html = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Toy Car Remote</title>
    <style>
        body { font-family: Arial, sans-serif; text-align: center; background: #222; color: white; padding: 10px; user-select: none; }
        h1 { margin-top: 10px; font-size: 24px; color: #4CAF50; }
        .control-panel { display: flex; flex-direction: column; align-items: center; gap: 15px; margin: 20px auto; width: 300px; }
        .row { display: flex; justify-content: center; gap: 15px; width: 100%; }
        .btn { background: #444; border: 2px solid #555; color: white; padding: 20px; font-size: 18px; font-weight: bold; border-radius: 12px; cursor: pointer; flex: 1; text-align: center; }
        .btn:active { background: #4CAF50; border-color: #4CAF50; }
        .stop { background: #f44336; border-color: #d32f2f; }
        .stop:active { background: #b71c1c; }
        .headlight { background: #555; border-color: #666; }
        .headlight.on { background: #FFD54F; border-color: #FFC107; color: #222; }
        .slider-container { width: 280px; margin: 30px auto; background: #333; padding: 15px; border-radius: 15px; border: 1px solid #444; }
        .slider { width: 100%; height: 20px; border-radius: 10px; background: #555; outline: none; -webkit-appearance: none; }
        .slider::-webkit-slider-thumb { -webkit-appearance: none; width: 30px; height: 30px; border-radius: 50%; background: #4CAF50; cursor: pointer; }
        .footer { margin-top: 30px; font-size: 13px; color: #999; padding-bottom: 10px; }
        .footer a { color: #4CAF50; text-decoration: none; }
        .footer a:active { text-decoration: underline; }
    </style>
</head>
<body>
    <h1>Toy Car Control</h1>
    <div class="control-panel">
        <div class="row">
            <button class="btn" onmousedown="sendCommand('forward')" onmouseup="sendCommand('stopDrive')" ontouchstart="sendCommand('forward')" ontouchend="sendCommand('stopDrive')">▲ FORWARD</button>
        </div>
        <div class="row">
            <button class="btn" onmousedown="sendCommand('left')" onmouseup="sendCommand('stopSteer')" ontouchstart="sendCommand('left')" ontouchend="sendCommand('stopSteer')">◀ LEFT</button>
            <button class="btn stop" onclick="sendCommand('stopAll')">■ OFF</button>
            <button class="btn" onmousedown="sendCommand('right')" onmouseup="sendCommand('stopSteer')" ontouchstart="sendCommand('right')" ontouchend="sendCommand('stopSteer')">RIGHT ▶</button>
        </div>
        <div class="row">
            <button class="btn" onmousedown="sendCommand('backward')" onmouseup="sendCommand('stopDrive')" ontouchstart="sendCommand('backward')" ontouchend="sendCommand('stopDrive')">▼ REVERSE</button>
        </div>
        <div class="row">
            <button class="btn headlight" id="headlightBtn" onclick="toggleHeadlights()">💡 HEADLIGHTS</button>
        </div>
    </div>
    <div class="slider-container">
        <h3>Drive Motor Max Speed</h3>
        <input type="range" min="400" max="1023" value="800" class="slider" id="speedSlider" onchange="updateSpeed(this.value)">
    </div>
    <div class="footer">
        Made by Padmajyothish K | <a href="www.linkedin.com/in/padma-jyothish-k-81a3aa256" target="_blank">LinkedIn</a>
    </div>
    <script>
        let headlightsOn = false;

        function sendCommand(action) {
            fetch('/' + action);
        }
        function updateSpeed(val) {
            fetch('/speed?val=' + val);
        }
        function toggleHeadlights() {
            headlightsOn = !headlightsOn;
            const btn = document.getElementById('headlightBtn');
            if (headlightsOn) {
                fetch('/headlightOn');
                btn.classList.add('on');
            } else {
                fetch('/headlightOff');
                btn.classList.remove('on');
            }
        }
    </script>
</body>
</html>
)rawliteral";

void handleRoot() {
  server.send(200, "text/html", remote_html);
}

// --- Drive Motor Logic (OUT3/OUT4) ---
void driveForward() {
  analogWrite(ENB, currentSpeed);
  digitalWrite(IN3, LOW);   // Anti-clockwise
  digitalWrite(IN4, HIGH);
}

void driveBackward() {
  analogWrite(ENB, currentSpeed);
  digitalWrite(IN3, HIGH);  // Clockwise
  digitalWrite(IN4, LOW);
}

void stopDrive() {
  analogWrite(ENB, 0);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}

// --- Steering Motor Logic (OUT1/OUT2) ---
void steerLeft() {
  digitalWrite(ENA, HIGH);
  digitalWrite(IN1, LOW);   // Anti-clockwise
  digitalWrite(IN2, HIGH);
}

void steerRight() {
  digitalWrite(ENA, HIGH);
  digitalWrite(IN1, HIGH);  // Clockwise
  digitalWrite(IN2, LOW);
}

void stopSteer() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(ENA, LOW);
}

// --- Headlight Logic ---
void headlightOn() {
  digitalWrite(LED_PIN, HIGH);
  headlightsOn = true;
  server.send(200, "text/plain", "OK");
}

void headlightOff() {
  digitalWrite(LED_PIN, LOW);
  headlightsOn = false;
  server.send(200, "text/plain", "OK");
}

// --- Master Kill Switch ---
void stopAll() {
  stopDrive();
  stopSteer();
}

void setup() {
  Serial.begin(115200);

  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENB, OUTPUT);
  pinMode(LED_PIN, OUTPUT);

  stopAll();
  digitalWrite(LED_PIN, LOW); // Headlights off at boot

  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.print("Connected! Remote Dashboard URL: http://");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  server.on("/forward", driveForward);
  server.on("/backward", driveBackward);
  server.on("/left", steerLeft);
  server.on("/right", steerRight);
  server.on("/stopDrive", stopDrive);
  server.on("/stopSteer", stopSteer);
  server.on("/stopAll", stopAll);
  server.on("/headlightOn", headlightOn);
  server.on("/headlightOff", headlightOff);

  server.on("/speed", []() {
    if (server.hasArg("val")) {
      currentSpeed = server.arg("val").toInt();
      Serial.print("Drive limit updated to: ");
      Serial.println(currentSpeed);
    }
    server.send(200, "text/plain", "OK");
  });

  server.begin();
}

void loop() {
  server.handleClient();
}