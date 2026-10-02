/*
  NightLite — automatic desk/night lamp
  Target: ESP32 DevKit V1

  Libraries:
    Adafruit GFX Library
    Adafruit SSD1306

  Pin map:
    GPIO34  LDR divider midpoint (ADC)
    GPIO27  HC-SR501 PIR OUT
    GPIO25  PWM lamp MOSFET gate
    GPIO21  OLED SDA
    GPIO22  OLED SCL
    GPIO14  mode button -> GND
    GPIO26  brightness + -> GND
    GPIO33  brightness - -> GND

  Physical hardware is NOT claimed as tested by this repository yet.
*/

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

constexpr int OLED_W = 128;
constexpr int OLED_H = 64;
constexpr uint8_t OLED_ADDR = 0x3C;

Adafruit_SSD1306 display(OLED_W, OLED_H, &Wire, -1);

constexpr int LDR_PIN = 34;
constexpr int PIR_PIN = 27;
constexpr int LAMP_PIN = 25;
constexpr int BTN_MODE = 14;
constexpr int BTN_UP = 26;
constexpr int BTN_DOWN = 33;

constexpr int DARK_THRESHOLD = 1800;
constexpr unsigned long MOTION_TIMEOUT_MS = 30000;
constexpr unsigned long DEBOUNCE_MS = 180;

enum class Mode { AUTO_MODE, MANUAL_MODE };
Mode mode = Mode::AUTO_MODE;

int manualBrightness = 180;
unsigned long lastMotionMs = 0;

bool oledReady = false;

int readLightRaw() {
  return analogRead(LDR_PIN);
}

int readLightPercent(int raw) {
  // LDR divider orientation used by this design:
  // brighter light -> higher ADC reading -> lower darkness percentage.
  return constrain(map(raw, 0, 4095, 100, 0), 0, 100);
}

void setLamp(int pwm) {
  analogWrite(LAMP_PIN, constrain(pwm, 0, 255));
}

bool pressed(int pin) {
  static unsigned long lastModePress = 0;
  static unsigned long lastUpPress = 0;
  static unsigned long lastDownPress = 0;

  unsigned long now = millis();
  unsigned long *lastPress = nullptr;

  if (pin == BTN_MODE) lastPress = &lastModePress;
  else if (pin == BTN_UP) lastPress = &lastUpPress;
  else lastPress = &lastDownPress;

  if (digitalRead(pin) == LOW && now - *lastPress >= DEBOUNCE_MS) {
    *lastPress = now;
    return true;
  }
  return false;
}

void drawStatus(int lightPct, bool motion, int lampPct) {
  if (!oledReady) return;

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("NIGHTLITE");
  display.printf("Light : %3d%%\n", lightPct);
  display.printf("Motion: %s\n", motion ? "YES" : "NO");
  display.printf("Lamp  : %3d%%\n", lampPct);
  display.printf("Mode  : %s\n", mode == Mode::AUTO_MODE ? "AUTO" : "MANUAL");
  display.display();
}

void setup() {
  pinMode(PIR_PIN, INPUT);
  pinMode(BTN_MODE, INPUT_PULLUP);
  pinMode(BTN_UP, INPUT_PULLUP);
  pinMode(BTN_DOWN, INPUT_PULLUP);

  analogReadResolution(12);
  pinMode(LAMP_PIN, OUTPUT);
  setLamp(0);

  Wire.begin(21, 22);
  oledReady = display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR);

  lastMotionMs = millis();
}

void loop() {
  if (pressed(BTN_MODE)) {
    mode = (mode == Mode::AUTO_MODE) ? Mode::MANUAL_MODE : Mode::AUTO_MODE;
  }

  if (mode == Mode::MANUAL_MODE) {
    if (pressed(BTN_UP)) manualBrightness = min(255, manualBrightness + 15);
    if (pressed(BTN_DOWN)) manualBrightness = max(0, manualBrightness - 15);
  }

  const int rawLight = readLightRaw();
  const int lightPct = readLightPercent(rawLight);
  const bool motion = digitalRead(PIR_PIN) == HIGH;

  if (motion) {
    lastMotionMs = millis();
  }

  int lampPwm = 0;

  if (mode == Mode::MANUAL_MODE) {
    lampPwm = manualBrightness;
  } else if (rawLight < DARK_THRESHOLD) {
    const bool recentMotion = (millis() - lastMotionMs) < MOTION_TIMEOUT_MS;
    lampPwm = recentMotion ? 255 : 40;
  }

  setLamp(lampPwm);
  drawStatus(lightPct, motion, map(lampPwm, 0, 255, 0, 100));

  delay(50);
}
