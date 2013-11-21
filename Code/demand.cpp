#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <list>
#include <vector>

#include "demand.h"
using namespace std;


unsigned int dcol; //Initialize for demand  csv columns
unsigned int drow; // Initialize for demand csv rows
//std::vector<std::vector<string> > demand_matrix(DEMANDROW,std::vector<string>(DEMANDCOL));



void split_line(string& line, string delim, int dfile,std::vector< std::vector<std::vector< int > > > & demmat)
{
    size_t pos = 0;
    while ((pos = line.find(delim, (pos + 1))) != string::npos) 
    {
        string p = line.substr(0, pos);
        //values.push_back(p);
        //demand_array[row][col]=p;
        demmat[dfile][drow][dcol]=stoi(p);
        line = line.substr(pos + 1);
        
    }
    if (!line.empty()) 
    {
       drow++;
       dcol=0;
	   
      demmat[dfile][drow][dcol]=stoi(line);
       //demand_array[row][col]=line;
       //values.push_back(line);
       dcol++;
    }
}

//std::vector<std::vector<string> > read_demandCsv()
void read_demandCsv(std::vector< std::vector<std::vector< int > > > & demmat,int NumberOfFiles,int rows,int columns)
{

	for(unsigned int dfile=0;dfile<NumberOfFiles;dfile++)
	{
		dcol=0;
		drow=0;
		char demandName[120];
		sprintf_s(demandName,"demand%d.csv",dfile);

		ifstream file, file1;
		//try
		//{
		//	file.open("demand.csv"); // declare file stream: http://www.cplusplus.com/reference/iostream/ifstream/
		
		//file.open("demand.csv");
		//file.open("dem0.csv");
		file.open(demandName);
		if(!file.is_open())
		{
			cout<< "Input demand file could not be opended!!!"<<endl;
		}

		//}
		//catch(exception e)
		//{
		//	std::cerr<< "Unable to read demand file \t"; 
		//}
		//ifstream file ( "demand05.txt" ); // Read from text file need to check if it works or not.
		string value;
  
		while (file.good())
		{
			getline ( file, value, ',' ); // read a string until next comma: http://www.cplusplus.com/reference/string/getline/			
			if (value.find("\n") != string::npos) 
			{
					split_line(value, "\n",dfile,demmat);
			} 
			else 
			{                              
				//demmat[0][0][0]=stoi(value);
				demmat[dfile][drow][dcol]=stoi(value);
				dcol++;
			}
			
		}

		//cout<<demand_matrix.size()<<endl;
		//cout<<demand_matrix[0].size()<<endl;
		//cout<<demand_matrix[0][0].size()<<endl;
		file.close();
		file.clear();
	
	}

	
	
	for(unsigned int file=0;file<NumberOfFiles;file++)
	{
		cout<<"Demand variable from file#" <<"\t" <<file<<endl;
		for(int i =0; i<rows;i++)
		{
			for (int j=0; j<columns; j++)
			{
				cout<<"("<<i<<","<<j<<")"<<demmat[file][i][j]<<"\t";
			}
		} 
	
	}



	
}


