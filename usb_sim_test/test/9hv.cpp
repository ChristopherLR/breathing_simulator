#include <Arduino.h>
#include <Wire.h>
#include <stdint.h>
#include <elapsedMillis.h>

#define VERSION "0.1.0"
#define BAUD 115200
#define HEARTBEAT_DELAY 500

#define OFFSET 32000
#define SCALE 140.0
#define FM_ADDRESS 0x40
#define BUFFER_SIZE 64

#if defined(ARDUINO_ARDUINO_NANO33BLE) || defined(ARDUINO_SAMD_NANO_33_IOT)
#define PWM A0
#define EN  A7
#define IN1 A6
#define IN2 A3
#define FAN_RPM A2
#define FAN_PWM A1
#endif

#ifdef ARDUINO_TEENSY41
#define PWM A0
#define EN  35
#define IN1 36
#define IN2 37
#define CONT1 22
#define CONT2 23
#endif

char buf[BUFFER_SIZE];

void process_input(const char *);
void process_incoming_byte(const byte);
char reset = 0;
elapsedMillis runtime;
elapsedMillis timer;

void setup() {

  Wire.begin();
  Serial.begin(BAUD);

  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(EN, OUTPUT);
  pinMode(PWM, OUTPUT);
  pinMode(FAN_PWM, OUTPUT);
  pinMode(FAN_RPM, INPUT);

  // MAX 4095
  analogWriteResolution(12);

  digitalWrite(EN, LOW);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  analogWrite(PWM, 0);
  analogWrite(FAN_PWM, 0);

  while (!Serial); // wait for serial port to connect. Needed for native USB
  Wire.beginTransmission(FM_ADDRESS);
  Wire.write(0x10);
  Wire.write(0x00);
  Wire.endTransmission();

  Serial.println("TEST");
  reset = 1;
  timer = 0;

  digitalWrite(EN, HIGH);
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  analogWrite(PWM, 0);
}


static uint16_t driver_pwm = 0;
static uint16_t fan_pwm = 0;
static uint8_t control1 = 0;
static uint8_t control2 = 0;
static uint16_t counter = 0;
static uint8_t running = 1;

void loop() {
  while (Serial.available() > 0){
    process_incoming_byte(Serial.read());
  }
  static uint16_t a;
  static uint8_t  b;
  static float flow;
  if (reset){
    Wire.beginTransmission(FM_ADDRESS);
    Wire.write(0x20);
    Wire.write(0x00);
    Wire.endTransmission(); 
    Wire.beginTransmission(FM_ADDRESS);
    Wire.write(0x10);
    Wire.write(0x00);
    Wire.endTransmission(); 
    reset = 0;
  }

  if (runtime > 10 && running){
    runtime = 0;
    char avail = Wire.requestFrom(FM_ADDRESS, 2);
    if (avail == 2) {
      a = Wire.read();
      b = Wire.read();
      a = (a << 8) | b;
      flow = ((float)a - OFFSET)/SCALE;
    }  

    Serial.print(driver_pwm);Serial.print(",");
    Serial.print(fan_pwm);Serial.print(",");
    Serial.print(flow); Serial.print(",");
    Serial.println(timer);

    // counter += 1;
    // if (counter >= 500){
    //   driver_pwm += 1;
    //   analogWrite(PWM, driver_pwm);
    //   counter = 0;
    //   if (flow >= 130){
    //     running = false;
    //     digitalWrite(EN, LOW);
    //     driver_pwm = 0;
    //   }
    // }
  }
}

void process_input(const char * data){
  String dat = String(data+1);
  if(data[0] == 'p'){
    driver_pwm = (uint16_t)dat.toInt();
    digitalWrite(EN, HIGH);
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    analogWrite(PWM, driver_pwm);
  }
  if(data[0] == 'f'){
    fan_pwm = (uint16_t)dat.toInt();
    digitalWrite(EN, HIGH);
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    analogWrite(FAN_PWM, fan_pwm);
  }
  if(data[0] == 'n'){
    driver_pwm = 0;
    control1 = 0;
    control2 = 0;
    digitalWrite(EN, LOW);
  }

}
const unsigned int MAX_INPUT = 50;
void process_incoming_byte (const byte inByte){
  static char input_line [MAX_INPUT];
  static unsigned int input_pos = 0;

  switch (inByte){
    case '\n':   // end of text
      input_line [input_pos] = 0;  // terminating null byte

      process_input(input_line);
      
      // reset buffer for next time
      input_pos = 0;  
      break;

    case '\r':   // discard carriage return
      break;

    default:
      // keep adding if not full ... allow for terminating null byte
      if (input_pos < (MAX_INPUT - 1))
        input_line [input_pos++] = inByte;
      break;
    }
}