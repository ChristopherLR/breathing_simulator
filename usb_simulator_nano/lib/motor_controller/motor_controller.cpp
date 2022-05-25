#include "motor_controller.h"

#include <Arduino.h>

#include "configuration.h"

#if MC_SPARK
#include <Servo.h>
#endif

#include <stdint.h>

#if MC_SPARK
uint8_t Spark::Initialise() {
  motor_.attach(signal_);
  return 0;
};

void Spark::Forward(uint16_t speed) {
  if (speed > max_value_) {
    motor_.writeMicroseconds(forward_max_);
  } else {
    motor_.writeMicroseconds(midpoint_ + speed);
  }
}

void Spark::Reverse(uint16_t speed) {
  if (speed > max_value_) {
    motor_.writeMicroseconds(reverse_min_);
  } else {
    motor_.writeMicroseconds(midpoint_ - speed);
  }
}

void Spark::Stop() { motor_.writeMicroseconds(midpoint_); }
#endif

uint8_t L298N::Initialise() {
  pinMode(in1_, OUTPUT);
  pinMode(in2_, OUTPUT);
  pinMode(en_, OUTPUT);
  pinMode(signal_, OUTPUT);
  digitalWrite(en_, LOW);
  digitalWrite(in1_, LOW);
  digitalWrite(in2_, LOW);
  return 0;
}

void L298N::Forward(uint16_t speed) {
  digitalWrite(en_, HIGH);
  digitalWrite(in1_, HIGH);
  digitalWrite(in2_, LOW);
  if (speed > max_value_) {
    analogWrite(signal_, max_value_);
  } else {
    analogWrite(signal_, speed);
  }
}

void L298N::Reverse(uint16_t speed) {
  digitalWrite(en_, HIGH);
  digitalWrite(in1_, LOW);
  digitalWrite(in2_, HIGH);
  if (speed > max_value_) {
    analogWrite(signal_, max_value_);
  } else {
    analogWrite(signal_, speed);
  }
}

void L298N::Stop() {
  digitalWrite(en_, LOW);
  digitalWrite(in1_, LOW);
  digitalWrite(in2_, LOW);
  analogWrite(signal_, 0);
}