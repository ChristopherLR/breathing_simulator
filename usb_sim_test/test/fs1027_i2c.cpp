#include <Arduino.h>
#include <Wire.h>
#include <stdint.h>

#define FM_ADDRESS 0x50
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

void PrintBin(int var) {
  for (unsigned int test = 0x80; test; test >>= 1) {
    Serial.write(var & test ? '1' : '0');
  }
  Serial.println();
}

void ReadFlow() {
  char avail = Wire.requestFrom(FM_ADDRESS, 5);
  static uint8_t chk1;
  static uint8_t chk2;
  static uint8_t chk3;
  static int8_t msb;
  static int8_t lsb;
  static int16_t raw_flow;
  static float flow;

  if (avail == 5) {
    chk1 = Wire.read();
    msb = Wire.read();
    lsb = Wire.read();
    chk2 = Wire.read();
    chk3 = Wire.read();

    Serial.print("MSB: ");
    // Serial.println(msb);
    PrintBin(msb);
    Serial.print("LSB: ");
    // Serial.println(lsb);
    PrintBin(lsb);
    Serial.print("CHK1: ");
    PrintBin(chk1);
    Serial.print("CHK2: ");
    PrintBin(chk2);
    Serial.print("CHK3: ");
    PrintBin(chk3);

    raw_flow = (msb << 8) | lsb;
    flow = ((raw_flow - 409) / (float)3277) * 200;
    Serial.println(flow);
  }
}

void loop() {
  delay(10);
  ReadFlow();
}