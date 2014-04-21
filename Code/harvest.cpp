// File:		harvest.cpp
// Author: 		Ashis Lamsal
// Last Update:	3/4/2013
// Decription:	Mainly contains the function related to harvesting the eligible land covers
//----------------------------------------------------------------------------
#include "lcc.h"
#include "lccres.h"
#include "vegetation.h"
#include "randnum.h"
#include "harvest.h"
#include <vector>
#include <string>
#include <iostream>
#include <random>
#include <algorithm>

using namespace std;

std::vector<lccCells> harvneighborVecObjList, harvneighbourVecCells;// vector to hold 8 neighborhing cells from all eligible cells temporaily

//Get harvest neighbouring cell
bool getHarvestNeighbour(int row, int col, int lagdistance, int harvest_patchSize,int trtclass ,bool iscompactNeighbour)
{
	int rwlag=-lagdistance;
	int cclag=-lagdistance;
	bool isNeighboor=false;
	int count_neighbour=0; // Neighbour member count
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
						if((int)harvestgrid[cindex]==(1+trtclass))
						{
							
							if((rlag!= 0)||(clag=!0))
							{/*
							
								if((iscompactNeighbour) && (harvest_patchSize>3))
								{
									count_neighbour++;
									if(count_neighbour>2) // Eligible cell should have more than 3 same neighborhoods 
									{
										isNeighboor=true;
										count_neighbour=0;
										return true;
									}
								}

								else if((iscompactNeighbour) && ((harvest_patchSize> 1) && (harvest_patchSize <=3)))
								{
									count_neighbour++;
									if(count_neighbour>1) // Eligible cell should have more than 2 same neighborhoods 
									{
										isNeighboor=true;
										count_neighbour=0;
										return true;
									}
								}

								else
								{
									isNeighboor=true;
									return true;
							
								}*/
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
			cout<<"Harvest lag out of bound:: Press ENTER to continue"<<endl;
			cin.get();
		}
	}
}
//Fill harvest neighbour based on lag distance
std::vector<lccCells> fillHarvestNeighborhood(std::vector<lccCells> vecobj, int irow, int icol,int &demand, int &patch_size, int distlag, int mpatchsize)
{
	std::vector<lccCells> neighborVecObj; // vector to hold 8 neighborhing cells temporaily
	int initdem=demand;
	int veclen=vecobj.size();
	unsigned int iteration_count1=0;
	unsigned int iteration_count2=0;
	unsigned int iteration_count3=0;
	bool iterationflag= true;

	int distanceLag=distlag;
	std::vector<int>harvLaggrid; // Container to store status of cell already existed in neighbourhood vector or not
	harvLaggrid.resize(size);		//Resize vetor to landscape size
	std::fill(harvLaggrid.begin(),harvLaggrid.end(),0); // Filling vector with 0s

	if((demand>0) && (veclen>0))
	{	
		int ilcccol2;
		int ilccrow2;
		int index1;
		int index2;

		lccCells tempCells;
		for (int dlag=0;dlag<distanceLag;dlag++)
		{
			for(int j=-1-dlag;j<=1+dlag;j++)
			{
				for (int k=-1-dlag;k<=1+dlag;k++)
				{
					if(j!=0||k!=0)
					{
						irow=irow+j;
						icol=icol+k;

						index1=irow*maxcol+icol;

						for(unsigned int i=0;i<veclen;i++)
						{
							ilcccol2=vecobj.at(i).lccCol;
							ilccrow2=vecobj.at(i).lccRow;	
							index2=ilccrow2*maxcol+ilcccol2;

							//cout<<"ilcccol2:"<<ilcccol2<<endl;
							//cout<<"ilccrow2:"<<ilccrow2<<endl;

							if((((icol>=0) && (icol<=maxcol)) && ((ilcccol2>=0) && (ilcccol2<=maxcol))) && (((irow>=0) && (irow<=maxrow)) && ((ilccrow2>=0) && (ilccrow2<=maxrow))))
							{
								if((demand>0) && (patch_size<mpatchsize))
								{
										if((index1==index2) && (harvtempgridFlag[index1]==0) && (harvLaggrid[index1]==0) &&(harvestgrid[index1]==0))  
										{ 
											tempCells.lccCol=ilcccol2;
											tempCells.lccRow=ilccrow2;
											neighborVecObj.push_back(tempCells);	// Store the changed neighbour cells
											harvLaggrid[index1]=1;

										}
									
								}
							}

						}							
					}						
				}
			}
		}
	}
	harvLaggrid.clear();
	// shuffle the list	
	/*
	std::random_device rd;
    std::mt19937 g(rd());
	shuffle(neighborVecObj.begin(),neighborVecObj.end(),g);	
	*/
	return(neighborVecObj);
}

//Allocate the forest harvest
void allocateHarvest(std::vector<lccCells> harvCells,  int harvDemand, int mharvpatchSize,short int treatmentunit, int treatmentclass)
{
	int rand_harvrow,rand_harvcol,rand_index,harv_index,neighcell_index;
	bool harvNeigh=false;
	int harv_patch;
	int harv_lag=3;		// harvesting  adjacent neighbours
	int harvneighDemand;
	int counter1=0; //Counts the demand allocaiton while loop (1st loop)
	int counter2=0;	//Counts the neighbour demand allocation while loop (2nd loop)
	
	cout<< "Harvest demand ::"<<harvDemand <<"\t Harvest eligible pixels: "<<harvCells.size()<< endl;

	if(harvCells.size()>=harvDemand)
	{
		 while((harvCells.size()>0) && (harvDemand>0))
		 {
			//Generare a random integer between 1 and cell length.
			rand_index=(unsigned int) rand_int(harvCells.size());
	
			//Retrive random index from vector
			rand_harvrow=harvCells.at(rand_index-1).lccRow;
			rand_harvcol=harvCells.at(rand_index-1).lccCol;
	
			// harvest index
			harv_index=rand_harvrow*maxcol + rand_harvcol;
			harv_patch=0;
			if(gentreatment(harv_index,treatmentunit,treatmentclass))
			{
				harvDemand--;
				harv_patch++;
				harvestgrid[harv_index]=treatmentclass+1;
				harvtempgridFlag[harv_index]=1;
				
				harvCells.erase(harvCells.begin()+rand_index-1);
				if(harvCells.size()!=0)
				{
					harvCells.begin(); 
					
					// shuffle the list	
					/*
					std::random_device rd;
					std::mt19937 g(rd());
					shuffle(harvCells.begin(),harvCells.end(),g);	
					*/
				}
				cout <<"Remaining to harvest 1: "<< harvDemand<<endl;
				harvNeigh=true;
				harvneighbourVecCells=fillHarvestNeighborhood(harvCells,rand_harvrow,rand_harvcol,harvDemand,harv_patch,harv_lag,mharvpatchSize);
				
				

			}			
		    
			
			while((harvNeigh) && (harvneighbourVecCells.size()>0) && (harv_patch<=mharvpatchSize) && (harvDemand>0))

			{						
				harvneighDemand=harvDemand;
				for (unsigned int j=0;j<harvneighbourVecCells.size();j++)
				{
					if((harvDemand>0) && (harv_patch <= mharvpatchSize))
					{
						int neighrow=harvneighbourVecCells.at(j).lccRow;
						int neighcol=harvneighbourVecCells.at(j).lccCol;

						neighcell_index=neighrow*maxcol + neighcol;

						if((getHarvestNeighbour(neighrow,neighcol,harv_lag,harv_patch,treatmentclass,true)) && (harvtempgridFlag[neighcell_index]==0))
						{
							if(gentreatment(neighcell_index,treatmentunit,treatmentclass))
							{
								harvDemand--;
								harv_patch++;
								harvestgrid[neighcell_index]=treatmentclass+1;
								harvtempgridFlag[neighcell_index]=1;
								
								counter2=0;
								
								harvneighbourVecCells.erase(harvneighbourVecCells.begin()+j);
								if(harvneighbourVecCells.size()!=0)
								{
									harvneighbourVecCells.begin();

								}

								cout<< "Harvest demand ::"<<harvDemand <<"\t Harvest eligible Neigh pixels: "<<harvneighbourVecCells.size()<< endl;
								cout <<"Remaining to harvest 2: "<< harvDemand<<endl;
														
								harvneighborVecObjList=fillHarvestNeighborhood(harvCells,neighrow,neighcol,harvDemand,harv_patch,harv_lag,mharvpatchSize); //Works under patch lag neighbourhood
								
								harvneighbourVecCells.insert(harvneighbourVecCells.end(),harvneighborVecObjList.begin(),harvneighborVecObjList.end());
														
							}
						 }
					}
				} // end for loop
				
				if(harvneighDemand==harvDemand && counter2>NO_OF_ITERATION*(harvneighbourVecCells.size()))
				{

					harvNeigh=false;
					break;

				}
				counter2++;

			}// end of 2nd while
		 
		}//end 1st while

	}  //end if
	else
	{
		cout<< "Harvest demand ::"<<harvDemand <<"\t Harvest eligible cells: "<<harvCells.size()<< endl;
		cout<< " Please make sure demands must me smaller than eligible cells"<<endl;
		
		
	}
	


}