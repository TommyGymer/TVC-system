#include <Arduino.h>
#include "Quat.h"

Quat::Quat(void)
{
	w = 0;
	i = 0;
	j = 0;
	k = 0;
}

Quat::Quat(float _w, float _i, float _j, float _k)
{
	w = _w;
	i = _i;
	j = _j;
	k = _k;
}

Quat Quat::mult(float s)
{
	Quat n(w, i, j, k);
	n.w *= s;
	n.i *= s;
	n.j *= s;
	n.k *= s;
	return n;
}

Quat Quat::div(float s)
{
	Quat n(w, i, j, k);
	n.w /= s;
	n.i /= s;
	n.j /= s;
	n.k /= s;
	return n;
}

Quat Quat::mult(Quat other)
{
	Quat n(0, 0, 0, 0);
	n.w = w * other.w - i * other.i - j * other.j - k * other.k;
	n.i = w * other.i + i * other.w + j * other.k - k * other.j;
	n.j = w * other.j - i * other.k + j * other.w + k * other.i;
	n.k = w * other.k + i * other.j - j * other.i + k * other.w;
	return n;
}

Quat Quat::add(Quat other)
{
	Quat n(w, i, j, k);
	n.w += other.w;
	n.i += other.i;
	n.j += other.j;
	n.k += other.k;
	return n;
}

Quat Quat::sub(Quat other)
{
	Quat n(w, i, j, k);
	n.w -= other.w;
	n.i -= other.i;
	n.j -= other.j;
	n.k -= other.k;
	return n;
}

float Quat::magnitude(){
	return sqrt(sq(w) + sq(i) + sq(j) + sq(k));
}

Quat Quat::normalised()
{
	Quat n(w, i, j, k);
	float m = magnitude();
	n.w /= m;
	n.i /= m;
	n.j /= m;
	n.k /= m;
	return n;
}

Quat Quat::axis_angle()
{
	Quat n(w, i, j, k);
	Quat norm(0, -k, 0, i);
	float s = acos(j);
	n.w = cos(s / 2);
	n.i = norm.i * sin(s / 2);
	n.j = norm.j * sin(s / 2);
	n.k = norm.k * sin(s / 2);
	return n;
}

Quat Quat::axis_angle(float scale)
{
	Quat n(w, i, j, k);
	Quat norm = normalised();
	float s = magnitude() * scale;
	n.w = cos(s / 2);
	n.i = norm.i * sin(s / 2);
	n.j = norm.j * sin(s / 2);
	n.k = norm.k * sin(s / 2);
	return n;
}