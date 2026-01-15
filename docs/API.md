# API Documentation

REST API reference and command protocol for WiFi USB HID Control.

## Authentication

All API endpoints and web pages require HTTP Basic Authentication.

**Default Credentials:**
- Username: `admin`
- Password: `WiFi_HID!826`

You can change these credentials in `nodemcu/config.h` (WEB_AUTH_USER and WEB_AUTH_PASS).

## HTTPS Support

The device supports both HTTP and HTTPS for secure communication:

- **HTTP**: Port 80 (default, always available)
- **HTTPS**: Port 443 (enabled by default, uses ~15-20KB RAM)

To disable HTTPS and save memory, set `ENABLE_HTTPS 0` in `nodemcu/config.h`.

**HTTPS Certificate:** Self-signed certificate (valid 10 years). Browsers will show a security warning - this is expected. You can regenerate the certificate by running `./generate_cert.sh` in the `nodemcu/` directory.

## REST API Endpoints

Base URL:
- HTTP: `http://<device-ip>` or `http://192.168.4.1` (AP mode)
- HTTPS: `https://<device-ip>` or `https://192.168.4.1` (AP mode)

### Web Interface

- **GET /** - Main control interface (HTML)
- **GET /setup** - WiFi configuration page (HTML)
- **GET /manage-actions.html** - Quick actions management interface (HTML)
- **GET /manage-scripts.html** - Quick scripts management interface (HTML)
- **GET /manage-os.html** - Custom operating systems management (HTML)

---

### POST /api/command

Send a command to Pro Micro. Parameter: `cmd` (required)

```bash
curl -u admin:WiFi_HID!826 -X POST http://192.168.1.100/api/command -d "cmd=TYPE:Hello"
curl -u admin:WiFi_HID!826 -X POST http://192.168.1.100/api/command -d "cmd=ENTER"
curl -u admin:WiFi_HID!826 -X POST http://192.168.1.100/api/command -d "cmd=GUI_R"
```

Response: `{"status": "ok", "message": "Command sent"}`

---

### POST /api/script

Execute DuckyScript. Parameter: `script` (required)

```bash
curl -u admin:WiFi_HID!826 -X POST http://192.168.1.100/api/script -d "script=GUI r
DELAY 500
STRING notepad
ENTER"
```

---

### GET /api/jiggler

Control mouse jiggler with configurable movement patterns.

**Parameters:**
- `enable` (required): `1` to enable, `0` to disable
- `type` (optional): Movement pattern - `simple`, `circles`, or `random` (default: `simple`)
- `diameter` (optional): Movement diameter in pixels, 1-50 (default: `2`)
- `delay` (optional): Delay between movements in milliseconds, 100-60000 (default: `2000`)

```bash
# Enable with default settings (simple movement, 2px diameter, 2000ms delay)
curl -u admin:WiFi_HID!826 http://192.168.1.100/api/jiggler?enable=1

# Enable with circular movement, 10px diameter, 1000ms delay
curl -u admin:WiFi_HID!826 "http://192.168.1.100/api/jiggler?enable=1&type=circles&diameter=10&delay=1000"

# Enable with random movement, 25px diameter, 5000ms delay
curl -u admin:WiFi_HID!826 "http://192.168.1.100/api/jiggler?enable=1&type=random&diameter=25&delay=5000"

# Disable jiggler
curl -u admin:WiFi_HID!826 http://192.168.1.100/api/jiggler?enable=0
```

Response: `{"status": "ok", "enabled": true/false}`

---

### GET /api/status

Get device status (WiFi mode, SSID, IP)

```bash
curl -u admin:WiFi_HID!826 http://192.168.1.100/api/status
```

---

### GET /api/wifi

Get current WiFi settings

```bash
curl -u admin:WiFi_HID!826 http://192.168.1.100/api/wifi
```

---

### POST /api/wifi

Save WiFi credentials and restart. Parameters: `ssid`, `password`

```bash
curl -u admin:WiFi_HID!826 -X POST http://192.168.4.1/api/wifi -d "ssid=MyNetwork" -d "password=pass123"
```

---

### GET /api/scan

Scan for WiFi networks. Returns array of `{ssid, rssi, encryption}`

```bash
curl -u admin:WiFi_HID!826 http://192.168.1.100/api/scan
```

---

### GET /api/quickactions

Get all quick actions for a specific operating system.

**Parameters:**
- `os` (required): Operating system name (e.g., `Windows`, `MacOS`, `Linux`, or custom OS name)

```bash
curl -u admin:WiFi_HID!826 http://192.168.1.100/api/quickactions?os=Windows
```

Response: Array of quick action objects with `cmd`, `label`, `desc`, and `class` fields

---

### POST /api/quickactions

Add a new quick action for a specific operating system.

**Parameters:**
- `os` (required): Operating system name
- `cmd` (required): DuckyScript command (e.g., `GUI_R`, `CTRL_C`)
- `label` (required): Button label
- `desc` (optional): Description
- `class` (optional): Button color class (default: `btn-primary`)

```bash
curl -u admin:WiFi_HID!826 -X POST http://192.168.1.100/api/quickactions \
  -d "os=Windows" \
  -d "cmd=GUI_R" \
  -d "label=Run Dialog" \
  -d "desc=Open Windows Run dialog" \
  -d "class=btn-info"
```

Response: `{"status": "ok", "message": "Quick action saved"}`

---

### POST /api/quickactions/delete

Delete a quick action from a specific operating system.

**Parameters:**
- `os` (required): Operating system name
- `index` (required): Index of the quick action to delete (0-based)

```bash
curl -u admin:WiFi_HID!826 -X POST http://192.168.1.100/api/quickactions/delete \
  -d "os=Windows" \
  -d "index=0"
```

---

### POST /api/quickactions/reorder

Reorder quick actions for a specific operating system.

**Parameters:**
- `os` (required): Operating system name
- `from` (required): Current index of the action
- `to` (required): New index for the action

```bash
curl -u admin:WiFi_HID!826 -X POST http://192.168.1.100/api/quickactions/reorder \
  -d "os=Windows" \
  -d "from=0" \
  -d "to=2"
```

---

### GET /api/quickscripts

Get all quick scripts for a specific operating system.

**Parameters:**
- `os` (required): Operating system name

```bash
curl -u admin:WiFi_HID!826 http://192.168.1.100/api/quickscripts?os=MacOS
```

Response: Array of quick script objects with `id`, `label`, `script`, and `class` fields

---

### POST /api/quickscripts

Add or update a quick script for a specific operating system.

**Parameters:**
- `os` (required): Operating system name
- `id` (required): Unique script identifier (lowercase, no spaces)
- `label` (required): Button label
- `script` (required): DuckyScript content (multi-line)
- `class` (optional): Button color class (default: `btn-primary`)

```bash
curl -u admin:WiFi_HID!826 -X POST http://192.168.1.100/api/quickscripts \
  -d "os=MacOS" \
  -d "id=open-terminal" \
  -d "label=Terminal" \
  -d "script=GUI SPACE
DELAY 500
STRING Terminal
ENTER" \
  -d "class=btn-success"
```

Response: `{"status": "ok", "message": "Quick script saved"}`

---

### POST /api/quickscripts/delete

Delete a quick script from a specific operating system.

**Parameters:**
- `os` (required): Operating system name
- `id` (required): Script ID to delete

```bash
curl -u admin:WiFi_HID!826 -X POST http://192.168.1.100/api/quickscripts/delete \
  -d "os=MacOS" \
  -d "id=open-terminal"
```

---

### GET /api/customos

Get list of all custom operating systems.

```bash
curl -u admin:WiFi_HID!826 http://192.168.1.100/api/customos
```

Response: Array of custom OS names (e.g., `["Android", "ChromeOS", "Raspberry Pi"]`)

---

### POST /api/customos

Add a new custom operating system.

**Parameters:**
- `name` (required): Operating system name

```bash
curl -u admin:WiFi_HID!826 -X POST http://192.168.1.100/api/customos \
  -d "name=Android"
```

Response: `{"status": "ok", "message": "Custom OS added"}`

---

### POST /api/customos/delete

Delete a custom operating system and all its associated quick actions and scripts.

**Parameters:**
- `name` (required): Operating system name to delete

```bash
curl -u admin:WiFi_HID!826 -X POST http://192.168.1.100/api/customos/delete \
  -d "name=Android"
```

Response: `{"status": "ok", "message": "Custom OS deleted"}`

---

### GET /api/scripts

List all saved DuckyScripts.

```bash
curl -u admin:WiFi_HID!826 http://192.168.1.100/api/scripts
```

Response: Array of script filenames

---

### POST /api/scripts

Save a DuckyScript to LittleFS storage.

**Parameters:**
- `name` (required): Script name (used as filename)
- `content` (required): Script content

```bash
curl -u admin:WiFi_HID!826 -X POST http://192.168.1.100/api/scripts \
  -d "name=my-script" \
  -d "content=GUI r
DELAY 500
STRING notepad
ENTER"
```

Response: `{"status": "ok", "message": "Script saved"}`

---

### POST /api/scripts/load

Load a saved DuckyScript from storage.

**Parameters:**
- `name` (required): Script name to load

```bash
curl -u admin:WiFi_HID!826 -X POST http://192.168.1.100/api/scripts/load \
  -d "name=my-script"
```

Response: `{"status": "ok", "content": "script content here"}`

---

### POST /api/scripts/delete

Delete a saved DuckyScript.

**Parameters:**
- `name` (required): Script name to delete

```bash
curl -u admin:WiFi_HID!826 -X POST http://192.168.1.100/api/scripts/delete \
  -d "name=my-script"
```

Response: `{"status": "ok", "message": "Script deleted"}`

---

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

### Keyboard Capture (Press/Release)

Used by the Keyboard Capture feature for real-time key forwarding with separate press and release events:

- **Key Press:** `KEY_PRESS:<key>` - Press and hold a key
- **Key Release:** `KEY_RELEASE:<key>` - Release a specific key
- **Release All:** `KEY_RELEASE_ALL` - Release all currently held keys

**Supported keys:**
- Modifiers: `CTRL`, `SHIFT`, `ALT`, `GUI`
- Navigation: `UP`, `DOWN`, `LEFT`, `RIGHT`, `HOME`, `END`, `PAGEUP`, `PAGEDOWN`
- Editing: `ENTER`, `TAB`, `BACKSPACE`, `DELETE`, `INSERT`, `ESC`
- Function keys: `F1` through `F12`
- Single characters: `a`-`z`, `0`-`9`, symbols

**Examples:**
```bash
# Press Ctrl, press C, release C, release Ctrl (copy shortcut)
curl -u admin:WiFi_HID!826 -X POST http://192.168.1.100/api/command -d "cmd=KEY_PRESS:CTRL"
curl -u admin:WiFi_HID!826 -X POST http://192.168.1.100/api/command -d "cmd=KEY_PRESS:c"
curl -u admin:WiFi_HID!826 -X POST http://192.168.1.100/api/command -d "cmd=KEY_RELEASE:c"
curl -u admin:WiFi_HID!826 -X POST http://192.168.1.100/api/command -d "cmd=KEY_RELEASE:CTRL"

# Release all keys (safety command)
curl -u admin:WiFi_HID!826 -X POST http://192.168.1.100/api/command -d "cmd=KEY_RELEASE_ALL"
```

### Mouse

- **Move:** `MOUSE_MOVE:x,y` - Relative movement in pixels
- **Clicks:** `MOUSE_LEFT`, `MOUSE_RIGHT`, `MOUSE_MIDDLE`, `MOUSE_DOUBLE`
- **Scroll:** `SCROLL:amount` - Positive=down, negative=up

### Utility

- **Delay:** `DELAY:ms` - Pause execution (max 10000ms)
- **Ping:** `PING` - Test communication (responds with PONG)
- **Status:** `STATUS` - Get jiggler status
- **LED:** `LED_ON`, `LED_OFF`
- **Jiggler:** `JIGGLE_ON <type> <diameter> <delay>`, `JIGGLE_OFF`
  - Example: `JIGGLE_ON circles 10 2000` - Enable circular movement with 10px diameter, 2000ms delay
  - Example: `JIGGLE_ON random 25 5000` - Enable random movement with 25px diameter, 5000ms delay
  - Example: `JIGGLE_ON simple 2 2000` - Enable simple movement (default)

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

DuckyScripts can be saved to and loaded from LittleFS for reuse. The system supports two types of scripts:

1. **Saved Scripts** - General purpose scripts stored as `/scripts_<name>.txt` files. Managed via `/api/scripts` endpoints.
2. **Quick Scripts** - Per-OS script buttons stored as `/quickscripts_<OS>.txt` files. Managed via `/api/quickscripts` endpoints and the web interface at `/manage-scripts.html`.

Quick Actions are stored separately in `/quickactions_<OS>.txt` files for instant keyboard shortcuts.

## Notes

- All endpoints return JSON: `{"status": "ok/error", "message": "..."}`
- HTTP Basic Authentication is required for all endpoints and web pages
- Default credentials: `admin` / `WiFi_HID!826` (change in config.h)
- Add delays between rapid commands to avoid buffer overflow
