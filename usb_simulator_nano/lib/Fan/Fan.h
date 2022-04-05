#pragma once

#include <Arduino.h>
#include <ArduinoJson.h>
#include <enums.h>
#include <stdint.h>
#include <flow_definition.pb.h>

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
  uint32_t trigger1_delay;
  uint32_t trigger2_delay;
} ConstProfile;

typedef struct {
  uint32_t duration;
  uint32_t count;
  uint32_t interval;
  uint32_t trigger1_delay;
  uint32_t trigger2_delay;
  bool confirmed;
} DynamicProfile;

Result InitialiseFan();
uint8_t FanLoop();
void FanGo();
void FanStop();
void PrintFlow();
void StopMotor();

void SetFin();
AckResponse ProcessAck(const unsigned char length);
void ConfirmFlow();
void ConfirmFlowProfile();

void PrintProfile();
void PrintDynamicProfile();

void SetConstFlow(InterfaceMessage*);
void SetManualFlow(InterfaceMessage*);
void SetDynamicFlow(InterfaceMessage*);
bool SetDynamicFlowInterval(InterfaceMessage*);

void SendFlow();
void SendManualFlow();
void SendDynamicFlow();
void SendConstFlow();
void RunDynamicProfile();

