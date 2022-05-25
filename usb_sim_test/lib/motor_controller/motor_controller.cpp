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

void Calibrate() {
  for (int i = 500; i <= 2500; i += 10) {
    motor.writeMicroseconds(i);
    delay(100);
    Serial.print("p: ");
    Serial.println(i);
  }

  motor.writeMicroseconds(1500);
  Serial.println("fin");
}
