# WiFi Controlled RC Car using NodeMCU (ESP8266) & L298N

A DIY WiFi-controlled toy car built using a NodeMCU (ESP8266) and an L298N motor driver. The car is controlled entirely through a built-in web dashboard — connect to the car's WiFi from any phone or laptop browser and drive it in real time. No app installation required.

## Features

- Real-time directional control: forward, reverse, left, right
- Independent control of drive motor and steering motor using a dual H-bridge (L298N)
- Adjustable speed slider for the drive motor using PWM
- Headlight toggle controlled via onboard LEDs
- Fully self-contained web server hosted on the ESP8266 — no internet or cloud services required
- Mobile-friendly touch controls

## Hardware Required

| Component | Quantity |
|---|---|
| NodeMCU (ESP8266) | 1 |
| L298N Motor Driver Module | 1 |
| DC Motors (drive + steering) | 2 |
| LEDs (headlights) | 2 |
| Resistors (220–330 ohm) | 2 |
| Power source (battery pack) | 1 |
| Jumper wires | As needed |
| Chassis / toy car body | 1 |

## Pin Mapping

### NodeMCU to L298N

| NodeMCU Pin | GPIO | L298N Pin | Purpose |
|---|---|---|---|
| D5 | GPIO14 | ENA | Steering motor enable |
| D6 | GPIO12 | IN1 | Steering direction |
| D7 | GPIO13 | IN2 | Steering direction |
| D2 | GPIO4 | IN3 | Drive motor direction |
| D1 | GPIO5 | IN4 | Drive motor direction |
| D4 | GPIO2 | ENB | Drive motor speed (PWM) |

### L298N to Motors

| L298N Output | Connected To |
|---|---|
| OUT1 / OUT2 | Steering motor |
| OUT3 / OUT4 | Drive motor |

### Headlights

| NodeMCU Pin | GPIO | Connected To |
|---|---|---|
| D0 | GPIO16 | Headlight LEDs (x2, with current-limiting resistors) |

## Wiring Notes

- L298N's 5V/VCC and GND connect to the NodeMCU's VIN/5V and GND (or to a shared ground with a separate battery pack if motors require higher current).
- Each headlight LED should have its own resistor (220–330 ohm) in series, with both LED cathodes connected to GND.
- Motor direction (clockwise/anticlockwise) depends on physical wiring polarity. If a motor spins the wrong way after testing, swap the corresponding `HIGH`/`LOW` values in the relevant function in the code — no rewiring needed.

## Software Setup

1. Install the [Arduino IDE](https://www.arduino.cc/en/software).
2. Add ESP8266 board support via Board Manager (`File > Preferences > Additional Board Manager URLs`):
   ```
   http://arduino.esp8266.com/stable/package_esp8266com_index.json
   ```
3. Select your board: `Tools > Board > NodeMCU 1.0 (ESP8266-12E Module)`.
4. Open the `.ino` file from this repository.
5. Update the WiFi credentials in the code:
   ```cpp
   const char* ssid = "YOUR_WIFI_SSID";
   const char* password = "YOUR_WIFI_PASSWORD";
   ```
6. Upload the code to your NodeMCU.

## Usage

1. After uploading, open the Arduino Serial Monitor (115200 baud) to find the IP address assigned to the NodeMCU.
2. Connect your phone or laptop to the same WiFi network as the NodeMCU.
3. Open a browser and navigate to the IP address shown in the Serial Monitor.
4. Use the on-screen controls to drive, steer, adjust speed, and toggle headlights.

## Future Improvements

- Live camera streaming for FPV control
- Obstacle detection using ultrasonic sensors
- Battery level monitoring and display
- Bluetooth fallback control mode

## Author

**Padmajyothish K**

## License

This project is open-source and available for personal and educational use.
