//----------------------------------------------------------------------------
// File:		lcc.cpp
// Author: 		Ashis Lamsal
// Last Update:	12/18/2012
// Decription:	
//----------------------------------------------------------------------------
#include "lcc.h"
#include <iostream>

using namespace std;

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
		if(buffergrid[index]>0 && lcc[index]>0)
		{
			for(int lclass=0;lclass<numlcc;lclass++) //loop to determine the lcc class and their corresponding lcc flag and input code 
			{
				if(buffergrid[index]==2 ||(lcc[index]==inlcccode[lclass] && lcc_flag[lclass]==0)) //Extract man made & non vegetated areas areas only(Exclude- natural vegetated areas) and treat them as buffer
					{
						//std::cout<<int(lcc[index])<<"\t"<<inlcccode[lclass]<<"\t"<<lcc_flag[lclass]<<endl;
						buffergrid[index]=2;
					}
			}
			
		}
		else
		{
			buffergrid[index]=0;
		}
	}

}

void merg_lccSnapshot()
{
	//for(int index=0;index<=size;index++)
	//{
	//	// Need to loop  through the lcc file and figure out.....
	//	if(lccgrid[index]==11)
	//	{
	//		temp[index]=211;
	//	}
	//	else if(lccgrid[index]==12)
	//	{
	//		temp[index]=212;
	//	}
	//	else if(lccgrid[index]==20)
	//	{
	//		temp[index]=213;
	//	}
	//	else if(lccgrid[index]==22)
	//	{
	//		temp[index]=214;
	//	}
	//	else if(lccgrid[index]==23)
	//	{
	//		temp[index]=215;
	//	}
	//	else if(lccgrid[index]==24)
	//	{
	//		temp[index]=216;
	//	}
	//	else if(lccgrid[index]==31)
	//	{
	//		temp[index]=217;
	//	}
	//	else if(lccgrid[index]==81)
	//	{
	//		temp[index]=227;
	//	}
	//	else if(lccgrid[index]==82)
	//	{
	//		temp[index]=228;
	//	}
	//	else if(lccgrid[index]==90)
	//	{
	//		temp[index]=229;
	//	}
	//	else if(lccgrid[index]==95)
	//	{
	//		temp[index]=230;
	//	}
	//	else
	//	{
	//		temp[index]=temp[index];
	//	}
	//	
	//}
	
	for(int index=0;index<=size;index++)
	{
		for(int lclass=0;lclass<numlcc;lclass++)
		{
			if(lccgrid[index]==inlcccode[lclass] && lcc_flag[lclass]==0 )
			{
				temp[index]=outlcccode[lclass];			
			}
		}
		
	}




}