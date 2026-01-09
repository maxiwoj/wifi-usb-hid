# API Documentation

REST API reference and command protocol for WiFi USB HID Control.

## Authentication

All API endpoints and web pages require HTTP Basic Authentication.

**Default Credentials:**
- Username: `admin`
- Password: `HID_Admin2024!`

You can change these credentials in `nodemcu/config.h` (WEB_AUTH_USER and WEB_AUTH_PASS).

## REST API Endpoints

Base URL: `http://<device-ip>` or `http://192.168.4.1` (AP mode)

### Web Interface

- **GET /** - Main control interface (HTML)
- **GET /setup** - WiFi configuration page (HTML)

---

### POST /api/command

Send a command to Pro Micro. Parameter: `cmd` (required)

```bash
curl -u admin:HID_Admin2024! -X POST http://192.168.1.100/api/command -d "cmd=TYPE:Hello"
curl -u admin:HID_Admin2024! -X POST http://192.168.1.100/api/command -d "cmd=ENTER"
curl -u admin:HID_Admin2024! -X POST http://192.168.1.100/api/command -d "cmd=GUI_R"
```

Response: `{"status": "ok", "message": "Command sent"}`

---

### POST /api/script

Execute DuckyScript. Parameter: `script` (required)

```bash
curl -u admin:HID_Admin2024! -X POST http://192.168.1.100/api/script -d "script=GUI r
DELAY 500
STRING notepad
ENTER"
```

---

### GET /api/jiggler

Control mouse jiggler. Parameter: `enable` (1 or 0)

```bash
curl -u admin:HID_Admin2024! http://192.168.1.100/api/jiggler?enable=1  # Enable
curl -u admin:HID_Admin2024! http://192.168.1.100/api/jiggler?enable=0  # Disable
```

---

### GET /api/status

Get device status (WiFi mode, SSID, IP)

```bash
curl -u admin:HID_Admin2024! http://192.168.1.100/api/status
```

---

### GET /api/wifi

Get current WiFi settings

```bash
curl -u admin:HID_Admin2024! http://192.168.1.100/api/wifi
```

---

### POST /api/wifi

Save WiFi credentials and restart. Parameters: `ssid`, `password`

```bash
curl -u admin:HID_Admin2024! -X POST http://192.168.4.1/api/wifi -d "ssid=MyNetwork" -d "password=pass123"
```

---

### GET /api/scan

Scan for WiFi networks. Returns array of `{ssid, rssi, encryption}`

```bash
curl -u admin:HID_Admin2024! http://192.168.1.100/api/scan
```

## Command Protocol

Commands sent via `/api/command` endpoint or DuckyScript.

### Keyboard

- **Type:** `TYPE:text` - Type without Enter
- **Type + Enter:** `TYPELN:text`
- **Special Keys:** `ENTER`, `ESC`, `TAB`, `BACKSPACE`, `DELETE`
- **Arrow Keys:** `UP`, `DOWN`, `LEFT`, `RIGHT`
- **Function Keys:** `F1` through `F12`
- **GUI Combos:** `GUI_R` (Run), `GUI_D` (Desktop), `GUI_SPACE` (Spotlight)
- **Ctrl Combos:** `CTRL_c`, `CTRL_v`, `CTRL_s`, etc.
- **Alt Combos:** `ALT_TAB`, `ALT_F4`, etc.
- **Special:** `CTRL_ALT_DEL`, `CTRL_ALT_T`

### Mouse

- **Move:** `MOUSE_MOVE:x,y` - Relative movement in pixels
- **Clicks:** `MOUSE_LEFT`, `MOUSE_RIGHT`, `MOUSE_MIDDLE`, `MOUSE_DOUBLE`
- **Scroll:** `SCROLL:amount` - Positive=down, negative=up

### Utility

- **Delay:** `DELAY:ms` - Pause execution (max 10000ms)
- **Ping:** `PING` - Test communication (responds with PONG)
- **Status:** `STATUS` - Get jiggler status
- **LED:** `LED_ON`, `LED_OFF`
- **Jiggler:** `JIGGLE_ON`, `JIGGLE_OFF`

## DuckyScript Reference

Simple scripting language for USB HID automation. Each command on its own line. Comments start with `//`.

### Commands

- **STRING** `text` - Type text
- **DELAY** `ms` - Pause execution
- **ENTER**, **ESC**, **TAB**, **BACKSPACE**, **DELETE** - Special keys
- **GUI** `[key]` - Windows/Command key (e.g., `GUI r`, `GUI SPACE`)
- **ALT** `key` - Alt combinations (e.g., `ALT TAB`, `ALT F4`)
- **CTRL** `key` - Ctrl combinations (e.g., `CTRL c`, `CTRL v`)
- **CTRL ALT** `key` - Ctrl+Alt combinations (e.g., `CTRL ALT DELETE`)
- **UP**, **DOWN**, **LEFT**, **RIGHT** - Arrow keys
- **F1** through **F12** - Function keys

### Example Scripts

**Open Notepad (Windows):**
```
GUI r
DELAY 500
STRING notepad
ENTER
DELAY 1000
STRING Hello World!
```

**Open TextEdit (macOS):**
```
GUI SPACE
DELAY 700
STRING TextEdit
DELAY 300
ENTER
```

**Open Terminal (Linux):**
```
CTRL ALT T
DELAY 1000
STRING echo "Hello"
ENTER
```

### Tips

- Add delays between commands for applications to respond
- macOS app names are case-sensitive
- Test scripts on similar systems first
- Avoid hardcoding passwords

## Script Storage

DuckyScripts can be saved to and loaded from LittleFS for reuse. Scripts are stored as files in the `/scripts_<name>.txt` format on the NodeMCU filesystem.

## Notes

- All endpoints return JSON: `{"status": "ok/error", "message": "..."}`
- HTTP Basic Authentication is required for all endpoints and web pages
- Default credentials: `admin` / `HID_Admin2024!` (change in config.h)
- Add delays between rapid commands to avoid buffer overflow
