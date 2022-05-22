#pragma once

#include <stdint.h>

#define MAX_PWM 4095

class Pid {
 public:
  Pid() : sample_time_(5){};
  void Initialise();
  double kP = 80.0;
  double kI = 0.5;
  double kD = 0.0;
  double Compute(double input);
  void Start();
  void SetSetpoint(double setpoint);

 private:
  uint8_t sample_time_;
  uint8_t hit_start_;
  uint64_t last_time_;
  double output_sum_;
  double last_input_;
  double last_output_;
  double setpoint_;
};
