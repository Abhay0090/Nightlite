/*
  NightLite — automatic desk / night lamp
  Target : ESP32 DevKit V1  (Arduino framework)
  Author : Abhay
  Repo   : github.com/Abhay0090/Nightlite

  Libraries (install via Arduino IDE Library Manager):
    - Adafruit GFX Library
    - Adafruit SSD1306

  Pin map
  ───────────────────────────────────────────
    GPIO34  LDR divider midpoint       (ADC, input-only)
    GPIO27  HC-SR501 PIR OUT           (digital input)
    GPIO25  PWM lamp — MOSFET gate     (PWM output)
    GPIO21  OLED SDA                   (I²C)
    GPIO22  OLED SCL                   (I²C)
    GPIO14  MODE button → GND          (input pullup)
    GPIO26  BRIGHTNESS+ button → GND   (input pullup)
    GPIO33  BRIGHTNESS− button → GND   (input pullup)
  ───────────────────────────────────────────

  Physical hardware is NOT yet claimed as tested.
  DARK_THRESHOLD must be calibrated on real hardware.
*/

// ─── Includes ────────────────────────────────────────────────────────────────
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// ─── OLED configuration ─────────────────────────────────────────────────────
constexpr int      OLED_W    = 128;
constexpr int      OLED_H    = 64;
constexpr uint8_t  OLED_ADDR = 0x3C;   // typical SSD1306 address
constexpr int      OLED_RST  = -1;     // no dedicated reset pin

Adafruit_SSD1306 display(OLED_W, OLED_H, &Wire, OLED_RST);
bool oledReady = false;                // tracks whether OLED initialised OK

// ─── Pin definitions ─────────────────────────────────────────────────────────
constexpr int PIN_LDR      = 34;       // ADC1 channel 6
constexpr int PIN_PIR      = 27;
constexpr int PIN_LAMP     = 25;
constexpr int PIN_BTN_MODE = 14;
constexpr int PIN_BTN_UP   = 26;
constexpr int PIN_BTN_DOWN = 33;

// ─── PWM configuration (LEDC) ───────────────────────────────────────────────
constexpr int    PWM_CHANNEL = 0;
constexpr int    PWM_FREQ    = 5000;   // 5 kHz — inaudible, fine for LEDs
constexpr int    PWM_RES     = 8;      // 8-bit → 0-255

// ─── Tuning constants ────────────────────────────────────────────────────────
// DARK_THRESHOLD: ADC reading below which the room is considered "dark".
// This is an initial design estimate.  After assembling the circuit:
//   1. Read serial output in a well-lit room  → note the ADC value.
//   2. Read serial output in a dark room      → note the ADC value.
//   3. Set DARK_THRESHOLD midway between those two readings.
constexpr int           DARK_THRESHOLD     = 1800;

// AUTO-mode lamp levels (0-255 PWM duty)
constexpr int           LAMP_FULL          = 255;   // motion + dark
constexpr int           LAMP_DIM           = 102;   // ~40 % — dark, no recent motion
constexpr int           LAMP_OFF           = 0;

// Timeouts
constexpr unsigned long MOTION_TIMEOUT_MS  = 30000;  // 30 s after last motion → dim
constexpr unsigned long DEBOUNCE_MS        = 180;    // button debounce window

// Manual-mode brightness step (per button press)
constexpr int           BRIGHTNESS_STEP    = 25;

// Display refresh interval (avoid flickering from too-frequent redraws)
constexpr unsigned long DISPLAY_INTERVAL   = 250;    // ms

// Serial debug baud rate
constexpr unsigned long SERIAL_BAUD        = 115200;

// ─── Modes ───────────────────────────────────────────────────────────────────
enum class Mode : uint8_t { AUTO, MANUAL };

// ─── State variables ─────────────────────────────────────────────────────────
Mode          currentMode       = Mode::AUTO;
int           manualBrightness  = 180;           // default ~70 %
int           currentLampPWM    = 0;             // last written PWM value
unsigned long lastMotionMs      = 0;
unsigned long lastDisplayMs     = 0;

// Per-button debounce state
struct Button {
  int           pin;
  bool          lastStable;
  unsigned long lastChangeMs;
};

Button btnMode = { PIN_BTN_MODE, HIGH, 0 };
Button btnUp   = { PIN_BTN_UP,   HIGH, 0 };
Button btnDown = { PIN_BTN_DOWN, HIGH, 0 };

// ─── Forward declarations ────────────────────────────────────────────────────
int           readLightRaw();
int           rawToPercent(int raw);
bool          isDark(int raw);
bool          isMotionDetected();
bool          isMotionRecent();
void          setLamp(int pwm);
bool          buttonPressed(Button &btn);
void          handleButtons();
void          runAutoMode(int lightRaw);
void          runManualMode();
void          updateOLED(int lightRaw, bool motion);
void          serialDebug(int lightRaw, bool motion);

// ═══════════════════════════════════════════════════════════════════════════════
//  SETUP
// ═══════════════════════════════════════════════════════════════════════════════
void setup() {
  // ── Serial ─────────────────────────────────────────────────────────────────
  Serial.begin(SERIAL_BAUD);
  Serial.println();
  Serial.println(F("========================================"));
  Serial.println(F("   NightLite — firmware starting        "));
  Serial.println(F("========================================"));

  // ── Pin modes ──────────────────────────────────────────────────────────────
  pinMode(PIN_LDR,      INPUT);          // ADC — no pullup needed
  pinMode(PIN_PIR,      INPUT);          // PIR module drives this pin
  pinMode(PIN_BTN_MODE, INPUT_PULLUP);   // buttons are active-LOW
  pinMode(PIN_BTN_UP,   INPUT_PULLUP);
  pinMode(PIN_BTN_DOWN, INPUT_PULLUP);

  // ── LEDC PWM for lamp ─────────────────────────────────────────────────────
  ledcSetup(PWM_CHANNEL, PWM_FREQ, PWM_RES);
  ledcAttachPin(PIN_LAMP, PWM_CHANNEL);
  setLamp(LAMP_OFF);

  // ── OLED init ──────────────────────────────────────────────────────────────
  Wire.begin();                          // SDA=21, SCL=22 by default on DevKit V1
  if (display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    oledReady = true;
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.println(F("NightLite"));
    display.println(F("starting..."));
    display.display();
    Serial.println(F("[OLED] initialised OK"));
  } else {
    oledReady = false;
    Serial.println(F("[OLED] FAILED — running without display"));
  }

  // ── PIR warm-up (HC-SR501 needs ~30-60 s, but we don't block) ─────────────
  Serial.println(F("[PIR]  warming up (may be noisy for ~60 s)"));

  Serial.println(F("[INIT] complete — entering main loop"));
  Serial.println();
}

// ═══════════════════════════════════════════════════════════════════════════════
//  LOOP
// ═══════════════════════════════════════════════════════════════════════════════
void loop() {
  // 1. Read sensors
  int  lightRaw = readLightRaw();
  bool motion   = isMotionDetected();

  // 2. Track last motion time
  if (motion) {
    lastMotionMs = millis();
  }

  // 3. Handle button presses (mode toggle, brightness adjust)
  handleButtons();

  // 4. Run the active mode
  switch (currentMode) {
    case Mode::AUTO:   runAutoMode(lightRaw);  break;
    case Mode::MANUAL: runManualMode();         break;
  }

  // 5. Update display (throttled)
  if (millis() - lastDisplayMs >= DISPLAY_INTERVAL) {
    lastDisplayMs = millis();
    updateOLED(lightRaw, motion);
    serialDebug(lightRaw, motion);
  }

  delay(20);   // small yield — keeps loop responsive without busy-spinning
}

// ═══════════════════════════════════════════════════════════════════════════════
//  SENSOR HELPERS
// ═══════════════════════════════════════════════════════════════════════════════

/** Read raw 12-bit ADC value from the LDR voltage divider. */
int readLightRaw() {
  return analogRead(PIN_LDR);
}

/**
 * Convert a raw ADC reading to a 0-100 "light" percentage.
 *
 * Divider orientation used here:
 *   3.3 V -> LDR -> GPIO34 -> 10 kOhm -> GND
 *
 * In this configuration:
 *   - bright light  -> LDR resistance drops -> voltage rises -> higher ADC
 *   - dark          -> LDR resistance rises -> voltage drops -> lower ADC
 *
 * So higher ADC = brighter.  We map 0->0 %, 4095->100 %.
 */
int rawToPercent(int raw) {
  return constrain(map(raw, 0, 4095, 0, 100), 0, 100);
}

/** Is the room considered "dark"? */
bool isDark(int raw) {
  return raw < DARK_THRESHOLD;
}

/** Instantaneous PIR reading (active-HIGH for HC-SR501). */
bool isMotionDetected() {
  return digitalRead(PIN_PIR) == HIGH;
}

/** Was motion seen within the timeout window? */
bool isMotionRecent() {
  return (millis() - lastMotionMs) < MOTION_TIMEOUT_MS;
}

// ═══════════════════════════════════════════════════════════════════════════════
//  LAMP CONTROL
// ═══════════════════════════════════════════════════════════════════════════════

/** Write a 0-255 PWM value to the lamp MOSFET gate. */
void setLamp(int pwm) {
  pwm = constrain(pwm, 0, 255);
  currentLampPWM = pwm;
  ledcWrite(PWM_CHANNEL, pwm);
}

// ═══════════════════════════════════════════════════════════════════════════════
//  MODE LOGIC
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * AUTO mode logic:
 *   bright room           -> lamp OFF
 *   dark + recent motion  -> lamp FULL
 *   dark + no motion      -> lamp DIM (after timeout)
 */
void runAutoMode(int lightRaw) {
  if (!isDark(lightRaw)) {
    setLamp(LAMP_OFF);
  } else if (isMotionRecent()) {
    setLamp(LAMP_FULL);
  } else {
    setLamp(LAMP_DIM);
  }
}

/** MANUAL mode: lamp follows manualBrightness (adjusted by buttons). */
void runManualMode() {
  setLamp(manualBrightness);
}

// ═══════════════════════════════════════════════════════════════════════════════
//  BUTTON HANDLING
// ═══════════════════════════════════════════════════════════════════════════════

/**
 * Returns true on a falling edge (button just pressed).
 * Buttons are INPUT_PULLUP, so pressed = LOW.
 */
bool buttonPressed(Button &btn) {
  bool reading = digitalRead(btn.pin);
  if (reading != btn.lastStable && (millis() - btn.lastChangeMs) > DEBOUNCE_MS) {
    btn.lastChangeMs = millis();
    btn.lastStable   = reading;
    if (reading == LOW) {       // falling edge = press
      return true;
    }
  }
  return false;
}

/** Poll all three buttons and apply their actions. */
void handleButtons() {
  // MODE button — toggle AUTO <-> MANUAL
  if (buttonPressed(btnMode)) {
    if (currentMode == Mode::AUTO) {
      currentMode = Mode::MANUAL;
      Serial.println(F("[BTN]  mode -> MANUAL"));
    } else {
      currentMode = Mode::AUTO;
      Serial.println(F("[BTN]  mode -> AUTO"));
    }
  }

  // BRIGHTNESS UP (only meaningful in MANUAL mode, but we allow adjusting
  // the stored value in AUTO too so it's ready when the user switches)
  if (buttonPressed(btnUp)) {
    manualBrightness = min(255, manualBrightness + BRIGHTNESS_STEP);
    Serial.print(F("[BTN]  brightness+ -> "));
    Serial.println(manualBrightness);
  }

  // BRIGHTNESS DOWN
  if (buttonPressed(btnDown)) {
    manualBrightness = max(0, manualBrightness - BRIGHTNESS_STEP);
    Serial.print(F("[BTN]  brightness- -> "));
    Serial.println(manualBrightness);
  }
}

// ═══════════════════════════════════════════════════════════════════════════════
//  DISPLAY
// ═══════════════════════════════════════════════════════════════════════════════

/** Draw the OLED status dashboard. */
void updateOLED(int lightRaw, bool motion) {
  if (!oledReady) return;

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  // ── Row 0: Title ───────────────────────────────────────────────────────────
  display.setCursor(0, 0);
  display.print(F("NightLite"));
  display.setCursor(80, 0);
  display.print(currentMode == Mode::AUTO ? F("[AUTO]") : F("[MAN]"));

  // ── Row 1: Light level ─────────────────────────────────────────────────────
  display.setCursor(0, 14);
  display.print(F("Light: "));
  display.print(rawToPercent(lightRaw));
  display.print(F("% "));
  display.print(isDark(lightRaw) ? F("(dark)") : F("(bright)"));

  // ── Row 2: Motion status ───────────────────────────────────────────────────
  display.setCursor(0, 26);
  display.print(F("Motion: "));
  display.print(motion ? F("YES") : F("no"));
  if (isMotionRecent() && !motion) {
    unsigned long ago = (millis() - lastMotionMs) / 1000;
    display.print(F(" ("));
    display.print(ago);
    display.print(F("s ago)"));
  }

  // ── Row 3: Lamp output ─────────────────────────────────────────────────────
  display.setCursor(0, 38);
  display.print(F("Lamp: "));
  int pct = map(currentLampPWM, 0, 255, 0, 100);
  display.print(pct);
  display.print(F("%"));

  // ── Row 4: Brightness bar ──────────────────────────────────────────────────
  display.setCursor(0, 52);
  display.print(F("Brt:"));
  int barW = map(currentLampPWM, 0, 255, 0, 90);
  display.drawRect(30, 52, 92, 10, SSD1306_WHITE);
  display.fillRect(31, 53, barW, 8, SSD1306_WHITE);

  display.display();
}

/** Print a compact debug line to Serial. */
void serialDebug(int lightRaw, bool motion) {
  Serial.print(F("[DBG] ADC="));
  Serial.print(lightRaw);
  Serial.print(F("  light="));
  Serial.print(rawToPercent(lightRaw));
  Serial.print(F("%  dark="));
  Serial.print(isDark(lightRaw) ? F("Y") : F("N"));
  Serial.print(F("  motion="));
  Serial.print(motion ? F("Y") : F("N"));
  Serial.print(F("  recent="));
  Serial.print(isMotionRecent() ? F("Y") : F("N"));
  Serial.print(F("  lamp="));
  Serial.print(currentLampPWM);
  Serial.print(F("/255  mode="));
  Serial.println(currentMode == Mode::AUTO ? F("AUTO") : F("MANUAL"));
}
