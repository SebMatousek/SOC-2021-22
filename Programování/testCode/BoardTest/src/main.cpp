#include <Arduino.h>

const int buzzerPin = 32;

void setup() {
  pinMode(26, OUTPUT);
  pinMode(27, OUTPUT);
  // Set initial rotation direction
  digitalWrite(26, HIGH);
  digitalWrite(27, LOW);
}

void loop() {

  }