//----------------------------------------------------------------------------
// File:		celllist.cpp
// Author: 		Mike Wimberly
// Last Update:	1/4/2008
// Decription:	Defines a linked list class for cell coordinates
//----------------------------------------------------------------------------



#include "StdAfx.h"
#include ".\celllist.h"

using namespace std;

// Constructor for the celllist object
celllist::celllist(void)
{
	first_cell = NULL;
	last_cell = NULL;
	current_cell = NULL;
}

// Reset pointer to the head of the list
void celllist::reset(void)
{
	current_cell = first_cell;
}

// Move pointer to next cell in the list
void celllist::increment(void)
{
	current_cell = current_cell->next_cell;
}

// Print cell based on current pointer location
void celllist::printcell(void)
{
	int curx;
	int cury;
	
	curx = current_cell->x;
	cury = current_cell->y;
	cout << "x= " << curx << " y= " << cury << "\n";
}

// Add a new cell to the head of the linked list
void celllist::addtohead(int xcoord, int ycoord)
{
	cell* new_cell = new cell;

	if(first_cell == NULL) {
		first_cell = new_cell;
		first_cell -> x = xcoord;
		first_cell -> y = ycoord;
		first_cell -> next_cell = NULL;
		first_cell -> prev_cell = NULL;
		current_cell = first_cell;
		last_cell = first_cell;
	} else {
		new_cell -> x = xcoord;
		new_cell -> y = ycoord;
		new_cell -> next_cell = first_cell;
		new_cell -> prev_cell = NULL;
		first_cell -> prev_cell = new_cell;
		first_cell = new_cell; 
	}

}

// Add a new cell to the tail of the linked list
void celllist::addtotail(int xcoord, int ycoord)
{
	cell* new_cell = new cell;

	if(last_cell == NULL) {
		last_cell = new_cell;
		last_cell -> x = xcoord;
		last_cell -> y = ycoord;
		last_cell -> next_cell = NULL;
		last_cell -> prev_cell = NULL;
		current_cell = last_cell;
		first_cell = last_cell;
	} else {
		new_cell -> x = xcoord;
		new_cell -> y = ycoord;
		new_cell -> prev_cell = last_cell;
		new_cell -> next_cell = NULL;
		last_cell -> next_cell = new_cell;
		last_cell = new_cell; 
	}

}

// Delete cell based on current pointer location
void celllist::deletecell(void)
{
	cell *leading_cell;// = new cell;
	cell *trailing_cell;// = new cell;

	leading_cell = current_cell -> next_cell;
	trailing_cell = current_cell -> prev_cell;

    if(leading_cell == NULL && trailing_cell == NULL) {
        first_cell = NULL;
		last_cell = NULL;
        delete current_cell;
		current_cell = NULL;
    // Delete the tail
	} else if(leading_cell == NULL) {
        trailing_cell -> next_cell = NULL;
        delete current_cell;
		last_cell = trailing_cell;
		current_cell = trailing_cell;
    // Delete the head
	} else if (trailing_cell == NULL) {
        leading_cell -> prev_cell = NULL;
        delete current_cell;
        first_cell = leading_cell;
		current_cell = leading_cell;
    } else {
        trailing_cell -> next_cell = leading_cell;
        leading_cell -> prev_cell = trailing_cell;
        delete current_cell;
		current_cell = leading_cell;
    }

}

// Print the coordinates of all the cells in the list
void celllist::printcelllist(void)
{
	cell* change_cell;// = new cell;
	cell* temp_cell;// = new cell;
	int curx;
	int cury;

	temp_cell = first_cell;

		while(1) {
			
			curx = temp_cell->x;
			cury = temp_cell->y;
			cout << "x= " << curx << " y= " << cury << "\n";
			if (temp_cell->next_cell == NULL) {
				break;
			} else {
				change_cell = temp_cell->next_cell;
				temp_cell = change_cell;
			}
		}

}

// Determine if the list contains any cells
int celllist::isempty(void)
{
	if(first_cell == NULL) {
		return 1;
	} else {
		return 0;
	}
}

// Determine if we are at the end of the list
int celllist::islast(void)
{
	if(current_cell == NULL) {
		return 1;
	} else {
		return 0;
	}
}

// Get column index based on current pointer location
int celllist::getcol(void)
{
	return current_cell -> x;
}

// Get row index based on current pointer location
int celllist::getrow(void)
{
	return current_cell -> y;
}

// Destructor for the celllist object
celllist::~celllist(void)
{
    cell *new_cell;// = new cell;

    while (first_cell != NULL) {
        new_cell = first_cell -> next_cell;
        delete first_cell;
        first_cell = new_cell;
    }
   
}
