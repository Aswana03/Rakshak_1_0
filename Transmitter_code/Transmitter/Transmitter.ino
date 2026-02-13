#include <SPI.h>
#include <LoRa.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
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

// ===================== ADXL335 =====================
#define PIN_X 32
#define PIN_Y 33
#define PIN_Z 34

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

// ===================== OLED FUNCTIONS =====================
void showSafe() {
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(25, 25);
  display.println("SAFE");
  display.display();
}

void showFall() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(15, 20);
  display.println("FALL DETECTED");
  display.display();
}

// ===================== SETUP =====================
void setup() {
  Serial.begin(115200);
  delay(2000);

  // ---------- OLED INIT ----------
  Wire.begin(21, 22);
  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    Serial.println("OLED not found");
    while (1);
  }
  showSafe();

  // ---------- ADC ----------
  analogReadResolution(12);
  analogSetAttenuation(ADC_11db);

  Serial.println("Calibrating ADXL335...");
  calibrateSensor();
  Serial.println("Calibration done");

  // ---------- LoRa ----------
  pinMode(LORA_SS, OUTPUT);
  digitalWrite(LORA_SS, HIGH);

  SPI.begin(18, 19, 23, LORA_SS);
  LoRa.setPins(LORA_SS, LORA_RST, LORA_DIO0);

  if (!LoRa.begin(433E6)) {
    Serial.println("LoRa init failed");
    while (1);
  }

  LoRa.setSpreadingFactor(7);
  LoRa.setSignalBandwidth(125E3);
  LoRa.setCodingRate4(5);
  LoRa.setSyncWord(0x12);
  LoRa.setTxPower(17);

  Serial.println("System Ready");
}

// ===================== LOOP =====================
void loop() {

  float gx = readAxisG(PIN_X);
  float gy = readAxisG(PIN_Y);
  float gz = readAxisG(PIN_Z);

  float magnitude = sqrt(gx * gx + gy * gy + gz * gz);
  unsigned long now = millis();

  // -------- FREE FALL DETECTION --------
  if (magnitude < FREEFALL_THRESHOLD_G) {
    if (!inFreefall) {
      inFreefall = true;
      freefallStart = now;
    }
  } 
  else {
    if (inFreefall && (now - freefallStart >= FREEFALL_MIN_MS)) {
      if (waitForImpact()) {
        Serial.println("🚨 FALL DETECTED");
        showFall();
        sendLoRaFallAlert();
        fallReported = true;
      }
      inFreefall = false;
    }
  }

  // -------- RESET TO SAFE --------
  if (fallReported && magnitude > 0.9 && magnitude < 1.2) {
    fallReported = false;
    showSafe();
  }

  delay(5);
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

void sendLoRaFallAlert() {
  LoRa.setPreambleLength(12);
  LoRa.enableCrc();

  for (int i = 0; i < 5; i++) {
    LoRa.beginPacket();
    LoRa.print("FALL DETECTED");
    LoRa.endPacket(true);
    delay(200);
  }
  Serial.println("📡 LoRa alert sent");
}
