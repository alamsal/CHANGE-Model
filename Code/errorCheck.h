//----------------------------------------------------------------------------
// File:		errorCheckH.h
// Author: 		Ashis Lamsal
// Last Update:	1/16/2012
// Decription:	Contains external variable declarations and function prototypes
//              for errorCheck.cpp
//----------------------------------------------------------------------------
#ifndef errorCheckH
#define errorCheckH
using namespace std;

class ErrorCheck
{
	private:
		int flag;
	public:
		void check_file_exits(char _infilename[], char _comfilename[],char _landfilename[],char _infirefilename[], char _inharvfile[], char _inlccfilename[]);
		void check_input_param(int &_cellsize,int &_read_map, int &_maxrow,int &_maxcol,int &_simfire_flag,int &_simharv_flag, int &_burnin,int &_maxyear,int &_runstep,int &_step,int &_runtype,int &_sumtype, int &_tsumtype,int &_snapsum, int &_landfiresum,int &_landstrusum,int &_biom_flag, int &_distnum, int &_mgmtnum);
		void check_fireRegim_param(int &_regnum,int &_shapetype,double &_w_dir,double &_w_var,double &_w_speed,double &_w_shift,double &_fire_cal,double &_fire_cal2,int &_fire_pat,int &_torus,int &_mintsfire,int &_nslopeclass,int &_runstep);
		int get_grid_class(char *gridname, int &_gridsize);
		void check_communityType_param(int &_numcom,int _comcode[],int _comnumstate[], int _init_state[], int _statecode[40][40],int _stateout[40][40],double _satefiremod[40][40],double _statefireinit[40][40],
			double _statesev[40][40],double _statesev2[40][40],int _initage[40][40],int _suclag[40][40],int _suctrans[40][40],int _suclagreset[40][40],int _fireexlag[40][40],int _fireextrans[40][40],int _hsfiretrans[40][40],
			int _hsfirereset[40][40], int _msfiretrans[40][40],int _msfirereset[40][40],int _lsfiretrans[40][40], int _lsfirereset[40][40], int _treatelig[40][40][5], int _treattrans[40][40][5],int _treatreset[40][40][5],double _distprob[40][40][5],int _disttrans[40][40][5],int _distreset[40][40][5],int &_distnum, int &_mgmtnum);
		void check_landType_param(int & _numland, int _landcode[],double _landfiremod[],double _landfireinit[],double _landsevmod[],double _landsevmod2[]);
		void check_management_param();
		void check_lcc_param();
		
};



#endif