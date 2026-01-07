# Setup Guide

Complete setup from Arduino IDE installation to first use.

## 1. Prepare Arduino IDE

### Install Arduino IDE
1. Download from [arduino.cc](https://www.arduino.cc/en/software) (version 1.8+ or 2.x)
2. Install and launch

### Add ESP8266 Support
1. **File → Preferences** → "Additional Board Manager URLs"
2. Add: `http://arduino.esp8266.com/stable/package_esp8266com_index.json`
3. **Tools → Board → Board Manager** → Search "esp8266" → Install "esp8266 by ESP8266 Community"

### Optional: OLED Display Libraries
If using 128x64 OLED display:
- **Tools → Manage Libraries** → Install "Adafruit GFX Library" and "Adafruit SSD1306"

## 2. Upload Pro Micro Code

1. Connect Pro Micro via USB
2. **Tools → Board** → Select **Arduino Leonardo**
3. **Tools → Port** → Select Pro Micro port
4. Open `pro-micro/pro-micro.ino`
5. Click **Upload** (or Ctrl+U / Cmd+U)
6. Wait for "Done uploading"
7. Open **Serial Monitor** (115200 baud) - should see "READY"

**Note:** If upload fails, try pressing reset button twice quickly.

## 3. Upload NodeMCU Code

1. Disconnect Pro Micro, connect NodeMCU via USB
2. **Tools → Board → ESP8266 Boards** → Select **NodeMCU 1.0 (ESP-12E Module)**
3. **Tools** settings:
   - Upload Speed: 115200
   - Flash Size: 4MB (FS:2MB OTA:~1019KB)
4. Open `nodemcu/nodemcu.ino`
5. Click **Upload**
6. Upload web files using LittleFS Data Upload tool (see [LITTLEFS_SETUP.md](LITTLEFS_SETUP.md))
7. Open **Serial Monitor** (115200 baud) - should see AP mode started

## 4. Connect Hardware

See [WIRING.md](WIRING.md) for detailed wiring diagrams.

**Quick summary:**
1. Connect 4 wires between devices:
   - Pro Micro **VCC** → NodeMCU **Vin** (power)
   - Pro Micro **GND** → NodeMCU **GND** (ground)
   - Pro Micro **TX (Pin 1)** → NodeMCU **RX** (crossed!)
   - Pro Micro **RX (Pin 0)** → NodeMCU **TX** (crossed!)
2. Connect Pro Micro to computer via USB (powers both devices)

## 5. First Use

### Access Web Interface
1. Connect to WiFi network **"USB-HID-Setup"** (password: **HID_M4ster**)
2. Open browser and go to **http://192.168.4.1**
3. You should see the control interface

### Optional: Configure WiFi
To connect to your home/office network:
1. Click **WiFi Settings** in web interface
2. Click **Scan Networks**
3. Select your network and enter password
4. Click **Save & Connect**
5. Device restarts and connects to your network
6. Find new IP address in Serial Monitor or router admin panel

### Test the System
1. **Type test:** Open notepad on target computer, use web interface to type text
2. **Mouse test:** Click arrow buttons or use live trackpad
3. **Jiggler test:** Enable mouse jiggler, watch cursor move every 2 seconds
4. **Script test:** Try a quick script for your OS

All features work in both AP mode (192.168.4.1) and Station mode (your network IP).

## 6. Troubleshooting

### Upload Issues
- **Pro Micro upload fails:** Press reset button twice quickly for bootloader mode. Ensure "Arduino Leonardo" board is selected.
- **NodeMCU upload fails:** Try different USB cable/port. Install CH340 or CP2102 drivers (Windows). Lower upload speed to 57600 if timeout occurs.

### WiFi Issues
- **Can't connect to WiFi:** ESP8266 only supports 2.4GHz networks. Check SSID/password (case-sensitive). Connect to AP mode and reconfigure.
- **Web interface won't load:** Verify correct IP address. Try different browser. Check firewall settings.

### Communication Issues
- **No communication between devices:** Verify TX/RX are crossed (not straight-through). Check GND connection. Verify both devices powered on.
- **Commands not working:** Click target application first. Pro Micro must be connected via USB to target computer. Some commands are OS-specific.

### Other Issues
- **Jiggler not working:** Check Serial Monitor for "OK:Jiggler enabled". LED should blink every 2 seconds. Movement is small (±2 pixels).
- **Script not executing:** Add delays between commands. Verify syntax. Ensure target app has focus.

## Quick Reference

**Pro Micro:** Board = Arduino Leonardo, Baud = 115200
**NodeMCU:** Board = NodeMCU 1.0, Upload = 115200, Flash = 4MB (FS:2MB)
**Default AP:** SSID = USB-HID-Setup, Password = HID_M4ster, IP = 192.168.4.1
