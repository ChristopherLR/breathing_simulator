#include <pid.h>
#include <Arduino.h>
#include <stdint.h>


PidState pid = {
  0,
  0.0f,
  0,
  0.0f,
  0.0f,
  0.0f,
  0
};

#define SAMPLE_TIME 5
static uint64_t now;
static uint64_t last_time;
static uint64_t time_d;
static float err;
static double input_d;
static double output_sum;
static double output_lin;
static double last_input;

void InitialisePid(float * input, uint16_t * output, double kp, double ki, double kd) {
  pid.input = input;
  pid.output = output;
  pid.kp = kp;
  pid.ki = ki;
  pid.kd = kd;
  last_time = millis();
}

void SetSetpoint(float setpoint) {
  pid.setpoint = setpoint;
}

void StartPid() {
  pid.hit_start = 0;
}


// Look into feed forward
// Kalman filters
void ComputePid() {
  now = millis();
  time_d = now - last_time;
  if (time_d < SAMPLE_TIME) return;
  err = pid.setpoint - *(pid.input);
  if (err < 0) {
    pid.hit_start = 1;
  } 
  // else if (pid.hit_start == 0) {
  //   return;
  // }


  input_d = *(pid.input) - last_input;
  
  output_lin = pid.kp * err;
  output_sum += pid.ki * err;
  output_lin += output_sum  - pid.kd * input_d;

  if (output_lin > MAX_PWM) {
    *(pid.output) = MAX_PWM;
  } else if (output_lin < 0) {
    *(pid.output) = 0;
  } else {
    *(pid.output) = output_lin;
  }

  last_input = *(pid.input);
  last_time = now;
}
