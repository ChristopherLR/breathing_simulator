#include <Arduino.h>
#include <elapsedMillis.h>
#include <flow_definition.pb.h>
#include <stdint.h>

#include "pb_decode.h"
#include "pb_encode.h"

#define VERSION "0.1.0"
#define BAUD 115200
#define HEARTBEAT_DELAY 500

#define OFFSET 32000
#define SCALE 140.0
#define FM_ADDRESS 0x40
#define BUFFER_SIZE 64

#if defined(ARDUINO_ARDUINO_NANO33BLE) || defined(ARDUINO_SAMD_NANO_33_IOT)
#define PWM A0
#define EN A7
#define IN1 A6
#define IN2 A3
#define TRIGGER1 A1
#define TRIGGER2 A2
#define LOW 0
#define HIGH 1
#endif
/*
#ifdef ARDUINO_TEENSY41
#define PWM A0
#define EN 35
#define IN1 36
#define IN2 37
#define CONT1 22
#define CONT2 23
#endif
*/

char buf[BUFFER_SIZE];

void process_input(const char *, const unsigned int);
void process_incoming_byte(const byte);

void setup() {
  Serial.begin(BAUD);

  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(EN, OUTPUT);
  pinMode(PWM, OUTPUT);

  // MAX 4095
  analogWriteResolution(12);

  digitalWrite(EN, LOW);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  analogWrite(PWM, 0);

  while (!Serial); // wait for serial port to connect. Needed for native USB
  Serial.println("TEST");
}

pb_ostream_t ostream;
pb_istream_t istream;

void loop() {
  // // static char buffer[256];
  // uint8_t buffer[InterfaceMessage_size];
  // size_t written;

  // InterfaceMessage encode = InterfaceMessage_init_zero;
  // encode.message_type = InterfaceMessage_MessageType::InterfaceMessage_MessageType_kConstantFlow;
  // encode.which_message = InterfaceMessage_constant_flow_tag; 
  // encode.message.constant_flow.flow = 10.0;
  // encode.message.constant_flow.duration = 100;

  // ostream = pb_ostream_from_buffer(buffer, sizeof(buffer));
  // bool status = 0;
  // status = pb_encode(&ostream, InterfaceMessage_fields, &encode);

  // if (!status) {
  //   Serial.println("Failed to encode");
  // }

  // written = ostream.bytes_written;
  // Serial.print("Written: "); Serial.println(written);

  // istream = pb_istream_from_buffer(buffer, written);
  // InterfaceMessage decoded = InterfaceMessage_init_zero;

  // status = pb_decode(&istream, InterfaceMessage_fields, &decoded);
  // if (!status) {
  //   Serial.println("Failed to decode");
  // }

  // Serial.print("Type: ");
  // Serial.println(decoded.message_type);
  // Serial.print("Flow: ");
  // Serial.println(decoded.message.constant_flow.flow);
  // Serial.print("Duration: ");
  // Serial.println(decoded.message.constant_flow.duration);
  while (Serial.available() > 0){
    process_incoming_byte(Serial.read());
  }
}

void process_input(const char *data, const unsigned int length) { 

  // Serial.print("Length: "); Serial.println(length);
  // for (unsigned int i = 0; i < length; i++) {
  //   Serial.write(data[i]);
  // }
  // Serial.write('\n');

  istream = pb_istream_from_buffer((const pb_byte_t*)data, length);
  InterfaceMessage decoded = InterfaceMessage_init_zero;

  bool status = pb_decode(&istream, InterfaceMessage_fields, &decoded);
  if (!status) {
    Serial.write("Failed to decode\n");
  }

  uint8_t buffer[InterfaceMessage_size];
  InterfaceMessage encode = InterfaceMessage_init_zero;

  switch (decoded.message_type) {
    case InterfaceMessage_MessageType_kVersionInfo:
      encode.message_type = InterfaceMessage_MessageType_kVersionInfo;
      encode.which_message = InterfaceMessage_version_info_tag; 
      encode.message.version_info.major = decoded.message.version_info.major;
      encode.message.version_info.minor = decoded.message.version_info.minor;
      encode.message.version_info.patch = decoded.message.version_info.patch;
      break;
    case InterfaceMessage_MessageType_kConstantFlow: encode.message_type = InterfaceMessage_MessageType_kConstantFlow;
      encode.which_message = InterfaceMessage_constant_flow_tag; 
      encode.message.constant_flow.flow = decoded.message.constant_flow.flow;
      encode.message.constant_flow.duration = decoded.message.constant_flow.duration;
      break;
    case InterfaceMessage_MessageType_kManualFlow:
      encode.message_type = InterfaceMessage_MessageType_kManualFlow;
      encode.which_message = InterfaceMessage_manual_flow_tag; 
      encode.message.manual_flow.flow = decoded.message.manual_flow.flow;
      encode.message.manual_flow.driver = decoded.message.manual_flow.driver;
      encode.message.manual_flow.fan_direction = decoded.message.manual_flow.fan_direction;
      break;
    case InterfaceMessage_MessageType_kDynamicFlow:
      encode.message_type = InterfaceMessage_MessageType_kDynamicFlow;
      encode.which_message = InterfaceMessage_dynamic_flow_tag; 
      encode.message.dynamic_flow.duration = decoded.message.dynamic_flow.duration;
      encode.message.dynamic_flow.count = decoded.message.dynamic_flow.count;
      encode.message.dynamic_flow.interval = decoded.message.dynamic_flow.interval;
      break;
    case InterfaceMessage_MessageType_kDynamicFlowInterval:
      encode.message_type = InterfaceMessage_MessageType_kDynamicFlowInterval;
      encode.which_message = InterfaceMessage_dynamic_flow_interval_tag; 
      encode.message.dynamic_flow_interval.interval = decoded.message.dynamic_flow_interval.interval;
      encode.message.dynamic_flow_interval.flow = decoded.message.dynamic_flow_interval.flow;
      encode.message.dynamic_flow_interval.final = decoded.message.dynamic_flow_interval.final;
      break;
    case InterfaceMessage_MessageType_kInformationRequest:
      encode.message_type = InterfaceMessage_MessageType_kInformationRequest;
      encode.which_message = InterfaceMessage_information_request_tag; 
      encode.message.information_request.data_type = decoded.message.information_request.data_type;
      break;
    default:
      break;
  }

  // Serial.print("Type: ");
  // Serial.println(decoded.message_type);
  // Serial.print("Flow: ");
  // Serial.println(decoded.message.constant_flow.flow);
  // Serial.print("Duration: ");
  // Serial.println(decoded.message.constant_flow.duration);


  // encode.message_type = InterfaceMessage_MessageType::InterfaceMessage_MessageType_kConstantFlow;
  // encode.which_message = InterfaceMessage_constant_flow_tag; 
  // encode.message.constant_flow.flow = 10.0;
  // encode.message.constant_flow.duration = 5;

  ostream = pb_ostream_from_buffer(buffer, sizeof(buffer));
  status = pb_encode(&ostream, InterfaceMessage_fields, &encode);

  size_t written = ostream.bytes_written;

  // Serial.print("Written: ");Serial.println(written);

  if (!status) {
    Serial.println("Failed to encode");
  }

  // Serial.print(written);
  Serial.write(written);
  for (unsigned char i = 0; i < written; i++){
    Serial.write(buffer[i]);
    // Serial.print(buffer[i]);
  }
  Serial.write('\n');

}

const unsigned int kMaxInput = 256;
void process_incoming_byte(const unsigned char length) {
  static char input_line[kMaxInput];

  for (unsigned char i = 0; i < length; i++){
    input_line[i] = Serial.read();
  }
  unsigned char end = Serial.read();
  process_input(input_line, length);

  // switch (in_byte) {
  //   case '\n':                    // end of text
  //     input_line[input_pos] = 0;  // terminating null byte
  //     process_input(input_line, input_pos);
  //     input_pos = 0; // reset buffer for next time
  //     break;
  //   default:
  //     // keep adding if not full ... allow for terminating null byte
  //     if (input_pos < (kMaxInput - 1)) input_line[input_pos++] = in_byte;
  //     break;
  // }
}