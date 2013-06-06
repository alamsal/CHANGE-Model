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

#include "lcc.h"
#include "randnum.h"
#include "celllist.h"
#include "probSurface.h"
#include "demand.h"

using namespace std;
// Merge buffer cells and Non Vegetated areas (with LCC flag 0) with buffer to prevent them from burning during LADS simulation.
void merg_lccBuffer(char *buffergrid,char *lcc)
{
	//for(int index=0;index<size;index++)
	//{
	//	//printf("%d",buffer[index]);
	//	//printf("%d",lccgrid[index]);
	//	if(buffergrid[index]>0 && lcc[index]>0)
	//	{
	//		if(buffergrid[index]==2 ||(lcc[index]>=11 && lcc[index]<41)||(lcc[index]>=81 && lcc[index]<=95)) //Extract man made & non vegetated areas areas only(Exclude- natural vegetated areas)
	//		{
	//			buffergrid[index]=2;
	//		}
	//		
	//	}
	//	else
	//	{
	//		buffergrid[index]=0;
	//	}
	
	//}
	for (int index=0;index<size;index++)
	{
		if((buffergrid[index]>=0) && (lcc[index]>0))
		{
			for(int lclass=0;lclass<numlcc;lclass++) //loop to determine the lcc class and their corresponding lcc flag and input code 
			{
				if((buffergrid[index]==0 )||((lcc[index]==inlcccode[lclass]) && (lcc_flag[lclass]==0))) //Extract man made & non vegetated areas areas only(Exclude- natural vegetated areas) and treat them as background (buffer=0)
					{
						//std::cout<<(int)buffergrid[index]<<"\t"<<int(lcc[index])<<"\t"<<inlcccode[lclass]<<"\t"<<lcc_flag[lclass]<<endl;
						buffergrid[index]=0;
						break;
					}
			}
			
		}
		else
		{
			buffergrid[index]=0;
		}
	}
	cout <<"Merge buffer with lcc grid" <<endl;
}
//Comparing neighbouring lcc 8 cells to generate LCC patch
//Need to update with MIKE's neighbourhood patch generation code
bool getNeighbour(int row,int col,int lcccode)
{
	if(((row<maxrow) && (row>0)) && ((col< maxcol)&& (col>0)))
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
// Create and return the map of structure vectors to store all extracted values based upon probability surfaces,LCC, and breakpoints.
Function: 	extract_LandCoverCells
Parameters: lclu grid, lclu class code 
Returns:vector of raster cells that have potiential  to change from one class to another
****************************************************************************/
std::map<int,vector<lccCells> > extract_LandCoverCells(char *lcc, int lccCode)
{
	unsigned int index; // array index
	unsigned int node_counter=0; //Counter to track the lenght of forest list.

	lccCells tempLcc; // Struct object to store row and columns of a raster cell.

	std::map<int,vector<lccCells> > ext_lcc_vector;	// Map of vectors containing sturcuture to hold all extracted values based upon probability surfaces and LCC.
	ext_lcc_vector.clear();

	int count=0; //Cell counter for each class of LCC.
	
	// Read LCC grid and extract forest cell into a separate linked list, named forest_list. 
	for(unsigned int row=0; row<maxrow; row++) 
	{
		for(unsigned int col=0; col<maxcol; col++) 
		{
			index=row*maxcol + col;			

			if(lcc[index]==lccCode) //Extract LCC class based upon lcc code 
			{
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
				count++;
			}
			
        }
    }		
		
	return ext_lcc_vector;
}


//Make a list of cells that are going to change
void extract_changeCells(char *lcc)
{
	 allocate_lccCells(lcc);
}


//Spatial allocation of Demands
void allocate_lccCells(char *lcc)
{
	
	// Extracted  cells from LCC
	std::map<int,vector<lccCells> > extracted_lcc[20];
	
	extracted_lcc[0]=extract_LandCoverCells(lcc,11); // Extract water cells from LCC
	extracted_lcc[1]=extract_LandCoverCells(lcc,12); // Extract ice/snow cells from LCC
	extracted_lcc[2]=extract_LandCoverCells(lcc,20); // Extract developed cells from LCC
	extracted_lcc[3]=extract_LandCoverCells(lcc,31); // Extract barren cells from LCC 
	extracted_lcc[4]=extract_LandCoverCells(lcc,41); // Extract deci forest cells from LCC 
	extracted_lcc[5]=extract_LandCoverCells(lcc,42); // Extract evergreen forest cells from LCC
	//extracted_lcc[6]=extract_LandCoverCells(lcc,43); // Extract mixed forest cells from LCC  /*Excluding mixed forest from Forsce MODEL becasue we do not have Probability surface for this*/
	extracted_lcc[6]=extract_LandCoverCells(lcc,52); // Extract shrubland cells from LCC 
	extracted_lcc[7]=extract_LandCoverCells(lcc,71); // Extract grassland cells from LCC 
	extracted_lcc[8]=extract_LandCoverCells(lcc,81); // Extract hay/pasture cells from LCC
	extracted_lcc[9]=extract_LandCoverCells(lcc,82); // Extract cropland cells from LCC
	//extracted_lcc[10]=extract_LandCoverCells(lcc,90); //Extract wetland cells from LCC     /*Excluding wetlands from Forsce MODEL becasue we do not have Probability surface for this*/
	
	//int extractSize= 10; //Becasue we got 10 sets of lcc transformation
	int lcccode[10]={11,12,20,31,41,42,52,71,81,82}; // Once we got all datasets, I think we can replace this array with inlcccode[40].
	
	std::map<int,vector<lccCells> > lcc_cells;
	std::vector<lccCells> vec_lcc_cells;

	//Read demand file in row/column order
	for(int i=0; i<DEMANDROW;i++)
	{
		for (int j=0; j<DEMANDCOL; j++)
		{
			//cout<<"("<<i<<","<<j<<")"<<demand_matrix[i][j]<<"\t";
			if(i==j)		//No demand allocation diagonally 
			{
				cout<<"No demand allocation"<<endl;
				
			}
			else
			{
				//demand value comes from here..
				int demand=stoi(demand_matrix[i][j]);	//Convert from string type to integer.
				lcc_cells=extracted_lcc[i];
				vec_lcc_cells=lcc_cells[j];
				cout<<vec_lcc_cells.size()<<"--"<<lcccode[j] <<"--"<<j <<"--"<<demand <<endl;
				space_allocation(vec_lcc_cells,lcccode[j],j,demand);
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
void space_allocation( std::vector<lccCells> vecobj,int lcccode, int prob_index, int demand)
{
	int rand_forestrow;
	int rand_forestcol;
	double tras_probability;
	unsigned int cell_index;
	double irand;
	int counter=0;
	if((demand>0)&&(vecobj.size()>0))
	{
		cout<< "Start Demand::"<<demand <<"\t Lcc Code::"<<lcccode<<"\t Transition prob pixel #::" <<vecobj.size() << endl;
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
				if(getNeighbour(rand_forestrow,rand_forestcol,lcccode))
				{
					int index_value=(int)(lccgrid[cell_index]);
					//cout<<index_value<<endl;   

					//If FORSCE change the cell transition form non-veg to vegetated; we must assign succesational stage [index]==1 to start future successional stages.
					if(((index_value==11)||(index_value==12)||(index_value==20)||(index_value==31)||(index_value==81)||(index_value==82)||(index_value==90)||(index_value==41)||(index_value==42)||(index_value==43)||(index_value==52)||(index_value==71))&&((lcccode==41)||(lcccode==42)||(lcccode==43)||(lcccode==52)||(lcccode==71)))	
					{
						stategrid[cell_index]=1; // When forsce simulate veg to veg/ non-veg to veg the successional stage= 1;timeinstage =0; age=0; time since fire=0. 
												// When forsce simulate non-veg to non-veg * veg to non-veg the buffer is  set to 0, which will handle by "merg_lccBuffer()" after the completion this demand allocation look at lads.cpp.
						timeinstage[cell_index]=0;	// time in current successional stage grid
						age[cell_index]=0;		    // patch age grid
						tsfire[cell_index]=0;      // time since last fire grid
					}
					lccgrid[cell_index]=lcccode; 

					//cout<<int(lccgrid[cell_index])<<endl;

					demand--;
					counter++;
				}

			}
			vecobj.erase(vecobj.begin()+rand_index-1);
			vecobj.begin();		

		}
		cout<< "Rem. to accomp demand::"<<demand <<"\t Lcc Code::"<<lcccode<<"\t Rem. trans. prob pixel #::" <<vecobj.size() <<"counter" <<counter<< endl; 
	}
	
	else
	{
		cout<< "Demand or transition prob pixel size/#::"<<demand <<"::\t"<<vecobj.size()<< endl;
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

//Reclassifying the LADS/FORESCE output into initial LCLU classes.
void reclassify_lclu(unsigned int stateout[],unsigned int lclustate[],unsigned int statecounter)
{
	for(unsigned int i=0;i<=statecounter;i++)
	{
	cout<<stateout[i] <<"****" <<lclustate[i]<<endl;
	}

	for(unsigned int index=0; index<=size;index++)
	{
		for(unsigned int i=0;i<=statecounter;i++)
		{
			if(temp[index]==stateout[i])
			{
				temp[index]=lclustate[i];
			}
		}
	}
	
	for(int index=0;index<=size;index++)
	{
		for(int lclass=0;lclass<numlcc;lclass++)
		{
			if(((int)temp[index]==outlcccode[lclass]) && (lcc_flag[lclass]==0))
			{
				temp[index]==(unsigned char)inlcccode[lclass];
			}
		}
		
	}

	lccgrid=reinterpret_cast<char*>(temp); //convert from 'unsigned char *' to 'char *' 

}
