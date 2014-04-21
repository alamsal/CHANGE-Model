//----------------------------------------------------------------------------
// File:		ladsio.cpp
// Author: 		Mike Wimberly
// Last Update:	1/4/2008
// Decription:	Contains functions to read and write ERDAS 7.4 files, and to
//              generate summaries of fire frequency and structure patterns
//----------------------------------------------------------------------------

#pragma hdrstop
#include "ladsio.h"
#include "lads.h"
#include "biomass.h"
#include "lcc.h"
#include <iostream>
using namespace std;
//----------------------------------------------------------------------------
// read_veg3_grid - reads in initial dead biomass grid
//----------------------------------------------------------------------------
struct image_header read_veg3_grid( char *filename, double *gridname, int timestep )
{
    int row, col;               // row and column counters
    int index;                  // array index
    int maxrow, maxcol;         // size of input array
    char infilename[30];        // input file name
    struct image_header imagehd;    // input file header
    char tvar;

    // open specified ascii grid file
    strcpy(infilename, filename);
    strcat(infilename, ".gis");
    FILE *ingrid;
    ingrid = fopen(infilename, "rb");

    fread(&imagehd, sizeof(struct image_header), 1, ingrid);

    maxrow = imagehd.rrows;
    maxcol = imagehd.rcols;

    // read in grid cell information
	for(row=0; row<maxrow; row++) {
        for(col=0; col<maxcol; col++) {
			index=row*maxcol + col;
            tvar = (char) fgetc(ingrid);
            gridname[index] = (double)tvar * timestep;
		}
	}

    fclose(ingrid);

    return(imagehd);

}

//----------------------------------------------------------------------------
// read_veg2_grid - reads in initial age and tsfire files in ERDAS GIS 7.4 format
//----------------------------------------------------------------------------
struct image_header read_veg2_grid( char *filename, short int *gridname, int timestep )
{
    int row, col;               // row and column counters
    int index;                  // array index
    int maxrow, maxcol;         // size of input array
    char infilename[30];        // input file name
    struct image_header imagehd;    // input file header
    char tvar;
    
    // open specified ascii grid file
    strcpy(infilename, filename);
    strcat(infilename, ".gis");
    FILE *ingrid;
    ingrid = fopen(infilename, "rb");

    fread(&imagehd, sizeof(struct image_header), 1, ingrid);

    maxrow = imagehd.rrows;
    maxcol = imagehd.rcols;

    // read in grid cell information
	for(row=0; row<maxrow; row++) {
        for(col=0; col<maxcol; col++) {
			index=row*maxcol + col;
            tvar = (char) fgetc(ingrid);
            gridname[index] = (int) tvar * timestep;
		}
	}

    fclose(ingrid);

    return(imagehd);

}




//----------------------------------------------------------------------------
// read_veg_grid - reads in initial successional stage grid
//----------------------------------------------------------------------------
struct image_header read_veg_grid( char *filename, short int *gridname )
{
    int row, col;               // row and column counters
    int index;                  // array index
    int maxrow, maxcol;         // size of input array
    char infilename[30];        // input file name
    struct image_header imagehd;    // input file header
    char tvar;

    // open specified ascii grid file
    strcpy(infilename, filename);
    strcat(infilename, ".gis");
    FILE *ingrid;
    ingrid = fopen(infilename, "rb");

    fread(&imagehd, sizeof(struct image_header), 1, ingrid);

    maxrow = imagehd.rrows;
    maxcol = imagehd.rcols;

    // read in grid cell information
	for(row=0; row<maxrow; row++) {
        for(col=0; col<maxcol; col++) {
			index=row*maxcol + col;
            tvar = (char) fgetc(ingrid);
            gridname[index] = (short int)tvar;
		}
	}

    fclose(ingrid);

    return(imagehd);

}

//----------------------------------------------------------------------------
// read_grid - reads 8-bit ERDAS 7.4 initialization grids
//----------------------------------------------------------------------------
struct image_header read_grid( char *filename, char *gridname )
{
    int row, col;               // row and column counters
    int index;                  // array index
    int maxrow, maxcol;         // size of input array
    char infilename[30];        // input file name
    struct image_header imagehd;    // input file header

    // open specified ascii grid file
    strcpy(infilename, filename);
    strcat(infilename, ".gis");
    FILE *ingrid;
    ingrid = fopen(infilename, "rb");

    fread(&imagehd, sizeof(struct image_header), 1, ingrid);
	//printf("%d",sizeof(struct image_header));
    maxrow = imagehd.rrows;
    maxcol = imagehd.rcols;

    // read in grid cell information
	for(row=0; row<maxrow; row++) {
        for(col=0; col<maxcol; col++) {
			index=row*maxcol + col;
            gridname[index] = (char) fgetc(ingrid);
			
		}
	}

    fclose(ingrid);

    return(imagehd);

}

//----------------------------------------------------------------------------
// read_16bit_grid - reads in 16-bit ERDAS 7.4 initalization grids
//----------------------------------------------------------------------------
struct image_header read_16bit_grid( char *filename, short int *gridname )
{
    int maxrow, maxcol;         // size of input array
    char infilename[30];        // input file name
    struct image_header imagehd;    // input file header

    // open specified grid file
    strcpy(infilename, filename);
    strcat(infilename, ".gis");
    FILE *ingrid;
    ingrid = fopen(infilename, "rb");

    fread(&imagehd, sizeof(struct image_header), 1, ingrid);

    maxrow = imagehd.rrows;
    maxcol = imagehd.rcols;

    fread(gridname, 2 * maxrow * maxcol, 1, ingrid);
    
    fclose(ingrid);

    return(imagehd);

}


//----------------------------------------------------------------------------
// gen_grid - generates a homogeneous grid array
//----------------------------------------------------------------------------
struct image_header gen_grid( int nrows, int ncols, int cellval, char *gridname )
{
    int row, col;               // row and column counters
    int index;                  // array index
    int maxrow, maxcol;         // size of input array
    struct image_header imagehd;    // input file header

    strcpy(imagehd.head, "HEAD74");
    imagehd.pack_type = 0;
    imagehd.nbands = 1;
    imagehd.rrows = nrows;
    imagehd.rcols = ncols;
    imagehd.xfirst = 0;
    imagehd.yfirst = 0;
    imagehd.num_classes = 1;
    imagehd.x_pixel_size = 1;
    imagehd.y_pixel_size = 1;

    maxrow = imagehd.rrows;
    maxcol = imagehd.rcols;

    // read in grid cell information
	for(row=0; row<maxrow; row++) {
        for(col=0; col<maxcol; col++) {
			index=row*maxcol + col;
            gridname[index] = (char)cellval;
		}
	}

    return(imagehd);

}


//----------------------------------------------------------------------------
// write_grid - writes output to a 8-bit ERDAS GIS file
//----------------------------------------------------------------------------
void write_grid( char *outfilename, unsigned char *gridname, struct image_header imagehd )
{

    FILE *outimage;         // Ouput image file

    outimage = fopen(outfilename, "wb");

    strcpy(imagehd.head, "HEAD74");
    imagehd.pack_type = 0;
    imagehd.nbands = 1;
    imagehd.num_classes = numstate;
    imagehd.iautyp = 2;
    imagehd.acre = (float)pow((double)cellsize, 2) / 10000;

    fwrite(&imagehd, sizeof(struct image_header), 1, outimage);

    fwrite(gridname, 1, maxrow * maxcol, outimage);

    fclose(outimage);

    return;
}

//----------------------------------------------------------------------------
// gen_strucsum - generates summary grids for percent time in each successional stage
//----------------------------------------------------------------------------
void gen_strucsum( char *runname, int nclass, int nsum, struct image_header out_head )
{
    int row;                // row counter
    int col;                // column counter
    int index;              // grid index
    int index2;             // index for accessing 3-d stucture summary array
    int clasloop;           // counter for looping through structure classes
    int struc;              // forest structure index
    char classtr[10];       // current summary year
    char outfilename[30];   // output file name

    for(clasloop = 0; clasloop < nclass; clasloop ++) {

        // open output file
        strcpy(outfilename, runname);
        strcat(outfilename, "s");
        itoa(clasloop + 1, classtr, 10);
        strcat(outfilename, classtr);
        strcat(outfilename, ".gis");

   	    for(row=0; row<maxrow; row++) {
		    for(col=0; col<maxcol; col++) {
			    index=row*maxcol + col;
                if(buffer[index] == 1) {
                    index2=row*maxcol + col + clasloop * size;
					struc = (int)ceil(100 * runstep * (long double)strucsum[index2]/(long double)nsum);
					//struc = (int)ceil(100 * (long double)strucsum[index2]/(long double)nsum);
                } else {
                    struc = 0;
                }
                temp[index] = (unsigned char)struc;
            }
        }

        write_grid(outfilename, temp, out_head);

    }

    return;
}


//----------------------------------------------------------------------------
// gen_firesum - generates summary grid for fire frequency
//----------------------------------------------------------------------------
void gen_firesum( char *filename, int nsum, struct image_header out_head, int timestep )
{
    int row;                // row counter
    int col;                // column counter
    int index;              // array index
    short int struc;        // forest structure index
    char outfilename[30];   // output file name

    // open output file
    strcpy(outfilename, filename);
    strcat(outfilename, "ff.gis");

    // write grid cell information
   	for(row=0; row<maxrow; row++) {
		for(col=0; col<maxcol; col++) {
			index=row*maxcol + col;
            if(buffer[index] == 1 && fsum1[index] > 0) {
                struc = (short)ceil((long double)nsum/(long double)fsum1[index]/ timestep);
            } else {
                struc = 0;
            }
            temp[index] = (unsigned char)struc;
        }
    }

    write_grid(outfilename, temp, out_head);

    return;
}

//----------------------------------------------------------------------------
// gen_sevsum - generates two fire severity summary grids
// one for % of stand-replacement fires, one for % of mixed-severity fires
//----------------------------------------------------------------------------
void gen_sevsum( char *filename, int nsum, struct image_header out_head )
{
    int row;                // row counter
    int col;                // column counter
    int index;              // array index
    short int struc;        // forest structure index
    char outfilename1[30], outfilename2[30];   // output file name


    // open output file
    strcpy(outfilename1, filename);
    strcat(outfilename1, "hs.gis");

    // write summary information - stand-replacement fire severity
   	for(row=0; row<maxrow; row++) {
		for(col=0; col<maxcol; col++) {
			index=row*maxcol + col;
            if(buffer[index] == 1 && fsum1[index] > 0) {
                struc = (short)ceil(100 *(long double)fsum2[index]/(long double)fsum1[index]);
            } else {
                struc = 0;
            }
            temp[index] = (unsigned char)struc;
        }
    }

    write_grid(outfilename1, temp, out_head);

    // open output file
    strcpy(outfilename2, filename);
    strcat(outfilename2, "ms.gis");

	// write summary information - mixed fire severity
   	for(row=0; row<maxrow; row++) {
		for(col=0; col<maxcol; col++) {
			index=row*maxcol + col;
            if(buffer[index] == 1 && fsum1[index] > 0) {
                struc = (short)ceil(100 *(long double)fsum3[index]/(long double)fsum1[index]);
            } else {
                struc = 0;
            }
            temp[index] = (unsigned char)struc;
        }
    }

    write_grid(outfilename2, temp, out_head);

    return;
}

//----------------------------------------------------------------------------
// gen_snapshot - generate "snapshot" of landscape configuration at a single point in time
//----------------------------------------------------------------------------
void gen_snapshot( char *runname, int year, struct image_header out_head,
    int snapsum, int transsum )
{
    int row;                // row counter
    int col;                // column counter
    int index;              // array index
    short int struc;        // forest structure index
    char yearstr[10];       // current summary year
    char outfilename[30];   // output file name

    // open output file
    strcpy(outfilename, runname);
    itoa(year, yearstr, 10);
    strcat(outfilename, yearstr);
    strcat(outfilename, ".gis");

    // write grid cell information to temporary grids
   	for(row=0; row<maxrow; row++) {
		for(col=0; col<maxcol; col++) {
			index=row*maxcol + col;
            if(buffer[index] == 1) 
			{
                struc = get_struc(index);
                temp[index] = (unsigned char)struc;
				
            }
			if(buffer[index]==0)
			{
                temp[index] = lccgrid[index];
            }

        }
    }
	
	//Added by Ashis 12/28/2012
	// write lcc info into temp grid

	
	merg_lccSnapshot();

    // write grids to output files
    if (snapsum > 0) {
        write_grid(outfilename, temp, out_head);
    } 
	
    return;
}

//----------------------------------------------------------------------------
// gen_biomass - generate "snapshot" grid of wood biomass variables
//----------------------------------------------------------------------------

void gen_biosum( char *runname, int year, struct image_header out_head)
{
    int row;                // row counter
    int col;                // column counter
    int index;              // array index
    double livebiomass;
    double livebiomass2;
    double deadbiomass;
    double deadbiomass2;
    char yearstr[10];       // current summary year
    char outfilename[30];   // output file name
    char outfilename2[30];  // transition output file name

    // open output files
    strcpy(outfilename, runname);
    itoa(year, yearstr, 10);
    strcat(outfilename, yearstr);
    strcat(outfilename, "l");
    strcat(outfilename, ".gis");

    strcpy(outfilename2, runname);
    itoa(year, yearstr, 10);
    strcat(outfilename2, yearstr);
    strcat(outfilename2, "d");
    strcat(outfilename2, ".gis");

    // write grid cell information to temporary grids
   	for(row=0; row<maxrow; row++) {
		for(col=0; col<maxcol; col++) {
			index=row*maxcol + col;

            if(buffer[index] == 1) {
                if(tsfire[index] == age[index]) {
                    livebiomass = calc_live_biomass ( index );
                } else {
                    livebiomass = calc_live_biomass ( index );
                }
                livebiomass2 = ceil(livebiomass/10) + 1;
                temp[index] = (unsigned char)livebiomass2;
                deadbiomass = deadgrid[index];
                deadbiomass2 = ceil(deadbiomass/10) + 1;
                temp2[index] = (unsigned char)deadbiomass2;
            } else {
                temp[index] = 0;
                temp2[index] = 0;
            }
        }
    }

    // write grids to output files
    write_grid(outfilename, temp, out_head);
    write_grid(outfilename2, temp2, out_head);




    return;
}

//----------------------------------------------------------------------------
// gen_agesum - generate "snapshot" of age variables
//----------------------------------------------------------------------------
void gen_agesum( char *runname, int year, struct image_header out_head, int timestep)
{
    int row;                // row counter
    int col;                // column counter
    int index;              // array index
    char yearstr[10];       // current summary year
    char outfilename[30];   // output file name
    char outfilename2[30];  // transition output file name

    // open output file
    strcpy(outfilename, runname);
    strcpy(outfilename2, runname);
    itoa(year, yearstr, 10);
    strcat(outfilename, yearstr);
    strcat(outfilename2, yearstr);
    strcat(outfilename, "a");
    strcat(outfilename2, "t");
    strcat(outfilename, ".gis");
    strcat(outfilename2, ".gis");

    // write grid cell information to temporary grids
   	for(row=0; row<maxrow; row++) {
		for(col=0; col<maxcol; col++) {
			index=row*maxcol + col;

            if(buffer[index] == 1) {
                if(age[index] <= 255 * timestep) {
                    temp[index] = 1 + (unsigned char)(age[index]/timestep);
                } else {
                    temp[index] = 255;
                }
                if(tsfire[index] <= 255 * timestep) {
                    temp2[index] = 1 + (unsigned char)(tsfire[index]/timestep);
                } else {
                    temp2[index] = 255;
                }
            } else {
                temp[index] = 0;
                temp2[index] = 0;
            }
        }
    }

    // write grids to output files
    write_grid(outfilename, temp, out_head);
    write_grid(outfilename2, temp2, out_head);

    return;
}

// Need to add a parameter for number of community types
void landscape_sum( int *classnum, int zonesum ) {

    int row;
    int col;
    int index;
    int classcnt;
    int zonecnt;
    int struc_type;
    int cursumzone;

    for(zonecnt = 0; zonecnt < zonesum; zonecnt++) {
		for(classcnt = 1; classcnt <= classnum[zonecnt]; classcnt++) {
            sum_array[classcnt][zonecnt] = 0;
        }
    }

    for(zonecnt = 0; zonecnt < zonesum; zonecnt++) {
        sum_species[zonecnt] = 0;
        sum_livebio[zonecnt] = 0;
        sum_deadwood[zonecnt] = 0;
    }

	/* Tally the number of cells in each structure */

    for(row=0; row<maxrow; row++) {
        for(col=0; col<maxcol; col++) {
            index=row*maxcol + col;
            if(buffer[index] == 1) {
                //if(zonesum == 1) {
                //    cursumzone = 0;
                //} else {
                cursumzone = comgrid[index] - 1;
                //}

			    struc_type = stategrid[index];
			    sum_array[struc_type][cursumzone] ++;
                if(spec_flag == 1) {
                    if(specgrid[index] == 1) {
                        sum_species[cursumzone] ++;
		            }
                }
                if( biom_flag == 1) {
                    sum_deadwood[cursumzone] += deadgrid[index];
                    if(tsfire[index] == age[index]) {
                        sum_livebio[cursumzone] += calc_live_biomass ( index );
                    } else {
                        sum_livebio[cursumzone] += calc_live_biomass ( index );
                    }
                }
	        }
        }
    }


	for(zonecnt = 0; zonecnt < zonesum; zonecnt++) {
		for(classcnt = 1; classcnt <= classnum[zonecnt]; classcnt++) {
            sum_array[classcnt][zonecnt] = 100 * sum_array[classcnt][zonecnt] /
                zsize[zonecnt];
        }
    }

    if(spec_flag == 1) {
        for(zonecnt = 0; zonecnt < zonesum; zonecnt++) {
            sum_species[zonecnt] = sum_species[zonecnt]/zsize[zonecnt];
        }
    }
    if(biom_flag == 1) {
        for(zonecnt = 0; zonecnt < zonesum; zonecnt++) {
            sum_livebio[zonecnt] = sum_livebio[zonecnt]/zsize[zonecnt];
            sum_deadwood[zonecnt] = sum_deadwood[zonecnt]/zsize[zonecnt];
        }
    }

    return;
}
//----------------------------------------------------------------------------
// gen_snapshot - generate "LCC snapshot" of landscape configuration at a single point in time
//----------------------------------------------------------------------------
void gen_lccsnapshot( char *runname, int year, struct image_header out_head,  int snapsum, int transsum )
{
    int row;                // row counter
    int col;                // column counter
    int index;              // array index
    short int struc;        // forest structure index
    char yearstr[10];       // current summary year
    char outfilename[30];   // output file name

    // open output file
    strcpy(outfilename, runname);
    itoa(year, yearstr, 10);
    strcat(outfilename, yearstr);
    strcat(outfilename, ".gis");
	
    // write grids to output files
    if (snapsum > 0) {

		write_grid(outfilename, temp, out_head);
    }

    return;
}


void gen_forescesnapshot( char *runname, int year, struct image_header out_head,  int snapsum, int transsum )
{
    int row;                // row counter
    int col;                // column counter
    int index;              // array index
    short int struc;        // forest structure index
    char yearstr[10];       // current summary year
    char outfilename[30];   // output file name

    // open output file
    strcpy(outfilename, runname);
    itoa(year, yearstr, 10);
    strcat(outfilename, yearstr);
    strcat(outfilename, ".gis");

    // write grids to output files
    if (snapsum > 0) {
	    unsigned char* foresceOnly;
		foresceOnly=reinterpret_cast<unsigned char*>(temp);
		write_grid(outfilename, foresceOnly, out_head);
		

		
    }

    return;
}
void gen_hnisnapshot( char *runname, int year, struct image_header out_head,int snapsum, int transsum )
{
	int row;                // row counter
    int col;                // column counter
    int index;              // array index
    short int struc;        // forest structure index
    char yearstr[10];       // current summary year
    char outfilename[30];   // output file name

    // open output file
    strcpy(outfilename, runname);
    itoa(year, yearstr, 10);
    strcat(outfilename, yearstr);
    strcat(outfilename, ".gis");

    // write grids to output files
    if (snapsum > 0) {
	    unsigned char* hni;
		hni=reinterpret_cast<unsigned char*>(hnigrid);
		write_grid(outfilename, hni, out_head);
		

		
    }

    return;
}

void gen_severitysnapshot( char *runname, int year, struct image_header out_head,int snapsum, int transsum )
{
	int row;                // row counter
    int col;                // column counter
    int index;              // array index
    short int struc;        // forest structure index
    char yearstr[10];       // current summary year
    char outfilename[30];   // output file name

    // open output file
    strcpy(outfilename, runname);
    itoa(year, yearstr, 10);
    strcat(outfilename, yearstr);
    strcat(outfilename, ".gis");

    // write grids to output files
    if (snapsum > 0) {
	    unsigned char* sev;
		sev=reinterpret_cast<unsigned char*>(severitygrid);
		write_grid(outfilename, sev, out_head);
					
    }

    return;
}

void get_harvestsnapshot(char *runname, int year, struct image_header out_head,int snapsum, int transsum )
{

	int row;                // row counter
    int col;                // column counter
    int index;              // array index
    short int struc;        // forest structure index
    char yearstr[10];       // current summary year
    char outfilename[30];   // output file name

    // open output file
    strcpy(outfilename, runname);
    itoa(year, yearstr, 10);
    strcat(outfilename, yearstr);
    strcat(outfilename, ".gis");

    // write grids to output files
    if (snapsum > 0) {
	    unsigned char *harv;
		harv=reinterpret_cast<unsigned char*>(harvestgrid);
		write_grid(outfilename, harv, out_head);
					
    }

    return;

}