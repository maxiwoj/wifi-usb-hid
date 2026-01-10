# Features

### Type Text
![Type Text Feature](resource/typeText.png)

Type text remotely and send keyboard shortcuts to the target computer.

### Mouse Control with Live Trackpad
![Mouse Control](resource/mouseControl.png)

Control mouse movement with directional buttons or use the live trackpad with adjustable sensitivity (0.5x - 10x) for smooth, intuitive control on desktop and mobile devices. Supports single-click, double-click gestures for highlighting text, and right-click for context menus.

### Quick Actions
Customizable one-click keyboard shortcuts for each operating system. Create, edit, and manage quick action buttons through the dedicated management interface at `/manage-actions.html`.

**Features:**
- Per-OS quick actions (Windows, macOS, Linux, and custom OS)
- DuckyScript command support (e.g., `GUI_R`, `CTRL_C`, `ALT_TAB`)
- Custom button labels and descriptions
- Five color schemes (Primary, Success, Warning, Info, Danger)
- Drag-and-drop reordering with up/down buttons
- Persistent storage across reboots

![Manage Quick Actions](resource/manageQuickActions.png)

### Quick Scripts
![Quick Scripts](resource/QuickScripts.png)

Create and manage custom DuckyScript automation buttons for each operating system. Access the full script editor at `/manage-scripts.html` to build complex automation workflows.

**Features:**
- OS-specific quick scripts for Windows, macOS, Linux, and custom OS
- Full DuckyScript editor with multi-line support
- Script ID system for updating existing scripts
- Custom button labels and color schemes
- Built-in example scripts for common tasks
- Persistent storage in LittleFS

![Manage Quick Scripts](resource/manageQuickScripts.png)

### Activity Log
![Activity Log](resource/ActivityLog.png)

Real-time activity logging shows all commands and actions with timestamps.

### Custom Operating Systems
![Manage Operating Systems](resource/manageOS.png)

Extend beyond Windows, macOS, and Linux by creating custom OS categories with their own quick actions and scripts. Perfect for specialized systems like Android, ChromeOS, Raspberry Pi, or any custom environment.

**Features:**
- Add unlimited custom OS categories
- Each custom OS has dedicated quick actions and scripts
- OS selection persists across page navigation
- Quick access to manage actions for any OS
- Delete custom OS when no longer needed

### WiFi Management
![WiFi Configuration](resource/wifiConfiguration.png)

Built-in WiFi manager with network scanner. Device starts in AP mode (SSID: "USB-HID-Setup") and can be configured to connect to your home/office network. Auto-reconnects on startup with automatic fallback to AP mode if connection fails.

### Additional Features

- **Web Authentication** - HTTP Basic Authentication protects all endpoints (default: admin/WiFi_HID!826)
- **HTTPS Support** - Secure communication with self-signed certificate (HTTP + HTTPS on ports 80 and 443)
- **Script Storage** - Save and load DuckyScripts to/from LittleFS for reuse
- **Advanced Mouse Jiggler** - Configurable automatic mouse movement with three patterns (Simple, Circles, Random), adjustable diameter (1-50px), and delay settings (100-60000ms) to prevent screen lock
- **REST API** - Programmatic control via HTTP/HTTPS endpoints
- **Optional OLED Display** - 128x64 display shows status, IP address, and web credentials
