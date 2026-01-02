# Setup Guide

This guide will walk you through the complete setup process from installing the Arduino IDE to using your WiFi USB HID Control system.

## Table of Contents

1. [Prepare Arduino IDE](#1-prepare-arduino-ide)
2. [Configure Pro Micro](#2-configure-pro-micro)
3. [Configure NodeMCU](#3-configure-nodemcu)
4. [Physical Connection](#4-physical-connection)
5. [Test Communication](#5-test-communication)
6. [First Use](#6-first-use)
7. [Troubleshooting](#7-troubleshooting)

---

## 1. Prepare Arduino IDE

### 1.1 Install Arduino IDE

1. Download Arduino IDE from [arduino.cc](https://www.arduino.cc/en/software)
2. Install version 1.8.x or 2.x
3. Launch Arduino IDE

### 1.2 Add ESP8266 Board Support

1. Open **File → Preferences** (or **Arduino IDE → Settings** on macOS)
2. In "Additional Board Manager URLs", add:
   ```
   http://arduino.esp8266.com/stable/package_esp8266com_index.json
   ```
3. If there are existing URLs, separate them with commas or newlines
4. Click **OK**

5. Open **Tools → Board → Board Manager**
6. Search for "esp8266"
7. Find "esp8266 by ESP8266 Community"
8. Click **Install** (this may take several minutes)
9. Wait for installation to complete
10. Close Board Manager

### 1.3 Verify Libraries

The following libraries should be available:

**Built-in Arduino libraries** (no installation needed):
- Keyboard
- Mouse
- EEPROM

**ESP8266 libraries** (included with ESP8266 board package):
- ESP8266WiFi
- ESP8266WebServer

To verify, go to **Sketch → Include Library** and check if these appear in the list.

---

## 2. Configure Pro Micro

### 2.1 Select Board

1. Connect Pro Micro to computer via USB cable
2. In Arduino IDE, go to **Tools → Board**
3. Select **Arduino Leonardo** (Pro Micro uses same chip)
4. Go to **Tools → Port**
5. Select the port for Pro Micro (e.g., COM3 on Windows, /dev/cu.usbmodem* on macOS)

**Note:** If Pro Micro doesn't appear, you may need to install drivers:
- Windows: Install SparkFun drivers
- macOS: Usually works without additional drivers
- Linux: May need to add user to dialout group

### 2.2 Open Pro Micro Code

1. In Arduino IDE, open `pro-micro/pro-micro.ino`
2. Review the code if desired

### 2.3 Upload to Pro Micro

1. Click **Upload** button (right arrow icon) or press **Ctrl+U** (Cmd+U on macOS)
2. Wait for "Compiling sketch..."
3. Wait for "Uploading..."
4. Look for "Done uploading" message

**Expected output:**
```
Sketch uses XXXX bytes (XX%) of program storage space.
Global variables use XXX bytes (XX%) of dynamic memory.
Done uploading.
```

### 2.4 Test Pro Micro

1. Keep Pro Micro connected
2. Open **Tools → Serial Monitor**
3. Set baud rate to **115200**
4. You should see "READY" after a few seconds
5. The built-in LED should blink 3 times on startup

**If you see "READY":** ✅ Pro Micro is working correctly!

**Troubleshooting:**
- No "READY" message: Check baud rate is 115200
- Upload error: Try pressing reset button twice quickly to enter bootloader
- Wrong port: Disconnect and reconnect, check port again

---

## 3. Configure NodeMCU

### 3.1 Select Board

1. Disconnect Pro Micro (important!)
2. Connect NodeMCU to computer via USB cable
3. In Arduino IDE, go to **Tools → Board → ESP8266 Boards**
4. Select **NodeMCU 1.0 (ESP-12E Module)**

### 3.2 Configure Upload Settings

Go to **Tools** menu and set:

- **Board:** "NodeMCU 1.0 (ESP-12E Module)"
- **Upload Speed:** "115200"
- **CPU Frequency:** "80 MHz"
- **Flash Size:** "4MB (FS:2MB OTA:~1019KB)"
- **Port:** Select NodeMCU port (different from Pro Micro)

**Note:** Port will be different from Pro Micro's port. It may show as:
- Windows: COM port (e.g., COM4)
- macOS: /dev/cu.usbserial* or /dev/cu.wchusbserial*
- Linux: /dev/ttyUSB*

### 3.3 Open NodeMCU Code

1. In Arduino IDE, open `nodemcu/nodemcu.ino`
2. Review the code if desired
3. Note the default AP credentials:
   - SSID: `USB-HID-Setup`
   - Password: `12345678`

### 3.4 Upload to NodeMCU

1. Click **Upload** button or press **Ctrl+U** (Cmd+U on macOS)
2. Wait for compilation (may take 1-2 minutes for ESP8266)
3. Wait for upload (can take 30-60 seconds)
4. Look for "Done uploading" message

**Expected output:**
```
Sketch uses XXXXX bytes (XX%) of program storage space.
Global variables use XXXXX bytes (XX%) of dynamic memory.
Done uploading.
```

### 3.5 Test NodeMCU

1. Keep NodeMCU connected
2. Open **Tools → Serial Monitor**
3. Set baud rate to **115200**
4. Press **Reset** button on NodeMCU
5. You should see startup messages

**Expected Serial Monitor output:**
```
Attempting to connect to saved network: [SSID or empty]
Failed to connect, starting AP mode
AP Mode started
SSID: USB-HID-Setup
Password: 12345678
IP: 192.168.4.1
Web server started
AP Mode - IP: 192.168.4.1
```

**If you see similar messages:** ✅ NodeMCU is working correctly!

---

## 4. Physical Connection

### 4.1 Review Wiring

Before connecting, review [WIRING.md](WIRING.md) for detailed instructions.

**Quick reminder - 4 wires needed:**
- Pro Micro VCC → NodeMCU Vin (5V power)
- Pro Micro GND → NodeMCU GND (ground)
- NodeMCU TX → Pro Micro RX/Pin 0 (data, crossed!)
- NodeMCU RX → Pro Micro TX/Pin 1 (data, crossed!)

### 4.2 Make Connections

1. **Disconnect both devices from USB**
2. Connect 4 jumper wires as described in WIRING.md:
   - **VCC to Vin** (power - red wire recommended)
   - **GND to GND** (ground - black wire recommended)
   - **TX to RX** (crossed! - yellow wire recommended)
   - **RX to TX** (crossed! - orange wire recommended)
3. **Double-check all connections**
4. Verify TX/RX are **crossed**, not straight through
5. Verify VCC→Vin power connection is secure

### 4.3 Power On

1. Connect **Pro Micro** to the computer you want to control (via USB)
2. **NodeMCU will power on automatically** (powered from Pro Micro's VCC pin)
3. Both devices should power on simultaneously
4. Pro Micro LED should blink 3 times (if code is loaded)
5. NodeMCU LED should blink during startup

**Note:** Only ONE USB cable is needed! NodeMCU gets power through the VCC→Vin connection.

---

## 5. Test Communication

### 5.1 Check Serial Communication (Optional)

If you want to verify communication:

1. Open Serial Monitor on NodeMCU's port (115200 baud)
2. Type `PING` and press Enter
3. You should see "PONG" response from Pro Micro
4. This confirms serial communication is working

### 5.2 Verify Pro Micro HID

Test if computer recognizes Pro Micro as HID device:

1. Pro Micro should appear as a keyboard/mouse device
2. No driver installation needed on most systems
3. Device is ready to receive commands

---

## 6. First Use

### 6.1 Understanding WiFi Behavior

The device features **intelligent automatic WiFi management**:

**On Every Startup:**
1. Device attempts to connect to the **last saved WiFi network**
2. If no saved network exists OR connection fails, device automatically enters **Access Point (AP) mode**
3. In AP mode, creates WiFi network: **"USB-HID-Setup"** (password: **12345678**)

**Important:** All HID control features (keyboard, mouse, trackpad, scripts) work in **both AP mode and Station mode**. You can use the device immediately in AP mode without configuring WiFi!

### 6.2 Connect to WiFi Setup (AP Mode)

On first boot (or if WiFi connection fails), the device creates its own WiFi access point:

1. On your phone or computer, look for WiFi network: **USB-HID-Setup**
2. Connect using password: **12345678**
3. Open web browser
4. Navigate to: **http://192.168.4.1**

You should see the web interface! **All features are fully functional in this mode.**

### 6.3 Configure WiFi (Optional)

To connect the device to your home/office WiFi network:

1. In the web interface, click **WiFi Settings** button
2. Click **Scan Networks** to see available WiFi networks
3. Wait for network list to appear
4. Click on your network name (or enter SSID manually)
5. Enter WiFi password
6. Click **Save & Connect**
7. Device will save credentials and restart

![WiFi Configuration](resource/wifiConfiguration.png)
*WiFi configuration interface with network scanner*

**After Configuration:**
- Device automatically connects to your WiFi on every startup
- If connection fails, it falls back to AP mode
- WiFi credentials are saved permanently in EEPROM
- You can reconfigure WiFi anytime by accessing the Settings page
- All features work in both AP and Station modes

**Why Configure WiFi?**
- Access device from anywhere on your network (not just direct connection)
- Control from multiple devices simultaneously
- Use while connected to your regular WiFi network
- More convenient for permanent installations

### 6.4 Find Device IP Address (Station Mode)

After successfully connecting to your WiFi network:

**Option 1: Serial Monitor**
1. Open Serial Monitor on NodeMCU (115200 baud)
2. Press Reset button
3. Look for "Station Mode - IP: xxx.xxx.xxx.xxx"

**Option 2: Router Admin Panel**
1. Log into your router
2. Look for connected devices
3. Find device named "ESP_XXXXXX"

**Option 3: Network Scanner**
1. Use network scanning app (e.g., Fing, Advanced IP Scanner)
2. Look for ESP8266 device

**Option 4: Check Web Interface**
1. The device status is shown at the top of the web interface
2. IP address, WiFi mode, and network name are displayed

### 6.5 Access Web Interface

**In AP Mode:**
- Navigate to: **http://192.168.4.1**

**In Station Mode (After WiFi Configuration):**
- Navigate to NodeMCU's IP address (e.g., http://192.168.1.100)

You'll see the main control interface with all features available:

![Web Interface](resource/UI_screenshot.png)
*The main control interface - works identically in both AP and Station modes*

### 6.6 Test Basic Functions

Try these quick tests:

**Test 1: Simple Keystroke**
1. Click on any application on target computer (to give it focus)
2. In web interface, click **Enter** button
3. Computer should register an Enter keypress

**Test 2: Type Text**
1. Open Notepad or text editor on target computer
2. In web interface, type "Hello World" in text field
3. Click **Type** button
4. Text should appear in Notepad

![Type Text Feature](resource/typeText.png)

**Test 3: Mouse Control**
1. In web interface, click arrow buttons in Mouse Control
2. Mouse cursor should move on target computer
3. Try the live trackpad by dragging on it to move the mouse smoothly

![Mouse Control](resource/mouseControl.png)

**Test 4: Mouse Jiggler**
1. Toggle Mouse Jiggler switch to ON
2. Watch mouse cursor on target computer
3. It should move slightly every 2 seconds
4. Pro Micro LED should blink with each movement

**Test 5: Quick Scripts**
1. Select your operating system from the OS dropdown
2. Click one of the quick script buttons (e.g., "Open Editor")
3. The corresponding application should launch on your computer

![Quick Scripts](resource/QuickScripts.png)

**Activity Log**

As you perform these tests, you'll see all actions logged in real-time at the bottom of the interface:

![Activity Log](resource/ActivityLog.png)

The activity log shows timestamps and descriptions of all commands sent, making it easy to track and debug your actions.

**If all tests pass:** 🎉 Your system is working perfectly!

---

## 7. Troubleshooting

### Problem: Can't upload to Pro Micro

**Solutions:**
1. **Driver issue:**
   - Install SparkFun Pro Micro drivers
   - On Linux: `sudo usermod -a -G dialout $USER` then logout/login

2. **Bootloader mode:**
   - Press reset button twice quickly
   - Upload immediately when port appears
   - You have ~8 seconds window

3. **Wrong board selected:**
   - Ensure "Arduino Leonardo" is selected
   - Not "Arduino Micro" or "Arduino Pro"

### Problem: Can't upload to NodeMCU

**Solutions:**
1. **Driver issue (Windows):**
   - Install CH340 or CP2102 USB drivers
   - Check Device Manager for COM port

2. **Wrong board:**
   - Select "NodeMCU 1.0 (ESP-12E Module)"
   - Check upload speed is 115200

3. **Upload timeout:**
   - Try different USB cable
   - Try different USB port
   - Lower upload speed to 57600

### Problem: NodeMCU won't connect to WiFi

**Check:**
1. **Network compatibility:**
   - ESP8266 only supports 2.4GHz WiFi
   - 5GHz networks won't work

2. **Credentials:**
   - Check SSID spelling (case-sensitive)
   - Verify password is correct
   - Special characters might cause issues

3. **Router settings:**
   - Ensure SSID broadcast is enabled
   - Check if MAC filtering is blocking device
   - Try disabling AP isolation

**Solution:**
- Connect to AP mode (USB-HID-Setup)
- Reconfigure WiFi settings
- Try different network if available

### Problem: No communication between devices

**Check wiring:**
1. **TX/RX crossed:**
   - NodeMCU TX → Pro Micro RX (Pin 0)
   - NodeMCU RX → Pro Micro TX (Pin 1)
   - NOT straight through!

2. **GND connected:**
   - This is critical - verify ground connection
   - Try different wire if needed

3. **Firm connections:**
   - Check all wires are fully inserted
   - Try different jumper wires

**Test:**
1. Open Serial Monitor on Pro Micro port (115200)
2. You should see "READY" message
3. If not, check Pro Micro code uploaded correctly

### Problem: Commands not working on computer

**Check:**
1. **Pro Micro connected:**
   - Must be connected via USB to target computer
   - Should appear as HID device

2. **Application focus:**
   - Click on target application first
   - Cursor must be where you want text to appear

3. **OS compatibility:**
   - Some commands are OS-specific
   - Select correct OS in dropdown

4. **Keyboard layout:**
   - System uses US keyboard layout
   - Special characters might differ on other layouts

### Problem: Mouse Jiggler not working

**Check:**
1. **Pro Micro receiving commands:**
   - Check Serial Monitor for "OK:Jiggler enabled"

2. **LED blinking:**
   - LED should blink every 2 seconds when active
   - If not, Pro Micro isn't running jiggler

3. **Mouse movement:**
   - Movement is small (±2 pixels)
   - May be hard to see on high-res displays
   - Try on lower resolution display

### Problem: Web interface not loading

**Check:**
1. **Same network:**
   - Both phone/computer and NodeMCU must be on same WiFi
   - Check WiFi connection

2. **Correct IP:**
   - Verify IP address from Serial Monitor
   - Check router for device IP

3. **Browser:**
   - Try different browser
   - Clear cache
   - Try incognito/private mode

4. **Firewall:**
   - Some firewalls block local network access
   - Try disabling temporarily

### Problem: DuckyScript not executing

**Check:**
1. **Syntax:**
   - Verify script syntax is correct
   - Check for typos in commands

2. **Delays:**
   - Add delays between commands
   - Some apps need time to respond

3. **Focus:**
   - Ensure target application has focus
   - Script might be typing to wrong window

**Example working script:**
```
DELAY 500
GUI r
DELAY 500
STRING notepad
ENTER
DELAY 1000
STRING Hello World!
```

### Getting More Help

**Serial Monitor Output:**
- Pro Micro: Check for "READY" and command responses
- NodeMCU: Check for WiFi status and IP address

**Check Connections:**
- Re-verify wiring matches [WIRING.md](WIRING.md)
- Use continuity tester on jumper wires

**Test Components:**
- Upload simple blink sketch to each board separately
- Verify each board works independently

**Activity Log:**
- Check web interface activity log for errors
- Shows what commands were sent

---

## Quick Reference

### Upload Settings Summary

**Pro Micro:**
- Board: Arduino Leonardo
- Port: Auto-detect
- Baud: 115200 (Serial Monitor)

**NodeMCU:**
- Board: NodeMCU 1.0 (ESP-12E Module)
- Upload Speed: 115200
- Flash Size: 4MB (FS:2MB OTA:~1019KB)
- Baud: 115200 (Serial Monitor)

### Default WiFi Credentials

- SSID: `USB-HID-Setup`
- Password: `12345678`
- IP: `192.168.4.1`

### Pin Connections

- NodeMCU TX → Pro Micro Pin 0 (RX)
- NodeMCU RX → Pro Micro Pin 1 (TX)
- NodeMCU GND → Pro Micro GND

---

## Next Steps

Once setup is complete:

1. **Explore features** - Try all buttons and functions
2. **Create scripts** - Write DuckyScript for automation
3. **Read API docs** - See [API.md](API.md) for programmatic control
4. **Customize** - Modify code for your specific needs

**Enjoy your WiFi USB HID Control system!** 🎉
