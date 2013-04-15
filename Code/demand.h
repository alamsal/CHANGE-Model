#ifndef demandH
#define demandH

#include <vector>
#include <string>

const int DEMANDROW = 17;
const int DEMANDCOL=17;

using namespace std;

extern std::vector<std::vector<string> > demand_matrix;

void split_line(string& line, string demlim);

std::vector<std::vector<string> > read_demandCsv();

#endif 
