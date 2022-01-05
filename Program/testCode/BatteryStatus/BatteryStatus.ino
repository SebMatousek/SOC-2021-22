const int batPin = 36;

int value = 0;

void setup() {
  Serial.begin(115200);
  delay(1000);
 // analogSetWidth(12);
}

void loop() {
  value = analogRead(batPin);
  Serial.println(value);
  delay(500);
}
