#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <list>
#include <vector>

#include "demand.h"
using namespace std;

unsigned int dcol=0; //Initialize for demand  csv columns
unsigned int drow=0; // Initialize for demand csv rows
std::vector<std::vector<string> > demand_matrix(DEMANDROW,std::vector<string>(DEMANDCOL));

void split_line(string& line, string delim)
{
    size_t pos = 0;
    while ((pos = line.find(delim, (pos + 1))) != string::npos) 
    {
        string p = line.substr(0, pos);
        //values.push_back(p);
        //demand_array[row][col]=p;
        demand_matrix[drow][dcol]=p;
        line = line.substr(pos + 1);
        
    }
    if (!line.empty()) 
    {
       drow++;
       dcol=0;
       demand_matrix[drow][dcol]=line;
       //demand_array[row][col]=line;
       //values.push_back(line);
       dcol++;
    }
}

std::vector<std::vector<string> > read_demandCsv()
{
	ifstream file ( "demand.csv" ); // declare file stream: http://www.cplusplus.com/reference/iostream/ifstream/
	string value;
  
	while ( file.good() )
	{
		getline ( file, value, ',' ); // read a string until next comma: http://www.cplusplus.com/reference/string/getline/
		if (value.find('\n') != string::npos) 
		{
				split_line(value, "\n");
		} 
		else 
		{                              
			demand_matrix[drow][dcol]=value;
			dcol++;
		}
	}

	cout<<demand_matrix.size()<<endl;
	cout<<demand_matrix[0].size()<<endl;
	cout<<demand_matrix[0][0].size()<<endl;

	/*for(int i =0; i<DEMANDROW;i++)
	{
		for (int j=0; j<DEMANDCOL; j++)
		{
			cout<<"("<<i<<","<<j<<")"<<demand_matrix[i][j]<<"\t";
		}
	}*/
	return demand_matrix;
}


