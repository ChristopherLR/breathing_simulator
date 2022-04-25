#include "motor_controller.h"

#include <Arduino.h>
#include <Servo.h>
#include <stdint.h>

void ServoMotorController::Initialise() { motor_.attach(signal_pin_); };

void ServoMotorController::Forward(uint16_t speed) {
  if (speed > max_value_) {
    motor_.writeMicroseconds(forward_max_);
  } else {
    motor_.writeMicroseconds(midpoint_ + speed);
  }
}

void ServoMotorController::Reverse(uint16_t speed) {
  if (speed > max_value_) {
    motor_.writeMicroseconds(reverse_min_);
  } else {
    motor_.writeMicroseconds(midpoint_ - speed);
  }
}

void ServoMotorController::Stop() { motor_.writeMicroseconds(midpoint_); }