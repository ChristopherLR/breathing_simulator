#include <Arduino.h>
#include <ArduinoJson.h>
#include <elapsedMillis.h>
#include <stdint.h>

#include "configuration.h"
#include "enums.h"
#include "fan.h"
#include "flow_meter.h"
#include "flow_profile.h"
#include "motor_controller.h"
#include "pid.h"

#define VERSION "0.5.0"
#define BAUD 115200
#define HEARTBEAT_DELAY 500
#define DEBUG 0

#if DEBUG
#define DBG(x) Serial.println(x)
#else
#define DBG(x)
#endif

#if FM_SFM3000
SFM3000 flow_meter;
#elif FM_SFM3003
SFM3003 flow_meter;
#endif

#if FAN_VACUUM
#define FAN_NAME "vacuum"
#elif FAN_9HV0412P3K001
#define FAN_NAME "9hv"
#elif FAN_IMPELLER
#define FAN_NAME "impeller"
#endif

#define BUFFER_SIZE 128

#define HIGH 1
#define LOW 0

#ifdef ARDUINO_TEENSY41
#define TRIGGER1 A10
#define TRIGGER2 A11
#if MC_SPARK
#define SIGNAL 10
#define MAX_VALUE 1000  // 16V (1000), 12V (750), 9V (650)
Spark motor = Spark(SIGNAL, MAX_VALUE);
#if FAN_VACUUM
#define KP 4.5
#define KI 0.225
#define KD 0.8
#define REVERSIBLE 1
#elif FAN_IMPELLER
#define KP 10
#define KI 0.2
#define KD 0.0
#define REVERSIBLE 1
#elif FAN_9HV0412P3K001
#define KP 40.5
#define KI 0.05
#define KD 0.8
#define REVERSIBLE 0
#endif
#endif
#if MC_L298N
#define SIGNAL A0
#define EN 35
#define IN1 36
#define IN2 37
#define TRIGGER1 2
#define TRIGGER2 3
#define MAX_VALUE 2700  // 16V (4096), 12V (3000)

L298N motor = L298N(IN1, IN2, EN, SIGNAL, MAX_VALUE);
#if FAN_VACUUM
#define REVERSIBLE 1
#define KP 150.0
#define KI 0.15
#define KD 0.0
#endif
#endif
#endif

#ifdef ARDUINO_ARDUINO_NANO33BLE
#define TRIGGER1 A1
#define TRIGGER2 A2

#if MC_SPARK
#define SIGNAL 10
#define MAX_VALUE 1000                   // 16V (1000), 12V (750), 9V (650)
Spark motor = Spark(SIGNAL, MAX_VALUE);  // 750
#if FAN_VACUUM
#define KP 4.5
#define KI 0.225
#define KD 0.8
#define REVERSIBLE 1
#elif FAN_IMPELLER
#define KP 10
#define KI 0.2
#define KD 0.0
#define REVERSIBLE 1
#elif FAN_9HV0412P3K001
#define KP 40.5
#define KI 0.05
#define KD 0.8
#define REVERSIBLE 0
#endif
#endif

#if MC_L298N
#define SIGNAL A0
#define EN A7
#define IN1 A6
#define IN2 A3
#define MAX_VALUE 4096  // 16V (4096), 12V (3000)

L298N motor = L298N(IN1, IN2, EN, SIGNAL, MAX_VALUE);

#if FAN_VACUUM
#define REVERSIBLE 1
#define KP 150
#define KI 0.15
#define KD 0.0
#elif FAN_9HV0412P3K001
#define REVERSIBLE 0
#define KP 80.0
#define KI 0.5
#define KD 0.0
#elif FAN_IMPELLER
#define REVERSIBLE 1
#define KP 80.0
#define KI 0.5
#define KD 0.0
#endif
#endif

#endif

#ifdef ARDUINO_SAMD_NANO_33_IOT
#define TRIGGER1 9
#define TRIGGER2 10

#if MC_SPARK
#define SIGNAL A0
#define MAX_VALUE 1000                   // 16V (1000), 12V (750), 9V (650)
Spark motor = Spark(SIGNAL, MAX_VALUE);  // 750
#if FAN_VACUUM
#define KP 4.0
#define KI 0.04
#define KD 0.0
#define REVERSIBLE 1
#endif
#endif

#endif

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
DynamicJsonDocument pid_info(64);
StaticJsonDocument<128> metadata;

DynamicProfile dynamic_profile;
ConstProfile const_profile;

Pid pid = Pid(KP, KI, KD);
Fan fan(motor, flow_meter, pid, TRIGGER1, TRIGGER2, REVERSIBLE);

void setup() {
  Serial.begin(BAUD);
  while (!Serial)
    ;  // wait for serial port to connect. Needed for native USB

  fan.Initialise();

  analogWriteResolution(12);
  metadata["version"] = VERSION;
  metadata["baud"] = BAUD;
  metadata["fan"] = FAN_NAME;
  metadata["kP"] = KP;
  metadata["kI"] = KI;
  metadata["kD"] = KD;
  metadata["mc"] = motor.name;
  metadata["fm"] = flow_meter.name;
  metadata["max"] = MAX_VALUE;
}

void loop() {
  if (Serial.available()) ProcessIncomingByte(Serial.read());

  switch (state) {
    case SystemState::kWaitingForConnection:
      break;
    case SystemState::kSendingHeartbeat:
      SendHeartbeat();
      break;
    case SystemState::kSendingConstFlow:
      if (fan.RunConstProfile(const_profile))
        state = SystemState::kSendingHeartbeat;
      break;
    case SystemState::kSendingDynamicFlow:
      if (fan.RunDynamicProfile(dynamic_profile))
        state = SystemState::kSendingHeartbeat;
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

      DBG(input_line);
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
      DBG("Receiving constant flow data");
      float flow = doc["f"] | -200.0;      // Flow
      unsigned int delay = doc["dl"] | 0;  // Delay
      unsigned int length = doc["d"] | 0;  // Duration

      const_profile = ConstProfile(flow, length);
      fan.SetTrigger1Delay(delay);
      fan.SetTrigger2Delay(delay);

      if (const_profile.Invalid()) {
        Serial.print("Error setting up, flow: ");
      } else {
        fan.Prepare();
        state = SystemState::kSendingConstFlow;
      }
      const_profile.Print();

      break;
    }
    case SystemEvent::kStartDynamicFlow: {
      DBG("Receiving dynamic flow data");
      unsigned int count = doc["c"] | 0;
      unsigned int delay = doc["dl"] | 0;
      unsigned int duration = doc["d"] | 0;
      unsigned short interval = doc["dfi"] | 0;

      dynamic_profile = DynamicProfile(count, interval, duration);
      fan.SetTrigger1Delay(delay);
      fan.SetTrigger2Delay(delay);

      if (dynamic_profile.Invalid()) Serial.print("Error setting up : ");

      dynamic_profile.Print();

      break;
    }
    case SystemEvent::kConfirmDynamicFlow: {
      uint8_t err;
      err = dynamic_profile.Confirm();
      if (err) {
        Serial.print("Dynamic Flow Confirmation Error: ");
        Serial.println(err);
      } else {
        state = SystemState::kSendingDynamicFlow;
        fan.Prepare();
      }
      break;
    }
    case SystemEvent::kRunDynamicFlow: {
      fan.Prepare();
      state = SystemState::kSendingDynamicFlow;
      break;
    }
    case SystemEvent::kReceiveDynamicFlowInterval: {
      unsigned short interval = doc["dfi"] | 0;
      float flow = doc["f"] | 0.0f;
      unsigned char fin = doc["fin"] | 0;

      dynamic_profile.SetInterval(interval, flow);
      break;
    }
    case SystemEvent::kEndFlow: {
      Serial.println("Stopping system");
      fan.Stop();
      break;
    }
    case SystemEvent::kSetPid: {
      double p = doc["kP"] | KP;
      double i = doc["kI"] | KI;
      double d = doc["kD"] | KD;
      pid.SetPid(p, i, d);

      pid_info["kP"] = p;
      pid_info["kI"] = i;
      pid_info["kD"] = d;
      serializeJson(pid_info, Serial);
      Serial.print("\r\n");

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
  if (strcmp(type, "pid") == 0) return SystemEvent::kSetPid;
  if (strcmp(type, "interval") == 0)
    return SystemEvent::kReceiveDynamicFlowInterval;
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
