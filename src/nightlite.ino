/*
  NightLite - automatic desk/night lamp
  ESP32 + LDR + PIR + SSD1306 OLED + MOSFET + buttons

  Behavior:
  - AUTO mode: lamp responds to darkness + motion.
  - Bright room: lamp off.
  - Dark + motion: lamp brightens.
  - Dark + no motion after timeout: lamp dims/off.
  - MANUAL mode: buttons control brightness.
*/

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define OLED_W 128
#define OLED_H 64
Adafruit_SSD1306 display(OLED_W, OLED_H, &Wire, -1);

const int LDR_PIN = 34;
const int PIR_PIN = 27;
const int LAMP_PIN = 25;
const int BTN_MODE = 14;
const int BTN_UP = 26;
const int BTN_DOWN = 33;

const int DARK_THRESHOLD = 1800;
const unsigned long MOTION_TIMEOUT = 30000;

enum Mode { AUTO_MODE, MANUAL_MODE };
Mode mode = AUTO_MODE;
int manualBrightness = 180;
unsigned long lastMotion = 0;

int readLightPercent() {
  int raw = analogRead(LDR_PIN);
  int pct = map(raw, 0, 4095, 100, 0);
  return constrain(pct, 0, 100);
}

void setLamp(int value) {
  analogWrite(LAMP_PIN, constrain(value, 0, 255));
}

void drawStatus(int lightPct, bool motion, int lampPct) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("NIGHTLITE");
  display.printf("Light: %d%%\n", lightPct);
  display.printf("Motion: %s\n", motion ? "YES" : "NO");
  display.printf("Lamp: %d%%\n", lampPct);
  display.printf("Mode: %s\n", mode == AUTO_MODE ? "AUTO" : "MANUAL");
  display.display();
}

void setup() {
  pinMode(PIR_PIN, INPUT);
  pinMode(BTN_MODE, INPUT_PULLUP);
  pinMode(BTN_UP, INPUT_PULLUP);
  pinMode(BTN_DOWN, INPUT_PULLUP);
  analogReadResolution(12);

  Wire.begin();
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);

  lastMotion = millis();
  setLamp(0);
}

void loop() {
  static bool prevMode = HIGH;
  bool modeNow = digitalRead(BTN_MODE);
  if (prevMode == HIGH && modeNow == LOW) {
    mode = (mode == AUTO_MODE) ? MANUAL_MODE : AUTO_MODE;
    delay(150);
  }
  prevMode = modeNow;

  if (digitalRead(BTN_UP) == LOW) {
    manualBrightness = min(255, manualBrightness + 15);
    delay(120);
  }
  if (digitalRead(BTN_DOWN) == LOW) {
    manualBrightness = max(0, manualBrightness - 15);
    delay(120);
  }

  int lightPct = readLightPercent();
  int rawLight = analogRead(LDR_PIN);
  bool motion = digitalRead(PIR_PIN);

  if (motion) lastMotion = millis();

  int lamp = 0;
  if (mode == MANUAL_MODE) {
    lamp = manualBrightness;
  } else if (rawLight < DARK_THRESHOLD) {
    lamp = motion || (millis() - lastMotion < MOTION_TIMEOUT) ? 255 : 40;
  }

  setLamp(lamp);
  drawStatus(lightPct, motion, map(lamp, 0, 255, 0, 100));
  delay(80);
}
