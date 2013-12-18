//----------------------------------------------------------------------------
// File:		hni.cpp
// Author: 		Aashis Lamsal
// Last Update:	11/14/2013
// Decription:	Contains functions to simulate hni 
//----------------------------------------------------------------------------

#include "hni.h"
#include "lcc.h"
#include "lccGrow.h"
#include "randnum.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <string>

# define  hnicode 2

using namespace std;

//extract eligible cells from hni to LCC
//ext_hni2lcc_vector - Map of vectors containing sturcuture to hold all extracted values based upon probability surfaces, LCC, and ownership.

void extract_hni2lcc(std::map<int,vector<lccCells> > &ext_hni2lcc_vector)
{
	lccCells hni2lcccells; //container to keep raster hni cell row columns
	ext_hni2lcc_vector.clear();
	unsigned int hindex=0; //Hni grid index
	for (unsigned int hrow=0;hrow<maxrow;hrow++)
	{
		for(unsigned int hcol=0;hcol<maxcol;hcol++)
		{
			hindex=hrow*maxcol+hcol;
			//Extracting hni cells only 2 are hni grid
			if((int)hnigrid[hindex]==2)
			{
				//filter Lcc based on owenership type and development restriction
				for (unsigned int olayer=0;olayer<numOwnership;olayer++)
				{
					if((ownergrid[hindex]==ownershipCode[olayer]) &&	(ownershipRestriction[olayer]==0))
					{

						for(unsigned int nprbsrfce=0;nprbsrfce<numProbsurface-1;nprbsrfce++)   // The hni should always be in the last of the LCC type. Here numProbsurface-1 represents all LCC classes except hni
						{
							if(probability_surfaces[nprbsrfce][hrow][hcol]>transitionThreshold[nprbsrfce])
							{
								hni2lcccells.lccRow=hrow;
								hni2lcccells.lccCol=hcol;
								ext_hni2lcc_vector[nprbsrfce].push_back(hni2lcccells);
							}
						}
					}
				}
			}
		}
	}
}
//Determine HNI neighbour lag
bool getHnilag(int row,int col,int hcode,int lagdistance)
{
	int rwlag=-lagdistance;
	int cclag=-lagdistance;
	bool isNeighboor=false;
	int cindex;
	if(((row<maxrow) && (row>0)) && ((col<maxcol)&& (col>0)))
	{
		try{
			for (int rlag=rwlag;rlag<lagdistance+1;rlag++)
			{
				for(int clag=cclag; clag<lagdistance+1;clag++ )
				{					
					cindex=((row-rlag)*maxcol+(col-clag));
					if((cindex<size) && (cindex>-1))
					{						
						if((int)hnigrid[cindex]==hcode)
						{
							if((rlag!= 0)||(clag=!0))
							{
								isNeighboor=true;
								return true;
							}
						}

					}					
				}
			}
			if(!isNeighboor)
			{
				return false;		
			}
		}
		catch(exception e) 
		{
			cout<<e.what();	
			cout<<"HNI neighbour LAG out of Bound:: Press ENTER to continue"<<endl;
			cin.get();
		}
	}
}
//Extract elibigle cells from LCC to hni
void extract_allocate_lcc2hni(int demperiod)
{
	
	int hnidemand,hrow,hcol,hindex;
	lccCells temphnicells;

	std::vector<std::map<int,vector<lccCells> > > extracted_lcc2hni;// Map of vectors containing sturcuture to hold all extracted values based upon probability surfaces, LCC, and ownership.
	extracted_lcc2hni.clear();
	
	getextractedCells(extracted_lcc2hni); 
	
	// container setup to store cells eligible from lcc to WUI
	std::map<int,vector<lccCells> > lcc2hni_cells;
	std::vector<lccCells> vec_lcc2hni_cells, vegetated_vec_lcc2hnicells;
	
	//Read eligible lcc2hni cells
	for(unsigned int row=0; row<numlcc;row++)	
	{
		lcc2hni_cells=extracted_lcc2hni[row];
		//extracting eligible lcc2hnicells
		vec_lcc2hni_cells=lcc2hni_cells[numlcc];  

		//selecting only natrual vegetated cells to go into hni
		for(unsigned index=0;index<vec_lcc2hni_cells.size();index++)
		{
			hrow=vec_lcc2hni_cells.at(index).lccRow;
			hcol=vec_lcc2hni_cells.at(index).lccCol;
			hindex=hrow*maxcol + hcol;

			if((lccgrid[hindex]==42) && (hnigrid[hindex]==1))
			{
				temphnicells.lccCol=hcol;
				temphnicells.lccRow=hrow;
				vegetated_vec_lcc2hnicells.push_back(temphnicells);
			}

		}
		cout<<"Eligible cells:"<<vegetated_vec_lcc2hnicells.size()<<endl;
		if(vegetated_vec_lcc2hnicells.size()>0)
		{
			for(unsigned int col=numlcc;col<numProbsurface;col++)
			{
				hnidemand=(int)demand_matrix[demperiod][row][col];
				cout<<"Demand:"<<hnidemand<<endl;
				allocate_lcc2hni(vegetated_vec_lcc2hnicells,col,hnidemand,hnicode,50);
				vegetated_vec_lcc2hnicells.clear();
			}
		}

	}

}
//Allocate cells from LCC to hni
void allocate_lcc2hni(std::vector <lccCells>lcc2hni_vec,int prob_index, int &demand,int hcode,int hnilag)
{
	int rand_lccrow;
	int rand_lcccol;
	unsigned int rand_lccindex,hni_index;
	double trans_prob;
	double irand;
	bool lag;

	if(((demand>0) && (lcc2hni_vec.size()>0)) &&(demand<=lcc2hni_vec.size()))
	{
		while((lcc2hni_vec.size()>0) && (demand>0))
		{
			//Generate a random integer between 1 and eligible cell size
			rand_lccindex=(unsigned int)rand_int(lcc2hni_vec.size());
			//Retrive random index from vector
			rand_lccrow=lcc2hni_vec.at(rand_lccindex-1).lccRow;
			rand_lcccol=lcc2hni_vec.at(rand_lccindex-1).lccCol;

			hni_index= rand_lccrow*maxcol+rand_lcccol;
			//Transition probability value
			trans_prob=(double)	probability_surfaces[prob_index][rand_lccrow][rand_lcccol];
			//Generate a uniform random variable for the forest cell
			irand=u0_1();
			if(irand<trans_prob)
			{
				lag=getHnilag(rand_lccrow,rand_lcccol,hcode,hnilag);
				if(lag)
				{
					cout <<"HNI demand: "<<demand<<"Eligible cells: "<<lcc2hni_vec.size()<<endl;
					/////////////////////////////////
					hnigrid[hni_index]=hcode;// Need to change the file provided by Zhiua because 255 does not read the system properly- it converted into -1. Therefore I put -1 just for testing purpose
					hnitempgridFlag[hni_index]=1;

				
					
					//spatail allocation
					//can use fill neighbour
					//find lag
					//or similar concepts to apply
					//Need to add algorithm to break out of loop to change the HNI
					/////////////////////////////////////
					demand--;
					
					lcc2hni_vec.erase(lcc2hni_vec.begin()+rand_lccindex-1); 
					if(lcc2hni_vec.size()!=0)
					{
						lcc2hni_vec.begin();
					}
				
				}
			}
		}
	
	}
	
}

//Allocate cells from hni to LCC
void allocate_hni2lcc(int demperiod)
{
	int hnidemand;
	std::map<int,vector<lccCells> > hni2lcc_cells; // A big group of cells changing to lcc in map object
	std::vector<lccCells>hni2lcc_vec;       //subset of map object in vecor
	extract_hni2lcc(hni2lcc_cells);
	
	//Reading demands for cell going from hni to lcc- hni2lcc
	for(unsigned int drow=numlcc;drow<numProbsurface;drow++)
	{
		for (unsigned int dcol=0;dcol<numlcc;dcol++)
		{
			hnidemand=(int)demand_matrix[demperiod][drow][dcol]; // hni demand
			
			
			hni2lcc_vec=hni2lcc_cells[dcol];

			cout<<"Eligible cell#:"<<hni2lcc_vec.size()<<" From: hni"<<inlcccode[drow]<<" To:"<<inlcccode[dcol] <<" Psurface#: "<<dcol <<" Demand#:"<<hnidemand<<" Lag#" <<pts_distanceLag[dcol]<<" Psize#" << pts_pathSize[dcol]<<" Mpsize#"<<meanpatchSize<<" Std#" << pts_stdDeviation[dcol] <<endl;
			if((hnidemand>0) && (hni2lcc_vec.size()>0))
			{
				//Spatial allocation of hni2lcc
				space_allocation(hni2lcc_vec,inlcccode[dcol],dcol,hnidemand,pts_distanceLag[dcol],true);
				cout<<hnidemand<<endl;
			}
		}
	}


}