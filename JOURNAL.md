# NightLite — Journal / Devlog

## Entry 1 — Project kickoff and architecture (Hour 0–2)

**Date**: October 2026  
**Duration**: ~2 hours

Started the NightLite project as my Hack Club Half Life warm-up build. The goal is simple: an automatic night lamp that turns on when it's dark and someone is nearby, and turns off (or dims) otherwise.

**What I did**:
- Chose the ESP32 DevKit V1 as the main controller — it has built-in WiFi, a 12-bit ADC, and plenty of GPIOs. Overkill for a night lamp, but great for learning.
- Decided on the sensor suite: LDR (light sensing), HC-SR501 PIR (motion detection), SSD1306 OLED (status display).
- Designed the two operating modes: AUTO (sensor-driven) and MANUAL (button-controlled brightness).
- Froze the pin assignments and documented them in `docs/WIRING.md`.
- Wrote the initial `docs/DESIGN.md` with the functional flow and architecture rationale.

**Key decisions**:
- Using a voltage divider with a 10kΩ pull-down for the LDR instead of a standalone LDR module, to learn how analog sensors work.
- AO3400A N-channel MOSFET for low-side lamp switching — it's logic-level (works with 3.3V gate drive) and has very low on-resistance.
- Three tactile buttons for the interface: MODE, BRIGHTNESS+, BRIGHTNESS−.

**What I learned**: Choosing the right MOSFET matters a lot. Many common MOSFETs (like IRF520) need 10V at the gate to fully turn on, which doesn't work with a 3.3V microcontroller. The AO3400A is specifically designed for logic-level operation.

---

## Entry 2 — Firmware development (Hour 2–5)

**Date**: October 2026  
**Duration**: ~3 hours

Wrote the complete firmware in `src/nightlite.ino`.

**What I did**:
- Implemented the ADC light sensing with raw-to-percent conversion. The LDR divider gives 0–4095 from the 12-bit ADC; I map that to 0–100% for the display.
- Implemented PIR motion detection with a 30-second timeout. When motion is detected, the timestamp is saved. The lamp goes from FULL to DIM after 30 seconds of no motion.
- Implemented the AUTO mode state machine: bright room → OFF, dark + motion → FULL, dark + no motion → DIM.
- Implemented MANUAL mode with button-controlled brightness in steps of 25 (out of 255).
- Added button debouncing using a per-button struct that tracks the last stable state and the last change timestamp. 180ms debounce window works well.
- Built the OLED dashboard with 5 rows: title/mode, light level, motion status, lamp output, and a visual brightness bar.
- Added serial debug output that prints a compact status line for calibration.
- Used ESP32's LEDC peripheral for hardware PWM at 5kHz, 8-bit resolution.

**Key decisions**:
- Used `INPUT_PULLUP` for buttons instead of external pull-up resistors — simpler wiring.
- Added an `oledReady` flag so the firmware doesn't crash if the OLED fails to initialize — it just runs without the display and still logs to serial.
- Set display refresh to every 250ms to avoid OLED flicker from too-frequent redraws.

**Challenges**: Getting the button debounce right took some iteration. The first version used a simple delay, which blocked the main loop. The final version uses non-blocking timestamp comparison.

---

## Entry 3 — Circuit design and schematic (Hour 5–7)

**Date**: October 2026  
**Duration**: ~2 hours

Created the complete wiring documentation and electrical schematic.

**What I did**:
- Drew ASCII-art circuit diagrams for each subsystem in `docs/WIRING.md`: LDR divider, PIR wiring, MOSFET lamp circuit, OLED I²C, and buttons.
- Created the formal schematic in `schematic/nightlite.svg`.
- Documented the power distribution: 5V from USB goes to ESP32 VIN, LED lamp, and PIR VCC. The onboard 3.3V regulator powers the LDR divider and OLED.
- Added a 220Ω gate resistor between GPIO25 and the AO3400A gate to limit inrush current during PWM transitions.

**What I learned**: Low-side MOSFET switching is simpler than high-side because you don't need a charge pump or bootstrap circuit. The MOSFET just interrupts the ground path, and the lamp's positive terminal goes straight to 5V.

---

## Entry 4 — BOM and sourcing (Hour 7–9)

**Date**: October 2026  
**Duration**: ~2 hours

Built the bill of materials with India-focused sourcing.

**What I did**:
- Found all components on Indian suppliers (robu.in, thinkingrobot.in) with actual prices.
- Created the machine-readable `bom.csv` compatible with Half Life's BOM import feature.
- Total estimated cost: ~$14.51 (well within the $65 Tier 2 budget).
- Identified 13 components total: ESP32, OLED, PIR, LDR, 2 resistors, MOSFET, LED module, 3 buttons, breadboard, jumper wires, USB cable, and PLA filament.

**Key decisions**:
- Chose a breadboard for prototyping rather than a custom PCB — faster iteration and easier to modify.
- Included PLA filament in the BOM for the 3D-printed enclosure.
- Used robu.in as the primary vendor — they have everything and ship across India.

---

## Entry 5 — Enclosure design (Hour 9–11)

**Date**: October 2026  
**Duration**: ~2 hours

Designed the 3D-printable enclosure for NightLite.

**What I did**:
- Measured all component dimensions from datasheets (ESP32, OLED, PIR, buttons).
- Designed a two-part enclosure: opaque base shell (70mm) + translucent diffuser cap (40mm).
- Specified all cutout positions: OLED window (front), 3 button holes (front), PIR dome (side), LDR hole (top), USB slot (back).
- Designed a snap-fit joint between base and diffuser for tool-free assembly.
- Documented everything in `docs/ENCLOSURE.md` and `cad/ENCLOSURE_SPEC.md`.

**Key decisions**:
- The LDR opening faces upward and away from the LED to prevent the lamp's own light from triggering the "bright room" condition.
- The PIR sensor faces sideways through a cutout, giving it a 120° detection cone across the desk area.
- Translucent PLA for the diffuser cap — it spreads the LED light evenly for a pleasant lamp effect.

**What I learned**: Designing an enclosure is surprisingly complex. You have to think about component clearances, cable routing, thermal management, and how the user will actually interact with the buttons and display.

---

## Next steps

- **Physical assembly**: Build the breadboard prototype once components arrive.
- **Calibration**: Measure real ADC values to set `DARK_THRESHOLD`.
- **Testing**: Verify all sensors, buttons, and OLED work together.
- **3D printing**: Print the enclosure and test component fit.
- **Final polish**: Update README with photos and demo video.
