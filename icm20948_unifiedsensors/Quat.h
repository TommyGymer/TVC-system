/*
/	Quaternion library for making and interacting with quaternions
*/

#ifndef Quat_h
#define Quat_h

#include <Arduino.h>

class Quat
{
	public:
		Quat();
		Quat(float w, float i, float j, float k);
		Quat mult(float s);
		Quat div(float s);
		Quat mult(Quat other);
		Quat add(Quat other);
		Quat sub(Quat other);
		Quat normalised();
		Quat axis_angle_weight(float weight);
		Quat axis_angle(float scale);
		Quat conjugate();
		Quat inverse();
		Quat rotate(Quat other);
		float magnitude();
		float w;
		float i;
		float j;
		float k;
};

#endif