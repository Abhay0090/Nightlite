# NightLite — CAD / Enclosure

This directory contains the enclosure design files for the NightLite smart desk lamp.

## Design overview

The enclosure is a two-part 3D-printable case:

1. **Base shell** — holds ESP32, OLED, buttons, PIR sensor, LDR, and MOSFET circuit
2. **Diffuser cap** — translucent top section that houses the LED and diffuses the light

The two parts connect via a snap-fit joint for tool-free assembly/disassembly.

## Files

| File | Description | Status |
|---|---|---|
| `ENCLOSURE_SPEC.md` | Detailed dimensions, clearances, and cutout coordinates | ✅ Complete |
| `nightlite-base.stl` | Base shell STL | ⬜ Pending (after hardware testing) |
| `nightlite-diffuser.stl` | Diffuser cap STL | ⬜ Pending |
| `nightlite-test-fit.stl` | Test piece for verifying button/OLED fit | ⬜ Pending |

## Dimensions

- **Width**: 80 mm
- **Depth**: 65 mm  
- **Height**: 110 mm (70 mm base + 40 mm diffuser)
- **Wall thickness**: 2.0 mm

## Printing recommendations

- **Material**: PLA (base), translucent PLA or PETG (diffuser)
- **Layer height**: 0.2 mm
- **Infill**: 20% (base), 100% (diffuser)
- **Estimated time**: ~4–5 hours total
- **Estimated filament**: ~40 g

See `ENCLOSURE_SPEC.md` for full component clearances and cutout coordinates.
