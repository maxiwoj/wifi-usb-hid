# WiFi USB HID Control - ESP32-S3 with ST7735 LCD

![Logo](../docs/resource/logo.bmp)

This is an all-in-one implementation of the WiFi USB HID Control project for **ESP32-S3 boards** with a built-in 
**0.96" ST7735 IPS LCD display**. This single board combines all features that previously required two separate boards (Pro Micro + NodeMCU).

## Hardware Specifications

![Lilygo T-Dongle-S3](../docs/resource/lilygo-t-dongle.jpeg)
*Lilygo T-Dongle-S3*

- **Board**: ESP32-S3 Dongle / T-Dongle-S3
- **Chip**: ESP32-S3R8 (with 8MB PSRAM)
- **Display**: 0.96" IPS LCD with ST7735 controller
- **Resolution**: 80x160 pixels (portrait) or 160x80 (landscape)
- **Interface**: SPI (for display)
- **USB**: Native USB OTG with HID support

### Compatible Boards
- [LILYGO T-Dongle-S3](https://lilygo.cc/en-us/products/t-dongle-s3) (supports microSD card for script storage)
- ESP32-S3 Development Board with ST7735 LCD
- Other ESP32-S3 boards with 0.96" ST7735 display (may require pin adjustments)

### SD Card Support
The LILYGO T-Dongle-S3 includes a microSD card slot. This project supports loading scripts directly from the SD card. 
If an SD card is present, the device will prioritize files on the SD card over the internal LittleFS storage. 
You can simply copy your `.txt` scripts to the root of the SD card.

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
- **Adafruit ST7735 and ST7789 Library** (by Adafruit) - *Used by some sub-tests*

#### LilyGO T-Dongle-S3 Specific Libraries:
For the best compatibility with the T-Dongle-S3, it is recommended to use the libraries provided by LilyGO:
1. Download the libraries from the [LilyGO T-Dongle-S3 Repository](https://github.com/Xinyuan-LilyGO/T-Dongle-S3) (check the `lib` folder).
2. Copy these libraries to your Arduino sketchbook `libraries` folder (e.g., `Documents/Arduino/libraries/`).

#### TFT_eSPI Configuration for T-Dongle-S3:
If you use the standard `TFT_eSPI` library, you **must** configure the `User_Setup.h` file in the library folder:
1. Locate `TFT_eSPI/User_Setup.h` in your Arduino libraries folder.
2. Replace its content with the configuration provided in this project's `User_Setup.h`.

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
2. Upload web files to LittleFS (see detailed instructions below)

## Uploading LittleFS Data (Web Interface Files)

The web interface files (HTML, CSS, JS) must be uploaded to the ESP32-S3's LittleFS filesystem for the web interface to function.

For detailed, step-by-step instructions on how to install the LittleFS upload plugin and upload your data, please refer to the **[LittleFS Setup Guide](../docs/LITTLEFS_SETUP.md)** in the main project directory.

## Re-uploading Sketches (Bootloader Mode)

**Important:** Once the sketch is running, the ESP32-S3 presents itself as a USB HID device (keyboard/mouse) instead of a serial port. To upload new code, you must enter bootloader mode.

### Method 1: Using BOOT + RESET Buttons
1. Hold the **BOOT** button
2. Press and release the **RESET** button (while still holding BOOT)
3. Release the **BOOT** button
4. The device should now appear as a serial port in Arduino IDE

### Method 2: Using BOOT Button Only (No RESET Button)
1. Unplug the device from USB
2. Hold the **BOOT** button
3. Plug the device back in (while holding BOOT)
4. Release the **BOOT** button after 1-2 seconds
5. The device should now appear as a serial port

### After Uploading
Press the **RESET** button (or unplug/replug) to exit bootloader mode and run your sketch.

## Troubleshooting Upload Issues

### "Resource busy" or "Could not open port" Error

This occurs when another process is using the serial port.

**Solutions:**
1. **Close the Serial Monitor** in Arduino IDE
2. **Close other serial applications** (terminal apps, other IDEs, etc.)
3. **Find and kill the blocking process:**
   ```bash
   lsof /dev/cu.usbmodem*
   kill -9 <PID>
   ```
4. **If still busy, restart:**
   - Unplug the device
   - Close Arduino IDE completely
   - Reopen Arduino IDE
   - Enter bootloader mode and plug in
   - Upload immediately

## Configuration

All project settings are located in the `config.h` file. You can adjust the following parameters there:

- **Display Pins**: Verify and adjust the `TFT_*` pin definitions to match your specific hardware.
- **Display Rotation**: Change `DISPLAY_ROTATION` if the screen orientation is incorrect.
- **WiFi Credentials**: Default AP SSID and password.
- **Web Authentication**: Default login credentials for the web interface.
- **SD Card Pins**: Configuration for the microSD card slot (pre-configured for T-Dongle-S3).

Always check `config.h` before compiling to ensure the settings match your hardware.

## Troubleshooting

### Device Not Appearing in Arduino IDE
- The device is likely in HID mode. Enter bootloader mode (see "Re-uploading Sketches" above)
- On T-Dongle-S3, the BOOT button is typically on the side or bottom of the board

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
