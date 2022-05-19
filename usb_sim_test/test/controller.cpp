#include <Arduino.h>
#include <Servo.h>
#include <stdint.h>
#include <motor_controller.h>

Servo motor;

void ProcessIncomingByte(const unsigned char data);
void ProcessInputLine();

int motor_val = 0;

void setup() {
  Serial.begin(115200);
  motor.attach(10);
}

void loop() {
  while (Serial.available() > 0){
    ProcessIncomingByte(Serial.read());
  }
}

void ProcessInputLine(const char * data){
  String dat = String(data);
  motor_val = dat.toInt();
  Serial.println(motor_val);
  motor.writeMicroseconds(motor_val);
}

void Calibrate(){
  for (int i = 500; i <= 2500; i+= 10){
    motor.writeMicroseconds(i);
    delay(100);
    Serial.print("p: "); Serial.println(i);
  }

  motor.writeMicroseconds(1500);
  Serial.println("fin");
}

const unsigned int MAX_INPUT = 50;
void ProcessIncomingByte(const unsigned char in){
  static char input_line [MAX_INPUT];
  static unsigned int input_pos = 0;
  Serial.print((char)in);

  switch (in){
    case 'p':
      Calibrate();
      break;

    case '\n':   // end of text
      input_line [input_pos] = 0;  // terminating null byte

      ProcessInputLine(input_line);
      
      // reset buffer for next time
      input_pos = 0;  
      break;

    case '\r':   // discard carriage return
      break;

    default:
      // keep adding if not full ... allow for terminating null byte
      if (input_pos < (MAX_INPUT - 1))
        input_line [input_pos++] = in;
      break;
    }
}