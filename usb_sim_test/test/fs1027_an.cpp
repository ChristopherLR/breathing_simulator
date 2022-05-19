#include <Arduino.h>
#include <stdint.h>

#define FM_ADDRESS 0x50
#define BAUD 9600
#define FLOW_PIN A0

void ResetSFM();
void StartConstMeasurement();
void StopConstMeasurement();
void ReadFlow();

void setup() {
  Serial.begin(BAUD);

  while (!Serial)
    ;

  Serial.println("TEST");

  ResetSFM();
  StartConstMeasurement();
  delay(10);
}

void ResetSFM() {}

void StartConstMeasurement() {}

void StopConstMeasurement() {}

void PrintBin(int var) {
  for (unsigned int test = 0x80; test; test >>= 1) {
    Serial.write(var & test ? '1' : '0');
  }
  Serial.println();
}

void ReadFlow() {
  uint16_t analogIn = analogRead(FLOW_PIN);
  Serial.println(analogIn);
}

void loop() {
  delay(10);
  ReadFlow();
}