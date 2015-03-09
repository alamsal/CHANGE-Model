//----------------------------------------------------------------------------
// File:		randnum.cpp
// Author: 		Mike Wimberly
// Last Update:	10/24/2000
// Decription:	Contains a u0_1 random number generator and functions for
//              generating other random variables
//----------------------------------------------------------------------------

#pragma hdrstop
#include "randnum.h"

/* A C-program for MT19937: Real number version                */
/*   genrand() generates one pseudorandom real number (double) */
/* which is uniformly distributed on [0,1]-interval, for each  */
/* call. sgenrand(seed) set initial values to the working area */
/* of 624 words. Before genrand(), sgenrand(seed) must be      */
/* called once. (seed is any 32-bit integer except for 0).     */
/* Integer generator is obtained by modifying two lines.       */
/*   Coded by Takuji Nishimura, considering the suggestions by */
/* Topher Cooper and Marc Rieffel in July-Aug. 1997.           */

/* This library is free software; you can redistribute it and/or   */
/* modify it under the terms of the GNU Library General Public     */
/* License as published by the Free Software Foundation; either    */
/* version 2 of the License, or (at your option) any later         */
/* version.                                                        */
/* This library is distributed in the hope that it will be useful, */
/* but WITHOUT ANY WARRANTY; without even the implied warranty of  */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.            */
/* See the GNU Library General Public License for more details.    */
/* You should have received a copy of the GNU Library General      */
/* Public License along with this library; if not, write to the    */
/* Free Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA   */ 
/* 02111-1307  USA                                                 */

/* Copyright (C) 1997 Makoto Matsumoto and Takuji Nishimura.       */
/* When you use this, send an email to: matumoto@math.keio.ac.jp   */
/* with an appropriate reference to your work.                     */

static unsigned long mt[N]; /* the array for the state vector  */
static int mti=N+1; /* mti==N+1 means mt[N] is not initialized */

//----------------------------------------------------------------------------
// Function: 	sgenrang
// Parameters:  seed
// Returns:	    void
// Description: Initialized MT random number generator
//----------------------------------------------------------------------------
void sgenrand(unsigned long seed)
{
    int i;

    for (i=0;i<N;i++) {
         mt[i] = seed & 0xffff0000;
         seed = 69069 * seed + 1;
         mt[i] |= (seed & 0xffff0000) >> 16;
         seed = 69069 * seed + 1;
    }
    mti = N;
}

double genrand(void)
{
    unsigned long y;
    static unsigned long mag01[2]={0x0, MATRIX_A};
    /* mag01[x] = x * MATRIX_A  for x=0,1 */

    if (mti >= N) { /* generate N words at one time */
        int kk;

        if (mti == N+1)   /* if sgenrand() has not been called, */
            sgenrand(4357); /* a default initial seed is used   */

        for (kk=0;kk<N-M;kk++) {
            y = (mt[kk]&UPPER_MASK)|(mt[kk+1]&LOWER_MASK);
            mt[kk] = mt[kk+M] ^ (y >> 1) ^ mag01[y & 0x1];
        }
        for (;kk<N-1;kk++) {
            y = (mt[kk]&UPPER_MASK)|(mt[kk+1]&LOWER_MASK);
            mt[kk] = mt[kk+(M-N)] ^ (y >> 1) ^ mag01[y & 0x1];
        }
        y = (mt[N-1]&UPPER_MASK)|(mt[0]&LOWER_MASK);
        mt[N-1] = mt[M-1] ^ (y >> 1) ^ mag01[y & 0x1];

        mti = 0;
    }
  
    y = mt[mti++];
    y ^= TEMPERING_SHIFT_U(y);
    y ^= TEMPERING_SHIFT_S(y) & TEMPERING_MASK_B;
    y ^= TEMPERING_SHIFT_T(y) & TEMPERING_MASK_C;
    y ^= TEMPERING_SHIFT_L(y);

    return ( (double)y * 2.3283064370807974e-10 ); /* reals */
	//   return y;  /* for integer generation */
}

//----------------------------------------------------------------------------
// Function: 	u0_1
// Parameters:  void
// Returns:	    u0_1 random number
// Description: MT random number generator - see information above
//----------------------------------------------------------------------------
double u0_1(void)
{

    double newnum;
    unsigned x, y;
	
    //x = rand() + 1;
    //y = RAND_MAX + 2;

    //newnum = (double)(x)/(double)(y);
    
		
	newnum = genrand();
		
	return (newnum);

}


//----------------------------------------------------------------------------
// Function: 	rand_int
// Parameters:  maxval = maximum integer value
// Returns:	    random integer between 1 and maxval (inclusive)
//----------------------------------------------------------------------------
int rand_int( int maxval )
{

	int newnum;
	double rand=genrand();
    newnum = (int)( 1 + maxval *rand);

    return( newnum );

}

//----------------------------------------------------------------------------
// Function: 	geo_rv
// Parameters:  p = parameter, expected value = 1/p
// Returns:	    geometric random variable with parameter p
//----------------------------------------------------------------------------
int geo_rv( double p )
{

    double u;

    if( p == 1.0 ) return( 1 );

    u = genrand();
    return(int)( ceil( log( u ) / log( 1.0 - p ) ) );

}

//----------------------------------------------------------------------------
// Function: 	poisson_rv
// Parameters:  lambda = parameter, equal to expected value
// Returns:	    poisson random variable with parameter lambda
//----------------------------------------------------------------------------
int poisson_rv( double lambda )
{

    double u;
    int i;
    double p, f;
    double x;

    i = 0;
    p = exp(-1.0 * lambda);
    f = p;

    u = genrand();

    while( 1 ) {
        if( u < f ) {
            x = i;
            break;
        }

        p = lambda * p / (i + 1);
        f += p;
        i += 1;
    }

    return(int)( x );

}

//----------------------------------------------------------------------------
// Function: 	expon_rv
// Parameters:  lambda = parameter, expected value = 1/lambda
// Returns:	    negative exponential random variable with parameter lambda
//----------------------------------------------------------------------------

double expon_rv(double lambda)
{
    double u;
    double erv;

    u = genrand();
    erv = -1 * lambda * log(u);
    return(erv);

}




//----------------------------------------------------------------------------
// Function: 	normal_rv
// Parameters:  mu =    mean
//              sigma = standard deviation
// Returns:	    normal random variable with parameters mu and sigma
//----------------------------------------------------------------------------
double normal_rv( double mu, double sigma )
{

    double y1, y2;
    double u;
    double z0;
    double z;

    while( 1 ) {
        y1 = expon_rv(1);
        y2 = expon_rv(1);
        if(y2 - pow((y1 - 1), 2)/2 > 0) {
            break;
        }
    }

    u = genrand();
    if(u < 0.5) {
        z0 = -1 * y1;
    } else {
        z0 = y1;
    }

    z = mu + sigma * z0;

    return( z );

}

int negbin_rv(double m, double k) {

    double u;
    double p;
    double pn;
    double pnm1;
    int n;

    u = genrand();

    p = pow(1 + (m / k) , -k);
    pnm1 = p;
    n = 0;

    while( 1 ) {
        if(u < p) {
            break;
        } else {
            n++;
            pn = ((k + n - 1)/n) * (m / (k + m)) * pnm1 ;
            pnm1 = pn;
            p += pn;
        }
    }

    return(n);

}
