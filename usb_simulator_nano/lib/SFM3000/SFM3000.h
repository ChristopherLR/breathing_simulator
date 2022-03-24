#pragma once

#include <enums.h>
#include <Arduino.h>

#define SCALE 140.0
#define OFFSET 32000
#define FM_ADDRESS 0x40

Result InitialiseSfm();
void GetFlow(Result * res, float * value);
