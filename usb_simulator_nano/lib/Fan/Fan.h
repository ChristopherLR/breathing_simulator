#pragma once

#include <Arduino.h>
#include <ArduinoJson.h>
#include <enums.h>
#include <stdint.h>

#ifdef ARDUINO_TEENSY41
#define PWM A0
#define EN 35
#define IN1 36
#define IN2 37
#define TRIGGER1 2
#define TRIGGER2 3
#endif

#ifdef ARDUINO_ARDUINO_NANO33BLE
#define PWM A0
#define EN A7
#define IN1 A6
#define IN2 A3
#define TRIGGER1 A1
#define TRIGGER2 A2
#endif

#ifdef ARDUINO_SAMD_NANO_33_IOT
#endif

typedef struct {
  float flow;
  uint32_t duration;
  uint16_t delay;
} ConstProfile;

typedef struct {
  uint32_t duration;
  uint32_t count;
  uint16_t delay;
  uint16_t interval;
  bool confirmed;
} DynamicProfile;

Result InitialiseFan();
void SetConstFlow(float flow, uint32_t dur, uint16_t delay);
void SendConstFlow();
uint8_t FanLoop();
void SendFlow();
void SendDynamicFlow();
void PrintProfile();
void FanGo();
void FanStop();
void PrintFlow();
void StopMotor();
void SetManualFlow(unsigned char motor_state, unsigned char driver);
void SetDynamicFlow(unsigned int count, unsigned int delay,
                      unsigned int duration, unsigned short interval);
void SendManualFlow();
void SetInterval(unsigned short interval, float flow);
void SetFin();
AckResponse ProcessAck(const uint8_t in);
void ConfirmFlow();
void ConfirmFlowProfile();
void PrintDynamicProfile();
void RunDynamicProfile();
