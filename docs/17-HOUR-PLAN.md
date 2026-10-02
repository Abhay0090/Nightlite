# NightLite — 17-Hour Design & Evidence Plan

This plan is for the Hack Club Half Life Warm-up Project submission. It separates work that can be completed before hardware arrives from evidence that must come from the real prototype.

## Hour 0–2 — Requirements and architecture
- Freeze the ESP32 + LDR + PIR + OLED + MOSFET + buttons architecture.
- Define AUTO and MANUAL behavior.
- Record pin assignments and power assumptions.
- Commit architecture notes.

Evidence:
- Git history
- docs/DESIGN.md
- docs/WIRING.md

## Hour 2–5 — Firmware prototype
- Implement ADC light sensing.
- Implement PIR motion detection and timeout.
- Implement automatic lamp brightness behavior.
- Implement manual brightness buttons.
- Implement OLED dashboard.
- Add comments explaining the control logic.

Evidence:
- src/nightlite.ino
- meaningful commits
- screenshots of code/serial output when available

## Hour 5–7 — Circuit and electrical design
- Draw the LDR voltage divider.
- Document MOSFET low-side switching.
- Verify GPIO voltage/power assumptions against component datasheets.
- Add a formal schematic after the first wiring is finalized.

Evidence:
- docs/WIRING.md
- schematic/
- component datasheets/links
- design decisions in the journal

## Hour 7–9 — BOM and sourcing
- Replace placeholder shopping links with current India-focused sources.
- Record approximate component costs.
- Keep the machine-readable bom.csv compatible with Half Life import.
- Separate required parts from optional parts.

Evidence:
- bom.csv
- sourcing notes
- commit history

## Hour 9–11 — Enclosure / CAD design
- Measure the actual ESP32, OLED, PIR, LDR and buttons.
- Design the enclosure around real dimensions.
- Add OLED opening, sensor openings, button holes, diffuser area and USB access.
- Export STL/STEP once dimensions are verified.

Evidence:
- CAD source files
- rendered views
- dimensions
- commits

## Hour 11–14 — Physical prototype and testing
This section requires the actual hardware.
- Assemble the circuit on the breadboard.
- Check power and common ground.
- Test OLED.
- Test LDR readings in bright/dark conditions.
- Test PIR triggering and timeout.
- Test MOSFET-controlled lamp.
- Tune DARK_THRESHOLD and MOTION_TIMEOUT.
- Capture real photos and short test evidence.

Do not claim these tests until they are actually performed.

Evidence:
- real prototype photos
- test notes
- measured observations
- firmware commits with tuned values

## Hour 14–16 — Integration and polish
- Transfer the verified circuit to perfboard if practical.
- Fit the electronics into the enclosure.
- Improve cable routing and diffuser placement.
- Update the wiring and CAD documentation to match the final build.
- Capture final photos.

Evidence:
- final prototype photos
- final CAD/render
- updated docs
- integration commits

## Hour 16–17 — Submission package
- Confirm README contains at least one project image.
- Confirm bom.csv is present and importable.
- Confirm journal/design/wiring files are committed.
- Remove unsupported claims.
- Add final real photos.
- Review the Half Life project page and make sure the logged work is supported by repository evidence.

## Important evidence rule

The concept SVG in images/ is a design visualization, not proof of a physical prototype. Real hardware photos and measured test results must be added after assembly.
