//----------------------------------------------------------------------------
// File:		probSurface.h
// Author: 		Ashis Lamsal
// Last Update:	3/4/2013
// Decription:	Contains external variable declarations and function prototypes for probSurface.cpp
//----------------------------------------------------------------------------

#ifndef probSurfaceH
#define probSurfaceH
#include<vector>
//Function Prototypes
//Read probability surface files
void read_probabilitySurfaces(std::vector< std::vector<std::vector< float > > > & grid_value,float NumberOfFiles,float rows,float columns);

#endif