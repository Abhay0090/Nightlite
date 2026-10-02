# NightLite Design

NightLite is a compact automatic desk/night lamp designed around an ESP32 DevKit V1. It combines ambient-light sensing, passive infrared motion detection, PWM brightness control and a small OLED status display into a single desk-friendly device.

## Design goals

- Detect ambient darkness with an LDR voltage divider.
- Detect nearby movement with a passive infrared (PIR) sensor.
- Automatically illuminate only when it is both dark and someone is present.
- Provide a simple manual override with up/down brightness buttons.
- Show live status (light level, motion, lamp output, mode) on a 0.96″ OLED.
- Fit all electronics into a compact 3D-printed enclosure suitable for a desk or nightstand.
- Keep total cost under the Hack Club Half Life Tier 2 budget ($65).

## Functional flow

```
┌──────────────┐
│  Read LDR    │───► ADC value → light percentage
└──────┬───────┘
       │
       ▼
┌──────────────┐
│  Read PIR    │───► motion detected? → update last-motion timestamp
└──────┬───────┘
       │
       ▼
┌──────────────┐
│  Mode check  │
└──┬───────┬───┘
   │       │
AUTO     MANUAL
   │       │
   ▼       ▼
┌──────┐  ┌──────────────────┐
│Bright│  │Use manualBrightness│
│room? │  │set by buttons      │
└──┬───┘  └────────┬─────────┘
   │               │
   ▼               ▼
  Yes→OFF       setLamp(manualBrightness)
  No + motion → FULL (255)
  No + timeout→ DIM  (102, ~40%)
       │
       ▼
┌──────────────┐
│ Update OLED  │───► display light%, motion, lamp%, mode
└──────────────┘
```

## Architecture decisions

### Why ESP32 instead of Arduino Uno?

- Built-in 12-bit ADC (vs 10-bit on Uno) — better light-level resolution.
- 3.3 V logic across the board — no level-shifting needed for the OLED.
- Built-in WiFi/BLE for potential future features (remote control, data logging).
- Hardware LEDC PWM — clean, flicker-free lamp control without timer hacks.
- Enough GPIOs for all sensors, buttons and display without a port expander.

### Why AO3400A MOSFET?

- Logic-level gate: fully enhanced at 2.5 V, so 3.3 V from ESP32 drives it directly.
- SOT-23 package: tiny, breadboard-friendly via breakout or dead-bug soldering.
- R_DS(on) < 40 mΩ: negligible heat at the currents a 5 V LED strip draws.
- Low-side switching keeps the high side simple (direct 5 V to lamp).

### Why SSD1306 OLED?

- Ubiquitous, cheap (~₹185 / $2.24 in India).
- I²C interface: only 2 wires (SDA, SCL) — saves GPIOs.
- 128×64 pixels is enough for a 5-line status dashboard.
- Well-supported by the Adafruit SSD1306 library.

### Why HC-SR501 PIR?

- Cheap (~₹52 / $0.63), widely available.
- Self-contained module with built-in amplifier and comparator.
- Adjustable sensitivity and hold time via onboard potentiometers.
- Digital output (HIGH/LOW) — no analog processing needed on ESP32.

## Prototype scope

This repository documents the **design and first firmware prototype**. Physical assembly and real-world testing are planned but **not yet claimed as complete**. Key calibration values (`DARK_THRESHOLD`, LDR divider orientation) are design estimates that must be verified on the real hardware.

## Future improvements

- **WiFi control**: add a simple web UI to adjust brightness and view sensor data remotely.
- **Sleep mode**: use ESP32 deep sleep when no motion for extended periods to save power.
- **Smooth transitions**: fade between brightness levels instead of instant switching.
- **NTP clock**: display time on the OLED when the lamp is idle.
- **Data logging**: log sensor readings to SPIFFS/SD for analysis.
