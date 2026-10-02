# NightLite 🌙💡

NightLite is an ESP32-based automatic desk/night lamp that combines ambient-light sensing, motion detection, PWM brightness control and a small OLED status display.

> Project status: Design/prototype stage. The repository documents the planned build and contains a first firmware prototype; physical assembly and testing are not yet claimed.

![NightLite concept render](images/nightlite-concept.svg)

## What it does

- Automatic mode: dark + motion turns the lamp on.
- Adaptive lighting: after motion stops, the lamp dims after a timeout.
- Bright-room detection: the lamp stays off when the room is bright.
- Manual mode: buttons provide a manual brightness override.
- OLED dashboard: shows light level, motion, lamp level and mode.
- Enclosure: planned compact 3D-printed desk/night-lamp enclosure.

## Hardware

| Part | Role |
|---|---|
| ESP32 DevKit V1 | Main controller |
| LDR + 10kΩ resistor | Ambient-light sensor |
| HC-SR501 PIR | Motion detection |
| 0.96″ SSD1306 OLED | Status display |
| AO3400A MOSFET | PWM lamp switching |
| 5V LED lamp | Light source |
| 3 tactile buttons | Mode / brightness controls |

The machine-readable parts list is in **bom.csv** for Half Life import.

## Repository

- src/nightlite.ino — firmware prototype
- docs/DESIGN.md — system design and behavior
- docs/WIRING.md — wiring and electrical notes
- docs/JOURNAL.md — build progress
- bom.csv — bill of materials
- schematic/ — schematic notes
- cad/ — enclosure CAD plan

## Functional flow

    Ambient light -> LDR -> ESP32 ADC -> PWM -> MOSFET -> LED lamp
    Motion       -> PIR -> ESP32     -> I2C OLED
    Buttons      -> ESP32

## Current limitations

The current firmware is an initial prototype and has not yet been validated on physical hardware. Thresholds, PIR timing, PWM behavior, power wiring and enclosure dimensions will be tuned during the physical build.

## Next milestones

1. Acquire the BOM.
2. Assemble the breadboard prototype.
3. Tune the LDR dark threshold.
4. Validate PIR detection and timeout.
5. Validate PWM lamp control.
6. Test the OLED interface and buttons.
7. Design/print the enclosure.
8. Add real build photos, measurements and test results.

## License

MIT
