// File:		demandH.h
// Author: 		Ashis Lamsal
// Last Update:	11/44/2013
// Decription:	Contains external variable declarations and function prototypes
//              for demand.cpp
//----------------------------------------------------------------------------
#ifndef demandH
#define demandH

#include <vector>
#include <string>

//using namespace std;



void split_line(std::string& line, std::string delim, int defile,std::vector< std::vector<std::vector< int > > > & demand);		//Method to split the lines in demand matrix csv

void read_demandCsv(std::vector< std::vector<std::vector< int > > > & demand,int NumberOfFiles,int rows,int columns);		//Method to read and retrun demand matrix

#endif 
