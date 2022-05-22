#include <Arduino.h>
#include <Wire.h>
#include <stdint.h>

#define OFFSET 32000
#define SCALE 140.0
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
  Wire.write(0x20);
  Wire.write(0x00);
  Wire.endTransmission();
}

void StartConstMeasurement() {
  Wire.beginTransmission(FM_ADDRESS);
  Wire.write(0x10);
  Wire.write(0x00);
  Wire.endTransmission();
}

void ReadFlow() {
  char avail = Wire.requestFrom(FM_ADDRESS, 2);
  static uint16_t raw_flow;
  static uint8_t lsb;
  static float flow;

  if (avail == 2) {
    raw_flow = Wire.read();
    lsb = Wire.read();
    raw_flow = (raw_flow << 8) | lsb;
    flow = ((float)raw_flow - OFFSET) / SCALE;
    Serial.println(flow);
  }
}

void loop() {
  delay(10);
  ReadFlow();
}