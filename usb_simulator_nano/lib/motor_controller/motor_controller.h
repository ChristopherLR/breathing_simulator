#pragma once

#include <Arduino.h>

#if MC_SPARK
#include <Servo.h>
#endif

#include <stdint.h>

class MotorController {
 public:
  virtual void Forward(uint16_t speed) = 0;
  virtual void Reverse(uint16_t speed) = 0;
  virtual void Stop() = 0;
  virtual uint8_t Initialise() = 0;
};

#define SPARK_MIDPOINT 1500

#if MC_SPARK
class Spark : public MotorController {
 public:
  Spark(uint8_t signal_pin, uint16_t max)
      : signal_(signal_pin),
        max_value_(max),
        forward_max_(SPARK_MIDPOINT + max),
        reverse_min_(SPARK_MIDPOINT - max){};

  void Forward(uint16_t speed);
  void Reverse(uint16_t speed);
  void Stop();
  uint8_t Initialise();

  const char* name = "Spark";

 private:
  uint8_t signal_;
  uint16_t max_value_;
  uint16_t forward_max_;
  uint16_t reverse_min_;
  uint16_t midpoint_ = SPARK_MIDPOINT;
  Servo motor_;
};

#endif

class L298N : public MotorController {
 public:
  L298N(uint8_t in1, uint8_t in2, uint8_t en, uint8_t signal, uint16_t max)
      : in1_(in1), in2_(in2), en_(en), signal_(signal), max_value_(max) {}

  void Forward(uint16_t speed);
  void Reverse(uint16_t speed);
  uint8_t Initialise();
  void Stop();

  const char* name = "L298N";

 private:
  uint8_t in1_;
  uint8_t in2_;
  uint8_t en_;
  uint8_t signal_;
  uint16_t max_value_;
};