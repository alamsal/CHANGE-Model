//----------------------------------------------------------------------------
// File:		vegetation.h
// Author: 		Mike Wimberly
// Last Update:	1/4/2008
// Decription:	Contains external variable declarations and function prototypes
//              for vegetation.cpp.
//----------------------------------------------------------------------------

#ifndef vegetationH
#define vegetationH

// Define header files
#include <vector>
#include <map>
#include <string>

// POINTERS TO MAIN LANDSCAPE ARRAYS
extern short int *age;	        //  patch age grid
extern short int *tsfire;       //  time since last fire grid
extern char *fgrid2;            //  fire pattern grid
extern char *regime;            //  fire regime grid
extern char *buffer;            //  fire buffer zone grid
extern char *ownergrid;		    //ownership type grid
extern unsigned long int *fsum1;       // summary grid for tallying fire occurence
extern unsigned long int *fsum2;
extern unsigned long int *fsum3;
extern char *severitygrid;     // fire severity grid
extern short int *bioagegrid;
extern double *deadgrid;
extern short int *timeinstage;
extern short int *stategrid;
extern char *comgrid;
extern float *fint;

// OTHER GLOBAL VARIABLES
extern int size;           // total # of cells in landscape (includes background)
extern int sumtype;                 // flag for type of output summary
extern int curfiresev;              // fire severity of current fire
extern int runstep;
extern int biom_flag;
extern int numpath;
extern int suclag[40][40];
extern int suctrans[40][40];
extern int firexlag[40][40];
extern int firextrans[40][40];
extern int suclagreset[40][40];
extern int hsfiretrans[40][40];
extern int hsfirereset[40][40];
extern int msfiretrans[40][40];
extern int msfirereset[40][40];
extern int lsfiretrans[40][40];
extern int lsfirereset[40][40];
extern int fire_pat;
extern double cr_p1[40];
extern double cr_p2[40];
extern double cr_p3a[40];
extern double cr_p3b[40];
extern double hs_mort[40];
extern double ms_mort[40];
extern double hs_consume[40];
extern double ms_consume[40];
extern double landsevmod[40];
extern double landsevmod2[40];
extern double statesev[40][40];
extern double statesev2[40][40];
extern double cursevmod[40][40];
extern double cursevmod2[40][40];	
extern int statecode[40][40];
extern double totburnarea[40];
extern double liburnarea[40];
extern double miburnarea[40];
extern double hiburnarea[40];
extern int stateout[40][40];
extern int distreset[40][40][5];
extern double reldistprob[40][40][5];
extern double anydistprob[40][40];
extern int disttrans[40][40][5];
extern int treattrans[40][40][5];
extern int treatreset[40][40][5];
extern int treatelig[40][40][5];



// Hold filtered raster cells
extern struct lccCells;//hold raster cells
extern unsigned int harvDemand[200][500][10];    // Harvest demand in each mgmt zone


// FUNCTION PROTOTYPES
short int get_struc( int index );
void grow_veg( void );
void disturb_veg( int landfiresum, int sevcnt, int firereg,bool lastFire);
void nsdisturb_veg( int distnum );
bool gentreatment( int index, short int treatmentunit, int treatmentclass );

#endif
