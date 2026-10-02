# NightLite Wiring

This document describes the complete wiring for the NightLite project. All connections assume an ESP32 DevKit V1 board.

## Pin map

| Function | ESP32 GPIO | Direction | Notes |
|---|---|---|---|
| LDR divider midpoint | GPIO34 (ADC1_CH6) | Input | Input-only pin, no internal pullup |
| HC-SR501 PIR OUT | GPIO27 | Input | Digital, active-HIGH |
| Lamp MOSFET gate | GPIO25 | Output | LEDC PWM channel 0 |
| OLED SDA | GPIO21 | I²C | Default Wire SDA |
| OLED SCL | GPIO22 | I²C | Default Wire SCL |
| Mode button | GPIO14 → GND | Input | INPUT_PULLUP, active-LOW |
| Brightness + | GPIO26 → GND | Input | INPUT_PULLUP, active-LOW |
| Brightness − | GPIO33 → GND | Input | INPUT_PULLUP, active-LOW |

## Power

| Rail | Source | Consumers |
|---|---|---|
| 5 V | USB (ESP32 DevKit) | ESP32 Vin, LED lamp (via MOSFET), HC-SR501 VCC |
| 3.3 V | ESP32 onboard regulator | LDR divider, OLED VCC |

**Important**: The HC-SR501 PIR module can accept 5–20 V on its VCC pin. Connect it to the ESP32's 5 V (VIN) rail, **not** 3.3 V.

## LDR voltage divider

```
  3.3 V ──── LDR ──┬── GPIO34 (ADC input)
                    │
                  10 kΩ
                    │
                   GND
```

**How it works**:
- In **bright light**, the LDR resistance drops (e.g., ~1 kΩ), so the voltage at GPIO34 rises → higher ADC reading.
- In **darkness**, the LDR resistance increases (e.g., ~100 kΩ), so the voltage at GPIO34 drops → lower ADC reading.
- The 10 kΩ resistor creates the lower leg of the divider.

**ADC note**: GPIO34 is an input-only pin on the ESP32 with no internal pullup/pulldown. It connects to ADC1 channel 6. The ESP32's ADC is 12-bit (0–4095) with a default 0–3.3 V range.

**Calibration**: The exact ADC values depend on your specific LDR and resistor tolerance. See `docs/FIRMWARE.md` for calibration instructions.

## PIR sensor (HC-SR501)

```
  HC-SR501 Module
  ┌─────────────────┐
  │  VCC  OUT  GND  │
  └──┬────┬────┬────┘
     │    │    │
     │    │    └──── ESP32 GND
     │    └───────── ESP32 GPIO27
     └────────────── ESP32 5V (VIN pin)
```

**Configuration**:
- The HC-SR501 has two onboard potentiometers:
  - **Sensitivity**: adjusts the detection range (3–7 m).
  - **Time delay**: adjusts how long OUT stays HIGH after motion (5 s – 5 min).
- Set the time delay to minimum (~5 s) since the firmware handles its own 30 s timeout.
- The jumper on the module should be in **"H" (repeatable trigger)** mode.

**Output behavior**: OUT goes HIGH when motion is detected and stays HIGH for the configured time delay. The firmware reads this as a simple digital HIGH/LOW.

## MOSFET lamp switching (AO3400A)

```
        5 V ─────────────── LED lamp (+)
                                │
                            LED lamp (−)
                                │
                             DRAIN
                              │
  GPIO25 ──── 220 Ω ──── GATE  AO3400A (N-channel)
                              │
                            SOURCE
                                │
                               GND
```

**Why low-side switching**: The MOSFET switches the ground side of the lamp. This means:
- The lamp's positive terminal connects directly to 5 V.
- The MOSFET interrupts the ground path.
- When the gate is HIGH (3.3 V from ESP32), the MOSFET turns on and current flows through the lamp.
- When the gate is LOW, the MOSFET is off and the lamp is dark.

**Why 220 Ω gate resistor**: This limits inrush current to the MOSFET gate capacitance during fast PWM transitions. It's optional for a single MOSFET at 5 kHz but good practice.

**AO3400A specifications**:
- V_GS(th): 0.65–1.45 V (fully enhanced well below 3.3 V)
- R_DS(on): < 40 mΩ at V_GS = 2.5 V
- I_D max: 5.8 A (far more than a USB-powered LED lamp needs)

## OLED display (SSD1306)

```
  SSD1306 Module
  ┌─────────────────────┐
  │  GND  VCC  SCL  SDA │
  └──┬────┬────┬────┬───┘
     │    │    │    │
     │    │    │    └──── ESP32 GPIO21 (SDA)
     │    │    └───────── ESP32 GPIO22 (SCL)
     │    └────────────── ESP32 3.3V
     └─────────────────── ESP32 GND
```

**I²C address**: The firmware uses `0x3C`, which is the default for most 0.96″ SSD1306 modules. Some modules use `0x3D` — if the OLED doesn't work, try changing `OLED_ADDR` in the firmware.

**Pull-ups**: Most SSD1306 breakout modules include onboard 4.7 kΩ pull-up resistors on SDA and SCL. No external pull-ups are needed.

## Buttons

```
  GPIO14 ──── [MODE  button] ──── GND
  GPIO26 ──── [UP    button] ──── GND
  GPIO33 ──── [DOWN  button] ──── GND
```

**How they work**: Each button connects the GPIO pin to GND when pressed. The firmware configures each pin as `INPUT_PULLUP`, so:
- **Released** = pin reads HIGH (pulled up internally).
- **Pressed** = pin reads LOW (connected to GND through button).

No external resistors are needed — the ESP32's internal pull-ups handle it.

## Complete wiring summary

```
ESP32 DevKit V1
┌─────────────────────────────────────────────┐
│                                             │
│  3.3V ──── LDR ──┬── GPIO34 (LDR ADC)      │
│                  10kΩ                       │
│  GND ────────────┘                          │
│                                             │
│  5V (VIN) ──── HC-SR501 VCC                 │
│  GPIO27 ────── HC-SR501 OUT                 │
│  GND ────────── HC-SR501 GND                │
│                                             │
│  GPIO25 ── 220Ω ── AO3400A Gate             │
│  GND ────────────── AO3400A Source           │
│  5V ── LED lamp ── AO3400A Drain            │
│                                             │
│  3.3V ──── SSD1306 VCC                      │
│  GND ───── SSD1306 GND                      │
│  GPIO21 ── SSD1306 SDA                      │
│  GPIO22 ── SSD1306 SCL                      │
│                                             │
│  GPIO14 ── [MODE btn] ── GND                │
│  GPIO26 ── [UP btn] ──── GND                │
│  GPIO33 ── [DOWN btn] ── GND                │
│                                             │
└─────────────────────────────────────────────┘
```
