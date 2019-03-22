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
#include "InterfaceGlobals.h"
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
//	WorkJobF JobFunc[NUM_JOBS];
	bool	(*JobFunc[NUM_JOBS])(sGirl*, sBrothel*, bool, string&);
	double	(*JobPerf[NUM_JOBS])(sGirl*, bool estimate);			// `J` a replacement for job performance - work in progress

	string JobName[NUM_JOBS];				// short descriptive name of job
	string JobQkNm[NUM_JOBS];				// a shorter name of job
	string JobDesc[NUM_JOBS];				// longer description of job
	string JobFilterName[NUMJOBTYPES];		// short descriptive name of job filter
	string JobFilterDesc[NUMJOBTYPES];		// longer description of job filter
	int JobFilterIndex[NUMJOBTYPES + 1];	// starting job index # for job filter
	string JobDescriptionCount(int job_id, int brothel_id, int day = SHIFT_DAY, bool isClinic = false, bool isStudio = false, bool isArena = false, bool isCentre = false, bool isHouse = false, bool isFarm = false);  // return a job description along with a count of how many girls are on it
    bool HandleSpecialJobs( int TargetBrothel, sGirl* Girl, int JobID, int OldJobID, bool Day0Night1, bool fulltime = false );  // check for and handle special job assignments

	void Setup();

	// - Misc
	static bool WorkVoid(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary);	// used for jobs that are not yet implemented

	// `J` When modifying Jobs, search for "J-Change-Jobs"  :  found in >> cJobManager.h > class cJobManager

	// - General
	static bool AddictBuysDrugs(string Addiction, string Drug, sGirl* girl, sBrothel* brothel, bool Day0Night1);
	static bool WorkFreetime(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary);	// resting
	static double JP_Freetime(sGirl* girl, bool estimate);	// not used
	static bool WorkTraining(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary);
	static double JP_Training(sGirl* girl, bool estimate);	// not used
	static bool WorkCleaning(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary);
	static double JP_Cleaning(sGirl* girl, bool estimate);
	static bool WorkSecurity(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary);
	static double JP_Security(sGirl* girl, bool estimate);
	static bool WorkAdvertising(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary);
	static double JP_Advertising(sGirl* girl, bool estimate);
	static bool WorkCustService(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary);
	static double JP_CustService(sGirl* girl, bool estimate);
	static bool WorkMatron(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary);
	static double JP_Matron(sGirl* girl, bool estimate);
	static bool WorkTorturer(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary);
	static double JP_Torturer(sGirl* girl, bool estimate);
	static bool WorkExploreCatacombs(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary);
	static double JP_ExploreCatacombs(sGirl* girl, bool estimate);
	static bool WorkBeastCare(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary);
	static double JP_BeastCare(sGirl* girl, bool estimate);
	// - Bar
	static bool WorkBarmaid(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary);
	static double JP_Barmaid(sGirl* girl, bool estimate);
	static bool WorkBarWaitress(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary);
	static double JP_BarWaitress(sGirl* girl, bool estimate);
	static bool WorkBarSinger(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary);
	static double JP_BarSinger(sGirl* girl, bool estimate);
	static bool WorkBarPiano(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary);
	static double JP_BarPiano(sGirl* girl, bool estimate);
	static bool WorkEscort(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary);
	static double JP_Escort(sGirl* girl, bool estimate);
	static bool WorkBarCook(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary);
	static double JP_Barcook(sGirl* girl, bool estimate);
	// - Gambling Hall
	static bool WorkHallDealer(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary);
	static double JP_HallDealer(sGirl* girl, bool estimate);
	static bool WorkHallEntertainer(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary);
	static double JP_HallEntertainer(sGirl* girl, bool estimate);
	static bool WorkHallXXXEntertainer(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary);
	static double JP_HallXXXEntertainer(sGirl* girl, bool estimate);
	static bool WorkHallWhore(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary);
	static double JP_HallWhore(sGirl* girl, bool estimate);
	// - Sleazy Bar
	static bool WorkSleazyBarmaid(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary);
	static double JP_SleazyBarmaid(sGirl* girl, bool estimate);
	static bool WorkSleazyWaitress(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary);
	static double JP_SleazyWaitress(sGirl* girl, bool estimate);
	static bool WorkBarStripper(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary);
	static double JP_BarStripper(sGirl* girl, bool estimate);
	static bool WorkBarWhore(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary);
	static double JP_BarWhore(sGirl* girl, bool estimate);
	// - Brothel
	static bool WorkBrothelMasseuse(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary);
	static double JP_BrothelMasseuse(sGirl* girl, bool estimate);
	static bool WorkBrothelStripper(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary);
	static double JP_BrothelStripper(sGirl* girl, bool estimate);
	static bool WorkPeepShow(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary);
	static double JP_PeepShow(sGirl* girl, bool estimate);
	static bool WorkWhore(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary);
	static double JP_Whore(sGirl* girl, bool estimate);
	static double JP_WhoreStreets(sGirl* girl, bool estimate);

	// - Movie Studio - Actress
	//BSIN
	//Xxtreme
	static bool WorkFilmBeast(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary);
	static double JP_FilmBeast(sGirl* girl, bool estimate);
	static bool WorkFilmBuk(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary);
	static double JP_FilmBuk(sGirl* girl, bool estimate);
	static bool WorkFilmThroat(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary);
	static double JP_FilmThroat(sGirl* girl, bool estimate);
	static bool WorkFilmBondage(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary);
	static double JP_FilmBondage(sGirl* girl, bool estimate);
	static bool WorkFilmPublicBDSM(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary);
	static double JP_FilmPublicBDSM(sGirl* girl, bool estimate);

	/*static bool WorkFilmDominatrix(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary);
	static double JP_FilmDom(sGirl* girl, bool estimate);*/

	//Adult
	static bool WorkFilmSex(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary);
	static double JP_FilmSex(sGirl* girl, bool estimate);
	static bool WorkFilmAnal(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary);
	static double JP_FilmAnal(sGirl* girl, bool estimate);
	static bool WorkFilmLesbian(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary);
	static double JP_FilmLesbian(sGirl* girl, bool estimate);
	static bool WorkFilmGroup(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary);
	static double JP_FilmGroup(sGirl* girl, bool estimate);
	static bool WorkFilmOral(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary);
	static double JP_FilmOral(sGirl* girl, bool estimate);
	static bool WorkFilmMast(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary);
	static double JP_FilmMast(sGirl* girl, bool estimate);
	static bool WorkFilmTitty(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary);
	static double JP_FilmTitty(sGirl* girl, bool estimate);
	static bool WorkFilmHandJob(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary);
	static double JP_FilmHandJob(sGirl* girl, bool estimate);
	static bool WorkFilmFootJob(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary);
	static double JP_FilmFootJob(sGirl* girl, bool estimate);
	
	//Actress
	//static bool WorkFilmIdol(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary);
	//static double JP_FilmIdol(sGirl* girl, bool estimate);
	static bool WorkFilmAction(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary);
	static double JP_FilmAction(sGirl* girl, bool estimate);
	static bool WorkFilmMusic(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary);
	static double JP_FilmMusic(sGirl* girl, bool estimate);
	static bool WorkFilmChef(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary);
	static double JP_FilmChef(sGirl* girl, bool estimate);
	static bool WorkFilmTease(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary);
	static double JP_FilmTease(sGirl* girl, bool estimate);
	static bool WorkFilmStrip(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary);
	static double JP_FilmStrip(sGirl* girl, bool estimate);
	
	//Rand
	static bool WorkFilmRandom(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary);
	static double JP_FilmRandom(sGirl* girl, bool estimate);

	// - Movie Studio - Crew
	static bool WorkFilmDirector(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary);
	static double JP_FilmDirector(sGirl* girl, bool estimate);
	static bool WorkFilmPromoter(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary);
	static double JP_FilmPromoter(sGirl* girl, bool estimate);
	static bool WorkCameraMage(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary);
	static double JP_CameraMage(sGirl* girl, bool estimate);
	static bool WorkCrystalPurifier(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary);
	static double JP_CrystalPurifier(sGirl* girl, bool estimate);
	static bool WorkFluffer(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary);
	static double JP_Fluffer(sGirl* girl, bool estimate);
	static bool WorkFilmStagehand(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary);
	static double JP_FilmStagehand(sGirl* girl, bool estimate);

	// - Arena - Fighting
	static bool WorkFightBeast(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary);
	static double JP_FightBeast(sGirl* girl, bool estimate);
	static bool WorkFightArenaGirls(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary);
	static double JP_FightArenaGirls(sGirl* girl, bool estimate);
	static bool WorkCombatTraining(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary);
	static double JP_CombatTraining(sGirl* girl, bool estimate);
	static bool WorkArenaJousting(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary);
	static double JP_ArenaJousting(sGirl* girl, bool estimate);
	static bool WorkArenaRacing(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary);
	static double JP_ArenaRacing(sGirl* girl, bool estimate);
	// - Arena - Staff
	static bool WorkDoctore(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary);
	static double JP_Doctore(sGirl* girl, bool estimate);
	static bool WorkCityGuard(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary);
	static double JP_CityGuard(sGirl* girl, bool estimate);
	static bool WorkBlacksmith(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary);
	static double JP_Blacksmith(sGirl* girl, bool estimate);
	static bool WorkCobbler(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary);
	static double JP_Cobbler(sGirl* girl, bool estimate);
	static bool WorkJeweler(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary);
	static double JP_Jeweler(sGirl* girl, bool estimate);
	static bool WorkCleanArena(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary);
	static double JP_CleanArena(sGirl* girl, bool estimate);

	//Comunity Centre
	static bool WorkCentreManager(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary);
	static double JP_CentreManager(sGirl* girl, bool estimate);
	static bool WorkFeedPoor(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary);
	static double JP_FeedPoor(sGirl* girl, bool estimate);
	static bool WorkComunityService(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary);
	static double JP_ComunityService(sGirl* girl, bool estimate);
	static bool WorkCleanCentre(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary);
	static double JP_CleanCentre(sGirl* girl, bool estimate);
	// Counseling Centre
	static bool WorkCounselor(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary);
	static double JP_Counselor(sGirl* girl, bool estimate);
	static bool WorkRehab(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary);
	static double JP_Rehab(sGirl* girl, bool estimate);
	static bool WorkCentreAngerManagement(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary);
	static double JP_CentreAngerManagement(sGirl* girl, bool estimate);
	static bool WorkCentreExTherapy(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary);
	static double JP_CentreExTherapy(sGirl* girl, bool estimate);
	static bool WorkCentreTherapy(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary);
	static double JP_CentreTherapy(sGirl* girl, bool estimate);

	// - Clinic - Surgery
	static bool WorkHealing(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary);
	static double JP_Healing(sGirl* girl, bool estimate);
	static bool WorkRepairShop(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary);
	static double JP_RepairShop(sGirl* girl, bool estimate);
	static bool WorkCureDiseases(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary);
	static double JP_CureDiseases(sGirl* girl, bool estimate);
	static bool WorkGetAbort(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary);
	static double JP_GetAbort(sGirl* girl, bool estimate);
	static bool WorkCosmeticSurgery(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary);
	static double JP_CosmeticSurgery(sGirl* girl, bool estimate);
	static bool WorkLiposuction(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary);
	static double JP_Liposuction(sGirl* girl, bool estimate);
	static bool WorkBreastReduction(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary);
	static double JP_BreastReduction(sGirl* girl, bool estimate);
	static bool WorkBoobJob(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary);
	static double JP_BoobJob(sGirl* girl, bool estimate);
	static bool WorkGetVaginalRejuvenation(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary);
	static double JP_GetVaginalRejuvenation(sGirl* girl, bool estimate);
	static bool WorkGetFacelift(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary);
	static double JP_GetFacelift(sGirl* girl, bool estimate);
	static bool WorkGetAssJob(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary);
	static double JP_GetAssJob(sGirl* girl, bool estimate);
	static bool WorkGetTubesTied(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary);
	static double JP_GetTubesTied(sGirl* girl, bool estimate);
	static bool WorkGetFertility(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary);
	static double JP_GetFertility(sGirl* girl, bool estimate);
	// - Clinic - Staff
	static bool WorkChairMan(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary);
	static double JP_ChairMan(sGirl* girl, bool estimate);
	static bool WorkDoctor(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary);
	static double JP_Doctor(sGirl* girl, bool estimate);
	static bool WorkNurse(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary);
	static double JP_Nurse(sGirl* girl, bool estimate);
	static bool WorkMechanic(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary);
	static double JP_Mechanic(sGirl* girl, bool estimate);
	static bool WorkIntern(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary);
	static double JP_Intern(sGirl* girl, bool estimate);
	static bool WorkJanitor(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary);
	static double JP_Janitor(sGirl* girl, bool estimate);

	// - Farm - Staff
	static bool WorkFarmManager(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary);
	static double JP_FarmManager(sGirl* girl, bool estimate);
	static bool WorkFarmVeterinarian(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary);
	static double JP_FarmVeterinarian(sGirl* girl, bool estimate);
	static bool WorkFarmMarketer(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary);
	static double JP_FarmMarketer(sGirl* girl, bool estimate);
	static bool WorkFarmResearch(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary);
	static double JP_FarmResearch(sGirl* girl, bool estimate);
	static bool WorkFarmHand(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary);
	static double JP_FarmHand(sGirl* girl, bool estimate);
	// - Farm - Laborers
	static bool WorkFarmer(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary);
	static double JP_Farmer(sGirl* girl, bool estimate);
	static bool WorkGardener(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary);
	static double JP_Gardener(sGirl* girl, bool estimate);
	static bool WorkShepherd(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary);
	static double JP_Shepherd(sGirl* girl, bool estimate);
	static bool WorkRancher(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary);
	static double JP_Rancher(sGirl* girl, bool estimate);
	static bool WorkCatacombRancher(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary);
	static double JP_CatacombRancher(sGirl* girl, bool estimate);
	static bool WorkBeastCapture(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary);
	static double JP_BeastCapture(sGirl* girl, bool estimate);
	static bool WorkMilker(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary);
	static double JP_Milker(sGirl* girl, bool estimate);
	static bool WorkMilk(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary);
	static double JP_Milk(sGirl* girl, bool estimate);
	// - Farm - Producers
	static bool WorkButcher(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary);
	static double JP_Butcher(sGirl* girl, bool estimate);
	static bool WorkBaker(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary);
	static double JP_Baker(sGirl* girl, bool estimate);
	static bool WorkBrewer(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary);
	static double JP_Brewer(sGirl* girl, bool estimate);
	static bool WorkTailor(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary);
	static double JP_Tailor(sGirl* girl, bool estimate);
	static bool WorkMakeItem(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary);
	static double JP_MakeItem(sGirl* girl, bool estimate);
	static bool WorkMakePotions(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary);
	static double JP_MakePotions(sGirl* girl, bool estimate);

	// house
	static bool WorkHeadGirl(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary);
	static double JP_HeadGirl(sGirl* girl, bool estimate);
	static bool WorkRecruiter(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary);
	static double JP_Recruiter(sGirl* girl, bool estimate);
	static bool WorkPersonalTraining(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary);
	static double JP_PersonalTraining(sGirl* girl, bool estimate);
	static bool WorkPersonalBedWarmer(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary);
	static bool WorkPersonalBedWarmers(sBrothel* brothel);
	static double JP_PersonalBedWarmer(sGirl* girl, bool estimate);
	static bool WorkCleanHouse(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary);
	static double JP_CleanHouse(sGirl* girl, bool estimate);
	static bool WorkHouseVacation(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary);
	static double JP_HouseVacation(sGirl* girl, bool estimate);
	static bool WorkHouseCook(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary);
	static double JP_HouseCook(sGirl* girl, bool estimate);
	static bool WorkHousePet(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary);
	static double JP_HousePet(sGirl* girl, bool estimate);
	static bool WorkFarmPonyGirl(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary);
	static bool WorkSOStraight(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary);
	static double JP_SOStraight(sGirl* girl, bool estimate);
	static bool WorkSOBisexual(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary);
	static double JP_SOBisexual(sGirl* girl, bool estimate);
	static bool WorkSOLesbian(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary);
	static double JP_SOLesbian(sGirl* girl, bool estimate);
	static bool WorkFakeOrgasm(sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary);
	static double JP_FakeOrgasm(sGirl* girl, bool estimate);




	// - stuff that does processing for jobs

	// MYR: New code for security.  All the old code is still here, commented out.
	static bool work_related_violence(sGirl*, bool, bool);
	static int guard_coverage(vector<sGang*> *v=0);
	static bool security_stops_rape(sGirl * girl, sGang *enemy_gang, int day_night);
	//static bool gang_stops_rape(sGirl* girl, sGang *gang, int chance, int day_night);
	static bool gang_stops_rape(sGirl* girl, vector<sGang *> gangs_guarding, sGang *enemy_gang, int coverage, int day_night);
	//static bool girl_fights_rape(sGirl*, int);
	static bool girl_fights_rape(sGirl* girl, sGang *enemy_gang, int day_night);
	static void customer_rape(sGirl* girl, int numberofattackers);
	static string GetGirlAttackedString(u_int attacktype = SKILL_COMBAT);	// `J` added attacktype to be used with sextype for more specific attacks defaulting to combat


	static bool Preprocessing(int action, sGirl* girl, sBrothel* brothel, bool Day0Night1, string& summary, string& message);
	static sCustomer GetMiscCustomer(sBrothel& brothel);

	bool work_show(sGirl* girl,sBrothel* brothel,string& summary,bool Day0Night1);
	void update_film(sBrothel *);
	long make_money_films();
	void save_films(ofstream &ofs);
	void load_films(ifstream &ifs);
	bool apply_job(sGirl* girl, int job, int brothel_id, bool Day0Night1, string & message);
	int get_num_on_job(sBrothel* brothel, int job_wanted, bool Day0Night1);
	static bool is_sex_type_allowed(unsigned int sex_type, sBrothel* brothel);
	static bool nothing_banned(sBrothel* brothel);
	#ifndef _DEBUG
		static void free();
	#else
		static void freeJobs();
	#endif
	
	//helpers
	static vector<sGirl*> girls_on_job(sBrothel *brothel, u_int job_wanted, bool Day0Night1);
	// need a function for seeing if there is a girl working on a job
	bool is_job_employed(sBrothel * brothel, u_int job_wanted, bool Day0Night1);
	static void get_training_set(vector<sGirl*> &v, vector<sGirl*> &set);
	static void do_training(sBrothel* brothel, bool Day0Night1);
	static void do_training_set(vector<sGirl*> girls, bool Day0Night1);
	static void do_solo_training(sGirl *girl, bool Day0Night1);
	static void do_advertising(sBrothel* brothel, bool Day0Night1);
	static void do_whorejobs(sBrothel* brothel, bool Day0Night1);
	static void do_custjobs(sBrothel* brothel, bool Day0Night1);
	~cJobManager(){}

	bool is_job_Paid_Player(u_int Job);		//	WD:	Test for all jobs paid by player
	bool FullTimeJob(u_int Job);			//	`J`	Test if job is takes both shifts
	string GirlPaymentText(sBrothel* brothel, sGirl* girl, int totalTips, int totalPay, int totalGold, bool Day0Night1);
	void FreeSlaves(sGirl* girl, bool multi = false);
	void ffsd_choice(int ffsd, vector<int> girl_array, string buildingtype, int buildingnum);
	void ffsd_outcome(vector<int> girl_array, string sub, int num);
};
