# NightLite Enclosure Design

This document specifies the enclosure design for the NightLite smart desk lamp.

## Design philosophy

The enclosure should:
- Look like a small desk lamp, not a project box with wires sticking out.
- Be compact enough for a nightstand or desk corner.
- Allow easy access to USB for programming and power.
- Expose sensors (PIR, LDR) while keeping the electronics protected.
- Be 3D-printable in PLA with no supports required (or minimal supports).

## Overall dimensions

| Parameter | Value | Notes |
|---|---|---|
| Width | 80 mm | Fits ESP32 DevKit (51 mm wide) + clearance |
| Depth | 65 mm | Fits ESP32 DevKit (28 mm deep) + wiring clearance |
| Height | 110 mm | Lamp section on top, electronics below |
| Wall thickness | 2.0 mm | Strong enough for PLA, printable |
| Corner radius | 3 mm | Rounded for aesthetics |

## Internal layout (bottom to top)

```
┌────────────────────────────────┐  110 mm
│          LED DIFFUSER          │  ← Top: frosted/translucent cap
│         (lamp section)         │
│  ┌──────────────────────────┐  │  70 mm
│  │                          │  │
│  │     LED strip/module     │  │  ← LEDs face upward into diffuser
│  │                          │  │
│  ├──────────────────────────┤  │  60 mm
│  │      PIR sensor dome     │  │  ← PIR window (side-facing)
│  ├──────────────────────────┤  │  45 mm
│  │                          │  │
│  │     OLED display          │  │  ← Front-facing cutout
│  │     (128x64, 0.96")      │  │
│  │                          │  │
│  ├──────────────────────────┤  │  25 mm
│  │  [MODE] [UP] [DOWN]      │  │  ← 3 button holes (front face)
│  ├──────────────────────────┤  │  15 mm
│  │                          │  │
│  │     ESP32 DevKit V1      │  │  ← Mounted on standoffs
│  │     (with USB port)      │  │
│  │                          │  │
│  └──────────────────────────┘  │  0 mm
│           ┌──────┐             │
│           │ USB  │             │  ← USB access slot (back face)
│           └──────┘             │
└────────────────────────────────┘
```

## Cutout specifications

### OLED display window (front face)
- Position: centered horizontally, 30 mm from bottom
- Cutout size: 27.0 × 15.0 mm (display active area is 25.2 × 12.7 mm)
- Mounting: friction fit or hot-glue from inside

### Button holes (front face)
- Three holes, 7 mm diameter each
- Spacing: 15 mm center-to-center
- Position: centered horizontally, 15 mm from bottom
- Labels: embossed "M" / "+" / "−" above each hole

### PIR window (side face, left or right)
- Position: 55 mm from bottom, centered on side face
- Cutout: 24 mm diameter circle
- The HC-SR501's Fresnel lens dome protrudes slightly through this opening

### LDR opening (top face, near diffuser)
- Position: small 5 mm hole on the top edge (facing upward)
- Purpose: allows the LDR to sense ambient room light, not just the lamp's own light
- Important: the LDR must face **away from** the LED to avoid self-illumination feedback

### USB access slot (back face)
- Position: bottom center of back face
- Cutout: 12 × 8 mm (fits micro-USB or USB-C connector with cable strain relief)

### LED diffuser chamber (top section)
- The top ~40 mm of the enclosure is a separate translucent/frosted cap
- Material: white/translucent PLA, or clear PLA sanded for diffusion
- The LED module sits at the bottom of this chamber, facing upward
- The diffuser softens the light for a pleasant lamp effect

## ESP32 mounting

- Four M2 standoffs (8 mm height) screwed into the base
- The DevKit V1 has mounting holes at standard positions
- USB port faces the back of the enclosure
- All wiring runs underneath the board to keep the interior tidy

## Assembly

The enclosure consists of two main parts:

1. **Base shell** — contains all electronics, buttons, OLED, sensors
2. **Diffuser cap** — snap-fit or friction-fit top section with LED

This two-part design allows:
- Easy access to electronics for debugging and reprogramming
- The diffuser cap can be swapped for different styles/colors
- No screws visible from the outside (snap-fit tabs on the inside)

## Material and printing notes

| Parameter | Recommendation |
|---|---|
| Material | PLA (base), translucent PLA or PETG (diffuser) |
| Layer height | 0.2 mm |
| Infill | 20% (base), 100% (diffuser for light spreading) |
| Supports | Minimal — design oriented to minimize overhangs |
| Print orientation | Base upright, diffuser upside-down |
| Estimated print time | ~3–4 hours (base) + ~1 hour (diffuser) |
| Estimated filament | ~40 g total |
