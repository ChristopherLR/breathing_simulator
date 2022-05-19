#include <Arduino.h>
#include <stdint.h>

#include "flow_meter.h"

#define BAUD 9600

SFM3003 fm;

void setup() {
  fm.Initialise();
  Serial.begin(BAUD);

  while (!Serial)
    ;

  Serial.println("TEST");

  delay(10);
}

float flow = 0;
void loop() {
  delay(10);
  uint8_t err = fm.GetFlow(&flow);
  if (!err) Serial.println(flow);
}