#pragma once

#include <stdint.h>

typedef struct {
  float * input;
  unsigned char * output;
  float setpoint;
  float kp;
  float ki;
  float kd;
  unsigned char hit_start;
} pid_state;

void initialise_pid(float * input, unsigned char * output, float kp, float ki, float kd);
void set_setpoint(float setpoint);
void compute_pid();
void start_pid();
