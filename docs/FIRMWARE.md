# Firmware Notes

## Intended environment

The firmware targets an **ESP32 DevKit V1** using the Arduino framework.

Install these libraries through the Arduino IDE Library Manager:

- Adafruit GFX Library
- Adafruit SSD1306

The firmware is a design-stage prototype. It has not been presented as physically tested yet.

## Control logic

1. Read the LDR divider through GPIO34.
2. Convert the ADC reading into a displayed light percentage.
3. Read PIR motion on GPIO27.
4. Remember the most recent motion timestamp.
5. In AUTO mode:
   - bright room → lamp OFF
   - dark + recent motion → lamp 100%
   - dark + no recent motion → lamp 40%
6. In MANUAL mode, UP/DOWN buttons change brightness.
7. MODE toggles AUTO/MANUAL.
8. OLED shows light, motion, lamp output and mode.

## First hardware calibration

The value `DARK_THRESHOLD = 1800` is an initial design value, not a measured result.

After assembling the real circuit:

- record several ADC values in daylight;
- record several values in the intended dark/night environment;
- choose a threshold with useful separation;
- update the constant;
- document the observed values in the journal.

The LDR divider orientation may invert the ADC relationship depending on which side of the divider contains the LDR. If the displayed percentage behaves backwards, swap the mapping or swap the LDR/resistor positions.

## PWM note

The sketch uses Arduino's `analogWrite()` interface. If the installed ESP32 Arduino core exposes a different PWM API, adapt `setLamp()` to the installed core's LEDC interface.

## Safety

The 5V lamp is switched through a logic-level MOSFET. Do not connect a higher-current lamp directly to an ESP32 GPIO. Keep the ESP32, PIR, OLED and MOSFET driver grounds common.
