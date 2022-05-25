#pragma once

#include <Arduino.h>
#include <ArduinoJson.h>
#include <elapsedMillis.h>
#include <enums.h>
#include <stdint.h>

#include "flow_meter.h"
#include "flow_profile.h"
#include "motor_controller.h"
#include "pid.h"

class Fan {
 public:
  Fan(MotorController& mc, FlowMeter& fm, Pid& pid, uint8_t trigger1,
      uint8_t trigger2, uint8_t reversible)
      : motor_(mc),
        flow_meter_(fm),
        pid_(pid),
        trigger1_(trigger1),
        trigger2_(trigger2),
        reversible_(reversible),
        print_interval_(5){};

  uint8_t Initialise();
  uint8_t RunConstProfile(ConstProfile& profile);
  uint8_t RunDynamicProfile(DynamicProfile& profile);
  void SetTrigger1Delay(uint16_t delay) { trigger1_delay_ = delay; };
  void SetTrigger2Delay(uint16_t delay) { trigger2_delay_ = delay; };
  void Prepare();
  void Stop();

  elapsedMillis fan_runtime;
  elapsedMillis sample_time;

 private:
  MotorController& motor_;
  FlowMeter& flow_meter_;
  Pid& pid_;
  uint8_t trigger1_;
  uint8_t trigger2_;
  uint8_t reversible_;
  uint8_t print_interval_;
  uint16_t trigger1_delay_;
  uint16_t trigger2_delay_;
  uint8_t sent_start_;
  uint8_t waiting_for_end_;
};