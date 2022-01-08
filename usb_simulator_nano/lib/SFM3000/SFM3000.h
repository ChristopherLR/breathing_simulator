#pragma once

#include <enums.h>
#include <Arduino.h>

#define SCALE 140.0
#define OFFSET 32000
#define FM_ADDRESS 0x40

result initialise_sfm();
void get_flow(result * res, float * value);
