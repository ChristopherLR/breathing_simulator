#include <Arduino.h>
#include <Wire.h>
#include <stdint.h>

#define OFFSET 12288
#define SCALE 120
#define FM_ADDRESS 0x40
#define BAUD 9600

void ResetSFM();
void StartConstMeasurement();
void StopConstMeasurement();
void ReadFlow();

void setup() {
  Wire.begin();
  Serial.begin(BAUD);

  while (!Serial)
    ;

  Serial.println("TEST");

  ResetSFM();
  StartConstMeasurement();
  delay(10);
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
    flow = (raw_flow + OFFSET) / (float)SCALE;
    Serial.println(flow);
  }
}

void loop() {
  delay(10);
  ReadFlow();
}