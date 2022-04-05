#pragma once

#include <Arduino.h>
#include <stdint.h>
#include <flow_definition.pb.h>

void SendSimulatorMessage(SimulatorMessage *message);
void SendInterfaceMessage(InterfaceMessage *message);

SimulatorMessage version_error = SimulatorMessage_init_zero;
version_error.message_type = SimulatorMessage_MessageType_kError;
version_error.message.error_message = "Version mismatch";

SimulatorMessage invalid_flow = SimulatorMessage_init_zero;
invalid_flow.message_type = SimulatorMessage_MessageType_kError;
invalid_flow.message.error_message = "Invalid Flow";

SimulatorMessage invalid_flow_interval = SimulatorMessage_init_zero;
invalid_flow_interval.message_type = SimulatorMessage_MessageType_kError;
invalid_flow_interval.message.error_message = "Invalid Flow Interval";

SimulatorMessage invalid_event = SimulatorMessage_init_zero;
invalid_event.message_type = SimulatorMessage_MessageType_kError;
invalid_event.message.error_message = "Invalid Event";