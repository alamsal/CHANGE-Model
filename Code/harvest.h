//----------------------------------------------------------------------------
// File:		harvest.h
// Author: 		Aashis Lamsal
// Last Update:	4/4/2014
// Decription:	Contains external variable declarations and function prototypes
//              for wui.cpp.
//----------------------------------------------------------------------------
#ifndef harvestH
#define harvestH

#include <vector>
#include <map>
#include <string>

using namespace std;


// Hold filtered raster cells
extern struct lccCells;
extern char *harvestgrid;		// Harvest grid
extern std::vector<int> harvtempgridFlag; //Harvest temp grid
extern int NO_OF_ITERATION;

//Allocate harvest
void allocateHarvest(std::vector<lccCells> harvCells,  int harvDemand, int mharvpatchSize,short int treatmentunit, int treatmentclass);
//Fill harvest neighbour based on lag distance
std::vector<lccCells> fillHarvestNeighborhood(std::vector<lccCells> vecobj, int irow, int icol,int &demand, int &patch_size, int distlag, int mpatchsize);




#endif