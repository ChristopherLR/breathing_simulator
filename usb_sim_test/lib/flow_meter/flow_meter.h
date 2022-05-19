#include <stdint.h>

#pragma once

class FlowMeter {
 public:
  virtual uint8_t GetFlow(float* res);
  virtual uint8_t Initialise();
  virtual uint8_t Reset();
};

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