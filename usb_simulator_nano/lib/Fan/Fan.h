#pragma once

#include <Arduino.h>
#include <enums.h>
#include <ArduinoJson.h>
#include <stdint.h>

#define IN1 36
#define IN2 37
#define ENA A0

#define FAN_PWM 15

#define TRIGGER1 2
#define TRIGGER2 3

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
