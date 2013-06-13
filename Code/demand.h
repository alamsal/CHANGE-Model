#ifndef demandH
#define demandH

#include <vector>
#include <string>

using namespace std;



void split_line(string& line, string delim, int defile,std::vector< std::vector<std::vector< int > > > & demand);		//Method to split the lines in demand matrix csv

void read_demandCsv(std::vector< std::vector<std::vector< int > > > & demand,int NumberOfFiles,int rows,int columns);		//Method to read and retrun demand matrix

#endif 
