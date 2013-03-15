//----------------------------------------------------------------------------
// File:		lcc.h
// Author: 		Ashis Lamsal
// Last Update:	12/16/2012
// Decription:	Contains external variable declarations and function prototypes
//              for lcc.cpp.
//----------------------------------------------------------------------------

#ifndef lccH
#define lccH

//Global variables
extern char *lccgrid;           // lcc classes grid
extern char *buffer;			// fire buffer zone grid
extern int size;				// total # of cells in landscape (includes background)
extern unsigned char *temp;				// temporary grid for holding classified veg data

extern int maxrow;         // max row and column address of grid
extern int maxcol;

extern int numlcc;					//Number of lcc types
extern int inlcccode[40];			//Input lcc code works upto 40 different LCC types.
extern int outlcccode[40];			//Output lcc code works upto 40 differtn LCC types
extern int lcc_flag[40];			//LCC flag- 1 simulate LCC 0 no simulate LCC

//Function prototypes
void merg_lccBuffer(char *buffergridname,char *lccgridname);	//Merging original buffer with LCC.
void merg_lccSnapshot(); // Merging lcc class into output grid snapshot.
void extract_forestCells(char *lccgridname);//Extract forest cells from LCC

#endif