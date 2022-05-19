#include "flow_meter.h"

#include <Wire.h>
#include <stdint.h>

uint8_t SFM3003::Initialise() {
  Wire.begin();
  uint8_t err = 0;
  err = SFM3003::Reset();
  if (err != 0) return 1;
  err = SFM3003::StartConstMeasurement();
  if (err != 0) return 2;
  return 0;
}

uint8_t SFM3003::Reset() {
  Wire.beginTransmission(address_);
  Wire.write(0x00);
  Wire.write(0x06);
  Wire.endTransmission();
  return 0;
}

uint8_t SFM3003::StartConstMeasurement() {
  Wire.beginTransmission(address_);
  Wire.write(0x36);
  Wire.write(0x08);
  Wire.endTransmission();
  return 0;
}

uint8_t SFM3003::GetFlow(float* res) {
  char avail = Wire.requestFrom(address_, 2);
  static int16_t raw_flow;
  static uint8_t lsb;

  if (avail == 2) {
    raw_flow = Wire.read();
    lsb = Wire.read();
    raw_flow = (raw_flow << 8) | lsb;
    *res = (raw_flow - offset_) / (float)scale_;
    return 0;
  }
  return 1;
}