# LittleFS Setup Guide

This guide explains how to upload the web interface files to LittleFS on the ESP8266.

## Why LittleFS?

LittleFS is used to store the HTML, CSS, and JavaScript files for the web interface, as well as any saved DuckyScripts. Without uploading the data to LittleFS, the web interface will not load.

## Prerequisites

- Arduino IDE 1.8+ or Arduino IDE 2.0+
- ESP8266 board support installed
- [ESP8266 LittleFS Data Upload tool](https://github.com/earlephilhower/arduino-littlefs-upload?tab=readme-ov-file) installed.
  - To install, copy the VSIX file to `~/.arduinoIDE/plugins/` on Mac and Linux or `C:\Users\<username>\.arduinoIDE\plugins\` on Windows (you may need to make this directory yourself beforehand). Restart the IDE.
- NodeMCU connected via USB

**Important:** The Arduino IDE does not currently support writing to the flash file system out of the box. You must install the LittleFS uploader plugin from the link above.

For more information, see the following forum posts:
- https://forum.arduino.cc/t/esp8266-spiffs-fs-uploader-tool-now-showing/1163746
- https://forum.arduino.cc/t/installing-esp8266-esp32-sketch-data-upload-tool/1248719

## Step 1: Upload the Filesystem
`[⌘]` + `[Shift]` + `[P]`, then `"Upload LittleFS to Pico/ESP8266/ESP32"`.

## Step 2: Verify LittleFS is Working

1.  Upload your sketch (`nodemcu.ino`) to the NodeMCU.
2.  Open **Tools → Serial Monitor** (set baud rate to 74880).
3.  Look for the message: `"LittleFS mounted successfully"`. If you see this, LittleFS is working correctly.
4.  Verify that the html files were uploaded correctly, the message should at least contain:
```txt
LittleFS contents:
index.html
script.js
setup.html
style.css
```

## Troubleshooting

### "LittleFS initialization failed"

If you see this message, it means the LittleFS upload failed or the filesystem is corrupt.

*   **Solution:** Run **Tools → ESP8266 LittleFS Data Upload** again.
*   **Check Flash Size:** Ensure you have selected the correct flash size for your board.

### "ESP8266 LittleFS Data Upload" option is missing

*   **Solution:** Make sure you have installed the ESP8266 filesystem uploader tool. You can find it [here](https://github.com/earlephilhower/arduino-littlefs-upload?tab=readme-ov-file).

## What Gets Stored in LittleFS?

*   `index.html`: The main web interface page.
*   `setup.html`: The WiFi setup page.
*   `style.css`: The stylesheet for the web interface.
*   `script.js`: The JavaScript for the web interface.
*   Saved DuckyScripts (one file per script, e.g., `/scripts_MyScript.txt`).

**Note:** You must upload the filesystem data every time you make changes to the HTML, CSS, or JavaScript files in the `data` directory.
