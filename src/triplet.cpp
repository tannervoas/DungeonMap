
#include <math.h>
#include <stdlib.h>
#include "triplet.h"

Triplet::Triplet(void)
{
	x = 0;
	y = 0;
	z = 0;
}

Triplet::Triplet(float x_, float y_, float z_)
{
	x = x_;
	y = y_;
	z = z_;
}

float Triplet::getFloatMag(void)
{
	return (float) sqrt(pow(x,2) + pow(y,2) + pow(z,2));
}

int Triplet::getIntMag(void)
{
	return (int)sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2));
}

Triplet Triplet::getNorm(void)
{
	float m = getFloatMag();
	return Triplet(x/m,y/m,z/m);
}

float dotTriplet(Triplet a, Triplet b)
{
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

Triplet crossTriplet(Triplet a, Triplet b)
{
	return Triplet(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x);
}

Triplet transform(float x, float y, float z, Triplet b1, Triplet b2, Triplet b3)
{
	float x_ = x * b1.x + y * b2.x + z * b3.x;
	float y_ = x * b1.y + y * b2.y + z * b3.y;
	float z_ = x * b1.z + y * b2.z + z * b3.z;
	return Triplet(x_, y_, z_);
}

Triplet interpolate4(float u, float v, Triplet ll, Triplet ul, Triplet lr, Triplet ur)
{
	float x = (1 - v) * (u * ul.x + (1 - u) * ur.x) + v * (u * ll.x + (1 - u) * lr.x);
	float y = (1 - v) * (u * ul.y + (1 - u) * ur.y) + v * (u * ll.y + (1 - u) * lr.y);
	float z = (1 - v) * (u * ul.z + (1 - u) * ur.z) + v * (u * ll.z + (1 - u) * lr.z);
	return Triplet(x, y, z);
}

Triplet negate(Triplet p)
{
	return Triplet(-1 * p.x, -1 * p.y, -1 * p.z);
}

Triplet refract(Triplet in, Triplet n, float old_n, float new_n)
{
	in = negate(in);
	float angle = acosf(dotTriplet(in, n));
	float mul = old_n / new_n;
	float n_angle = asinf(mul * sinf(angle));
	Triplet T = n * (mul * cos(angle) - cos(n_angle)) - in * mul;
	return T;
}

Triplet reflect(Triplet in, Triplet n)
{
	Triplet r = in - n * (2 * dotTriplet(in, n));
	return r;
}

Triplet operator- (Triplet p, Triplet q)
{
	return Triplet(p.x - q.x, p.y - q.y, p.z - q.z);
}

Triplet operator+(Triplet p, Triplet q)
{
	return Triplet(p.x + q.x, p.y + q.y, p.z + q.z);
}

Triplet operator*(Triplet p, float f)
{
	return Triplet(p.x * f, p.y * f, p.z * f);
}

Triplet operator/(Triplet p, float f)
{
	return Triplet(p.x / f, p.y / f, p.z / f);
}

char* repr(Triplet p)
{
	char* buff = (char*)malloc(sizeof(char) * 100);
	sprintf(buff, "Triplet: (%f %f %f)", p.x, p.y, p.z);
	return buff;
}
