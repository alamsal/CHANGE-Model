//----------------------------------------------------------------------------
// File:		fire.h
// Author: 		Mike Wimberly
// Last Update:	1/4/2008
// Decription:	Contains external variable declarations and function prototypes
//              for fire.cpp.
//----------------------------------------------------------------------------

#ifndef firesH
#define firesH

// POINTERS TO MAIN LANDSCAPE ARRAYS
extern short int *dem;
extern short int *age;		    // patch age grid
extern short int *tsfire;      // time since last fire grid
extern char *regime;           // fire regime grid
extern char *buffer;           // fire buffer zone grid
extern char *landgrid;             // slope position
extern char *comgrid;
extern char *fgrid1;           // fire grid #1
extern char *fgrid2;           // fire grid #2
extern float *fsusc;            // fire susceptibility grid
extern float *fint;
extern unsigned long int *fsum1;       // summary grid for tallying fire occurence
extern unsigned short int *processed;   // patch grid - flags cells in patch
extern unsigned short int *patchx;      // patch grid - x coords of patch cells
extern unsigned short int *patchy;      // patch grid - y coords of patch cells
extern unsigned long int *strucsum;    // summary grid for tallying structure locations
extern short int *stategrid;
extern char *firespreadgrid;

// OTHER GLOBAL VARIABLES
extern int burned;			// total number of cell burned by a given fire
extern int maxrow;         // max row and column address of grid
extern int maxcol;
extern int size;           // total # of cells in landscape (includes background)
extern int landsize;       // total # of cells in active landscape (no background)
extern int cellsize;       // length of one side of a cell (m)
extern int nslopeclass;
extern int torus;
extern int mintsfire;
extern double fire_cal; // fire spread calibration parameter
extern double fire_cal2;
extern double windfact[3][3];  // wind modifier to fire susceptibility
extern int sumtype;            // flag for type of output summary
extern double w_speed;      // prevailing wind speed
extern double w_dir;        // prevailing wind direction
extern double w_var;        // range of variability in wind direction
extern double w_shift;      // probability of wind shift during fire spread
extern double cell_ha;
extern double statefiremod[40][40];
extern double statesev[40][40];
extern double statefireinit[40][40];
extern double lowerslope[40];
extern double upperslope[40];
extern double slopefact[40];
extern double landfiremod[40];
extern double landfireinit[40];

// FUNCTION PROTOTYPES
int get_perim( void );
int fill_islands( void );
int firespread( int firereg, int fsize );
void set_winddir( double w_dir, double w_var, double w_speed );
double gen_firesev( double sevcut1, double sevcut2 );
double gen_firesize( double meansize, double stdsize, int sizedist );
double get_slopefact( double perc_slope );
double get_elongation( void );

#endif
