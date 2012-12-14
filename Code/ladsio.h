//----------------------------------------------------------------------------
// File:		ladsio.h
// Author: 		Mike Wimberly
// Last Update:	1/4/2008
// Decription:	Contains external variable declarations and function prototypes
//              for ladsio.cpp.
//----------------------------------------------------------------------------

#ifndef ladsioH
#define ladsioH

// POINTERS TO MAIN LANDSCAPE ARRAYS
extern short int *age;	        //patch age grid
extern short int *tsfire;       // time since last fire grid
extern char *regime;            // fire regime grid
extern char *buffer;            // fire buffer zone grid
extern unsigned char *temp;              // temporary grid for holding classified veg data
extern unsigned char *temp2;             // landscape transition grid
extern unsigned long int *fsum1;       // summary grid for tallying fire occurence
extern unsigned long int *fsum2;
extern unsigned long int *fsum3;
extern unsigned long int *strucsum;    // summary grid for tallying structure locations
extern int *specgrid;
extern short int *bioagegrid;
extern double *deadgrid;
extern char *zonesumgrid;
extern short int *stategrid;
extern char *comgrid;

// OTHER GLOBAL VARIABLES
extern int maxrow;         // max row and column address of grid
extern int maxcol;
extern int size;           // total # of cells in landscape (includes background)
extern int sumtype;                 // flag for type of output summary
extern int spec_hab;
extern int landsize;
extern int spec_flag;
extern int biom_flag;
extern double cr_p1[40];
extern double cr_p2[40];
extern double cr_p3a[40];
extern double cr_p3b[40];
extern double sum_species[100];
extern double sum_livebio[100];
extern double sum_deadwood[100];
extern double sum_array[40][100];
extern int zsize[100];
extern int numstate;
extern int cellsize;
extern int runstep;

// FUNCTION PROTOTYPES
struct image_header read_grid( char *filename, char *gridname );
struct image_header read_16bit_grid (char *filename, short int *gridname);
struct image_header read_veg_grid( char *filename, short int *gridname );
struct image_header read_veg2_grid( char *filename, short int *gridname, int timestep );
struct image_header read_veg3_grid( char *filename, double *gridname, int timestep );
struct image_header gen_grid( int nrows, int ncols, int cellval, char *gridname );
void gen_strucsum( char *runname, int nclass, int nsum, struct image_header out_head );
void gen_firesum( char *filename, int nsum, struct image_header out_head, int timestep );
void gen_sevsum(char *filename, int nsum, struct image_header out_head);
void gen_snapshot( char *runname, int year, struct image_header out_head,
    int snapsum, int transsum );
void gen_agesum( char *runname, int year, struct image_header out_head, int timestep );
void gen_biosum( char *runname, int year, struct image_header out_head );
void write_grid( char *outfilename, unsigned char *gridname, struct image_header imagehd );
void landscape_sum( int *classnum, int zonesum);
short int get_struc( int index );
#endif
