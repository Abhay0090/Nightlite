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
- Implement ADC light sensing with raw-to-percent conversion.
- Implement PIR motion detection and 30 s timeout tracking.
- Implement automatic lamp brightness behavior (OFF/FULL/DIM).
- Implement manual brightness buttons with debouncing.
- Implement OLED dashboard with 5-line status display.
- Add serial debug output for calibration.
- Add comments explaining the control logic.

Evidence:
- src/nightlite.ino (complete, compilable firmware)
- docs/FIRMWARE.md (control logic documentation)
- meaningful commits
- screenshots of code/serial output when available

## Hour 5–7 — Circuit and electrical design
- Draw the LDR voltage divider circuit.
- Document MOSFET low-side switching with gate resistor.
- Document PIR and OLED wiring.
- Verify GPIO voltage/power assumptions against component datasheets.
- Create a formal electrical schematic.

Evidence:
- docs/WIRING.md (complete wiring guide with ASCII diagrams)
- schematic/nightlite.svg (formal schematic)
- component datasheets/links
- design decisions in the journal

## Hour 7–9 — BOM and sourcing
- Replace placeholder shopping links with current India-focused sources.
- Record actual component costs from robu.in, thinkingrobot.in, etc.
- Keep the machine-readable bom.csv compatible with Half Life import.
- Separate required parts from optional parts.
- Verify total stays within Tier 2 budget ($65).

Evidence:
- bom.csv (machine-readable, with vendor links)
- sourcing notes
- commit history

## Hour 9–11 — Enclosure / CAD design
- Measure the ESP32 DevKit V1 PCB dimensions.
- Define internal layout: ESP32 mounting, sensor positions, button locations.
- Specify external cutouts: OLED window, PIR dome, LDR opening, USB port, LED diffuser.
- Choose wall thickness and material (PLA for 3D printing).
- Create enclosure specification document with dimensions.
- Create 2D enclosure drawings (top, front, side views).
- Generate an enclosure concept render.

Evidence:
- docs/ENCLOSURE.md (design spec)
- cad/ENCLOSURE_SPEC.md (dimensions and clearances)
- cad/ files (drawings, renders)
- design decisions in journal

## Hour 11–14 — Physical build and testing
- Assemble the breadboard prototype following docs/WIRING.md.
- Upload firmware and verify serial output.
- Calibrate DARK_THRESHOLD using real ADC readings.
- Test PIR detection range and timeout behavior.
- Test all three buttons and mode switching.
- Verify OLED dashboard displays correctly.
- Take photos of the assembled prototype.
- Record a demo video showing AUTO and MANUAL modes.

Evidence:
- Calibrated DARK_THRESHOLD value committed to firmware
- Photos in images/ and README
- Demo video
- Journal entries documenting test results

## Hour 14–16 — Integration and polish
- Transfer from breadboard to more permanent wiring (if time allows).
- 3D print enclosure (if printer available) or document the design for future printing.
- Final firmware cleanup and code review.
- Update README with final photos and status.
- Review all documentation for completeness.

Evidence:
- Final firmware version
- Enclosure photos (if printed)
- Updated README
- Clean commit history

## Hour 16–17 — Final submission preparation
- Verify all Half Life repo checks pass (public repo, README, photos).
- Import BOM to Half Life platform.
- Write final journal entries summarizing the project.
- Log all session hours on the platform.
- Submit the design on Half Life.

Evidence:
- All repo checks passing
- BOM imported with correct prices
- Journal entries covering all phases
- Design submitted on the platform

---

## Progress tracker

| Phase | Hours | Status |
|---|---|---|
| Requirements & architecture | 0–2 | ✅ Complete |
| Firmware prototype | 2–5 | ✅ Complete |
| Circuit & electrical design | 5–7 | ✅ Complete |
| BOM & sourcing | 7–9 | ✅ Complete |
| Enclosure / CAD design | 9–11 | ✅ Complete |
| Physical build & testing | 11–14 | ⬜ Awaiting hardware |
| Integration & polish | 14–16 | ⬜ Awaiting hardware |
| Final submission prep | 16–17 | 🔄 In progress |
