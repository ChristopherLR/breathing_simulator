#include <Arduino.h>


void setup() {

  // Wire.begin();
  Serial.begin(9600);
  while (!Serial); // wait for serial port to connect. Needed for native USB

}

void loop() {
  Serial.println("test2");
  delay(100);
}