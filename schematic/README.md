# Schematic

The electrical architecture is documented in [WIRING.md](../docs/WIRING.md).

Planned signal chain:

LDR -> ESP32 ADC
PIR -> ESP32 digital input
Buttons -> ESP32 digital inputs
ESP32 -> OLED over I2C
ESP32 PWM -> MOSFET gate -> 5V lamp

A formal schematic image will be added after the first breadboard prototype is verified.
