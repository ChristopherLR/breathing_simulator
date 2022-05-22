#include <Arduino.h>
#include <sfm3000.h>
#include <Wire.h>
#include <enums.h>

Result InitialiseSfm() {
  Wire.begin();
  Wire.beginTransmission(FM_ADDRESS);
  Wire.write(0x10);
  Wire.write(0x00);
  Wire.endTransmission();
  return Result::kOk;
}

void GetFlow(Result* res, float* value) {
  static uint16_t a;
  static uint8_t b;

  if (2 == Wire.requestFrom(FM_ADDRESS, 2)) {
    a = Wire.read();
    b = Wire.read();
    a = (a << 8) | b;
    *value = ((float)a - OFFSET) / SCALE;
    *res = Result::kOk;
  } else {
    *res = Result::kErr;
  }
}