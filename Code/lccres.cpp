// File:		lccres.cpp
// Author: 		Ashis Lamsal
// Last Update:	3/4/2013
// Decription:	Read  and allocate lcc parameters requried for land cover growth
//----------------------------------------------------------------------------
#include "lcc.h"
#include "randnum.h"
#include "lccres.h"
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <random>
#include <chrono>

using namespace std;

void checkLogExists( const char *name)
{
	ifstream file(name);
	if(file.good())
	{
		file.close();
		remove(name);
		writelog.open(name,fstream::out|fstream::app);
	}
	else
	{
		writelog.open(name,fstream::out|fstream::app);
	}
}

//Genreate mean normal distribution patch size 
int genLCCPatchSize(int meanpatch, int stddev)
{
	unsigned int mpatch=meanpatch;
	mpatch=normal_rv(meanpatch, stddev);

	return mpatch;

}


//Check ownership and change restriction and unrestriction
bool getOwnershipStatus( int lccCode, int ownerIndex)
{
	string allowedNotCode;
	for(unsigned int ownerLayer=0; ownerLayer<numOwnership; ownerLayer++)
	{
		 allowedNotCode=ownershipNotAllow[ownerLayer];

		if((ownergrid[ownerIndex]==ownershipCode[ownerLayer]) && (ownershipRestriction[ownerLayer]==0))
		{
			return true;
		}
		else if(((ownergrid[ownerIndex]==ownershipCode[ownerLayer]) && (ownershipRestriction[ownerLayer]==1)) && (allowedNotCode.find(std::to_string(lccCode))!=string::npos)) 
		{
			return false;		
		}
		else
		{
			return true;
		}
	}


}

// Get eligible cells after applying the ownership restriction
void getEligibleCellsAfterOwnership (const std::vector<lccCells> &tobe_check_vec_lcc_cells, std::vector<lccCells> &temp_vec_lcc_cells, int lcccode)
{
	int tempCol,tempRow,tempIndex;  // Temp variables to filter eligibility
	
	for(unsigned int it=0;it<tobe_check_vec_lcc_cells.size(); it++)
	{
		tempCol=tobe_check_vec_lcc_cells[it].lccCol;
		tempRow=tobe_check_vec_lcc_cells[it].lccRow;
		tempIndex=tempRow*maxcol+tempCol;
		
		if(getOwnershipStatus(lcccode,tempIndex))
		{
			temp_vec_lcc_cells.push_back(tobe_check_vec_lcc_cells[it]);
		}
				
	}

}

//Get eligible cells to harvest
void getEligibleHarvestCells( short int treatmentunit, int treatmentclass, std::vector<lccCells> & harvestEligibleCells )
{

	int index, curstate, curcom, newstate, eligiblecells;
	lccCells egCells;
	eligiblecells = 0;
	harvestEligibleCells.clear();
	for(unsigned int row=0; row<maxrow; row++) 
	{
			for(unsigned int col=0; col<maxcol; col++) 
			{
				index=row*maxcol + col;		

				if(ownergrid[index] == treatmentunit) 
				{

					curstate = stategrid[index];
					curcom = comgrid[index];
			
					if(treatelig[curcom - 1][curstate - 1][treatmentclass] == 1) 
					{
						egCells.lccRow=row;
						egCells.lccCol=col;
						harvestEligibleCells.push_back(egCells);
						eligiblecells++;

					}
				}
				
			}
	}
}


//Get Random arraylist
vector<int> RandFillArr(int arrsize)
{
	int randomNumber;
	vector <int> x;
	int i=0;
	while (i<arrsize)
	{
		        
			randomNumber = rand_int(arrsize);
			bool exists = std::find(x.begin(), x.end(), randomNumber) != x.end();
			if(!exists)
			{
				x.push_back(randomNumber);
				i++;
			}
		
	}
	

	return x;
}

