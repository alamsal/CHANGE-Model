//----------------------------------------------------------------------------
// File:		vegetation.cpp
// Author: 		Mike Wimberly
// Last Update:	1/4/2007
// Decription:	Contains functions compute forest structure classes, model
//              vegetative succession, and model disturbance effects on
//              vegetation.
//----------------------------------------------------------------------------

#pragma hdrstop
#include "fires.h"
#include "vegetation.h"
#include "randnum.h"
#include "lads.h"
#include "randnum.h"
#include "biomass.h"

//----------------------------------------------------------------------------
// get_struc - looks up the successional stage code for a given cell
//----------------------------------------------------------------------------

short int get_struc( int index )
{

    short int struc;        // structure index
    short int c_age;        // patch age
    short int c_tsfire;     // time since last fire

    // read veg information from arrays
    c_age = age[index];
    c_tsfire = tsfire[index];

    struc = 99;

    // if fire summary, then distinguish burned from unburned cells
    if( sumtype >= 3 ) {
        struc = (short int)firespreadgrid[index];
    // otherwise compute structure class based on user inputs
    } else {
        struc = stateout[comgrid[index] - 1][stategrid[index] - 1];
    }

    return( struc );

}


//----------------------------------------------------------------------------
// grow_veg - increments stand age and determines successional transitions
//----------------------------------------------------------------------------
void grow_veg( void ) {

    int index;
    double curlivewood;
    double curmort;

    // Increment the age of every cell
    for(index=0; index<size; index++) {
        if((buffer[index] > 0) && (regime[index]>0)) {					// Original: if(buffer[index] > 0) //Edited :  if((buffer[index] > 0) && (regime[index]>0))  // Date:4/15/2013
            age[index]+=(short int)runstep;
            tsfire[index]+=(short int)runstep;
            if(suclag[comgrid[index] - 1][stategrid[index] - 1] < 999) {
                timeinstage[index]+= (short int)runstep;
                // Determine if cell switches to a new successional stage
				if(timeinstage[index] >= suclag[comgrid[index] - 1][stategrid[index] - 1]) {
					if(suclagreset[comgrid[index] - 1][stategrid[index] - 1] < 999) {
						tsfire[index] = suclagreset[comgrid[index] - 1][stategrid[index] - 1];
					}                    
					stategrid[index] = suctrans[comgrid[index] - 1][stategrid[index] - 1];
                    timeinstage[index] = 0;
                }
            }
			
			// "understory" successional based on time time since last fire
			if(firexlag[comgrid[index] - 1][stategrid[index] - 1] < 999) {
				if(tsfire[index] >= firexlag[comgrid[index] - 1][stategrid[index] - 1]) {
					stategrid[index] = firextrans[comgrid[index] - 1][stategrid[index] - 1];
				}
			}

            // change in wood biomass variables
			if(biom_flag > 0) {
                bioagegrid[index] += runstep;
                curlivewood = calc_live_biomass( index );
                curmort = calc_chronic_mort( index );
                deadgrid[index] = calc_dead_decay( index );
                deadgrid[index] += curlivewood * curmort;
            }

        }
    }

    return;

}

//----------------------------------------------------------------------------
// disturb_veg - determines fire effects on vegetation
//----------------------------------------------------------------------------
void disturb_veg( int landfiresum, int sevcnt, int firereg ) {

    int index;
    double modfiresev, modfiresev2, modfiresev3;
    int landtype;
    double topoindex, topoindex2;
    double vegindex, vegindex2;
	double curfireodds, curfireodds2;
    double sevmod, sevmod2;
	double weathsevmod, weathsevmod2;
    double newbiomass;
    double oldbiomass;
    double pathpick;
    double cumprob;
    int lastfiresev;
    int curstate, newstate, newreset;

	// Compute odds ratios from base fire severity probabilities
	weathsevmod = cursevmod[firereg][sevcnt];
	weathsevmod2 = cursevmod2[firereg][sevcnt];
	
	// Initialize vegetation in the burned cells
    for( index = 0; index < size; index++ ) {
        if( fgrid2[index] >= 1 ) {
            // increment burned cells in fire summary grid
            if(regime[index] > 0 && buffer[index] == 1 && landfiresum == 1)  {
                fsum1[index]++;
            }
            // determine severity modifiers
            landtype = landgrid[index];
            topoindex = landsevmod[landgrid[index] - 1];
            topoindex2 = landsevmod2[landgrid[index] - 1];
         
			
			vegindex = statesev[comgrid[index] - 1][stategrid[index] - 1];
            vegindex2 = statesev2[comgrid[index] - 1][stategrid[index] - 1];
			curfireodds = vegindex / (1 - vegindex);
			curfireodds2 = vegindex2 / (1 - vegindex2);
			sevmod = topoindex * weathsevmod;
			sevmod2 = topoindex2 * weathsevmod2;
			
			// calculate modified severity by multiplying odds ratios by severity modifiers
			// then convert back to probabilities to get modified fire severities
			if(vegindex >= 1) {
                modfiresev = 1;
			} else {
                modfiresev = 1 / ((1 / (curfireodds * sevmod)) + 1); // p(high-severity)
			}
			if(vegindex2 >= 1) {
				modfiresev2 = 1;
			} else {
                modfiresev2 = 1 / ((1 / (curfireodds2 * sevmod2)) + 1); // p(mixed-severity | no high severity)
			}
			modfiresev3 = modfiresev + modfiresev2 * (1 - modfiresev); // p(mixed severity)

            if(biom_flag > 0) {
                oldbiomass = calc_live_biomass ( index );
            }

            if(age[index] == tsfire[index]) {
                lastfiresev = 1;
            } else {
                lastfiresev = 2;
            }

			if(fire_pat == 1) {
				pathpick = u0_1();
			} else {
				pathpick = fint[index];
			}
            cumprob = 0;
            curstate = stategrid[index];

			// Stand-replacmenet fires
			if(pathpick < modfiresev) {
                age[index] = 0;
                tsfire[index] = 0;
				//totburnarea[regime[index] - 1]++;
				hiburnarea[regime[index] - 1]++;
				
				newstate = hsfiretrans[comgrid[index] - 1][curstate - 1];
				newreset = hsfirereset[comgrid[index] - 1][curstate - 1];

				stategrid[index] = newstate;
				if(newreset == 0) {
					if(timeinstage[index] > suclag[comgrid[index] - 1][newstate - 1]) {
						timeinstage[index] = suclag[comgrid[index] - 1][newstate - 1];
					}
				} else {
					timeinstage[index] = 0;
				}

                if(biom_flag > 0) {
                    newbiomass = oldbiomass * (1 - hs_mort[ regime[index] - 1 ]);
                    bioagegrid[index] = (short int)calc_bio_age( newbiomass, index, 1 );
                    deadgrid[index] = deadgrid[index] * (1 - hs_consume[ regime[index] - 1 ]);
                    deadgrid[index] += oldbiomass * hs_mort[ regime[index] - 1 ];
                }

                // Increment fire severity summary
                if(regime[index] > 0 && buffer[index] == 1 && landfiresum == 1)  {
                    fsum2[index]++;
                }
            // "mixed-severity" fires
			} else if (pathpick < modfiresev3) {
                tsfire[index] = 0;

				//totburnarea[regime[index] - 1]++;
				miburnarea[regime[index] - 1]++;
				
				newstate = msfiretrans[comgrid[index] - 1][curstate - 1];
				newreset = msfirereset[comgrid[index] - 1][curstate - 1];
				
				stategrid[index] = newstate;

				if(newreset == 0) {
					if(timeinstage[index] > suclag[comgrid[index] - 1][newstate - 1]) {
						timeinstage[index] = suclag[comgrid[index] - 1][newstate - 1];
					}
				} else {
					timeinstage[index] = 0;
				}

                if(biom_flag > 0) {
                    newbiomass = oldbiomass * (1 - ms_mort[ regime[index] - 1 ]);
                    bioagegrid[index] = (short int)calc_bio_age( newbiomass, index, 2 );
                    deadgrid[index] = deadgrid[index] * (1 - ms_consume[ regime[index] - 1 ]);
                    deadgrid[index] += oldbiomass * ms_mort[ regime[index] - 1 ];
                }
                // Increment fire severity summary
                if(regime[index] > 0 && buffer[index] == 1 && landfiresum == 1)  {
                    fsum3[index]++;
                }

			// Non-lethal fires
			} else {
                tsfire[index] = 0;

				//totburnarea[regime[index] - 1]++;
				liburnarea[regime[index] - 1]++;
				
				newstate = lsfiretrans[comgrid[index] - 1][curstate - 1];
				newreset = lsfirereset[comgrid[index] - 1][curstate - 1];
				
				stategrid[index] = newstate;
				if(newreset == 0) {
					if(timeinstage[index] > suclag[comgrid[index] - 1][newstate - 1]) {
						timeinstage[index] = suclag[comgrid[index] - 1][newstate - 1];
					}
				} else {
					timeinstage[index] = 0;
				}

                if(biom_flag > 0) {
                    newbiomass = oldbiomass * (1 - ms_mort[ regime[index] - 1 ]);
                    bioagegrid[index] = (short int)calc_bio_age( newbiomass, index, 2 );
                    deadgrid[index] = deadgrid[index] * (1 - ms_consume[ regime[index] - 1 ]);
                    deadgrid[index] += oldbiomass * ms_mort[ regime[index] - 1 ];
                }


            }
        }  // end if cell is burned
    }

    return;

}

//----------------------------------------------------------------------------
// nsdisturb_veg - non-spatial disturbance effects on vegetation
//----------------------------------------------------------------------------
void nsdisturb_veg( int distnum ) {

	int index, curstate, curcom, nsdistcnt, newstate;
	double selectdist;

	for(index=0;index<size;index++) {
        curstate = stategrid[index];
		curcom = comgrid[index];
		if(u0_1() < anydistprob[curcom - 1][curstate - 1]) {
			selectdist = u0_1();
			for(nsdistcnt = 0; nsdistcnt < distnum; nsdistcnt++) {			
				if(selectdist < reldistprob[curcom - 1][curstate - 1][nsdistcnt]) {
					newstate = disttrans[comgrid[index] - 1][curstate - 1][nsdistcnt];
					stategrid[index] = newstate;
					if(distreset[curcom - 1][curstate - 1] == 0) {
						if(timeinstage[index] > suclag[comgrid[index] - 1][newstate - 1]) {
							timeinstage[index] = suclag[comgrid[index] - 1][newstate - 1];
						}
					} else {
						timeinstage[index] = 0;
					}
				break;
				}
			}
		}
	}

	return;
}

//----------------------------------------------------------------------------
// gentreatment - treatment effects on vegetation
//----------------------------------------------------------------------------
int gentreatment( short int treatmentunit, int treatmentclass ) {

	int index, curstate, curcom, newstate, treatedcells;
	
	treatedcells = 0;

	for(index=0;index<size;index++) {

		if(management[index] == treatmentunit) {

			curstate = stategrid[index];
			curcom = comgrid[index];

			if(treatelig[curcom - 1][curstate - 1][treatmentclass] == 1) {

				treatedcells++;
				newstate = treattrans[comgrid[index] - 1][curstate - 1][treatmentclass];
				stategrid[index] = newstate;
				if(treatreset[curcom - 1][curstate - 1][treatmentclass] == 0) {
					if(timeinstage[index] > suctrans[comgrid[index] - 1][newstate - 1]) {
						timeinstage[index] = suctrans[comgrid[index] - 1][newstate - 1];
					}
				} else {
					timeinstage[index] = 0;
				}
			}
		}
	} 

	return(treatedcells);

}