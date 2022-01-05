const int in1 = 27;
const int in2 = 26;

void setup() {
  // put your setup code here, to run once:
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);

  delay(2000);

  /*digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);

  delay(2000);*/
}
