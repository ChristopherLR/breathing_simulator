#include "fan.h"

#include <Arduino.h>
#include <ArduinoJson.h>
#include <elapsedMillis.h>
#include <stdint.h>

#include "enums.h"
#include "flow_meter.h"
#include "flow_profile.h"
#include "motor_controller.h"
#include "pid.h"

uint8_t Fan::Initialise() {
  pinMode(trigger1_, OUTPUT);
  pinMode(trigger2_, OUTPUT);
  digitalWrite(trigger1_, LOW);
  digitalWrite(trigger2_, LOW);

  pid_ = Pid();
  pid_.Initialise();

  uint8_t err = 0;
  err = motor_.Initialise();
  if (err) Serial.println("Error initialising motor");
  err = flow_meter_.Initialise();
  if (err) Serial.println("Error initialising flow meter");
  return err;
};

void Fan::Prepare() {
  sent_start_ = 0;
  fan_runtime = 0;
  sample_time = 0;
}

void Fan::Stop() {
  motor_.Stop();
  sent_start_ = 0;
  digitalWrite(trigger1_, LOW);
  digitalWrite(trigger2_, LOW);
}

uint8_t Fan::RunConstProfile(ConstProfile& profile) {
  if (sent_start_ == 0) {
    Serial.println("start_flow");
    fan_runtime = 0;
    waiting_for_end_ = 0;
    sent_start_ = 1;
    pid_.Start();
    pid_.SetSetpoint((double)profile.GetFlow());
  }

  float flow;
  uint8_t err = flow_meter_.GetFlow(&flow);
  if (!err && sample_time > print_interval_) {
    Serial.print("f:");
    Serial.print(fan_runtime);
    Serial.print(',');
    Serial.println(flow);
    sample_time = 0;
  }
  if (fan_runtime >= trigger1_delay_) digitalWrite(trigger1_, HIGH);
  if (fan_runtime >= trigger2_delay_) digitalWrite(trigger2_, HIGH);

  if (fan_runtime >= profile.GetDuration()) {
    pid_.SetSetpoint(0.0);
    waiting_for_end_ = 1;
  }

  if (!err) {
    double output = pid_.Compute(flow);
    if (output >= 0) {
      motor_.Forward((uint16_t)output);
    } else {
      motor_.Reverse((uint16_t)(-output));
    }

    if (waiting_for_end_ && flow < 3.0) {
      Stop();
      return 1;  // DONE
    }
  }

  return 0;
}

uint8_t Fan::RunDynamicProfile(DynamicProfile& profile) {
  if (sent_start_ == 0) {
    Serial.println("start_dynamic_flow");
    fan_runtime = 0;
    waiting_for_end_ = 0;
    sent_start_ = 1;
    pid_.Start();
  }

  float flow;
  uint8_t err = flow_meter_.GetFlow(&flow);
  if (!err && sample_time > print_interval_) {
    Serial.print("f:");
    Serial.print(fan_runtime);
    Serial.print(',');
    Serial.println(flow);
    sample_time = 0;
  }
  if (fan_runtime >= trigger1_delay_) digitalWrite(trigger1_, HIGH);
  if (fan_runtime >= trigger2_delay_) digitalWrite(trigger2_, HIGH);

  if (fan_runtime >= profile.GetDuration()) {
    pid_.SetSetpoint(0.0);
    waiting_for_end_ = 1;
  }

  if (!err) {
    pid_.SetSetpoint(profile.GetFlow(fan_runtime));
    double output = pid_.Compute(flow);
    if (output >= 0) {
      motor_.Forward((uint16_t)output);
    } else {
      motor_.Reverse((uint16_t)(-output));
    }

    if (waiting_for_end_ && flow < 3.0) {
      Stop();
      return 1;  // DONE
    }
  }

  return 0;
}