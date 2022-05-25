#include <Arduino.h>
#include <Wire.h>
#include <elapsedMillis.h>
#include <stdint.h>

#include "flow_meter.h"

#define BAUD 115200
#define HEARTBEAT_DELAY 500
#define BUFFER_SIZE 64

#define LOW 0
#define HIGH 1

#ifdef ARDUINO_TEENSY41
#define ENA A0
#define IN1 36
#define IN2 37
#endif

#if defined(ARDUINO_ARDUINO_NANO33BLE) || defined(ARDUINO_SAMD_NANO_33_IOT)

#define SIGNAL A0
#define EN A7
#define IN1 A6
#define IN2 A3
#endif

elapsedMillis runtime;
elapsedMillis timer;
SFM3003 fm;

char buf[BUFFER_SIZE];

void process_input(const char *);
void process_incoming_byte(const byte);

void setup() {
  Wire.begin();
  Serial.begin(BAUD);

  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(EN, OUTPUT);
  pinMode(SIGNAL, OUTPUT);

  digitalWrite(EN, LOW);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);

  while (!Serial)
    ;  // wait for serial port to connect. Needed for native USB
  fm.Initialise();

  Serial.println("TEST");
}

uint8_t err;
float flow;
uint8_t reset = 0;
uint8_t driver_pwm = 0;

void loop() {
  while (Serial.available() > 0) process_incoming_byte(Serial.read());

  err = fm.GetFlow(&flow);
  if (!err) {
    Serial.print("P: ");
    Serial.print(driver_pwm);
    Serial.print(", F: ");
    Serial.print(flow);
    Serial.print(", T: ");
    Serial.println(timer);
    delay(50);
  }
}

void process_input(const char *data) {
  String dat = String(data + 1);
  driver_pwm = (unsigned char)dat.toInt();
  if (data[0] == 'p') {
    digitalWrite(EN, HIGH);
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    analogWrite(SIGNAL, driver_pwm);
    timer = 0;
  }
  if (data[0] == 'n') {
    digitalWrite(EN, LOW);
  }
}
const unsigned int MAX_INPUT = 50;
void process_incoming_byte(const byte inByte) {
  static char input_line[MAX_INPUT];
  static unsigned int input_pos = 0;

  switch (inByte) {
    case '\n':                    // end of text
      input_line[input_pos] = 0;  // terminating null byte

      process_input(input_line);

      // reset buffer for next time
      input_pos = 0;
      break;

    case '\r':  // discard carriage return
      break;

    default:
      // keep adding if not full ... allow for terminating null byte
      if (input_pos < (MAX_INPUT - 1)) input_line[input_pos++] = inByte;
      break;
  }
}