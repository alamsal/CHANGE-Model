//----------------------------------------------------------------------------
// File:		randnum.h
// Author: 		Mike Wimberly
// Last Update:	10/24/2000
// Decription:	Contains definitions and function prototypes
//              for randnum.cpp.
//----------------------------------------------------------------------------

#ifndef randnumbakH
#define randnumbakH
//#include <condefs.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

// Period parameters
#define N 624
#define M 397
#define MATRIX_A 0x9908b0df   /* constant vector a */
#define UPPER_MASK 0x80000000 /* most significant w-r bits */
#define LOWER_MASK 0x7fffffff /* least significant r bits */

// Tempering parameters
#define TEMPERING_MASK_B 0x9d2c5680
#define TEMPERING_MASK_C 0xefc60000
#define TEMPERING_SHIFT_U(y)  (y >> 11)
#define TEMPERING_SHIFT_S(y)  (y << 7)
#define TEMPERING_SHIFT_T(y)  (y << 15)
#define TEMPERING_SHIFT_L(y)  (y >> 18)

// FUNCTION PROTOTYPES
double u0_1(void);
double genrand(void);
void sgenrand(unsigned long seed);
int rand_int( int maxval );
int poisson_rv( double lambda );
int geo_rv( double p );
double normal_rv(double mu, double sigma);
double expon_rv(double lambda);
int negbin_rv(double m, double k);

#endif
