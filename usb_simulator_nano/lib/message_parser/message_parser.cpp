#include <Arduino.h>
#include <stdint.h>
#include <message_parser.h>
#include <flow_definition.pb.h>
#include "pb_decode.h"
#include "pb_encode.h"

static pb_ostream_t msg_ostream;

void SendSimulatorMessage(SimulatorMessage *message){
  uint8_t buffer[SimulatorMessage_size];
  msg_ostream = pb_ostream_from_buffer(buffer, sizeof(buffer));
  bool status = pb_encode(&msg_ostream, SimulatorMessage_fields, message);
  size_t written = msg_ostream.bytes_written;

  if (!status) {
    Serial.write("Failed to encode");
  }

  Serial.write(written);
  for (unsigned char i = 0; i < written; i++){
    Serial.write(buffer[i]);
  }
  Serial.write('\n');
}

void SendInterfaceMessage(InterfaceMessage *message){
  uint8_t buffer[InterfaceMessage_size];
  msg_ostream = pb_ostream_from_buffer(buffer, sizeof(buffer));
  bool status = pb_encode(&msg_ostream, InterfaceMessage_fields, message);
  size_t written = msg_ostream.bytes_written;

  if (!status) {
    Serial.write("Failed to encode");
  }

  Serial.write(written);
  for (unsigned char i = 0; i < written; i++){
    Serial.write(buffer[i]);
  }
  Serial.write('\n');
}