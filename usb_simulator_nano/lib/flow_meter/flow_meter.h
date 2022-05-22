#pragma once

#include <stdint.h>

class FlowMeter {
 public:
  virtual uint8_t GetFlow(float* res) = 0;
  virtual uint8_t Initialise() = 0;
  virtual uint8_t Reset() = 0;
};

/* I2C Commands (CE)
Start flow measurement:   0x3603 (O2), 0x3608 (Air), 0x3632 (Mix)
Stop flow measurements:   0x3FF9
Read scale, offset, unit: 0x3661
scale:  (Byte 1 - msb, Byte 2 - lsb)
(Byte 3 - CRC)
offset: (Byte 4 - msb, Byte 5 - lsb)
(Byte 6 - CRC)
unit:   (Byte 7 - msb, Byte 8 - lsb)
(Byte 9 - CRC)
Soft reset:               0x0006
Enter sleep mode:         0x3677
Read product id:          0xE102
*/
class SFM3003 : public FlowMeter {
 public:
  uint8_t GetFlow(float* res);
  uint8_t Initialise();
  uint8_t Reset();

 private:
  const int32_t offset_ = -12288;
  const float scale_ = 120;
  const uint8_t address_ = 0x2D;
  uint8_t StartConstMeasurement();
};

/* I2C Commands
Start flow measurement:   0x1000
Start temp measurement:   0x1001
Read scale factor:        0x30DE
Read offset:              0x30DF
Read article number:      0x31E3, 0x31E4
Read serial number:       0x31AF
Soft reset:               0x2000
*/
class SFM3000 : public FlowMeter {
 public:
  uint8_t GetFlow(float* res);
  uint8_t Initialise();
  uint8_t Reset();

 private:
  const int32_t offset_ = 32000;
  const float scale_ = 140;
  const uint8_t address_ = 0x40;
  uint8_t StartConstMeasurement();
};