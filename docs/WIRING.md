# NightLite Wiring

## Core connections

| Part | ESP32 |
|---|---|
| LDR divider output | GPIO34 (ADC) |
| PIR OUT | GPIO27 |
| MOSFET gate | GPIO25 |
| OLED SDA | GPIO21 |
| OLED SCL | GPIO22 |
| OLED VCC | 3.3V |
| OLED GND | GND |
| Mode button | GPIO14 to GND |
| Brightness + button | GPIO26 to GND |
| Brightness - button | GPIO33 to GND |

## LDR divider

Connect the LDR and a 10k ohm resistor as a voltage divider. Feed the divider midpoint to GPIO34.

## Lamp driver

Use a logic-level N-channel MOSFET as a low-side switch. Lamp negative goes to MOSFET drain, MOSFET source goes to GND, and GPIO25 drives the gate. The lamp positive terminal goes to the regulated 5V supply.

**Important:** never connect the lamp load directly to an ESP32 GPIO.

## Power

Prototype from a regulated USB 5V supply. The ESP32 can be powered from its USB connector. Keep grounds common between the ESP32, PIR, OLED and lamp driver.
