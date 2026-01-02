# API Documentation

Complete API reference for WiFi USB HID Control system.

## Table of Contents

1. [REST API Endpoints](#rest-api-endpoints)
2. [Command Protocol](#command-protocol)
3. [DuckyScript Reference](#duckyscript-reference)
4. [Examples](#examples)

---

## REST API Endpoints

Base URL: `http://<device-ip>` (e.g., `http://192.168.1.100`)

### GET /

Main web interface.

**Response:** HTML page with full control interface

**Example:**
```bash
curl http://192.168.1.100/
```

---

### GET /setup

WiFi configuration interface.

**Response:** HTML page for WiFi setup

**Example:**
```bash
curl http://192.168.1.100/setup
```

---

### POST /api/command

Send a single command to Pro Micro.

**Parameters:**
- `cmd` (required) - Command string (see Command Protocol below)

**Response:**
```json
{
  "status": "ok",
  "message": "Command sent"
}
```

**Error Response:**
```json
{
  "status": "error",
  "message": "Missing cmd parameter"
}
```

**Examples:**

Type text:
```bash
curl -X POST http://192.168.1.100/api/command \
  -d "cmd=TYPE:Hello World"
```

Press Enter:
```bash
curl -X POST http://192.168.1.100/api/command \
  -d "cmd=ENTER"
```

Open Run dialog (Windows):
```bash
curl -X POST http://192.168.1.100/api/command \
  -d "cmd=GUI_R"
```

**JavaScript Example:**
```javascript
fetch('http://192.168.1.100/api/command', {
  method: 'POST',
  headers: {
    'Content-Type': 'application/x-www-form-urlencoded'
  },
  body: 'cmd=' + encodeURIComponent('TYPE:Hello')
})
.then(response => response.json())
.then(data => console.log(data));
```

---

### POST /api/script

Execute a DuckyScript.

**Parameters:**
- `script` (required) - Multi-line DuckyScript

**Response:**
```json
{
  "status": "ok",
  "message": "Script executed"
}
```

**Error Response:**
```json
{
  "status": "error",
  "message": "Missing script parameter"
}
```

**Example:**
```bash
curl -X POST http://192.168.1.100/api/script \
  -d "script=GUI r
DELAY 500
STRING notepad
ENTER"
```

**JavaScript Example:**
```javascript
const script = `GUI r
DELAY 500
STRING notepad
ENTER
DELAY 1000
STRING Hello World!`;

fetch('http://192.168.1.100/api/script', {
  method: 'POST',
  headers: {
    'Content-Type': 'application/x-www-form-urlencoded'
  },
  body: 'script=' + encodeURIComponent(script)
})
.then(response => response.json())
.then(data => console.log(data));
```

---

### GET /api/jiggler

Control mouse jiggler.

**Parameters:**
- `enable` (required) - "1" to enable, "0" to disable

**Response:**
```json
{
  "status": "ok",
  "enabled": true
}
```

**Examples:**

Enable jiggler:
```bash
curl http://192.168.1.100/api/jiggler?enable=1
```

Disable jiggler:
```bash
curl http://192.168.1.100/api/jiggler?enable=0
```

**JavaScript Example:**
```javascript
// Enable
fetch('http://192.168.1.100/api/jiggler?enable=1')
  .then(response => response.json())
  .then(data => console.log(data));

// Disable
fetch('http://192.168.1.100/api/jiggler?enable=0')
  .then(response => response.json())
  .then(data => console.log(data));
```

---

### GET /api/status

Get device status.

**Response:**
```json
{
  "wifi_mode": "Station",
  "ssid": "MyNetwork",
  "ip": "192.168.1.100",
  "connected": true
}
```

**Response in AP Mode:**
```json
{
  "wifi_mode": "AP",
  "ssid": "USB-HID-Setup",
  "ip": "192.168.4.1",
  "connected": false
}
```

**Example:**
```bash
curl http://192.168.1.100/api/status
```

**JavaScript Example:**
```javascript
fetch('http://192.168.1.100/api/status')
  .then(response => response.json())
  .then(data => {
    console.log('WiFi Mode:', data.wifi_mode);
    console.log('SSID:', data.ssid);
    console.log('IP:', data.ip);
  });
```

---

### GET /api/wifi

Get current WiFi settings.

**Response:**
```json
{
  "ssid": "MyNetwork",
  "mode": "Station"
}
```

**Example:**
```bash
curl http://192.168.1.100/api/wifi
```

---

### POST /api/wifi

Save new WiFi credentials and restart.

**Parameters:**
- `ssid` (required) - Network SSID
- `password` (required) - Network password

**Response:**
```json
{
  "status": "ok",
  "message": "WiFi credentials saved. Restarting..."
}
```

**Note:** Device will restart after this call and connect to new network.

**Example:**
```bash
curl -X POST http://192.168.4.1/api/wifi \
  -d "ssid=MyNetwork" \
  -d "password=MyPassword123"
```

**JavaScript Example:**
```javascript
fetch('http://192.168.4.1/api/wifi', {
  method: 'POST',
  headers: {
    'Content-Type': 'application/x-www-form-urlencoded'
  },
  body: 'ssid=MyNetwork&password=MyPassword123'
})
.then(response => response.json())
.then(data => console.log(data));
```

---

### GET /api/scan

Scan for available WiFi networks.

**Response:**
```json
[
  {
    "ssid": "MyNetwork",
    "rssi": -45,
    "encryption": 4
  },
  {
    "ssid": "NeighborWiFi",
    "rssi": -67,
    "encryption": 4
  }
]
```

**Encryption Types:**
- 2 = TKIP (WPA)
- 4 = CCMP (WPA2)
- 5 = WEP
- 7 = None (Open)
- 8 = Auto (WPA/WPA2)

**Example:**
```bash
curl http://192.168.1.100/api/scan
```

**JavaScript Example:**
```javascript
fetch('http://192.168.1.100/api/scan')
  .then(response => response.json())
  .then(networks => {
    networks.forEach(network => {
      console.log(`${network.ssid}: ${network.rssi} dBm`);
    });
  });
```

---

## Command Protocol

Commands sent from NodeMCU to Pro Micro via serial communication.

### Keyboard Commands

#### Type Text

**Format:** `TYPE:text`

Types the specified text without pressing Enter.

**Examples:**
- `TYPE:Hello World`
- `TYPE:username@example.com`
- `TYPE:This is a test`

---

#### Type Text with Enter

**Format:** `TYPELN:text`

Types the specified text and presses Enter.

**Examples:**
- `TYPELN:notepad`
- `TYPELN:Hello World`

---

#### Special Keys

**Enter:** `ENTER`
**Escape:** `ESC`
**Tab:** `TAB`
**Backspace:** `BACKSPACE`
**Delete:** `DELETE`

**Examples:**
```
ENTER
ESC
TAB
```

---

#### Arrow Keys

**Up:** `UP`
**Down:** `DOWN`
**Left:** `LEFT`
**Right:** `RIGHT`

---

#### Function Keys

**Format:** `F1` through `F12`

**Examples:**
```
F1
F5
F11
F12
```

---

#### GUI/Windows/Command Key Combinations

**GUI+R:** `GUI_R` (Run dialog on Windows)
**GUI+D:** `GUI_D` (Show desktop)
**GUI+Space:** `GUI_SPACE` (Spotlight on macOS)
**GUI alone:** `GUI` (Windows/Command key)

**Custom combinations:** `GUI_<key>`

**Examples:**
```
GUI_R       # Windows Run dialog
GUI_D       # Show desktop
GUI_L       # Lock computer (Windows)
GUI_E       # Open Explorer (Windows)
```

---

#### Keyboard Shortcuts

**Alt+Tab:** `ALT_TAB`
**Ctrl+Alt+Delete:** `CTRL_ALT_DEL`
**Ctrl+Alt+T:** `CTRL_ALT_T` (Terminal on Linux)

---

#### Ctrl Combinations

**Format:** `CTRL_<key>`

**Examples:**
```
CTRL_c      # Copy
CTRL_v      # Paste
CTRL_x      # Cut
CTRL_z      # Undo
CTRL_s      # Save
CTRL_a      # Select all
```

---

#### Alt Combinations

**Format:** `ALT_<key>`

**Examples:**
```
ALT_F4      # Close window
ALT_TAB     # Switch windows
```

---

### Mouse Commands

#### Mouse Movement

**Format:** `MOUSE_MOVE:x,y`

Moves mouse by x and y pixels (can be positive or negative).

**Examples:**
```
MOUSE_MOVE:10,0      # Move right 10 pixels
MOUSE_MOVE:-10,0     # Move left 10 pixels
MOUSE_MOVE:0,10      # Move down 10 pixels
MOUSE_MOVE:0,-10     # Move up 10 pixels
MOUSE_MOVE:50,50     # Move down-right
```

**Note:** Movement is relative, not absolute.

---

#### Mouse Clicks

**Left click:** `MOUSE_LEFT`
**Right click:** `MOUSE_RIGHT`
**Middle click:** `MOUSE_MIDDLE`
**Double click:** `MOUSE_DOUBLE`

**Examples:**
```
MOUSE_LEFT      # Single left click
MOUSE_DOUBLE    # Double left click
MOUSE_RIGHT     # Right click (context menu)
```

---

#### Mouse Scroll

**Format:** `SCROLL:amount`

Positive values scroll down, negative scroll up.

**Examples:**
```
SCROLL:5        # Scroll down
SCROLL:-5       # Scroll up
SCROLL:10       # Scroll down more
```

---

### Web Interface Trackpad

The web interface includes a live mouse trackpad for smooth, intuitive mouse control.

#### Features

**Live Tracking:**
- Drag on trackpad area to move mouse cursor
- Click/tap for left mouse click
- Double-click/double-tap for double-click
- Right-click (context menu) for right mouse click
- Visual cursor indicator shows touch position

**Adjustable Sensitivity:**
- Sensitivity slider: 0.5x to 10.0x (default: 1.5x)
- Lower values (0.5x - 1.0x) for precise control
- Medium values (1.5x - 3.0x) for normal usage
- Higher values (3.0x - 10.0x) for very fast movement

**Touch Support:**
- Full support for touch devices (phones, tablets)
- Multi-touch aware (single finger tracking)
- Smooth gesture recognition

**Throttling:**
- Commands throttled to 50ms intervals
- Prevents server flooding
- Ensures smooth performance

#### How to Use

**Desktop (Mouse):**
1. Click and drag on the trackpad area
2. Mouse cursor moves proportionally to your drag
3. Release to stop (optionally clicks if no movement)
4. Double-click quickly for double-click action
5. Right-click on trackpad for right mouse button

**Mobile (Touch):**
1. Touch and drag on the trackpad area
2. Mouse moves based on finger movement
3. Lift finger to stop (taps act as clicks)
4. Double-tap quickly for double-click
5. Use sensitivity slider to adjust tracking speed

#### Technical Details

**Movement Calculation:**
```javascript
moveX = deltaX * sensitivity
moveY = deltaY * sensitivity
```

**Throttle Rate:** 50ms (20 movements per second max)

**Click Detection:**
- Movement < 5 pixels (mouse) = click
- Movement < 10 pixels (touch) = tap
- Double-click window: 300ms

**Command Sent:**
```
MOUSE_MOVE:x,y
```

Where x and y are relative pixel movements multiplied by sensitivity.

#### Example Usage

**Scenario 1: Fine Control (Precision)**
- Set sensitivity to 0.5x - 0.8x
- Make small, precise movements
- Ideal for graphic design, selecting text, detailed work

**Scenario 2: Normal Usage**
- Set sensitivity to 1.5x - 2.5x (default range)
- Balanced speed and control
- Ideal for general navigation and everyday use

**Scenario 3: Fast Navigation**
- Set sensitivity to 3.0x - 5.0x
- Make larger movements quickly
- Ideal for browsing, moving across large screens

**Scenario 4: Ultra-Fast Movement**
- Set sensitivity to 6.0x - 10.0x
- Very responsive, small drags = large movements
- Ideal for multi-monitor setups, 4K displays, quick positioning

**Scenario 5: Mobile Control**
- Use on smartphone or tablet
- Touch and drag naturally
- Start with 2.0x - 3.0x for mobile, adjust based on screen size

---

### Utility Commands

#### Delay

**Format:** `DELAY:ms`

Pauses execution for specified milliseconds (max 10000ms = 10 seconds).

**Examples:**
```
DELAY:500       # Wait 0.5 seconds
DELAY:1000      # Wait 1 second
DELAY:2000      # Wait 2 seconds
```

---

#### Ping Test

**Command:** `PING`
**Response:** `PONG`

Tests communication between NodeMCU and Pro Micro.

---

#### Status Check

**Command:** `STATUS`
**Response:** `STATUS:Jiggler=ON` or `STATUS:Jiggler=OFF`

Gets current Pro Micro status.

---

#### LED Control

**LED On:** `LED_ON`
**LED Off:** `LED_OFF`

Controls the built-in LED.

---

#### Mouse Jiggler

**Enable:** `JIGGLE_ON`
**Disable:** `JIGGLE_OFF`

Enables or disables automatic mouse movement.

**Response:**
- `OK:Jiggler enabled`
- `OK:Jiggler disabled`

---

## DuckyScript Reference

DuckyScript is a simple scripting language for USB HID automation, originally created for USB Rubber Ducky.

### Syntax

Each command is on its own line. Comments start with `//`.

```
// This is a comment
DELAY 500
STRING Hello World
ENTER
```

### Supported Commands

#### STRING

Types the specified text.

**Syntax:** `STRING <text>`

**Examples:**
```
STRING Hello World
STRING notepad
STRING https://example.com
```

---

#### DELAY

Pauses execution for specified milliseconds.

**Syntax:** `DELAY <milliseconds>`

**Examples:**
```
DELAY 500
DELAY 1000
DELAY 2000
```

**Best Practice:** Add delays between commands to allow applications to respond.

---

#### ENTER, ESC, TAB, BACKSPACE, DELETE

Special keys.

**Examples:**
```
ENTER
ESC
TAB
BACKSPACE
DELETE
```

---

#### GUI

Windows/Command/Super key.

**Syntax:** `GUI [key]`

**Examples:**
```
GUI             # Press Windows/Command key alone
GUI r           # Windows+R (Run dialog)
GUI d           # Windows+D (Show desktop)
GUI SPACE       # Windows+Space or Command+Space (Spotlight)
```

---

#### ALT

Alt key combinations.

**Syntax:** `ALT <key>` or `ALT TAB`

**Examples:**
```
ALT TAB         # Alt+Tab
ALT F4          # Alt+F4
```

---

#### CTRL

Ctrl key combinations.

**Syntax:** `CTRL <key>`

**Examples:**
```
CTRL c          # Ctrl+C (copy)
CTRL v          # Ctrl+V (paste)
CTRL s          # Ctrl+S (save)
```

---

#### CTRL ALT

Ctrl+Alt combinations.

**Syntax:** `CTRL ALT <key>` or special shortcuts

**Examples:**
```
CTRL ALT DELETE    # Ctrl+Alt+Delete
CTRL ALT T         # Ctrl+Alt+T (Terminal on Linux)
```

---

#### Arrow Keys

**UP, DOWN, LEFT, RIGHT**

**Examples:**
```
UP
DOWN
LEFT
RIGHT
```

---

#### Function Keys

**F1 through F12**

**Examples:**
```
F1
F5
F11
F12
```

---

#### Comments

Lines starting with `//` are ignored.

**Examples:**
```
// Open Run dialog
GUI r
// Wait for it to open
DELAY 500
// Type notepad
STRING notepad
// Execute
ENTER
```

---

### Complete DuckyScript Examples

#### Example 1: Open Notepad (Windows)

```
// Open Run dialog
GUI r
DELAY 500
// Type notepad
STRING notepad
ENTER
DELAY 1000
// Type message
STRING Hello from DuckyScript!
```

---

#### Example 2: Open TextEdit (macOS)

```
// Open Spotlight
GUI SPACE
DELAY 700
// Type TextEdit (case-sensitive!)
STRING TextEdit
DELAY 300
ENTER
DELAY 1000
// Type message
STRING Hello from macOS!
```

---

#### Example 3: Open Terminal and Run Command (Linux)

```
// Open terminal
CTRL ALT T
DELAY 1000
// Run system update
STRING sudo apt update
ENTER
DELAY 500
// Type password (be careful with this!)
STRING mypassword
ENTER
```

---

#### Example 4: Create and Save Text File (Windows)

```
// Open Notepad
GUI r
DELAY 500
STRING notepad
ENTER
DELAY 1000
// Type content
STRING This is a test file.
ENTER
STRING Created with DuckyScript automation.
DELAY 500
// Save file
CTRL s
DELAY 500
// Type filename
STRING test.txt
ENTER
DELAY 500
// Close Notepad
ALT F4
```

---

#### Example 5: Open Website in Browser

```
// Open Run dialog
GUI r
DELAY 500
// Open default browser with URL
STRING chrome https://example.com
ENTER
```

---

#### Example 6: Take Screenshot (Windows)

```
// Open Snipping Tool
GUI r
DELAY 500
STRING snippingtool
ENTER
DELAY 2000
// Take new screenshot
CTRL n
```

---

### Best Practices

1. **Add Delays:** Always add delays between commands
   ```
   GUI r
   DELAY 500          // Wait for Run dialog
   STRING notepad
   DELAY 100          // Wait before pressing Enter
   ENTER
   ```

2. **Test on Similar System:** Test scripts on a similar system first

3. **Be Careful with Passwords:** Avoid hardcoding passwords in scripts

4. **Comment Your Code:** Use comments to explain what script does
   ```
   // Open calculator
   GUI r
   DELAY 500
   STRING calc
   ENTER
   ```

5. **Handle Errors:** Consider what happens if a window doesn't open
   ```
   GUI r
   DELAY 1000         // Longer delay for slower systems
   STRING notepad
   ENTER
   DELAY 2000         // Wait for Notepad to fully load
   ```

6. **Adjust Delays for Your System:** Slower systems need longer delays

7. **macOS Spotlight Requirements:**
   - Application names are **case-sensitive** (e.g., "TextEdit" not "textedit")
   - Use longer delays (700ms) for Spotlight to open
   - Add delay after typing app name before pressing Enter (300ms)
   ```
   // macOS - Open TextEdit via Spotlight
   GUI SPACE           // Open Spotlight (Command+Space)
   DELAY 700           // Wait for Spotlight to appear
   STRING TextEdit     // Must use proper capitalization!
   DELAY 300           // Wait for search results
   ENTER
   ```

   **macOS Spotlight Troubleshooting:**
   - If Spotlight doesn't open, check macOS keyboard shortcuts in System Preferences
   - Default is Command+Space, but it can be changed
   - Alternative methods for macOS:
     ```
     // Method 1: Use Launchpad (F4 key or Fn+F4)
     F4
     DELAY 500
     STRING TextEdit
     DELAY 300
     ENTER

     // Method 2: Open from Applications folder
     GUI SPACE
     DELAY 700
     STRING /Applications/TextEdit.app
     ENTER

     // Method 3: Use Finder
     GUI SPACE
     DELAY 700
     STRING Finder
     ENTER
     DELAY 500
     GUI SHIFT a         // Go to Applications
     DELAY 500
     STRING TextEdit
     ENTER
     ```

8. **Windows vs macOS Differences:**
   - **Windows:** Use `GUI r` (Run dialog) - fast, reliable
   - **macOS:** Use `GUI SPACE` (Spotlight) - requires proper app names
   - **Linux:** Use `CTRL ALT T` (Terminal) or GUI key for app launcher

---

## Examples

### Python Example: Type Text

```python
import requests

def type_text(ip, text):
    url = f"http://{ip}/api/command"
    data = {"cmd": f"TYPE:{text}"}
    response = requests.post(url, data=data)
    return response.json()

# Usage
result = type_text("192.168.1.100", "Hello World")
print(result)
```

---

### Python Example: Execute Script

```python
import requests

def execute_script(ip, script):
    url = f"http://{ip}/api/script"
    data = {"script": script}
    response = requests.post(url, data=data)
    return response.json()

# Usage
script = """
GUI r
DELAY 500
STRING notepad
ENTER
DELAY 1000
STRING Hello from Python!
"""

result = execute_script("192.168.1.100", script)
print(result)
```

---

### Python Example: Mouse Jiggler Control

```python
import requests
import time

def set_jiggler(ip, enabled):
    url = f"http://{ip}/api/jiggler"
    params = {"enable": "1" if enabled else "0"}
    response = requests.get(url, params=params)
    return response.json()

# Usage - Run jiggler for 1 hour
ip = "192.168.1.100"

# Enable
print("Enabling jiggler...")
set_jiggler(ip, True)

# Wait 1 hour
time.sleep(3600)

# Disable
print("Disabling jiggler...")
set_jiggler(ip, False)
```

---

### Node.js Example: Send Commands

```javascript
const axios = require('axios');

async function sendCommand(ip, cmd) {
  const url = `http://${ip}/api/command`;
  const params = new URLSearchParams();
  params.append('cmd', cmd);

  const response = await axios.post(url, params);
  return response.data;
}

async function main() {
  const ip = '192.168.1.100';

  // Type text
  await sendCommand(ip, 'TYPE:Hello from Node.js!');

  // Press Enter
  await sendCommand(ip, 'ENTER');
}

main();
```

---

### cURL Examples

```bash
# Type text
curl -X POST http://192.168.1.100/api/command -d "cmd=TYPE:Hello"

# Press Enter
curl -X POST http://192.168.1.100/api/command -d "cmd=ENTER"

# Open Run dialog
curl -X POST http://192.168.1.100/api/command -d "cmd=GUI_R"

# Enable jiggler
curl http://192.168.1.100/api/jiggler?enable=1

# Get status
curl http://192.168.1.100/api/status

# Execute script
curl -X POST http://192.168.1.100/api/script -d "script=GUI r
DELAY 500
STRING notepad
ENTER"
```

---

### Bash Script Example: Automation

```bash
#!/bin/bash

IP="192.168.1.100"
API="http://$IP/api"

# Function to send command
send_cmd() {
  curl -s -X POST "$API/command" -d "cmd=$1"
}

# Function to execute script
exec_script() {
  curl -s -X POST "$API/script" -d "script=$1"
}

# Open notepad and type message
SCRIPT="GUI r
DELAY 500
STRING notepad
ENTER
DELAY 1000
STRING Automated message from bash script
ENTER
STRING Current time: $(date)
ENTER"

exec_script "$SCRIPT"
```

---

## Error Handling

All API endpoints return JSON responses.

**Success Response:**
```json
{
  "status": "ok",
  "message": "..."
}
```

**Error Response:**
```json
{
  "status": "error",
  "message": "Error description"
}
```

**HTTP Status Codes:**
- 200 = Success
- 400 = Bad Request (missing parameters)
- 500 = Internal Server Error

---

## Rate Limiting

There are no built-in rate limits, but be mindful:

- Don't send commands faster than they can be executed
- Add appropriate delays in scripts
- Pro Micro processes commands sequentially
- Flooding with commands may cause buffer overflow

**Recommended:** Add 50-100ms delay between rapid commands.

---

## Security Considerations

1. **No Authentication:** API has no built-in authentication
2. **Local Network Only:** Keep device on trusted local network
3. **Firewall:** Consider firewall rules if needed
4. **Change Default AP Password:** Modify code to change default WiFi password
5. **HTTPS:** Not supported - traffic is unencrypted

**For production use, consider:**
- Adding authentication headers
- Using VPN for remote access
- Implementing HTTPS (requires additional libraries)
- Adding API key validation

---

**Need more examples?** Check the web interface source code in `nodemcu/nodemcu.ino` for JavaScript examples!
