//----------------------------------------------------------------------------
// File:		lads.h
// Author: 		Mike Wimberly
// Last Update:	1/4/2008
// Decription:	Main header file for the LADS (Landscape Age-Class Dynamics
//              Simulator) model. Holds global structure definitions.
//----------------------------------------------------------------------------

//#include <condefs.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <fstream>
#include <iostream>
//define_USE_MATH_DEFINES;

// Holds fire severity classes
struct fire_sev {
    int nclass;
    int lowerbnd[40];
    int upperbnd[40];
    double sev[40];
    double sev2[40];
};


// Header information for ERDAS 7.4 files
struct image_header {
    char head[6];
    short int pack_type;
    short int nbands;
    char notused2[6];
    int rcols;
    int rrows;
    int xfirst;
    int yfirst;
    char notused3[56];
    short int map_units;
    short int num_classes;
    char notused4[14];
    short int iautyp;
    float acre;
    float xmap;
    float ymap;
    float x_pixel_size;
    float y_pixel_size;
};

// Holds definitions of vegetation structure classes
struct veg_class {
    int nclass;
    int ndiv;
    int lowerage[40];
    int upperage[40];
    int lowertfire[40];
    int uppertfire[40];
    int vcat[40];
};

// Holds mortality rates
struct mortality {
    int nclass;
    int lowerbnd[40];
    int upperbnd[40];
    double mort[40][40];
};


// linked list structure
struct cell_list {
    int x;
    int y;
    struct cell_list *prev_ptr;
    struct cell_list *next_ptr;
};




