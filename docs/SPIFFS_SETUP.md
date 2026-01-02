# SPIFFS Setup Guide

This guide explains how to upload and initialize SPIFFS (SPI Flash File System) for script storage on the ESP8266.

## Why SPIFFS?

SPIFFS is used to store saved DuckyScripts permanently on the ESP8266. Without SPIFFS, script storage features will not work, but the rest of the system will function normally.

## Prerequisites

- Arduino IDE 1.8+ or Arduino IDE 2.0+
- ESP8266 board support installed
- NodeMCU connected via USB

## Step 1: Install ESP8266 Filesystem Uploader

### For Arduino IDE 1.8.x:

1. Open Arduino IDE
2. Go to **Tools → Board → Boards Manager**
3. Search for "esp8266"
4. Install "esp8266 by ESP8266 Community" (if not already installed)
5. The filesystem uploader tool is included with the ESP8266 board package

### For Arduino IDE 2.0+:

The filesystem uploader is included with ESP8266 board support. No additional installation needed.

## Step 2: Create Data Directory (Optional)

For this project, you don't need to create any initial files. The system will create script files automatically when you save scripts. However, if you want to initialize an empty SPIFFS:

1. In your project directory (`wifi-usb-hid/nodemcu/`), create a folder named `data`
2. The folder can be empty - we just need to initialize the filesystem

**Note:** This step is optional. The code will work even without initial files.

## Step 3: Upload SPIFFS Filesystem

1. **Select the correct board:**
   - Go to **Tools → Board**
   - Select **"NodeMCU 1.0 (ESP-12E Module)"** or your specific ESP8266 board

2. **Select the correct port:**
   - Go to **Tools → Port**
   - Select the COM port where your NodeMCU is connected

3. **Select SPIFFS size:**
   - Go to **Tools → Flash Size**
   - Select **"4MB (FS:2MB OTA:~1019KB)"** or **"4MB (FS:3MB OTA:~512KB)"** depending on your board
   - This determines how much space is available for SPIFFS

4. **Upload the filesystem:**
   - Go to **Tools → ESP8266 Sketch Data Upload**
   - Wait for the upload to complete (may take 10-30 seconds)
   - You'll see "SPIFFS Image Uploaded" when done

## Step 4: Verify SPIFFS is Working

1. Upload your sketch (`nodemcu.ino`) to the NodeMCU
2. Open **Tools → Serial Monitor** (set baud rate to 115200)
3. Look for one of these messages:
   - ✅ `"SPIFFS mounted successfully"` - SPIFFS is working!
   - ⚠️ `"SPIFFS initialization failed"` - SPIFFS upload may have failed

## Troubleshooting

### "SPIFFS initialization failed"

**Possible causes:**
1. SPIFFS filesystem was not uploaded
   - **Solution:** Run **Tools → ESP8266 Sketch Data Upload** again
   
2. Wrong flash size selected
   - **Solution:** Check your board's flash size and select the correct option in **Tools → Flash Size**

3. Board doesn't have enough flash memory
   - **Solution:** Use a board with at least 4MB flash, or reduce SPIFFS size

### "ESP8266 Sketch Data Upload" option is missing

**Possible causes:**
1. ESP8266 board support not installed
   - **Solution:** Install ESP8266 board support via Board Manager

2. Wrong board selected
   - **Solution:** Make sure you've selected an ESP8266 board (not Arduino board)

3. Arduino IDE version too old
   - **Solution:** Update to Arduino IDE 1.8.13+ or Arduino IDE 2.0+

### Upload fails or takes too long

1. **Check USB connection** - Try a different USB cable or port
2. **Check drivers** - Make sure CP2102 or CH340 drivers are installed
3. **Try holding BOOT button** - Some boards require holding BOOT during upload
4. **Reduce SPIFFS size** - Try a smaller filesystem size

## Alternative: Format SPIFFS via Code

If you can't use the upload tool, you can format SPIFFS programmatically by temporarily adding this to `setup()`:

```cpp
// TEMPORARY: Format SPIFFS (remove after first run)
if (!SPIFFS.begin()) {
  Serial.println("Formatting SPIFFS...");
  SPIFFS.format();
  SPIFFS.begin();
}
```

**⚠️ Warning:** Remove this code after the first successful run, as formatting erases all files!

## SPIFFS Size Recommendations

- **Minimum:** 64KB (for a few small scripts)
- **Recommended:** 1MB (for many scripts)
- **Maximum:** Depends on your board's flash size (usually 2-3MB on 4MB boards)

## What Gets Stored in SPIFFS?

- Saved DuckyScripts (one file per script)
- File naming: `/scripts_<script_name>.txt`
- Scripts persist across reboots
- Scripts can be loaded, saved, and deleted via the web interface

## Notes

- SPIFFS upload is **separate** from sketch upload
- You need to upload SPIFFS **before** or **after** uploading your sketch
- SPIFFS files persist even when you upload a new sketch (unless you reformat)
- If you change SPIFFS size, you'll need to re-upload the filesystem

---

**Need help?** Check the Serial Monitor output for detailed error messages.


