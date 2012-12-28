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

//Function prototypes
void merg_lccBuffer(char *buffergridname,char *lccgridname);	//Merging original buffer with LCC.
void merg_lccSnapshot(); // Merging lcc class into output grid snapshot.

#endif