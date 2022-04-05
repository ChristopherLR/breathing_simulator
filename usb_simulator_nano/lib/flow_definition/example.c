/*

void process_input(const char *data, const unsigned int length) { 

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

  ostream = pb_ostream_from_buffer(buffer, sizeof(buffer));
  status = pb_encode(&ostream, InterfaceMessage_fields, &encode);

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
*/