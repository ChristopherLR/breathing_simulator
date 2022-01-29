#include <PID.h>
#include <Arduino.h>

pid_state pid = {
  0,
  0,
  0,
  0.0f,
  0.0f,
  0.0f,
  0
};

#define SAMPLE_TIME 10
static unsigned long now;
static unsigned long last_time;
static unsigned long time_d;
static float err;
static float input_d;
static float output_sum;
static float output_lin;
static float last_input;

void initialise_pid(float * input, unsigned char * output, float kp, float ki, float kd) {
  pid.input = input;
  pid.output = output;
  pid.kp = kp;
  pid.ki = ki;
  pid.kd = kd;
  last_time = millis();
}

void set_setpoint(float setpoint) {
  pid.setpoint = setpoint;
}

void start_pid() {
  pid.hit_start = 0;
}


// Look into feed forward
// Kalman filters
void compute_pid() {
  now = millis();
  time_d = now - last_time;
  if (time_d < SAMPLE_TIME) return;
  err = pid.setpoint - *(pid.input);
  if (err < 0) pid.hit_start = 1;


  input_d = *(pid.input) - last_input;
  
  output_lin = pid.kp * err;
  output_sum += pid.ki * err;
  output_lin += output_sum  - pid.kd * input_d;

  if (output_lin > 255) {
    *(pid.output) = 255;
  } else if (output_lin < 0) {
    *(pid.output) = 0;
  } else {
    *(pid.output) = output_lin;
  }

  last_input = *(pid.input);
  last_time = now;
}
