#pragma once

#include <stdint.h>

class ConstProfile {
 public:
  ConstProfile() : flow_(0.0), duration_(0), confirmed_(0){};
  ConstProfile(float flow, uint32_t duration)
      : flow_(flow), duration_(duration), confirmed_(0){};

  void Print();
  uint8_t Invalid();
  uint8_t Confirm();
  float GetFlow();
  uint8_t GetConfirmed();
  uint32_t GetDuration();

 private:
  float flow_;
  uint32_t duration_;
  uint8_t confirmed_;
};

class DynamicProfile {
 public:
  DynamicProfile() : count_(0), interval_(0), duration_(0), confirmed_(0){};
  DynamicProfile(uint32_t count, uint16_t interval, uint32_t duration)
      : count_(count),
        interval_(interval),
        duration_(duration),
        confirmed_(0){};

  void Print();
  uint8_t Invalid();
  void SetInterval(uint16_t interval, float flow);
  float GetFlow(uint16_t runtime);
  uint8_t GetConfirmed();
  uint32_t GetDuration();
  uint8_t Confirm();

 private:
  uint32_t count_;
  uint16_t interval_;
  uint32_t duration_;
  uint8_t confirmed_;
  float flow_setpoint_[10000];
};