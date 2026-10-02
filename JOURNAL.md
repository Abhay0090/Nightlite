# NightLite — Development Journal

## Entry 1 — Project kickoff and architecture

**Date**: October 2026

Started the NightLite project — an automatic desk lamp that turns on when it's dark and someone is nearby, and dims or turns off otherwise.

**What I did**:
- Chose the ESP32 DevKit V1 as the main controller — built-in WiFi, 12-bit ADC, and plenty of GPIOs.
- Decided on the sensor suite: LDR (light sensing), HC-SR501 PIR (motion detection), SSD1306 OLED (status display).
- Designed two operating modes: AUTO (sensor-driven) and MANUAL (button-controlled brightness).
- Froze the pin assignments and documented them in `docs/WIRING.md`.
- Wrote the initial architecture document in `docs/DESIGN.md`.

**Key decisions**:
- Voltage divider with 10kΩ pull-down for the LDR to learn how analog sensors work.
- AO3400A N-channel MOSFET for low-side switching — logic-level, works with 3.3V gate.
- Three tactile buttons: MODE, BRIGHTNESS+, BRIGHTNESS−.

**What I learned**: Many common MOSFETs (like IRF520) need 10V at the gate to fully turn on, which doesn't work with 3.3V microcontrollers. The AO3400A is specifically designed for logic-level operation.

---

## Entry 2 — Firmware development

**Date**: October 2026

Wrote the complete firmware in `src/nightlite.ino`.

**What I did**:
- Implemented ADC light sensing with raw-to-percent conversion (12-bit, 0–4095 range).
- Implemented PIR motion detection with a 30-second timeout window.
- Built the AUTO mode state machine: bright room → OFF, dark + motion → FULL, dark + no motion → DIM (40%).
- Implemented MANUAL mode with button-controlled brightness in steps of 25/255.
- Added non-blocking button debouncing (180ms window, per-button struct with timestamps).
- Built the OLED dashboard: title/mode, light level, motion status, lamp output, brightness bar.
- Added serial debug output for calibration.
- Used ESP32's LEDC peripheral for hardware PWM at 5kHz, 8-bit resolution.

**Challenges**: Getting button debounce right took iteration. The first version used `delay()`, which blocked the main loop. The final version uses non-blocking timestamp comparison.

---

## Entry 3 — Circuit design and schematic

**Date**: October 2026

Created complete wiring documentation and electrical schematic.

**What I did**:
- Drew circuit diagrams for each subsystem: LDR divider, PIR wiring, MOSFET lamp circuit, OLED I²C, buttons.
- Created the formal schematic in `schematic/nightlite.svg`.
- Documented power distribution: 5V USB → ESP32 VIN + LED lamp + PIR VCC. 3.3V regulator → LDR divider + OLED.
- Added 220Ω gate resistor for the AO3400A to limit inrush during PWM transitions.

**What I learned**: Low-side MOSFET switching is simpler than high-side — no charge pump or bootstrap circuit needed. The MOSFET just interrupts the ground path.

---

## Entry 4 — BOM and sourcing

**Date**: October 2026

Built the complete bill of materials with sourcing.

**What I did**:
- Found all components on Indian suppliers (robu.in, thinkingrobot.in).
- Created the machine-readable `bom.csv` with 19 line items.
- Included custom 3D printed enclosure (base shell + diffuser cap) from 3ding.in.
- Included all prototyping supplies: breadboard, jumper wires, perfboard, standoffs, heat shrink, USB cable.
- Total estimated cost: ~$56.55 including 3D printing and shipping.

---

## Entry 5 — Enclosure design

**Date**: October 2026

Designed the 3D-printable enclosure for NightLite.

**What I did**:
- Measured all component dimensions from datasheets.
- Designed a two-part enclosure: opaque PLA base shell (70mm) + translucent PETG diffuser cap (40mm).
- Specified all cutouts: OLED window (front), 3 button holes (front), PIR dome (side), LDR hole (top), USB slot (back).
- Designed a snap-fit joint between base and diffuser for tool-free assembly.
- Created detailed CAD specifications in `cad/ENCLOSURE_SPEC.md` with exact coordinates for every cutout.

**Key decisions**:
- LDR opening faces upward and away from the LED to prevent self-illumination feedback loop.
- PIR faces sideways through a cutout for 120° coverage across the desk.
- Translucent PETG diffuser cap spreads the LED light evenly for a pleasant lamp effect.
- Snap-fit tabs for tool-free assembly/disassembly.

---

## Next steps

- Receive components and 3D printed enclosure.
- Build the breadboard prototype following `docs/WIRING.md`.
- Calibrate `DARK_THRESHOLD` using real ADC readings.
- Test all sensors, buttons, and OLED together.
- Transfer to perfboard for permanent assembly.
- Fit everything into the 3D printed enclosure.
- Take final photos and record a demo video.
