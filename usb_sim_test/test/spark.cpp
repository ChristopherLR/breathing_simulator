#include <Arduino.h>
#include <Servo.h>
#include <stdint.h>

#include "flow_meter.h"

Servo motor;
SFM3003 fm;

void ProcessInputLine(const char* data);
void ProcessIncomingByte(const unsigned char data);

int motor_val = 0;

void setup() {
  Serial.begin(9600);
  while (!Serial)
    ;
  Serial.println("SPARK TEST");
  fm.Initialise();
  motor.attach(10);
}

uint8_t err;
float flow;

void loop() {
  while (Serial.available() > 0) ProcessIncomingByte(Serial.read());

  err = fm.GetFlow(&flow);
  if (!err) {
    Serial.print("Flow: ");
    Serial.print(flow);
    Serial.print(", Motor: ");
    Serial.println(motor_val);
    delay(50);
  }
}

void ProcessInputLine(const char* data) {
  String dat = String(data);
  motor_val = dat.toInt();
  motor.writeMicroseconds(motor_val);
}

const unsigned int MAX_INPUT = 50;
void ProcessIncomingByte(const unsigned char in) {
  static char input_line[MAX_INPUT];
  static unsigned int input_pos = 0;
  Serial.print((char)in);

  switch (in) {
    case '\n':                    // end of text
      input_line[input_pos] = 0;  // terminating null byte

      ProcessInputLine(input_line);

      // reset buffer for next time
      input_pos = 0;
      break;

    case '\r':  // discard carriage return
      break;

    default:
      // keep adding if not full ... allow for terminating null byte
      if (input_pos < (MAX_INPUT - 1)) input_line[input_pos++] = in;
      break;
  }
}