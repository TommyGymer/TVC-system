/*
	A servo library for causing an angle change
*/

#ifndef PServo_h
#define PServo_h

#include <Arduino.h>
#include <Servo.h>

class PServo
{
	public:
		Servo servo;
		PServo();
		PServo(int min, int max) : _min(min), _max(max) {}
		PServo(int min, int max, float range) : _min(min), _max(max), _range(range) {}
		bool connect(int pin);
		void diconnect(void);
		void write(int val);
		void writeMicro(int val);
		void writeAngle(float angle);
		int _pin{12};
		int _min{15};
		int _max{180};
		float _range{PI};
};

#endif