# WiFi USB HID Control

Remote control your computer's keyboard and mouse over WiFi - a dual-microcontroller USB HID automation tool.

![Web Interface](docs/resource/UI_screenshot.png)

## Overview

Control a computer's keyboard and mouse remotely using Pro Micro (ATmega32U4) as a USB HID device and NodeMCU (ESP8266) for WiFi connectivity. Perfect for automation, testing, accessibility, or education.

## Features

A comprehensive list of features is available in the [FEATURES.md](docs/FEATURES.md) file.

-   Type Text
-   Mouse Control with Live Trackpad
-   Quick Actions
-   Quick Scripts
-   Activity Log
-   Custom Operating Systems
-   WiFi Management
-   File Management
-   Web Authentication
-   HTTPS Support
-   Script Storage
-   Advanced Mouse Jiggler
-   REST API
-   Optional OLED Display

See **[FEATURES.md](docs/FEATURES.md)** for a detailed description of each feature.


## Hardware

**Required:**
- Pro Micro (ATmega32U4) - 5V/16MHz
- NodeMCU (ESP8266)
- 4× jumper wires (male-to-male)
- 1× Micro-USB cable

**Optional:**
- 128x64 I2C OLED display - Connect to D3 (SDA) and D4 (SCL) on NodeMCU
- Breadboard for easier connections

![Connected Hardware with Display](docs/resource/display_connected_pic.jpeg)
*NodeMCU and Pro Micro with optional OLED display*

![Assembled Hardware in a small box](docs/resource/assembled.jpeg)
*NodeMCU and Pro Micro with optional OLED display assembled in a small 3d printed box*

## Software

- Arduino IDE 1.8+
- ESP8266 board support (install via Board Manager)
- Built-in libraries: Keyboard, Mouse, EEPROM
- Optional for OLED: Adafruit GFX, Adafruit SSD1306

## Quick Start

1. **Wire** - Connect 4 wires: VCC→Vin, GND→GND, TX→RX, RX→TX (crossed)
2. **Upload** - Flash `pro-micro/pro-micro.ino` (Leonardo board) and `nodemcu/nodemcu.ino` (NodeMCU 1.0)
3. **Upload web files** - Use LittleFS upload tool for `nodemcu/data/` folder
4. **Connect** - Plug Pro Micro into computer via USB (powers both devices)
5. **Access** - Connect to WiFi "USB-HID-Setup" (password: HID_M4ster), open http://192.168.4.1 or https://192.168.4.1
6. **Login** - Enter web credentials (username: admin, password: WiFi_HID!826)
7. **HTTPS Note** - For HTTPS, accept the browser security warning for the self-signed certificate

See **[SETUP.md](docs/SETUP.md)** for detailed instructions and **[WIRING.md](docs/WIRING.md)** for connection diagrams.

## Usage

- **AP Mode** (default): Device creates WiFi network "USB-HID-Setup"
- **Station Mode**: Configure WiFi via web interface to connect to your network
- **API**: See [API.md](docs/API.md) for REST endpoints and DuckyScript reference

All features work in both AP and Station modes.

## Safety Notice

For authorized use only on your own devices. Not for unauthorized access or malicious purposes.

## License

Provided as-is for educational and personal use.
