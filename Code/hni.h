//----------------------------------------------------------------------------
// File:		hni.h
// Author: 		Aashis Lamsal
// Last Update:	11/44/2013
// Decription:	Contains external variable declarations and function prototypes
//              for wui.cpp.
//----------------------------------------------------------------------------
#ifndef hniH
#define hniH

// Define header files
#include <vector>
#include <map>
#include <string>

using namespace std;
//Global variables

extern int size;				// total # of cells in landscape (includes background)
extern int maxrow;         // max row and column address of grid
extern int maxcol;
extern int numProbsurface; // Number of probability surfaces
extern float transitionThreshold[40]; // Trasitin threshold for each probability class
extern int numlcc;					//Number of lcc types
extern unsigned int inlcccode[40];			//Input lcc code works upto 40 different LCC types.

extern char *lccgrid;           // lcc classes grid
extern char *buffer;			// fire buffer zone grid
extern char *comgrid;			// community type grid
extern char *ownergrid;			// ownership type grid
extern short int *stategrid;	//successional state
extern char *hnigrid;	       //Human natural interface grid
// Hold filtered raster cells
extern struct lccCells;//hold raster cells

//Patch Size parameters
extern int pts_distanceLag[40];	 // Input patch distance lag
extern unsigned int pts_patchSize[40];		 // Input mean patch size 
extern unsigned int pts_stdDeviation[40];	 // Input patch standard deviation
extern unsigned int pts_patchLag[40];        // Input patch lag for delinate patch shape

extern std::vector< std::vector<std::vector< int > > > demand_matrix;  //Container to hold demand matrix
extern std::vector< std::vector<std::vector< float > > >probability_surfaces; //Holds the probability surfaces rasters as 3D vector

extern std::vector<int>hnitempgridFlag;     //temp grid to hold hni trasition flag (0- ready for trasition & 1- already changed & no trasnition)

//Allocate cells from hni to LCC
void allocate_hni2lcc(int demperiod);
//Allocate cells from LCC to hni
void allocate_lcc2hni(std::vector <lccCells>lcc2hni_vec,int prob_index, int &demand,int hnicode,int hnilag,int hni_plag);
//Extract elibigle cells from LCC to hni
void extract_allocate_lcc2hni(int demperiod);
//
void extract_hni2lcc(std::map<int,vector<lccCells> > &ext_hni2lcc_vector);

//Determine HNI neighbour lag
bool getHnilag(int row,int col,int hcode,int lagdistance, bool iscompactNeighbour);


#endif
