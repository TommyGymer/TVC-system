#include <Servo.h>

Servo servo;                                                        //min: 15   max: 180  range: PI rad
int pin = 12;
int pos = 0;

void setup() {
  Serial.begin(115200);                                             //start the serial connection at 115200 baud
  while (!Serial)                                                   //wait for a serial connection
  Serial.println("Serial connected");
  delay(10);                                                        //pause to ensure connection
  servo.attach(pin);
}

//takes an angle in radians and returns the equivilent servo value
int valForAngle(float angle){
  return floor(15 + ((angle / PI) * 165));
}

void loop() {
  if(Serial.available() > 0){
    float val = Serial.parseFloat();
    Serial.println(val);
    servo.write(valForAngle(val));
    Serial.read();
  }
}
