﻿/*
SimplexNoise 1.0.0
-----
DevDad - Afan Olovcic @ www.art-and-code.com - 08/12/2015

This algorithm was originally designed by Ken Perlin, but my code has been
adapted and extended from the implementation written by Stefan Gustavson (stegu@itn.liu.se)
and modified to fit to Unreal Engine 4


* This is a clean, fast, modern and free Perlin Simplex noise function.
* If we change float to double it could be even faster but there is no double type in Blueprint
* All Public Functions are BlueprintCallable so they can be used in every blueprint


From DevDad and Dedicated to you and Unreal Community
Use it free for what ever you want
I only request that you mention me in the credits for your game in the way that feels most appropriate to you.

*/


#include "SimplexNoiseBPLibrary.h"
#include "SimplexNoisePrivatePCH.h"

// USimplexNoiseBPLibrary
#define FASTFLOOR(x) ( ((x)>0) ? ((int)x) : (((int)x)-1) )

USimplexNoiseBPLibrary::USimplexNoiseBPLibrary(const class FObjectInitializer& PCIP)
	: Super(PCIP)
{

}


unsigned char USimplexNoiseBPLibrary::perm[512] = { 151,160,137,91,90,15,
131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,8,99,37,240,21,10,23,
190, 6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,35,11,32,57,177,33,
88,237,149,56,87,174,20,125,136,171,168, 68,175,74,165,71,134,139,48,27,166,
77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,55,46,245,40,244,
102,143,54, 65,25,63,161, 1,216,80,73,209,76,132,187,208, 89,18,169,200,196,
135,130,116,188,159,86,164,100,109,198,173,186, 3,64,52,217,226,250,124,123,
5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,189,28,42,
223,183,170,213,119,248,152, 2,44,154,163, 70,221,153,101,155,167, 43,172,9,
129,22,39,253, 19,98,108,110,79,113,224,232,178,185, 112,104,218,246,97,228,
251,34,242,193,238,210,144,12,191,179,162,241, 81,51,145,235,249,14,239,107,
49,192,214, 31,181,199,106,157,184, 84,204,176,115,121,50,45,127, 4,150,254,
138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180,
151,160,137,91,90,15,
131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,8,99,37,240,21,10,23,
190, 6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,35,11,32,57,177,33,
88,237,149,56,87,174,20,125,136,171,168, 68,175,74,165,71,134,139,48,27,166,
77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,55,46,245,40,244,
102,143,54, 65,25,63,161, 1,216,80,73,209,76,132,187,208, 89,18,169,200,196,
135,130,116,188,159,86,164,100,109,198,173,186, 3,64,52,217,226,250,124,123,
5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,189,28,42,
223,183,170,213,119,248,152, 2,44,154,163, 70,221,153,101,155,167, 43,172,9,
129,22,39,253, 19,98,108,110,79,113,224,232,178,185, 112,104,218,246,97,228,
251,34,242,193,238,210,144,12,191,179,162,241, 81,51,145,235,249,14,239,107,
49,192,214, 31,181,199,106,157,184, 84,204,176,115,121,50,45,127, 4,150,254,
138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180
};

void USimplexNoiseBPLibrary::setNoiseSeed(const int32& newSeed)
{
	FMath::RandInit(newSeed);
	for (uint16 it = 0; it < 256; ++it)
	{
		uint8 nextNum = FMath::RandRange(0, 255);
		USimplexNoiseBPLibrary::perm[it] = (unsigned char)nextNum;
		USimplexNoiseBPLibrary::perm[it + 256] = (unsigned char)nextNum;
	}
}

void USimplexNoiseBPLibrary::setNoiseFromStream(FRandomStream& RandStream)
{
	for (uint16 it = 0; it < 256; ++it)
	{
		uint8 nextNum = RandStream.RandRange(0, 255);
		USimplexNoiseBPLibrary::perm[it] = (unsigned char)nextNum;
		USimplexNoiseBPLibrary::perm[it + 256] = (unsigned char)nextNum;
	}
}

static unsigned char simplex[64][4] = {
	{ 0,1,2,3 },{ 0,1,3,2 },{ 0,0,0,0 },{ 0,2,3,1 },{ 0,0,0,0 },{ 0,0,0,0 },{ 0,0,0,0 },{ 1,2,3,0 },
	{ 0,2,1,3 },{ 0,0,0,0 },{ 0,3,1,2 },{ 0,3,2,1 },{ 0,0,0,0 },{ 0,0,0,0 },{ 0,0,0,0 },{ 1,3,2,0 },
	{ 0,0,0,0 },{ 0,0,0,0 },{ 0,0,0,0 },{ 0,0,0,0 },{ 0,0,0,0 },{ 0,0,0,0 },{ 0,0,0,0 },{ 0,0,0,0 },
	{ 1,2,0,3 },{ 0,0,0,0 },{ 1,3,0,2 },{ 0,0,0,0 },{ 0,0,0,0 },{ 0,0,0,0 },{ 2,3,0,1 },{ 2,3,1,0 },
	{ 1,0,2,3 },{ 1,0,3,2 },{ 0,0,0,0 },{ 0,0,0,0 },{ 0,0,0,0 },{ 2,0,3,1 },{ 0,0,0,0 },{ 2,1,3,0 },
	{ 0,0,0,0 },{ 0,0,0,0 },{ 0,0,0,0 },{ 0,0,0,0 },{ 0,0,0,0 },{ 0,0,0,0 },{ 0,0,0,0 },{ 0,0,0,0 },
	{ 2,0,1,3 },{ 0,0,0,0 },{ 0,0,0,0 },{ 0,0,0,0 },{ 3,0,1,2 },{ 3,0,2,1 },{ 0,0,0,0 },{ 3,1,2,0 },
	{ 2,1,0,3 },{ 0,0,0,0 },{ 0,0,0,0 },{ 0,0,0,0 },{ 3,1,0,2 },{ 0,0,0,0 },{ 3,2,0,1 },{ 3,2,1,0 } };


double USimplexNoiseBPLibrary::_grad(int hash, double x)
{
	int h = hash & 15;
	double grad = 1.0 + (h & 7);						// Gradient value 1.0, 2.0, ..., 8.0
	if (h & 8) grad = -grad;							// Set a random sign for the gradient
	return (grad * x);									// Multiply the gradient with the distance
}


double USimplexNoiseBPLibrary::_grad(int hash, double x, double y)
{
	int h = hash & 7;									// Convert low 3 bits of hash code
	double u = h < 4 ? x : y;							// into 8 simple gradient directions,
	double v = h < 4 ? y : x;							// and compute the dot product with (x,y).
	return ((h & 1) ? -u : u) + ((h & 2) ? -2.0f*v : 2.0f*v);
}


double USimplexNoiseBPLibrary::_grad(int hash, double x, double y, double z)
{
	int h = hash & 15;									// Convert low 4 bits of hash code into 12 simple
	double u = h < 8 ? x : y;							// gradient directions, and compute dot product.
	double v = h < 4 ? y : h == 12 || h == 14 ? x : z;	// Fix repeats at h = 12 to 15
	return ((h & 1) ? -u : u) + ((h & 2) ? -v : v);
}


double USimplexNoiseBPLibrary::_grad(int hash, double x, double y, double z, double t)
{
	int h = hash & 31;									// Convert low 5 bits of hash code into 32 simple
	double u = h < 24 ? x : y;							// gradient directions, and compute dot product.
	double v = h < 16 ? y : z;
	double w = h < 8 ? z : t;
	return ((h & 1) ? -u : u) + ((h & 2) ? -v : v) + ((h & 4) ? -w : w);
}


double USimplexNoiseBPLibrary::_simplexNoise1D(double x)
{
	int i0 = FASTFLOOR(x);
	int i1 = i0 + 1;
	double x0 = x - i0;
	double x1 = x0 - 1.0;

	double n0, n1;

	double t0 = 1.0 - x0 * x0;
	//  if(t0 < 0.0f) t0 = 0.0f;
	t0 *= t0;
	n0 = t0 * t0 * _grad(perm[i0 & 0xff], x0);

	double t1 = 1.0 - x1 * x1;
	//  if(t1 < 0.0f) t1 = 0.0f;
	t1 *= t1;
	n1 = t1 * t1 * _grad(perm[i1 & 0xff], x1);
	// The maximum value of this noise is 8*(3/4)^4 = 2.53125
	// A factor of 0.395 would scale to fit exactly within [-1,1], but
	// we want to match PRMan's 1D noise, so we scale it down some more.
	return 0.25* (n0 + n1);
}


double USimplexNoiseBPLibrary::_simplexNoise2D(double x, double y)
{
#define F2 0.366025403784438							// F2 = 0.5*(sqrt(3.0)-1.0)
#define G2 0.211324865405187							// G2 = (3.0-Math.sqrt(3.0))/6.0

	double n0, n1, n2;									// Noise contributions from the three corners
	
	// Skew the input space to determine which simplex cell we're in

	double s = (x + y) * F2;							// Hairy factor for 2D
	double xs = x + s;
	double ys = y + s;
	int i = FASTFLOOR(xs);
	int j = FASTFLOOR(ys);

	double t = (double)(i + j) * G2;
	double X0 = i - t;									// Unskew the cell origin back to (x,y) space
	double Y0 = j - t;
	double x0 = x - X0;									// The x,y distances from the cell origin
	double y0 = y - Y0;

	// For the 2D case, the simplex shape is an equilateral triangle.
	// Determine which simplex we are in.
	int i1, j1; // Offsets for second (middle) corner of simplex in (i,j) coords
	if (x0 > y0) { i1 = 1; j1 = 0; } // lower triangle, XY order: (0,0)->(1,0)->(1,1)
	else { i1 = 0; j1 = 1; }      // upper triangle, YX order: (0,0)->(0,1)->(1,1)

								  // A step of (1,0) in (i,j) means a step of (1-c,-c) in (x,y), and
								  // a step of (0,1) in (i,j) means a step of (-c,1-c) in (x,y), where
								  // c = (3-sqrt(3))/6

	double x1 = x0 - i1 + G2; // Offsets for middle corner in (x,y) unskewed coords
	double y1 = y0 - j1 + G2;
	double x2 = x0 - 1.0 + 2.0 * G2; // Offsets for last corner in (x,y) unskewed coords
	double y2 = y0 - 1.0 + 2.0 * G2;

	// Wrap the integer indices at 256, to avoid indexing perm[] out of bounds
	int ii = i & 0xff;
	int jj = j & 0xff;

	// Calculate the contribution from the three corners
	double t0 = 0.5 - x0 * x0 - y0 * y0;
	if (t0 < 0.0f) n0 = 0.0f;
	else {
		t0 *= t0;
		n0 = t0 * t0 * _grad(perm[ii + perm[jj]], x0, y0);
	}

	double t1 = 0.5 - x1 * x1 - y1 * y1;
	if (t1 < 0.0) n1 = 0.0;
	else {
		t1 *= t1;
		n1 = t1 * t1 * _grad(perm[ii + i1 + perm[jj + j1]], x1, y1);
	}

	double t2 = 0.5 - x2 * x2 - y2 * y2;
	if (t2 < 0.0) n2 = 0.0;
	else {
		t2 *= t2;
		n2 = t2 * t2 * _grad(perm[ii + 1 + perm[jj + 1]], x2, y2);
	}

	// Add contributions from each corner to get the final noise value.
	// The result is scaled to return values in the interval [-1,1]
	return 40.0 / 0.884343445 * (n0 + n1 + n2);	//accurate to e-9 so that values scale to [-1, 1], same acc as F2 G2.
}


double USimplexNoiseBPLibrary::_simplexNoise3D(double x, double y, double z)
{

	// Simple skewing factors for the 3D case
#define F3 0.3333333333333333333
#define G3 0.1666666666666666667

	double n0, n1, n2, n3; // Noise contributions from the four corners

						  // Skew the input space to determine which simplex cell we're in
	double s = (x + y + z) * F3; // Very nice and simple skew factor for 3D
	double xs = x + s;
	double ys = y + s;
	double zs = z + s;
	int i = FASTFLOOR(xs);
	int j = FASTFLOOR(ys);
	int k = FASTFLOOR(zs);

	double t = (double)(i + j + k) * G3;
	double X0 = i - t; // Unskew the cell origin back to (x,y,z) space
	double Y0 = j - t;
	double Z0 = k - t;
	double x0 = x - X0; // The x,y,z distances from the cell origin
	double y0 = y - Y0;
	double z0 = z - Z0;

	// For the 3D case, the simplex shape is a slightly irregular tetrahedron.
	// Determine which simplex we are in.
	int i1, j1, k1; // Offsets for second corner of simplex in (i,j,k) coords
	int i2, j2, k2; // Offsets for third corner of simplex in (i,j,k) coords

					/* This code would benefit from a backport from the GLSL version! */
	if (x0 >= y0) {
		if (y0 >= z0)
		{
			i1 = 1; j1 = 0; k1 = 0; i2 = 1; j2 = 1; k2 = 0;
		} // X Y Z order
		else if (x0 >= z0) { i1 = 1; j1 = 0; k1 = 0; i2 = 1; j2 = 0; k2 = 1; } // X Z Y order
		else { i1 = 0; j1 = 0; k1 = 1; i2 = 1; j2 = 0; k2 = 1; } // Z X Y order
	}
	else { // x0<y0
		if (y0 < z0) { i1 = 0; j1 = 0; k1 = 1; i2 = 0; j2 = 1; k2 = 1; } // Z Y X order
		else if (x0 < z0) { i1 = 0; j1 = 1; k1 = 0; i2 = 0; j2 = 1; k2 = 1; } // Y Z X order
		else { i1 = 0; j1 = 1; k1 = 0; i2 = 1; j2 = 1; k2 = 0; } // Y X Z order
	}

	// A step of (1,0,0) in (i,j,k) means a step of (1-c,-c,-c) in (x,y,z),
	// a step of (0,1,0) in (i,j,k) means a step of (-c,1-c,-c) in (x,y,z), and
	// a step of (0,0,1) in (i,j,k) means a step of (-c,-c,1-c) in (x,y,z), where
	// c = 1/6.

	double x1 = x0 - i1 + G3; // Offsets for second corner in (x,y,z) coords
	double y1 = y0 - j1 + G3;
	double z1 = z0 - k1 + G3;
	double x2 = x0 - i2 + 2.0 * G3; // Offsets for third corner in (x,y,z) coords
	double y2 = y0 - j2 + 2.0 * G3;
	double z2 = z0 - k2 + 2.0 * G3;
	double x3 = x0 - 1.0 + 3.0 * G3; // Offsets for last corner in (x,y,z) coords
	double y3 = y0 - 1.0 + 3.0 * G3;
	double z3 = z0 - 1.0 + 3.0 * G3;

	// Wrap the integer indices at 256, to avoid indexing perm[] out of bounds
	int ii = i & 0xff;
	int jj = j & 0xff;
	int kk = k & 0xff;

	// Calculate the contribution from the four corners
	double t0 = 0.6 - x0 * x0 - y0 * y0 - z0 * z0;
	if (t0 < 0.0) n0 = 0.0;
	else {
		t0 *= t0;
		n0 = t0 * t0 * _grad(perm[ii + perm[jj + perm[kk]]], x0, y0, z0);
	}

	double t1 = 0.6 - x1 * x1 - y1 * y1 - z1 * z1;
	if (t1 < 0.0) n1 = 0.0;
	else {
		t1 *= t1;
		n1 = t1 * t1 * _grad(perm[ii + i1 + perm[jj + j1 + perm[kk + k1]]], x1, y1, z1);
	}

	double t2 = 0.6 - x2 * x2 - y2 * y2 - z2 * z2;
	if (t2 < 0.0) n2 = 0.0;
	else {
		t2 *= t2;
		n2 = t2 * t2 * _grad(perm[ii + i2 + perm[jj + j2 + perm[kk + k2]]], x2, y2, z2);
	}

	double t3 = 0.6 - x3 * x3 - y3 * y3 - z3 * z3;
	if (t3 < 0.0) n3 = 0.0;
	else {
		t3 *= t3;
		n3 = t3 * t3 * _grad(perm[ii + 1 + perm[jj + 1 + perm[kk + 1]]], x3, y3, z3);
	}

	// Add contributions from each corner to get the final noise value.
	// The result is scaled to stay just inside [-1,1]
	return 32.0 * (n0 + n1 + n2 + n3); // TODO: The scale factor is preliminary!
}


double USimplexNoiseBPLibrary::_simplexNoise4D(double x, double y, double z, double w)
{
#define F4 0.3090169943749474 // F4 = (Math.sqrt(5.0)-1.0)/4.0
#define G4 0.1381966011250105 // G4 = (5.0-Math.sqrt(5.0))/20.0

	double n0, n1, n2, n3, n4; // Noise contributions from the five corners

							  // Skew the (x,y,z,w) space to determine which cell of 24 simplices we're in
	double s = (x + y + z + w) * F4; // Factor for 4D skewing
	double xs = x + s;
	double ys = y + s;
	double zs = z + s;
	double ws = w + s;
	int i = FASTFLOOR(xs);
	int j = FASTFLOOR(ys);
	int k = FASTFLOOR(zs);
	int l = FASTFLOOR(ws);

	double t = (i + j + k + l) * G4; // Factor for 4D unskewing
	double X0 = i - t; // Unskew the cell origin back to (x,y,z,w) space
	double Y0 = j - t;
	double Z0 = k - t;
	double W0 = l - t;

	double x0 = x - X0;  // The x,y,z,w distances from the cell origin
	double y0 = y - Y0;
	double z0 = z - Z0;
	double w0 = w - W0;

	// For the 4D case, the simplex is a 4D shape I won't even try to describe.
	// To find out which of the 24 possible simplices we're in, we need to
	// determine the magnitude ordering of x0, y0, z0 and w0.
	// The method below is a good way of finding the ordering of x,y,z,w and
	// then find the correct traversal order for the simplex were in.
	// First, six pair-wise comparisons are performed between each possible pair
	// of the four coordinates, and the results are used to add up binary bits
	// for an integer index.
	int c1 = (x0 > y0) ? 32 : 0;
	int c2 = (x0 > z0) ? 16 : 0;
	int c3 = (y0 > z0) ? 8 : 0;
	int c4 = (x0 > w0) ? 4 : 0;
	int c5 = (y0 > w0) ? 2 : 0;
	int c6 = (z0 > w0) ? 1 : 0;
	int c = c1 + c2 + c3 + c4 + c5 + c6;

	int i1, j1, k1, l1; // The integer offsets for the second simplex corner
	int i2, j2, k2, l2; // The integer offsets for the third simplex corner
	int i3, j3, k3, l3; // The integer offsets for the fourth simplex corner

						// simplex[c] is a 4-vector with the numbers 0, 1, 2 and 3 in some order.
						// Many values of c will never occur, since e.g. x>y>z>w makes x<z, y<w and x<w
						// impossible. Only the 24 indices which have non-zero entries make any sense.
						// We use a thresholding to set the coordinates in turn from the largest magnitude.
						// The number 3 in the "simplex" array is at the position of the largest coordinate.
	i1 = simplex[c][0] >= 3 ? 1 : 0;
	j1 = simplex[c][1] >= 3 ? 1 : 0;
	k1 = simplex[c][2] >= 3 ? 1 : 0;
	l1 = simplex[c][3] >= 3 ? 1 : 0;
	// The number 2 in the "simplex" array is at the second largest coordinate.
	i2 = simplex[c][0] >= 2 ? 1 : 0;
	j2 = simplex[c][1] >= 2 ? 1 : 0;
	k2 = simplex[c][2] >= 2 ? 1 : 0;
	l2 = simplex[c][3] >= 2 ? 1 : 0;
	// The number 1 in the "simplex" array is at the second smallest coordinate.
	i3 = simplex[c][0] >= 1 ? 1 : 0;
	j3 = simplex[c][1] >= 1 ? 1 : 0;
	k3 = simplex[c][2] >= 1 ? 1 : 0;
	l3 = simplex[c][3] >= 1 ? 1 : 0;
	// The fifth corner has all coordinate offsets = 1, so no need to look that up.

	double x1 = x0 - i1 + G4; // Offsets for second corner in (x,y,z,w) coords
	double y1 = y0 - j1 + G4;
	double z1 = z0 - k1 + G4;
	double w1 = w0 - l1 + G4;
	double x2 = x0 - i2 + 2.0 * G4; // Offsets for third corner in (x,y,z,w) coords
	double y2 = y0 - j2 + 2.0 * G4;
	double z2 = z0 - k2 + 2.0 * G4;
	double w2 = w0 - l2 + 2.0 * G4;
	double x3 = x0 - i3 + 3.0 * G4; // Offsets for fourth corner in (x,y,z,w) coords
	double y3 = y0 - j3 + 3.0 * G4;
	double z3 = z0 - k3 + 3.0 * G4;
	double w3 = w0 - l3 + 3.0 * G4;
	double x4 = x0 - 1.0 + 4.0 * G4; // Offsets for last corner in (x,y,z,w) coords
	double y4 = y0 - 1.0 + 4.0 * G4;
	double z4 = z0 - 1.0 + 4.0 * G4;
	double w4 = w0 - 1.0 + 4.0 * G4;

	// Wrap the integer indices at 256, to avoid indexing perm[] out of bounds
	int ii = i & 0xff;
	int jj = j & 0xff;
	int kk = k & 0xff;
	int ll = l & 0xff;

	// Calculate the contribution from the five corners
	double t0 = 0.6 - x0 * x0 - y0 * y0 - z0 * z0 - w0 * w0;
	if (t0 < 0.0) n0 = 0.0;
	else {
		t0 *= t0;
		n0 = t0 * t0 * _grad(perm[ii + perm[jj + perm[kk + perm[ll]]]], x0, y0, z0, w0);
	}

	double t1 = 0.6 - x1 * x1 - y1 * y1 - z1 * z1 - w1 * w1;
	if (t1 < 0.0) n1 = 0.0;
	else {
		t1 *= t1;
		n1 = t1 * t1 * _grad(perm[ii + i1 + perm[jj + j1 + perm[kk + k1 + perm[ll + l1]]]], x1, y1, z1, w1);
	}

	double t2 = 0.6 - x2 * x2 - y2 * y2 - z2 * z2 - w2 * w2;
	if (t2 < 0.0) n2 = 0.0;
	else {
		t2 *= t2;
		n2 = t2 * t2 * _grad(perm[ii + i2 + perm[jj + j2 + perm[kk + k2 + perm[ll + l2]]]], x2, y2, z2, w2);
	}

	double t3 = 0.6 - x3 * x3 - y3 * y3 - z3 * z3 - w3 * w3;
	if (t3 < 0.0) n3 = 0.0;
	else {
		t3 *= t3;
		n3 = t3 * t3 * _grad(perm[ii + i3 + perm[jj + j3 + perm[kk + k3 + perm[ll + l3]]]], x3, y3, z3, w3);
	}

	double t4 = 0.6 - x4 * x4 - y4 * y4 - z4 * z4 - w4 * w4;
	if (t4 < 0.0) n4 = 0.0;
	else {
		t4 *= t4;
		n4 = t4 * t4 * _grad(perm[ii + 1 + perm[jj + 1 + perm[kk + 1 + perm[ll + 1]]]], x4, y4, z4, w4);
	}

	// Sum up and scale the result to cover the range [-1,1]
	return 27.0 * (n0 + n1 + n2 + n3 + n4);
}

// 1D Simplex Noise

float USimplexNoiseBPLibrary::SimplexNoise1D(float x, float inFactor)
{
	return (float)_simplexNoise1D(x * inFactor);
}



// 2D Simplex Noise

float USimplexNoiseBPLibrary::SimplexNoise2D(float x, float y, float inFactor)
{
	return (float)_simplexNoise2D(x * inFactor, y * inFactor);
}




// 3D Simplex Noise
float USimplexNoiseBPLibrary::SimplexNoise3D(float x, float y, float z, float inFactor)
{
	return (float)_simplexNoise3D(x * inFactor, y * inFactor, z* inFactor);
}




// 4D Simplex Noise
float USimplexNoiseBPLibrary::SimplexNoise4D(float x, float y, float z, float w, float inFactor)
{
	return (float)_simplexNoise4D(x * inFactor, y * inFactor, z * inFactor, w * inFactor);
}

// Scaled by float value

float USimplexNoiseBPLibrary::SimplexNoiseScaled1D(float x, float scaleOut, float inFactor)
{
	return _simplexNoise1D(x * inFactor) * scaleOut;
}


float USimplexNoiseBPLibrary::SimplexNoiseScaled2D(float x, float y, float scaleOut, float inFactor)
{
	return _simplexNoise2D(x * inFactor, y * inFactor) * scaleOut;
}


float USimplexNoiseBPLibrary::SimplexNoiseScaled3D(float x, float y, float z, float scaleOut, float inFactor)
{
	return _simplexNoise3D((x * inFactor), (y * inFactor), (z * inFactor)) * scaleOut;
}


float USimplexNoiseBPLibrary::SimplexNoiseScaled4D(float x, float y, float z, float w, float scaleOut, float inFactor)
{
	return _simplexNoise4D(x * inFactor, y * inFactor, z * inFactor, w * inFactor) * scaleOut;
};

// Return value in Range between two float numbers
// Return Value is scaled by difference between rangeMin & rangeMax value


float USimplexNoiseBPLibrary::SimplexNoiseInRange1D(float x, float rangeMin, float rangeMax, float inFactor)
{
	if (rangeMax < rangeMin)rangeMax = rangeMin + 1.0f; // prevent negative numbers in that case we will return value between 0 - 1
	return SimplexNoiseScaled1D(x, (rangeMax - rangeMin), inFactor) + rangeMin;
}


float USimplexNoiseBPLibrary::SimplexNoiseInRange2D(float x, float y, float rangeMin, float rangeMax, float inFactor)
{
	if (rangeMax < rangeMin)rangeMax = rangeMin + 1.0f; // prevent negative numbers in that case we will return value between 0 - 1
	return SimplexNoiseScaled2D(x,y, (rangeMax - rangeMin), inFactor) + rangeMin;
}


float USimplexNoiseBPLibrary::SimplexNoiseInRange3D(float x, float y, float z, float rangeMin, float rangeMax, float inFactor)
{
	if (rangeMax < rangeMin)rangeMax = rangeMin + 1.0f; // prevent negative numbers in that case we will return value between 0 - 1
	return SimplexNoiseScaled3D(x,y,z, (rangeMax - rangeMin), inFactor) + rangeMin;
}


float USimplexNoiseBPLibrary::SimplexNoiseInRange4D(float x, float y, float z, float w, float rangeMin, float rangeMax, float inFactor)
{
	if (rangeMax < rangeMin)rangeMax = rangeMin + 1.0f; // prevent negative numbers in that case we will return value between 0 - 1
	return SimplexNoiseScaled4D(x,y,z,w, (rangeMax - rangeMin), inFactor) + rangeMin;
}
