/*
* Author: Tanner Voas
* Notes: Stores 3 floating point values. Also has a variety of math operations
* which we frequently perform throughout our code.
*/
#ifndef TRIPLET_INCLUDED
#define TRIPLET_INCLUDED

#include <assert.h>
#include <stdio.h>

#include "settings.h"

/**
* Triplet
**/
struct Triplet //Self made 3 variable vector or coordinate system.
{
	float x, y, z;
	// Creates a triplet of given type
	Triplet(float x_, float y_, float z_);
	Triplet(void);

	//Get floating point magnitude of our triplet
	float getFloatMag(void);
	//Get int magnitude
	int getIntMag(void);
	//Returns normalized version of Triplet
	Triplet getNorm(void);
};
//Return dot product of two triplets
float dotTriplet(Triplet a, Triplet b);
//Returns cross product of two triplets
Triplet crossTriplet(Triplet a, Triplet b);
//transformation from one coordinate system to another based on the basis vector b1, b2, and b3.
Triplet transform(float x, float y, float z, Triplet b1, Triplet b2, Triplet b3);
//Interpolates Billinearly 4 vectors based on u and v
Triplet interpolate4(float u, float v, Triplet ll, Triplet ul, Triplet lr, Triplet ur);
//Negates a Triplet object
Triplet negate(Triplet p);
//Reflects a Triplet based off a normal
Triplet reflect(Triplet in, Triplet n);
//Refracts a Triplet based off a normal and the index of refrections
Triplet refract(Triplet in, Triplet n, float old_n, float new_n);
//subtracts, add two triplets
Triplet operator- (Triplet p, Triplet q);
Triplet operator+ (Triplet p, Triplet q);
//Multiplies and divides a triplet by a float
Triplet operator* (Triplet p, float f);
Triplet operator/ (Triplet p, float f);
//Gets string representation of a triplet. Useful for debugging.
char* repr(Triplet p);
#endif
