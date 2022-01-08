#include <Fan.h>
#include <Arduino.h>
#include <enums.h>
#include <ArduinoJson.h>
#include <elapsedMillis.h>
#include <SFM3000.h>
#include <stdint.h>

const_profile c_profile = { 0.0, 0, 0 };
static elapsedMillis fan_runtime;
static fan_state f_state = fan_state::idle;

// SFM
static result sfm_res = result::ok;
static float  flow = 0.0;

// Motor
static uint8_t motor_pwm = 0;
static uint8_t driver_pwm = 255;
static uint8_t motor = 0;
static uint8_t sent_start = 0;

result initialise_fan(){
  pinMode(DRIVER_POS, OUTPUT);
  pinMode(DRIVER_NEG, OUTPUT);
  pinMode(DRIVER_PWM, OUTPUT);
  pinMode(FAN_PWM, OUTPUT);
  digitalWrite(DRIVER_POS, LOW);
  digitalWrite(DRIVER_NEG, LOW);
  analogWrite(FAN_PWM, LOW);
  analogWrite(DRIVER_PWM, LOW);
  pinMode(TRIGGER1, OUTPUT);
  pinMode(TRIGGER2, OUTPUT);
  digitalWrite(TRIGGER1, LOW);
  digitalWrite(TRIGGER2, LOW);

  return initialise_sfm();
}

void fan_go(){
  digitalWrite(DRIVER_POS, HIGH);
  digitalWrite(DRIVER_NEG, LOW);
  analogWrite(DRIVER_PWM, driver_pwm);
  analogWrite(FAN_PWM, motor_pwm);
}

void fan_stop(){
  digitalWrite(DRIVER_POS, LOW);
  digitalWrite(DRIVER_NEG, LOW);
  analogWrite(DRIVER_PWM, 0);
  analogWrite(FAN_PWM, 0);
}

void set_const_flow(float flow, unsigned int duration, unsigned int delay){
  c_profile.flow = flow;
  c_profile.duration = duration;
  c_profile.delay = delay;
  f_state = fan_state::sending_constant_flow;
  motor_pwm = 255;
  driver_pwm = 255;
  sent_start = 0;
}

void print_profile(){
  Serial.println(String("Flow: ") + String(c_profile.flow)+ String(", Duration: ") + String(c_profile.duration) + String(", Delay: ") + String(c_profile.delay));
}

uint8_t fan_loop(){
  switch (f_state){
    case fan_state::sending_constant_flow: {
      send_const_flow();
      return 0;
    }
    case fan_state::sending_manual_flow: {
      send_manual_flow();
      return 0;
    }
    case fan_state::waiting_for_flow_end: {
      send_flow();
      return 0;
    }
    case fan_state::sending_dynamic_flow: {
      return 1;
    }
    case fan_state::idle: {
      return 1;
    }
    default: return 1;
  }
  return 1;
}

void send_flow(){
  get_flow(&sfm_res, &flow);
  if (sfm_res == result::ok) {
    print_flow();
  } 
  if (flow < 3){
    digitalWrite(TRIGGER1, LOW);
    digitalWrite(TRIGGER2, LOW);
    f_state = fan_state::idle;
  }
}

void set_manual_flow(unsigned char motor_state, unsigned char driver_val, unsigned char motor_val){
  motor = motor_state;
  driver_pwm = driver_val;
  motor_pwm = motor_val;
  f_state = fan_state::sending_manual_flow;
  fan_runtime = 0;
}

void stop_motor(){
  fan_stop();
  motor_pwm = 0;
  driver_pwm = 0;
  f_state = fan_state::idle;
}

void send_manual_flow(){
  if (motor == 0) {
    fan_stop();
    return;
  }
  fan_go();
  print_flow();
}

void print_flow(){
  Serial.print("f:");
  Serial.print(fan_runtime);
  Serial.print(',');
  Serial.print(flow);
  Serial.print(',');
  Serial.print(motor_pwm);
  Serial.print(',');
  Serial.println(driver_pwm);
}

void send_const_flow(){
  if (sent_start == 0){
    Serial.println("start_flow");
    fan_runtime = 0;
    sent_start = 1;
  }

  get_flow(&sfm_res, &flow);
  if (sfm_res == result::ok) {
    print_flow();
  }

  if (fan_runtime >= c_profile.delay){
    digitalWrite(TRIGGER1, HIGH);
    digitalWrite(TRIGGER2, HIGH);
  }

  if (fan_runtime < c_profile.duration && sfm_res == result::ok){
    if (flow < c_profile.flow){
      if (motor_pwm < 255) motor_pwm ++;
      if (driver_pwm < 255) driver_pwm ++;
      fan_go();
    } else if (flow > c_profile.flow){
      if (motor_pwm > 0 ) motor_pwm --;
      if (driver_pwm > 0 ) driver_pwm --;
      fan_stop();
    }
  }
  if (fan_runtime > c_profile.duration){
    fan_stop();
    motor_pwm = 0;
    driver_pwm = 0;
    f_state = fan_state::waiting_for_flow_end;
  }
}