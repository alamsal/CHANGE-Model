//----------------------------------------------------------------------------
// File:		lcc.h
// Author: 		Ashis Lamsal
// Last Update:	12/16/2012
// Decription:	Contains external variable declarations and function prototypes
//              for lcc.cpp.
//----------------------------------------------------------------------------

#ifndef lccH
#define lccH
// Define header files
#include <vector>
#include <map>
#include <string>

using namespace std;

//Global variables
extern char *lccgrid;           // lcc classes grid
extern char *buffer;			// fire buffer zone grid
extern char *comgrid;			// community type grid
extern char *ownergrid;			// ownership type grid
extern char *hnigrid;
extern short int *stategrid;	//successional state
//Patch Size parameters
extern int pts_distanceLag[40];	 // Input patch distance lag
extern unsigned int pts_patchSize[40];		 // Input mean patch size 
extern unsigned int pts_stdDeviation[40];	 // Input patch standard deviation
extern unsigned int pts_patchLag[40];        // Input patch lag for delinate patch shape
extern short int *timeinstage;	// time in current successional stage grid
extern short int *age;		    // patch age grid
extern short int *tsfire;      // time since last fire grid

extern int size;				// total # of cells in landscape (includes background)
extern unsigned char *temp;				// temporary grid for holding classified veg data

extern int maxrow;         // max row and column address of grid
extern int maxcol;
extern int rowDemand;		//Total no of rows in demand file
extern int colDemand;		//Total no of columns in demand file
extern int numDemand;		//Total no of demand file

extern int numlcc;					//Number of lcc types
extern unsigned int inlcccode[40];			//Input lcc code works upto 40 different LCC types.
extern unsigned int outlcccode[40];			//Output lcc code works upto 40 differtn LCC types
extern unsigned int lcc_flag[40];			//LCC flag- 1 simulate LCC 0 no simulate LCC
extern unsigned int numOwnership;				// Number of ownership types
extern unsigned int ownershipCode[40];			// ID code for ownership types
extern unsigned int ownershipRestriction[40];			// Ownership restriction flag (1= Non-developable, 0=Developable)

extern int meanpatchSize;			//Mean output patch size
extern int numProbsurface; // Number of probability surfaces
extern float transitionThreshold[40]; // Trasitin threshold for each probability class
extern ofstream writelog; //log file	

// Hold filtered raster cells
extern struct lccCells
{
	int lccRow;
	int lccCol;
};

extern std::vector< std::vector<std::vector< int > > > demand_matrix;  //Container to hold demand matrix
extern std::vector< std::vector<std::vector< float > > >probability_surfaces; //Holds the probability surfaces rasters as 3D vector
extern std::vector<int> tempgridFlag;		//temp grid to hold vegetation trasition flag (0- ready for trasition & 1- already changed & no trasnition)
extern std::vector<int>hnitempgridFlag;     //temp grid to hold hni trasition flag (0- ready for trasition & 1- already changed & no trasnition)
//Function prototypes

//Merging original buffer with LCC.
void merg_lccBuffer();	

//LCC neighbour lag  iscompactNeighbour for compacting the neighbourhood cells
bool getNeighbour(int row,int col,int lcccode);
bool getNeighbourLag(int row,int col,int lcccode,int lagdistance, bool iscompactNeighbour);

//Allow broad LCLU classes & HNI transitions
bool cellTrasition(int cellindex, int lcccode, bool hni_trasition);

//Find neighbourhood based on lag distance
std::vector<lccCells> fillNeighborhood(std::vector<lccCells> vecobj, int irow, int icol,int lcccode,int prob_index,int &demand, int &patch_size, int dlag,bool hni_trasition);
std::vector<lccCells> fillEightNeighborhood(std::vector<lccCells> vecobj, int row, int col,int lcccode,int prob_index,int &demand, int &patch_size,bool ishni); // Recursive function to change the landscape's transition

// Extract LCC cells based upon lcccode
std::map <int,std::vector <lccCells> > extract_LandCoverCells(int lccCode); 

//Extract and allocte HNI
void extract_hnicells(int demperiod);

//Extract LCC cells goint to change
void extract_lcccells(int demperiod);

//Get all eligible trasition cells into a vector; vector stores row & columns of all LCC classes including LCC to hni
std::vector<std::map<int,vector<lccCells> > > getextractedCells(std::vector<std::map<int,vector<lccCells> > > &temp_lcc);

//Spatial allocation of LCC demands
void allocate_lccCells(int demandperiod); 

//Spatial allocation of cells in the 2D grid
void space_allocation( std::vector<lccCells> vecobj,int lcccode, int prob_index, int &demand,int lag,unsigned int plag,bool ishni_transition);// Allocate space in lcc grid as well as change the community type

// Merging lcc class into output grid snapshot.Place to merge LADS & FORESCE outputs
void merg_lccSnapshot(); 

//Reclassify into broad landcover class
void reclassify_HumanDominated();

//Reclassify into broad landcover class
void reclassify_NatureDominated(unsigned int stateout[],unsigned int lclustate[],unsigned int statecounter);

//Reclassifying the LADS/FORESCE output into initial LCLU classes.
void reclassify_lclu(unsigned int stateout[],unsigned int lclustate[],unsigned int statecounter);


#endif