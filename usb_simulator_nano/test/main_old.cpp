#include <Arduino.h>
#include <ArduinoJson.h>
#include <elapsedMillis.h>
#include <stdint.h>

#include "enums.h"
#include "fan.h"
#include "sfm3000.h"

#define VERSION "0.3.1"
#define BAUD 115200
#define HEARTBEAT_DELAY 500

#define BUFFER_SIZE 128

#define HIGH 1
#define LOW 0

elapsedMillis runtime;

void ProcessIncomingByte(const byte);
SystemEvent ProcessInput(const char *);
Result connect();
void SendHeartbeat();
void PrintRequest(Result res);
void TransistionState();

SystemEvent event = SystemEvent::kNone;
SystemState state = SystemState::kWaitingForConnection;

DynamicJsonDocument doc(1024);
StaticJsonDocument<128> metadata;

void setup() {
  Serial.begin(BAUD);
  while (!Serial)
    ;  // wait for serial port to connect. Needed for native USB

  InitialiseFan();

  analogWriteResolution(12);
  metadata["version"] = VERSION;
  metadata["baud"] = BAUD;
}

void loop() {
  if (Serial.available()) ProcessIncomingByte(Serial.read());

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

void ProcessIncomingByte(const uint8_t in) {
  static char input_line[BUFFER_SIZE];
  static unsigned int input_pos = 0;

  switch (in) {
    case '\n': {
      input_line[input_pos] = 0;  // terminating null byte
      input_pos = 0;

      Serial.println(input_line);
      event = ProcessInput(input_line);
      TransistionState();

      // reset buffer for next time
      break;
    }
    case '\r': {
      break;
    }
    default: {
      // keep adding if not full ... allow for terminating null byte
      if (input_pos < (BUFFER_SIZE - 1)) input_line[input_pos++] = in;
      break;
    }
  }
}

void TransistionState() {
  static Result res = Result::kOk;

  switch (event) {
    case SystemEvent::kNone:
      break;
    case SystemEvent::kInvalid: {
      Serial.println("Invalid event");
      break;
    }
    case SystemEvent::kAttemptConnect: {
      res = connect();
      if (res == Result::kOk) state = SystemState::kSendingHeartbeat;
      if (res == Result::kErr) Serial.println("Error connecting");
      break;
    }
    case SystemEvent::kStartConstantFlow: {
      Serial.println("Setting up constant flow");
      float flow = doc["f"] | 0.0;         // Flow
      unsigned int delay = doc["dl"] | 0;  // Delay
      unsigned int length = doc["d"] | 0;  // Duration

      if (flow <= 0.0 || length <= 0) {
        res = Result::kErr;
        Serial.print("Error setting up, flow: ");
        Serial.print(flow);
        Serial.print(", delay: ");
        Serial.println(length);
      }

      if (res == Result::kOk) {
        SetConstFlow(flow, length, delay);
        PrintProfile();
        state = SystemState::kSendingFlow;
      }

      break;
    }
    case SystemEvent::kStartManualFlow: {
      Serial.println("Starting manual flow");
      unsigned char motor_state = doc["ms"] | 0;  // Motor State
      unsigned char driver = doc["dv"] | 0;       // Driver

      SetManualFlow(motor_state, driver);
      state = SystemState::kSendingFlow;
      break;
    }
    case SystemEvent::kStartDynamicFlow: {
      Serial.println("Receiving dynamic flow data");
      unsigned int count = doc["c"] | 0;
      unsigned int delay = doc["dl"] | 0;
      unsigned int duration = doc["d"] | 0;
      unsigned short interval = doc["dfi"] | 0;

      if (count <= 0 || duration <= 0 || interval < 5) {
        res = Result::kErr;
        Serial.print("Error setting up, count: ");
        Serial.print(count);
        Serial.print(", delay: ");
        Serial.print(delay);
        Serial.print(", interval: ");
        Serial.print(interval);
        Serial.print(", duration: ");
        Serial.println(duration);
      }

      if (res == Result::kOk) {
        SetDynamicFlow(count, delay, duration, interval);
        state = SystemState::kSendingFlow;
      }
      break;
    }
    case SystemEvent::kConfirmDynamicFlow: {
      ConfirmFlowProfile();
      state = SystemState::kSendingFlow;
      break;
    }
    case SystemEvent::kRunDynamicFlow: {
      RunDynamicProfile();
      state = SystemState::kSendingFlow;
      break;
    }
    case SystemEvent::kReceiveDynamicFlowInterval: {
      unsigned short interval = doc["dfi"] | 0;
      float flow = doc["f"] | 0.0f;
      unsigned char fin = doc["fin"] | 0;

      if (fin != 0) {
        SetFin();
        state = SystemState::kSendingFlow;
        Serial.println("received_fin");
      }
      SetInterval(interval, flow);
      break;
    }
    case SystemEvent::kEndFlow: {
      Serial.println("Stopping system");

      StopMotor();
      break;
    }
    default:
      Serial.println("Unknown request");
  }
}

SystemEvent ProcessInput(const char *data) {
  if (strcmp(data, "ack") == 0) return SystemEvent::kNone;
  if (strcmp(data, "nak") == 0) return SystemEvent::kNone;

  DeserializationError err = deserializeJson(doc, data);

  if (err != DeserializationError::Ok)
    Serial.println("Error deserialising input");

  const char *type = doc["t"] | "NA";
  // Serial.println(type);

  if (strcmp(type, "NA") == 0) return SystemEvent::kInvalid;
  if (strcmp(type, "connect") == 0) return SystemEvent::kAttemptConnect;
  if (strcmp(type, "const") == 0) return SystemEvent::kStartConstantFlow;
  if (strcmp(type, "dynamic") == 0) return SystemEvent::kStartDynamicFlow;
  if (strcmp(type, "confirm") == 0) return SystemEvent::kConfirmDynamicFlow;
  if (strcmp(type, "interval") == 0)
    return SystemEvent::kReceiveDynamicFlowInterval;
  if (strcmp(type, "manual") == 0) return SystemEvent::kStartManualFlow;
  if (strcmp(type, "run") == 0) return SystemEvent::kRunDynamicFlow;

  return SystemEvent::kInvalid;
}

void SendHeartbeat() {
  if (runtime > 1000) {
    Serial.println("alive");
    runtime = 0;
  }
}

Result connect() {
  size_t len = serializeJson(metadata, Serial);
  Serial.print("\r\n");
  if (len > 0) return Result::kOk;
  return Result::kErr;
}
