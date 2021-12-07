#include <Arduino.h>
#include <Wire.h>
#include <stdint.h>
#include <ArduinoJson.h>
#include <elapsedMillis.h>

#define VERSION "0.1.0"
#define BAUD 115200
#define HEARTBEAT_DELAY 500

#define OFFSET 32000
#define SCALE 140.0
#define FM_ADDRESS 0x40
#define BUFFER_SIZE 64

#define DRIVER_POS 37
#define DRIVER_NEG 36
#define DRIVER_PWM 15

char buf[BUFFER_SIZE];

enum State {
  waiting_for_connection,
  sending_heartbeat,
  sending_const_flow,
  sending_dynamic_flow, 
};

enum Request {
  invalid,
  none,
  attempt_connect,
  start_constant_flow,
  start_dynamic_flow,
  end_flow,
};

enum Result {
  success,
  error
};

typedef struct const_profile {
  float flow;
  unsigned int delay;
  unsigned int length;
} const_profile;

unsigned char motor_pwm = 0;

elapsedMillis runtime;

Result connect();
void send_heartbeat();
void send_const_flow();
Result setup_const_flow();
Request process_input();

Request req = Request::none;
State state = State::waiting_for_connection;

DynamicJsonDocument doc(1024);
StaticJsonDocument<256> metadata;

const_profile c_profile = { 0.0, 0, 0 };

void setup() {

  Wire.begin();
  Serial.begin(BAUD);

  pinMode(DRIVER_POS, OUTPUT);
  pinMode(DRIVER_NEG, OUTPUT);
  pinMode(DRIVER_PWM, OUTPUT);

  digitalWrite(DRIVER_POS, LOW);
  digitalWrite(DRIVER_NEG, LOW);
  analogWrite(DRIVER_PWM, LOW);

  while (!Serial); // wait for serial port to connect. Needed for native USB

  metadata["version"] = VERSION;
  metadata["baud"] = BAUD;

  Wire.beginTransmission(FM_ADDRESS);
  Wire.write(0x10);
  Wire.write(0x00);
  Wire.endTransmission();
}

void loop() {
  if (2 == Wire.requestFrom(FM_ADDRESS, 2)) {
    a = Wire.read();
    b = Wire.read();
    a = (a << 8) | b;
    flow = ((float)a - OFFSET)/SCALE;
    Serial.println(flow);
  }
}

void serialEvent(){
  Request req = process_input();

  switch (req) {
    case none:
      break;
    case invalid: {
      Serial.println("Invalid request");
      break;
    }
    case attempt_connect: {
      Result res = connect();
      if (res == Result::success) state = State::sending_heartbeat;
      break;
    }
    case start_constant_flow: {
      Result res = setup_const_flow();
      if (res == Result::success) state = State::sending_const_flow;
      break;
    }
    case start_dynamic_flow: {
      Serial.println("Unimplemented");
      break;
    }
    case end_flow: {
      Serial.println("Unimplemented");
      break;
    }
    default:
      Serial.println("Unknown request");
  }


}

Request process_input() {
  if (Serial.available() > 0){
    Serial.readBytes(buf, BUFFER_SIZE);
    DeserializationError err = deserializeJson(doc, buf);

    const char * type = doc["type"] | "NA";
    Serial.println(type);

    if (strcmp(type, "NA") == 0) return Request::invalid;

    if (strcmp(type, "connect") == 0){
      return Request::attempt_connect;
    } else if (strcmp(type, "const") == 0) {
      return Request::start_constant_flow;
    } else if (strcmp(type, "dynamic") == 0) {
      return Request::start_dynamic_flow;
    } else {
      return Request::invalid;
    }
  }

  return Request::none;
}

void send_heartbeat() {
  if (runtime > 1000){
    Serial.println("alive");
    runtime = 0;
  }
}

volatile uint16_t a;
volatile uint8_t  b;
volatile float flow;

void send_const_flow(){
  if (runtime < c_profile.length){
    if (2 == Wire.requestFrom(FM_ADDRESS, 2)) {
      a = Wire.read();
      b = Wire.read();
      a = (a << 8) | b;
      flow = ((float)a - OFFSET)/SCALE;
      Serial.println(flow);
    }

    if (flow < c_profile.flow && motor_pwm < 255){
      motor_pwm += 1;
    } else if (flow > c_profile.flow && motor_pwm > 0){
      motor_pwm -= 1;
    }

    digitalWrite(DRIVER_POS, HIGH);
    digitalWrite(DRIVER_NEG, LOW);
    analogWrite(DRIVER_PWM, motor_pwm);

    Serial.println(motor_pwm);
  }
  if (runtime > c_profile.length){
    digitalWrite(DRIVER_POS, LOW);
    digitalWrite(DRIVER_NEG, LOW);
    analogWrite(DRIVER_PWM, 0); 
    runtime = 0;
    state = sending_heartbeat;
  }
}

Result setup_const_flow(){
  float flow = doc["flow"]   | 0.0;
  int delay  = doc["delay"]  | 0;
  int length = doc["length"] | 0;

  c_profile.flow = flow;
  c_profile.delay = delay;
  c_profile.length = length;
  runtime = 0;

  Serial.println(flow);
  if (flow > 0.0 && length > 0) return Result::success;
  return Result::error;
}

Result connect() {
  size_t len = serializeJson(metadata, Serial);
  Serial.print("\r\n");
  if (len > 0) return Result::success;
  return Result::error;
}