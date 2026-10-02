# Firmware Notes

## Intended environment

The firmware targets an **ESP32 DevKit V1** using the Arduino framework. It is written as a single `.ino` file (`src/nightlite.ino`) for simplicity.

### Board setup (Arduino IDE)

1. **Board Manager** → install "esp32 by Espressif Systems".
2. **Board** → select "ESP32 Dev Module".
3. **Upload speed** → 921600 (or 115200 if unstable).
4. **Flash size** → 4 MB.
5. **Partition scheme** → Default 4 MB with spiffs.

### Required libraries

Install these through the Arduino IDE Library Manager:

| Library | Version | Purpose |
|---|---|---|
| Adafruit GFX Library | ≥ 1.11 | Graphics primitives for the OLED |
| Adafruit SSD1306 | ≥ 2.5 | SSD1306 OLED driver |

Both libraries and their dependencies are installed automatically when you search for "Adafruit SSD1306" in the Library Manager and click "Install All".

## Control logic

The main loop runs continuously (~50 Hz with the 20 ms delay) and follows this sequence every iteration:

1. **Read LDR** — `analogRead(GPIO34)` returns a 12-bit value (0–4095).
2. **Read PIR** — `digitalRead(GPIO27)` returns HIGH if motion is detected.
3. **Update motion timestamp** — if motion is detected, record `millis()`.
4. **Handle buttons** — poll all three buttons with 180 ms debounce:
   - MODE button (GPIO14): toggles between AUTO and MANUAL.
   - UP button (GPIO26): increases `manualBrightness` by 25 (capped at 255).
   - DOWN button (GPIO33): decreases `manualBrightness` by 25 (floored at 0).
5. **Run active mode**:
   - **AUTO mode**:
     - Bright room (ADC ≥ `DARK_THRESHOLD`) → lamp OFF (PWM = 0).
     - Dark + recent motion (within 30 s) → lamp FULL (PWM = 255).
     - Dark + no recent motion → lamp DIM (PWM = 102, ~40%).
   - **MANUAL mode**:
     - Lamp output = `manualBrightness` (adjusted by buttons).
6. **Update OLED** (throttled to every 250 ms to avoid flicker):
   - Line 1: "NightLite" + mode indicator `[AUTO]` or `[MAN]`.
   - Line 2: Light percentage + dark/bright label.
   - Line 3: Motion status + seconds since last motion.
   - Line 4: Lamp output percentage.
   - Line 5: Visual brightness bar (filled rectangle).
7. **Serial debug** — prints a compact one-line status for calibration.

## PWM implementation

The firmware uses the ESP32's hardware LEDC peripheral for PWM:

- **Channel**: 0
- **Frequency**: 5 kHz (inaudible, no LED flicker)
- **Resolution**: 8-bit (0–255 duty cycle)
- **Output pin**: GPIO25 → AO3400A MOSFET gate

The `ledcSetup()` and `ledcAttachPin()` calls in `setup()` configure this. The `setLamp()` function writes to the channel via `ledcWrite()`.

## Button debouncing

Each button has a `Button` struct that tracks:
- `pin` — the GPIO number
- `lastStable` — the last confirmed stable reading
- `lastChangeMs` — timestamp of the last state change

The `buttonPressed()` function returns `true` only on a falling edge (HIGH → LOW transition) that occurs at least `DEBOUNCE_MS` (180 ms) after the previous state change. This prevents multiple triggers from contact bounce.

## OLED failure handling

The `oledReady` flag is set during `setup()` based on whether `display.begin()` succeeds. If the OLED fails to initialise (wrong address, not connected, defective), the firmware continues running normally — it just skips all `display.*` calls. Serial output still works for debugging.

## First hardware calibration

The value `DARK_THRESHOLD = 1800` is an initial design estimate, not a measured result.

After assembling the real circuit:

1. Open Serial Monitor at 115200 baud.
2. Record several ADC values in a well-lit room (daylight or desk lamp on).
3. Record several ADC values in a dark room (lights off, curtains closed).
4. Set `DARK_THRESHOLD` to a value midway between the two ranges.
5. Re-upload and verify the lamp turns on/off at the right ambient level.

Example calibration data:

| Condition | Expected ADC range | Light % |
|---|---|---|
| Direct sunlight | 3500–4095 | 85–100% |
| Well-lit room | 2500–3500 | 61–85% |
| Dim room | 1200–2500 | 29–61% |
| Dark (night) | 0–1200 | 0–29% |

**Note**: These ranges are rough estimates. Your specific LDR, resistor tolerance, and divider orientation will produce different values. Always calibrate with your actual hardware.

## Serial debug format

The serial output looks like this:

```
[DBG] ADC=2847  light=69%  dark=N  motion=N  recent=N  lamp=0/255  mode=AUTO
[DBG] ADC=1523  light=37%  dark=Y  motion=Y  recent=Y  lamp=255/255  mode=AUTO
[DBG] ADC=1523  light=37%  dark=Y  motion=N  recent=Y  lamp=255/255  mode=AUTO
[DBG] ADC=1523  light=37%  dark=Y  motion=N  recent=N  lamp=102/255  mode=AUTO
```

This makes it easy to monitor sensor behavior and diagnose issues during development.
