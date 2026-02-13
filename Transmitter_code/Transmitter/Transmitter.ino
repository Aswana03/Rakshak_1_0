#include <SPI.h>
#include <LoRa.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <TinyGPSPlus.h>
#include <math.h>

// ===================== OLED =====================
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 42
#define OLED_ADDR 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// ===================== LoRa =====================
#define LORA_SS    5
#define LORA_RST   27
#define LORA_DIO0  26

// ===================== GPS =====================
#define GPS_RX 16   // ESP32 RX ← GPS TX
#define GPS_TX 17   // ESP32 TX → GPS RX
TinyGPSPlus gps;
HardwareSerial gpsSerial(2);

// ===================== ADXL335 =====================
#define PIN_X 32
#define PIN_Y 33
#define PIN_Z 34

// ===================== BUTTON =====================
#define CANCEL_BUTTON 25
#define PANIC_BUTTON 4

#define EVENT_FALL  "F01"
#define EVENT_PANIC "P01"

// ===================== CONSTANTS =====================
const float VCC = 3.3;
const float ADC_MAX = 4095.0;
const float SENSITIVITY = 0.33;

const float FREEFALL_THRESHOLD_G = 0.5;
const unsigned long FREEFALL_MIN_MS = 150;
const float IMPACT_THRESHOLD_G = 3.1;
const unsigned long IMPACT_WINDOW_MS = 500;

// ===================== VARIABLES =====================
float vZeroX, vZeroY, vZeroZ;
unsigned long freefallStart = 0;
bool inFreefall = false;
bool fallReported = false;

unsigned long alertTime = 0;
bool alertActive = false;

// ===================== OLED HELPERS =====================
void showCenteredText(const char* msg, uint8_t size) {
  display.clearDisplay();
  display.setTextSize(size);
  display.setTextColor(SSD1306_WHITE);

  int16_t x1, y1;
  uint16_t w, h;
  display.getTextBounds(msg, 0, 0, &x1, &y1, &w, &h);

  display.setCursor((SCREEN_WIDTH - w) / 2,
                    (SCREEN_HEIGHT - h) / 2);
  display.println(msg);
  display.display();
}

bool showCountdownWithCancel(int seconds) {
  for (int i = seconds; i > 0; i--) {

    // ---- CHECK BUTTON ----
    if (digitalRead(CANCEL_BUTTON) == LOW) {
      showCenteredText("CANCELLED", 1);
      delay(1000);
      showCenteredText("SAFE", 2);
      return false;   // cancelled
    }

    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(20, 10);
    display.print("Sending in");
    display.setCursor(45, 30);
    display.print(i);
    display.display();

    delay(1000);
  }
  return true; // countdown completed
}


void showMessageSent() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(5, 15);
  display.println("MESSAGE");
  display.setCursor(30, 30);
  display.println("SENT");
  display.display();
}

void showStartupAnimation() {

  unsigned long startTime = millis();

  int barX = 38;
  int barY = 20;
  int barLength = 8;

  // ===== 5 SECONDS LOADING BAR =====
  while (millis() - startTime < 5000) {

    for (int i = 0; i <= barLength; i++) {

      display.clearDisplay();
      display.setTextSize(1);
      display.setTextColor(SSD1306_WHITE);

      display.setCursor(barX - 6, barY);
      display.print("[");

      for (int j = 0; j < barLength; j++) {
        if (j < i) display.print((char)219);
        else display.print(" ");
      }

      display.print("]");

      display.display();
      delay(150);

      if (millis() - startTime >= 5000) break;
    }
  }

  // ===== SHOW RAKSHAK 1.0 AFTER LOADING =====
  display.clearDisplay();
  display.setTextSize(1.8);
  display.setTextColor(SSD1306_WHITE);

  const char* text = "RAKSHAK 1.0";

  int16_t x1, y1;
  uint16_t w, h;
  display.getTextBounds(text, 0, 0, &x1, &y1, &w, &h);

  display.setCursor(
    (SCREEN_WIDTH - w) / 2,
    (SCREEN_HEIGHT - h) / 2
  );

  display.print(text);
  display.display();

  delay(5000);   // show title for 5 seconds

  display.clearDisplay();
  display.display();
}

// ===================== SETUP =====================
void setup() {
  Serial.begin(115200);
  delay(2000);

  pinMode(CANCEL_BUTTON, INPUT_PULLUP);
  pinMode(PANIC_BUTTON, INPUT_PULLUP);

  // ---------- OLED ----------
  Wire.begin(21, 22);
  display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR);

  showStartupAnimation();     // 🎬 animated RAKSHAK 1.0
  showCenteredText("SAFE", 2);  // 🎬 moving RAKSHAK 1.0



  // ---------- ADC ----------
  analogReadResolution(12);
  analogSetAttenuation(ADC_11db);
  calibrateSensor();

  // ---------- GPS ----------
  gpsSerial.begin(9600, SERIAL_8N1, GPS_RX, GPS_TX);

  // ---------- LoRa ----------
  pinMode(LORA_SS, OUTPUT);
  digitalWrite(LORA_SS, HIGH);

  SPI.begin(18, 19, 23, LORA_SS);
  LoRa.setPins(LORA_SS, LORA_RST, LORA_DIO0);

  if (!LoRa.begin(433E6)) {
    while (1);
  }

  LoRa.setSpreadingFactor(7);
  LoRa.setSignalBandwidth(125E3);
  LoRa.setCodingRate4(5);
  LoRa.setSyncWord(0x12);
  LoRa.setTxPower(17);
}

// ===================== LOOP =====================
void loop() {

  // -------- GPS PARSING (NON-BLOCKING) --------
  while (gpsSerial.available()) {
    gps.encode(gpsSerial.read());
  }

  float gx = readAxisG(PIN_X);
  float gy = readAxisG(PIN_Y);
  float gz = readAxisG(PIN_Z);

  float magnitude = sqrt(gx * gx + gy * gy + gz * gz);
  unsigned long now = millis();

  // -------- PANIC BUTTON (INSTANT ALERT) --------
  if (digitalRead(PANIC_BUTTON) == LOW) {
    showCenteredText("PANIC!", 2);
    sendHelpWithGPS(EVENT_PANIC);
    showMessageSent();
    alertTime = millis();
    alertActive = true;


    // prevent multiple sends on long press
    delay(2000);
  }


  // -------- FALL DETECTION --------
  if (magnitude < FREEFALL_THRESHOLD_G) {
    if (!inFreefall) {
      inFreefall = true;
      freefallStart = now;
    }
  } 
  else {
    if (inFreefall && (now - freefallStart >= FREEFALL_MIN_MS)) {
      if (waitForImpact()) {
    showCenteredText("FALL!", 2);

    // 10-second delay with countdown
   bool proceed = showCountdownWithCancel(10);

    if (proceed) {
      sendHelpWithGPS(EVENT_FALL);
      showMessageSent();
      fallReported = true;
      alertTime = millis();
      alertActive = true;
    } else {
      fallReported = false;
    }

  }

      inFreefall = false;
    }
  }

  // -------- RESET TO SAFE --------
  if (fallReported && magnitude > 0.9 && magnitude < 1.2) {
    fallReported = false;
    showCenteredText("SAFE", 2);
  }

  delay(5);

    // -------- AUTO RETURN TO SAFE AFTER 10 SECONDS --------
  if (alertActive && millis() - alertTime >= 10000) {
    alertActive = false;
    fallReported = false;
    showCenteredText("SAFE", 2);
  }

}

// ===================== FUNCTIONS =====================

float readAxisG(int pin) {
  int raw = analogRead(pin);
  float voltage = (raw * VCC) / ADC_MAX;

  float zero = (pin == PIN_X) ? vZeroX :
               (pin == PIN_Y) ? vZeroY : vZeroZ;

  return (voltage - zero) / SENSITIVITY;
}

void calibrateSensor() {
  long sx = 0, sy = 0, sz = 0;
  for (int i = 0; i < 200; i++) {
    sx += analogRead(PIN_X);
    sy += analogRead(PIN_Y);
    sz += analogRead(PIN_Z);
    delay(5);
  }
  vZeroX = (sx / 200.0) * VCC / ADC_MAX;
  vZeroY = (sy / 200.0) * VCC / ADC_MAX;
  vZeroZ = (sz / 200.0) * VCC / ADC_MAX;
}

bool waitForImpact() {
  unsigned long start = millis();
  while (millis() - start < IMPACT_WINDOW_MS) {
    float gx = readAxisG(PIN_X);
    float gy = readAxisG(PIN_Y);
    float gz = readAxisG(PIN_Z);
    float mag = sqrt(gx * gx + gy * gy + gz * gz);
    if (mag > IMPACT_THRESHOLD_G) return true;
    delay(5);
  }
  return false;
}

// ===================== LORA + GPS MESSAGE =====================
void sendHelpWithGPS(const char* eventCode) {

  String message = "id:123,";
  message += eventCode;
  message += ",";

  if (gps.location.isValid()) {
    message += String(gps.location.lat(), 6);
    message += ",";
    message += String(gps.location.lng(), 6);
  } else {
    message += "NO_GPS";
  }

  LoRa.setPreambleLength(12);
  LoRa.enableCrc();

  for (int i = 0; i < 5; i++) {
    LoRa.beginPacket();
    LoRa.print(message);
    LoRa.endPacket(true);
    delay(200);
  }

  Serial.println(message);
}