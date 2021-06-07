#include "PServo.h"

PServo servo(15, 180);                                                        //min: 15   max: 180  range: PI rad
int pin = 12;
int pos = 0;

void setup() {
  Serial.begin(115200);                                             //start the serial connection at 115200 baud
  while (!Serial)                                                   //wait for a serial connection
  Serial.println("Serial connected");
  delay(10);                                                        //pause to ensure connection
  servo.connect(pin);
}

void loop() {
  if(Serial.available() > 0){
    float val = Serial.parseFloat();
    Serial.println(val);
    servo.writeAngle(val);
    Serial.read();
  }
}
