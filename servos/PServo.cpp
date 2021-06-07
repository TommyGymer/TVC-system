#include <Arduino.h>
#include <Servo.h>
#include "PServo.h"

PServo::PServo(void){}

bool PServo::connect(int pin)
{
	if(servo.attached()){
		return false;
	}
	servo.attach(pin);
	return true;
}

void PServo::diconnect(void)
{
	servo.detach();
}

void PServo::write(int val)
{
	servo.write(val);
}

void PServo::writeMicro(int val)
{
	servo.writeMicroseconds(val);
}

void PServo::writeAngle(float angle)
{
	servo.write(floor(15 + ((angle / PI) * 165)));
}