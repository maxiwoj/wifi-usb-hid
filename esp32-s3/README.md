# WiFi USB HID Control - ESP32-S3 with ST7735 LCD

This is an all-in-one implementation of the WiFi USB HID Control project for **ESP32-S3 boards** with a built-in **0.96" ST7735 IPS LCD display**. This single board combines all features that previously required two separate boards (Pro Micro + NodeMCU).

## Hardware Specifications

- **Board**: ESP32-S3 Dongle (Płytka rozwojowa ESP32 S3)
- **Chip**: ESP32-S3R8 (with 8MB PSRAM)
- **Display**: 0.96" IPS LCD with ST7735 controller
- **Resolution**: 80x160 pixels (portrait) or 160x80 (landscape)
- **Interface**: SPI (for display)
- **USB**: Native USB OTG with HID support

## Features

- ✅ Native USB HID support (keyboard and mouse emulation)
- ✅ WiFi web server with REST API
- ✅ Color IPS LCD display (ST7735)
- ✅ DuckyScript parser
- ✅ Mouse Jiggler with multiple patterns
- ✅ Quick Actions and Scripts
- ✅ File Management
- ✅ Access Point and Station modes
- ✅ Web-based configuration

## Key Differences from ESP32-C3 Version

| Feature | ESP32-C3 | ESP32-S3 |
|---------|----------|----------|
| USB HID | ❌ Not supported | ✅ **Full support** |
| Display Type | OLED (SSD1306, I2C) | **IPS LCD (ST7735, SPI)** |
| Display Size | 0.42" (72x40) | **0.96" (80x160)** |
| Colors | Monochrome | **Full color (65K)** |
| PSRAM | No | **8MB PSRAM** |
| All-in-one | No (needs Pro Micro) | **Yes** ✅ |

## Software Setup

### Step 1: Install Arduino IDE

Download and install the latest Arduino IDE from [arduino.cc](https://www.arduino.cc/en/software).

### Step 2: Install ESP32 Board Support

1. Open Arduino IDE
2. Go to **File > Preferences**
3. Add to **Additional Board Manager URLs**:
   ```
   https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
   ```
4. Go to **Tools > Board > Boards Manager**
5. Search for "esp32" and install **ESP32 by Espressif Systems** (version 2.0.14 or newer)

### Step 3: Install Required Libraries

Install these libraries via **Sketch > Include Library > Manage Libraries**:

#### Required Libraries:
- **Adafruit GFX Library** (by Adafruit)
- **Adafruit ST7735 and ST7789 Library** (by Adafruit)

#### Built-in Libraries (included with ESP32 core):
- WiFi
- WebServer
- Preferences
- LittleFS
- USB (for HID)

### Step 4: Configure the Board

1. **Select Board:**
   - Go to **Tools > Board > esp32**
   - Select **ESP32S3 Dev Module**

2. **Configure USB Settings (CRITICAL FOR HID):**
   - **USB CDC On Boot**: **Enabled** ⚠️ REQUIRED
   - **Upload Mode**: **UART0 / Hardware CDC**
   - **USB Mode** (if visible): **Hardware CDC and JTAG**

3. **Other Settings:**
   - **PSRAM**: **OPI PSRAM** (your board has 8MB)
   - **Flash Size**: 4MB or 8MB (match your board)
   - **Partition Scheme**: **Default 4MB with spiffs**
   - **CPU Frequency**: 240MHz

4. Select the correct **Port**

### Step 5: Configure Display Pins

**IMPORTANT**: The default pin configuration in `config.h` may need adjustment for your specific board.

Open `config.h` and verify/adjust these pins based on your board's documentation.

### Step 6: Upload Sketch and LittleFS

1. Upload the sketch: **Sketch > Upload**
2. Upload web files: **Tools > ESP32 Sketch Data Upload** (requires ESP32FS plugin)

## Configuration

The main settings to adjust in `config.h`:

```cpp
// Display pins - VERIFY FOR YOUR BOARD!
#define TFT_CS    10
#define TFT_RST   14
#define TFT_DC    13
#define TFT_MOSI  11
#define TFT_SCLK  12
#define TFT_BL    9

// Display rotation
#define DISPLAY_ROTATION 1  // 0=0°, 1=90°, 2=180°, 3=270°
```

## Troubleshooting

### Display Issues
- **Blank/garbled**: Check pins in `config.h`, try different `initR()` method in `display_manager.cpp`
- **Wrong orientation**: Adjust `DISPLAY_ROTATION` in `config.h`
- **No backlight**: Verify `TFT_BL` pin

### USB HID Not Working
- **Enable "USB CDC On Boot"** in Tools menu (most common issue)
- Reconnect USB cable after upload
- Check Serial Monitor for "USB HID initialized successfully"

See full README for complete troubleshooting guide.

## First Use

1. Device creates AP: `USB-HID-Setup` (password: `HID_M4ster`)
2. Connect and go to `http://192.168.4.1`
3. Login: `admin` / `WiFi_HID!826`
4. Configure your WiFi or use directly

## Support

For detailed documentation, troubleshooting, and support, see the main project repository.
