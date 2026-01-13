# WiFi USB HID Control - ESP32-C3 with Built-in OLED

This is an all-in-one implementation of the WiFi USB HID Control project for ESP32-C3 boards with a built-in 0.42" OLED display. This single board combines all the features that previously required two separate boards (Pro Micro + NodeMCU).

## Features

- ✅ Native USB HID support (keyboard and mouse emulation)
- ✅ WiFi web server with REST API
- ✅ Built-in 0.42" OLED display (72x40 pixels)
- ✅ DuckyScript parser
- ✅ Mouse Jiggler with multiple patterns
- ✅ Quick Actions and Scripts
- ✅ File Management
- ✅ Access Point and Station modes
- ✅ Web-based configuration

## Hardware Requirements

- **ESP32-C3 board with built-in 0.42" OLED display**
  - The board should have an integrated I2C OLED display (typically 72x40 pixels)
  - ESP32-C3 with native USB support
- **USB cable** (USB-C or Micro-USB depending on your board)

## Board Configuration

### OLED Display Pins

The OLED display is typically connected via I2C. **Check your board's documentation** for the exact pin configuration. Common configurations:

- **Option 1** (default in config.h):
  - SDA: GPIO8
  - SCL: GPIO9

- **Option 2** (alternative):
  - SDA: GPIO5
  - SCL: GPIO6

To change the pins, edit `config.h`:

```cpp
#define OLED_SDA 8  // Change to your board's SDA pin
#define OLED_SCL 9  // Change to your board's SCL pin
```

### OLED Display Specifications

- **Resolution**: 72x40 pixels (typical for 0.42" OLED)
- **Driver**: SSD1306
- **I2C Address**: 0x3C (or 0x3D as fallback)

**Note**: Some 0.42" displays might have different resolutions. If your display doesn't work, check the specs and update `SCREEN_WIDTH` and `SCREEN_HEIGHT` in `config.h`.

## Software Setup

### 1. Install Arduino IDE

Download and install the latest Arduino IDE from [arduino.cc](https://www.arduino.cc/en/software).

### 2. Install ESP32 Board Support

1. Open Arduino IDE
2. Go to **File > Preferences**
3. Add to **Additional Board Manager URLs**:
   ```
   https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
   ```
4. Go to **Tools > Board > Boards Manager**
5. Search for "esp32" and install **ESP32 by Espressif Systems**

### 3. Install Required Libraries

Install these libraries via **Sketch > Include Library > Manage Libraries**:

- **Adafruit GFX Library** (by Adafruit)
- **Adafruit SSD1306** (by Adafruit)
- **ArduinoJson** (by Benoit Blanchon) - if needed for JSON parsing

The following libraries are included with ESP32 board support:
- WiFi
- WebServer
- Preferences
- LittleFS
- USB (for HID)

### 4. Configure the Board

1. Select your board:
   - Go to **Tools > Board > ESP32 Arduino**
   - Select **ESP32C3 Dev Module** (or your specific board model)

2. Configure USB settings:
   - **USB CDC On Boot**: Enabled
   - **USB Mode**: USB-OTG (TinyUSB)
   - **Upload Mode**: UART0 / Hardware CDC (depending on your board)

3. Select the correct **Port** (COM port on Windows, /dev/tty* on Linux/Mac)

### 5. Upload the Sketch

1. Open `esp32-c3.ino` in Arduino IDE
2. Click **Upload** (or press Ctrl+U)
3. Wait for the upload to complete

### 6. Upload the Web Interface Files (LittleFS)

The web interface files need to be uploaded to the ESP32-C3's flash memory using LittleFS.

#### Install LittleFS Upload Tool:

1. Download the **ESP32 LittleFS Uploader** plugin:
   - [ESP32FS Plugin releases](https://github.com/lorol/arduino-esp32fs-plugin/releases)

2. Extract and copy the `ESP32FS` folder to:
   - **Windows**: `C:\Users\<username>\Documents\Arduino\tools\`
   - **Linux**: `~/Arduino/tools/`
   - **macOS**: `~/Documents/Arduino/tools/`

3. Restart Arduino IDE

4. With the sketch open, go to **Tools > ESP32 Sketch Data Upload**

5. Wait for the upload to complete

## First Time Setup

### 1. Connect to the Access Point

After uploading the firmware, the ESP32-C3 will create a WiFi access point:

- **SSID**: `USB-HID-Setup`
- **Password**: `HID_M4ster`
- **IP Address**: `192.168.4.1`

The OLED display will show these credentials.

### 2. Configure WiFi (Optional)

1. Connect to the AP and open a browser
2. Navigate to `http://192.168.4.1/setup`
3. Login with default credentials:
   - **Username**: `admin`
   - **Password**: `WiFi_HID!826`
4. Enter your WiFi network credentials
5. Click **Save and Connect**

The device will attempt to connect to your WiFi network. If successful, it will display the new IP address on the OLED. If it fails, it will fall back to AP mode.

### 3. Access the Web Interface

Open your browser and navigate to:
- **AP Mode**: `http://192.168.4.1`
- **Station Mode**: `http://<device-ip>` (shown on OLED or in serial monitor)

## Usage

### Web Interface

The main interface provides:
- **OS Selection**: Choose between Windows, macOS, Linux, or custom OS profiles
- **Mouse Trackpad**: Control the mouse pointer from your phone/tablet
- **Quick Actions**: One-click buttons for common tasks
- **Mouse Jiggler**: Keep your computer active with automatic mouse movement
- **DuckyScript Editor**: Write and execute keyboard automation scripts
- **File Manager**: Upload, download, and manage files on the device

### REST API

The device exposes a REST API for programmatic control. See the main project documentation for API details.

### Serial Monitor

Open the Serial Monitor (115200 baud) to see:
- Boot messages
- WiFi connection status
- IP address
- Command execution logs
- Debug information

## Configuration

Edit `config.h` to customize:

```cpp
// WiFi AP settings
#define AP_SSID "USB-HID-Setup"
#define AP_PASS "HID_M4ster"

// Web authentication
#define WEB_AUTH_USER "admin"
#define WEB_AUTH_PASS "WiFi_HID!826"

// OLED display pins (adjust for your board!)
#define OLED_SDA 8
#define OLED_SCL 9

// Display resolution (adjust if needed)
#define SCREEN_WIDTH 72
#define SCREEN_HEIGHT 40
```

## Troubleshooting

### Display Not Working

1. **Check I2C pins**: Verify SDA and SCL pins in `config.h` match your board
2. **Check I2C address**: Open Serial Monitor and look for I2C scan results
3. **Check connections**: Ensure display VCC is connected to 3.3V (NOT 5V)
4. **Try alternative pins**: Some boards use GPIO5/GPIO6 instead of GPIO8/GPIO9

### USB HID Not Working

1. **Check USB Mode**: Ensure "USB Mode" is set to "USB-OTG (TinyUSB)" in Arduino IDE
2. **Check CDC On Boot**: Enable "USB CDC On Boot" in Tools menu
3. **Try different cable**: Some USB cables are charge-only and don't support data
4. **Reconnect**: Unplug and replug the USB cable after upload

### WiFi Connection Issues

1. **Check credentials**: Verify SSID and password in setup page
2. **Signal strength**: Ensure the device is within range of your WiFi
3. **Fallback to AP**: If connection fails, device returns to AP mode automatically
4. **Reset WiFi**: Clear saved credentials by uploading a blank EEPROM sketch

### Web Interface Not Loading

1. **Verify LittleFS upload**: Ensure web files were uploaded using ESP32 Sketch Data Upload
2. **Check IP address**: Verify IP from OLED display or Serial Monitor
3. **Browser cache**: Try clearing browser cache or use incognito mode
4. **Authentication**: Remember to use admin/WiFi_HID!826 for login

## Differences from Two-Board Setup

This ESP32-C3 version combines features from both boards:

| Feature | Pro Micro + NodeMCU | ESP32-C3 |
|---------|---------------------|----------|
| USB HID | ATmega32U4 (Pro Micro) | ESP32-C3 native USB |
| WiFi/Web | ESP8266 (NodeMCU) | ESP32-C3 WiFi |
| Display | 0.96" (128x64) | 0.42" (72x40) |
| Boards Required | 2 | 1 |
| Serial Communication | Required between boards | Not needed |
| Power | 2 USB connections or shared power | Single USB connection |

### Advantages of ESP32-C3

- ✅ **Simpler hardware**: Only one board required
- ✅ **No serial wiring**: HID commands are internal
- ✅ **More compact**: Smaller footprint
- ✅ **Lower power**: Single board consumes less
- ✅ **Built-in display**: No external wiring needed
- ✅ **More memory**: ESP32-C3 has more RAM and flash
- ✅ **Faster processor**: RISC-V 160MHz vs 80MHz/16MHz

### Display Size Considerations

The 0.42" OLED has less screen space (72x40 vs 128x64), so the display output is more compact:
- Abbreviated status information
- Truncated long text
- Essential info only

All functionality remains the same - only the visual display is adapted for the smaller screen.

## Security Notes

- **Change default passwords**: Update `WEB_AUTH_USER` and `WEB_AUTH_PASS` in `config.h`
- **Use HTTPS**: Enable HTTPS in production (requires certificate setup)
- **Secure your WiFi**: Use a strong password for both AP and Station mode
- **Physical security**: This device can control any connected computer - keep it secure

## License

This project inherits the license from the main WiFi USB HID Control project.

## Support

For issues, questions, or contributions, see the main project repository.

## Additional Resources

- [ESP32-C3 Datasheet](https://www.espressif.com/sites/default/files/documentation/esp32-c3_datasheet_en.pdf)
- [Arduino-ESP32 Documentation](https://docs.espressif.com/projects/arduino-esp32/en/latest/)
- [DuckyScript Documentation](https://docs.hak5.org/hak5-usb-rubber-ducky/duckyscript-tm-quick-reference)
- Main project documentation in `/docs` folder
