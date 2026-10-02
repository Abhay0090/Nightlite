# ðŸŒ™ NightLite â€” Project Status Dashboard

> **Last synced**: 2026-10-02 Â· **Repo**: [Abhay0090/Nightlite](https://github.com/Abhay0090/Nightlite) Â· **Stage**: Design / Pre-prototype

---

## ðŸ“Š Overall Progress

```mermaid
pie title 17-Hour Plan Completion
    "âœ… Done" : 9
    "ðŸ”² Remaining" : 8
```

| Phase | Hours | Status | Deliverables |
|---|---|---|---|
| Requirements & Architecture | 0â€“2 | âœ… Complete | `docs/DESIGN.md`, `docs/WIRING.md`, pin map frozen |
| Firmware Prototype | 2â€“5 | âœ… Complete | `src/nightlite.ino` â€” ADC, PIR, AUTO/MANUAL, OLED, debounce |
| Circuit & Electrical Design | 5â€“7 | âœ… Complete | `docs/WIRING.md`, `schematic/nightlite.svg` |
| BOM & Sourcing | 7â€“9 | âœ… Complete | `bom.csv` â€” India-focused, ~$13.74 estimate |
| Enclosure / CAD Design | 9â€“11 | â¬œ Not started | `cad/` â€” placeholder README only |
| Physical Build & Testing | 11â€“14 | â¬œ Blocked (hardware) | Awaiting component delivery |
| Integration & Polish | 14â€“16 | â¬œ Blocked | Depends on physical build |
| Final Submission Prep | 16â€“17 | â¬œ Blocked | Depends on integration |

---

## ðŸ—‚ï¸ Repository File Map

```
Nightlite/
â”œâ”€â”€ README.md                  âœ… Concept render + feature summary
â”œâ”€â”€ BOM.md                     âœ… Half Life mirror (auto-generated)
â”œâ”€â”€ JOURNAL.md                 âœ… Half Life mirror (auto-generated)
â”œâ”€â”€ bom.csv                    âœ… India-focused, machine-readable
â”œâ”€â”€ src/
â”‚   â””â”€â”€ nightlite.ino          âœ… Full firmware prototype
â”œâ”€â”€ docs/
â”‚   â”œâ”€â”€ 17-HOUR-PLAN.md        âœ… Full plan with evidence checklist
â”‚   â”œâ”€â”€ DESIGN.md              âœ… Architecture & functional flow
â”‚   â”œâ”€â”€ FIRMWARE.md            âœ… Control logic & calibration notes
â”‚   â”œâ”€â”€ WIRING.md              âœ… Pin map + divider/PIR/OLED wiring
â”‚   â””â”€â”€ JOURNAL.md             âœ… Dev journal
â”œâ”€â”€ schematic/
â”‚   â””â”€â”€ nightlite.svg          âœ… Formal electrical schematic
â”œâ”€â”€ cad/
â”‚   â””â”€â”€ README.md              â¬œ Placeholder only
â””â”€â”€ images/
    â””â”€â”€ nightlite-concept.svg  âœ… Product concept render
```

---

## ðŸ”§ Hardware Architecture

### Pin Assignments (Frozen)

| Function | GPIO | Type | Notes |
|---|---|---|---|
| LDR divider midpoint | **34** | ADC input | `3.3V â†’ LDR â†’ GPIO34 â†’ 10kÎ© â†’ GND` |
| HC-SR501 PIR OUT | **27** | Digital input | Active-high motion detect |
| Lamp MOSFET gate | **25** | PWM output | AO3400A low-side switch |
| OLED SDA | **21** | IÂ²C | SSD1306 @ `0x3C` |
| OLED SCL | **22** | IÂ²C | â€” |
| Mode button | **14** | Input pullup â†’ GND | Toggles AUTO â†” MANUAL |
| Brightness UP | **26** | Input pullup â†’ GND | Manual mode only |
| Brightness DOWN | **33** | Input pullup â†’ GND | Manual mode only |

### Key Constants

| Constant | Value | Notes |
|---|---|---|
| `DARK_THRESHOLD` | 1800 | ADC threshold â€” **uncalibrated, needs real-world tuning** |
| `MOTION_TIMEOUT_MS` | 30000 ms | 30s after last motion â†’ dim lamp |
| `DEBOUNCE_MS` | 180 ms | Button debounce window |
| `manualBrightness` default | 180 / 255 | ~70% duty cycle |

---

## ðŸ’° BOM Summary (India Sourced)

| Component | Cost (USD) | Source |
|---|---|---|
| ESP32 DevKit V1 | $5.00 | [thinkingrobot.in](https://thinkingrobot.in/products/esp32-wroom-32-devkit-v1-wi-fi-bluetooth-iot-board) |
| 0.96â€³ SSD1306 OLED | $2.24 | [robu.in](https://robu.in/product-category/oled-display/) |
| HC-SR501 PIR | $0.63 | [robu.in](https://robu.in/product/pir-motion-sensor-detector-module-hc-sr501/) |
| LDR Photoresistor | $0.18 | [robu.in](https://robu.in/product/4mm-ldr-sensor-photoresistor-photo-cell-5-10k-gl4516) |
| 10kÎ© Resistor | $0.01 | robu.in |
| 220Î© Resistors | $0.01 | robu.in |
| AO3400A MOSFET | ~$0.50 | robu.in |
| Misc (buttons, wires, etc.) | ~$5.17 | â€” |
| **Total estimate** | **~$13.74** | Excl. shipping |

---

## ðŸ§  Firmware Architecture

```mermaid
flowchart TD
    A["loop()"] --> B["Read LDR (ADC)"]
    B --> C["Read PIR"]
    C --> D{"Mode?"}
    D -->|AUTO| E{"Room bright?"}
    E -->|Yes| F["Lamp OFF"]
    E -->|No| G{"Recent motion?"}
    G -->|Yes| H["Lamp 100%"]
    G -->|No| I["Lamp 40% (dim)"]
    D -->|MANUAL| J["Use manual brightness"]
    F --> K["Update OLED"]
    H --> K
    I --> K
    J --> K
    K --> L["Check buttons"]
    L --> A
```

**Key firmware features:**
- **AUTO mode**: Dark + motion â†’ 100% Â· Dark + no motion (30s timeout) â†’ 40% dim Â· Bright room â†’ off
- **MANUAL mode**: UP/DOWN buttons adjust brightness in steps
- **Button debounce**: 180ms window, INPUT_PULLUP, active-low
- **OLED fallback**: `oledReady` flag â€” graceful degradation if OLED fails to init
- **Libraries**: Adafruit GFX + Adafruit SSD1306

---

## âš ï¸ Blockers & Open Items

| # | Item | Type | Status |
|---|---|---|---|
| 1 | **Git not installed** on dev machine | Tooling | ðŸ”§ Installing now |
| 2 | **CAD / Enclosure design** not started | Deliverable | â¬œ Needs dimensions, cutouts, mounting |
| 3 | **Hardware not arrived** | Physical | ðŸšš Awaiting components |
| 4 | **`DARK_THRESHOLD` uncalibrated** | Firmware | â³ Needs real LDR readings |
| 5 | **PIR output polarity** unverified | Hardware | â³ Verify with actual HC-SR501 |
| 6 | **OLED IÂ²C address** assumed `0x3C` | Hardware | â³ Verify on real module |
| 7 | **LDR divider orientation** assumed | Hardware | â³ Verify ADC direction |

---

## ðŸŽ¯ Next Actions (Priority Order)

1. **Install Git** â†’ clone repo locally â†’ enable direct pushes
2. **CAD enclosure package** â€” dimensions, OLED cutout, button holes, PIR window, LDR opening, LED diffuser chamber, USB access, ESP32 mounting
3. **Enclosure concept render** â€” match existing NightLite design language
4. **Wait for hardware** â†’ physical assembly â†’ calibrate `DARK_THRESHOLD`
5. **Integration testing** â†’ verify all sensors + OLED + buttons working together
6. **Final submission prep** â€” photos, demo video, journal updates

---

> [!NOTE]
> This dashboard is a living document. It will be updated as the project progresses through the 17-hour plan. The repo currently has **BOM + firmware + wiring + schematic + design docs + CAD placeholder + journal + 17-hour plan + concept visualization** â€” 9 of 17 hours' worth of deliverables are committed.
