//----------------------------------------------------------------------------
// File:		lccres.h
// Author: 		Ashis Lamsal
// Last Update:	9/26/2013
// Decription:	Contains external variable declarations and function prototypes for lccres.cpp
//----------------------------------------------------------------------------
#ifndef lccresH
#define lccresH

#include <vector>
#include <map>
#include <string>

using namespace std;

// Hold filtered raster cells
// Hold filtered raster cells


// Hold filtered raster cells
extern struct lccCells;

extern ofstream writelog;

extern int maxcol; 
extern unsigned int numOwnership;				        // Number of ownership types
extern unsigned int ownershipCode[40];			        // ID code for ownership types
extern unsigned int ownershipRestriction[40];			// Ownership restriction flag (1= Non-developable, 0=Developable)
extern string ownershipNotAllow[40];                       // Ownership  not allow
extern char *ownergrid;									//ownership type grid

//Generate LCC patch size from patch mean and patch standard deviation - Normal distribution
int genLCCPatchSize(int meanpatch, int stddev);

// Retrun eligible cells after applying the ownership restriction
void getEligibleCellsAfterOwnership (const std::vector<lccCells> &tobe_check_vec_lcc_cells, std::vector<lccCells> &temp_vec_lcc_cells, int lcccode);

//Check ownership restriction and unrestriction
bool getOwnershipStatus( int lccCode, int ownerIndex);

//Model log file
void checkLogExists( const char *name);

#endif
