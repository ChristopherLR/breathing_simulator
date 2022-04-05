#include <Arduino.h>
#include <elapsedMillis.h>
#include <enums.h>
#include <fan.h>
#include <sfm3000.h>
#include <stdint.h>

#include <flow_definition.pb.h>
#include <message_parser.h>
#include "pb_decode.h"
#include "pb_encode.h"

#define MAJOR_VERSION 0
#define MINOR_VERSION 4
#define PATCH_VERSION 0
#define BAUD 115200
#define HEARTBEAT_DELAY 500

#define BUFFER_SIZE 128

#define HIGH 1
#define LOW 0

elapsedMillis runtime;

void ProcessIncomingProto(const unsigned char);
void ProcessInput(const char *data, const unsigned int length);
void SendHeartbeat();
void PrintRequest(Result res);
void TransistionState();

SystemEvent event = SystemEvent::kNone;
SystemState state = SystemState::kWaitingForConnection;

SimulatorMessage simulator_version = SimulatorMessage_init_zero;
simulator_version.message_type = SimulatorMessage_MessageType_kVersionInfo;
simulator_version.which_message = SimulatorMessage_version_info_tag; 
simulator_version.message.version_info.major = MAJOR_VERSION;
simulator_version.message.version_info.minor = MINOR_VERSION;
simulator_version.message.version_info.patch = PATCH_VERSION;



void setup() {
  Serial.begin(BAUD);
  while (!Serial); // Wait for serial connection
  InitialiseFan();
  analogWriteResolution(12);
}

void loop() {
  if (Serial.available()) ProcessIncomingProto(Serial.read());

  switch (state) {
    case SystemState::kWaitingForConnection:
      break;
    case SystemState::kSendingHeartbeat:
      SendHeartbeat();
      break;
    case SystemState::kSendingFlow:
      if (FanLoop()) state = SystemState::kSendingHeartbeat;
      break;
    default:
      Serial.println("Unknown state");
  }
}

const unsigned int kMaxInput = 256;
void ProcessIncomingProto(const unsigned char length) {
  static char input_line[kMaxInput];

  for (unsigned char i = 0; i < length; i++) {
    input_line[i] = Serial.read();
  }
  unsigned char end = Serial.read();
  ProcessInput(input_line, length);
}


// TODO: Implement
bool ValidVersion(InterfaceMessage * message) {
  uint32_t major = message->message.version_info.major;
  uint32_t minor = message->message.version_info.minor;
  uint32_t patch = message->message.version_info.patch;
  return true;
}

bool ValidConstantFlow(InterfaceMessage * message) {
  float flow = message->message.constant_flow.flow;
  uint32_t duration = flow_message->message.constant_flow.duration;

  if (flow > 220.0f || flow <= 0.0f || duration <= 0){
    return false;
  }
  return true;
}

bool ValidDynamicFlow(InterfaceMessage * message) {
  uint32_t duration = message->message.dynamic_flow.duration;
  uint32_t interval = message->message.dynamic_flow.interval;
  uint32_t count    = message->message.dynamic_flow.count;
  if (count <= 0 || duration <= 0 || interval <= 0) {
    return false;
  }
  return true;
}

// TODO: Implement
bool ValidDynamicFlowInterval(InterfaceMessage * message) {
  uint32_t interval = message->message.dynamic_flow_interval.interval;
  float flow        = message->message.dynamic_flow_interval.flow;
  uint32_t final    = message->message.dynamic_flow_interval.final;
  return true;
}

void ProcessInput(const char *data, const unsigned int length) {
  istream = pb_istream_from_buffer((const pb_byte_t *)data, length);
  InterfaceMessage decoded = InterfaceMessage_init_zero;

  bool status = pb_decode(&istream, InterfaceMessage_fields, &decoded);
  if (!status) {
    Serial.write("Failed to decode\n");
  }

  switch (decoded.message_type) {
    case InterfaceMessage_MessageType_kVersionInfo:
      bool compatible = ValidVersion(&decoded);
      if (!compatible) {
        SendSimulatorMessage(&version_error);
      } else {
        SendSimulatorMessage(&simulator_version);
        state = SystemState::kSendingHeartbeat;
      }
      break;
    case InterfaceMessage_MessageType_kConstantFlow:
      bool valid = ValidConstantFlow(&decoded);
      if (!valid) {
        SendSimulatorMessage(&invalid_flow);
      } else {
        SetConstFlow(&decoded);
        state = SystemState::kSendingFlow;
      }
      break;
    case InterfaceMessage_MessageType_kManualFlow:
      bool valid = ValidManualFlow(&decoded);
      if (!valid) {
        SendSimulatorMessage(&invalid_flow);
      } else {
        SetManualFlow(&decoded);
        state = SystemState::kSendingFlow;
      }
      break;
    case InterfaceMessage_MessageType_kDynamicFlow:
      bool valid = ValidDynamicFlow(&decoded);
      if (!valid) {
        SendSimulatorMessage(&invalid_flow);
      } else {
        SetDynamicFlow(&decoded);
      }
      break;
    case InterfaceMessage_MessageType_kDynamicFlowInterval:
      bool valid = ValidDynamicFlowInterval(&decoded);
      if (!valid) {
        SendSimulatorMessage(&invalid_flow_interval);
      } else {
        bool fin = SetDynamicFlowInterval(&decoded);
        if (fin){
          RunDynamicProfile();
          state = SystemState::kSendingFlow;
        }
      }
      break;
    case InterfaceMessage_MessageType_kInformationRequest:
      switch(decoded.message.information_request.data_type) {
        case InformationRequest_DataType_kDynamicFlow:
          ConfirmFlowProfile();
          break;
        default:
          SendSimulatorMessage(&invalid_event);
          break;
      }
      break;
    case InterfaceMessage_MessageType_kRunDynamicFlowRequest:
      RunDynamicProfile();
      state = SystemState::kSendingFlow;
      break;
    case InterfaceMessage_MessageType_kAck:
      break;
    case InterfaceMessage_MessageType_kNack:
      break;
    default:
      SendSimulatorMessage(&invalid_event);
      break;
  }
}

void SendHeartbeat() {
  if (runtime > 1000) {
    SimulatorMessage heartbeat = SimulatorMessage_init_zero;
    heartbeat.message_type = InterfaceMessage_MessageType_kHeartbeat;
    SendSimulatorMessage(&heartbeat);
    runtime = 0;
  }
}