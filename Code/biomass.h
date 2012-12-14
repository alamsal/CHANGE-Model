//----------------------------------------------------------------------------
// File:		biomass.h
// Author: 		Mike Wimberly
// Last Update:	8/2/2002
// Decription:	Contains external variable declarations and function prototypes
//              for biomass.cpp.
//----------------------------------------------------------------------------

#ifndef biomassH
#define biomassH

// POINTERS TO MAIN LANDSCAPE ARRAYS
extern short int *age;	        //  patch age grid
extern short int *tsfire;       //  time since last fire grid
extern char *regime;            //  fire regime grid

// OTHER GLOBAL VARIABLES

extern struct mortality hs_cmort;
extern struct mortality ms_cmort;
extern struct mortality decayrate;
extern double cr_p1[40];
extern double cr_p2[40];
extern double cr_p3a[40];
extern double cr_p3b[40];

// FUNCTION PROTOTYPES
double calc_live_biomass( int index );
int calc_bio_age( double biomass, int index, int lastfiresev );
double calc_dead_decay( int index );
double calc_chronic_mort( int index );

#endif
