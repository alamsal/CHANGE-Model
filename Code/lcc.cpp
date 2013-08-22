//----------------------------------------------------------------------------
// File:		lcc.cpp
// Author: 		Ashis Lamsal
// Last Update:	12/18/2012
// Decription:	The lcc.cpp module merge man made and non vegetated areas with buffer and prevent them from fire simulation.
// It also generates explicitly coded LCC output for landscape fire simulation snapshot results based on LCC classes provided in lcc.gis file.
//----------------------------------------------------------------------------
//#include <boost/thread.hpp>
//#include <boost/array.hpp>			//Got this error after VS & Java udpate but not sure which one casue this one need to figure out for parallel threads.
//#include <boost/lambda/lambda.hpp>

#include <iostream>
#include <map>
#include <vector>
#include <cstdlib>
#include <string>
#include <algorithm>

#include "ladsio.h"

#include "lcc.h"
#include "randnum.h"
#include "celllist.h"
#include "probSurface.h"
#include "demand.h"



using namespace std;
std::vector<lccCells> neighborVecObj; // vector to hold all neighborhing cells temporaily

// Merge buffer cells and Non Vegetated areas (with LCC flag 0) with buffer to prevent them from burning during LADS simulation.
void merg_lccBuffer()
{
	/*
	for(int index=0;index<size;index++)
	{
		//printf("%d",buffer[index]);
		//printf("%d",lccgrid[index]);
		if(buffergrid[index]>0 && lcc[index]>0)
		{
			if(buffergrid[index]==2 ||(lcc[index]>=11 && lcc[index]<41)||(lcc[index]>=81 && lcc[index]<=95)) //Extract man made & non vegetated areas areas only(Exclude- natural vegetated areas)
			{
				buffergrid[index]=2;
			}
			
		}
		else
		{
			buffergrid[index]=0;
		}
	
	} */
	int j=0;
	int lclass;
	for (int index=0;index<size;index++)
	{
		if(((int)buffer[index]>=0) && ((int)lccgrid[index]>0))
		{
			for(lclass=0;lclass<numlcc;lclass++) //loop to determine the lcc class and their corresponding lcc flag and input code 
			{
				if(((int)buffer[index]==0 )||(((int)lccgrid[index]==(int)inlcccode[lclass]) && ((int)lcc_flag[lclass]==0))) //Extract man made & non vegetated areas areas only(Exclude- natural vegetated areas) and treat them as background (buffer=0)
					{
						buffer[index]=0;
						j++;
						break;
						
					}
			}
			
		}
		// Incase of LCC<0
		else
		{
			buffer[index]=0;
		}
	}
	cout <<"Merge buffer with lcc grid" <<endl;
}

/****************************************************************************
//Comparing 8 neighbouring lcc cells to initialize the LCC patch seed
Function: 	getNeighbour
Parameters: cell row, cell column
Returns:	bool
****************************************************************************/
bool getNeighbour(int row,int col,int lcccode)
{
	if(((row<maxrow) && (row>0)) && ((col<maxcol)&& (col>0)))
	{
		try{
			if(((int)lccgrid[((row-1)*maxcol+(col-1))]==lcccode)||((int)lccgrid[((row-1)*maxcol+(col))]==lcccode)||((int)lccgrid[((row-1)*maxcol+(col+1))]==lcccode)||((int)lccgrid[((row)*maxcol+(col-1))]==lcccode)
				||((int)lccgrid[((row)*maxcol+(col+1))]==lcccode)||((int)lccgrid[((row+1)*maxcol+(col-1))]==lcccode)||((int)lccgrid[((row+1)*maxcol+(col))]==lcccode)||((int)lccgrid[((row+1)*maxcol+(col+1))]==lcccode)) 
			{
				return true;
			}
			else
			{
				return false;
			}
		}
		catch(exception e) 
		{
			cout<<e.what();	
			cout<< row << col<< lcccode;
		}
	}
	
}

/****************************************************************************
//Allocation of cells in eight neighbourhoood locations recursively
Function: 	getEightNeighborhood
Parameters: Extracted LCLU cell vectors to change, cell row, cell column,lcccode to be assigned after change, probability surface index in the probaility surfaces vector, total # of demand

Returns:	
****************************************************************************/
void getEightNeighborhood(std::vector<lccCells> vecobj, int irow, int icol,int lcccode,int prob_index,int &demand, int &patch_size)
{
	int initdem=demand;
	int veclen=vecobj.size();

	if((demand>0) && (veclen>0))
	{	
		int ilcccol2;
		int ilccrow2;
		int index1;
		int index2;
		double trans_probaility;
		double irand;
		
		lccCells tempCells;

		for(int j=-1;j<=1;j++)
			{
				for (int k=-1;k<=1;k++)
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
								if(demand>0)
								{
									if((index1==index2) && (lccgrid[index1]!=lcccode) && (tempgridFlag[index1]==0) )
									{
										//Get the transitional probability value;
										trans_probaility=(double)(probability_surfaces[prob_index][ilccrow2][ilcccol2]);
										//Generate a uniform random variable;
										irand=u0_1();
										if(irand<trans_probaility)
										{
											int index_value= (int)(lccgrid[index1]);
											//If FORSCE change the cell transition form non-veg to vegetated; we must assign succesational stage [index]==1 to start future successional stages.
											if(((index_value==11)||(index_value==12)||(index_value==20)||(index_value==30)||(index_value==81)||(index_value==82)||(index_value==41)||(index_value==42)||(index_value==43)||(index_value==52)||(index_value==71))&&((lcccode==41)||(lcccode==42)||(lcccode==52)||(lcccode==71)))	
											{												//Put the smallest value from state,initage,and tsfire grids
												stategrid[index1]=1; // When forsce simulate veg to veg/ non-veg to veg the successional stage= 1;timeinstage =0; age=0; time since fire=0. 
																		// When forsce simulate non-veg to non-veg * veg to non-veg the buffer is  set to 0, which will handle by "merg_lccBuffer()" after the completion this demand allocation look at lads.cpp.
												timeinstage[index1]=1;	// time in current successional stage grid
												age[index1]=5;		    // patch age grid
												lccgrid[index1]=lcccode;
												tsfire[index1]=1;      // time since last fire grid
												buffer[index1]=1;		//Active simulation
												tempgridFlag[index1]=1;		// To prevent from further trasition of the same cell
												
											}
											else
											{
												lccgrid[index1]=lcccode; 
												buffer[index1]=250;			//Prevent from simulation human dominated cell
												tempgridFlag[index1]=1;		// To prevent from further trasition of the same cell
											}
										
										demand--;
										patch_size++;

										tempCells.lccCol=ilcccol2;
										tempCells.lccRow=ilccrow2;
										neighborVecObj.push_back(tempCells);	// Store the changed neighbour cells
										
										}
							
									}
								}
								else
								{
									return;
								}								
							}							
						}						
					}
				}
				//cout<<"demand:\t"<<demand<<endl;
			}
		if(((initdem==demand) && (demand>0) && (neighborVecObj.size()<1))|| (patch_size>=meanpatchSize)) 
		{
			space_allocation( vecobj, lcccode,  prob_index,  demand); // Assign new seed to change

		}
		else //Recursive algorithm to visit all the neighbouring cells
		{
			if((demand>0) && (neighborVecObj.size()>0))
			{
				for( unsigned int i=0;i<neighborVecObj.size();i++)
				{
					int neighrow=neighborVecObj.at(i).lccRow;
					int neighcol=neighborVecObj.at(i).lccCol;
					
					neighborVecObj.erase(neighborVecObj.begin()+i);
					neighborVecObj.begin();			
					
					getEightNeighborhood(vecobj,neighrow,neighcol,lcccode,prob_index,demand,patch_size);


				}
			}
		}
		
	}
}

/****************************************************************************
// Create and return the map of structure vectors to store all extracted values based upon probability surfaces,LCC, and breakpoints.
Function: 	extract_LandCoverCells
Parameters: lclu grid, lclu class code 
Returns:vector of raster cells that have potiential  to change from one class to another
****************************************************************************/
std::map<int,vector<lccCells> > extract_LandCoverCells(int lccCode)
{
	unsigned int index; // array index
	unsigned int node_counter=0; //Counter to track the lenght of forest list.

	lccCells tempLcc; // Struct object to store row and columns of a raster cell.

	std::map<int,vector<lccCells> > ext_lcc_vector;	// Map of vectors containing sturcuture to hold all extracted values based upon probability surfaces and LCC.
	ext_lcc_vector.clear();

	unsigned int count=0; //Cell counter for each class of LCC.
	
	// Read LCC grid and extract forest cell into a separate linked list, named forest_list. 
	for(unsigned int row=0; row<maxrow; row++) 
	{
		for(unsigned int col=0; col<maxcol; col++) 
		{
			index=row*maxcol + col;			

			if(lccgrid[index]==lccCode) //Extract LCC class based upon lcc code 
			{
				/*
				if((probability_surfaces[0][row][col])>0.95)			//Extract cells having  water transition prob threshold >95% i.e 0.95 ( to water)
				{
					tempLcc.lccRow=row;
					tempLcc.lccCol=col;
					ext_lcc_vector[0].push_back(tempLcc);
				}
				if((probability_surfaces[1][row][col])>0.95)			//Extract cells having  ice transition prob threshold >95% i.e 0.95 ( to ice)
				{
					tempLcc.lccRow=row;
					tempLcc.lccCol=col;
					ext_lcc_vector[1].push_back(tempLcc);
				}
				if((probability_surfaces[2][row][col])>0.93)			//Extract cells having transition prob threshold>93% ie. 0.93 (to urban)
				{
					tempLcc.lccRow=row;
					tempLcc.lccCol=col;
					ext_lcc_vector[2].push_back(tempLcc);
				}
				if((probability_surfaces[3][row][col])>0.85)			//Extract cells having trasnition prob threshold>85 ie. 0.84 ( to barren)
				{
					tempLcc.lccRow=row;
					tempLcc.lccCol=col;
					ext_lcc_vector[3].push_back(tempLcc);
				}
				if((probability_surfaces[4][row][col])>0.85)			//to decdidous forest
				{
					tempLcc.lccRow=row;
					tempLcc.lccCol=col;
					ext_lcc_vector[4].push_back(tempLcc);
					
				}
				if((probability_surfaces[5][row][col])>0.85)			//to evergreen forest
				{
					tempLcc.lccRow=row;
					tempLcc.lccCol=col;
					ext_lcc_vector[5].push_back(tempLcc);
				}
				if((probability_surfaces[6][row][col])>0.85)			//to shrubland
				{
					tempLcc.lccRow=row;
					tempLcc.lccCol=col;
					ext_lcc_vector[6].push_back(tempLcc);
				}
				if((probability_surfaces[7][row][col])>0.85)			//to grassland
				{
					tempLcc.lccRow=row;
					tempLcc.lccCol=col;
					ext_lcc_vector[7].push_back(tempLcc);
				}
				if((probability_surfaces[8][row][col])>0.90)			//to hay/pasture
				{
					tempLcc.lccRow=row;
					tempLcc.lccCol=col;
					ext_lcc_vector[8].push_back(tempLcc);
				}
				if((probability_surfaces[9][row][col])>0.85)			//to crops
				{
					tempLcc.lccRow=row;
					tempLcc.lccCol=col;
					ext_lcc_vector[9].push_back(tempLcc);
				}
				*/
				for (unsigned int i=0;i<numProbsurface;i++)
				{
					if((probability_surfaces[i][row][col])>transitionThreshold[i])			//Extract cells having  water transition prob threshold >95% i.e 0.95 ( to water)
					{
						tempLcc.lccRow=row;
						tempLcc.lccCol=col;
						ext_lcc_vector[i].push_back(tempLcc);
					}

				}

				count++;
			}
			
        }
    }		
		
	return ext_lcc_vector;
}


//Make a list of cells that are going to change
void extract_changeCells(int demperiod)
{
	 allocate_lccCells(demperiod);
}


//Spatial allocation of Demands
void allocate_lccCells(int demperiod)
{
	
	// Extracted  cells from LCC
	std::map<int,vector<lccCells> > extracted_lcc[50];
	/*
	extracted_lcc[0]=extract_LandCoverCells(11); // Extract water cells from LCC
	extracted_lcc[1]=extract_LandCoverCells(12); // Extract ice/snow cells from LCC
	extracted_lcc[2]=extract_LandCoverCells(20); // Extract developed cells from LCC
	extracted_lcc[3]=extract_LandCoverCells(30); // Extract barren cells from LCC 
	extracted_lcc[4]=extract_LandCoverCells(41); // Extract deci forest cells from LCC 
	extracted_lcc[5]=extract_LandCoverCells(42); // Extract evergreen forest cells from LCC
	extracted_lcc[6]=extract_LandCoverCells(52); // Extract shrubland cells from LCC 
	extracted_lcc[7]=extract_LandCoverCells(71); // Extract grassland cells from LCC 
	extracted_lcc[8]=extract_LandCoverCells(81); // Extract hay/pasture cells from LCC
	extracted_lcc[9]=extract_LandCoverCells(82); // Extract cropland cells from LCC
	extracted_lcc[10]=extract_LandCoverCells(90); // Extract cropland cells from LCC
	extracted_lcc[11]=extract_LandCoverCells(95); // Extract cropland cells from LCC
	*/
	for (unsigned int i=0;i<numlcc;i++)
	{
		extracted_lcc[i]=extract_LandCoverCells(inlcccode[i]);
	}

	//int extractSize= 10; //Becasue we got 10 sets of lcc transformation
	//int lcccode[10]={11,12,20,30,41,42,52,71,81,82}; // Once we got all datasets, I think we can replace this array with inlcccode[40].
	
	std::map<int,vector<lccCells> > lcc_cells;
	std::vector<lccCells> vec_lcc_cells;

	//Read demand file in row/column order
		for(unsigned int i=0; i<12;i++)	// Total
		{
			for (unsigned int j=0; j<12; j++)
			{
				//cout<<"("<<i<<","<<j<<")"<<demand_matrix[i][j]<<"\t";
				if(i==j)		//No demand allocation diagonally 
				{
					cout<<"No demand allocation"<<endl;
				
				}
				else
				{
					//demand value comes from here..
					//int demand=(int)demand_matrix[i][j][dfile];	//Convert from string type to integer.
					int demand=(int)demand_matrix[demperiod][i][j];
					lcc_cells=extracted_lcc[i];
					
					vec_lcc_cells=lcc_cells[j];

					cout<<vec_lcc_cells.size()<<"--"<<inlcccode[j] <<"--"<<j <<"--"<<demand <<endl;
					space_allocation(vec_lcc_cells,inlcccode[j],j,demand);
				}

			}
			cout<<endl<<endl;
		}
	

	////********************************************************************///
	// NEED to update current LCC to extract following cells- ASK Mike & Zhihuwa//
	// Extract woody wetlands cells from LCC 
	std::map<int,vector<lccCells> > lcc_woody_wetlands;
	// Extract herbaceous wetlands cells from LCC
	std::map<int,vector<lccCells> > lcc_herbaceous_wetlands;
	// Extract mecanically distrubed/ non forest cells from LCC
	std::map<int,vector<lccCells> > lcc_md_nf;
	// Extract mecanically distrubed/open cells from LCC
	std::map<int,vector<lccCells> > lcc_md_op;
	// Extract mecanically distrubed/private cells from LCC 
	std::map<int,vector<lccCells> > lcc_md_pvt;
	// Extract mining cells from LCC 
	std::map<int,vector<lccCells> > lcc_minning;
	////********************************************************************///
}


/****************************************************************************
//Spatial allocation of cells in the 2D grid
Function: 	space_allocation
Parameters: Extracted LCLU cell vectors to change, lcccode to be assigned after change, probability surface index in the probaility surfaces vector, total # of demand

Returns:	
****************************************************************************/
void space_allocation( std::vector<lccCells> vecobj,int lcccode, int prob_index, int &demand)
{
	int rand_forestrow;
	int rand_forestcol;
	double tras_probability;
	unsigned int cell_index;
	double irand;
	int counter=0;
	int patch_size=0;		//Counter to hold patch size

	if(((demand>0) && (vecobj.size()>0)) && (demand<vecobj.size()))
	{
		//cout<< "Start Demand::"<<demand <<"\t Lcc Code::"<<lcccode<<"\t Transition prob pixel #::" <<vecobj.size() << endl;
		while((vecobj.size()>0) && (demand>0))
		{

			//Generare a random integer between 1 and cell length.
			unsigned int rand_index=(unsigned int) rand_int(vecobj.size());


			//Retrive random index from vector
			rand_forestrow=vecobj.at(rand_index-1).lccRow;
			rand_forestcol=vecobj.at(rand_index-1).lccCol;


			////Transition probability value
			tras_probability=(double)(probability_surfaces[prob_index][rand_forestrow][rand_forestcol]);		//(1/255*probability_surfaces[1][rand_forestrow][rand_forestcol])

			////Generate a uniform random variable for the forest cell
			irand=u0_1();
			if(irand<tras_probability)
			{
				cell_index=rand_forestrow*maxcol + rand_forestcol;
				if(((getNeighbour(rand_forestrow,rand_forestcol,lcccode)) && (lcccode != lccgrid[cell_index]) && (tempgridFlag[cell_index]==0) ))
				{
					int index_value=(int)(lccgrid[cell_index]);
					//cout<<index_value<<endl;   

					//If FORSCE change the cell transition form non-veg to vegetated; we must assign succesational stage [index]==1 to start future successional stages.
					if(((index_value==11)||(index_value==12)||(index_value==20)||(index_value==30)||(index_value==81)||(index_value==82)||(index_value==41)||(index_value==42)||(index_value==43)||(index_value==52)||(index_value==71))&&((lcccode==41)||(lcccode==42)||(lcccode==52)||(lcccode==71)))	
					{

						//Put the smallest value from state,initage,and tsfire grids
						stategrid[cell_index]=1; // When forsce simulate veg to veg/ non-veg to veg the successional stage= 1;timeinstage =0; age=0; time since fire=0. 
												// When forsce simulate non-veg to non-veg * veg to non-veg the buffer is  set to 0, which will handle by "merg_lccBuffer()" after the completion this demand allocation look at lads.cpp.
						timeinstage[cell_index]=1;	// time in current successional stage grid
						age[cell_index]=5;		    // patch age grid
						tsfire[cell_index]=1;      // time since last fire grid
						lccgrid[cell_index]=lcccode;
						buffer[cell_index]=1;		//Active simulation
						tempgridFlag[cell_index]=1;		// To prevent from further trasition of the same cell
						
					}
					else
					{
						lccgrid[cell_index]=lcccode; 
						buffer[cell_index]=250;			//prevent from simulation for human dominated
						tempgridFlag[cell_index]=1;		// To prevent from further trasition of the same cell
					}
					//cout<<int(lccgrid[cell_index])<<endl;
					demand--;
					counter++;
					patch_size++;

					
					getEightNeighborhood( vecobj, rand_forestrow, rand_forestcol, lcccode, prob_index, demand,patch_size);
				}
				
			}
			vecobj.erase(vecobj.begin()+rand_index-1);
			if(vecobj.size()!=0)
			{
				vecobj.begin(); 
			}
			
			
		}
		//cout<< "Rem. to accomp demand::"<<demand <<"\t Lcc Code::"<<lcccode<<"\t Rem. trans. prob pixel #::" <<vecobj.size() <<"counter" <<counter<< endl; 
	}
	else
	{
		cout<< "Demand & transition prob pixel size/#::"<<demand <<"::\t"<<vecobj.size()<< endl;
	}

	

}

// Assign output codes to  NDLC 2006 input LCC codes to generate output grid.
//Place to merge LADS & FORESCE outputs
void merg_lccSnapshot()
{
	/*
	for(int index=0;index<=size;index++)
	{
		// Need to loop  through the lcc file and figure out.....
		if(lccgrid[index]==11)
		{
			temp[index]=211;
		}
		else if(lccgrid[index]==12)
		{
			temp[index]=212;
		}
		else if(lccgrid[index]==20)
		{
			temp[index]=213;
		}
		else if(lccgrid[index]==22)
		{
			temp[index]=214;
		}
		else if(lccgrid[index]==23)
		{
			temp[index]=215;
		}
		else if(lccgrid[index]==24)
		{
			temp[index]=216;
		}
		else if(lccgrid[index]==31)
		{
			temp[index]=217;
		}
		else if(lccgrid[index]==81)
		{
			temp[index]=227;
		}
		else if(lccgrid[index]==82)
		{
			temp[index]=228;
		}
		else if(lccgrid[index]==90)
		{
			temp[index]=229;
		}
		else if(lccgrid[index]==95)
		{
			temp[index]=230;
		}
		else
		{
			temp[index]=temp[index];
		}
		
	}
	*/
	
	for(int index=0;index<=size;index++)
	{
		for(int lclass=0;lclass<numlcc;lclass++)
		{
			if((lccgrid[index]==inlcccode[lclass]) && (lcc_flag[lclass]==0))
			{
				temp[index]=outlcccode[lclass];			
			}
		}
		
	}
	
}

//Reclassify into broad landcover class
void reclassify_HumanDominated()
{
	for(unsigned int index=0;index<=size;index++)
	{
		for(unsigned int j=0;j<numlcc;j++)
		{
			if(((temp[index])==(outlcccode[j])) && (lcc_flag[j]==0))
			{
				temp[index]=inlcccode[j];
				//cout<< temp[index];
			}
		}
		
	}

}
//Reclassify into broad landcover class
void reclassify_NatureDominated(unsigned int stateout[],unsigned int lclustate[],unsigned int statecounter)
{
	for(unsigned int i=0;i<=statecounter;i++)
	{
	cout<<stateout[i] <<"****" <<lclustate[i]<<endl;
	}
	
	for(unsigned int index=0; index<=size;index++)
	{
		for(unsigned int i=0;i<=statecounter;i++)
		{
			if((int)temp[index]==(int)stateout[i])
			{
				temp[index]=lclustate[i];
				break;
			}
		}
	}
}
//Reclassifying the LADS/FORESCE output into initial LCLU classes.
void reclassify_lclu(unsigned int stateout[],unsigned int lclustate[],unsigned int statecounter)
{
	reclassify_NatureDominated(stateout,lclustate,statecounter);
	reclassify_HumanDominated();

	//Assign temp lcc grid as new land cover land use grid (new lccgrid) for subsequent processing	
	strcpy(lccgrid,reinterpret_cast<char*>(temp)); //convert from 'unsigned char *' to 'char *'
	

	
}

