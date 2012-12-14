//----------------------------------------------------------------------------
// File:		celllist.cpp
// Author: 		Mike Wimberly
// Last Update:	1/4/2008
// Decription:	Defines a linked list class for cell coordinates
//----------------------------------------------------------------------------

#pragma once

#include <iostream>

class celllist
{
public:
	celllist(void);
	void printcelllist(void);
	void addtohead(int xcoord, int ycoord);
	void addtotail(int xcoord, int ycoord);
	void deletecell(void);
	void reset(void);
	void increment(void);
	void printcell(void);
	int isempty(void);
	int islast(void);
	int getcol(void);
	int getrow(void);
	~celllist(void);
private:
	struct cell
	{
		int x;
		int y;
		cell *next_cell;
		cell *prev_cell;
	} * first_cell;
	cell* last_cell;
	cell* current_cell;
};
