# NightLite Enclosure — CAD Specification

This document provides precise dimensions and clearances for creating the NightLite enclosure in any CAD tool (FreeCAD, Fusion 360, TinkerCAD, OpenSCAD, etc.).

## Component dimensions (measured/datasheet)

### ESP32 DevKit V1
| Parameter | Value |
|---|---|
| PCB length | 51.4 mm |
| PCB width | 28.0 mm |
| PCB thickness | 1.6 mm |
| Total height (with components) | ~10 mm |
| USB connector protrusion | 2.5 mm beyond PCB edge |
| Mounting hole diameter | 2.1 mm (M2 screw) |
| Mounting hole positions | 4 corners, 2.5 mm from edges |

### SSD1306 OLED (0.96″ I2C)
| Parameter | Value |
|---|---|
| Module PCB | 27.3 × 27.8 mm |
| Active display area | 25.2 × 12.7 mm |
| Display position on PCB | Centered horizontally, flush with top edge |
| Pin header | 4 pins, bottom edge |
| Recommended cutout | 27.0 × 15.0 mm |

### HC-SR501 PIR Sensor
| Parameter | Value |
|---|---|
| PCB diameter | 32 mm |
| PCB to dome top | 24 mm |
| Fresnel lens dome diameter | 23 mm |
| Fresnel lens dome height | 11 mm |
| Detection angle | 120° |
| Recommended cutout | 24 mm diameter circle |

### AO3400A MOSFET (SOT-23)
| Parameter | Value |
|---|---|
| Package | 2.9 × 1.6 × 1.1 mm (negligible) |
| Mounting | Soldered to a small breakout or dead-bug on breadboard |

### Tactile Buttons (6×6 mm)
| Parameter | Value |
|---|---|
| Body | 6.0 × 6.0 × 5.0 mm |
| Actuator diameter | 3.5 mm |
| Actuator protrusion | 2.5 mm above body |
| Recommended hole diameter | 7.0 mm |

### LED Module
| Parameter | Value |
|---|---|
| Varies by module chosen | Typically 30–50 mm long |
| Mounting | Hot glue or screws to top platform |

## Enclosure external dimensions

```
        ┌───── 80 mm ─────┐
        │                  │
   ┌────┤                  ├────┐
   │    │   DIFFUSER CAP   │    │  ← 40 mm tall, translucent
   │    │   (snap-fit)     │    │
   ├────┤──────────────────├────┤  ← Snap-fit joint
   │    │                  │    │
   │    │                  │    │
   │    │    BASE SHELL    │    │  ← 70 mm tall, opaque
   │    │                  │    │
   │    │                  │    │
   └────┤                  ├────┘
        │                  │
        └───── 65 mm ─────┘
              (depth)

   Total height: 110 mm
```

## Internal clearances

| Zone | Height range | Contents | Clearance |
|---|---|---|---|
| ESP32 bay | 0–20 mm | DevKit V1 on 8 mm standoffs | 2 mm side, 5 mm above |
| Button zone | 20–30 mm | 3 tactile buttons | Aligned with front face holes |
| OLED zone | 30–50 mm | SSD1306 module | Press-fit into front window |
| PIR zone | 50–65 mm | HC-SR501 module | Dome protrudes through side hole |
| LED platform | 65–70 mm | LED module mounting surface | Flat platform |
| Diffuser zone | 70–110 mm | Empty chamber for light diffusion | Open interior |

## Cutout coordinates (from bottom-left of front face)

| Cutout | X (mm) | Y (mm) | Width (mm) | Height (mm) | Shape |
|---|---|---|---|---|---|
| OLED window | 26.5 | 30.0 | 27.0 | 15.0 | Rectangle, 1 mm radius corners |
| Mode button | 20.0 | 20.0 | 7.0 | — | Circle |
| UP button | 40.0 | 20.0 | 7.0 | — | Circle |
| DOWN button | 60.0 | 20.0 | 7.0 | — | Circle |
| PIR dome (side) | 28.0 | 55.0 | 24.0 | — | Circle |
| LDR hole (top) | 10.0 | — | 5.0 | — | Circle, top face |
| USB slot (back) | 28.0 | 2.0 | 12.0 | 8.0 | Rectangle |

## Snap-fit joint (base ↔ diffuser)

- Joint at 70 mm height
- 4 snap tabs (one per side), each:
  - Width: 8 mm
  - Hook depth: 1.5 mm
  - Hook height: 2 mm
  - Deflection angle: 30°
- The diffuser cap slides down over the base and clicks into place
- Removal: gentle squeeze on two opposite tabs

## STL export checklist

- [ ] Base shell (opaque PLA)
- [ ] Diffuser cap (translucent PLA or PETG)
- [ ] Test fit piece (button holes + OLED window only — for fit verification)
- [ ] Verify all cutouts don't intersect walls at wrong angles
- [ ] Check minimum wall thickness ≥ 1.2 mm everywhere

## File naming convention

```
cad/
├── ENCLOSURE_SPEC.md          ← This file
├── nightlite-base.stl         ← Base shell (when created)
├── nightlite-diffuser.stl     ← Diffuser cap (when created)
├── nightlite-test-fit.stl     ← Test piece (when created)
└── README.md                  ← Overview
```
