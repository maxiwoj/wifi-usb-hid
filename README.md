# WiFi USB HID Control

Remote control your computer's keyboard and mouse over WiFi - a dual-microcontroller USB HID automation tool.

![Web Interface](docs/resource/UI_screenshot.png)

## Overview

Control a computer's keyboard and mouse remotely using Pro Micro (ATmega32U4) as a USB HID device and NodeMCU (ESP8266) for WiFi connectivity. Perfect for automation, testing, accessibility, or education.

## Features

### Type Text
![Type Text Feature](docs/resource/typeText.png)

Type text remotely and send keyboard shortcuts to the target computer.

### Mouse Control with Live Trackpad
![Mouse Control](docs/resource/mouseControl.png)

Control mouse movement with directional buttons or use the live trackpad with adjustable sensitivity (0.5x - 10x) for smooth, intuitive control on desktop and mobile devices. Supports single-click, double-click gestures for highlighting text, and right-click for context menus.

### Quick Actions
Customizable one-click keyboard shortcuts for each operating system. Create, edit, and manage quick action buttons through the dedicated management interface at `/manage-actions.html`.

**Features:**
- Per-OS quick actions (Windows, macOS, Linux, and custom OS)
- DuckyScript command support (e.g., `GUI_R`, `CTRL_C`, `ALT_TAB`)
- Custom button labels and descriptions
- Five color schemes (Primary, Success, Warning, Info, Danger)
- Drag-and-drop reordering with up/down buttons
- Persistent storage across reboots

![Manage Quick Actions](docs/resource/ManageQuickActions.png)

### Quick Scripts
![Quick Scripts](docs/resource/QuickScripts.png)

Create and manage custom DuckyScript automation buttons for each operating system. Access the full script editor at `/manage-scripts.html` to build complex automation workflows.

**Features:**
- OS-specific quick scripts for Windows, macOS, Linux, and custom OS
- Full DuckyScript editor with multi-line support
- Script ID system for updating existing scripts
- Custom button labels and color schemes
- Built-in example scripts for common tasks
- Persistent storage in LittleFS

![Manage Quick Scripts](docs/resource/ManageQuickScripts.png)

### Activity Log
![Activity Log](docs/resource/ActivityLog.png)

Real-time activity logging shows all commands and actions with timestamps.

### Custom Operating Systems
![Manage Operating Systems](docs/resource/ManageOS.png)

Extend beyond Windows, macOS, and Linux by creating custom OS categories with their own quick actions and scripts. Perfect for specialized systems like Android, ChromeOS, Raspberry Pi, or any custom environment.

**Features:**
- Add unlimited custom OS categories
- Each custom OS has dedicated quick actions and scripts
- OS selection persists across page navigation
- Quick access to manage actions for any OS
- Delete custom OS when no longer needed

### WiFi Management
![WiFi Configuration](docs/resource/wifiConfiguration.png)

Built-in WiFi manager with network scanner. Device starts in AP mode (SSID: "USB-HID-Setup") and can be configured to connect to your home/office network. Auto-reconnects on startup with automatic fallback to AP mode if connection fails.

### Additional Features

- **Web Authentication** - HTTP Basic Authentication protects all endpoints (default: admin/WiFi_HID!826)
- **HTTPS Support** - Secure communication with self-signed certificate (HTTP + HTTPS on ports 80 and 443)
- **Script Storage** - Save and load DuckyScripts to/from LittleFS for reuse
- **Advanced Mouse Jiggler** - Configurable automatic mouse movement with three patterns (Simple, Circles, Random), adjustable diameter (1-50px), and delay settings (100-60000ms) to prevent screen lock
- **REST API** - Programmatic control via HTTP/HTTPS endpoints
- **Optional OLED Display** - 128x64 display shows status, IP address, and web credentials

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
