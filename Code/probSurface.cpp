// File:		probSurface.cpp
// Author: 		Ashis Lamsal
// Last Update:	3/4/2013
// Decription:	Read all probability files into a 3 dimensional vector
//----------------------------------------------------------------------------
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <vector>

using namespace std;
/* Reference examples on multidimensional vector defination and access.
	vector<vector<vector<float>>> phantom_energy (height, vector<vector<float>> (width, vector<float> (depth, 0.0f)));
	vector<vector<int> > dynamicArray(ROWS, vector<int>(COLUMNS));

	for(int i = 0;i < dynamicArray.size();++i)
	{
		  for(int j = 0;j < dynamicArray[i].size();++j)
		  {
			   dynamicArray[i][j] = i*j;
		  }
	}
 
	for(int i = 0;i < dynamicArray.size();++i)
	{           
		 for(int j = 0;j < dynamicArray[i].size();++j)
		 {
			  cout << dynamicArray[i][j] << endl;
		 }
	}
*/

void read_probabilitySurfaces(std::vector< std::vector<std::vector< float > > > & grid_value,float NumberOfFiles,float rows,float columns)
{
   FILE *f[20];
    char FileName[120];  /* Plenty large for your file naming */
	unsigned int err;

	//int grid_value[5][5][5];
	
    //cout<<sizeof(grid_value);
	//Open all probability files eg. prob0.gis,prob1.gis,prob2.gis..
    for (unsigned int i=0; i<NumberOfFiles; i++){
                         sprintf_s(FileName,"prob%d.gis",i);
                       // f[i]=fopen(FileName,"rb");
						if((err=fopen_s(&f[i],FileName,"rb"))!=0)
						{
							printf("@@@ERROR in opening probability *.gis file!");
						}
						else
						{
							cout<<"The probability file to be processed:"<<FileName<<endl;
							
						
						}
                            
    }

    cout<<"Reading probability surface files..."<<endl;


	//Loop through all probability files and read their rows/columns into 3D vector named grid-value[][][].
    for (unsigned int nof=0; nof<NumberOfFiles;nof++)
    {
        fseek ( f[nof], 128, SEEK_SET ); //Skip 128 byte *.GIS header byte to read acutal data from prob grid. 

        for(unsigned int row=0; row<rows; row++) 
        {
            for(unsigned register int col=0; col<columns; col++) 
            {
              unsigned int tvar = (unsigned int) fgetc(f[nof]);
              grid_value[nof][row][col]=(float)tvar/255.0;
             // cout<<"("<<nof<<","<<col+1<<","<<row+1<<")"<<grid_value[nof][row][col]<<"\t";
			  if(grid_value[nof][row][col]<0)
			  {
				  cout<<"Hit the -ve Value !! in the Probability GIS file. Program is Ready to quit...."<<endl;
				   cin.get();
				   exit(1);
			  
			  }
                
    		}
    	}
    }
    /*for(int row=0; row<10; row++) 
        {
            for(int col=0; col<100; col++) 
            {
                    cout<<grid_value[1][row][col]<<"\t";
                
    		}
    	} */

}
