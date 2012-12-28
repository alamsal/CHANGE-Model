//----------------------------------------------------------------------------
// File:		lcc.cpp
// Author: 		Ashis Lamsal
// Last Update:	12/18/2012
// Decription:	
//----------------------------------------------------------------------------
#include "lcc.h"


void merg_lccBuffer(char *buffergrid,char *lcc)
{
	for(int index=0;index<size;index++)
	{
		//printf("%d",buffer[index]);
		//printf("%d",lccgrid[index]);
		if(buffergrid[index]>0 && lcc[index]>0)
		{
			if(buffergrid[index]==2 ||(lcc[index]>=11 && lcc[index]<41)||(lcc[index]>=81 && lcc[index]<=95)) //Extract natural vegetation areas only(Exclude-man made & non vegetated areas)
			{
				buffergrid[index]=2;
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
	for(int index=0;index<=size;index++)
	{
		
		if(lccgrid[index]==11)
		{
			temp[index]=211;
		}
		else if(lccgrid[index]==12)
		{
			temp[index]=212;
		}
		else if(lccgrid[index]==21)
		{
			temp[index]=221;
		}
		else if(lccgrid[index]==22)
		{
			temp[index]=222;
		}
		else if(lccgrid[index]==23)
		{
			temp[index]=223;
		}
		else if(lccgrid[index]==24)
		{
			temp[index]=224;
		}
		else if(lccgrid[index]==31)
		{
			temp[index]=231;
		}
		else if(lccgrid[index]==81)
		{
			temp[index]=281;
		}
		else if(lccgrid[index]==82)
		{
			temp[index]=282;
		}
		else if(lccgrid[index]==90)
		{
			temp[index]=290;
		}
		else if(lccgrid[index]==95)
		{
			temp[index]=295;
		}
		else
		{
			temp[index]=temp[index];
		}
		
	}
	
}