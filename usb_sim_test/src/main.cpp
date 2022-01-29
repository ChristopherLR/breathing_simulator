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

#define PWM A0
#define EN  35
#define IN1 36
#define IN2 37


char buf[BUFFER_SIZE];

void process_input(const char *);
void process_incoming_byte(const byte);
char reset = 0;

void setup() {

  Wire.begin();
  Serial.begin(BAUD);

  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(EN, OUTPUT);
  pinMode(PWM, OUTPUT);

  digitalWrite(EN, LOW);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  analogWrite(PWM, 0);

  while (!Serial); // wait for serial port to connect. Needed for native USB
  Wire.beginTransmission(FM_ADDRESS);
  Wire.write(0x10);
  Wire.write(0x00);
  Wire.endTransmission();

  Serial.println("TEST");
  reset = 1;
}



void loop() {
  volatile uint16_t a;
  volatile uint8_t  b;
  volatile float flow;
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

  char avail = Wire.requestFrom(FM_ADDRESS, 2);
  if (avail == 2) {
    a = Wire.read();
    b = Wire.read();
    a = (a << 8) | b;
    flow = ((float)a - OFFSET)/SCALE;
    Serial.println(flow);
  } else {
  }

}

void serialEvent(){
  while (Serial.available() > 0){
    process_incoming_byte(Serial.read());
  }
}

static uint8_t driver_pwm = 255;
void process_input(const char * data){
  String dat = String(data+1);
  driver_pwm = (unsigned char)dat.toInt();
  if(data[0] == 'p'){
    Serial.println("POS");
    digitalWrite(EN, HIGH);
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    analogWrite(PWM, driver_pwm);
  }
  if(data[0] == 'n'){
    Serial.println("NEG");
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