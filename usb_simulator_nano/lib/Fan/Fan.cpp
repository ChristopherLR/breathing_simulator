#include <Arduino.h>
#include <ArduinoJson.h>
#include <fan.h>
#include <pid.h>
#include <sfm3000.h>
#include <elapsedMillis.h>
#include <enums.h>
#include <stdint.h>

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

Result InitialiseFan() {
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

void SetConstFlow(float flow, uint32_t dur, uint16_t delay) {
  c_profile.flow = flow;
  c_profile.duration = dur;
  c_profile.delay = delay;
  SetSetpoint(flow);
  f_state = FanState::kSendingConstantFlow;
  driver_pwm = MAX_PWM;
  sent_start = 0;
}

void SetDynamicFlow(unsigned int count, unsigned int delay,
                    unsigned int duration, unsigned short interval) {
  d_profile.duration = duration;
  d_profile.count = count;
  d_profile.delay = delay;
  d_profile.interval = interval;
  d_profile.confirmed = false;

  Serial.println("Receiving dynamic flow");
  PrintDynamicProfile();
  f_state = FanState::kReceivingDynamicFlow;
  fan_runtime = 0;
  sample_time = 0;
}

void PrintDynamicProfile() {
  Serial.print("Duration: ");
  Serial.print(d_profile.duration);
  Serial.print(", Count: ");
  Serial.print(d_profile.count);
  Serial.print(", Delay: ");
  Serial.print(d_profile.delay);
  Serial.print(", Interval: ");
  Serial.print(d_profile.interval);
  Serial.print(", Confirmed: ");
  Serial.println(d_profile.confirmed);
}

void SetManualFlow(unsigned char motor_state, unsigned char driver_val) {
  motor = motor_state;
  driver_pwm = driver_val;
  f_state = FanState::kSendingManualFlow;
  fan_runtime = 0;
  sample_time = 0;
}

void PrintProfile() {
  Serial.println(String("Flow: ") + String(c_profile.flow) +
                 String(", Duration: ") + String(c_profile.duration) +
                 String(", Delay: ") + String(c_profile.delay));
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
    Serial.print("i");
    Serial.print(i * d_profile.interval);
    Serial.print(", f");
    Serial.println(flow_setpoint[i]);
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
    Serial.println("done");
  } else {
    d_profile.confirmed = false;
    Serial.println("profile_not_confirmed");
  }
  f_state = FanState::kIdle;
}

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
    Serial.println("start_dynamic_flow");
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

  if (fan_runtime >= d_profile.delay) {
    digitalWrite(TRIGGER1, HIGH);
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

void PrintFlow() {
  Serial.print("f:");
  Serial.print(fan_runtime);
  Serial.print(',');
  Serial.print(flow);
  Serial.print(',');
  Serial.println(driver_pwm);
}

void SetInterval(unsigned short interval, float flow) {
  uint16_t idx = interval / d_profile.interval;
  flow_setpoint[idx] = flow;
  // Serial.print(interval); Serial.print(", ");
  // Serial.print(d_profile.interval); Serial.print(", ");
  // Serial.print(idx); Serial.print(", ");
  // Serial.println(flow_setpoint[idx]);
}

void SendConstFlow() {
  if (sent_start == 0) {
    Serial.println("start_flow");
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

  if (fan_runtime >= c_profile.delay) {
    digitalWrite(TRIGGER1, HIGH);
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