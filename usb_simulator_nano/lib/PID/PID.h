#pragma once

#include <stdint.h>

#define MAX_PWM 4095

class Pid {
 public:
  Pid(double p, double i, double d) : kP(p), kI(i), kD(d), sample_time_(1){};
  void Initialise();

  void SetP(double p) { kP = p; };
  double kP;
  void SetI(double i) { kI = i; };
  double kI;
  void SetD(double d) { kD = d; };
  double kD;
  void SetPid(double p, double i, double d) {
    kP = p;
    kI = i;
    kD = d;
  }
  double Compute(double input);
  void Start();
  void SetSetpoint(double setpoint) { setpoint_ = setpoint; };
  void SetInitialOutput(double output) { initial_output_ = output; };

 private:
  uint8_t sample_time_;
  uint8_t hit_start_;
  uint64_t last_time_;
  double output_sum_;
  double last_input_;
  double last_output_;
  double setpoint_;
  double initial_output_;
};
