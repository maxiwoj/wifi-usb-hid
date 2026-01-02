# SPIFFS Setup Guide

This guide explains how to upload the web interface files to SPIFFS (SPI Flash File System) on the ESP8266.

## Why SPIFFS?

SPIFFS is used to store the HTML, CSS, and JavaScript files for the web interface, as well as any saved DuckyScripts. Without uploading the data to SPIFFS, the web interface will not load.

## Prerequisites

- Arduino IDE 1.8+ or Arduino IDE 2.0+
- ESP8266 board support installed
- [ESP8266 Sketch Data Upload tool](https://github.com/esp8266/arduino-esp8266fs-plugin) installed.
- NodeMCU connected via USB

## Step 1: Upload the Filesystem

1.  **Select the correct board:**
    *   Go to **Tools → Board**
    *   Select **"NodeMCU 1.0 (ESP-12E Module)"** or your specific ESP8266 board

2.  **Select the correct port:**
    *   Go to **Tools → Port**
    *   Select the COM port where your NodeMCU is connected

3.  **Select SPIFFS size:**
    *   Go to **Tools → Flash Size**
    *   Select **"4MB (FS:2MB OTA:~1019KB)"** or **"4MB (FS:3MB OTA:~512KB)"** depending on your board. This determines how much space is available for SPIFFS.

4.  **Upload the filesystem:**
    *   Go to **Tools → ESP8266 Sketch Data Upload**
    *   This will upload the contents of the `nodemcu/data` directory to the ESP8266's SPIFFS.
    *   Wait for the upload to complete. You'll see "SPIFFS Image Uploaded" when done.

## Step 2: Verify SPIFFS is Working

1.  Upload your sketch (`nodemcu.ino`) to the NodeMCU.
2.  Open **Tools → Serial Monitor** (set baud rate to 115200).
3.  Look for the message: `"SPIFFS mounted successfully"`. If you see this, SPIFFS is working correctly.

## Troubleshooting

### "SPIFFS initialization failed"

If you see this message, it means the SPIFFS upload failed or the filesystem is corrupt.

*   **Solution:** Run **Tools → ESP8266 Sketch Data Upload** again.
*   **Check Flash Size:** Ensure you have selected the correct flash size for your board.

### "ESP8266 Sketch Data Upload" option is missing

*   **Solution:** Make sure you have installed the ESP8266 filesystem uploader tool. You can find it [here](https://github.com/esp8266/arduino-esp8266fs-plugin).

## What Gets Stored in SPIFFS?

*   `index.html`: The main web interface page.
*   `setup.html`: The WiFi setup page.
*   `style.css`: The stylesheet for the web interface.
*   `script.js`: The JavaScript for the web interface.
*   Saved DuckyScripts (one file per script, e.g., `/scripts_MyScript.txt`).

**Note:** You must upload the filesystem data every time you make changes to the HTML, CSS, or JavaScript files in the `data` directory.