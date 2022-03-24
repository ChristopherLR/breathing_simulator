#pragma once

#include <stdint.h>

#define MAX_PWM 4095

typedef struct {
  float * input;
  float setpoint;
  uint16_t * output;
  double kp;
  double ki;
  double kd;
  uint16_t hit_start;

} PidState;

void InitialisePid(float * input, uint16_t * output, double kp, double ki, double kd);
void SetSetpoint(float setpoint);
void ComputePid();
void StartPid();
