//----------------------------------------------------------------------------
// File:		errorCheckH.cpp
// Author: 		Ashis Lamsal
// Last Update:	1/16/2012
// Decription:	This module is responsible for checking *.gis file's availability in the program's running path, comparing their unsigned integer value classes with paramater files,
// checking each line of parameter files to check for invalid  parameter values.
//----------------------------------------------------------------------------
#include "errorCheck.h"
#include <vector>
#include <fstream>
#include <string>
#include <iostream>
#include<set>
using namespace std;

void ErrorCheck:: check_file_exits(char _infilename[], char _comfilename[],char _landfilename[],char _infirefilename[], char _inharvfilename[], char _inlccfilename[])
{
	cout<< "Checking Parameter files" <<endl;
	std::vector <std::string> filenames;
	filenames.push_back(_infilename);
	filenames.push_back(_comfilename);
	filenames.push_back(_landfilename);
	filenames.push_back(_infirefilename);
	filenames.push_back(_inharvfilename);
	filenames.push_back(_inlccfilename);
	
	for ( unsigned int i=0; i<filenames.size();i++)
	{
		ifstream inputfile(filenames[i]);
		if(inputfile.good())
		{
			cout<< "File name:\t"<<filenames[i]<<"\t OK" << endl;
		}
		else
		{			
			cout<< "@@@ ERROR:\t"<<filenames[i]<<"not avilable @@@" << endl;
			std::cout<<"Please correct the input files before running this application."<<endl;
			cin.get();
			exit(1);
		}

	}

	

}
void ErrorCheck::check_input_param(int &_cellsize,int &_read_map, int &_maxrow,int &_maxcol,int &_simfire_flag,int &_simharv_flag, int &_burnin,int &_maxyear,int &_runstep,int &_step,int &_runtype,int &_sumtype, int &_tsumtype,int &_snapsum, int &_landfiresum,int &_landstrusum,int &_biom_flag, int &_distnum, int &_mgmtnum)
{
	cout<< "Checking MAIN INPUT parameter variables"<<endl;
	std::vector <int> param_flag;
	param_flag.push_back((_cellsize>0)?0:1);
	param_flag.push_back(((_read_map==0) || (_read_map==1) ||(_read_map=2))?0:1);
	param_flag.push_back((_maxrow>0)?0:1);
	param_flag.push_back((_maxcol>0)?0:1);
	param_flag.push_back((_simfire_flag==0 || _simfire_flag==1)?0:1);
	param_flag.push_back((_simharv_flag==0 || _simharv_flag==1)?0:1);
	param_flag.push_back((_burnin>=0)?0:1);
	param_flag.push_back((_maxyear>=1)?0:1);
	param_flag.push_back((_runstep>=1)?0:1);
	param_flag.push_back((_step>=1 && (_step%_runstep==0))?0:1);
	param_flag.push_back((_runtype==1 || _runtype==2)?0:1);
	param_flag.push_back(((_sumtype==0) ||(_sumtype==1)||(_sumtype==2)||(_sumtype==3) ||(_sumtype==4))?0:1);
	param_flag.push_back((_tsumtype==0 || _tsumtype==1 ||_tsumtype==2 ||_tsumtype==3  || _tsumtype==4)?0:1);
	param_flag.push_back((_snapsum==0 ||_snapsum==1 ||_snapsum==2)?0:1);
	param_flag.push_back((_landfiresum==0 || _landfiresum==1)?0:1);
	param_flag.push_back((_landstrusum==0 || _landstrusum==1)?0:1);
	param_flag.push_back((_biom_flag==0 || _biom_flag==1)?0:1);
	param_flag.push_back((_distnum>=0)?0:1);
	param_flag.push_back((_mgmtnum>=0)?0:1);

	for (unsigned int i=0;i<param_flag.size();i++)
	{
		if(param_flag[i]==1)
		{
			cout<<"@@@ ERROR: Invaid MAIN INPUT File parameters"<<endl;
			cin.get();
			exit(1);			//Program exit

		}
		else
		{
			cout <<"line# \t" <<i+1 <<"\t OK" <<endl;
		}
	}


}
void ErrorCheck::check_fireRegim_param(int &_regnum,int &_shapetype,double &_w_dir,double &_w_var,double &_w_speed,double &_w_shift,double &_fire_cal,double &_fire_cal2,int &_fire_pat,int &_torus,int &_mintsfire,int &_nslopeclass, int &_runstep)
{
	cout<< "Checking FIRE REGIME parameter variables"<<endl;
	std::vector<int> fire_param_flag;
	fire_param_flag.push_back((_regnum>=1)?0:1);
	fire_param_flag.push_back((_shapetype==1 || _shapetype==2)?0:1);
	fire_param_flag.push_back((_w_dir>=0 && _w_dir<=360)?0:1);
	fire_param_flag.push_back((_w_var>=0 && _w_var<=360)?0:1);
	fire_param_flag.push_back((_w_speed>=0 && _w_speed<=10)?0:1);
	fire_param_flag.push_back((_w_shift>=0 && _w_shift<=10)?0:1);
	fire_param_flag.push_back((_fire_cal>=0 && _fire_cal<=1)?0:1);
	fire_param_flag.push_back((_fire_cal2>=0 && _fire_cal2<=1)?0:1);
	fire_param_flag.push_back((_fire_pat==1 || _fire_pat==2)?0:1);
	fire_param_flag.push_back((_torus==0||_torus==1)?0:1);
	fire_param_flag.push_back((_mintsfire>=0 && _mintsfire%_runstep==0)?0:1);
	fire_param_flag.push_back((_nslopeclass>=-99999 && _nslopeclass<=99999)?0:1);

	for(unsigned int i=0;i<fire_param_flag.size();i++)
	{
		if(fire_param_flag[i]==1)
		{
			cout<<"@@@ ERROR: Invaid FIRE REGIME File parameters"<<endl;
			cin.get();
			exit(1);	
		}
		else
		{
			cout <<"line# \t" <<i+1 <<"\t OK" <<endl;
		}
	}

}

void ErrorCheck::check_communityType_param(int &_numcom,int _comcode[],int _comnumstate[], int _init_state[], int _statecode[40][40],int _stateout[40][40],double _statefiremod[40][40],double _statefireinit[40][40],double _statesev[40][40],double _statesev2[40][40],int _initage[40][40],int _suclag[40][40],int _suctrans[40][40],int _suclagreset[40][40],int _fireexlag[40][40],int _fireextrans[40][40],int _hsfiretrans[40][40],int _hsfirereset[40][40], int _msfiretrans[40][40],int _msfirereset[40][40],int _lsfiretrans[40][40], int _lsfirereset[40][40], int _treatelig[40][40][5], int _treattrans[40][40][5],int _treatreset[40][40][5],double _distprob[40][40][5],int _disttrans[40][40][5],int _distreset[40][40][5],int &_distnum, int &_mgmtnum)
{
	cout<< "Checking COMMUNITY FILE parameter variables"<<endl;
	std::vector<int> community_param_flag;

	if(_numcom>=1)
	{
		for( int comcnt=0;comcnt<_numcom;comcnt++)
		{
			community_param_flag.push_back((_comcode[comcnt]>=0)?0:1);
			community_param_flag.push_back((_comnumstate[comcnt]>=0)?0:1);
			community_param_flag.push_back((_init_state[comcnt]>=0)?0:1);
			for( int statecnt=0; statecnt<_comnumstate[comcnt];statecnt++)
			{
				community_param_flag.push_back((_statecode[comcnt][statecnt]>=1)?0:1);
				community_param_flag.push_back((_stateout[comcnt][statecnt]>=0)?0:1);
				community_param_flag.push_back((_statefiremod[comcnt][statecnt]>=0)?0:1);
				community_param_flag.push_back((_statefireinit[comcnt][statecnt]>=0)?0:1);
				community_param_flag.push_back((_statesev[comcnt][statecnt]>=0 && _statesev[comcnt][statecnt]<=1)?0:1);
				community_param_flag.push_back((_statesev2[comcnt][statecnt]>=0 && _statesev2[comcnt][statecnt]<=1)?0:1);
				community_param_flag.push_back((_initage[comcnt][statecnt]>=0)?0:1);
				community_param_flag.push_back((_suclag[comcnt][statecnt]>0 && _suclag[comcnt][statecnt]<=999)?0:1);
				community_param_flag.push_back((_suctrans[comcnt][statecnt]>0 && _suctrans[comcnt][statecnt]<=999)?0:1); 
				community_param_flag.push_back((_suclagreset[comcnt][statecnt]>0 && _suclagreset[comcnt][statecnt]<=999)?0:1);
				community_param_flag.push_back((_fireexlag[comcnt][statecnt]>0 && _fireexlag[comcnt][statecnt]<=999)?0:1);
				community_param_flag.push_back((_fireextrans[comcnt][statecnt]>0 && _fireextrans[comcnt][statecnt]<=999)?0:1);
				community_param_flag.push_back((_hsfiretrans[comcnt][statecnt]>=0)?0:1);
				community_param_flag.push_back((_hsfirereset[comcnt][statecnt]>=0)?0:1);
				community_param_flag.push_back((_msfiretrans[comcnt][statecnt]>=0)?0:1);
				community_param_flag.push_back((_msfirereset[comcnt][statecnt]>=0)?0:1);
				community_param_flag.push_back((_lsfiretrans[comcnt][statecnt]>=0)?0:1);
				community_param_flag.push_back((_lsfirereset[comcnt][statecnt]>=0)?0:1);
				for( int classcnt=0; classcnt<_mgmtnum;classcnt++)
				{
					community_param_flag.push_back((_treatelig[comcnt][statecnt][classcnt]>=0)?0:1);
					community_param_flag.push_back((_treattrans[comcnt][statecnt][classcnt]>=0)?0:1);
					community_param_flag.push_back((_treatreset[comcnt][statecnt][classcnt]>=0)?0:1);		
				}
				for(  int classcnt=0; classcnt<_distnum;classcnt++)
				{
					community_param_flag.push_back((_distprob[comcnt][statecnt][classcnt]>=0)?0:1);	
					community_param_flag.push_back((_disttrans[comcnt][statecnt][classcnt]>=0)?0:1);	
					community_param_flag.push_back((_distreset[comcnt][statecnt][classcnt]>=0)?0:1);					
				}
				
			}

		}
	
	}
	else
	{
		cout<<"@@@ ERROR: Invaid number of community types"<<endl;
		cin.get();
		exit(1);
	}

	for(unsigned int i=0;i<community_param_flag.size();i++)
	{
		if(community_param_flag[i]==1)
		{
			cout<<"@@@ ERROR: Invaid COMMUNITY FILE parameters"<<endl;
			cin.get();
			exit(1);	
		}
		
	}
	cout<<"COMMUNITY FILE \t OK"<<endl;

}
void ErrorCheck::check_landType_param(int &_numland, int _landcode[],double _landfiremod[],double _landfireinit[],double _landsevmod[],double _landsevmod2[])
{
	cout<< "Checking LAND FILE parameter variables"<<endl;
	std::vector<int> landtype_param_flag;

	if(_numland>0)
	{
		for( int landcnt=0; landcnt<_numland;landcnt++)
		{
			landtype_param_flag.push_back((_landcode[landcnt]>0)?0:1);
			landtype_param_flag.push_back((_landfiremod[landcnt]>0)?0:1);
			landtype_param_flag.push_back((_landfireinit[landcnt]>0)?0:1);
			landtype_param_flag.push_back((_landsevmod[landcnt]>0)?0:1);
			landtype_param_flag.push_back((_landsevmod2[landcnt]>0)?0:1);

		}
	}
	else
	{		
		std::cout<<"@@@ ERROR: Invalid number of landtypes"<<endl;
	}
	
	for(unsigned int i=0;i<landtype_param_flag.size();i++)
	{
		if(landtype_param_flag[i]==1)
		{
			cout<<"@@@ ERROR: Invaid LAND FILE  parameters"<<endl;
			cin.get();
			exit(1);	
		}
		else
		{
			cout <<"line# \t" <<i+1 <<"\t OK" <<endl;
		}
	}

}

int ErrorCheck::get_grid_class(char *gridname, int &_gridsize)
{
  std::set<int> grid_values ;
  std::set<int>::iterator it;
  grid_values.clear();
  grid_values.insert(gridname,gridname+_gridsize);
  
  //for (it=grid_values.begin(); it!=grid_values.end(); ++it)
  //std::cout << ' ' << int(*it)<<endl;
  return(grid_values.size());

}