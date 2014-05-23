//----------------------------------------------------------------------------
// File:		hni.cpp
// Author: 		Aashis Lamsal
// Last Update:	11/14/2013
// Decription:	Contains functions to simulate hni 
//----------------------------------------------------------------------------

#include "hni.h"
#include "lcc.h"
#include "lccres.h"
#include "randnum.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <string>

# define  hnicode 2

using namespace std;

std::vector<lccCells> hni_neighborVecObjList, hni_neighbourVecCells;// vector to hold 8 neighborhing cells from all eligible cells temporaily
//ext_hni2lcc_vector - Map of vectors containing sturcuture to hold all extracted values based upon probability surfaces, LCC, and ownership.

//Extract and allocate eligible cells from hni to LCC
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
			//Extracting hni cells only;
			//hni value:2  and extracting evergreen and decidious cells
			int hni2Lcc=lccgrid[hindex];
			if((int)hnigrid[hindex]==2 && ((hni2Lcc==42)||(hni2Lcc==41)))
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

//Allocate cells from hni to developed
void allocate_hni2lcc(int demperiod)
{
	int hnidemand;
	std::map<int,vector<lccCells> > hni2lcc_cells; // A big group of cells changing to lcc in map object
	std::vector<lccCells>hni2lcc_vec,eligible_hni2lcc_vec;       //subset of map object in vecor
	extract_hni2lcc(hni2lcc_cells);
	
	//Reading demands for cell going from hni to developed - hni2lcc
	for(unsigned int drow=numlcc;drow<numProbsurface;drow++)
	{
		for (unsigned int dcol=0;dcol<numlcc;dcol++)
		{
			hnidemand=(int)demand_matrix[demperiod][drow][dcol]; // hni demand
			
			
			hni2lcc_vec=hni2lcc_cells[dcol];
			//filter eligible cells
			getEligibleCellsAfterOwnership(hni2lcc_vec,eligible_hni2lcc_vec,inlcccode[dcol]);

			
			cout<<"Eligible cell before ownership :"<<hni2lcc_vec.size()<<" From HNI: "<<inlcccode[drow]<<" To: "<<inlcccode[dcol] <<" Psurface#: "<<dcol <<" Demand#: "<<hnidemand<<" Lag#" <<pts_distanceLag[dcol]<<" Psize#" << pts_patchSize[dcol]<<" Mpsize#"<<meanpatchSize<<" Std#" << pts_stdDeviation[dcol] <<endl;
			cout<<"Eligible cell after ownership :"<<eligible_hni2lcc_vec.size()<<" From HNI: "<<inlcccode[drow]<<" To: "<<inlcccode[dcol] <<" Psurface#: "<<dcol <<" Demand#: "<<hnidemand<<" Lag#" <<pts_distanceLag[dcol]<<" Psize#" << pts_patchSize[dcol]<<" Mpsize#"<<meanpatchSize<<" Std#" << pts_stdDeviation[dcol] <<endl;
			
			writelog<<"Eligible cell before ownership :"<<hni2lcc_vec.size()<<" From HNI: "<<inlcccode[drow]<<" To: "<<inlcccode[dcol] <<" Psurface#: "<<dcol <<" Demand#: "<<hnidemand<<" Lag#" <<pts_distanceLag[dcol]<<" Psize#" << pts_patchSize[dcol]<<" Mpsize#"<<meanpatchSize<<" Std#" << pts_stdDeviation[dcol] <<endl;
			writelog<<"Eligible cell after ownership :"<<eligible_hni2lcc_vec.size()<<" From HNI: "<<inlcccode[drow]<<" To: "<<inlcccode[dcol] <<" Psurface#: "<<dcol <<" Demand#: "<<hnidemand<<" Lag#" <<pts_distanceLag[dcol]<<" Psize#" << pts_patchSize[dcol]<<" Mpsize#"<<meanpatchSize<<" Std#" << pts_stdDeviation[dcol] <<endl;

			if((hnidemand>0) && (eligible_hni2lcc_vec.size()>0))
			{
				//Spatial allocation of hni2lcc
				space_allocation(eligible_hni2lcc_vec,inlcccode[dcol],dcol,hnidemand,pts_distanceLag[dcol],pts_patchLag[dcol],true);
				cout<<hnidemand<<endl;
			}

			//clean up the vectors	
			hni2lcc_vec.clear();
			eligible_hni2lcc_vec.clear();
		}

	}


}

//Determine HNI neighbour lag
bool getHnilag(int row,int col,int hcode,int lagdistance,int hni_patchSize, bool iscompactNeighbour)
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
						if((int)hnigrid[cindex]==hcode)
						{
							if((rlag!= 0)||(clag=!0))
							{
								/*
								if((iscompactNeighbour) && (hni_patchSize>2))
								{
									count_neighbour++;
									if(count_neighbour>1) // Eligible cell should have more than 2 same neighbourhoods
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
							
								}
								*/
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
	else
	{
		false;
	}
}

//Find hni neighbourhood distance
std::vector<lccCells> hni_fillNeighborhood(std::vector<lccCells> vecobj, int inrow, int incol,int prob_index,int &demand, int &patch_size, int dlag)
{
	std::vector<lccCells> neighborVecObj,tempneighVecObj; // vector to hold 8 neighborhing cells temporaily
	neighborVecObj.clear();
	tempneighVecObj.clear();
	int initdem=demand;
	int veclen=vecobj.size();
	bool iterationflag= true;
	unsigned int irow,icol;
	int distanceLag=dlag;
	std::vector<int>tempLaggrid; // Container to store status of cell already existed in neighbourhood vector or not
	tempLaggrid.resize(size);		//Resize vetor to landscape size
	std::fill(tempLaggrid.begin(),tempLaggrid.end(),0); // Filling vector with 0s

	if((demand>0) && (veclen>0))
	{	
		int ilcccol2;
		int ilccrow2;
		int index1;
		int index2;

		lccCells tempCells;
		
			for(int j=inrow-dlag;j<=inrow+dlag;j++)
			{
				for (int k=incol-dlag;k<=incol+dlag;k++)
				{
					if(j!=inrow||k!=incol)
					{
						irow=j;
						icol=k;

						index1=irow*maxcol+icol;

						for(unsigned int i=0;i<veclen;i++)
						{
							ilcccol2=vecobj.at(i).lccCol;
							ilccrow2=vecobj.at(i).lccRow;	
							index2=ilccrow2*maxcol+ilcccol2;

							if((((icol>=0) && (icol<=maxcol)) && ((ilcccol2>=0) && (ilcccol2<=maxcol))) && (((irow>=0) && (irow<=maxrow)) && ((ilccrow2>=0) && (ilccrow2<=maxrow))))
							{
								if(demand>0 && patch_size<meanpatchSize)
								{
										if((index1==index2) && (tempLaggrid[index1]==0) && (hnitempgridFlag[index1]==0) && hnigrid[index1]==1)  
											 
										{ 
											tempCells.lccCol=ilcccol2;
											tempCells.lccRow=ilccrow2;
											tempneighVecObj.push_back(tempCells);	// Store the changed neighbour cells

											//neighborVecObj.push_back(tempCells);	// Store the changed neighbour cells
											
											tempLaggrid[index1]=1;
										
										}
								}
							}

						}							
					}						
				}
			}
			//Randomize the order of neighbouring cells
			if(tempneighVecObj.size()!=0)
			{
				vector<int> randValue=RandFillArr(tempneighVecObj.size());
			
				int tempval;
				for(unsigned int i=0;i<tempneighVecObj.size();i++)
				{
					tempval=randValue[i]; 
					tempval=tempval-1;
					neighborVecObj.push_back(tempneighVecObj[tempval]);	

				}

			}




		
	}
	tempLaggrid.clear();
	tempneighVecObj.clear();
	return(neighborVecObj);
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
	std::vector<lccCells> vec_lcc2hni_cells, eligible_vec_lcc2hni_cells;
	
	//Read eligible lcc2hni cells
	for(unsigned int row=0; row<numlcc;row++)	
	{
		lcc2hni_cells=extracted_lcc2hni[row];
		//extracting eligible lcc2hnicells based upon probability
		vec_lcc2hni_cells=lcc2hni_cells[numlcc];

		//filter lcc2hnni eligible cells based upon ownership restriction
		cout<<"Eligible cells before ownership restriction, LCC2HNI: "<< vec_lcc2hni_cells.size() <<endl;
		getEligibleCellsAfterOwnership(vec_lcc2hni_cells,eligible_vec_lcc2hni_cells,999);

		
		cout<<"Eligible cells after ownership restriction, LCC2HNI: " << eligible_vec_lcc2hni_cells.size()<<endl;

		writelog<<"Eligible cells before ownership restriction, LCC2HNI: "<< vec_lcc2hni_cells.size() <<endl;
		writelog<<"Eligible cells after ownership restriction, LCC2HNI: " << eligible_vec_lcc2hni_cells.size()<<endl;

		//selecting only natrual vegetated cell goes to hni [ Becareful while selecting the hni demands the program doesn't have any restriction to Nat veg convert into hni if are there sufficient eligible cells and demands. No matter wetland, herb, shurb, evergreen or deci can goes to hni]
		if((eligible_vec_lcc2hni_cells.size()>0) && (lcc_flag[row]==1))
		{
			for(unsigned int col=numlcc;col<numProbsurface;col++)
			{
				hnidemand=(int)demand_matrix[demperiod][row][col];
				cout<<"Demand LCC2HNI:"<<hnidemand<<endl;
				if(hnidemand>0)
				{
					allocate_lcc2hni(eligible_vec_lcc2hni_cells,col,hnidemand,hnicode,pts_distanceLag[numlcc],pts_patchLag[numlcc]);
					eligible_vec_lcc2hni_cells.clear();
					break;
				}
			}
		}

		//clean up the vectors
		vec_lcc2hni_cells.clear();
		eligible_vec_lcc2hni_cells.clear();
			
	}

}

//Allocate cells from LCC to hni
void allocate_lcc2hni(std::vector <lccCells>lcc2hni_vec,int prob_index, int &demand,int hcode,int hni_lag, int hni_plag)
{
	int rand_lccrow;
	int rand_lcccol;
	unsigned int rand_lccindex,hni_index;
	double trans_prob;
	double irand;
	bool islag_distance;
	int hni_patch; // Hni patch size
	int hni_meanpatchsize;
	bool hni_fillNeighbour=false; //flag to enable to neighbouring pixel to fill
	bool afterIteraiton =false; //flag to enable setting pixel change on not adjacent neighbourig cells after preset total no# iteration accomplished . To prevent from infinite loop during demand allocation 
	int hni_neighdemand=0;
	double hni_tras_probability;
	int counter1=0,counter2=0;
	int olddemand=0;
	int patchcol=prob_index; // column value to generate patch from index


	if(((demand>0) && (lcc2hni_vec.size()>0)) &&(demand<=lcc2hni_vec.size()))
	{
		while((lcc2hni_vec.size()>0) && (demand>0))
		{
			hni_meanpatchsize=genLCCPatchSize(pts_patchSize[patchcol],pts_stdDeviation[patchcol]);
			olddemand=demand;
			hni_patch=0;
			//Generate a random integer between 1 and eligible cell size
			rand_lccindex=(unsigned int)rand_int(lcc2hni_vec.size());
			//Retrive random index from vector
			rand_lccrow=lcc2hni_vec.at(rand_lccindex-1).lccRow;
			rand_lcccol=lcc2hni_vec.at(rand_lccindex-1).lccCol;			
			//Transition probability value
			trans_prob=(double)	probability_surfaces[prob_index][rand_lccrow][rand_lcccol];
			//Generate a uniform random variable for the forest cell
			irand=u0_1();
			if(irand<trans_prob)
			{
				hni_index= rand_lccrow*maxcol+rand_lcccol;
				// No lag constrain for seed placement
				if((afterIteraiton) || (hni_lag==-9))
				{
					if(hnitempgridFlag[hni_index]==0)  // we can put natural vegetation class as well for filtering
					{
						cout <<"HNI demand: "<<demand<<"Eligible cells: "<<lcc2hni_vec.size()<<endl;
					
						hnigrid[hni_index]=hcode;// 0 background, 1 non hni, 2 hni
						hnitempgridFlag[hni_index]=1;
					
						hni_patch++;
						demand--;
					
						lcc2hni_vec.erase(lcc2hni_vec.begin()+rand_lccindex-1); 
						if(lcc2hni_vec.size()!=0)
						{
							lcc2hni_vec.begin();
						}
						hni_fillNeighbour=true;
						hni_neighbourVecCells=hni_fillNeighborhood( lcc2hni_vec, rand_lccrow, rand_lcccol,prob_index, demand,hni_patch,hni_plag);// Works under hni patch size lag
					}

				}

				// lag constraion for seed palcement
				else
				{
					islag_distance=getHnilag(rand_lccrow,rand_lcccol,hcode,hni_lag, hni_patch,false);
					if((islag_distance) &&( hnitempgridFlag[hni_index]==0))
					{
						cout <<"HNI demand: "<<demand<<"Eligible cells: "<<lcc2hni_vec.size()<<endl;
					
						hnigrid[hni_index]=hcode;// 0 background, 1 non hni, 2 hni
						hnitempgridFlag[hni_index]=1;
					
						hni_patch++;
						demand--;
					
						lcc2hni_vec.erase(lcc2hni_vec.begin()+rand_lccindex-1); 
						if(lcc2hni_vec.size()!=0)
						{
							lcc2hni_vec.begin();
						}
						hni_fillNeighbour=true;
						hni_neighbourVecCells=hni_fillNeighborhood( lcc2hni_vec, rand_lccrow, rand_lcccol,prob_index, demand,hni_patch,hni_plag);// Works under hni distance lag
					
					}
				}


			}
			// Program enter here after seed selection
			while((hni_neighbourVecCells.size()>0) && (demand>0) && (hni_patch<hni_meanpatchsize) && (hni_fillNeighbour))
			{
				hni_neighdemand=demand;

				for( unsigned int i=0;i<hni_neighbourVecCells.size();i++)
				{ 
					if((demand>0) && (hni_patch < hni_meanpatchsize))
					{
						int neighrow=hni_neighbourVecCells.at(i).lccRow;
						int neighcol=hni_neighbourVecCells.at(i).lccCol;


						////Transition probability value
						hni_tras_probability=(double)(probability_surfaces[prob_index][neighrow][neighcol]);		//(1/255*probability_surfaces[1][rand_forestrow][rand_forestcol])

						////Generate a uniform random variable for the forest cell
						irand=u0_1();
						if(irand<hni_tras_probability)
						{
							hni_index=neighrow*maxcol + neighcol;

							if((getHnilag(neighrow,neighcol,hcode,hni_plag,hni_patch,true)) && (hnitempgridFlag[hni_index]==0))  //Enforce patch lag while cell transtion
							{

								hnigrid[hni_index]=hcode;// Need to change the file provided by Zhiua because 255 does not read the system properly- it converted into -1. Therefore I put -1 just for testing purpose
								hnitempgridFlag[hni_index]=1;
								demand--;
								hni_patch++;
								counter2=0;
								cout<< "Rem demand: "<<demand<<"\t Eligible cells in NeigbourHood List: "<< hni_neighbourVecCells.size()<<endl;
								writelog<< "Rem demand: "<<demand<<"\t Eligible cells in NeigbourHood List: "<< hni_neighbourVecCells.size()<<endl;
								hni_neighbourVecCells.erase(hni_neighbourVecCells.begin()+i);
								if(hni_neighbourVecCells.size()!=0)
								{
									hni_neighbourVecCells.begin(); 
								}

								hni_neighborVecObjList=hni_fillNeighborhood(lcc2hni_vec,neighrow,neighcol,prob_index,demand,hni_patch,hni_plag); //Works under patch lag neighbourhood
								hni_neighbourVecCells.insert(hni_neighbourVecCells.end(),hni_neighborVecObjList.begin(),hni_neighborVecObjList.end());

								cout<< "Code:2-Rem. to accomp demand::"<<demand <<"\t Rem. trans. prob pixel #::" <<lcc2hni_vec.size() <<"counter" <<counter2<< endl; 
								writelog<< "Code:2-Rem. to accomp demand::"<<demand <<"\t Rem. trans. prob pixel #::" <<lcc2hni_vec.size() <<"counter" <<counter2<< endl; 
										

							}

						}
					}

				} //end for loop

				if(hni_neighdemand==demand && counter2>NO_OF_ITERATION*(hni_neighbourVecCells.size()))
				{

					hni_fillNeighbour=false;
					break;

				}
				counter2++;
			} // end 2nd while

			if(olddemand==demand && counter1>NO_OF_ITERATION*(lcc2hni_vec.size()))
			{

				afterIteraiton=true;
				counter1=0;
			}
			counter1++;

		} // end 1st while
	
	}// end if
	else
	{
		
		cout<< "Demand ::"<<demand <<"\t HNI eligible cells ::"<<lcc2hni_vec.size()<< endl;
		cout<< " Please make sure demands must me smaller than eligible cells"<<endl;
	}
	
}





