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

#define DRIVER_POS 37
#define DRIVER_NEG 36
#define DRIVER_PWM 15


char buf[BUFFER_SIZE];

void process_data(const char *);
void process_incoming_byte(const byte);

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
  Wire.beginTransmission(FM_ADDRESS);
  Wire.write(0x10);
  Wire.write(0x00);
  Wire.endTransmission();

  Serial.println("TEST");
}

volatile char reset = 0;


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

const unsigned int MAX_INPUT = 50;
void process_data (const char * data){
  Serial.println (data);
}

void process_incoming_byte (const byte inByte){
  static char input_line [MAX_INPUT];
  static unsigned int input_pos = 0;

  switch (inByte){
    case '\n':   // end of text
      input_line [input_pos] = 0;  // terminating null byte

      process_data (input_line);
      
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