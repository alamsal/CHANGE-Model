#ifndef demandH
#define demandH

#include <vector>
#include <string>

using namespace std;

const int DEMANDROW = 10; //Total # of rows in demand matrix
const int DEMANDCOL=10;   //Total # of columns in demand matrix



extern std::vector<std::vector<string> > demand_matrix; //Container to hold demand matrix

void split_line(string& line, string demlim);			//Method to split the lines in demand matrix csv

std::vector<std::vector<string> > read_demandCsv();		//Method to read and retrun demand matrix

#endif 
