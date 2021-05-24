void setup() {
  Serial.begin(2000000);
  pinMode(18, INPUT);
}

void loop() {
  Serial.println(analogRead(18));
}
