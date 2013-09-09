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
extern short int *stategrid;	//successional state

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

extern int meanpatchSize;			//Mean output patch size
extern int numProbsurface; // Number of probability surfaces
extern float transitionThreshold[40]; // Trasitin threshold for each probability class
	

// Hold filtered raster cells
struct lccCells
{
	int lccRow;
	int lccCol;
};

extern std::vector< std::vector<std::vector< int > > > demand_matrix;  //Container to hold demand matrix

extern std::vector< std::vector<std::vector< float > > >probability_surfaces; //Holds the probability surfaces rasters as 3D vector

extern std::vector<int> tempgridFlag;		//temp grid to hold vegetation trasition flag (0- ready for trasition & 1- already changed & no trasnition)

//Function prototypes
void merg_lccBuffer();	//Merging original buffer with LCC.
void merg_lccSnapshot(); // Merging lcc class into output grid snapshot.
void extract_changeCells(int demperiod);//Extract LCC cells goint to change




void allocate_lccCells(int demandperiod); //allocate  LCC cells

void space_allocation( std::vector<lccCells> vecobj,int lcccode, int prob_index, int &demand);// Allocate space in lcc grid as well as change the community type

void reclassify_lclu(unsigned int stateout[],unsigned int lclustate[],unsigned int statecounter);

std::vector<lccCells> fillEightNeighborhood(std::vector<lccCells> vecobj, int row, int col,int lcccode,int prob_index,int &demand, int &patch_size); // Recursive function to change the landscape's transition

//std::vector <lccCells> ext_Cells(char *lccgridname); //Extract vector of cells from LLC

std::map <int,std::vector <lccCells> > extract_LandCoverCells(int lccCode); // Extract LCC cells based upon lcccode

#endif