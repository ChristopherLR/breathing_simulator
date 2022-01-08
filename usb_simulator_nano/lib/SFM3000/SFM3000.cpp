#include <SFM3000.h>
#include <enums.h>
#include <Arduino.h>
#include <Wire.h>

result initialise_sfm(){
  Wire.begin();
  Wire.beginTransmission(FM_ADDRESS);
  Wire.write(0x10);
  Wire.write(0x00);
  Wire.endTransmission();
  return result::ok;
}

void get_flow(result * res, float * value){
  static uint16_t a;
  static uint8_t b;

  if (2 == Wire.requestFrom(FM_ADDRESS, 2)) {
    a = Wire.read();
    b = Wire.read();
    a = (a << 8) | b;
    *value = ((float)a - OFFSET)/SCALE;
    *res = result::ok;
  } else {
    *res = result::err;
  }
}