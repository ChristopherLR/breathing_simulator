#pragma once

#include <Arduino.h>
#include <stdint.h>
#include <flow_definition.pb.h>
#include "pb_decode.h"
#include "pb_encode.h"

void SendSimulatorMessage(SimulatorMessage *message);
void SendInterfaceMessage(InterfaceMessage *message);