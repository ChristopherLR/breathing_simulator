#include <Arduino.h>
#include <Wire.h>
#include <elapsedMillis.h>
#include <stdint.h>

#define OFFSET -12288
#define SCALE 120
#define FM_ADDRESS 0x2D
#define BAUD 9600

void ResetSFM();
void StartConstMeasurement();
void StopConstMeasurement();
void ReadFlow();

elapsedMillis runtime;
elapsedMillis elapsed;

void setup() {
  Wire.begin();
  Serial.begin(BAUD);

  while (!Serial)
    ;

  ResetSFM();
  StartConstMeasurement();
  delay(10);
  runtime = 0;
  elapsed = 0;
}

void ResetSFM() {
  Wire.beginTransmission(FM_ADDRESS);
  Wire.write(0x00);
  Wire.write(0x06);
  Wire.endTransmission();
}

void StartConstMeasurement() {
  Wire.beginTransmission(FM_ADDRESS);
  Wire.write(0x36);
  Wire.write(0x08);
  Wire.endTransmission();
}

void StopConstMeasurement() {
  Wire.beginTransmission(FM_ADDRESS);
  Wire.write(0x3F);
  Wire.write(0xF9);
  Wire.endTransmission();
}

void ReadFlow() {
  char avail = Wire.requestFrom(FM_ADDRESS, 2);
  static uint8_t msb;
  static uint8_t lsb;
  static int16_t raw_flow;
  static float flow;

  if (avail == 2) {
    msb = Wire.read();
    lsb = Wire.read();
    raw_flow = (msb << 8) | lsb;
    flow = (raw_flow - OFFSET) / (float)SCALE;
    Serial.print(runtime);
    Serial.print(", ");
    Serial.println(flow);
  }
}

void loop() {
  if (elapsed >= 5) {
    ReadFlow();
    elapsed = 0;
  }
}