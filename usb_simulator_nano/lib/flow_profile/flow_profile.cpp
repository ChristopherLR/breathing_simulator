#include "flow_profile.h"

#include <Arduino.h>

uint32_t ConstProfile::GetDuration() { return duration_; }
uint32_t DynamicProfile::GetDuration() { return duration_; }

uint8_t ConstProfile::GetConfirmed() { return confirmed_; }
uint8_t DynamicProfile::GetConfirmed() { return confirmed_; }

void ConstProfile::Print() {
  Serial.print("Duration: ");
  Serial.print(duration_);
  Serial.print(", Flow: ");
  Serial.print(flow_);
  Serial.print(", Confirmed: ");
  Serial.println(confirmed_);
}

uint8_t ConstProfile::Invalid() {
  if (duration_ <= 100) return 1;
  if (flow_ >= 200) return 2;
  if (flow_ <= -150) return 3;
  return 0;
}

float ConstProfile::GetFlow() { return flow_; }

void DynamicProfile::Print() {
  Serial.print("Duration: ");
  Serial.print(duration_);
  Serial.print(", Count: ");
  Serial.print(count_);
  Serial.print(", Interval: ");
  Serial.print(interval_);
  Serial.print(", Confirmed: ");
  Serial.println(confirmed_);
}

uint8_t DynamicProfile::Invalid() {
  if (duration_ <= 100) return 1;
  if (interval_ <= 0) return 2;
  if (count_ <= 0) return 3;
  return 0;
}

typedef enum {
  kStillProcessing,
  kAck,
  kNak,
} AckResponse;

#define MAX_LEN 10
AckResponse ProcessAck(const uint8_t in) {
  static char input_line[MAX_LEN];
  static unsigned int input_pos = 0;

  switch (in) {
    case '\n': {
      input_line[input_pos] = 0;  // terminating null byte
      input_pos = 0;

      if (strncmp(input_line, "ack", 3) == 0) return AckResponse::kAck;
      return AckResponse::kNak;
    }
    case '\r': {
      return AckResponse::kStillProcessing;
    }
    default: {
      // keep adding if not full ... allow for terminating null byte
      if (input_pos < (MAX_LEN - 1)) input_line[input_pos++] = in;
      return AckResponse::kStillProcessing;
    }
  }
}

uint8_t DynamicProfile::Confirm() {
  Print();

  uint8_t can_continue = 1;
  uint8_t fail_count = 0;
  uint64_t start_time;
  uint64_t now;
  AckResponse res;

  for (uint32_t i = 0; i < count_; i++) {
    Serial.print("i");
    Serial.print(i * interval_);
    Serial.print(", f");
    Serial.println(flow_setpoint_[i]);

    res = AckResponse::kStillProcessing;
    start_time = millis();
    now = start_time;
    while (now - start_time < 1000 && res == AckResponse::kStillProcessing) {
      now = millis();
      if (fail_count > 3) return 1;
      if (Serial.available()) res = ProcessAck(Serial.read());
      if (res == AckResponse::kNak) can_continue = 0;
    }
    if (res == AckResponse::kStillProcessing) {
      fail_count += 1;
      can_continue = 0;
    }
  }
  if (can_continue) {
    confirmed_ = true;
    return 0;
  } else {
    confirmed_ = false;
    return 2;
  }
}

void DynamicProfile::SetInterval(uint16_t interval, float flow) {
  uint16_t idx = interval / interval_;
  if (idx > setpoint_size_) return;
  flow_setpoint_[idx] = flow;
}

float DynamicProfile::GetFlow(uint16_t runtime) {
  uint16_t idx = runtime / interval_;
  if (idx > setpoint_size_) return 0;
  return flow_setpoint_[idx + 1];
}