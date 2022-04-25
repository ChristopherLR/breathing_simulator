#include <Servo.h>
#include <stdint.h>

#pragma once

class MotorController {
 public:
  virtual void Forward(uint16_t speed);
  virtual void Reverse(uint16_t speed);
  virtual void Stop();
  virtual void Initialise();
};

class ServoMotorController : MotorController {
 public:
  ServoMotorController(uint16_t max_value, uint16_t midpoint,
                       unsigned char signal_pin) {
    max_value_ = max_value;
    forward_max_ = midpoint + max_value;
    reverse_min_ = midpoint - max_value;
    signal_pin_ = signal_pin;
    midpoint_ = midpoint;
  };

  void Forward(uint16_t speed);
  void Reverse(uint16_t speed);
  void Initialise();
  void Stop();

 private:
  uint16_t max_value_;
  uint16_t forward_max_;
  uint16_t reverse_min_;
  uint16_t midpoint_;
  unsigned char signal_pin_;
  Servo motor_;
};