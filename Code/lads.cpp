// lads.cpp : Defines the entry point for the console application.
//Main entry point of the CHANGE model and keeps track of human-natural, human dominated, vegetation dominated simulation on every time stamp 

//----------------------------------------------------------------------------
// File:		lads.cpp  (CHANGE)
// Author: 		Aashis Lamsal & Mike Wimberly
// Last Update:	6/4/2014
//----------------------------------------------------------------------------
#include "lads.h"
#include "ladsio.h"
#include "randnum.h"
#include "biomass.h"
#include "vegetation.h"
#include "fires.h"
#include "celllist.h"
#include "lcc.h"
#include "probSurface.h"
#include "errorCheck.h"
#include "demand.h"
#include "hni.h"
#include "lccres.h"
#include "harvest.h"
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include "stdafx.h"

using namespace std;

// POINTERS TO MAIN LANDSCAPE ARRAYS
//---------------------------------------------------------------------------
short int *stategrid;	// successional stage grid
char *comgrid;			// community type grid
char *lccgrid;			// lcc type grid
char *ownergrid;		//ownership type grid
char *hnigrid;			//Human natural interface grid
char *harvestgrid;	// Harvest grid
short int *timeinstage;	// time in current successional stage grid
short int *dem;			// elevation grid
short int *age;		    // patch age grid
short int *tsfire;      // time since last fire grid
char *regime;           // fire regime grid
char *buffer;           // fire buffer zone grid
char *landgrid;         // slope position
char *fgrid1;           // fire simulation grid #1
char *fgrid2;           // fire simulation grid #2
char *severitygrid;    // fire severity grid
int *specgrid;          // species metapopulation grid #1
int *specgrid2;         // species metapopulation grid #2
int *dispgrid;          // species dispersal grid
float *fsusc;           // fire susceptibility grid
float *fint;			// fire intensity grid			
short int *bioagegrid;  // "biological age" grid for log volume simulation
double *deadgrid;       // dead wood volume grid for log volume simulation
unsigned char *temp;             // temporary grid for holding classified veg data
unsigned char *temp2;            // landscape transition grid
unsigned long int *fsum1;        // summary grid for tallying fire occurence
unsigned long int *fsum2;        // summary grid for tallying fire severity (high)
unsigned long int *fsum3;		 // summary grid for tallying fire severity (mixed)
unsigned short int *processed;   // patch grid - flags cells in patch
unsigned short int *patchx;      // patch grid - x coords of patch cells
unsigned short int *patchy;      // patch grid - y coords of patch cells
unsigned long int *strucsum;     // summary grid for tallying structure locations
char *zonesumgrid;				 // zonal summary grid
char *firespreadgrid;			 // keeps track of burned areas for fire map output			


// OTHER GLOBAL VARIABLES
int biom_flag;		// flag for log biomass simulation
int burned;			// total number of cell burned by a given fire
int cellsize;		// length of one side of a cell (m)
double cell_ha;		// cell area in ha
int landsize;		// total # of cells in active landscape (no background)
int maxrow;			// max row address of grid
int maxcol;			// max row address of grid
int nodata;			// nodata value for input grids
int runstep;		// base run step (for most purposes, should be 10)
int size;			// total # of cells in landscape (includes background)
int spec_flag;		// flag for species metapopulation simulation
int spec_hab;		// species metapopulation parameter - under construction...
int sumtype;		// flag for type of fire output summary
int tsumtype;		// flag for treatment output summary
int read_map;       // read external map or generate simulated landscape
int torus;			// indicates whether fires wrap around simulation landscape edges
int mintsfire;		// number of years after burnining at which a cell can reburn
int hni_flag;		// flag - indicates whether HNI will be simulated


// Community Type/Successional Stage Parameters
// The first array index is the community type, the second array index is the successional stage
// Currently can have up to 20 community types and 20 successional stages per community
// A third array index is the treatment type of non-spatial disturbance type
// 9-21-2012: Code updated to allow 40 community types and successional stages per community
int numstate;					// Total number of successional stage (across all communities)
int largestnumstate;			// The largest number of stages/community
int numcom;						// Number of community types
int comcode[40];				// ID code for community type
int comnumstate[40];			// Numeber of successional stages in each community type
int statecode[40][40];			// ID code for successional stage
int stateout[40][40];			// Output code for successional stage
int lclustate[40][40];			// Output code for LCLU successional stage
double statefiremod[40][40];	// Fire spread modifier for successional stage
double statefireinit[40][40];	// Fire initiation modifier for successional stage
double statesev[40][40];		// Fire severity (stand replacmenent) modifier 
double statesev2[40][40];		// Fire severity (mixed severity) modifier 
int suclag[40][40];				// Lag before successional transitions for 
int suclagreset[40][40];		// Is tsfire reset after succession?
int initage[40][40];			// Default initial age 
int suctrans[40][40];			// Successional transition class 
int firexlag[40][40];			// Fire exclusion transition lag 
int firextrans[40][40];			// Fire exclusion transition class 
double distprob[40][40][5];		// Probability of non-spatial disturbance
double reldistprob[40][40][5];	// Relative probability of non-spatial disturbance
double anydistprob[40][40];		// Probability of any non-spatial disturbance occuring
int disttrans[40][40][5];		// Non-spatial disturbance transition
int distreset[40][40][5];		// Non-spatial disturbance age reset flag
int treattrans[40][40][5];		// Forest management transition
int treatreset[40][40][5];		// Forest management age reset flag
int treatelig[40][40][5];		// Forest management treatment eligibility flag
int lsfiretrans[40][40];		// Non-lethal fire transition
int msfiretrans[40][40];		// Mixed-severity fire transition
int hsfiretrans[40][40];		// Stand-replacmenet fire transition
int lsfirereset[40][40];		// Non-lethal fire are reset flag
int msfirereset[40][40];		// Mixed seerity fire age reset flag
int hsfirereset[40][40];		// Stand-replacement fire age reset flag
double cursevmod[40][40];		// Weather modifier (stand replacement)
double cursevmod2[40][40];		// Weather modifier (mixed severity)


// Landtype Parameters
// 9-21-2012: Code updated to allow up to 40 landtypes
int numland;					// Number of landtypes
int landcode[40];				// ID code for landtype
double landfiremod[40];			// Fire spread modifier for landtype
double landfireinit[40];		// Fire initiation modifier for landtype
double landsevmod[40];			// Fire severity (stand replacmenent) modifier for landtype
double landsevmod2[40];			// Fire severity (mixed severity) modifier for landtype

// Fire Parameters
// 9-21-2012: Code updated to allow up to 40 slope classes
int nslopeclass;				// Number of classes for slope modifiers
double lowerslope[40];			// Lower bounds of slope classes
double upperslope[40];			// Upper bounds of slope classes
double slopefact[40];			// Slope factor for each class
double fire_cal;				// fire spread calibration parameter #1
double fire_cal2;				// fire spread calibration parameter #2
int fire_pat;				// controls fire severity patterns
double windfact[3][3];          // wind modifier to fire susceptibility
double w_speed;                 // prevailing wind speed
double w_dir;                   // prevailing wind direction
double w_var;                   // range of variability in wind direction
double w_shift;                 // probability of wind shift during fire spread

// Wood biomass parameters
double cr_p1[40];       // Chapman-Richards parameter #1 (a)
double cr_p2[40];       // Chapman-Richards parameter #2 (b)
double cr_p3a[40];      // Chapman-Richards parameter #3 (c) - high sev fire
double cr_p3b[40];      // Chapman-Richards parameter #3 (c) - mod sev fire
double hs_mort[40];         // high-severity fire mortality (by climate zone)
double ms_mort[40];         // mod-severity fire mortality (by climate zone)
double hs_consume[40];      // high-severity fire fuel consumption (by climate zone)
double ms_consume[40];      // mod-severity fire fuel consumption (by climate zone)
struct mortality hs_cmort;  // post hs-fire chronic mortality (by time since fire)
struct mortality ms_cmort;  // post ms-fire chronic mortality (by time since fire)
struct mortality decayrate; // dead wood decay (by time since fire)

//Land cover parameters
int numlcc;					//Number of lcc types
unsigned int inlcccode[40];			//Input lcc code works upto 40 different LCC types.
unsigned int outlcccode[40];			//Output lcc code works upto 40 differtn LCC types
unsigned int lcc_flag[40];			//LCC flag- 1 simulate LCC 0 no simulate LCC

//Patch Size parameters
int pts_distanceLag[40];	 // Input patch distance lag
unsigned int pts_patchSize[40];		 // Input mean patch size 
unsigned int pts_stdDeviation[40];	 // Input patch standard deviation
unsigned int pts_patchLag[40];        // Input patch lag to structure patch shape

//probability surface paramters
int prbcnt;						// counts number of prob surfaces
int numProbsurface;				// no of prob surfaces
float probRows;					// no of prob rows
float probColumns;				// no of prob columns
float transitionThreshold[40];   // probsufaces trasition for each class	

// ownership parameters
unsigned int numOwnership;				// Number of ownership types
unsigned int ownershipCode[40];			// ID code for ownership types
unsigned int ownershipRestriction[40];			// Ownership restriction flag (1= Non-developable, 0=Developable)
string ownershipNotAllow[40];                 //Destination LCLU class(es) that not allowed to chage in restricted area

// management parameters
unsigned int harvDemand[200][500][10];    // Harvest demand in each mgmt zone
unsigned int harvmeanSize[200][500][10];      // Harvest patch size

//Human-natural paramters
int numHni;    //Number of HNI types
int hniCode[40];	//Code for HNI types
int hniRestriction[40];	// Simulation restriction on HNI - Need to think more..

//demand paramters
int numDemand;				//number of demand files
int rowDemand;              //number of demand rows
int colDemand;				//number of demand columns
int meanpatchSize;				//mean output patch size

// Summary arrays
double sum_deadwood[100];   // dead wood loading summary array
double sum_livebio[100];    // live wood loading summary array
double sum_species[100];    // species metapopulation summary array
int    zsize[100];          // sizes of landscape summary zones
double sum_array[40][100];  // forest structure summary array
double totburnarea[40];		// total area burned summary array
double liburnarea[40];		// non-lethal area burned summary array
double miburnarea[40];		// mixed-severity area burned summary array
double hiburnarea[40];		// stand-replacement area burned summary array
double totburncount[40];	// total number of fires summary array

// Number of iteraiton 
int NO_OF_ITERATION=5;

// Probabilty surfaces container
std::vector< std::vector<std::vector< float > > > probability_surfaces; //Holds the probability surfaces rasters as 3D vector
std::vector< std::vector<std::vector< int > > > demand_matrix; // Holds the demand csv files

//Harvest cells
 std::vector<lccCells> harvestCells; //Holds the eligible harvest cells

//temp grid to hold vegetation trasition flag (0- ready for trasition & 1- already changed & no trasnition)
std::vector <int> tempgridFlag,hnitempgridFlag,harvtempgridFlag;

//log file
ofstream writelog;



int main( int argc, char *argv[] ) {
	
	char runname[80];       // run name
	char infilename[80];    // main input file name
	char landfilename[80];  // landtype input file name
	char comfilename[80];	// community type input file name
	char firefilename[80];  // fire regime input file name
	char outfilename[80];   // output file name (fire summary)
	char outfilename2[80];  // output file name (landscape summary)
	char outfilename3[80];  // output file name (treatment summary)
	char harvfilename[80];	// input file name (treatment priority)
	char lccfilename[80];	// input file name (land cover class)
	char prbfilename[80];   // input prob parameters file name( probability surface parameters) 
	char dmdfilename[80];	// input demand parameters file (FORESCE demands )
	char ptsizefilename[80]; // input patch size parameters file
	char ownwershipfilename[80]; // input ownwership paramters file
	char mgmtfilename[80];   //input management parameters file
	char hnifilename[80];   // input hni paramters filename
	char yearstr[10];       // current year

	int init_state[40];		// default initial state for each community type
	int input_error_flag;	// input files error checking flag
	int burnin;      	    // number of years burn-in time
	int maxyear;    	    // max year in the simulation
	int step;        	    // time step between summaries
	int runtype;            // type of output from run
	int landfiresum;        // input flag for landscape fire summary
	int landstrusum;        // input flag for landscape structure summary
	int snapsum;            // input flag for landscape snapshot output
	int regcnt;             // fire regime counter
	int sevcnt;				// severity class counter
	int zonecnt;            // summary zone counter
	int statecnt;			// summary state counter
	int landcnt;			// landtype counter
	int comcnt;				// community counter
	int classcnt;           // summary zone counter
	int treatcnt;			// treatment counter
	int biosum;             // biomass loading flag
	int year;               // simulation year
	int fperim;             // fire perimeter
	int nfires;			    // number of fires in a given year
	double nfires_f;		// number of fires in a given year, generated from normal dist
	int fire;			    // fire loop counter
	int next_sum;		    // next summary year
	int index;              // cell index - unique value for each cell
	int is_bdin;            // flag indicating when landscape is "burned in"
	int islands;            // tallys number of interior fire cells
	int shapetype;          // flag for normal vs. lognormal fire size dist
	int endy;               // end year for the current parameter set
	int regnum;             // number of disturbance regimes
	int simfire_flag;		// flag - indicates whether fires will be simulated
	int simharv_flag;		// flag - indicates whether management will be simulated
	int distnum;			// number of non-spatial disturbance types
	int mgmtnum;			// number of treatment types
	int munitnum[5];		// number of units listed for each treatment type
	int maxtreat[5];		// maximum number of units treated during each run step
	int cumtreat[5];		// cumulative treatment area
	int curtreat;			// current treatment loop in treatment loop
	int treatunit;			// current treatment unit in treatment loop
	int initindex;			// initial treatment unit
	int unitcounter[5];		// treatment unit counter
	int unitcnt;			// counts number of units treated
	int lcccnt;				// counts number of lcc types
	int demperiod;          // counts demand period
	int harvperiod;           // coutns harvesting demand period
	unsigned int nowner;	 // counts number of ownwership types
	unsigned int harvzone;   //harvest zone counter
	unsigned int harvtype;    //harvest type counter
	int simfire;             // fire simulation counter
	int numsimfire;          //No of fire simulation

	// 9-21-2012: Code updated to allow up to 40 fire regimes
	double fsize;           // fire size
	double nfr[40][10];         // natural fire rotation
	double mff[40][10];			// mean number of fires/year
	double mfsize[40][10];      // mean fire size (in cells)
	double sdfsize[40][10];     // standard devlation of fire size (in cells)
	double mfsizeha[40][10];    // mean fire size (in ha)
	double sdfsizeha[40][10];   // standard devlation of fire size (in ha)
	double sevlow[40][10];		// lower severity cutoff - stand replacement/non stand replacement
	double sevhigh[40][10];		// upper severity cutoff - stand replacement/non stand replacement
	double sev2low[40][10];		// lower severity cutoff - mixed/non lethal
	double sev2high[40][10];	// upper severity cutoff - mixed/non lethal
	double mlsize[40][10];      // mean fire size (natural log cells)
	double stdlsize[40][10];    // standard devlation of fire size (natural log cells)
	double rsize[40];       // size of each disturbance regime
	double curfiresev;      // severity of current fire (probability of stand-replacement fire)
	double curfiresev2;		// severity of current fire (probability of mixed fire if non stand replacement)
	double curmixedfire;	// severity of current fire (probability of mixed severity)
	double curlowfire;		// severity of current fire (probability of non lethal)
	double felong;			// elongation index of current fire
	double meanfireout;		// mean fire size output
	double tempprob;		// temp variable for non-spatial disturbance probability calculation
	double totprob;			// total of non-spatial disturbance probabilities
	double cumprob;			// cumulative non-spatial disturbance probability

	//6/5/2013- Aashis
	unsigned int com_counter=0;		//Total # of communties coutner
	unsigned int com_stateout[255];	//Community o/p codes container
	unsigned int com_lclustate[255]; //Community LCLU codes container


	struct image_header buffer_head;    // ERDAS file header for buffer grid
	struct image_header regime_head;    // ERDAS file header for regime grid
	struct image_header land_head;      // ERDAS file head for topo grid
	struct image_header dem_head;		// ERDAS file header for dem grid
	struct image_header com_head;       // ERDAS file head for age grid
	struct image_header lcc_head;		// ERDAS file head for lcc grid
	struct image_header hni_head;		// ERDAS file head for hni grid
	struct image_header owner_head;		// ERDAS file head for owner grid
	struct image_header state_head;		// ERDAS file header for successional stage grid
	struct image_header bioage_head;    // ERDAS file head for bioage grid
	struct image_header dead_head;      // ERDAS file head for dead grid
	struct image_header age_head;		// ERDAS file header for forest age grid
	struct image_header tsfire_head;	// ERDAS file header for time since fire grid
	
	//Check log exists
	checkLogExists("log.txt");
	// Read input file name from the command line
	strcpy(runname, argv[1]);
	strcpy(infilename, argv[1]);
	strcat(infilename, ".in"); //Main paramters file
	strcpy(comfilename, runname);
	strcat(comfilename, ".ctp"); // Community paramters file
	strcpy(landfilename, runname);
	strcat(landfilename, ".lnd");	// Land type  paramters file
	strcpy(firefilename, runname);
	strcat(firefilename, ".fre");  // Fire paramters file
	strcpy(lccfilename, runname);
	strcat(lccfilename, ".lcc");	// Land-cover paramters file
	strcpy(prbfilename,runname);
	strcat(prbfilename,".prb");		// Probability parameters file
	strcpy(dmdfilename,runname);
	strcat(dmdfilename,".dmd");		// Demand paramters file
	strcpy(ptsizefilename,runname);
	strcat(ptsizefilename,".pts");  // Patch size distribution paraters file
	strcpy(ownwershipfilename,runname); //Ownership parameters file
	strcat(ownwershipfilename,".own");
	
	strcpy(hnifilename,runname);    //HNI parameter file
	strcat(hnifilename,".hni");
	
	strcpy(mgmtfilename,runname);    //Mgmt parameter file
	strcat(mgmtfilename,".mgmt");


	//Check parameter files existance
	input_error_flag=0; //Check file input error if flag=1
	if(input_error_flag==1)
	{
		ErrorCheck check;
		check.check_file_exits(infilename, comfilename,landfilename,firefilename, harvfilename,ptsizefilename);

	}
	{
		cout<<"input_error_flag=0 =>> Error checking is disabled"<<endl;

	}

	// Open input files and read scenario parameters
	ifstream infile;
	infile.open(infilename);

	ifstream comfile;
	comfile.open(comfilename);

	ifstream landfile;
	ifstream infirefile;
	ifstream inharvfile;
	ifstream inlccfile;
	ifstream inprbfile;
	ifstream indmdfile;
	ifstream inptsizefile;
	ifstream inownershipfile;
	ifstream inmgmtfile;

	// Read main input file
	infile >> cellsize; infile.ignore(100, '\n');
	infile >> read_map; infile.ignore(100, '\n');
	infile >> maxrow; infile.ignore(100, '\n');
	infile >> maxcol; infile.ignore(100, '\n');
	infile >> simfire_flag; infile.ignore(100, '\n');
	infile >> simharv_flag; infile.ignore(100, '\n');
	infile >> burnin; infile.ignore(100, '\n');
	infile >> maxyear; infile.ignore(100, '\n');
	infile >> runstep; infile.ignore(100, '\n');
	infile >> step; infile.ignore(100, '\n');
	infile >> runtype; infile.ignore(100, '\n');//Vegetation summary flag
	infile >> sumtype; infile.ignore(100, '\n'); //Fire summary flag
	infile >> tsumtype; infile.ignore(100, '\n'); //Harvest summary flag
	infile >> snapsum; infile.ignore(100, '\n');	// Landscape map flag
	infile >> landfiresum; infile.ignore(100, '\n'); //Landscape fire summary
	infile >> landstrusum; infile.ignore(100, '\n'); //Landscape structure summary flag
	infile >> biom_flag; infile.ignore(100, '\n');	 //Biomass flag
	infile >> distnum; infile.ignore(100, '\n');	//# of non spatial disturbances
	infile >> mgmtnum; infile.ignore(100, '\n');	//# of treatments
	infile>> hni_flag;infile.ignore(100,'\n');
	infile>>numsimfire;infile.ignore(100,'\n');
	infile.close();

	//Check input file parameters
	if(input_error_flag==1)
	{
		ErrorCheck check;
		check.check_input_param(cellsize,read_map, maxrow,maxcol,simfire_flag,simharv_flag,burnin,maxyear,runstep,step,runtype,sumtype,tsumtype,snapsum,landfiresum,landstrusum,biom_flag,distnum,mgmtnum);

	}

	// Read community input file
	numstate = 0;
	largestnumstate = 0;
	comfile >> numcom; comfile.ignore(100, '\n');
	for(comcnt = 0; comcnt < numcom; comcnt++) {
		comfile >> comcode[comcnt]; comfile.ignore(100, '\n');
		comfile >> comnumstate[comcnt]; comfile.ignore(100, '\n');
		comfile >> init_state[comcnt]; comfile.ignore(100, '\n');
		for(statecnt = 0; statecnt < comnumstate[comcnt]; statecnt++) {
			numstate ++;
			comfile >> statecode[comcnt][statecnt]; comfile.ignore(100, '\n');
			if(statecode[comcnt][statecnt] > largestnumstate) {
				largestnumstate = statecode[comcnt][statecnt];
			}

			comfile >> stateout[comcnt][statecnt]; comfile.ignore(100, '\n');	
			com_stateout[com_counter]=stateout[comcnt][statecnt];		

			comfile >> lclustate[comcnt][statecnt]; comfile.ignore(100, '\n');						// 6/5/2013 - recently added LCLU code
			com_lclustate[com_counter]=lclustate[comcnt][statecnt];
			com_counter++;
			cout<<stateout[comcnt][statecnt]<<"\t" <<lclustate[comcnt][statecnt]<<"\t" << comcnt <<"\t" <<statecnt<<endl;

			comfile >> statefiremod[comcnt][statecnt]; comfile.ignore(100, '\n');
			comfile >> statefireinit[comcnt][statecnt]; comfile.ignore(100, '\n');
			comfile >> statesev[comcnt][statecnt]; comfile.ignore(100, '\n');
			comfile >> statesev2[comcnt][statecnt]; comfile.ignore(100, '\n');
			comfile >> initage[comcnt][statecnt]; comfile.ignore(100, '\n');
			comfile >> suclag[comcnt][statecnt]; comfile.ignore(100, '\n');
			comfile >> suctrans[comcnt][statecnt]; comfile.ignore(100, '\n');
			comfile >> suclagreset[comcnt][statecnt]; comfile.ignore(100, '\n');
			comfile >> firexlag[comcnt][statecnt]; comfile.ignore(100, '\n');
			comfile >> firextrans[comcnt][statecnt]; comfile.ignore(100, '\n');
			comfile >> hsfiretrans[comcnt][statecnt]; 
			comfile >> hsfirereset[comcnt][statecnt]; comfile.ignore(100, '\n');
			comfile >> msfiretrans[comcnt][statecnt]; 
			comfile >> msfirereset[comcnt][statecnt]; comfile.ignore(100, '\n');
			comfile >> lsfiretrans[comcnt][statecnt]; 
			comfile >> lsfirereset[comcnt][statecnt]; comfile.ignore(100, '\n');
			for(classcnt = 0; classcnt < mgmtnum; classcnt ++) {
				comfile >> treatelig[comcnt][statecnt][classcnt];
				comfile >> treattrans[comcnt][statecnt][classcnt];
				comfile >> treatreset[comcnt][statecnt][classcnt]; comfile.ignore(100, '\n');
			}
			for(classcnt = 0; classcnt < distnum; classcnt ++) {
				comfile >> distprob[comcnt][statecnt][classcnt];
				comfile >> disttrans[comcnt][statecnt][classcnt]; 
				comfile >> distreset[comcnt][statecnt][classcnt];comfile.ignore(100, '\n');
			}
		}
	}
	comfile.close();

	//	//Check community type input file parameters
	if(input_error_flag==1)
	{
		//Need to add lclustate as parameter to check
		ErrorCheck check;
		check.check_communityType_param(numcom,comcode,comnumstate,init_state,statecode,stateout,statefiremod,statefireinit,statesev,statesev2,initage,suclag,suctrans,suclagreset,firexlag,
			firextrans,hsfiretrans,hsfirereset,msfiretrans,msfirereset,lsfiretrans,lsfirereset,treatelig,treattrans,treatreset,distprob,disttrans,distreset,distnum,mgmtnum);

	}


	for(comcnt = 0; comcnt < numcom; comcnt++) {
		for(statecnt = 0; statecnt < comnumstate[comcnt]; statecnt ++) {
			totprob = 0;
			tempprob = 1;
			for(classcnt = 0; classcnt < distnum; classcnt ++) {
				totprob = totprob + distprob[comcnt][statecnt][classcnt];
				tempprob = tempprob * (1 - distprob[comcnt][statecnt][classcnt]);
			}
			anydistprob[comcnt][statecnt] = 1 - tempprob;
			cumprob = 0;
			for(classcnt = 0; classcnt < distnum; classcnt ++) {
				cumprob = cumprob + distprob[comcnt][statecnt][classcnt];
				reldistprob[comcnt][statecnt][classcnt] = cumprob;
			}
		}
	}

	// Only read fire regime and landtype intput files if we're simulating fire 
	if(simfire_flag == 1) {

		landfile.open(landfilename);
		infirefile.open(firefilename);
	}

	// Read fire regime input file
	if( simfire_flag == 1 ) {
		infirefile >> regnum; infirefile.ignore(100, '\n');
		infirefile >> shapetype; infirefile.ignore(100, '\n');
		infirefile >> w_dir;infirefile.ignore(100, '\n');
		infirefile >> w_var; infirefile.ignore(100, '\n');
		infirefile >> w_speed;infirefile.ignore(100, '\n');
		infirefile >> w_shift;infirefile.ignore(100, '\n');
		infirefile >> fire_cal; infirefile.ignore(100, '\n');
		infirefile >> fire_cal2; infirefile.ignore(100, '\n');
		infirefile >> fire_pat; infirefile.ignore(100, '\n');
		infirefile >> torus; infirefile.ignore(100, '\n');
		infirefile >> mintsfire; infirefile.ignore(100, '\n');
		infirefile >> nslopeclass; infirefile.ignore(100, '\n');
		for(classcnt = 0; classcnt < nslopeclass; classcnt ++) {
			infirefile >> lowerslope[classcnt];
			infirefile >> upperslope[classcnt];
			infirefile >> slopefact[classcnt];
			infirefile.ignore(100, '\n');
		}

	}

	//Check fire regime file parameters
	if(input_error_flag==1)
	{
		ErrorCheck check;
		check.check_fireRegim_param(regnum,shapetype,w_dir,w_var,w_speed,w_shift,fire_cal,fire_cal2,fire_pat,torus,mintsfire,nslopeclass,runstep);

	}

	// Read landtype input file
	if( simfire_flag == 1 ) {
		landfile >> numland; landfile.ignore(100,'\n');
		for(landcnt = 0; landcnt < numland; landcnt ++) {
			landfile >> landcode[landcnt]; landfile.ignore(100, '\n');
			landfile >> landfiremod[landcnt]; landfile.ignore(100, '\n');
			landfile >> landfireinit[landcnt]; landfile.ignore(100, '\n');
			landfile >> landsevmod[landcnt]; landfile.ignore(100, '\n');
			landfile >> landsevmod2[landcnt]; landfile.ignore(100, '\n');
		}
		landfile.close();
		//Check land type file parameters
		if(input_error_flag==1)
		{
			ErrorCheck check;
			check.check_landType_param(numland,landcode,landfiremod,landfireinit,landsevmod,landsevmod2);

		}
	}

	// Read lcc inputfile -Modified by Ashis 12/12/2012
	inlccfile.open(lccfilename);
	//inlccfile.ignore(100,'\n'); //Skip to the new line
	inlccfile>>numlcc; inlccfile.ignore(100,'\n');
	cout<<"Total # of LCC class descriptionin parameter file: "<<numlcc<<endl;
	//inlccfile.ignore(100,'\n'); //Skip to the new line
	std::cout<<"Input LCC class Code \t Output LCC class code \t Simulation Flag"<<endl;
	for(lcccnt=0;lcccnt<numlcc;lcccnt++)
	{
		inlccfile>>inlcccode[lcccnt]>>outlcccode[lcccnt]>>lcc_flag[lcccnt];inlccfile.ignore(100,'\n');			
		std::cout<<inlcccode[lcccnt]<<"\t"<< outlcccode[lcccnt]<<"\t"<<lcc_flag[lcccnt] <<endl; 

	}
	inlccfile.close();
	//Read demand paramter file
	indmdfile.open(dmdfilename);
	indmdfile>>numDemand;indmdfile.ignore(100,'\n');
	indmdfile>>rowDemand;indmdfile.ignore(100,'\n');
	indmdfile>>colDemand;indmdfile.ignore(100,'\n');
	indmdfile.close();


	//Read ownwership paramter file
	inownershipfile.open(ownwershipfilename);
	inownershipfile>>numOwnership; inownershipfile.ignore(100,'\n');
	std::cout<< "Total # of ownership layers: "<<numOwnership<<endl;
	inownershipfile.ignore(100,'\n');// Skip to the new line
	std::cout<<"Code \t Restricion \t Allowed Class(es)"<<endl;
	for(nowner=0;nowner<numOwnership; nowner++)
	{
		inownershipfile>>ownershipCode[nowner]>>ownershipRestriction[nowner]>>ownershipNotAllow[nowner];inownershipfile.ignore(100,'\n');
		std::cout<<ownershipCode[nowner]<<"\t"<< ownershipRestriction[nowner]<<"\t"<<ownershipNotAllow[nowner]<<endl; 		
	}
	inownershipfile.close();

	//Read management/harvest parameters
	inmgmtfile.open(mgmtfilename);

	for(demperiod=0;demperiod<numDemand; demperiod++)
	{   
		inmgmtfile.ignore(100,'\n');
		
		for (harvzone=0;harvzone<numOwnership;harvzone++)
		{
			inmgmtfile.ignore(100,'\n');


			for(harvtype=0;harvtype<mgmtnum;harvtype++)
			{
				 inmgmtfile>>harvmeanSize[demperiod][harvzone][harvtype];
				 cout<< "Dem: "<<demperiod <<"\t"<<"Hzone: "<<harvzone <<"\t"<<"Htype: "<<harvtype <<"\t"<< "HSize" <<harvmeanSize[demperiod][harvzone][harvtype]<<endl;

			}
			inmgmtfile.ignore(100,'\n');

			for(harvtype=0;harvtype<mgmtnum;harvtype++)
			{
				 inmgmtfile>>harvDemand[demperiod][harvzone][harvtype];
				 cout<< "Dem: "<<demperiod <<"\t"<<"Hzone: "<<harvzone <<"\t"<<"Htype: "<<harvtype <<"\t"<<"HDemand"<<harvDemand[demperiod][harvzone][harvtype]<<endl;

			}
			inmgmtfile.ignore(100,'\n');
		}
	 
	}
	inmgmtfile.close();


	//Read probability sufrace paramter file
	inprbfile.open(prbfilename);
	inprbfile>>numProbsurface;inprbfile.ignore(100,'\n');
	probRows=maxrow;
	probColumns=maxcol;


	for(prbcnt=0;prbcnt<numProbsurface;prbcnt++)
	{
		inprbfile>>transitionThreshold[prbcnt];inprbfile.ignore(100,'\n');
		cout<<transitionThreshold[prbcnt]<<endl;
	}

	inprbfile.close();
	
	//Read patch size paramter file
	inptsizefile.open(ptsizefilename);
	//inptsizefile.ignore(100,'\n'); //Skip to the new line
	std::cout<<"Patch lag\ Distance Lag \t Mean Patch Size \t SDT Deviation"<<endl;
	for(lcccnt=0;lcccnt<numProbsurface;lcccnt++)
	{
		inptsizefile>>pts_patchLag[lcccnt]>>pts_distanceLag[lcccnt]>>pts_patchSize[lcccnt]>>pts_stdDeviation[lcccnt];inptsizefile.ignore(100,'\n');			
		std::cout<<pts_patchLag[lcccnt]<<"\t"<<pts_distanceLag[lcccnt]<<"\t"<< pts_patchSize[lcccnt]<<"\t"<<pts_stdDeviation[lcccnt] <<endl; 

	}
	inptsizefile.close();

	// Crreate or resize  the probability surface vector size to make dynamic  size
	probability_surfaces.resize(numProbsurface);
	for(unsigned int i=0;i<numProbsurface;i++)
	{
		probability_surfaces[i].resize(probRows);
		for(unsigned int j=0;j<probRows;j++)
		{
			probability_surfaces[i][j].resize(probColumns);
		}
	}
	// Read probability surfaces raster files
	read_probabilitySurfaces(probability_surfaces,numProbsurface,probRows,probColumns);
	//probability_surfaces;


	// Only read wood biomass intput file if we're simulating biomass
	biosum = 0;
	if(biom_flag == 1) {
		ifstream infile3;
		infile3.open("biomass.in");

		infile3 >> biosum; infile3.ignore(100, '\n');
		for(regcnt = 0; regcnt < regnum; regcnt ++) {
			infile3 >> cr_p1[regcnt];
		}
		infile3.ignore(100, '\n');
		for(regcnt = 0; regcnt < regnum; regcnt ++) {
			infile3 >> cr_p2[regcnt];
		}
		infile3.ignore(100, '\n');
		for(regcnt = 0; regcnt < regnum; regcnt ++) {
			infile3 >> cr_p3a[regcnt];
		}
		infile3.ignore(100, '\n');
		for(regcnt = 0; regcnt < regnum; regcnt ++) {
			infile3 >> cr_p3b[regcnt];
		}
		infile3.ignore(100, '\n');
		for(regcnt = 0; regcnt < regnum; regcnt ++) {
			infile3 >> hs_mort[regcnt];
		}
		infile3.ignore(100, '\n');
		for(regcnt = 0; regcnt < regnum; regcnt ++) {
			infile3 >> ms_mort[regcnt];
		}
		infile3.ignore(100, '\n');
		for(regcnt = 0; regcnt < regnum; regcnt ++) {
			infile3 >> hs_consume[regcnt];
		}
		infile3.ignore(100, '\n');
		for(regcnt = 0; regcnt < regnum; regcnt ++) {
			infile3 >> ms_consume[regcnt];
		}
		infile3.ignore(100, '\n');
		infile3 >> decayrate.nclass; infile3.ignore(100, '\n');
		for(classcnt = 0; classcnt < decayrate.nclass; classcnt ++) {
			infile3 >> decayrate.lowerbnd[classcnt];
			infile3 >> decayrate.upperbnd[classcnt];
			for(regcnt = 0; regcnt < regnum; regcnt ++) {
				infile3 >> decayrate.mort[classcnt][regcnt];
			}
			infile3.ignore(100, '\n');
		}
		infile3 >> hs_cmort.nclass; infile3.ignore(100, '\n');
		for(classcnt = 0; classcnt < hs_cmort.nclass; classcnt ++) {
			infile3 >> hs_cmort.lowerbnd[classcnt];
			infile3 >> hs_cmort.upperbnd[classcnt];
			for(regcnt = 0; regcnt < regnum; regcnt ++) {
				infile3 >> hs_cmort.mort[classcnt][regcnt];
			}
			infile3.ignore(100, '\n');
		}
		infile3 >> ms_cmort.nclass; infile3.ignore(100, '\n');
		for(classcnt = 0; classcnt < ms_cmort.nclass; classcnt ++) {
			infile3 >> ms_cmort.lowerbnd[classcnt];
			infile3 >> ms_cmort.upperbnd[classcnt];
			for(regcnt = 0; regcnt < regnum; regcnt ++) {
				infile3 >> ms_cmort.mort[classcnt][regcnt];
			}
			infile3.ignore(100, '\n');
		}

	}

	// Basic raster computations - cell size and number of cells
	cell_ha = pow((double)cellsize, 2) / 10000;
	size = maxrow * maxcol;
	


	// Setup outputfiles
	strcpy(outfilename, argv[1]);
	strcpy(outfilename2, argv[1]);
	strcpy(outfilename3, argv[1]);
	// Fire summary output file
	ofstream outfire;
	if(simfire_flag == 1) {
		if(sumtype == 2 || sumtype == 4) {
			outfire.open("firesum.fir", ios::app);
		} else if(sumtype == 1 || sumtype == 3) {
			outfire.open(strcat(outfilename, ".fir"));
		}
	}

	// Vegetation summary output file
	ofstream outsum;
	if(runtype == 1) {
		outsum.open(strcat(outfilename2, ".sum"));
	} else {
		outsum.open("ladsout.sum", ios::app);
	}

	// Allocate memory for arrays
	dem = new short int[size];
	stategrid = new short int[size];
	comgrid = new char[size];
	lccgrid= new char[size];
	ownergrid= new char[size];
	hnigrid=new char[size];
	timeinstage = new short int[size];
	age = new short int[size];
	tsfire = new short int[size];
	regime = new char[size];

	buffer = new char[size];
	fgrid1 = new char[size];
	landgrid = new char[size];
	fgrid2 = new char[size];
	fsusc = new float[size];
	temp = new unsigned char[size];
	temp2 = new unsigned char[size];
	fint = new float[size];
	severitygrid= new char[size];
	harvestgrid=new char[size];

	//Fill severity grid with 0
	//std::fill(severitygrid,severitygrid+size, static_cast<short int>(0));
	if(landfiresum == 1) {
		fsum1 = new unsigned long int[size];
		fsum2 = new unsigned long int[size];
		fsum3 = new unsigned long int[size];
	}

	if(sumtype > 0) {
		firespreadgrid = new char[size];
		processed = new unsigned short int[size];
		patchx = new unsigned short int[size];
		patchy = new unsigned short int[size];
	}

	if(landstrusum == 1) {
		strucsum = new unsigned long int[size * largestnumstate];
	}

	if(biom_flag > 0) {
		bioagegrid = new short int[size];
		deadgrid = new double[size];
	}

	if(runtype > 0) {
		zonesumgrid = new char[size];
	}

	// initialize random number generator
	sgenrand(time((unsigned long)(time_t)0));

	// setup state variable grids
	// either read from input files
	if( read_map > 0 ) 
	{
		buffer_head = read_grid("buffer", buffer);
		if( simfire_flag == 1 ) {
			regime_head = read_grid("regime", regime);
			dem_head = read_16bit_grid("elevation", dem);
			//Compare # of fire regime class in regime grid with runname.fre file
			if(input_error_flag==1)
			{
				int no_fire_regime;  // Number of fire regime
				ErrorCheck ec;
				no_fire_regime=ec.get_grid_class(regime,size);
				if(no_fire_regime==regnum)
				{
					std::cout<<"# of Fire regime class \t OK"<<endl;
				}
				else
				{
					std::cout<<"@@@ ERROR:Number of fire regime is mismatched between runname.fre & regime.gis file"<<endl;
					cin.get();
					exit(1);
				}
			}
		}

		// Read grid landtype.gis
		land_head = read_grid("landtype", landgrid);
		//Compare # of class in landtype grid with runname.lnd file
		if(input_error_flag==1)
		{
			int no_landtype; // Number of landtypes
			ErrorCheck ec;
			no_landtype=ec.get_grid_class(landgrid,size);
			if(no_landtype==numland)
			{
				std::cout<<"# of Landtype class \t OK"<<endl;
			}
			else
			{
				std::cout<<"@@@ ERROR:Number of landtypes is mismatched between runname.lnd & landtype.gis file"<<endl;
				cin.get();
				exit(1);
			}

		}
		//Read grid ownership.gis
		owner_head=read_grid("ownership",ownergrid);
		
		//Read grid hni.gis
		if(hni_flag==1)
		{
			hni_head=read_grid("inithni",hnigrid);
		}
		
		// Read grid community.gis
		com_head = read_grid("community", comgrid);  
		
		//Compare # of class in community grid with runname.ctp file
		if(input_error_flag==1)
		{
			int no_community; // Number of community
			ErrorCheck ec;
			no_community=ec.get_grid_class(comgrid,size);
			if(no_community==numcom)
			{
				std::cout<<"# of Community class \t OK"<<endl;
			}
			else
			{
				std::cout<<"@@@ ERROR:Number of communities is mismatched between runname.ctp & community.gis file"<<endl;
				cin.get();
				exit(1);
			}

		}
		//Read grid lcc.gis
		lcc_head= read_grid("initlclu",lccgrid);			 
		//Compare # of class in lcc grid with runname.lcc file
		if(input_error_flag==1)
		{
			int no_lcc; // Number of lcc class
			ErrorCheck ec;
			no_lcc=ec.get_grid_class(lccgrid,size);
			if(no_lcc<= numlcc)
			{
				std::cout<<"# of LCC class in *.GIS \t"<<no_lcc <<endl;
			}
			else
			{
				std::cout<<"@@@ ERROR:Number of communities is mismatched between runname.lcc & lcc.gis file "<<endl;
				cin.get();
				exit(1);
			}

		}


		// or generate homogeneous grids
		// not sure if this still works - do I need to add a generator for the elevation grid?
	} 
	else 
	{
		buffer_head = gen_grid(maxrow, maxcol, 1, buffer);
		if (simfire_flag == 1) {
			regime_head = gen_grid(maxrow, maxcol, 1, regime);
		}
		land_head = gen_grid(maxrow, maxcol, 1, landgrid);
		com_head = gen_grid(maxrow, maxcol, 1, comgrid);
	}

	// set size counters to zero
	landsize=0;

	if( simfire_flag == 1) {
		for(regcnt = 0; regcnt < regnum; regcnt ++) {
			rsize[regcnt] = 0;
		}
	}


	// Note that zsize contains the sizes of the communities - since these are the summary zones
	for(zonecnt = 0; zonecnt < numcom; zonecnt ++) {
		zsize[zonecnt] = 0;
	}



	// compute the size of the active landscape, fire regime zones, and summary zones
	for(index=0; index<size; index++) {
		if(buffer[index] > 0)  {
			landsize ++;
			if( simfire_flag == 1) {
				if( regime[index] > 0 ) {
					rsize[regime[index] - 1]++;
				}
			}
			if( read_map > 0 && runtype > 0 ) {
				// 9/24/2012 - Added comgrid condition to handle problem w/ Erik and Rob's input files
				if( buffer[index] == 1 && comgrid[index] > 0 ) {
					zsize[comgrid[index] - 1] += 1;
				}
			} else {
				if( buffer[index] == 1 ) {
					zsize[0] += 1;
				}
			}
		}
	}

	// If read map flag is set, read initial landscape configurations from input files
	if( read_map > 1) 
	{
		state_head = read_veg_grid("initstate", stategrid);
		age_head = read_veg2_grid("initage", age, runstep);
		tsfire_head = read_veg2_grid("inittsfire", tsfire, runstep);
		if(biom_flag > 0) {
			bioage_head = read_veg2_grid("initbioage", bioagegrid, runstep);
			dead_head = read_veg3_grid("initdead", deadgrid, runstep);
		}
		// "fix" ages and tsfire values so they are within the possible range for each stage


		for(index=0; index<size; index++) {
			if(age[index] < initage[comgrid[index] - 1][stategrid[index] - 1]) 
			{
				age[index] = initage[comgrid[index] - 1][stategrid[index] - 1];
				if(biom_flag > 0) {
					bioagegrid[index] = age[index];
				}
			} 
			else if(age[index] >= initage[comgrid[index] - 1][stategrid[index] - 1] + suclag[comgrid[index] - 1][stategrid[index] - 1]) 
			{
				age[index] = initage[comgrid[index] - 1][stategrid[index] - 1] + suclag[comgrid[index] - 1][stategrid[index] - 1] - runstep;
				if(biom_flag > 0) {
					bioagegrid[index] = age[index];
				}
			}
			timeinstage[index] = age[index] - initage[comgrid[index] - 1][stategrid[index] - 1];
		}



		// otherwise use default initial stage for each community type
	} 
	else 
	{
		for(index=0; index<size; index++) {
			if((buffer[index] > 0) && (regime[index]>0)) {
				stategrid[index] = (short int)init_state[comgrid[index] - 1];
				timeinstage[index] = 0;
				age[index] = initage[comgrid[index] - 1][stategrid[index] - 1];
				tsfire[index] = 0;
				if(biom_flag > 0) {
					bioagegrid[index] = age[index];
					deadgrid[index] = 0;
				}
			}
		}
	}

	// initialize summary grids
	for(index=0; index<size; index++) {
		if((buffer[index] > 0) && (regime[index]>0)) {
			temp[index] = get_struc(index);
			//         if(temp[index]==0)
			//{
			//             cout << "come here"<<endl;
			//}
			if(landfiresum == 1) {
				fsum1[index] = 0;
				fsum2[index] = 0;
				fsum3[index] = 0;
			}
			if(landstrusum == 1) {
				for(regcnt=0; regcnt < largestnumstate; regcnt++) {
					strucsum[index + regcnt * size] = 0;
				}
			}
		}
		severitygrid[index]=0;
		harvestgrid[index]=0;
	}

	// initialize summary arrays
	for(regcnt = 0; regcnt < regnum; regcnt ++) {
		totburnarea[regcnt] = 0;
		liburnarea[regcnt] = 0;
		miburnarea[regcnt] = 0;
		hiburnarea[regcnt] = 0;
		totburncount[regcnt] = 0;
	}

	is_bdin = 0;
	next_sum = step;
	endy = 0;
	//gen_forescesnapshot(runname, 50, buffer_head, snapsum, 0);
	//Integrating buffer with LCC type added by Ashis 12/18/2012
	
	// Create or resize  the demand matrix grid to make dynamic  size
	demand_matrix.resize(numDemand);
	for(unsigned int i=0;i<numDemand;i++)
	{
		demand_matrix[i].resize(rowDemand);
		for(unsigned int j=0;j<rowDemand;j++)
		{
			demand_matrix[i][j].resize(colDemand);
		}
	}
	cout<<"Reading demand files (csvs)..."<<endl;
	read_demandCsv(demand_matrix,numDemand,rowDemand,colDemand);
	demand_matrix;	
	


	//Loop through the demand years
	for(demperiod=0;demperiod<numDemand;demperiod++)
	{
		harvtempgridFlag.clear();
		harvtempgridFlag.resize(size,0);


		writelog<<"START DEMAND PERIOD #"<<demperiod<<" ****************************************************************************************************************************"<<endl;
		
		
		
		
		//HUMAN DOMINATED LCLU CHANGE- FORESCE REGION

		tempgridFlag.clear();
		tempgridFlag.resize(size,0);
		
		//Implement HNI algorithm
		if(hni_flag==1)
		{
			hnitempgridFlag.clear();         // holds hni trasition flags temporarily
			hnitempgridFlag.resize(size,0);  // memory allocation 
			
		
			extract_hnicells(demperiod);
			gen_hnisnapshot("hni", ZeroPadNumber(demperiod), buffer_head, snapsum, 0);
		}

		// Implement fORSCE (demand-allocation) algorithm
		extract_lcccells(demperiod); 
		//gen_forescesnapshot(runname, 50+demperiod, buffer_head, snapsum, 0);
		merg_lccBuffer(); //This function will make buffer=0 for Veg to non-veg & non-veg to non-veg trasnision to prevent the cells from simulation.
		//gen_forescesnapshot(runname, 60+demperiod, buffer_head, snapsum, 0); //Temporary intermediate snapshot  from forsce only- just to make sure program is working.


		//BEYOND THIS POINT THE CONTROL HANDOVER TO LADS FOR FURTHER SIMULATION

		// process the simulation step by step
		for(year=runstep; year<=maxyear || is_bdin ==0;year+=runstep){

			// If the end year for the current parameter set if reached,
			// then read in the next parameter set
			if( simfire_flag == 1 ) {
				if((is_bdin == 0 && year == runstep) || (is_bdin == 1 && year > endy )) {
					// Read parameters for the specified number of fire regimes
					// Added by Ashis: Loop to read parameter fire in the fire regime input file .
					for(regcnt = 0; regcnt < regnum; regcnt ++) {
						infirefile >> endy;
						for(sevcnt = 0; sevcnt < 2; sevcnt ++) {
							infirefile >> nfr[regcnt][sevcnt];
							infirefile >> mfsizeha[regcnt][sevcnt];
							infirefile >> sdfsizeha[regcnt][sevcnt];
							infirefile >> cursevmod[regcnt][sevcnt];
							infirefile >> cursevmod2[regcnt][sevcnt];
							mfsize[regcnt][sevcnt] = mfsizeha[regcnt][sevcnt] / cell_ha;
							sdfsize[regcnt][sevcnt] = sdfsizeha[regcnt][sevcnt] / cell_ha;
							mlsize[regcnt][sevcnt] = log( pow(mfsize[regcnt][sevcnt],2) /
								sqrt( pow(mfsize[regcnt][sevcnt],2) + pow(sdfsize[regcnt][sevcnt],2) ) );
							stdlsize[regcnt][sevcnt] = sqrt( log( pow(sdfsize[regcnt][sevcnt],2) /
								pow(mfsize[regcnt][sevcnt],2) + 1) );
							if (nfr[regcnt][sevcnt] == 0) {
								mff[regcnt][sevcnt] = 0;
							} else {
								mff[regcnt][sevcnt] = runstep * rsize[regcnt] / (nfr[regcnt][sevcnt] * 
									(mfsize[regcnt][sevcnt]));
							}
						}
						infirefile.ignore(100, '\n');
					}
				}
			}

			// Test to see if the burn-in period is finished
			//Added By Ashis: Burn in period is optional for new the model, Mike used it to model historical landscapes before European came to US.
			if(year > burnin && is_bdin == 0) {
				is_bdin = 1;
				year = runstep;
				next_sum = step;
				for(regcnt = 0; regcnt < regnum; regcnt ++) {
					totburnarea[regcnt] = 0;
					totburncount[regcnt] = 0;
					liburnarea[regcnt] = 0;
					miburnarea[regcnt] = 0;
					hiburnarea[regcnt] = 0;
				}
			}

			// Set up fire spread summary grid
			if(sumtype > 0) {
				for(index=0; index<size; index++) {
					firespreadgrid[index] = 0;
				}
			}

			// Increment the age of every cell based on fire rotation, fire severity etc.
			grow_veg();

			// Non-spatial disturbances 
			nsdisturb_veg(distnum);
			
			// Forest management disturbances
			if( simharv_flag == 1 ) 
			{
				for(harvperiod=0;harvperiod<numDemand; harvperiod++)
				{   	
					if(harvperiod==demperiod)
						{
							for (harvzone=0;harvzone<numOwnership;harvzone++)
							{
								for(harvtype=0;harvtype<mgmtnum;harvtype++)
								{

										cout<< "Dem: "<<harvperiod <<"\t"<<"Hzone: "<<harvzone <<"\t"<<"Htype: "<<harvtype <<"\t"<<harvDemand[harvperiod][harvzone][harvtype]<<endl;
										
										//Allocate harvest with demand	
										if(harvDemand[harvperiod][harvzone][harvtype]>0)
										{

											// Determine the total area treated within the unit
											treatunit= ownershipCode[harvzone];   //treatment zone
											getEligibleHarvestCells(treatunit, harvtype,harvestCells);
											printf("Harvestsize=%d harvtype=%d period=%d unit=%d \n",harvestCells.size(),harvtype,harvperiod,treatunit);
											allocateHarvest(harvestCells, harvDemand[harvperiod][harvzone][harvtype], harvmeanSize[harvperiod][harvzone][harvtype],treatunit,harvtype);
										}
										
										
									}
							
								}
							}
	 			
					}
				//Output the harvest grid
				
				get_harvestsnapshot("harvest",ZeroPadNumber(demperiod), buffer_head,snapsum,0 );
				std::fill(harvestgrid,harvestgrid+size,0);

			}	
			//End forest harvesting
			
			// Fire disturbances
			if( simfire_flag == 1) 
			{
				//Repeat fire simulations - edited Ashis 4/21/2014
				for (simfire=0; simfire<numsimfire;simfire++)
				{	
					//Reset severity grid with 0
					std::fill(severitygrid,severitygrid+size,0);
				
					// Cycle through the fire loop
					for(regcnt = 0; regcnt < regnum; regcnt ++) {
						for(sevcnt = 0; sevcnt < 2; sevcnt ++ ) {
							if( mff[regcnt][sevcnt] < 30) {
								nfires = poisson_rv( mff[regcnt][sevcnt]);
							} else {
								nfires_f = normal_rv( mff[regcnt][sevcnt], sqrt(mff[regcnt][sevcnt]));
								if (nfires_f < 0) {
									nfires = 0;
								} else {
									nfires = floor(nfires_f + 0.5);
								}
							}

							
								for( fire = 0;fire < nfires; fire++ ) 
								{
									totburncount[regcnt]++;		// Increment the fire counter
									// Generate a maximum fire size
									fsize = gen_firesize(mfsize[regcnt][sevcnt], sdfsize[regcnt][sevcnt], shapetype);
									totburnarea[regcnt] += fsize;	// Increment the (potential) total area burned
									// Figure out the base distribution of severities
									curfiresev = gen_firesev(sevlow[regcnt][sevcnt], sevhigh[regcnt][sevcnt] );
									curfiresev2 = gen_firesev(sev2low[regcnt][sevcnt], sev2high[regcnt][sevcnt] );
									curmixedfire = curfiresev2 * (1 - curfiresev);
									curlowfire = 1 - (curfiresev + curmixedfire);
									// distribute fire across the landscape
									//printf("%d\t%d\n",regcnt,int(fsize));
							
									firespread( regcnt, (int)fsize );
									// Print fire information to screen							

									printf("runname=%s year=%d size=%lf regime=%d %d\n", runname, year, fsize, regcnt + 1, is_bdin);							

									// modify vegetation affected by fire
									if(simfire!=numsimfire-1)
									{
										disturb_veg( landfiresum, sevcnt, regcnt,false);					
									}
									else
									{
										disturb_veg( landfiresum, sevcnt, regcnt,true);	
									}
								}
							
						}

					} // end fires loop

				
				//Gen fire severity
				std::string str="severity";
				

				str.append(ZeroPadNumber(demperiod));
				gen_severitysnapshot(&str[0],ZeroPadNumber(simfire), buffer_head, snapsum, 0);

				} //end simulatin loop


			} // end if fireflag == 1
			

			// If landscape structure summary is selected, increment structure
			// summary grids
			if(is_bdin == 1 && landstrusum == 1) {
				for( index = 0; index < size; index++ ) {
					if(buffer[index] == 1) {
						strucsum[index + (stategrid[index] - 1) * size] ++;
					}
				}
			}

			// If the current year is a summary year, write output grids
			//if( year == next_sum && is_bdin == 1 ) //Orignal LADS
			if( year == 5 && is_bdin == 1 ) //Changed by Aashis next_sum =5 for each 5 year period
			{ 

				// Output the landscape "snapshot"
				if(snapsum >= 1) {
					gen_snapshot("state", ZeroPadNumber(demperiod), buffer_head, snapsum, 0);
					reclassify_lclu(com_stateout,com_lclustate,com_counter-1);
					gen_lccsnapshot("lclu", ZeroPadNumber(demperiod), buffer_head, snapsum, 0); //Temporary intermediate snapshot  of lclugrid- just to make sure program is working.
					writelog<<"END DEMAND PERIOD #"<<ZeroPadNumber(demperiod)<<" ****************************************************************************************************************************"<<endl;
				}
				// or output age summaries
				if(snapsum == 2) {
					gen_agesum(runname, year, buffer_head, runstep);
				}

				// Output wood biomass summaries
				if(biosum > 0 && biom_flag > 0) {
					gen_biosum(runname, year, buffer_head);
				}

				// Output fire summaries
				// To a separate file for each model run
				if(simfire_flag == 1 && sumtype == 1) {
					itoa(year, yearstr, 10);
					outfire << yearstr; 
					for(regcnt = 0; regcnt < regnum; regcnt ++) {
						if(totburncount[regcnt] > 0) {
							meanfireout = totburnarea[regcnt] / totburncount[regcnt];
						} else {
							meanfireout = 0;
						}
						outfire << " " << liburnarea[regcnt] << " " << miburnarea[regcnt] << " " << hiburnarea[regcnt] << " " << meanfireout;
						totburnarea[regcnt] = 0;
						totburncount[regcnt] = 0;
						liburnarea[regcnt] = 0;
						miburnarea[regcnt] = 0;
						hiburnarea[regcnt] = 0;
					}
					outfire << endl;
					// or all runs appended to a single file
				} else if (simfire_flag == 1 && sumtype == 2) {
					itoa(year, yearstr, 10);
					outfire << runname << " " << yearstr;
					for(regcnt = 0; regcnt < regnum; regcnt ++) {
						if(totburncount[regcnt] > 0) {
							meanfireout = totburnarea[regcnt] / totburncount[regcnt];
						} else {
							meanfireout = 0;
						}
						outfire << " " << liburnarea[regcnt] << " " << miburnarea[regcnt] << " " << hiburnarea[regcnt] << " " << meanfireout;
						totburnarea[regcnt] = 0;
						totburncount[regcnt] = 0;
						liburnarea[regcnt] = 0;
						miburnarea[regcnt] = 0;
						hiburnarea[regcnt] = 0;
					}
					outfire << endl;
				}


				// Output successional stage summaries
				// This is coded a bit differently from the preceding sections - this way is more concise
				if(runtype > 0) {

					// Compute the successional stage summaries for each community type
					landscape_sum(comnumstate, numcom );

					// Output the runname if we're appending to a single file
					if(runtype == 2) {
						outsum  << runname << " ";
					}

					outsum << year << " ";

					// Write the % area of each structure (and possbility biomass summaries) for each zone
					for(zonecnt = 0; zonecnt < numcom; zonecnt ++) {
						for(classcnt = 1; classcnt <= comnumstate[zonecnt]; classcnt ++) {
							outsum << sum_array[classcnt][zonecnt] << " ";
						}
						if(biom_flag > 0) {
							outsum << sum_livebio[zonecnt] << " " << sum_deadwood[zonecnt] << " ";
						}
					}

					outsum << endl;

				}

				next_sum += step;	// Set the next summary year

			} // end summary year if statement


		}  // end year loop
		
		


		// Do I need to worry about explicitly closing any other files?
		outfire.close();
		outsum.close();

		// Write fire frequency summary grid
		if(landfiresum == 1) {
			gen_firesum(runname, maxyear, buffer_head, runstep);
			gen_sevsum(runname, maxyear, buffer_head);
		}

		// Write the structure summary grid
		if(landstrusum == 1) {
			gen_strucsum(runname, largestnumstate, maxyear, buffer_head);
		}

	}





	// Free memory (am I missing any arrays here?)
	delete(age);
	delete(tsfire);
	delete(regime);
	delete(buffer);
	delete(comgrid);
	delete(fgrid1);
	delete(fgrid2);
	delete(fsusc);
	delete(fint);
	delete(temp);
	delete(temp2);
	delete(fsum1);
	delete(processed);
	delete(patchx);
	delete(patchy);
	delete(strucsum);
	//delete(lccgrid); // Unable to delete lccgrid why?
	writelog.clear();
	writelog.close();
	printf("Simulation is finished !!!");
	//cin.get();

	return 0;
}




