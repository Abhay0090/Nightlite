# NightLite Design

NightLite is a compact automatic desk/night lamp designed around an ESP32.

## Design goals
- Detect ambient darkness with an LDR.
- Detect nearby movement with a PIR sensor.
- Automatically illuminate only when useful.
- Provide a simple manual override.
- Show live status on a small OLED.
- Fit the electronics into a 3D-printed enclosure.

## Functional flow

Dark + motion -> bright lamp

Dark + no motion -> dim lamp after timeout

Bright environment -> lamp off

Manual mode -> user-controlled brightness

## Prototype status

This repository contains the planned electronics, firmware prototype, wiring documentation and BOM. Hardware assembly and physical testing are still to be completed.

## Planned enclosure

A small vertical desk enclosure will hold the ESP32, OLED, buttons and sensors. The LED/lamp will sit behind a diffuser so the enclosure produces a soft light instead of a harsh point source.
