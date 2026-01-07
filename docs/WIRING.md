# Hardware Wiring Guide

Connect NodeMCU (ESP8266) and Pro Micro (ATmega32U4) via serial communication. NodeMCU is powered by Pro Micro - only ONE USB cable needed.

![Connected Hardware Example](resource/connected_picture.jpeg)

## Connection Table

| NodeMCU Pin | Pro Micro Pin | Wire Color    | Description       |
|-------------|---------------|---------------|-------------------|
| Vin         | VCC           | Red           | Power (5V)        |
| GND         | GND           | Black         | Common ground     |
| TX          | RX (Pin 0)    | Yellow/White  | Serial TX→RX      |
| RX          | TX (Pin 1)    | Orange/Green  | Serial RX→TX      |

**Important:** TX/RX must be crossed (not straight-through)!

## Wiring Diagram

```
Pro Micro          Wire Color        NodeMCU
─────────          ──────────        ───────
VCC (5V out)  ───────[RED]─────────> Vin (5V in)
GND           ──────[BLACK]─────────> GND
TX (Pin 1)    ─────[ORANGE]────────> RX
RX (Pin 0)    <────[YELLOW]───────── TX
    │
   USB
    │
 Computer
(Power + HID)
```

## Step-by-Step Instructions

1. **Disconnect both devices from USB**
2. **Connect 4 wires:**
   - Red wire: Pro Micro **VCC** → NodeMCU **Vin**
   - Black wire: Pro Micro **GND** → NodeMCU **GND**
   - Yellow wire: NodeMCU **TX** → Pro Micro **RX (Pin 0)** (crossed!)
   - Orange wire: NodeMCU **RX** → Pro Micro **TX (Pin 1)** (crossed!)
3. **Double-check connections** - especially that TX/RX are crossed
4. **Connect Pro Micro to computer via USB** - powers both devices

## Optional: OLED Display

If using a 128x64 I2C OLED display:
- Display **VCC** → NodeMCU **3.3V**
- Display **GND** → NodeMCU **GND**
- Display **SDA** → NodeMCU **D3** (GPIO4)
- Display **SCL** → NodeMCU **D4** (GPIO5)

**Don't use D0** - GPIO16 doesn't support I2C!

## Power Notes

- Single USB cable powers both devices (Pro Micro → computer)
- NodeMCU powered via VCC→Vin connection
- Total consumption: ~350-400mA (safe for USB 2.0/3.0)
- Use quality USB cable for reliable power
- If power issues occur, try USB 3.0 port or powered hub

## Setup Options

- **Breadboard:** Easy to modify, good for testing, no soldering
- **Direct wiring:** More compact, fewer components
- **Permanent:** Solder connections, use perf board, add case

## Common Mistakes

❌ **TX/RX straight-through** - Must be crossed (TX→RX, RX→TX)
❌ **Missing GND** - Communication won't work without common ground
❌ **Missing VCC→Vin** - NodeMCU won't power on
❌ **Using 3.3V instead of VCC** - Insufficient power
❌ **Both USB cables connected** - Only Pro Micro needs USB
❌ **Wrong serial pins** - Use Pro Micro pins 0 (RX) and 1 (TX), not USB serial

## Troubleshooting

- **NodeMCU won't power on:** Check VCC→Vin connection. Try different USB port/cable.
- **No communication:** Verify TX/RX are crossed. Check GND connection. Upload code to both devices.
- **Intermittent issues:** Usually power-related. Try USB 3.0 port or powered hub.
- **NodeMCU resets randomly:** Insufficient power. Use better USB cable or powered hub.

## Safety

- Disconnect USB before changing wiring
- Never connect output pins together (VCC to VCC)
- NodeMCU may get warm during WiFi operation (normal)
- If components get hot, disconnect immediately

See [SETUP.md](SETUP.md) for software setup instructions.
