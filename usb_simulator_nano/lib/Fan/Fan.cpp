#include <Arduino.h>
#include <fan.h>
#include <pid.h>
#include <sfm3000.h>
#include <elapsedMillis.h>
#include <enums.h>
#include <stdint.h>
#include <flow_definition.pb.h>
#include <message_parser.h>
#include "pb_decode.h"
#include "pb_encode.h"

ConstProfile c_profile = {0.0, 0, 0};
static float flow_setpoint[1500];
static uint8_t fin_receiving = 0;
DynamicProfile d_profile = {0, 0, 0, 20, Result::kErr};

static elapsedMillis fan_runtime;
static elapsedMillis sample_time;
static FanState f_state = FanState::kIdle;

// SFM
static Result sfm_res = Result::kOk;
static float flow = 0.0;

// Motor
static uint16_t driver_pwm = MAX_PWM;
static FanDirection fan_direction = FanDirection::kClockwise;
static uint8_t motor = 0;
static uint8_t sent_start = 0;

// PID
const double kP = 80, kI = 0.5, kD = 0.0;

SimulatorMessage runtime_flow = SimulatorMessage_init_zero;
SimulatorMessage flow_interval = SimulatorMessage_init_zero;

Result InitialiseFan() {
  runtime_flow.message_type = SimulatorMessage_MessageType_kFlow;
  flow_interval.message_type = SimulatorMessage_MessageType_kFlowInterval;
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(EN, OUTPUT);
  pinMode(PWM, OUTPUT);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(EN, LOW);
  pinMode(TRIGGER1, OUTPUT);
  pinMode(TRIGGER2, OUTPUT);
  digitalWrite(TRIGGER1, LOW);
  digitalWrite(TRIGGER2, LOW);
  InitialisePid(&flow, &driver_pwm, kP, kI, kD, &fan_direction);

  return InitialiseSfm();
}

void FanGo() {
  digitalWrite(EN, HIGH);
  if (fan_direction == FanDirection::kClockwise) {
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
  } else {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
  }
  analogWrite(PWM, driver_pwm);
}

void FanStop() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(EN, LOW);
  analogWrite(PWM, 0);
}

void SetConstFlow(InterfaceMessage *message){
  c_profile.flow            = message->message.constant_flow.flow;
  c_profile.duration        = message->message.constant_flow.duration;
  c_profile.trigger1_delay  = message->message.constant_flow.trigger1_delay;
  c_profile.trigger2_delay  = message->message.constant_flow.trigger2_delay;
  SetSetpoint(c_profile.flow);
  f_state = FanState::kSendingConstantFlow;
  driver_pwm = MAX_PWM;
  sent_start = 0;
}

void SetDynamicFlow(InterfaceMessage *message) {
  d_profile.duration  = message->message.dynamic_flow.duration;
  d_profile.count     = message->message.dynamic_flow.count;
  d_profile.trigger1_delay     = message->message.dynamic_flow.trigger1_delay;
  d_profile.trigger2_delay     = message->message.dynamic_flow.trigger2_delay;
  d_profile.interval  = message->message.dynamic_flow.interval;
  d_profile.confirmed = false;

  f_state = FanState::kReceivingDynamicFlow;
  fan_runtime = 0;
  sample_time = 0;
}


void SetManualFlow(InterfaceMessage *message) {
  motor         = message->message.manual_flow.motor_state;
  driver_pwm    = message->message.manual_flow.driver;
  ManualFlow_FanDirection mf_fan_direction = message->message.manual_flow.fan_direction;

  switch (mf_fan_direction){
    case ManualFlow_FanDirection_kCounterClockwise: {
      fan_direction = FanDirection::kCounterClockwise;
      break;
    }
    case ManualFlow_FanDirection_kClockwise: {
      fan_direction = FanDirection::kClockwise;
      break;
    }
  }

  f_state = FanState::kSendingManualFlow;
  fan_runtime = 0;
  sample_time = 0;
}


uint8_t FanLoop() {
  switch (f_state) {
    case FanState::kSendingConstantFlow: {
      SendConstFlow();
      return 0;
    }
    case FanState::kSendingManualFlow: {
      SendManualFlow();
      return 0;
    }
    case FanState::kWaitingForFlowEnd: {
      SendFlow();
      return 0;
    }
    case FanState::kReceivingDynamicFlow: {
      if (fin_receiving) {
        f_state = FanState::kConfirmingFlow;
      }
      return 0;
    }
    case FanState::kConfirmingFlow: {
      ConfirmFlow();
      return 0;
    }
    case FanState::kSendingDynamicFlow: {
      SendDynamicFlow();
      return 0;
    }
    case FanState::kIdle: {
      return 1;
    }
    default:
      return 1;
  }
  return 1;
}

void PrintFlow() {
  runtime_flow.message.flow_info.timestamp = fan_runtime;
  runtime_flow.message.flow_info.flow = flow;
  SendSimulatorMessage(&runtime_flow);
}

// TODO: implement
void PrintDynamicProfile(){

}

void SendFlow() {
  GetFlow(&sfm_res, &flow);
  if (sfm_res == Result::kOk) {
    if (sample_time > 100) {
      PrintFlow();
      sample_time = 0;
    }
  }
  if (flow < 3) {
    digitalWrite(TRIGGER1, LOW);
    digitalWrite(TRIGGER2, LOW);
    f_state = FanState::kIdle;
  }
}

static uint64_t start_time;
static uint64_t now;
static AckResponse res;


void ConfirmFlow() {
  Serial.println("confirming_flow");
  PrintDynamicProfile();

  uint8_t can_continue = 1;
  uint8_t fail_count = 0;

  for (uint32_t i = 0; i < d_profile.count; i++) {

    flow_interval.message.flow_info.timestamp = i * d_profile.interval;
    flow_interval.message.flow_info.flow = flow_setpoint[i];
    SendSimulatorMessage(&flow_interval);
    res = AckResponse::kStillProcessing;
    start_time = millis();
    now = start_time;
    while (now - start_time < 1000 && res == AckResponse::kStillProcessing) {
      now = millis();
      if (fail_count > 3) {
        f_state = FanState::kIdle;
        return;
      }

      if (Serial.available()) res = ProcessAck(Serial.read());
      if (res == AckResponse::kNak) can_continue = 0;
    }
    if (res == AckResponse::kStillProcessing) {
      fail_count += 1;
      can_continue = 0;
    }
  }
  if (can_continue) {
    d_profile.confirmed = true;
  } else {
    d_profile.confirmed = false;
  }
  f_state = FanState::kIdle;
}

const unsigned int kMaxInput = 256;
AckResponse ProcessAck(const unsigned char length) {
  static char input_line[kMaxInput];

  for (unsigned char i = 0; i < length; i++) {
    input_line[i] = Serial.read();
  }
  unsigned char end = Serial.read();
  pb_istream_t proc_istream = pb_istream_from_buffer((const pb_byte_t *)input_line, length);
  InterfaceMessage decoded = InterfaceMessage_init_zero;

  switch (decoded.message_type) {
    case InterfaceMessage_MessageType_kAck:
      return AckResponse::kAck;
    case InterfaceMessage_MessageType_kNack:
      return AckResponse::kNak;
    default:
      return AckResponse::kNak;
  }
}



void SetFin() {
  fin_receiving = 1;
  f_state = FanState::kConfirmingFlow;
}

void ConfirmFlowProfile() {
  f_state = FanState::kConfirmingFlow;
  d_profile.confirmed = false;
}

void StopMotor() {
  FanStop();
  driver_pwm = 0;
  f_state = FanState::kIdle;
}

void RunDynamicProfile() {
  // if (d_profile.confirmed) {
  sent_start = 0;
  f_state = FanState::kSendingDynamicFlow;
  // } else {
  //   Serial.println("Flow not confirmed");
  // }
}

void SendDynamicFlow() {
  if (sent_start == 0) {
    fan_runtime = 0;
    sent_start = 1;
    StartPid();
  }

  GetFlow(&sfm_res, &flow);
  if (sfm_res == Result::kOk) {
    if (sample_time > 10) {
      PrintFlow();
      sample_time = 0;
    }
  }

  if (fan_runtime >= d_profile.trigger1_delay) {
    digitalWrite(TRIGGER1, HIGH);
  }

  if (fan_runtime >= d_profile.trigger2_delay) {
    digitalWrite(TRIGGER2, HIGH);
  }

  if (fan_runtime < d_profile.duration && sfm_res == Result::kOk) {
    uint16_t idx = fan_runtime / d_profile.interval;
    SetSetpoint(flow_setpoint[idx + 1]);
    ComputePid();
    FanGo();
  }

  if (fan_runtime > d_profile.duration) {
    FanStop();
    driver_pwm = 0;
    f_state = FanState::kWaitingForFlowEnd;
  }
}

void SendManualFlow() {
  if (motor == 0) {
    FanStop();
    return;
  }
  FanGo();
  if (sample_time > 10) {
    PrintFlow();
    sample_time = 0;
  }
}



bool SetDynamicFlowInterval(InterfaceMessage *message) {
  uint32_t interval = message->message.dynamic_flow_interval.interval;
  float flow        = message->message.dynamic_flow_interval.flow;
  uint32_t final    = message->message.dynamic_flow_interval.final;

  uint32_t idx = interval / d_profile.interval;
  flow_setpoint[idx] = flow;

  if (final){
    SetFin();
  }
  return final;
}

void SendConstFlow() {
  if (sent_start == 0) {
    fan_runtime = 0;
    sent_start = 1;
    StartPid();
  }

  GetFlow(&sfm_res, &flow);
  if (sfm_res == Result::kOk) {
    if (sample_time > 10) {
      PrintFlow();
      sample_time = 0;
    }
  }

  if (fan_runtime >= c_profile.trigger1_delay) {
    digitalWrite(TRIGGER1, HIGH);
  }

  if (fan_runtime >= c_profile.trigger2_delay) {
    digitalWrite(TRIGGER2, HIGH);
  }

  if (fan_runtime < c_profile.duration && sfm_res == Result::kOk) {
    ComputePid();
    FanGo();
  }

  if (fan_runtime > c_profile.duration) {
    FanStop();
    driver_pwm = 0;
    f_state = FanState::kWaitingForFlowEnd;
    sent_start = 0;
  }
}