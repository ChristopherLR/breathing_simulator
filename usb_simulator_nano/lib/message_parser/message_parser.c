#include <message_parser.h>
#include <flow_definition.pb.h>
#include "pb_decode.h"
#include "pb_encode.h"

pb_ostream_t ostream;
pb_istream_t istream;

void SendSimulatorMessage(SimulatorMessage *message){
  uint8_t buffer[SimulatorMessage_size];
  ostream = pb_ostream_from_buffer(buffer, sizeof(buffer));
  bool status = pb_encode(&ostream, SimulatorMessage_fields, message);
  size_t written = ostream.bytes_written;

  if (!status) {
    Serial.println("Failed to encode");
  }

  Serial.write(written);
  for (unsigned char i = 0; i < written; i++){
    Serial.write(buffer[i]);
  }
  Serial.write('\n');
}

void SendInterfaceMessage(InterfaceMessage *message){
  uint8_t buffer[InterfaceMessage_size];
  ostream = pb_ostream_from_buffer(buffer, sizeof(buffer));
  bool status = pb_encode(&ostream, InterfaceMessage_fields, message);
  size_t written = ostream.bytes_written;

  if (!status) {
    Serial.println("Failed to encode");
  }

  Serial.write(written);
  for (unsigned char i = 0; i < written; i++){
    Serial.write(buffer[i]);
  }
  Serial.write('\n');
}