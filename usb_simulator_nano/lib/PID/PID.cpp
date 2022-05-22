#include <Arduino.h>
#include <pid.h>
#include <stdint.h>

void Pid::Initialise() {}

void Pid::Start() {
  hit_start_ = 0;
  last_time_ = millis();
  last_input_ = 0.0;
  last_output_ = 0.0;
  output_sum_ = 0.0;
}

// Look into feed forward
// Kalman filters
double Pid::Compute(double input) {
  uint64_t now = millis();
  uint64_t time_delta = now - last_time_;

  double err = setpoint_ - input;

  if (err < 0) hit_start_ = 1;

  if (time_delta < sample_time_) return last_output_;

  double input_delta = input - last_input_;

  double output = kP * err;
  output_sum_ += kI * err;
  output += output_sum_ - kD * input_delta;

  last_input_ = input;
  last_output_ = output;
  last_time_ = now;

  return output;
}
