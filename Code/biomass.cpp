//----------------------------------------------------------------------------
// File:		biomass.cpp
// Author: 		Mike Wimberly
// Last Update:	8/2/2002
// Decription:	Contains functions to compute biomass response to landscape
//              change
//
//----------------------------------------------------------------------------

#pragma hdrstop
#include "fires.h"
#include "biomass.h"
#include "lads.h"
#include "randnum.h"
#include "vegetation.h"


//----------------------------------------------------------------------------
// Function:    live_biomass
// Parameters:  standage: stand age
//              cr_p1: maximum biomass value
//              cr_p2: rate parameter
//              cr_p3: shape parameter
// Returns:     live tree biomass for a stand of the given age
// Description: Computes biomass for a stand of a given age using the Chapman-
//              Richards equation
//----------------------------------------------------------------------------
double calc_live_biomass( int index ) {

    double standage;
    double standbiomass;
    double cr_p3;
    int curregime;

    standage = bioagegrid[index];
    curregime = (int)regime[index];

    if(tsfire[index] == age[index]) {
        cr_p3 = cr_p3a[curregime - 1];
    } else {
        cr_p3 = cr_p3b[curregime - 1];
    }

    standbiomass = cr_p1[curregime - 1] * pow((1 - exp( -1 * cr_p2[curregime - 1] * standage)),cr_p3);

    return standbiomass;

}

int calc_bio_age( double biomass, int index, int lastfiresev ) {

    double standage;
    int standage2;
    double cr_p3;
    int curregime;

    curregime = (int)regime[index];

    if(lastfiresev == 1) {
        cr_p3 = cr_p3a[curregime - 1];
    } else {
        cr_p3 = cr_p3b[curregime - 1];
    }

    standage = log(1 - pow(biomass/cr_p1[curregime - 1], 1/cr_p3))/(-1 * cr_p2[curregime - 1]);

    if( fmod(standage, 1) < 0.5 ) {
        standage2 =(int) floor(standage);
    } else {
        standage2 =(int) ceil(standage);
    }

    return standage2;

}

double calc_dead_decay( int index ) {

    double deadwoodt1;
    double deadwoodt2;
    double exp_decay;
    int classloop;
    int firereg;
    int curtsfire;

    deadwoodt1 = deadgrid[index];
    curtsfire = tsfire[index];
    firereg = regime[index] - 1;

    for (classloop = 0; classloop < decayrate.nclass; classloop ++) {
        if( curtsfire >= decayrate.lowerbnd[classloop] && curtsfire < decayrate.upperbnd[classloop] ) {
            exp_decay = decayrate.mort[classloop][firereg];
        }
    }

    deadwoodt2 = deadwoodt1 * exp(-1 * exp_decay * runstep);

    return (deadwoodt2);

}

double calc_chronic_mort( int index ) {

    double mortrate;
    int classloop;
    int curage;
    int curtsfire;
    int firereg;

    curage = age[index];
    curtsfire = tsfire[index];
    firereg = regime[index] - 1;

    if( curage == curtsfire ) {

        for (classloop = 0; classloop < hs_cmort.nclass; classloop ++) {
            if( curtsfire >= hs_cmort.lowerbnd[classloop] && curtsfire < hs_cmort.upperbnd[classloop] ) {
                mortrate = hs_cmort.mort[classloop][firereg];
            }
        }

    } else {

        for (classloop = 0; classloop < ms_cmort.nclass; classloop ++) {
            if( curtsfire >= ms_cmort.lowerbnd[classloop] && curtsfire < ms_cmort.upperbnd[classloop] ) {
                mortrate = ms_cmort.mort[classloop][firereg];
            }
        }

    }

    return (mortrate);

}
