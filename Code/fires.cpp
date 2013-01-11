//----------------------------------------------------------------------------
// File:		fire.cpp
// Author: 		Mike Wimberly
// Last Update:	1/4/2008
// Decription:	Contains functions to model fire spread and to compute the
//              shapes of burned patches.
//				Has been updated to compute modified fire spread probabilities
//				based on odds ratios instead of the previous approach.
//----------------------------------------------------------------------------

#pragma hdrstop
#include "lads.h"
#include "fires.h"
#include "randnum.h"
#include "celllist.h"

int fminrow;
int fmaxrow;
int fmincol;
int fmaxcol;

using namespace std;

//------------------------------------------------------------
// firespread function - iterates through the fire spread loop
//------------------------------------------------------------
int firespread( int firereg, int fsize ) {

    double f_susc;	// fire spread sucsceptibility
    int row;		// row index
    int col;		// column index
    int index;		// array index
    int i, j;        // cell neighborhood indices
    int row2;       // row counter
    int col2;       // column counter
    int index2;     // array index
    int exitnow;    // indicates if fire has reached max size
    int curburn;    // tallys cells burned in current iteration
    double totprob;         // total fire spread probability
    int cellcnt;            // counter for neighboring cells
    int goodcell;           // indicates whether or not fire can burn into cell
    int nextrow;			// used to pull row indices out of the linked list			
    int nextcol;			// used to pull column indices out of the linked list
	int passcount;			// keeps track of the number of iterations of the fire spread algorithm
	int inbound;
    double dist;			// distance between cells
    double slope;			// slope angle
    double slopefact;		// slope modifier to fire spread
	double spreadmod;		// composite fire spread modifiers
	double fire_cal_odds;
	double irand;			// u0_1 random variable assigned to all cells in each iteration of the algorithm
							// used later on in the fire effects calculations

	celllist ffirst_ptr;
    
	// Compute odds ratio for the base fire spread probability
	fire_cal_odds = fire_cal / (1 - fire_cal);
	
	// Initialize the fire grids
    for(index=0;index<size;index++)
	{
        if(regime[index] > 0)  //Edited by Ashis:1/4/2013 : if(regime[index] > 0)
		{		
			fsusc[index] = statefireinit[comgrid[index] - 1][stategrid[index] - 1] * landfireinit[landgrid[index] - 1];
			//printf("%d \t %d \n",statefireinit[comgrid[index] - 1][stategrid[index] - 1],landfireinit[landgrid[index] - 1]);
        }
        fgrid1[index] = 0;
        fgrid2[index] = 0;
    }

    // Select starting cell for fire, weighted by vegetation and topography
    while( 1 ) {
	    row = rand_int(maxrow) - 1;
		col = rand_int(maxcol) - 1;
		index = row * maxcol + col;
        if((int)regime[index] == (firereg + 1) ) {  //Edited by Ashis:1/4/2013 : f( ((int)regime[index] == firereg + 1)
		    
			f_susc = fsusc[index];
			
			//printf("Fire suc...%f\t\t%f\n",f_susc,u0_1());
		    
			if(u0_1() < f_susc )
			{
				break;
			}
        }
    }

    // Burn the initial cell 
    ffirst_ptr.addtotail(col, row);
    fgrid2[index] = 1;
    burned = 1;
	firespreadgrid[index] = 1;


    //Define the boundaries of a box encompassing the fire
    //Problem? What happens if initial fire location is already at
    // minrow?
    if (row < (maxrow - 1))
        fmaxrow = row + 1;
    if (row > 0)
        fminrow = row - 1;
    if (col < (maxcol - 1))
        fmaxcol = col + 1;
    if (col > 0)
        fmincol = col - 1;

    // Set initial wind direction
    set_winddir(w_dir, w_var, w_speed);

    // Iterate through the burning algorithm until an area equal to
    // the fire size has been burned
    if(fsize == 1 ) {
		return(1);
	}
    
    exitnow = 0;
    curburn = 0;
	passcount = 2;

    while(1) {
		// Go once through the list of burning cells
		irand = u0_1();
		while(ffirst_ptr.islast() == 0) {
            nextrow = ffirst_ptr.getrow();
            nextcol = ffirst_ptr.getcol();

            totprob = 0;
            cellcnt = 0;
			//printf("Go once through the list of burning cells\n");
            // Loop through neighboring cells
            row = nextrow;
            col = nextcol;
            index = row * maxcol + col;
            for(i=-1;i<=1;i++) {
                for(j=-1;j<=1;j++) {
                    // eight-cell neighborhood
                    if( i!=0 || j!=0 ) {
                        // can burn into cell if it's on the grid,
                        // doesn't have a nodata value, and hasn't been
                        // burned
                        goodcell = 1;
                        row2 = row + i;
                        col2 = col + j;
						if(row2 < maxrow && row2 >= 0 && col2 < maxcol && col2 >= 0) {
							inbound = 1;
						} else if(torus == 1) {
							inbound = 1;
							if(row2 >= maxrow)
								row2 = 0;
							if(row2 < 0)
								row2 = maxrow - 1;
							if(col2 >= maxcol)
								col2 = 0;
							if(col2 < 0)
								col2 = maxcol - 1;
						} else {
							inbound = 0;
						}
						
						if(inbound == 1) {
			                index2 = row2 * maxcol + col2;
							if(fgrid2[index2] != 0 )
								goodcell = 0;
							if(regime[index2] <= 0) // Comment Need special care for Buffer and Regime to create good cell.- if(regime[index2] <= 0)
								goodcell = 0;
							if(tsfire[index2] < mintsfire)
								goodcell = 0;
							// Adjust fire spread probability if we're
							// next to a burned cell
							if (goodcell == 1) {
								dist = sqrt(pow((double)i,2) + pow((double)j,2));
								slope = ((double)dem[index2] - (double)dem[index]) / (dist * (double)cellsize ) ;
								slopefact = get_slopefact( slope );
								//Modify the fire spread odds ratio
								spreadmod = windfact[i+1][j+1] * slopefact * statefiremod[comgrid[index2] - 1][stategrid[index2] - 1] * landfiremod[landgrid[index2] - 1];
								// Convert modified odds ratio back to a probability
								f_susc = 1 / ((1 / (fire_cal_odds * spreadmod)) + 1);
							// otherwise no probability of spread
							} else {
								f_susc = 0;
							}

							if (u0_1() < f_susc) {
                                // assign burned cell a value of 1
                                fgrid2[index2] = 1; // changed this from one;
								fint[index2] = irand;
								if(sumtype > 0) {
									firespreadgrid[index2] = passcount;
                                }
                                ffirst_ptr.addtotail(col2, row2);
                                // push out fire boundary box
                                if(row2 == fminrow && fminrow > 0)
                                        fminrow --;
                                if(row2 == fmaxrow && fmaxrow < (maxrow - 1))
                                        fmaxrow ++;
                                if(col2 == fmincol && fmincol > 0)
                                        fmincol --;
                                if(col2 == fmaxcol && fmaxcol < (maxcol - 1))
                                        fmaxcol ++;
                                burned ++;
                                curburn ++;
                                // Bail out if we've exceeded the fire size
								if(burned >= fsize) {
                                        exitnow = 1;
								}
							}
						}  
                        if (exitnow == 1) break;
                    } // end if( i!=0 || j!=0 )
                    if (exitnow == 1) break;
                }  // end for j
                if (exitnow == 1) break;
            } // end for i

			if (exitnow == 1) {
				break;
			} else {
				ffirst_ptr.increment();
			}

		} // end cell list loop

		passcount++;
		if(exitnow==1) {
			return(burned);
		} else {
			ffirst_ptr.reset();
			// Go through the list and delete any cells that have burned out
			while(1) {
				if(u0_1() < fire_cal2)  {
					ffirst_ptr.deletecell();
					if (ffirst_ptr.isempty() == 1) {
						cout << "curburn == 0\n";
						return(burned);  
					} 
				} else {
					ffirst_ptr.increment();
				}
				if(ffirst_ptr.islast() == 1) {
					ffirst_ptr.reset();
					break;
				}
			}
			continue;
		}

	} // end while (1) loop

} // end function

//----------------------------------------------------------------------------
// fill islands - fills in "holes" for external fire perimeter calculations
//----------------------------------------------------------------------------
int fill_islands( void ) {

	int row;            // row counter
    int col;            // column counter
    int index;          // grid cell index
    int row2;           // row counter
    int col2;           // column counter
    int index2;         // grid cell index
    int row3;           // row counter
    int col3;           // column counter
    int i;              // cell neighborhood indices
    int j;
    int listlen;        // length of patch arrays
    int extborder;      //
    int newitem;
    int addedcells;
    int list;
    int totinterior;

    // initialize process indicator array
    for(index = 0; index < size; index ++) {
        processed[index] = 0;
    }

    totinterior = 0;

    // process the new fire patch that is currently in fgrid2
    // write results onto fgrid1
	for(row=fminrow; row<fmaxrow; row++) {
        for(col=fmincol; col<fmaxcol; col++) {
			index=row*maxcol + col;
            // only process cells that haven't been assigned to a patch
            if(processed[index] == 1)
                continue;
            // nodata cells are output as nodata
            if(regime[index] == 0) {
                fgrid1[index] = 0;
            // burned cells are all output as burned
            } else if(fgrid2[index] >= 1) {
                fgrid1[index] = 1;
            // otherwise, we have an unburned cell
            } else {
                listlen = 1;
                patchx[0] = (unsigned short int)col;
                patchy[0] = (unsigned short int)row;
                extborder = 0;
                // expand the patch until there are no neighbors to add
                while( 1 ) {
                    newitem = listlen - 1;
                    addedcells = 0;
                    // loop through the current patch list
                    for(list = 0; list < listlen; list++) {
                        row2 = patchy[list];
                        col2 = patchx[list];
                        // loop through neighbors of the current cell
					    for(i=-1;i<=1;i++) {
						    for(j=-1;j<=1;j++) {
                                // rook's neighborhood
							    if(i==0 || j==0) {
    								row3 = row2 + i;
	    							col3 = col2 + j;
                                    index2 = row3 * maxcol + col3;
                                    // if the cell is outside the fire box or is
                                    // nodata, then the patch is not an unburned
                                    // island
                                    if(row3 > fmaxrow || row3 < fminrow ||
                                       col3 > fmaxcol || col3 < fmincol) {
                                        extborder = 1;
                                    } else if(regime[index2] == 0) {
                                        extborder = 1;
                                    // if not, add the cell the the patch
                                    } else if(fgrid2[index2] == 0 &&
                                      processed[index2] == 0) {
                                        newitem ++;
                                        patchy[newitem] = (unsigned short int)row3;
                                        patchx[newitem] = (unsigned short int)col3;
                                        processed[index2] = 1;
                                        addedcells ++;
                                    }
                                }
                            }  // end for j
                        }  // end for i
                    } // end for list

                    // update list length for newly added cells
                    listlen = newitem + 1;
                    // if no new cells, patch is complete
                    if(addedcells == 0) {
                        break;
                    }
                } // end while(1)

                for(list = 0; list < listlen; list++) {
                    row3 = patchy[list];
                    col3 = patchx[list];
                    index2 = row3 * maxcol + col3;
                    // classify exterior cells
                    if(extborder == 1) {
                        fgrid1[index2] = 2;
                    // classify interior cells
                    } else {
                        totinterior ++;
                        fgrid1[index2] = 1;
                    }
                }
            }
		}   // end for row
	}   // end for col

    return(totinterior);
}


//----------------------------------------------------------------------------
// get_perim - calculates fire perimeters
//----------------------------------------------------------------------------
int get_perim( void )
{
    int row;        // row counter
    int col;        // column counter
    int index;      // array index
    int i;          // cell neighborhood indices
    int j;
    int row2;       // row counter
    int col2;       // column counter
    int index2;     // array index
    int perimcnt;   // number of perimeter cell edges

    perimcnt = 0;

    // loop through grid
    for(row=fminrow;row<=fmaxrow;row++) {
        for(col=fmincol;col<=fmaxcol;col++) {
            index=row*maxcol + col;
            // if cell is burned, loop through neighbors
            if(fgrid1[index] == 1) {
                for(i=-1;i<=1;i++) {
		            for(j=-1;j<=1;j++) {
                        // rook's adjacency rule
			            if(i==0 || j==0) {
                            row2 = row + i;
                            col2 = col + j;
                            index2 = row2 * maxcol + col2;
                            // increment perimeter counter if neighboring cell
                            // is unburned or on edge of map
                            if(row2 == -1 || col2 == -1 || row2 == maxrow || col2 == maxcol || fgrid1[index2] == 2 || regime[index2] == 0) {
                                perimcnt ++;
                            }
                        }
                    }   // end for j
                }   // end for i
            }
        }   // end for col
    }   // end for row

    return(perimcnt);

}

//----------------------------------------------------------------------------
// get_elongation - calculates elongation index
//----------------------------------------------------------------------------
double get_elongation( void )
{
    int row;        // row counter
    int col;        // column counter
	double xsum;
	double ysum;
	double cellcnt;
    int index;      // array index
    double anglecnt, anglecnt2;   // number of perimeter cell edges
	double slope1, slope2, intercept1, intercept2;
	double xcentroid, ycentroid;
	double xcell, ycell;
	double inertia1, inertia2, maxelongation;
	double disttoline1, disttoline2;
	double elongation;
	double a1, b1, c1, a2, b2, c2;
	
	//ofstream testfile;
	//testfile.open("testelong.out",ios::app);
	
	xsum = 0;
	ysum = 0;
	cellcnt = 0;
	//maxinertia = 0;
	//mininertia = 999999999999999999.9;
	maxelongation = 0;
	
	// find the centroid of the patch
	for(row=fminrow;row<=fmaxrow;row++) {
        for(col=fmincol;col<=fmaxcol;col++) {
			index=row*maxcol + col;
			if(fgrid2[index] == 1) {
				xsum += col;
				ysum += (maxrow - row);
				cellcnt ++;
			}
		}
	}

	xcentroid = xsum / cellcnt;
	ycentroid = ysum / cellcnt;
	
	//testfile << xcentroid << " " << ycentroid << endl;

    // loop through grid
	for(anglecnt=0; anglecnt<180; anglecnt++) {
		anglecnt2 = anglecnt + 90;
		slope1 = tan(anglecnt * 3.14159265358979323846264338327950288419716939937510582/180.0);
		slope2 = tan(anglecnt2 * 3.14159265358979323846264338327950288419716939937510582/180.0);
		intercept1 = ycentroid - slope1 * xcentroid;
		intercept2 = ycentroid - slope2 * xcentroid;
		a1 = slope1;
		b1 = -1.0;
		c1 = intercept1;
		a2 = slope2;
		b2 = -1.0;
		c2 = intercept2;		

		//testfile << a1 << " " << b1 << " " << c1 << endl;
		
		inertia1 = 0.0;
		inertia2 = 0.0;
		
		for(row=fminrow;row<=fmaxrow;row++) {
			for(col=fmincol;col<=fmaxcol;col++) {
				xcell = (double)col;
				ycell = (double)(maxrow - row);
				//index=row*maxcol + col;
				// if cell is burned, loop through neighbors
				index=row*maxcol + col;
				if(fgrid2[index] == 1) {
					//disttoline = abs((x2 - x1) * (y1 - y0) - (x1 - x0) * (y2 - y1)) / sqrt(pow((x2 - x1), 2) + pow((y2 - y1), 2));
					disttoline1 = abs(a1 * xcell - ycell + c1) / sqrt(pow(a1, 2) + pow(b1, 2));
					disttoline2 = abs(a2 * xcell - ycell + c2) / sqrt(pow(a2, 2) + pow(b2, 2));
					inertia1 += pow(disttoline1, 2);
					inertia2 += pow(disttoline2, 2);
					//testfile << xcell << " " << ycell << " " << disttoline << endl;
				}
			}
		}
		elongation = abs(inertia1 - inertia2) / (inertia1 + inertia2);
		
		//testfile << inertia << endl;
		if(elongation > maxelongation) {
			maxelongation = elongation;
		}
	}

	//elongation = (maxinertia - mininertia) / (maxinertia + mininertia);

	//testfile.close();
    return(maxelongation);

}

//----------------------------------------------------------------------------
// set_winddir - chooses random wind directions based on supplied parameters
//----------------------------------------------------------------------------
void set_winddir( double w_dir, double w_var, double w_speed )
{

    int winddir;
    int i;
    int j;
	int neighbors;
    double u0_1( void );


	winddir = rand_int(8) - 1;
		
	for(neighbors = 0; neighbors < 8; neighbors++) {
		switch(neighbors) {
			case 0: i=-1;j=-1;break;
			case 1: i=-1;j=0;break;
			case 2: i=-1;j=1;break;
			case 3: i=0;j=1;break;
			case 4: i=1;j=1;break;
			case 5: i=1;j=0;break;
			case 6: i=1;j=-1;break;
			case 7: i=-0;j=-1;break;
		}
		if (neighbors == winddir) {
			windfact[i+1][j+1] = w_speed;
		} else {
			windfact[i+1][j+1] = 1;
		}

	}

return;

}

//----------------------------------------------------------------------------
// gen_firesev - generates a baseline fire severity from a uniform distribution
//----------------------------------------------------------------------------
double gen_firesev( double sevcut1, double sevcut2 )
{
    double fsev;       // fire susceptibility vegetation modifier

    fsev = sevcut1 + u0_1() * (sevcut2 - sevcut1);

    return (fsev);

}

//----------------------------------------------------------------------------
// gen_firesize - generates a random fire size as either a normal or lognormal random variable
//----------------------------------------------------------------------------
double gen_firesize( double meansize, double stdsize, int sizedist )
{

	double fsize, mlsize, stdlsize;

	if(sizedist == 1) {
		fsize = normal_rv(meansize, stdsize);
    } else if(sizedist == 2) {
		mlsize = log( pow(meansize,2) /sqrt( pow(meansize,2) + pow(stdsize,2) ) );
		stdlsize = sqrt( log( pow(stdsize,2) / pow(meansize,2) + 1) );
		fsize = exp(normal_rv(mlsize, stdlsize));
    }
    
	fsize = floor(fsize + 0.5);

    if(fsize < 1) {
		fsize = 1;
    }

    // Fire size cannot be larger than landscape size
	if( fsize > landsize ) {
		fsize= landsize;
	}

    return (fsize);

}


//----------------------------------------------------------------------------
// get_slopefact - looks up a fire spread factor based on the supplied slope
//----------------------------------------------------------------------------
double get_slopefact( double perc_slope ) {

    double outfact;
    int classcnt;

    outfact = 1;

    for( classcnt = 0; classcnt < nslopeclass; classcnt ++) {
        if(perc_slope >= lowerslope[classcnt] && perc_slope < upperslope[classcnt]) {
            outfact = slopefact[classcnt];
        }
    }

    return( outfact );
}

