#pragma once

#include <Arduino.h>
#include <enums.h>
#include <ArduinoJson.h>
#include <stdint.h>

#define DRIVER_POS D2
#define DRIVER_NEG D3
#define DRIVER_PWM 16u

#define FAN_PWM A3

#define TRIGGER1 D9
#define TRIGGER2 D8

typedef struct const_profile {
  float flow;
  unsigned int duration;
  unsigned int delay;
} const_profile;

result initialise_fan();
void set_const_flow(float flow, unsigned int duration, unsigned int delay);
uint8_t fan_loop();
void send_flow();
void send_const_flow();
void send_dynamic_flow();
void print_profile();
void fan_go();
void fan_stop();
void print_flow();
void stop_motor();
void set_manual_flow(unsigned char motor_state, unsigned char driver, unsigned char motor);
void send_manual_flow();
