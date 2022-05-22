#pragma once

#include <Arduino.h>
#include <Servo.h>
#include <stdint.h>

class MotorController {
 public:
  virtual void Forward(uint16_t speed) = 0;
  virtual void Reverse(uint16_t speed) = 0;
  virtual void Stop() = 0;
  virtual uint8_t Initialise() = 0;
};

class Spark : public MotorController {
 public:
  Spark(uint8_t signal_pin) : signal_(signal_pin){};

  void Forward(uint16_t speed);
  void Reverse(uint16_t speed);
  void Stop();
  uint8_t Initialise();

 private:
  const uint16_t max_value_ = 1000;
  const uint16_t forward_max_ = 2500;
  const uint16_t reverse_min_ = 500;
  const uint16_t midpoint_ = 1500;
  uint8_t signal_;
  Servo motor_;
};

class L298N : public MotorController {
 public:
  L298N(uint8_t in1, uint8_t in2, uint8_t en, uint8_t signal)
      : in1_(in1), in2_(in2), en_(en), signal_(signal) {}

  void Forward(uint16_t speed);
  void Reverse(uint16_t speed);
  uint8_t Initialise();
  void Stop();

 private:
  uint8_t in1_;
  uint8_t in2_;
  uint8_t en_;
  uint8_t signal_;
};