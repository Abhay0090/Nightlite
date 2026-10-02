# NightLite — Extended Dev Journal

This journal documents the development process in more detail than the root `JOURNAL.md`. It captures design decisions, problems encountered, and lessons learned.

## Session 1: Why this project?

I wanted a warm-up project that hits several learning goals at once:
- **Analog sensors**: The LDR voltage divider teaches analog input, ADC resolution, and calibration.
- **Digital sensors**: The PIR module teaches digital input, interrupts (future), and timeout logic.
- **Output control**: PWM through a MOSFET teaches power electronics basics without dealing with high voltage.
- **Display**: The SSD1306 OLED teaches I²C communication, graphics libraries, and UI design on constrained hardware.
- **Embedded state machines**: The AUTO/MANUAL mode system is a simple but real state machine.

A night lamp is the perfect vehicle because the end result is actually useful — I can put it on my desk.

## Session 2: Choosing the ESP32

I considered three microcontrollers:

| Board | ADC | PWM | I²C | WiFi | Price | Verdict |
|---|---|---|---|---|---|---|
| Arduino Uno | 10-bit, 6 ch | Software timers | Yes | No | ~$5 | Too limited |
| Arduino Nano | 10-bit, 8 ch | Software timers | Yes | No | ~$3 | Better, but no WiFi |
| ESP32 DevKit V1 | 12-bit, 18 ch | Hardware LEDC | Yes | Yes + BLE | ~$5 | ✅ Winner |

The ESP32 costs about the same as a Nano but gives me WiFi (for future features), a better ADC, and hardware PWM. The only downside is it's 3.3V logic, which means I need a logic-level MOSFET.

## Session 3: The MOSFET problem

My first instinct was to use an IRF520 MOSFET (they're everywhere in starter kits). But I looked at the datasheet and found the problem:

- IRF520 V_GS(th) = 2–4V
- R_DS(on) specified at V_GS = 10V
- At V_GS = 3.3V (ESP32 output), the MOSFET barely turns on

This means the lamp would be dim and the MOSFET would overheat. Not good.

The AO3400A solves this:
- V_GS(th) = 0.65–1.45V
- R_DS(on) < 40mΩ at V_GS = 2.5V
- Fully enhanced at 3.3V

It's tiny (SOT-23 package) but handles up to 5.8A, which is way more than a USB-powered LED strip needs.

## Session 4: LDR voltage divider math

The voltage divider formula:

```
V_out = V_in × R2 / (R1 + R2)
```

Where R1 = LDR (variable) and R2 = 10kΩ (fixed).

- **Bright light**: LDR ≈ 1kΩ → V_out = 3.3 × 10k/(1k+10k) = 3.0V → ADC ≈ 3723
- **Dim room**: LDR ≈ 10kΩ → V_out = 3.3 × 10k/(10k+10k) = 1.65V → ADC ≈ 2048
- **Dark**: LDR ≈ 100kΩ → V_out = 3.3 × 10k/(100k+10k) = 0.3V → ADC ≈ 372

So the ADC range for "dark to bright" is roughly 300–3700. My initial `DARK_THRESHOLD = 1800` means "anything below 44% of the range is dark." This is a rough guess — I'll calibrate with the real hardware.

## Session 5: Button debouncing

Mechanical buttons "bounce" — when you press them, the contacts make and break several times in a few milliseconds. Without debouncing, one press can register as 5–10 presses.

My debouncing approach:
1. Read the button pin.
2. If it differs from the last stable reading AND at least 180ms have passed since the last change:
   - Accept the new reading as stable.
   - If the new stable reading is LOW (pressed), return `true`.
3. Otherwise, return `false`.

This is a "software debounce with timestamp" approach. It's non-blocking (no `delay()` calls) so the main loop keeps running smoothly.

## Session 6: Enclosure design thinking

The hardest part of the enclosure design was the LDR placement. If the LDR faces the LED, the lamp's own light will fool the sensor into thinking the room is bright, causing the lamp to turn off — creating an oscillation loop.

Solution: The LDR opening faces **upward** through the top of the base shell, while the LED is in the diffuser cap above. A small partition separates the two optically. The LDR senses the room's ambient light coming from above, not the lamp's own light reflected back.

The PIR sensor faces sideways through a cutout in the side wall. Its 120° detection cone covers the desk area in front of the lamp. The HC-SR501's Fresnel lens dome protrudes slightly through the opening for maximum sensitivity.
