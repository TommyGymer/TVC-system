#include <Servo.h>

Servo servo;
int pin = 12;
int pos = 0;

void setup() {
  servo.attach(pin);
}

void loop() {
  for(pos = 0; pos <= 180; pos += 1){
    servo.write(pos);
    delay(20);
  }
  for(pos = 180; pos >= 0; pos -= 1){
    servo.write(pos);
    delay(20);
  }
}
