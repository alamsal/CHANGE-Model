// File:		lccGrow.cpp
// Author: 		Ashis Lamsal
// Last Update:	3/4/2013
// Decription:	Read  and allocate lcc parameters requried for land cover growth
//----------------------------------------------------------------------------
#include <vector>
#include <string>
#include <iostream>
#include "randnum.h"

using namespace std;

//Genreate mean normal distribution patch size 
int genLCCPatchSize(int meanpatch, int stddev)
{
	unsigned int mpatch=meanpatch;
	mpatch=normal_rv(meanpatch, stddev);

	return mpatch;

}


