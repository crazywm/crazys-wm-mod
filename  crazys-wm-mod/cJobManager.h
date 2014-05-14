/*
 * Copyright 2009, 2010, The Pink Petal Development Team.
 * The Pink Petal Devloment Team are defined as the game's coders 
 * who meet on http://pinkpetal.org     // old site: http://pinkpetal .co.cc
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#pragma once
#include <string>
#include <vector>
#include "cGirls.h"
using namespace std;
//I need a better place for this
#define FOREACH(x,y) for(u_int x=0;x<y.size();x++)
struct sGirl;
struct sBrothel;
struct sGang;
//struct sJobBase;
//string sJobBase::m_Name;

//class cJobManager;
// typedef for the work fucntion
//typedef bool (cJobManager::*WorkJobF)(sGirl*,sBrothel*,int,string&);

struct sFilm
{
	//int total_girls;
	//int total_cost;
	float quality_multiplyer;
	vector<int> scene_quality;
	char time;
	int final_quality;
	bool sex_acts_flags[5];
	int total_customers;
	sFilm()
	{
		quality_multiplyer=0;
	}
};

//mainly a list of functions 
class cJobManager
{
	vector<sFilm *> film_list;
	// bah 2d array time for speed
	vector< vector<u_int> > job_groups;
public:
	//static vector<sJobBase *> job_list; - Changed until it is working - necro
	// using an array of function pointers
//	WorkJobF JobFunctions[NUM_JOBS];
	bool (*JobFunctions[NUM_JOBS])(sGirl*,sBrothel*,int,string&);

	string JobName[NUM_JOBS];  // short descriptive name of job
	string JobDescription[NUM_JOBS];  // longer description of job
	string JobFilterName[NUMJOBTYPES];  // short descriptive name of job filter
	string JobFilterDescription[NUMJOBTYPES];  // longer description of job filter
	unsigned int JobFilterIndex[NUMJOBTYPES+1];  // starting job index # for job filter
	string JobDescriptionCount(int job_id, int brothel_id, bool day = true, bool isClinic = false, bool isStudio = false, bool isArena = false, bool isCentre = false, bool isHouse = false);  // return a job description along with a count of how many girls are on it
	bool HandleSpecialJobs(int TargetBrothel, sGirl* Girl, int JobID, int OldJobID, bool DayOrNight);  // check for and handle special job assignments

	void Setup();

	// - Misc
	static bool WorkVoid(sGirl* girl, sBrothel* brothel, int DayNight, string& summary);	// used for jobs that are not yet implemented

	// - General
	static bool WorkFreetime(sGirl* girl, sBrothel* brothel, int DayNight, string& summary);	// resting
	static bool AddictBuysDrugs(string Addiction, string Drug, sGirl* girl, sBrothel* brothel, int DayNight);

	static bool WorkTraining(sGirl* girl, sBrothel* brothel, int DayNight, string& summary);
	static bool WorkCleaning(sGirl* girl, sBrothel* brothel, int DayNight, string& summary);
	static bool WorkSecurity(sGirl* girl, sBrothel* brothel, int DayNight, string& summary);
	static bool WorkMatron(sGirl* girl, sBrothel* brothel, int DayNight, string& summary);
	static bool WorkAdvertising(sGirl* girl, sBrothel* brothel, int DayNight, string& summary);
	static bool WorkCustService(sGirl* girl, sBrothel* brothel, int DayNight, string& summary);
	static bool WorkTorturer(sGirl* girl, sBrothel* brothel, int DayNight, string& summary);
	static bool WorkExploreCatacombs(sGirl* girl, sBrothel* brothel, int DayNight, string& summary);
	static bool WorkBeastCapture(sGirl* girl, sBrothel* brothel, int DayNight, string& summary);
	static bool WorkBeastCare(sGirl* girl, sBrothel* brothel, int DayNight, string& summary);
	static bool WorkRecruiter(sGirl* girl, sBrothel* brothel, int DayNight, string& summary);

	// - Bar
	static bool WorkBar(sGirl* girl, sBrothel* brothel, int DayNight, string& summary);
	static bool WorkBarmaid(sGirl* girl, sBrothel* brothel, int DayNight, string& summary);
	static bool WorkBarWaitress(sGirl* girl, sBrothel* brothel, int DayNight, string& summary);
	static bool WorkBarSinger(sGirl* girl, sBrothel* brothel, int DayNight, string& summary);
	static bool WorkBarPiano(sGirl* girl, sBrothel* brothel, int DayNight, string& summary);

	// - Sleazy Bar
	static bool WorkSleazyBarmaid(sGirl* girl, sBrothel* brothel, int DayNight, string& summary);
	static bool WorkSleazyWaitress(sGirl* girl, sBrothel* brothel, int DayNight, string& summary);
	static bool WorkBarStripper(sGirl* girl, sBrothel* brothel, int DayNight, string& summary);
	static bool WorkBarWhore(sGirl* girl, sBrothel* brothel, int DayNight, string& summary);

	// - Gambling Hall
	static bool WorkHall(sGirl* girl, sBrothel* brothel, int DayNight, string& summary);
	static bool WorkHallDealer(sGirl* girl, sBrothel* brothel, int DayNight, string& summary);
	static bool WorkHallEntertainer(sGirl* girl, sBrothel* brothel, int DayNight, string& summary);
	static bool WorkHallXXXEntertainer(sGirl* girl, sBrothel* brothel, int DayNight, string& summary);
	static bool WorkHallWhore(sGirl* girl, sBrothel* brothel, int DayNight, string& summary);
	static bool WorkShow(sGirl* girl, sBrothel* brothel, int DayNight, string& summary);

	// - Brothel
	static bool WorkWhore(sGirl* girl, sBrothel* brothel, int DayNight, string& summary);
	static bool WorkBrothelStripper(sGirl* girl, sBrothel* brothel, int DayNight, string& summary);
	static bool WorkBrothelMasseuse(sGirl* girl, sBrothel* brothel, int DayNight, string& summary);
	static bool WorkPeepShow(sGirl* girl, sBrothel* brothel, int DayNight, string& summary);

	//- Stables
	static bool WorkMilk(sGirl* girl, sBrothel* brothel, int DayNight, string& summary);

	// - Clinic
	static bool WorkDoctor(sGirl* girl, sBrothel* brothel, int DayNight, string& summary);
	static bool WorkHealing(sGirl* girl, sBrothel* brothel, int DayNight, string& summary);
	static bool WorkGetAbort(sGirl* girl, sBrothel* brothel, int DayNight, string& summary);
	static bool WorkPhysicalSurgery(sGirl* girl, sBrothel* brothel, int DayNight, string& summary);
	static bool WorkRepairShop(sGirl* girl, sBrothel* brothel, int DayNight, string& summary);
	static bool WorkNurse(sGirl* girl, sBrothel* brothel, int DayNight, string& summary);
	static bool WorkMechanic(sGirl* girl, sBrothel* brothel, int DayNight, string& summary);
	static bool WorkChairMan(sGirl* girl, sBrothel* brothel, int DayNight, string& summary);
	static bool WorkJanitor(sGirl* girl, sBrothel* brothel, int DayNight, string& summary);
	static bool WorkBreastReduction(sGirl* girl, sBrothel* brothel, int DayNight, string& summary);
	static bool WorkLiposuction(sGirl* girl, sBrothel* brothel, int DayNight, string& summary);
	static bool WorkBoobJob(sGirl* girl, sBrothel* brothel, int DayNight, string& summary);

	// - Movie Studio
	static bool WorkFluffer(sGirl* girl, sBrothel* brothel, int DayNight, string& summary);
	static bool WorkFilmSex(sGirl* girl, sBrothel* brothel, int DayNight, string& summary);
	static bool WorkFilmBeast(sGirl* girl, sBrothel* brothel, int DayNight, string& summary);
	static bool WorkFilmGroup(sGirl* girl, sBrothel* brothel, int DayNight, string& summary);
	static bool WorkFilmAnal(sGirl* girl, sBrothel* brothel, int DayNight, string& summary);
	static bool WorkFilmLesbian(sGirl* girl, sBrothel* brothel, int DayNight, string& summary);
	static bool WorkFilmBondage(sGirl* girl, sBrothel* brothel, int DayNight, string& summary);
	static bool WorkFilmOral(sGirl* girl, sBrothel* brothel, int DayNight, string& summary);
	static bool WorkFilmMast(sGirl* girl, sBrothel* brothel, int DayNight, string& summary);
	static bool WorkFilmTitty(sGirl* girl, sBrothel* brothel, int DayNight, string& summary);
	static bool WorkFilmStrip(sGirl* girl, sBrothel* brothel, int DayNight, string& summary);
	static bool WorkFilmRandom(sGirl* girl, sBrothel* brothel, int DayNight, string& summary);
	static bool WorkCameraMage(sGirl* girl, sBrothel* brothel, int DayNight, string& summary);
	static bool WorkCrystalPurifier(sGirl* girl, sBrothel* brothel, int DayNight, string& summary);
	static bool WorkFilmDirector(sGirl* girl, sBrothel* brothel, int DayNight, string& summary);
	static bool WorkFilmPromoter(sGirl* girl, sBrothel* brothel, int DayNight, string& summary);
	static bool WorkFilmStagehand(sGirl* girl, sBrothel* brothel, int DayNight, string& summary);

	//- Arena
	static bool WorkFightBeast(sGirl* girl, sBrothel* brothel, int DayNight, string& summary);
	static bool WorkFightArenaGirls(sGirl* girl, sBrothel* brothel, int DayNight, string& summary);
	static bool WorkCombatTraining(sGirl* girl, sBrothel* brothel, int DayNight, string& summary);
	static bool WorkDoctore(sGirl* girl, sBrothel* brothel, int DayNight, string& summary);
	static bool WorkCleanArena(sGirl* girl, sBrothel* brothel, int DayNight, string& summary);
	static bool WorkCityGuard(sGirl* girl, sBrothel* brothel, int DayNight, string& summary);

	//Comunity Centre
	static bool WorkComunityService(sGirl* girl, sBrothel* brothel, int DayNight, string& summary);
	static bool WorkMakeItem(sGirl* girl, sBrothel* brothel, int DayNight, string& summary);
	static bool WorkFeedPoor(sGirl* girl, sBrothel* brothel, int DayNight, string& summary);
	static bool WorkCentreManager(sGirl* girl, sBrothel* brothel, int DayNight, string& summary);
	static bool WorkCleanCentre(sGirl* girl, sBrothel* brothel, int DayNight, string& summary);

	//drug Centre
	static bool WorkDrugCounselor(sGirl* girl, sBrothel* brothel, int DayNight, string& summary);
	static bool WorkRehab(sGirl* girl, sBrothel* brothel, int DayNight, string& summary);

	// house
	static bool WorkPersonalTraining(sGirl* girl, sBrothel* brothel, int DayNight, string& summary);
	static bool WorkPersonalBedWarmer(sGirl* girl, sBrothel* brothel, int DayNight, string& summary);
	static bool WorkCleanHouse(sGirl* girl, sBrothel* brothel, int DayNight, string& summary);
	static bool WorkHeadGirl(sGirl* girl, sBrothel* brothel, int DayNight, string& summary);

	// - stuff that does processing for jobs

	// MYR: New code for security.  All the old code is still here, commented out.
	static bool work_related_violence(sGirl*, int, bool);
	static int guard_coverage(vector<sGang*> *v=0);
	static bool security_stops_rape(sGirl * girl, sGang *enemy_gang, int day_night);
	//static bool gang_stops_rape(sGirl* girl, sGang *gang, int chance, int day_night);
	static bool gang_stops_rape(sGirl* girl, vector<sGang *> gangs_guarding, sGang *enemy_gang, int coverage, int day_night);
	//static bool girl_fights_rape(sGirl*, int);
	static bool girl_fights_rape(sGirl* girl, sGang *enemy_gang, int day_night);
	static void customer_rape(sGirl*);
	static string GetGirlAttackedString();


	static bool Preprocessing(int action, sGirl* girl, sBrothel* brothel, int DayNight, string& summary, string& message);
	static void GetMiscCustomer(sBrothel* brothel, sCustomer& cust);

	bool work_show(sGirl * girl,sBrothel * brothel,string& summary,int DayNight);
	void update_film(sBrothel *);
	long make_money_films();
	void save_films(ofstream &ofs);
	void load_films(ifstream &ifs);
	bool apply_job(sGirl * girl,int job,int brothel_id,bool day_or_night,string & message);
	int get_num_on_job(int job,int brothel_id,bool day_or_night);
	static bool is_sex_type_allowed(unsigned int sex_type, sBrothel* brothel);
	#ifndef _DEBUG
		static void free();
	#else
		static void freeJobs();
	#endif
	
	//helpers
	static vector<sGirl*> girls_on_job(sBrothel *brothel, u_int job_wanted, int day_or_night);
	//need a function for seeing if there is a girl working on a job
	bool is_job_employed(sBrothel * brothel,u_int job_wanted,int day_or_night);
	static void get_training_set(vector<sGirl*> &v, vector<sGirl*> &set);
	static void do_training(sBrothel* brothel, int DayNight);
	static void do_training_set(vector<sGirl*> girls, int day_night);
	static void do_solo_training(sGirl *girl, int DayNight);
	static void do_advertising(sBrothel* brothel, int daynight);
	static void do_whorejobs(sBrothel* brothel, int daynight);
	static void do_custjobs(sBrothel* brothel, int daynight);
	~cJobManager(){}

	bool is_job_Paid_Player(u_int Job);						//	WD:	Test for all jobs paid by player
};
