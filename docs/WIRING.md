# Hardware Wiring Guide

This guide provides detailed instructions for physically connecting the NodeMCU (ESP8266) and Pro Micro (ATmega32U4) microcontrollers.

## Overview

The connection between NodeMCU and Pro Micro uses serial communication (UART) for data transfer. The NodeMCU is powered directly from the Pro Micro's VCC pin, which provides 5V from the USB connection.

**Important:** The NodeMCU is powered by the Pro Micro. Only ONE USB cable is needed (to Pro Micro).

## Connection Summary

| NodeMCU Pin | Pro Micro Pin | Wire Color Suggestion | Description        |
|-------------|---------------|----------------------|--------------------|
| TX          | RX (Pin 0)    | Yellow or White      | Serial data TX→RX  |
| RX          | TX (Pin 1)    | Orange or Green      | Serial data RX→TX  |
| GND         | GND           | Black                | Common ground      |
| Vin         | VCC           | Red                  | Power (5V)         |

## Detailed Pin Diagram

```
    NodeMCU ESP8266                      Pro Micro ATmega32U4
   ┌──────────────────┐                ┌──────────────────┐
   │                  │                │                  │
   │  (Not Connected) │                │    [USB Port]    │
   │                  │                │   (Power & HID)  │
   │                  │                │                  │
   │  [Vin] D0    D1  │<───────RED─────┤ [VCC] GND  RST   │
   │   D3   D4    3V  │                │   A3   A2   A1   │
   │   D5   D6    D7  │                │   A0   15   14   │
   │   D8  [RX]  [TX] │──YELLOW───────>│   16   10    9   │
   │ [GND] 3V    EN   │<─ORANGE────────┤    8    7    6   │
   │                  │──BLACK────────>│    5    4 [TX1]  │
   │                  │                │ [RX0][GND]       │
   └──────────────────┘                └────────┬─────────┘
                                                │
                                               USB
                                                │
                                            Computer
                                       (Power Source & HID)
```

## ASCII Art Schematic

```
                           Pro Micro (USB Only)
                          ┌──────────┐
                          │   VCC    ├──────┐
                          │   TX(1)  ├────┐ │
                          │   RX(0)  ├──┐ │ │
                          │   GND    ├┐ │ │ │
                          └────┬─────┘│ │ │ │
                               │      │ │ │ │
                              USB     │ │ │ │
                               │      │ │ │ │
                          Computer    │ │ │ │
                                      │ │ │ │
                                      │ │ │ └──RED (5V Power)
                                      │ │ └────ORANGE (Serial RX)
                                      │ └──────YELLOW (Serial TX)
                                      └────────BLACK (Ground)
                                              │ │ │ │
    NodeMCU (Powered by Pro Micro)            │ │ │ │
   ┌──────────┐                               │ │ │ │
   │   Vin    │<──────────────────────────────┘ │ │ │
   │   TX     ├────────────────────────────────┘ │ │
   │   RX     │<──────────────────────────────────┘ │
   │   GND    │<────────────────────────────────────┘
   └──────────┘
```

## Step-by-Step Connection Instructions

### Step 1: Identify the Pins

**On NodeMCU:**
- **Vin pin** - Power input (accepts 5V) - usually on corner of board
- **TX pin** - Usually labeled "TX" or "TXD" (near the USB port)
- **RX pin** - Usually labeled "RX" or "RXD" (next to TX)
- **GND pin** - Multiple GND pins available (use any)

**On Pro Micro:**
- **VCC pin** - 5V output (top left when USB faces up)
- **Pin 0 (RX)** - Labeled "0" or "RXI" (Serial1 RX)
- **Pin 1 (TX)** - Labeled "1" or "TXO" (Serial1 TX)
- **GND pin** - Multiple GND pins available (use any)

**Note:** Pro Micro pins may not be pre-soldered. You may need to solder header pins yourself.

### Step 2: Prepare Your Wires

You'll need **4 male-to-male jumper wires**:
1. One for VCC → Vin (suggest **red** - power)
2. One for TX → RX (suggest yellow or white)
3. One for RX → TX (suggest orange or green)
4. One for GND → GND (suggest **black** - ground)

Using different colors helps identify connections later. **Red for power and black for ground is standard practice.**

### Step 3: Connect Power (FIRST!)

**Start with power connection:**

1. Connect **Pro Micro VCC** → **NodeMCU Vin** (use red wire)

**Important:**
- VCC provides 5V from Pro Micro's USB connection
- NodeMCU Vin accepts 5V input
- This powers the NodeMCU from the Pro Micro

### Step 4: Connect Ground

2. Connect **Pro Micro GND** → **NodeMCU GND** (use black wire)

**Why GND is critical:** Without a common ground, the serial communication will not work reliably or at all. The devices need a common voltage reference.

### Step 5: Make the TX/RX Connections (CROSSED!)

**CRITICAL:** TX and RX connections must be **crossed** (not straight through):

3. Connect **NodeMCU TX** → **Pro Micro RX (Pin 0)** (use yellow wire)
4. Connect **NodeMCU RX** → **Pro Micro TX (Pin 1)** (use orange wire)

**Why crossed?** TX (transmit) from one device must go to RX (receive) on the other device, and vice versa.

### Step 6: Verify Connections

Double-check:
- [ ] Pro Micro VCC connects to NodeMCU Vin (5V power)
- [ ] NodeMCU TX connects to Pro Micro Pin 0 (RX) - CROSSED
- [ ] NodeMCU RX connects to Pro Micro Pin 1 (TX) - CROSSED
- [ ] Pro Micro GND connects to NodeMCU GND
- [ ] All connections are firm and secure
- [ ] No accidental bridges between pins
- [ ] Wires are not loose or damaged

## Power Connections

### How to Power the System

The entire system is powered by a **single USB connection** to the Pro Micro:

1. **Pro Micro** → USB Cable → Computer (provides HID functionality + power for both devices)
2. **NodeMCU** → Powered from Pro Micro's VCC pin (no USB needed)

**Power Flow:**
```
Computer USB Port (5V, up to 500mA)
         ↓
   Pro Micro USB
         ↓
   Pro Micro VCC Pin (5V)
         ↓
   NodeMCU Vin Pin
         ↓
   NodeMCU (ESP8266 WiFi)
```

### Power Requirements

**Pro Micro (ATmega32U4):**
- Operating voltage: 5V
- Current consumption: ~50mA typical

**NodeMCU (ESP8266):**
- Operating voltage: 3.3V (regulated internally from 5V Vin)
- Current consumption: 80mA typical, up to 300mA during WiFi transmission

**Total System:**
- Maximum current: ~350-400mA
- USB 2.0 ports typically provide 500mA
- USB 3.0 ports typically provide 900mA
- **This configuration is safe for standard USB ports**

### Important Power Considerations

✅ **Advantages:**
- Only one USB cable needed
- Simpler setup
- More portable
- No need for extra power adapter

⚠️ **Important Notes:**
- Use a good quality USB cable (not damaged)
- Ensure USB port provides adequate current (500mA minimum)
- Pro Micro MUST be connected to the target computer (for HID functionality)
- NodeMCU does NOT need its own USB connection

⚠️ **When Power Issues May Occur:**
- Very old computers with limited USB power
- USB hubs without external power
- Damaged or low-quality USB cables
- Multiple high-power devices on same USB hub

**If you experience power issues** (NodeMCU not starting, WiFi disconnects):
- Try a different USB port (preferably USB 3.0)
- Use a powered USB hub
- Check cable quality
- As last resort, power NodeMCU separately via its USB port (but this defeats the single-cable advantage)

## Physical Setup Options

### Option 1: Breadboard Setup (Recommended for Testing)

```
  [NodeMCU on left side of breadboard]
         |  |  |  |
      [Jumper wires through breadboard]
         |  |  |  |
      [Red, Black, Yellow, Orange wires]
         |  |  |  |
  [Pro Micro on right side of breadboard]
         |
    [USB Cable to Computer]
```

**Connections on breadboard:**
- Red wire: VCC to Vin (power)
- Black wire: GND to GND (ground)
- Yellow wire: TX to RX (data, crossed)
- Orange wire: RX to TX (data, crossed)

**Advantages:**
- Easy to modify
- Clear visualization
- Simple troubleshooting
- No soldering required
- Can see all connections clearly

### Option 2: Direct Wire Connection

Connect jumper wires directly from NodeMCU pins to Pro Micro pins.

**Advantages:**
- More compact
- Fewer components
- Good for permanent installation

### Option 3: Permanent Installation

For permanent setups:
- Solder connections for reliability
- Use heat shrink tubing
- Mount on perf board or custom PCB
- Add case for protection

## Connection Verification Checklist

Before powering on:

- [ ] **Power:** Pro Micro VCC connects to NodeMCU Vin (red wire)
- [ ] **Ground:** Pro Micro GND connects to NodeMCU GND (black wire)
- [ ] **Data TX:** NodeMCU TX connects to Pro Micro RX/Pin 0 (yellow wire) - **CROSSED**
- [ ] **Data RX:** NodeMCU RX connects to Pro Micro TX/Pin 1 (orange wire) - **CROSSED**
- [ ] All wire connections are secure and fully inserted
- [ ] No short circuits between adjacent pins
- [ ] No accidental bridges on breadboard (if used)
- [ ] Only **ONE** USB cable ready (for Pro Micro)
- [ ] NodeMCU USB port is **NOT** connected
- [ ] Workspace is clean and organized
- [ ] Wire colors match the suggested scheme (easier troubleshooting)

## Common Wiring Mistakes

### ❌ Mistake 1: Straight-Through TX/RX
```
NodeMCU TX → Pro Micro TX  (WRONG!)
NodeMCU RX → Pro Micro RX  (WRONG!)
```

### ✅ Correct: Crossed TX/RX
```
NodeMCU TX → Pro Micro RX  (CORRECT!)
NodeMCU RX → Pro Micro TX  (CORRECT!)
```

### ❌ Mistake 2: Missing GND Connection
Without GND, devices can't communicate properly. Always connect ground!

### ❌ Mistake 3: Missing Power Connection
Forgetting to connect VCC to Vin. NodeMCU won't turn on without power!

### ❌ Mistake 4: Connecting to 3.3V instead of VCC
```
Pro Micro 3.3V → NodeMCU Vin  (WRONG! - Insufficient voltage)
Pro Micro VCC → NodeMCU Vin   (CORRECT! - 5V power)
```
The 3.3V pin doesn't provide enough current for ESP8266.

### ❌ Mistake 5: Connecting NodeMCU 3V3 to Pro Micro
```
NodeMCU 3V3 → Pro Micro VCC  (WRONG! - Never do this!)
```
Never connect output pins together. Only VCC→Vin is correct.

### ❌ Mistake 6: Using Wrong Serial Pins on Pro Micro
- Use pins **0 (RX)** and **1 (TX)** - these are Serial1
- Don't use the USB serial (Serial) - that's for debugging only

### ❌ Mistake 7: Connecting Both USB Cables
You only need **ONE** USB cable (to Pro Micro). NodeMCU gets power from VCC pin.

## Visual Reference

### NodeMCU Pin Layout
```
    ┌─────────────┐
    │   USB Port  │  ← Not used for power
    └─────────────┘
     │           │
   [Vin] D0    [TX]  ← Connect TX to Pro Micro RX
     D1  D2    [RX]  ← Connect RX to Pro Micro TX
     D3  D4     3V3
     D5  D6     D7
     D8 [GND]   3V

  [Vin]  = Connect to Pro Micro VCC (5V power in)
  [TX]   = Connect to Pro Micro Pin 0 (RX)
  [RX]   = Connect to Pro Micro Pin 1 (TX)
  [GND]  = Connect to Pro Micro GND
```

### Pro Micro Pin Layout
```
    ┌─────────────┐
    │   USB Port  │  ← Connect to Computer
    └─────────────┘
     │           │
    RAW [GND]   RST
   [VCC] A3     A2   ← VCC = 5V output
    A1   A0     15
    14   16     10
    9    8      7
    6    5      4
   [TX1][RX0] [GND]  ← Serial1 pins
     1    0

  [VCC]  = Connect to NodeMCU Vin (5V power out)
  [TX1]  = Pin 1, connect to NodeMCU RX
  [RX0]  = Pin 0, connect to NodeMCU TX
  [GND]  = Connect to NodeMCU GND
```

### Complete Wiring Visual Summary
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

## Troubleshooting Wiring Issues

### Problem: NodeMCU doesn't power on (no LED)

**Check:**
1. **VCC to Vin connection** - Most common issue!
2. Pro Micro is connected to USB and powered on
3. Red wire is properly connected to both VCC and Vin
4. No loose connections
5. USB port is providing power (try different port)

**Test:**
- Measure voltage between NodeMCU Vin and GND (should be ~5V)
- Check if Pro Micro is getting power (LED should be on)
- Try connecting NodeMCU to its own USB temporarily to verify it works

### Problem: NodeMCU powers on but WiFi doesn't work

**Check:**
1. NodeMCU blue LED should blink during boot
2. Code uploaded correctly to NodeMCU
3. Sufficient power - try USB 3.0 port or powered hub
4. Check Serial Monitor for error messages

**Power-related:**
- ESP8266 draws high current during WiFi transmission
- Weak USB ports may cause brownouts
- Try better quality/shorter USB cable

### Problem: NodeMCU and Pro Micro can't communicate

**Check:**
1. TX/RX are **crossed** (not straight through)
2. GND is connected (critical!)
3. Connections are firm and making contact
4. No damaged wires
5. Correct pins identified on both boards
6. Both devices have power

**Test:**
- Upload code to both devices first
- Use Serial Monitor on Pro Micro to check for "READY" message
- Check if Pro Micro LED blinks on startup (indicates code is running)
- Send PING command via NodeMCU, should get PONG response

### Problem: Pro Micro not acting as USB HID

**Check:**
1. Pro Micro is connected via USB to the target computer
2. Correct board selected in Arduino IDE (Arduino Leonardo)
3. Code uploaded successfully
4. USB cable supports data (not power-only cable)

### Problem: Intermittent communication or random resets

**Possible causes:**
1. **Insufficient power** - Most common!
   - Try USB 3.0 port (900mA vs 500mA)
   - Use powered USB hub
   - Check cable quality
2. Loose wire connections - check all 4 wires
3. Poor quality jumper wires - replace with better quality
4. Interference - keep wires short and away from power cables
5. Missing or loose GND connection
6. Breadboard with poor contacts

### Problem: System works but NodeMCU keeps resetting

**Power issue symptoms:**
- Works initially then resets
- Resets when WiFi connects
- Unstable operation

**Solutions:**
1. Use USB 3.0 port (provides more current)
2. Use high-quality, short USB cable
3. Add 100µF capacitor between Vin and GND on NodeMCU (optional)
4. Use powered USB hub
5. Check all connections are secure

## Next Steps

Once your wiring is complete:

1. **Double-check all connections** using the checklist above
2. **Don't power on yet** - proceed to [SETUP.md](SETUP.md) to upload the code first
3. **After code upload** - connect both devices and test communication

## Safety Notes

### Electrical Safety
- ✅ **Do** connect Pro Micro VCC (5V) to NodeMCU Vin - this is correct
- ❌ **Never** connect 5V output pins together (VCC to VCC, etc.)
- ❌ **Never** connect 3.3V pins to 5V pins
- ❌ **Never** connect NodeMCU 3V3 output to Pro Micro
- ⚠️ **Always** disconnect USB power before changing wiring

### Physical Safety
- Don't force pins or connectors
- Check for shorts before powering on
- Use proper ESD (electrostatic discharge) precautions
- Keep workspace clean and dry
- Ensure good ventilation (components may get warm)

### Power Safety
- USB ports are current-limited, but still verify connections first
- NodeMCU may get warm during WiFi operation (this is normal)
- If any component gets hot (not just warm), disconnect immediately
- Use quality USB cables and ports
- Don't exceed USB port current ratings

### First Power-On
When powering on for the first time:
1. Verify all connections match the diagrams
2. Look for any smoke or unusual smells
3. Check that both devices power on
4. NodeMCU blue LED should blink briefly on boot
5. Pro Micro LED should be steady (or blink 3 times with code loaded)

**If anything seems wrong, disconnect USB immediately!**

---

**Wiring complete?** Continue to [SETUP.md](SETUP.md) for software setup and programming!
