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
#include <algorithm>
#include "cJobManager.h"
#include "cBrothel.h"
#include "cClinic.h"
#include "cMovieStudio.h"
#include "cArena.h"
#include "cCentre.h"
#include "cHouse.h"
#include "cCustomers.h"
#include "cRng.h"
#include "cInventory.h"
#include "sConfig.h"
#include "cRival.h"
#include <sstream>
#include "CLog.h"
#include "cTrainable.h"
#include "cTariff.h"
#include "cGold.h"
#include "cGangs.h"
#include "cMessageBox.h"
#include "libintl.h"

extern cRng g_Dice;
extern CLog g_LogFile;
extern cCustomers g_Customers;
extern cInventory g_InvManager;
extern cBrothelManager g_Brothels;
extern cClinicManager  g_Clinic;
extern cMovieStudioManager  g_Studios;
extern cArenaManager  g_Arena;
extern cCentreManager  g_Centre;
extern cHouseManager  g_House;
extern cGangManager g_Gangs;
extern cMessageQue g_MessageQue;

void cJobManager::Setup()
{
	// much simplier then trying to get the sJob working with this (plus a smaller memory footprint...?maybe)
	// - General
	JobFunctions[JOB_RESTING] = &WorkFreetime;
	JobFunctions[JOB_TRAINING] = &WorkTraining;
	JobFunctions[JOB_CLEANING] = &WorkCleaning;
	JobFunctions[JOB_SECURITY] = &WorkSecurity;
	JobFunctions[JOB_ADVERTISING] = &WorkAdvertising;
	JobFunctions[JOB_CUSTOMERSERVICE] = &WorkCustService;
	JobFunctions[JOB_MATRON] = &WorkMatron;
	JobFunctions[JOB_TORTURER] = &WorkTorturer;
	JobFunctions[JOB_EXPLORECATACOMBS] = &WorkExploreCatacombs;
	JobFunctions[JOB_RECRUITER] = &WorkRecruiter;
	// - Bar
	JobFunctions[JOB_BARMAID] = &WorkBarmaid;
	JobFunctions[JOB_WAITRESS] = &WorkBarWaitress;
	JobFunctions[JOB_SINGER] = &WorkBarSinger;
	JobFunctions[JOB_PIANO] = &WorkBarPiano;
	// - Brothel
	JobFunctions[JOB_WHOREBROTHEL] = &WorkWhore;
	JobFunctions[JOB_WHORESTREETS] = &WorkWhore;
	JobFunctions[JOB_BROTHELSTRIPPER] = &WorkBrothelStripper;
	JobFunctions[JOB_MASSEUSE] = &WorkBrothelMasseuse;
	JobFunctions[JOB_PEEP] = &WorkPeepShow;
	// - Sleazy Bar
	JobFunctions[JOB_SLEAZYBARMAID] = &WorkSleazyBarmaid;
	JobFunctions[JOB_SLEAZYWAITRESS] = &WorkSleazyWaitress;
	JobFunctions[JOB_BARSTRIPPER] = &WorkBarStripper;	
	JobFunctions[JOB_BARWHORE] = &WorkBarWhore;
	// - Gambling Hall
	JobFunctions[JOB_WHOREGAMBHALL] = &WorkHallWhore;
	JobFunctions[JOB_DEALER] = &WorkHallDealer;
	JobFunctions[JOB_ENTERTAINMENT] = &WorkHallEntertainer;
	JobFunctions[JOB_XXXENTERTAINMENT] = &WorkHallXXXEntertainer;
	// - Stables
	JobFunctions[JOB_BEASTCAPTURE] = &WorkBeastCapture;
	JobFunctions[JOB_BEASTCARER] = &WorkBeastCare;
	JobFunctions[JOB_MILK] = &WorkMilk;
	// - Movie Crystal Studio
	JobFunctions[JOB_FILMBEAST] = &WorkFilmBeast;
	JobFunctions[JOB_FILMGROUP] = &WorkFilmGroup;
	JobFunctions[JOB_FILMSEX] = &WorkFilmSex;	
	JobFunctions[JOB_FILMANAL] = &WorkFilmAnal;	
	JobFunctions[JOB_FILMLESBIAN] = &WorkFilmLesbian;	
	JobFunctions[JOB_FILMBONDAGE] = &WorkFilmBondage;
	JobFunctions[JOB_FILMORAL] = &WorkFilmOral;
	JobFunctions[JOB_FILMMAST] = &WorkFilmMast;
	JobFunctions[JOB_FILMTITTY] = &WorkFilmTitty;
	JobFunctions[JOB_FILMSTRIP] = &WorkFilmStrip;
//	JobFunctions[JOB_FILMRANDOM] = &WorkFilmRandom;		// This job is handled different than others, it is in cMovieStudio.cpp UpdateGirls()
	// - Film Crew
	JobFunctions[JOB_DIRECTOR] = &WorkFilmDirector;
	JobFunctions[JOB_PROMOTER] = &WorkFilmPromoter;
	JobFunctions[JOB_CAMERAMAGE] = &WorkCameraMage;	
	JobFunctions[JOB_CRYSTALPURIFIER] = &WorkCrystalPurifier;
	JobFunctions[JOB_FLUFFER] = &WorkFluffer;
	JobFunctions[JOB_STAGEHAND] = &WorkFilmStagehand;

	// - Arena (these jobs gain bonus if in same building as a clinic)
	JobFunctions[JOB_FIGHTBEASTS] = &WorkFightBeast;	// ************** TODO
	JobFunctions[JOB_FIGHTARENAGIRLS] = &WorkFightArenaGirls;	// ************** TODO
	JobFunctions[JOB_FIGHTTRAIN] = &WorkCombatTraining;	// 
	JobFunctions[JOB_DOCTORE] = &WorkDoctore;	// 
	JobFunctions[JOB_CLEANARENA] = &WorkCleanArena;	// 
	JobFunctions[JOB_CITYGUARD] = &WorkCityGuard;	// 
	// - Community Centre
	JobFunctions[JOB_FEEDPOOR] = &WorkFeedPoor;	// ************** TODO
	//JobFunctions[JOB_MAKEITEMS] = &WorkMakeItem;	// ************** TODO
	JobFunctions[JOB_COMUNITYSERVICE] = &WorkComunityService;	// ************** TODO
	JobFunctions[JOB_CENTREMANAGER] = &WorkCentreManager;	// ************** TODO
	JobFunctions[JOB_CLEANCENTRE] = &WorkCleanCentre;	// ************** TODO
	// - drug Centre
	JobFunctions[JOB_DRUGCOUNSELOR] = &WorkDrugCounselor;	// ************** TODO
	JobFunctions[JOB_REHAB] = &WorkRehab;	// ************** TODO
	// - Clinic
	JobFunctions[JOB_DOCTOR] = &WorkDoctor;
	JobFunctions[JOB_GETABORT] = &WorkGetAbort;
	JobFunctions[JOB_PHYSICALSURGERY] = &WorkPhysicalSurgery;	
	JobFunctions[JOB_HEALING] = &WorkHealing;
	JobFunctions[JOB_REPAIRSHOP] = &WorkRepairShop;	
	JobFunctions[JOB_NURSE] = &WorkNurse;
	JobFunctions[JOB_MECHANIC] = &WorkMechanic;
	//clinic staff
	JobFunctions[JOB_JANITOR] = &WorkJanitor;	
	JobFunctions[JOB_CHAIRMAN] = &WorkChairMan;
	//surgery
	JobFunctions[JOB_LIPO] = &WorkLiposuction;	
	JobFunctions[JOB_BREASTREDUCTION] = &WorkBreastReduction;
	JobFunctions[JOB_BOOBJOB] = &WorkBoobJob;
	JobFunctions[JOB_VAGINAREJUV] = &WorkGetVaginalRejuvination;
	JobFunctions[JOB_FACELIFT] = &WorkGetFacelift;
	// - House
	JobFunctions[JOB_PERSONALTRAINING] = &WorkPersonalTraining;	// ************** TODO
	JobFunctions[JOB_PERSONALBEDWARMER] = &WorkPersonalBedWarmer;	// ************** TODO
	JobFunctions[JOB_CLEANHOUSE] = &WorkCleanHouse;	// 
	JobFunctions[JOB_HEADGIRL] = &WorkHeadGirl;	// 
#if 0
	// - Community Centre
	JobFunctions[JOB_COLLECTDONATIONS] = &WorkVoid;	// ************** TODO
	JobFunctions[JOB_FEEDPOOR] = &WorkFeedPoor;	// ************** TODO
	JobFunctions[JOB_MAKEITEMS] = &WorkMakeItem;	// ************** TODO
	JobFunctions[JOB_SELLITEMS] = &WorkVoid;	// ************** TODO
	JobFunctions[JOB_COMUNITYSERVICE] = &WorkComunityService;	// ************** TODO
	// - Drug Lab (these jobs gain bonus if in same building as a clinic)
	JobFunctions[JOB_VIRASPLANTFUCKER] = &WorkVoid;	// ************** TODO
	JobFunctions[JOB_SHROUDGROWER] = &WorkVoid;	// ************** TODO
	JobFunctions[JOB_FAIRYDUSTER] = &WorkVoid;	// ************** TODO
	JobFunctions[JOB_DRUGDEALER] = &WorkVoid;	// ************** TODO
	// - Alchemist lab (these jobs gain bonus if in same building as a drug lab)
	JobFunctions[JOB_FINDREGENTS] = &WorkVoid;	// ************** TODO
	JobFunctions[JOB_BREWPOTIONS] = &WorkVoid;	// ************** TODO
	JobFunctions[JOB_POTIONTESTER] = &WorkVoid;	// ************** TODO
	// - Arena (these jobs gain bonus if in same building as a clinic)
	JobFunctions[JOB_FIGHTBEASTS] = &WorkVoid;	// ************** TODO
	JobFunctions[JOB_WRESTLE] = &WorkVoid;	// ************** TODO
	JobFunctions[JOB_FIGHTTODEATH] = &WorkVoid;	// ************** TODO
	JobFunctions[JOB_FIGHTVOLUNTEERS] = &WorkVoid;	// ************** TODO
	JobFunctions[JOB_COLLECTBETS] = &WorkVoid;	// ************** TODO
	// - Skills Centre
	JobFunctions[JOB_TEACHBDSM] = &WorkVoid;	// ************** TODO
	JobFunctions[JOB_TEACHSEX] = &WorkVoid;	// ************** TODO
	JobFunctions[JOB_TEACHBEAST] = &WorkVoid;	// ************** TODO
	JobFunctions[JOB_TEACHMAGIC] = &WorkVoid;	// ************** TODO
	JobFunctions[JOB_TEACHCOMBAT] = &WorkVoid;	// ************** TODO
	JobFunctions[JOB_DAYCARE] = &WorkVoid;	// ************** TODO
	JobFunctions[JOB_SCHOOLING] = &WorkVoid;	// ************** TODO
	JobFunctions[JOB_TEACHDANCING] = &WorkVoid;	// ************** TODO
	JobFunctions[JOB_TEACHSERVICE] = &WorkVoid;	// ************** TODO
	JobFunctions[JOB_TRAIN] = &WorkVoid;	// ************** TODO

#endif

	

/*
 *	Names and descriptions of each job and job filter, along with first job index for each job filter
 */
	// - General Jobs
	JobFilterName[JOBFILTER_GENERAL] = "General";
	JobFilterDescription[JOBFILTER_GENERAL] = "These are general jobs that work individually in any building.";
	JobFilterIndex[JOBFILTER_GENERAL] = JOB_RESTING;
	JobName[JOB_RESTING] = "Free Time";
	JobDescription[JOB_RESTING] = "She will take some time off, maybe do some shopping or walk around town. If the girl is unhappy she may try to escape.";
	JobName[JOB_TRAINING] = "Practice Skills";
	JobDescription[JOB_TRAINING] = "She will train either alone or with others to improve her skills.";
	JobName[JOB_CLEANING] = "Cleaning";
	JobDescription[JOB_CLEANING] = "She will clean the building, as filth will put off some customers.";
	JobName[JOB_SECURITY] = "Security";
	JobDescription[JOB_SECURITY] = "She will patrol the building, stopping mis-deeds.";
	JobName[JOB_ADVERTISING] = "Advertising";
	JobDescription[JOB_ADVERTISING] = "She will advertise the building's features in the city.";
	JobName[JOB_CUSTOMERSERVICE] = "Customer Service";
	JobDescription[JOB_CUSTOMERSERVICE] = "She will look after customer needs.";
	JobName[JOB_MATRON] = "Matron";
	JobDescription[JOB_MATRON] = "This girl will look after the other girls. Only non-slave girls can have this position and you must pay them 300 gold per week. Also, it takes up both shifts. (max 1)";
	JobName[JOB_TORTURER] = "Torturer";
	JobDescription[JOB_TORTURER] = "She will torture the prisoners in addition to your tortures, she will also look after them to ensure they don't die. (max 1 for all brothels)";
	JobName[JOB_EXPLORECATACOMBS] = "Explore Catacombs";
	JobDescription[JOB_EXPLORECATACOMBS] = "She will explore the catacombs looking for treasure and capturing monsters and monster girls. Needless to say, this is a dangerous job.";
	JobName[JOB_RECRUITER] = "Recruiter";
	JobDescription[JOB_RECRUITER] = "She will go out and try and recruit girls for you.";

	// - Bar Jobs
	JobFilterName[JOBFILTER_BAR] = "Bar";
	JobFilterDescription[JOBFILTER_BAR] = "These are jobs for running a bar.";
	JobFilterIndex[JOBFILTER_BAR] = JOB_BARMAID;
	JobName[JOB_BARMAID] = "Barmaid";
	JobDescription[JOB_BARMAID] = "She will staff the bar and serve drinks.";
	JobName[JOB_WAITRESS] = "Waitress";
	JobDescription[JOB_WAITRESS] = "She will bring drinks and food to customers at the tables.";
	JobName[JOB_SINGER] = "Singer";
	JobDescription[JOB_SINGER] = "She will sing for the customers.";
	JobName[JOB_PIANO] = "Piano";
	JobDescription[JOB_PIANO] = "She will play the piano for the customers.";

	// - Gambling Hall Jobs
	JobFilterName[JOBFILTER_GAMBHALL] = "Gambling Hall";
	JobFilterDescription[JOBFILTER_GAMBHALL] = "These are jobs for running a gambling hall.";
	JobFilterIndex[JOBFILTER_GAMBHALL] = JOB_DEALER;
	JobName[JOB_DEALER] = "Game Dealer";
	JobDescription[JOB_DEALER] = "She will manage a game in the gambling hall. (requires 3)";
	JobName[JOB_ENTERTAINMENT] = "Entertainer";
	JobDescription[JOB_ENTERTAINMENT] = "She will provide entertainment to the customers.";
	JobName[JOB_XXXENTERTAINMENT] = "XXX Entertainer";
	JobDescription[JOB_XXXENTERTAINMENT] = "She will provide sexual entertainment to the customers.";
	JobName[JOB_WHOREGAMBHALL] = "Hall Whore";
	JobDescription[JOB_WHOREGAMBHALL] = "She will give her sexual favors to the customers.";

	// - Sleazy Bar Jobs		// Changing all references to Strip Club, just sounds better to me and more realistic. -PP
	JobFilterName[JOBFILTER_SLEAZYBAR] = "Strip Club";
	JobFilterDescription[JOBFILTER_SLEAZYBAR] = "These are jobs for running a Strip Club.";
	JobFilterIndex[JOBFILTER_SLEAZYBAR] = JOB_SLEAZYBARMAID;
	JobName[JOB_SLEAZYBARMAID] = "Strip Club Barmaid";
	JobDescription[JOB_SLEAZYBARMAID] = "She will staff the bar and serve drinks while dressed in lingerie or fetish costumes.";
	JobName[JOB_SLEAZYWAITRESS] = "Strip Club Waitress";
	JobDescription[JOB_SLEAZYWAITRESS] = "She will bring drinks and food to the customers at the tables while dressed in lingerie or fetish costumes.";
	JobName[JOB_BARSTRIPPER] = "Strip Club Stripper";
	JobDescription[JOB_BARSTRIPPER] = "She will strip on the tables and stage for the customers.";
	JobName[JOB_BARWHORE] = "Strip Club Whore";
	JobDescription[JOB_BARWHORE] = "She will provide sex to the customers.";

	// - Brothel Jobs
	JobFilterName[JOBFILTER_BROTHEL] = "Brothel";
	JobFilterDescription[JOBFILTER_BROTHEL] = "These are jobs for running a brothel.";
	JobFilterIndex[JOBFILTER_BROTHEL] = JOB_MASSEUSE;
	JobName[JOB_MASSEUSE] = "Masseuse";
	JobDescription[JOB_MASSEUSE] = "She will give massages to the customers.";
	JobName[JOB_BROTHELSTRIPPER] = "Brothel Stripper";
	JobDescription[JOB_BROTHELSTRIPPER] = "She will strip for the customers.";
	JobName[JOB_WHOREBROTHEL] = "Brothel Whore";
	JobDescription[JOB_WHOREBROTHEL] = "She will whore herself to customers within the building's walls. This is safer but a little less profitable.";
	JobName[JOB_WHORESTREETS] = "Whore on Streets";
	JobDescription[JOB_WHORESTREETS] = "She will whore herself on the streets. It is more dangerous than whoring inside but more profitable.";
	JobName[JOB_PEEP] = "Peep Show";
	JobDescription[JOB_PEEP] = "She will let people watch her change and maybe more...";

	// - Stables Jobs
	JobFilterName[JOBFILTER_STABLES] = "Stables";
	JobFilterDescription[JOBFILTER_STABLES] = "These are jobs for caputring and caring for beasts";
	JobFilterIndex[JOBFILTER_STABLES] = JOB_BEASTCAPTURE;
	JobName[JOB_BEASTCAPTURE] = "Beast Capture";
	JobDescription[JOB_BEASTCAPTURE] = "She will go out into the wilds to capture beasts. (max 2)";
	JobName[JOB_BEASTCARER] = "Beast Carer";
	JobDescription[JOB_BEASTCARER] = "She will look after the needs of the beasts in your possession. (max 1)";
	JobName[JOB_MILK] = "Get Milked";
	JobDescription[JOB_MILK] = "She will have her breasts milked";

	// - Clinic staff
	JobFilterName[JOBFILTER_CLINICSTAFF] = gettext("Clinic Staff");
	JobFilterDescription[JOBFILTER_CLINICSTAFF] = gettext("These are jobs that help run a medical clinic.");
	JobFilterIndex[JOBFILTER_CLINICSTAFF] = JOB_CHAIRMAN;
	JobName[JOB_CHAIRMAN] = gettext("Chairman");
	JobDescription[JOB_CHAIRMAN] = gettext("She will watch over the staff of the clinic");
	JobName[JOB_DOCTOR] = gettext("Doctor");
	JobDescription[JOB_DOCTOR] = gettext("She will become a doctor. Doctors earn extra cash from treating locals. (requires 1)");
	JobName[JOB_NURSE] = gettext("Nurse");
	JobDescription[JOB_NURSE] = gettext("Will help the doctor and heal sick people.");
	JobName[JOB_MECHANIC] = gettext("Mechanic");
	JobDescription[JOB_MECHANIC] = gettext("Will help the doctor and repair Constructs.");
	JobName[JOB_JANITOR] = gettext("Janitor");
	JobDescription[JOB_JANITOR] = gettext("She will clean the clinic");
	JobName[JOB_CLINICREST] = gettext("Time off");
	JobDescription[JOB_CLINICREST] = gettext("She will rest");

	// - Clinic Jobs
	JobFilterName[JOBFILTER_CLINIC] = gettext("Medical Clinic");
	JobFilterDescription[JOBFILTER_CLINIC] = gettext("These are jobs for running a medical clinic.");
	JobFilterIndex[JOBFILTER_CLINIC] = JOB_GETABORT;
	JobName[JOB_GETABORT] = gettext("Get Abortion");
	JobDescription[JOB_GETABORT] = gettext("She will get an abortion, removing pregnancy and/or insemination.(takes 2 days)");
	JobName[JOB_PHYSICALSURGERY] = gettext("Cosmetic Surgery");
	JobDescription[JOB_PHYSICALSURGERY] = gettext("She will undergo magical surgery to \"enhance\" her appearance. (takes up to 5 days)");
	JobName[JOB_LIPO] = gettext("Liposuction");
	JobDescription[JOB_LIPO] = gettext("She will undergo liposuction to \"enhance\" her figure. (takes up to 5 days)");
	JobName[JOB_BREASTREDUCTION] = gettext("Breast Reduction Surgery");
	JobDescription[JOB_BREASTREDUCTION] = gettext("She will undergo breast reduction surgery. (takes up to 5 days)");
	JobName[JOB_BOOBJOB] = gettext("Boob Job");
	JobDescription[JOB_BOOBJOB] = gettext("She will undergo surgery to \"enhance\" her bust. (takes up to 5 days)");
	JobName[JOB_VAGINAREJUV] = gettext("Vaginal Rejuvination");
	JobDescription[JOB_VAGINAREJUV] = gettext("She will undergo surgery to make her a virgin again. (takes up to 5 days)");
	JobName[JOB_FACELIFT] = gettext("Face Lift");
	JobDescription[JOB_FACELIFT] = gettext("She will undergo surgery to make her younger. (takes up to 5 days)");
	JobName[JOB_HEALING] = gettext("Healing");
	JobDescription[JOB_HEALING] = gettext("She will have her wounds attended. This takes 1 day for each wound trait.");
	JobName[JOB_REPAIRSHOP] = gettext("Repair Shop");
	JobDescription[JOB_REPAIRSHOP] = gettext("Construct girls will be quickly repaired here.");
	

	//- Moive Jobs
	JobFilterName[JOBFILTER_MOVIESTUDIO] = gettext("Actresses");
	JobFilterDescription[JOBFILTER_MOVIESTUDIO] = gettext("These are the ladies starring in the films.");
	JobFilterIndex[JOBFILTER_MOVIESTUDIO] = JOB_FILMBEAST;
	JobName[JOB_FILMBEAST] = gettext("Film Bestiality");
	JobDescription[JOB_FILMBEAST] = gettext("She will film a bestiality scenes.");
	JobName[JOB_FILMSEX] = gettext("Film Sex");
	JobDescription[JOB_FILMSEX] = gettext("She will film a normal sex scenes.");
	JobName[JOB_FILMGROUP] = ("Film Group");
	JobDescription[JOB_FILMGROUP] = ("She will film a group sex scene.");
	JobName[JOB_FILMANAL] = gettext("Film Anal");
	JobDescription[JOB_FILMANAL] = gettext("She will film an anal scenes.");
	JobName[JOB_FILMLESBIAN] = gettext("Film Lesbian");
	JobDescription[JOB_FILMLESBIAN] = gettext("She will do a lesbian scene.");
	JobName[JOB_FILMBONDAGE] = gettext("Film Bondage");
	JobDescription[JOB_FILMBONDAGE] = gettext("She will perform a in bondage scenes.");
	JobName[JOB_FILMORAL] = ("Film Oral Sex");
	JobDescription[JOB_FILMORAL] = ("She will perform in an oral sex scene.");
	JobName[JOB_FILMMAST] = ("Film Masturbation");
	JobDescription[JOB_FILMMAST] = ("She will perform in a masturbation scene.");
	JobName[JOB_FILMTITTY] = ("Film Titty Fuck");
	JobDescription[JOB_FILMTITTY] = ("She will perform in a titty fuck scene.");
	JobName[JOB_FILMSTRIP] = ("Film Strip tease");
	JobDescription[JOB_FILMSTRIP] = ("She will perform in a strip tease scene.");
	JobName[JOB_FILMRANDOM] = ("Film a random scene");
	JobDescription[JOB_FILMRANDOM] = ("She will perform in a random sex scene.");

	//- Studio Crew
	JobFilterName[JOBFILTER_STUDIOCREW] = gettext("Studio Crew");
	JobFilterDescription[JOBFILTER_STUDIOCREW] = gettext("These are jobs for running a movie studio.");
	JobFilterIndex[JOBFILTER_STUDIOCREW] = JOB_DIRECTOR;
	JobName[JOB_DIRECTOR] = gettext("Director");
	JobDescription[JOB_DIRECTOR] = gettext("She directs the filming, and keeps the girls in line. (max 1)");
	JobName[JOB_PROMOTER] = gettext("Promoter");
	JobDescription[JOB_PROMOTER] = gettext("She advertises the movies. (max 1)");
	JobName[JOB_CAMERAMAGE] = gettext("Camera Mage");
	JobDescription[JOB_CAMERAMAGE] = gettext("She will film the scenes. (requires 1)");
	JobName[JOB_CRYSTALPURIFIER] = gettext("Crystal Purifier");
	JobDescription[JOB_CRYSTALPURIFIER] = gettext("She will clean up the filmed scenes. (requires 1)");
	JobName[JOB_FLUFFER] = gettext("Fluffer");
	JobDescription[JOB_FLUFFER] = gettext("She will keep the porn stars aroused.");
	JobName[JOB_STAGEHAND] = gettext("Stagehand");
	JobDescription[JOB_STAGEHAND] = gettext("She helps setup equipment, and keeps the studio clean.");
	JobName[JOB_FILMFREETIME] = gettext("Time off");
	JobDescription[JOB_FILMFREETIME] = gettext("She takes time off resting and recovering.");

	//- Arena
	JobFilterName[JOBFILTER_ARENA] = gettext("Arena");
	JobFilterDescription[JOBFILTER_ARENA] = gettext("These are jobs for running an arena.");
	JobFilterIndex[JOBFILTER_ARENA] = JOB_FIGHTBEASTS;
	JobName[JOB_FIGHTBEASTS] = gettext("Fight Beasts");
	JobDescription[JOB_FIGHTBEASTS] = gettext("She will fight to the death against beasts you own. Dangerous.");
	JobName[JOB_FIGHTARENAGIRLS] = gettext("Cage Match");
	JobDescription[JOB_FIGHTARENAGIRLS] = gettext("She will fight against other girls. Dangerous.");
	JobFilterIndex[JOBFILTER_ARENA] = JOB_FIGHTBEASTS;
	JobName[JOB_FIGHTTRAIN] = gettext("Combat Training");
	JobDescription[JOB_FIGHTTRAIN] = gettext("She will practice combat.");
	JobName[JOB_DOCTORE] = gettext("Doctore");
	JobDescription[JOB_DOCTORE] = gettext("She will watch over the girls in the arena.");
	JobName[JOB_CLEANARENA] = gettext("Clean Arena");
	JobDescription[JOB_CLEANARENA] = gettext("She will clean the arena.");
	JobName[JOB_ARENAREST] = gettext("Time off");
	JobDescription[JOB_ARENAREST] = gettext("She will rest.");
	JobName[JOB_CITYGUARD] = gettext("City Guard");
	JobDescription[JOB_CITYGUARD] = gettext("She will help keep crossgate safe.");

	//- Community Centre
	JobFilterName[JOBFILTER_COMMUNITYCENTRE] = gettext("Community Centre");
	JobFilterDescription[JOBFILTER_COMMUNITYCENTRE] = gettext("These are jobs for running a community centre.");
	JobFilterIndex[JOBFILTER_COMMUNITYCENTRE] = JOB_FEEDPOOR;
	JobName[JOB_FEEDPOOR] = gettext("Feed Poor");
	JobDescription[JOB_FEEDPOOR] = gettext("She will work in a soup kitchen.");
	JobName[JOB_COMUNITYSERVICE] = gettext("Community Service");
	JobDescription[JOB_COMUNITYSERVICE] = gettext("She will go around town and help out where she can.");
	JobName[JOB_CENTREMANAGER] = gettext("Centre Manager");
	JobDescription[JOB_CENTREMANAGER] = gettext("She will look after the girls working in the centre.");
	JobName[JOB_CLEANCENTRE] = gettext("Clean Centre");
	JobDescription[JOB_CLEANCENTRE] = gettext("She will clean the centre.");
	JobName[JOB_CENTREREST] = gettext("Time off");
	JobDescription[JOB_CENTREREST] = gettext("She will rest.");

	//- drug centre
	JobFilterName[JOBFILTER_DRUGCENTRE] = gettext("Drug Centre");
	JobFilterDescription[JOBFILTER_DRUGCENTRE] = gettext("These are jobs for running a drug centre.");
	JobFilterIndex[JOBFILTER_DRUGCENTRE] = JOB_DRUGCOUNSELOR;
	JobName[JOB_DRUGCOUNSELOR] = gettext("Drug Counselor");
	JobDescription[JOB_DRUGCOUNSELOR] = gettext("She will help girls get over there drug addictions.");
	JobName[JOB_REHAB] = gettext("Rehab");
	JobDescription[JOB_REHAB] = gettext("She will go to rehab to get over her drug addiction.");

	//- House
	JobFilterName[JOBFILTER_HOUSE] = gettext("Player House");
	JobFilterDescription[JOBFILTER_HOUSE] = gettext("These are jobs your girls can do at your house.");
	JobFilterIndex[JOBFILTER_HOUSE] = JOB_PERSONALTRAINING;
	JobName[JOB_PERSONALTRAINING] = gettext("Personal Training");
	JobDescription[JOB_PERSONALTRAINING] = gettext("You will over see her training personal.");
	JobName[JOB_PERSONALBEDWARMER] = gettext("Bed Warmer");
	JobDescription[JOB_PERSONALBEDWARMER] = gettext("She will stay in your bed at night with you.");
	JobName[JOB_CLEANHOUSE] = gettext("Clean House");
	JobDescription[JOB_CLEANHOUSE] = gettext("She will clean your house.");
	JobName[JOB_HEADGIRL] = gettext("Head Girl");
	JobDescription[JOB_HEADGIRL] = gettext("She takes care of the girls in your house.");
	JobName[JOB_HOUSEREST] = gettext("Time off");
	JobDescription[JOB_HOUSEREST] = gettext("She takes time off resting and recovering.");

# if 0

	//- Community Centre
	JobFilterName[JOBFILTER_COMMUNITYCENTRE] = gettext("Community Centre");
	JobFilterDescription[JOBFILTER_COMMUNITYCENTRE] = gettext("These are jobs for running a community centre.");
	JobFilterIndex[JOBFILTER_COMMUNITYCENTRE] = JOB_COLLECTDONATIONS;
	JobName[JOB_COLLECTDONATIONS] = gettext("Collect Donations");
	JobDescription[JOB_COLLECTDONATIONS] = gettext("She will collect money to help the poor.");
	JobName[JOB_FEEDPOOR] = gettext("Feed Poor");
	JobDescription[JOB_FEEDPOOR] = gettext("She will work in a soup kitchen.");
	JobName[JOB_MAKEITEMS] = gettext("Make Crafts");
	JobDescription[JOB_MAKEITEMS] = gettext("She will craft cheap handmade items.");
	JobName[JOB_SELLITEMS] = gettext("Sell Crafts");
	JobDescription[JOB_SELLITEMS] = gettext("She will go out and sell previously crafted items.");
	JobName[JOB_COMUNITYSERVICE] = gettext("Community Service");
	JobDescription[JOB_COMUNITYSERVICE] = gettext("She will go around town and help out where she can.");

	JobFilterName[JOBFILTER_DRUGLAB] = gettext("Drug Lab");
	JobFilterDescription[JOBFILTER_DRUGLAB] = gettext("These are jobs for running a drug lab.");
	JobFilterIndex[JOBFILTER_DRUGLAB] = JOB_VIRASPLANTFUCKER;
	JobName[JOB_VIRASPLANTFUCKER] = gettext("Fuck Viras Plants");
	JobDescription[JOB_VIRASPLANTFUCKER] = gettext("She will allow Viras plants to inseminate her, creating Vira Blood.");
	JobName[JOB_SHROUDGROWER] = gettext("Grow Shroud Mushrooms");
	JobDescription[JOB_SHROUDGROWER] = gettext("She will grow Shroud Mushrooms, which require a lot of care and may explode.");
	JobName[JOB_FAIRYDUSTER] = gettext("Fairy Duster");
	JobDescription[JOB_FAIRYDUSTER] = gettext("She will capture fairies and grind them into a fine powder known as Fairy Dust.");
	JobName[JOB_DRUGDEALER] = gettext("Drug Dealer");
	JobDescription[JOB_DRUGDEALER] = gettext("She will go out and sell your drugs on the streets.");

	JobFilterName[JOBFILTER_ALCHEMISTLAB] = gettext("Alchemist Lab");
	JobFilterDescription[JOBFILTER_ALCHEMISTLAB] = gettext("These are jobs for running an alchemist lab.");
	JobFilterIndex[JOBFILTER_ALCHEMISTLAB] = JOB_FINDREGENTS;
	JobName[JOB_FINDREGENTS] = gettext("Find Reagents");
	JobDescription[JOB_FINDREGENTS] = gettext("She will go out and look for promising potion ingredients.");
	JobName[JOB_BREWPOTIONS] = gettext("Brew Potions");
	JobDescription[JOB_BREWPOTIONS] = gettext("She will brew a random potion using available ingredients.");
	JobName[JOB_POTIONTESTER] = gettext("Guinea Pig");
	JobDescription[JOB_POTIONTESTER] = gettext("She will test out random potions. Who knows what might happen? (requires 1)");

	JobFilterName[JOBFILTER_ARENA] = gettext("Arena");
	JobFilterDescription[JOBFILTER_ARENA] = gettext("These are jobs for running an arena.");
	JobFilterIndex[JOBFILTER_ARENA] = JOB_FIGHTBEASTS;
	JobName[JOB_FIGHTBEASTS] = gettext("Fight Beasts");
	JobDescription[JOB_FIGHTBEASTS] = gettext("She will fight to the death against beasts you own. Dangerous.");
	JobName[JOB_WRESTLE] = gettext("Wrestle");
	JobDescription[JOB_WRESTLE] = gettext("She will take part in a wrestling match. Not dangerous.");
	JobName[JOB_FIGHTTODEATH] = gettext("Cage Match");
	JobDescription[JOB_FIGHTTODEATH] = gettext("She will fight to the death against other girls. Dangerous.");
	JobName[JOB_FIGHTVOLUNTEERS] = gettext("Fight Volunteers");
	JobDescription[JOB_FIGHTVOLUNTEERS] = gettext("She will fight against customer volunteers for gold prizes. Not dangerous.");
	JobName[JOB_COLLECTBETS] = gettext("Collect Bets");
	JobDescription[JOB_COLLECTBETS] = gettext("She will collect bets from the customers. (requires 1)");

	JobFilterName[JOBFILTER_TRAININGCENTRE] = gettext("Training Centre");
	JobFilterDescription[JOBFILTER_TRAININGCENTRE] = gettext("These are jobs for running a training centre.");
	JobFilterIndex[JOBFILTER_TRAININGCENTRE] = JOB_TEACHBDSM;
	JobName[JOB_TEACHBDSM] = gettext("Teach BDSM");
	JobDescription[JOB_TEACHBDSM] = gettext("She will teach other girls in BDSM skills.");
	JobName[JOB_TEACHSEX] = gettext("Teach Sex");
	JobDescription[JOB_TEACHSEX] = gettext("She will teach other girls the ins and outs of regular sex.");
	JobName[JOB_TEACHBEAST] = gettext("Teach Bestiality");
	JobDescription[JOB_TEACHBEAST] = gettext("She will teach other girls how to safely fuck beasts.");
	JobName[JOB_TEACHMAGIC] = gettext("Teach Magic");
	JobDescription[JOB_TEACHMAGIC] = gettext("She will teach other girls how to wield magic.");
	JobName[JOB_TEACHCOMBAT] = gettext("Teach Combat");
	JobDescription[JOB_TEACHCOMBAT] = gettext("She will teach other girls how to fight.");
	JobName[JOB_DAYCARE] = gettext("Daycare");
	JobDescription[JOB_DAYCARE] = gettext("She will look after local children. (non-sexual)");
	JobName[JOB_SCHOOLING] = gettext("Schooling");
	JobDescription[JOB_SCHOOLING] = gettext("She will teach the local children. (non-sexual)");
	JobName[JOB_TEACHDANCING] = gettext("Teach Dancing");
	JobDescription[JOB_TEACHDANCING] = gettext("She will teach other girls dancing and social skills.");
	JobName[JOB_TEACHSERVICE] = gettext("Teach Service");
	JobDescription[JOB_TEACHSERVICE] = gettext("She will teach other girls a variety of service skills.");
	JobName[JOB_TRAIN] = gettext("Student");
	JobDescription[JOB_TRAIN] = gettext("She will learn from all of the other girls who are teaching here.");

#endif

	// extra JobFilterIndex, needed by FOR loops (looping from JobFilterIndex[XYZ] to JobFilterIndex[XYZ+1])
	JobFilterIndex[NUMJOBTYPES] = NUM_JOBS;

}

#ifdef _DEBUG
void cJobManager::freeJobs()
{
}
#else
void cJobManager::free()
{
}
#endif

// ----- Misc

bool cJobManager::WorkVoid(sGirl* girl, sBrothel* brothel, int DayNight, string& summary)
{
	summary += gettext("This job isn't implemented yet");
	girl->m_Events.AddMessage(gettext("This job isn't implemented yet"), IMGTYPE_PROFILE, EVENT_DEBUG);
	return false;
}

bool cJobManager::Preprocessing(int action, sGirl* girl, sBrothel* brothel, int DayNight, string& summary, string& message)
{
	brothel->m_Filthiness++;
	g_Girls.AddTiredness(girl);

	if(g_Girls.DisobeyCheck(girl, action, brothel))			// they refuse to have work 

	{

//		summary += "She has refused to work.\n";			// WD:	Refusal message done in cBrothelManager::UpdateGirls()
		string message = girl->m_Realname + gettext(" refused to work during the ");
		if(DayNight == 0)
			message += gettext("day");
		else
			message += gettext("night");
		message += gettext(" Shift.");

		girl->m_Events.AddMessage(message, IMGTYPE_PROFILE, EVENT_NOWORK);

		return true;
	}
	return false;
}

void cJobManager::GetMiscCustomer(sBrothel* brothel, sCustomer& cust)
{
	g_Customers.GetCustomer(cust, brothel);
	brothel->m_MiscCustomers+=1;
}

bool cJobManager::is_sex_type_allowed(unsigned int sex_type, sBrothel* brothel)
{
	if(sex_type == SKILL_ANAL && brothel->m_RestrictAnal)
		return false;
	else if(sex_type == SKILL_BEASTIALITY && brothel->m_RestrictBeast)
		return false;
	else if(sex_type == SKILL_BDSM && brothel->m_RestrictBDSM)
		return false;
	else if(sex_type == SKILL_NORMALSEX && brothel->m_RestrictNormal)
		return false;
	else if(sex_type == SKILL_ORALSEX && brothel->m_RestrictOral)
		return false;
	else if(sex_type == SKILL_TITTYSEX && brothel->m_RestrictTitty)
		return false;
	else if(sex_type == SKILL_GROUP && brothel->m_RestrictGroup)
		return false;
	else if(sex_type == SKILL_LESBIAN && brothel->m_RestrictLesbian)
		return false;

	return true;
}

// ----- Job related

vector<sGirl*> cJobManager::girls_on_job(sBrothel *brothel, u_int job_wanted, int day_or_night)
{
	u_int job_id;
	sGirl* girl;
	vector<sGirl*> v;

	for(girl = brothel->m_Girls; girl; girl = girl->m_Next) {
		if (day_or_night == 0)	
		    job_id = girl->m_DayJob;
		else			
			job_id = girl->m_NightJob;
/*
 *		having sorted that out
 */
		if(job_id != job_wanted) 
			continue;
		
		v.push_back(girl);
	}
	return v;
}

bool cJobManager::is_job_employed(sBrothel * brothel,u_int job_wanted,int day_or_night)
{
	u_int job_id;
	sGirl* girl;
	for(girl = brothel->m_Girls; girl; girl = girl->m_Next) {
		if(day_or_night == 0)	
			job_id = girl->m_DayJob;
		else			
			job_id = girl->m_NightJob;
		if(job_id == job_wanted)
		return true;
	}
	return false;
}

void cJobManager::do_advertising(sBrothel* brothel, int DayNight)
{  // advertising jobs are handled before other jobs, more particularly before customer generation
	brothel->m_AdvertisingLevel = 1.0;  // base multiplier
	sGirl* current = brothel->m_Girls;


	while(current)
	{
		string summary = "";
		bool refused = false;
		if((current->m_DayJob == JOB_ADVERTISING) && (DayNight == 0)) // Added test for current shift, was running each shift twice -PP
		{
			refused = WorkAdvertising(current,brothel,0,summary);

			if(refused)	// if she refused she still gets tired
				g_Girls.AddTiredness(current);
		}
		if((current->m_NightJob == JOB_ADVERTISING) && (DayNight == 1)) // Added test for current shift, was running each shift twice -PP
		{
			refused = WorkAdvertising(current,brothel,1,summary);

			if(refused)	// if she refused she still gets tired
				g_Girls.AddTiredness(current);
		}
		current = current->m_Next;
	}
}
void cJobManager::do_whorejobs(sBrothel* brothel, int DayNight)
{ // Whores get first crack at any customers.
	sGirl* current = brothel->m_Girls;
	
	
	
	while(current)
	{
		string summary = "";
		u_int sw = 0;						//	Job type
		if(current->m_JustGaveBirth)		// if she gave birth, let her rest this week
			sw = JOB_RESTING;
		else
			sw = (DayNight == SHIFT_DAY) ? current->m_DayJob : current->m_NightJob;
		
		bool refused = false;
		if(sw == JOB_WHOREBROTHEL)
		{
			refused = WorkWhore(current,brothel,DayNight,summary);

			if(refused)	// if she refused she still gets tired
				g_Girls.AddTiredness(current);
		}
		if(sw == JOB_BARWHORE)
		{
			refused = WorkBarWhore(current,brothel,DayNight,summary);

			if(refused)	// if she refused she still gets tired
				g_Girls.AddTiredness(current);
		}
		if(sw == JOB_WHOREGAMBHALL)
		{
			refused = WorkHallWhore(current,brothel,DayNight,summary);

			if(refused)	// if she refused she still gets tired
				g_Girls.AddTiredness(current);
		}
			
		current = current->m_Next;
	}
	
}
void cJobManager::do_custjobs(sBrothel* brothel, int DayNight)
{ // Customer taking jobs get first crack at any customers before customer service.
	sGirl* current = brothel->m_Girls;
	
	
	
	while(current)
	{
		string summary = "";
		u_int sw = 0;						//	Job type
		if(current->m_JustGaveBirth)		// if she gave birth, let her rest this week
			sw = JOB_RESTING;
		else
			sw = (DayNight == SHIFT_DAY) ? current->m_DayJob : current->m_NightJob;
		
		bool refused = false;
		switch(sw)
		{
		case JOB_BARMAID:
			refused = WorkBarmaid(current,brothel,DayNight,summary);
			break;
		case JOB_WAITRESS:
			refused = WorkBarWaitress(current,brothel,DayNight,summary);
			break;
		case JOB_SINGER:
			refused = WorkBarSinger(current,brothel,DayNight,summary);
			break;
		/*case JOB_PIANO:
			refused = WorkBarPiano(current,brothel,DayNight,summary);
			break;*/
		case JOB_DEALER:
			refused = WorkHallDealer(current,brothel,DayNight,summary);
			break;
		case JOB_ENTERTAINMENT:
			refused = WorkHallEntertainer(current,brothel,DayNight,summary);
			break;
		case JOB_XXXENTERTAINMENT:
			refused = WorkHallXXXEntertainer(current,brothel,DayNight,summary);
			break;
		case JOB_SLEAZYBARMAID:
			refused = WorkSleazyBarmaid(current,brothel,DayNight,summary);
			break;
		case JOB_SLEAZYWAITRESS:
			refused = WorkSleazyWaitress(current,brothel,DayNight,summary);
			break;
		case JOB_BARSTRIPPER:
			refused = WorkBarStripper(current,brothel,DayNight,summary);
			break;
		case JOB_MASSEUSE:
			refused = WorkBrothelMasseuse(current,brothel,DayNight,summary);
			break;
		case JOB_BROTHELSTRIPPER:
			refused = WorkBrothelStripper(current,brothel,DayNight,summary);
			break;
		//case JOB_PEEP:
		//	refused = WorkPeepShow(current,brothel,DayNight,summary);
		//	break;
		//case JOB_BEASTCAPTURE:
		//	refused = WorkBeastCapture(current,brothel,DayNight,summary);
		//	break;
		//case JOB_BEASTCARER:
		//	refused = WorkBeastCare(current,brothel,DayNight,summary);
		//	break;
		//case JOB_MILK:
		//	refused = WorkMilk(current,brothel,DayNight,summary);
			break;
		default:
			break;
		}
		if(refused)	
				g_Girls.AddTiredness(current);
			
		current = current->m_Next;
	}
	
}
int cJobManager::get_num_on_job(int job, int brothel_id, bool day_or_night)
{
	return 0;
}

bool cJobManager::is_job_Paid_Player(u_int Job)
{
/*
 *	WD:	Tests for jobs that the player pays the wages for
 *
 *	Jobs that earn money for the player are commented out
 *	
 */

	return (
		// ALL JOBS

		// - General
		Job ==	JOB_RESTING				||	// relaxes and takes some time off
		Job ==	JOB_TRAINING			||	// trains skills at a basic level
		Job ==	JOB_CLEANING			||	// cleans the building
		Job ==	JOB_SECURITY			||	// Protects the building and its occupants
		Job ==	JOB_ADVERTISING			||	// Goes onto the streets to advertise the buildings services
		Job ==	JOB_CUSTOMERSERVICE		||	// looks after customers needs (customers are happier when people are doing this job)
		Job ==	JOB_MATRON				||	// looks after the needs of the girls (only 1 allowed)
		Job ==	JOB_TORTURER			||	// Tortures the people in the dungeons to help break their will (this is in addition to player torture) (only 1 allowed)
		//Job ==	JOB_EXPLORECATACOMBS	||	// goes adventuring in the catacombs
		Job ==	JOB_BEASTCAPTURE		||	// captures beasts for use (only 2 allowed) (creates beasts resource)
		Job ==	JOB_BEASTCARER			||	// cares for captured beasts (only 1 allowed) (creates beasts resource and lowers their loss)

		// Clinic
		Job ==	JOB_DOCTOR				||	// becomes a doctor (requires 1) (will make some extra cash for treating locals)
		Job ==	JOB_CHAIRMAN			||	//
		Job ==	JOB_JANITOR			    ||	//
		
		// - Movie Crystal Studio
		Job ==	JOB_FILMBEAST			||	// films this sort of scene in the movie (uses beast resource)
		Job ==	JOB_FILMSEX				||	// films this sort of scene in the movie
		Job ==	JOB_FILMANAL			||	// films this sort of scene in the movie
		Job ==	JOB_FILMLESBIAN			||	// films this sort of scene in the movie. thinking about changing to Lesbian
		Job ==	JOB_FILMBONDAGE			||	// films this sort of scene in the movie
		Job ==	JOB_FILMGROUP			||	// films this sort of scene in the movie
		Job ==	JOB_FILMORAL			||	// films this sort of scene in the movie
		Job ==	JOB_FILMMAST			||	// films this sort of scene in the movie
		Job ==	JOB_FILMTITTY			||	// films this sort of scene in the movie
		Job ==	JOB_FILMSTRIP			||	// films this sort of scene in the movie
		Job ==	JOB_FILMRANDOM			||	// Films a random sex scene
		Job ==	JOB_DIRECTOR			||	// Direcets the movies
		Job ==	JOB_PROMOTER			||	// Advertises the studio's films
		Job ==	JOB_CAMERAMAGE			||	// Uses magic to record the scenes to crystals (requires at least 1)
		Job ==	JOB_CRYSTALPURIFIER		||	// Post editing to get the best out of the film (requires at least 1)
		Job ==	JOB_FLUFFER				||	// Keeps the porn stars and animals aroused
		Job ==	JOB_STAGEHAND			||	// Sets up equipment, and keeps studio clean
		Job ==	JOB_FILMFREETIME		||	// Taking a day off

		// - Community Centre
		Job ==	JOB_FEEDPOOR			||	// work in a soup kitchen
		Job ==	JOB_COMUNITYSERVICE		||	// Goes around town helping where they can
		Job ==	JOB_CENTREMANAGER		||	// work in a soup kitchen
		Job ==	JOB_CLEANCENTRE		    ||	// Goes around town helping where they can

		//arena
		Job ==	JOB_DOCTORE		        ||	// 
		Job ==	JOB_CLEANARENA		    ||	//


		//house
		Job ==	JOB_PERSONALBEDWARMER	||	// 
		Job ==	JOB_CLEANHOUSE			||	// 
		Job ==	JOB_RECRUITER			||	// 
		Job ==	JOB_HEADGIRL			||	//

		// - Brothel
		//Job ==	JOB_WHOREBROTHEL		||	// whore herself inside the building
		//Job ==	JOB_WHORESTREETS		||	// whore self on the city streets
		//Job ==	JOB_BROTHELSTRIPPER		||	// strips for customers and sometimes sex
		//Job ==	JOB_MASSEUSE			||	// gives massages to patrons and sometimes sex

		// - Gambling Hall - Using WorkHall() or WorkWhore()
		//Job ==	JOB_WHOREGAMBHALL		||	// looks after customers sexual needs
		//Job ==	JOB_DEALER				||	// dealer for gambling tables (requires at least 3 for all jobs in this filter to work)
		//Job ==	JOB_ENTERTAINMENT		||	// sings, dances and other shows for patrons
		//Job ==	JOB_XXXENTERTAINMENT	||	// naughty shows for patrons

		// - Bar Using - WorkBar() or WorkWhore()
		//Job ==	JOB_BARMAID				||	// serves at the bar (requires 1 at least)
		//Job ==	JOB_WAITRESS			||	// waits on the tables (requires 1 at least)
		//Job ==	JOB_STRIPPER			||	// strips for the customers and gives lapdances
		//Job ==	JOB_WHOREBAR			||	// offers sexual services to customers
		//Job ==	JOB_SINGER				||	// sings int the bar (girl needs singer trait)

/*		
 *		Not Implemented
 */

#if 0
		
		// - Community Centre
		Job ==	JOB_COLLECTDONATIONS	||	// collects money to help the poor
		Job ==	JOB_FEEDPOOR			||	// work in a soup kitchen
		Job ==	JOB_MAKEITEMS			||	// makes cheap crappy handmade items for selling to raise money (creates handmade item resource)
		Job ==	JOB_SELLITEMS			||	// goes out and sells the made items (sells handmade item resource)
		Job ==	JOB_COMUNITYSERVICE		||	// Goes around town helping where they can

		// - Drug Lab (these jobs gain bonus if in same building as a clinic)
		Job ==	JOB_VIRASPLANTFUCKER	||	// the plants need to inseminate with other humanoids to proper-gate, creates vira blood items
		Job ==	JOB_SHROUDGROWER		||	// They require lots of care, and may explode. Produces shroud mushroom item.
		Job ==	JOB_FAIRYDUSTER			||	// captures and pounds faries to dust, produces fairy dust item
		Job ==	JOB_DRUGDEALER			||	// goes out onto the streets to sell the items made with the other jobs

		// - Alchemist lab (these jobs gain bonus if in same building as a drug lab)
		Job ==	JOB_FINDREGENTS			||	// goes around and finds ingredients for potions, produces ingredients resource.
		Job ==	JOB_BREWPOTIONS			||	// will randomly produce any items with the word "potion" in the name, uses ingredients resource
		Job ==	JOB_POTIONTESTER		||	// Potion tester tests potions, they may die doing this or random stuff may happen. (requires 1)

		// - Arena (these jobs gain bonus if in same building as a clinic)
		Job ==	JOB_FIGHTBEASTS			||	// customers come to place bets on who will win, girl may die (uses beasts resource)
		Job ==	JOB_WRESTLE				||	// as above no chance of dying
		Job ==	JOB_FIGHTTODEATH		||	// as above but against other girls (chance of dying)
		Job ==	JOB_FIGHTVOLUNTEERS		||	// fight against customers who volunteer for prizes of gold
		Job ==	JOB_COLLECTBETS			||	// collects the bets from customers (requires 1)

		// - Skills Centre
		Job ==	JOB_TEACHBDSM			||	// teaches BDSM skills
		Job ==	JOB_TEACHSEX			||	// teaches general sex skills
		Job ==	JOB_TEACHBEAST			||	// teaches beastiality skills
		Job ==	JOB_TEACHMAGIC			||	// teaches magic skills
		Job ==	JOB_TEACHCOMBAT			||	// teaches combat skills
		Job ==	JOB_DAYCARE				||	// looks after the local kids (non sex stuff of course)
		Job ==	JOB_SCHOOLING			||	// teaches the local kids (non sex stuff of course)
		Job ==	JOB_TEACHDANCING		||	// teaches dancing and social skills
		Job ==	JOB_TEACHSERVICE		||	// teaches misc skills
		Job ==	JOB_TRAIN				||	// trains the girl in all the disicplince for which there is a teacher

		// - Clinic
		Job ==	JOB_DOCTOR				||	// becomes a doctor (requires 1) (will make some extra cash for treating locals)
		Job ==	JOB_GETABORT			||	// gets an abortion (takes 2 days)
		Job ==	JOB_PHYSICALSURGERY		||	// magical plastic surgery (takes 5 days)
		Job ==	JOB_HEALING				||	// takes 1 days for each wound trait received.
		Job ==	JOB_REPAIRSHOP			||	// construct girls can get repaired quickly
#endif

		// - extra unassignable
		//Job ==	JOB_INDUNGEON			||
		//Job ==	JOB_RUNAWAY				||

		false );


}

string cJobManager::JobDescriptionCount(int job_id, int brothel_id, bool day, bool isClinic, bool isStudio, bool isArena, bool isCentre, bool isHouse)
{
	stringstream text;
	text << JobName[job_id];
	text << " (";
	if (isStudio)
	{
		text << g_Studios.GetNumGirlsOnJob(0, job_id, day);
	}
	else if (isClinic)
	{
		text << g_Clinic.GetNumGirlsOnJob(0, job_id, day);
	}
	else if (isArena)
	{
		text << g_Arena.GetNumGirlsOnJob(0, job_id, day);
	}
	else if (isCentre)
	{
		text << g_Centre.GetNumGirlsOnJob(0, job_id, day);
	}
	else if (isHouse)
	{
		text << g_House.GetNumGirlsOnJob(0, job_id, day);
	}
	else 
	{
		text << g_Brothels.GetNumGirlsOnJob(brothel_id, job_id, day);
	}
	text << ")";
	return text.str();
}

bool cJobManager::HandleSpecialJobs(int TargetBrothel, sGirl* Girl, int JobID, int OldJobID, bool DayOrNight)
{
	bool MadeChanges = true;  // whether a special case applies to specified job or not
	Girl->m_WorkingDay = 0; // TODO put this in method in cGirl
	// Special cases?
	if(u_int(JobID) == JOB_MATRON)
	{
		if(g_Brothels.GetNumGirlsOnJob(TargetBrothel, JOB_MATRON, DayOrNight) == 1)
			g_MessageQue.AddToQue(gettext("You can only have one matron per brothel."), 0);
		else if(Girl->m_States&(1<<STATUS_SLAVE))
			g_MessageQue.AddToQue(gettext("The matron cannot be a slave."), 0);
		else
			Girl->m_NightJob = Girl->m_DayJob = JOB_MATRON;
	}
	else if(u_int(JobID) == JOB_DIRECTOR)
	{
		if(g_Studios.GetNumGirlsOnJob(TargetBrothel, JOB_DIRECTOR, DayOrNight) == 1)
			g_MessageQue.AddToQue(gettext("There can be only one Director!"), 0);
		else if(Girl->m_States&(1<<STATUS_SLAVE))
			g_MessageQue.AddToQue(gettext("The Director cannot be a slave."), 0);
		else
			Girl->m_NightJob = JOB_DIRECTOR;
	}
	else if(u_int(JobID) == JOB_HEADGIRL)
	{
		if(g_House.GetNumGirlsOnJob(TargetBrothel, JOB_HEADGIRL, DayOrNight) == 1)
			g_MessageQue.AddToQue(gettext("There can be only one Head girl!"), 0);
		else if(Girl->m_States&(1<<STATUS_SLAVE))
			g_MessageQue.AddToQue(gettext("The head girl cannot be a slave."), 0);
		else
			Girl->m_NightJob = Girl->m_DayJob = JOB_HEADGIRL;
	}
	else if(u_int(JobID) == JOB_DOCTORE)
	{
		if(g_House.GetNumGirlsOnJob(TargetBrothel, JOB_DOCTORE, DayOrNight) == 1)
			g_MessageQue.AddToQue(gettext("There can be only one Doctore!"), 0);
		else if(Girl->m_States&(1<<STATUS_SLAVE))
			g_MessageQue.AddToQue(gettext("The Doctore cannot be a slave."), 0);
		else
			Girl->m_NightJob = Girl->m_DayJob = JOB_DOCTORE;
	}
	else if(u_int(JobID) == JOB_CHAIRMAN)
	{
		if(g_House.GetNumGirlsOnJob(TargetBrothel, JOB_CHAIRMAN, DayOrNight) == 1)
			g_MessageQue.AddToQue(gettext("There can be only one Chairman!"), 0);
		else if(Girl->m_States&(1<<STATUS_SLAVE))
			g_MessageQue.AddToQue(gettext("The Chairman cannot be a slave."), 0);
		else
			Girl->m_NightJob = Girl->m_DayJob = JOB_CHAIRMAN;
	}
	else if(u_int(JobID) == JOB_CENTREMANAGER)
	{
		if(g_House.GetNumGirlsOnJob(TargetBrothel, JOB_CENTREMANAGER, DayOrNight) == 1)
			g_MessageQue.AddToQue(gettext("There can be only one Centre Manager!"), 0);
		else if(Girl->m_States&(1<<STATUS_SLAVE))
			g_MessageQue.AddToQue(gettext("The Centre Manager cannot be a slave."), 0);
		else
			Girl->m_NightJob = Girl->m_DayJob = JOB_CENTREMANAGER;
	}
	else if(u_int(JobID) == JOB_PROMOTER)
	{
		if(g_Studios.GetNumGirlsOnJob(TargetBrothel, JOB_PROMOTER, DayOrNight) == 1)
			g_MessageQue.AddToQue(gettext("There can be only one Promoter."), 0);
		else
			Girl->m_NightJob = JOB_PROMOTER;
	}
	else if(u_int(JobID) == JOB_DOCTOR)
	{ 
		if(Girl->m_States&(1<<STATUS_SLAVE))
			g_MessageQue.AddToQue(gettext("The doctor cannot be a slave."), 0);
		else
			Girl->m_NightJob = Girl->m_DayJob = JOB_DOCTOR;
	}
	else if(u_int(JobID) == JOB_DRUGCOUNSELOR)
	{ 
		if(g_Centre.GetNumGirlsOnJob(TargetBrothel, JOB_DRUGCOUNSELOR, DayOrNight) == 1)
			g_MessageQue.AddToQue(gettext("There can be only one drug counselor!"), 0);
		else if(Girl->m_States&(1<<STATUS_SLAVE))
			g_MessageQue.AddToQue(gettext("The drug counselor cannot be a slave."), 0);
		else
			Girl->m_NightJob = Girl->m_DayJob = JOB_DRUGCOUNSELOR;
	}
	else if(u_int(JobID) == JOB_TORTURER)
	{
		if(g_Brothels.GetNumGirlsOnJob(-1, JOB_TORTURER, DayOrNight) == 1)
			g_MessageQue.AddToQue(gettext("You can only have one torturer among all of your brothels."), 0);
		else if(Girl->m_States&(1<<STATUS_SLAVE))
			g_MessageQue.AddToQue(gettext("The torturer cannot be a slave."), 0);
		else
			Girl->m_NightJob = Girl->m_DayJob = JOB_TORTURER;
	}
	else if(u_int(JobID) == JOB_RECRUITER)
	{
		if(g_House.GetNumGirlsOnJob(-1, JOB_RECRUITER, DayOrNight) == 1)
			g_MessageQue.AddToQue(gettext("You can only have one recruiter among all of your brothels."), 0);
		else if(Girl->m_States&(1<<STATUS_SLAVE))
			g_MessageQue.AddToQue(gettext("The recruiter cannot be a slave."), 0);
		else
			Girl->m_NightJob = Girl->m_DayJob = JOB_RECRUITER;
	}
	else if(u_int(JobID) == JOB_HEALING)
	{
		if(g_Clinic.GetNumGirlsOnJob(-1, JOB_DOCTOR, DayOrNight) == 0)
		{
			g_MessageQue.AddToQue(gettext("You must have a doctor for healing."), 0);
			if(DayOrNight)
				Girl->m_DayJob = JOB_RESTING;
			else
				Girl->m_NightJob = JOB_RESTING;
		}
		else 
		{
			Girl->m_DayJob = Girl->m_NightJob = JOB_HEALING;				
		}
	}
	else if(u_int(JobID) == JOB_GETABORT)
	{
		if(g_Clinic.GetNumGirlsOnJob(-1, JOB_DOCTOR, DayOrNight) == 0)
		{
			g_MessageQue.AddToQue(gettext("You must have a doctor for the abortion."), 0);
			if(DayOrNight)
				Girl->m_DayJob = Girl->m_NightJob = JOB_RESTING;
		}
		else if (!Girl->is_pregnant())
		{
			g_MessageQue.AddToQue(gettext("Oops, the girl is not pregant."), 0);
		}
		else 
		{
			Girl->m_DayJob = Girl->m_NightJob = JOB_GETABORT;
		}
	}
	else if(u_int(JobID) == JOB_PHYSICALSURGERY)
	{
		if(g_Clinic.GetNumGirlsOnJob(-1, JOB_DOCTOR, DayOrNight) == 0)
		{
			g_MessageQue.AddToQue(gettext("You must have a doctor for that operation."), 0);
			if(DayOrNight)
				Girl->m_DayJob = Girl->m_NightJob = JOB_RESTING;
		}
		else 
		{
			Girl->m_DayJob = Girl->m_NightJob = JOB_PHYSICALSURGERY;
		}
	}
	else if(u_int(JobID) == JOB_REPAIRSHOP)
	{
		if(g_Clinic.GetNumGirlsOnJob(-1, JOB_DOCTOR, DayOrNight) == 0)
		{
			g_MessageQue.AddToQue(gettext("You must have a doctor for that operation."), 0);
			if(DayOrNight)
				Girl->m_DayJob = Girl->m_NightJob = JOB_RESTING;
		}
		else if (g_Girls.HasTrait(Girl, "Construct"))
		{
			Girl->m_DayJob = Girl->m_NightJob = JOB_REPAIRSHOP;
			}
		else
			{
			g_MessageQue.AddToQue(gettext("You must be a Construct for this operation."), 0);
		}
	}
	else if(u_int(JobID) == JOB_BREASTREDUCTION)
	{
		if(g_Clinic.GetNumGirlsOnJob(-1, JOB_DOCTOR, DayOrNight) == 0)
		{
			g_MessageQue.AddToQue(gettext("You must have a doctor for that operation."), 0);
			if(DayOrNight)
				Girl->m_DayJob = Girl->m_NightJob = JOB_RESTING;
		}
		else if (!g_Girls.HasTrait(Girl, "Small Boobs"))
		{
			Girl->m_DayJob = Girl->m_NightJob = JOB_BREASTREDUCTION;
			}
		else
			{
			g_MessageQue.AddToQue(gettext("Her boobs can't get no smaller."), 0);
		}
	}
	else if(u_int(JobID) == JOB_BOOBJOB)
	{
		if(g_Clinic.GetNumGirlsOnJob(-1, JOB_DOCTOR, DayOrNight) == 0)
		{
			g_MessageQue.AddToQue(gettext("You must have a doctor for that operation."), 0);
			if(DayOrNight)
				Girl->m_DayJob = Girl->m_NightJob = JOB_RESTING;
		}
		else if (!g_Girls.HasTrait(Girl, "Abnormally Large Boobs"))
		{
			Girl->m_DayJob = Girl->m_NightJob = JOB_BOOBJOB;
			}
		else
			{
			g_MessageQue.AddToQue(gettext("Her boobs can't get no bigger."), 0);
		}
	}
	else if(u_int(JobID) == JOB_FACELIFT)
	{
		if(g_Clinic.GetNumGirlsOnJob(-1, JOB_DOCTOR, DayOrNight) == 0)
		{
			g_MessageQue.AddToQue(gettext("You must have a doctor for that operation."), 0);
			if(DayOrNight)
				Girl->m_DayJob = Girl->m_NightJob = JOB_RESTING;
		}
		else if (g_Girls.GetStat(Girl, STAT_AGE) >= 20)
		{
			Girl->m_DayJob = Girl->m_NightJob = JOB_FACELIFT;
			}
		else
			{
			g_MessageQue.AddToQue(gettext("She is to young for a face lift."), 0);
		}
	}
	else if(u_int(JobID) == JOB_VAGINAREJUV)
	{
		if(g_Clinic.GetNumGirlsOnJob(-1, JOB_DOCTOR, DayOrNight) == 0)
		{
			g_MessageQue.AddToQue(gettext("You must have a doctor for that operation."), 0);
			if(DayOrNight)
				Girl->m_DayJob = Girl->m_NightJob = JOB_RESTING;
		}
		else if (Girl->m_Virgin)
		{
			g_MessageQue.AddToQue(gettext("She is a virgin and has no need of this operation."), 0);
			}
		else
			{
				Girl->m_DayJob = Girl->m_NightJob = JOB_VAGINAREJUV;
		}
	}
	else if(u_int(JobID) == JOB_LIPO)
	{
		if(g_Clinic.GetNumGirlsOnJob(-1, JOB_DOCTOR, DayOrNight) == 0)
		{
			g_MessageQue.AddToQue(gettext("You must have a doctor for that operation."), 0);
			if(DayOrNight)
				Girl->m_DayJob = Girl->m_NightJob = JOB_RESTING;
		}
		else if (!g_Girls.HasTrait(Girl, "Great Figure"))
		{
			Girl->m_DayJob = Girl->m_NightJob = JOB_LIPO;
			}
		else
			{
			g_MessageQue.AddToQue(gettext("She already has a great figure and doesn't need this."), 0);
		}
	}
	else if(u_int(JobID) == JOB_REHAB)
	{
		if(g_Centre.GetNumGirlsOnJob(-1, JOB_DRUGCOUNSELOR, DayOrNight) == 0)
		{
			g_MessageQue.AddToQue(gettext("You must have a drug counselor for rehab."), 0);
			if(DayOrNight)
				Girl->m_DayJob = Girl->m_NightJob = JOB_RESTING;
		}
		else if (g_Girls.HasTrait(Girl, "Shroud Addict") || g_Girls.HasTrait(Girl, "Fairy Dust Addict") || g_Girls.HasTrait(Girl, "Viras Blood Addict"))
		{
			Girl->m_DayJob = Girl->m_NightJob = JOB_REHAB;
			}
		else
			{
			g_MessageQue.AddToQue(gettext("She has no addiction."), 0);
		}
	}
	else if(u_int(JobID) == JOB_FILMANAL ||
		u_int(JobID) == JOB_FILMSEX ||
		u_int(JobID) == JOB_FILMBEAST ||
		u_int(JobID) == JOB_FILMBONDAGE ||
		u_int(JobID) == JOB_FILMGROUP ||
		u_int(JobID) == JOB_FILMORAL ||
		u_int(JobID) == JOB_FILMMAST ||
		u_int(JobID) == JOB_FILMTITTY ||
		u_int(JobID) == JOB_FILMSTRIP ||
		u_int(JobID) == JOB_FILMLESBIAN)
	{
		if(g_Studios.GetNumGirlsOnJob(-1, JOB_CAMERAMAGE, false) == 0 && g_Studios.GetNumGirlsOnJob(-1, JOB_CRYSTALPURIFIER, false) == 0)
		{
			g_MessageQue.AddToQue(gettext("You must have one cameramage and one crystal purifier."), 0);
			Girl->m_DayJob = Girl->m_NightJob = JOB_RESTING;
		}
		else 
		{
			Girl->m_NightJob = u_int(JobID);
		}
	}
	else
	{  // Special cases were checked and don't apply, so just set the job as requested
		MadeChanges = false;
		if(DayOrNight)
			Girl->m_DayJob = JobID;
		else
			Girl->m_NightJob = JobID;
	}

	if (JobID != OldJobID)
	{  // handle instances where special job has been removed, specifically where it actually matters
		if (u_int(OldJobID) == JOB_MATRON || 
			u_int(OldJobID) == JOB_TORTURER || 
			u_int(OldJobID) == JOB_RECRUITER || 
			u_int(OldJobID) == JOB_HEADGIRL || 
			u_int(OldJobID) == JOB_DOCTORE || 
			u_int(OldJobID) == JOB_CENTREMANAGER || 
			u_int(OldJobID) == JOB_CHAIRMAN || 
			u_int(OldJobID) == JOB_PHYSICALSURGERY ||
			u_int(OldJobID) == JOB_GETABORT ||
			u_int(OldJobID) == JOB_BOOBJOB ||
			u_int(OldJobID) == JOB_BREASTREDUCTION ||
			u_int(OldJobID) == JOB_LIPO ||
			u_int(OldJobID) == JOB_FACELIFT ||
			u_int(OldJobID) == JOB_VAGINAREJUV ||
			u_int(OldJobID) == JOB_HEALING ||
			u_int(OldJobID) == JOB_DOCTOR ||
			u_int(OldJobID) == JOB_REHAB ||
			u_int(OldJobID) == JOB_DRUGCOUNSELOR ||
			u_int(OldJobID) == JOB_CAMERAMAGE ||
			u_int(OldJobID) == JOB_CRYSTALPURIFIER)
		{
			if (u_int(JobID) != JOB_MATRON && 
				u_int(JobID) != JOB_TORTURER &&
				u_int(JobID) != JOB_RECRUITER &&
				u_int(JobID) != JOB_HEADGIRL &&
				u_int(JobID) != JOB_DOCTORE &&
				u_int(JobID) != JOB_CENTREMANAGER &&
				u_int(JobID) != JOB_CHAIRMAN &&
				u_int(JobID) != JOB_PHYSICALSURGERY &&
				u_int(JobID) != JOB_GETABORT &&
				u_int(JobID) != JOB_BOOBJOB &&
				u_int(JobID) != JOB_BREASTREDUCTION &&
				u_int(JobID) != JOB_LIPO &&
				u_int(JobID) != JOB_FACELIFT &&
				u_int(JobID) != JOB_VAGINAREJUV &&
				u_int(JobID) != JOB_HEALING &&
				u_int(JobID) != JOB_DOCTOR &&
				u_int(JobID) != JOB_REHAB &&
				u_int(JobID) != JOB_DRUGCOUNSELOR &&
				u_int(JobID) != JOB_CAMERAMAGE &&
				u_int(JobID) != JOB_CRYSTALPURIFIER
				)
			{  // for these Day+Night jobs, switch leftover day or night job back to resting
				if (DayOrNight)
					Girl->m_NightJob = JOB_RESTING;
				else
					Girl->m_DayJob = JOB_RESTING;
			}
		}
	}

	return MadeChanges;
}

// ------ Work Related Violence fns

// MYR: Rewrote the work_related_violence to add the security guard job.
//      Girls and gangs now fight using the attack/defense/agility style combat.
//      This will have to be changed to the new style of combat eventually.
//      A new kind of weaker gang not associated with any of the enemy organizations.
//      This "gang" doesn't have weapon levels or access to healing potions.
//      They're just a bunch of toughs out to make life hard for our working girls.

/*
 * return true if violence was committed against the girl
 * false if nothing happened, or if violence was committed
 * against the customer.
 */
bool cJobManager::work_related_violence(sGirl* girl, int DayNight, bool streets)
{
	cConfig cfg;
	int rape_chance = (int)cfg.prostitution.rape_brothel();
	//vector<sGang *> gang_v;
	vector<sGang *> gangs_guarding = g_Gangs.gangs_on_mission(MISS_GUARDING);

	//int gang_chance = guard_coverage(&gang_v);
	int gang_coverage = guard_coverage(&gangs_guarding);
/*
 *	the base chance of an attempted rape is higher on the
 *	streets
 */
	if(streets) 
		rape_chance = (int)cfg.prostitution.rape_streets();
	
/*
 *	night time doubles the chance of attempted rape
 *	and reduces the chance for a gang to catch it by ten
 */
	if(DayNight == 1) {
		rape_chance *= 2;
		gang_coverage = (int)((float)gang_coverage*0.8f);
	}
/* 
 *	if the player has a -ve disposition, this can 
 *	scare the would-be rapist into behaving himself
 */
	if(g_Dice.percent(g_Brothels.GetPlayer()->disposition() * -1)) 
		rape_chance = 0;
	

	if(g_Dice.percent(rape_chance)) {
		/*sGang *gang = g_Gangs.random_gang(gang_v);

		if(gang_stops_rape(girl, gang, gang_chance, DayNight)) 
			return false;
		if(girl_fights_rape(girl, DayNight)) 
			return false; */

		// Get a random weak gang. These are street toughs looking to abuse a girl.
		// They have low stats and smaller numbers. Define it here so that even if
		// the security girl or defending gang is defeated, any casualties they inflicts
		// carry over to the next layer of defense.
		sGang *enemy_gang = g_Gangs.GetTempWeakGang();
		// There is also between 1 and 10 of them, not 15 every time
		enemy_gang->m_Num = g_Dice%10 + 1;
		
		// Three more lines of defense

		// 1. Brothel security
		if (security_stops_rape(girl, enemy_gang, DayNight))
			return false;

		// 2. Defending gangs
		if (gang_stops_rape(girl, gangs_guarding, enemy_gang, gang_coverage, DayNight)) 
			return false;

		// 3. The attacked girl herself
		if (girl_fights_rape(girl, enemy_gang, DayNight)) 
			return false;

		// If all defensive measures fail...
		customer_rape(girl);
		return true;
	}
	g_Girls.UpdateEnjoyment(girl, ACTION_SEX, +1, true);
/*
 *	the fame thing could work either way.
 *	of course, that road leads to us keeping reputation for
 *	different things - beating the customer would be a minus
 *	for the straight sex crowd, but a big pull for the 
 *	femdom lovers...
 *
 *	But let's get what we have working first
 */
	if((girl->has_trait("Yandere") || girl->has_trait("Tsundere") || girl->has_trait("Aggressive")) && g_Dice.percent(30)) {
		girl->m_Events.AddMessage(gettext("She beat the customer silly."), IMGTYPE_PROFILE, DayNight);
		g_Girls.UpdateStat(girl, STAT_FAME, -1);
	}
	return false;
}

/*
 * We need a cGuards guard manager. Or possible a cGuardsGuards manager.
 */ 
int cJobManager::guard_coverage(vector<sGang*> *vpt)
{
	int pc = 0;
	vector<sGang*> v = g_Gangs.gangs_on_mission(MISS_GUARDING);

	if(vpt != 0) 
		*vpt = v;

	for(u_int i = 0; i < v.size(); i++) {
		sGang *gang = v[i];
/*
 *		our modifier is gang int - 50
 *		so that gives us a value from -50 to + 50
 */
		float mod = (float)gang->intelligence() - 50;
/*
 *		double it to make it -100 to +100
 *		and then divide by 100 to make it -1.0 to +1.0
 *
 *		can just divide by 50, really
 */
		mod /= 50;
/*
 *		add one so we have a value from 0.0 to 2.0
 */
		mod += 1;
/* 
 *		and that's the multiplier
 *
 *		a base 6% per gang member give a full strength gang
 *		with average int a 90% chance of stopping a rape
 *		at 100 int they get 180. 
 */
 		pc += int(6 * gang->m_Num * mod);
	}
/*
 *	with 6 brothels, that one gang gives a 15% chance to stop
 *	any given rape - rising to 30 for 100 intelligence
 *
 *	so 3 gangs on guard duty, at full strength, get you 90%
 *	coverage. And 4 get you 180 which should be bulletproof
 */
	return pc / g_Brothels.GetNumBrothels();
}

// True means security intercepted the perp(s)

bool cJobManager::security_stops_rape(sGirl * girl, sGang *enemy_gang, int day_night)
{
	// MYR: Note to self: STAT_HOUSE isn't the brothel number :)
	int GirlsBrothelNo = g_Brothels.GetGirlsCurrentBrothel(girl);
	sBrothel * Brothl = g_Brothels.GetBrothel(GirlsBrothelNo);
	int SecLev = Brothl->m_SecurityLevel, OrgNumMem = enemy_gang->m_Num;
	sGirl * SecGuard;

	// A gang takes 5 security points per member to stop
	if (SecLev < OrgNumMem * 5)
		return false;

	// Security guards on duty this shift
	vector<sGirl *> SecGrd = g_Brothels.GirlsOnJob(GirlsBrothelNo, JOB_SECURITY, day_night == SHIFT_DAY);
	// Security guards with enough health to fight
	vector<sGirl *> SecGrdWhoCanFight;

	if (SecGrd.size() == 0)
		return false;

	// Remove security guards who are too wounded to fight
	// Easier to work from the end
	for(int i = SecGrd.size() - 1; i >= 0 ; i--) 
	{
	  SecGuard = SecGrd[i];
	  if (SecGuard->m_Stats[STAT_HEALTH] <= 50)
		  SecGrd.pop_back();
	  else
	  {
		SecGrdWhoCanFight.push_back(SecGrd.back());
		SecGrd.pop_back();
	  }
	}

	// If all the security guards are too wounded to fight
	if (SecGrdWhoCanFight.size() == 0)
		return false;

	// Get a random security guard
	SecGuard = SecGrdWhoCanFight.at(g_Dice%SecGrdWhoCanFight.size());


    // Most of the rest of this is a copy-paste from customer_rape
	bool res = g_Gangs.GirlVsEnemyGang(SecGuard, enemy_gang);

	// Earn xp for all kills, even if defeated
	int xp = 3;

	if (g_Girls.HasTrait(SecGuard, "Quick Learner"))
		xp += 2;
	else if (g_Girls.HasTrait(SecGuard, "Slow Learner"))
		xp -= 2;

	g_Girls.UpdateStat(SecGuard, STAT_EXP, (OrgNumMem - enemy_gang->m_Num) * xp);

	if (res)  // Security guard wins
	{
		g_Girls.UpdateSkill(SecGuard, SKILL_COMBAT, 1);
		g_Girls.UpdateSkill(SecGuard, SKILL_MAGIC, 1);
		g_Girls.UpdateStat(SecGuard, STAT_LIBIDO, OrgNumMem - enemy_gang->m_Num);  // There's nothing like killin ta make ya horny!
		g_Girls.UpdateStat(SecGuard, STAT_CONFIDENCE, OrgNumMem - enemy_gang->m_Num);
		g_Girls.UpdateStat(SecGuard, STAT_FAME, OrgNumMem - enemy_gang->m_Num);
		g_Girls.UpdateEnjoyment(girl, ACTION_COMBAT, OrgNumMem - enemy_gang->m_Num, true);
		g_Girls.UpdateEnjoyment(girl, ACTION_WORKSECURITY, OrgNumMem - enemy_gang->m_Num, true);

		stringstream msg;

		// I decided to not say gang in the text. It can be confused with a player or enemy organization's
		// gang, when it is neither.
		if (OrgNumMem == 1)
		{
		  msg << gettext("Security Report:\nA customer tried to attack ") << girl->m_Realname << gettext(", but you intercepted and beat ");
		  int roll = g_Dice%10 + 1;
		  if (roll == 5)
			  msg << gettext("her.");
		  else
		      msg << gettext("him.");
		}
		else
		{
		  msg << gettext("Security Report:\n");
		  if (enemy_gang->m_Num == 0)
			msg << gettext("A group of customers tried to attack ") << girl->m_Realname << gettext(". You intercepted and thrashed all ") << OrgNumMem << gettext(" of them.");
		  else
			msg << gettext("A group of ") << OrgNumMem << gettext(" customers tried to attack ") << girl->m_Realname << gettext(". They fled after you intercepted and thrashed ")
			  << (OrgNumMem - enemy_gang->m_Num) << gettext(" of them.");
	    
		}
		SecGuard->m_Events.AddMessage(msg.str(), IMGTYPE_DEATH, EVENT_WARNING/*day_night*/);
	} 
	else  // Loss
	{
		stringstream ss;

		ss << gettext("Security Problem:\n") << gettext("Trying to defend ") << girl->m_Realname << gettext(". You defeated ")
		   << (OrgNumMem - enemy_gang->m_Num) << gettext(" of ") << OrgNumMem << gettext(" before:\n") << SecGuard->m_Realname << GetGirlAttackedString();

		SecGuard->m_Events.AddMessage(ss.str(), IMGTYPE_DEATH, EVENT_DANGER);

		// Bad stuff
		g_Girls.UpdateStat(SecGuard, STAT_HAPPINESS, -40);
		g_Girls.UpdateStat(SecGuard, STAT_CONFIDENCE, -40);
		g_Girls.UpdateStat(SecGuard, STAT_OBEDIENCE, -10);
		g_Girls.UpdateStat(SecGuard, STAT_SPIRIT, -40);
		g_Girls.UpdateStat(SecGuard, STAT_LIBIDO, -40);
		g_Girls.UpdateStat(SecGuard, STAT_TIREDNESS, 60);
		g_Girls.UpdateStat(SecGuard, STAT_PCFEAR, 20);
		g_Girls.UpdateStat(SecGuard, STAT_PCLOVE, -20);
		g_Girls.UpdateStat(SecGuard, STAT_PCHATE, 20);
		g_Girls.GirlInjured(SecGuard, 10); // MYR: Note
		g_Girls.UpdateEnjoyment(SecGuard, ACTION_WORKSECURITY, -30, true);
		g_Girls.UpdateEnjoyment(SecGuard, ACTION_COMBAT, -30, true);
	}

	// Win or lose, subtract 5 security points per gang member
	Brothl->m_SecurityLevel = Brothl->m_SecurityLevel - OrgNumMem * 5;

	return res;
}

bool cJobManager::gang_stops_rape(sGirl* girl, vector<sGang *> gangs_guarding, sGang *enemy_gang,
	int coverage, int day_night)
{
	if((g_Dice%100 + 1) > coverage) 
		return false;

	sGang *guarding_gang = gangs_guarding.at(g_Dice%gangs_guarding.size());

	// GangVsGang returns true if enemy_gang loses
	bool guards_win = g_Gangs.GangBrawl(guarding_gang, enemy_gang);

	if(!guards_win) {
		stringstream gang_s, girl_s;
		gang_s << guarding_gang->m_Name << gettext(" was defeated defending ") << girl->m_Realname << gettext(".");
		girl_s << guarding_gang->m_Name << gettext(" was defeated defending you from a gang of rapists.");
		guarding_gang->m_Events.AddMessage(gang_s.str(), IMGTYPE_PROFILE,  EVENT_WARNING);
		girl->m_Events.AddMessage(girl_s.str(), IMGTYPE_DEATH, EVENT_WARNING /*day_night*/);
		return false;
	}

/*
 * options: smart guards send them off with a warning
 * dead customers should impact disposition and suspicion
 * might also need a bribe to cover it up
 */
	stringstream gang_ss, girl_ss;
	int roll = g_Dice.random(100) + 1;
	if(roll < guarding_gang->intelligence() / 2) 
	{
		gang_ss <<	gettext("One of the ") << guarding_gang->m_Name << gettext(" found a customer getting violent with ")
			<< girl->m_Realname << gettext(" and sent him packing, in fear for his life.");
		girl_ss << gettext("Customer attempt to rape her, but guards ") << guarding_gang->m_Name << gettext(" scared him off.");
	}
	else if(roll < guarding_gang->intelligence())
	{
		gang_ss	<< gettext("One of the ") << guarding_gang->m_Name << gettext(" found a customer hurting  ") << girl->m_Realname
			<< gettext(" and beat him bloody before sending him away.");
		girl_ss	<< gettext("Customer attempt to rape her, but guards ") << guarding_gang->m_Name << gettext(" gave him a beating.");
	}
	else 
	{
		gang_ss	<< gettext("One of the ") << guarding_gang->m_Name << gettext(" killed a customer who was trying to rape ")
			<< girl->m_Realname << gettext(".");
		girl_ss	<< gettext("Customer attempt to rape her, but guards ") << guarding_gang->m_Name << gettext(" killed them.");
	}
	cerr << "gang ss=" << gang_ss.str() << endl;
	cerr << "girl ss=" << girl_ss.str() << endl;

	girl->m_Events.AddMessage(girl_ss.str(), IMGTYPE_DEATH, day_night);
	guarding_gang->m_Events.AddMessage(gang_ss.str(), IMGTYPE_PROFILE, EVENT_GANG);
	
	return true;
}

#if 0
/*
 * options: smart guards send them off with a warning
 * dead customers should impact disposition and suspicion
 * might also need a bribe to cover it up
 */
bool cJobManager::gang_stops_rape(sGirl* girl, sGang *gang, int chance, int day_night)
{
	if(g_Dice.percent(chance) == false) 
		return false;

	bool gang_wins = false;
	int wlev = *g_Gangs.GetWeaponLevel();
/*
 *	the odds are tipped very much in favour of the gang
 *
 *	they need to blow both magic and combat rolls three times
 *	to lose - even for a new gang with about 30%, that works out
 *	about an 89% chance to win, rising to 98% with weapon level 4
 *
 *	for gangs in the 90% range, you're looking at 1 in a million chances
 */
 	int bonus = 0;
/*
 *	make the gang leader tougher
 */
	if(gang->m_Num == 1) 
		bonus = 10;
	
	int combat = gang->combat() + 4 * wlev + bonus;
	int magic = gang->magic() + 4 * wlev + bonus;
	for(int i = 0; i < 3; i++) {
		gang_wins = g_Dice.percent(combat) || g_Dice.percent(magic) ;
		if(gang_wins) 
						break;
		
	}
/*
 *	if the gang member lost, tell the player about it
 *	no point in subtelties of simulation if the customer
 *	never knows
 */
	if(!gang_wins) {
		string s;
		if(gang->m_Num == 1) 
			s =	gettext("The leader of this gang is dead; killed attempting to prevent a rape. You may want to hire a new gang.");
		else 
			s =	gettext("One of this gang's members turned up dead. It looks like he attacked a rapist and lost.");
		
		gang->m_Num --;
		gang->m_Events.AddMessage(s, 0, 0);
	return false;
	}

	stringstream gang_ss, girl_ss;
	int roll = g_Dice.random(100) + 1;
	if(roll < gang->intelligence() / 2) {
		gang_ss <<	gettext("One of the ") <<	gang->m_Name <<	gettext(" found a customer getting violent with ")
			<<	girl->m_Realname <<	gettext(" and sent him packing, in fear for his life");
		girl_ss <<	gettext("Customer attempt to rape her ") <<gettext("but the guard scared him off");
	}
	else if(roll < gang->intelligence()) {
		gang_ss	<<	gettext("One of the ") << gang->m_Name << gettext(" found a customer hurting  ") << girl->m_Realname
			<< gettext(" and beat him bloody before sending him away");
		girl_ss	<<	gettext("Customer attempt to rape her but ") << gettext("the guard gave him a beating");
	}
	else {
		gang_ss	<<	gettext("One of the ") << gang->m_Name << gettext(" killed a customer who was trying to rape ")
			<<	girl->m_Realname;
		girl_ss	<<	gettext("Customer attempt to rape her but ") << gettext("the was killed by the guard.");
	}
	cerr << "gang ss=" << gang_ss.str() << endl;
	cerr << "girl ss=" << girl_ss.str() << endl;
	gang->m_Events.AddMessage(gang_ss.str(), 0, 0);
	girl->m_Events.AddMessage(girl_ss.str(), IMGTYPE_DEATH, day_night);
	return true;
}
#endif

// true means she won

bool cJobManager::girl_fights_rape(sGirl* girl, sGang *enemy_gang, int day_night)
{
	int OrgNumMem = enemy_gang->m_Num;

	bool res = g_Gangs.GirlVsEnemyGang(girl, enemy_gang);

	// Earn xp for all kills, even if defeated
	int xp = 3;

	if (g_Girls.HasTrait(girl, "Quick Learner"))
		xp += 2;
	else if (g_Girls.HasTrait(girl, "Slow Learner"))
		xp -= 2;

	g_Girls.UpdateStat(girl, STAT_EXP, (OrgNumMem - enemy_gang->m_Num) * xp);

	if (res)  // She wins.  Yay!
	{
		g_Girls.UpdateSkill(girl, SKILL_COMBAT, 1);
		g_Girls.UpdateSkill(girl, SKILL_MAGIC, 1);
		g_Girls.UpdateStat(girl, STAT_AGILITY, 1);
		g_Girls.UpdateStat(girl, STAT_LIBIDO, OrgNumMem - enemy_gang->m_Num);  // There's nothing like killin ta make ya horny!
		g_Girls.UpdateStat(girl, STAT_CONFIDENCE, OrgNumMem - enemy_gang->m_Num);
		g_Girls.UpdateStat(girl, STAT_FAME, OrgNumMem - enemy_gang->m_Num);

		g_Girls.UpdateEnjoyment(girl, ACTION_COMBAT, OrgNumMem - enemy_gang->m_Num, true);

		stringstream msg;

		// MYR: I decided to not say gang in the text. It can be confused with a player or enemy organization's
		//     gang, when it is neither.
		if (OrgNumMem == 1)
		{
		  msg << gettext("A customer tried to attack her. She killed ");
		  int roll = g_Dice%10 + 1;
		  if (roll == 5)
			  msg << gettext("her.");
		  else
		      msg << gettext("him.");
		}
		else
		{
		  if (enemy_gang->m_Num == 0)
		    msg << gettext("A group of customers tried to assault her. She thrashed all ") << OrgNumMem << gettext(" of them.");
		  else
			msg << gettext("A group of ") << OrgNumMem << gettext(" customers tried to assault her. They fled after she killed ")
			  << (OrgNumMem - enemy_gang->m_Num) << gettext(" of them.");
	    
		}
		girl->m_Events.AddMessage(msg.str(), IMGTYPE_DEATH, EVENT_WARNING/*day_night*/);
	}

    // Losing is dealt with later in customer_rapes (called from work_related_violence)
	return res;
}
#if 0
/* 
 * Again, we can elaborate this
 *
 * some traits should give a bonus. There should be a chance of
 * dead customers. There should probably be a chance of a dead girls
 *
 * returns true if the girl is successful in fighting the rapist off
 */
bool cJobManager::girl_fights_rape(sGirl* girl, int day_night)
{
	string msg;
	bool res = false;
	int roll = g_Dice%100;

	if(g_Girls.HasTrait(girl, "Merciless")) {
		msg =	gettext("The customer tried to rape this girl.  She leaves him beaten and bloody, but still breathing.");
		res = true;
	}
	else if(roll < girl->combat() && roll < girl->magic()) {
		msg =	gettext("The customer attempted to rape her but she remembered her self-defence training and fought him off");
		res = true;
	}
	girl->m_Events.AddMessage(msg, IMGTYPE_DEATH, day_night);
	return false;
}
#endif

/*
 * I think these next three could use a little detail
 * MYR: Added the requested detail (in GetGirlAttackedString() below)
 */
void cJobManager::customer_rape(sGirl* girl)
{
	stringstream ss;

	ss << girl->m_Realname << GetGirlAttackedString();

	girl->m_Events.AddMessage(ss.str(), IMGTYPE_DEATH, EVENT_DANGER);

	// Made this more harsh, so the player hopefully notices it
	//g_Girls.UpdateStat(girl, STAT_HEALTH, -(g_Dice%10 + 5));  // Oops, can drop health below zero after combat is considered
	g_Girls.UpdateStat(girl, STAT_HAPPINESS, -40);
	g_Girls.UpdateStat(girl, STAT_CONFIDENCE, -40);
	g_Girls.UpdateStat(girl, STAT_OBEDIENCE, -10);
	g_Girls.UpdateStat(girl, STAT_SPIRIT, -40);
	g_Girls.UpdateStat(girl, STAT_LIBIDO, -40);
	g_Girls.UpdateStat(girl, STAT_TIREDNESS, 60);
	g_Girls.UpdateStat(girl, STAT_PCFEAR, 20);
	g_Girls.UpdateStat(girl, STAT_PCLOVE, -20);
	g_Girls.UpdateStat(girl, STAT_PCHATE, 20);
	g_Girls.GirlInjured(girl, 10); // MYR: Note
	g_Girls.UpdateEnjoyment(girl, ACTION_SEX, -30, true);
}
#if 0
void cJobManager::customer_rape(sGirl* girl)
{
	girl->m_Events.AddMessage(girl->m_Realname + " " + GetGirlAttackedString()/*"She was beaten and raped, and the perpetrator escaped"*/, IMGTYPE_DEATH, EVENT_DANGER);
	g_Girls.UpdateStat(girl, STAT_HEALTH, -2);
	g_Girls.UpdateStat(girl, STAT_SPIRIT, -1);
	g_Girls.GirlInjured(girl, 3);
	g_Girls.UpdateEnjoyment(girl, ACTION_SEX, -8, true);
}
#endif

// MYR: Lots of different ways to say the girl had a bad day
// doc: let's have this return a string, instead of a stringstream:
// the caller doesn't need the stream and gcc is giving weird type coercion 
// errors

string cJobManager::GetGirlAttackedString()
{
	int roll1 = 0, roll2 = 0, roll3 = 0;
	stringstream ss;

	ss << gettext(" was ");

    roll1 = g_Dice % 21 + 1;   // Remember to update this when new strings are added
	switch (roll1)
	{
	  case 1: ss << gettext("cornered"); break;
	  case 2: ss << gettext("hauled to the dungeon"); break;
	  case 3: ss << gettext("put on the wood horse"); break;
	  case 4: ss << gettext("tied to the bed"); break;
	  case 5: ss << gettext("locked in the closet"); break;
	  case 6: ss << gettext("drugged"); break;
	  case 7: ss << gettext("tied up"); break;
	  case 8: ss << gettext("wrestled to the floor"); break;
	  case 9: ss << gettext("slapped in irons"); break;
	  case 10: ss << gettext("cuffed"); break;
	  case 11: ss << gettext("put in THAT machine"); break;
	  case 12: ss << gettext("konked on the head"); break;
	  case 13: ss << gettext("calmly sat down"); break;
	  case 14: ss << gettext("hand-cuffed during kinky play"); break;
	  case 15: ss << gettext("caught off guard during fellatio"); break;
	  case 16: ss << gettext("caught cheating at cards"); break;
	  case 17: ss << gettext("found sleeping on the job"); break;
	  case 18: ss << gettext("chained to the porch railing"); break;
	  case 19: ss << gettext("tied up BDSM-style"); break;
	  case 20: ss << gettext("stretched out on the torture table"); break;	
	  case 21: ss << gettext("tied up and hung from the rafters"); break;
	}

	ss << gettext(" and ");

    roll2 = g_Dice % 20 + 1;
	switch (roll2)
	{
	  case 1: ss << gettext("abused"); break;
	  case 2: ss << gettext("whipped"); break;
	  case 3: ss << gettext("yelled at"); break;
	  case 4: ss << gettext("assaulted"); break;
	  case 5: ss << gettext("raped"); break;
	  case 6: ss << gettext("her cavities were explored"); break;
	  case 7: ss << gettext("her hair was done"); break;
	  case 8: ss << gettext("her feet were licked"); break;
	  case 9: ss << gettext("relentlessly tickled"); break;
	  case 10: ss << gettext("fisted"); break;
	  case 11: ss << gettext("roughly fondled"); break;
	  case 12: ss << gettext("lectured to"); break;
	  case 13: ss << gettext("had her fleshy bits pierced"); break;
	  case 14: ss << gettext("slapped around"); break;
	  case 15: ss << gettext("penetrated by some object"); break;
	  case 16: ss << gettext("shaved"); break;
	  case 17: ss << gettext("tortured"); break;
	  case 18: ss << gettext("forced outside"); break;
	  case 19: ss << gettext("forced to walk on a knotted rope"); break;
	  case 20: ss << gettext("her skin was pierced by sharp things"); break;
	}

	ss << gettext(" by ");

    roll3 = g_Dice % 21 + 1;
	switch (roll3)
	{
	  case 1: ss << gettext("customers."); break;
	  case 2: ss << gettext("some sadistic monster."); break;
	  case 3: ss << gettext("a police officer."); break;
	  case 4: ss << gettext("the other girls."); break;
	  case 5: ss << gettext("Batman! (Who knew?)"); break;
	  case 6: ss << gettext("a ghost (She claims.)"); break;
	  case 7: ss << gettext("an enemy gang member."); break;
	  case 8: ss << gettext("priests."); break;
	  case 9: ss << gettext("orcs. (What?)"); break;
	  case 10: ss << gettext("a doppleganger disguised as a human."); break;
	  case 11: ss << gettext("a jealous wife."); break;
	  case 12: ss << gettext("a jealous husband."); break;
	  case 13: ss << gettext("a public health official."); break;
	  case 14: ss << gettext("you!"); break;
	  case 15: ss << gettext("some raving loonie."); break;
	  case 16: ss << gettext("a ninja!"); break;
	  case 17: ss << gettext("a pirate. (Pirates rule! Ninjas drool!)"); break;
	  case 18: ss << gettext("members of the militia."); break;
	  case 19: ss << gettext("your mom (It runs in the family.)"); break;
	  case 20: ss << gettext("tentacles from the sewers."); break;
	  case 21: ss << gettext("a vengeful family member."); break;
	}

	return ss.str();
}

// ------ Training

/*
 * let's look at this a little differently...
 */
void cJobManager::get_training_set(vector<sGirl*> &v, vector<sGirl*> &t_set)
{
	u_int max = 4;
	u_int v_siz = v.size();

	// empty out the trainable set
	t_set.clear();
/*
 *	if there's nothing in the vector, return with t_set empty
 */
 	if(v_siz == 0u)
		return;
/*
 *	if there's just one, return with t_set containing that
 */
 	if(v_siz == 1u)
	{
		t_set.push_back(v.back());
		v.pop_back();
		return;
	}
/*
 *	we want to split the girls into groups of four
 *	but if possible we don't want any singletons
 *
 *	So... if there would be one left over,
 *	we make the first group a group of three.
 *	subsequent groups will have modulus 2 and thus
 *	generate quads, until the end when we have a pair left.
 *
 *	Easier to explain in code than words, really.
 */
 	if((v_siz % 4) == 1)
		max --;		// reduce it by one, this time only
/*
 *	get the limiting variable for the loop
 *	we don't compare against v.size() because
 *	that will change as the loop progresses
 */
	u_int lim = (max < v_siz ? max : v_siz);
/*
 *	now loop until we hit max, or run out of girls
 */
 	for(u_int i = 0; i < lim; i++)
	{
		t_set.push_back(v.back());
		v.pop_back();
	}
}

bool cJobManager::WorkTraining(sGirl* girl, sBrothel* brothel, int DayNight, string& summary)
{
	// training is already handled in UpdateGirls
	//do_training(brothel, DayNight);

	return false;
}

void cJobManager::do_solo_training(sGirl *girl, int DayNight)
{
	TrainableGirl trainee(girl);
	girl->m_Events.AddMessage(gettext("She trained during this shift by herself, so learning anything worthwhile was difficult."), IMGTYPE_PROFILE, DayNight);
/*
 *	50% chance nothing happens
 */
 	if(g_Dice.percent(50))
	{
		girl->m_Events.AddMessage(gettext("Sadly, she didn't manage to improve herself in any way."), IMGTYPE_PROFILE, EVENT_SUMMARY);
		return;
	}
/*
 *	otherwise, pick a random attribute and raise it 1-3 points
 */
	int amt = 1 + g_Dice%3;
	string improved = trainee.update_random(amt);
	stringstream ss;
	ss.str("");
	ss << gettext("She managed to gain ") << amt << gettext(" ") << improved << gettext(".");
	girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_SUMMARY);
}

void cJobManager::do_training_set(vector<sGirl*> girls, int DayNight)
{
	sGirl *girl;
	stringstream ss;
/*
 *	we're getting a vector of 1-4 girls here
 *	(the one is possible if only one girl trains)
 */
	if(girls.empty())
	{  // no girls? that shouldn't happen
		g_LogFile.ss() << "Logic Error in cJobManager::do_training_set: empty set passed for training!";
		g_LogFile.ssend();
		return;
	}
	else if(girls.size() == 1)
	{  // special case for only one girl
		do_solo_training(girls[0], DayNight);
		return;
	}
/*
 *	OK. Now, as I was saying. We have an array of sGirl* pointers..
 *	We need that to be a list of TrainableGirl objects:
 */
 	vector<TrainableGirl> set;
/*
 *	4 is the maximum set size. I should probably consider
 *	making that a class constant - or a static class member
 *	initialised from the config file. Later for that.
 */
	u_int num_girls = girls.size();
	for(u_int i = 0; i < num_girls; i++)
	{
		girl = girls[i];
		if(girl == 0)
			break;
		set.push_back(TrainableGirl(girl));
	}
/*
 *	now get an idealized composite of the girls in the set
 *
 *	and the get a vector of the indices of the most efficient
 *	three attributes for them to train 
 */
	IdealGirl ideal(set);
	vector<int> indices = ideal.training_indices();
/*
 *	OK. Loop over the girls, and then over the indices
 */
 	for(u_int g_idx = 0; g_idx < set.size(); g_idx++) 
	{
		TrainableGirl &trainee = set[g_idx];

		for(u_int i_idx = 0; i_idx < indices.size(); i_idx++) 
		{
			int index = indices[i_idx];
/*
 *			the base value for any increase is the difference
 *			between the value of girl's stat, and the value of best
 *			girl in the set
 */         int aaa = ideal[index].value(); //    [index].value();
			int bbb = trainee[index].value();
			int diff = aaa - bbb;
			if(diff < 0)  // don't want a negative training value
				diff = 0;
/*
 *			plus one - just so the teacher gets a chance to 
 *			learn something
 */
			diff ++;
/*
 *			divide by 10 for the increase
 */
 			int inc = (int)floor(1.0 * (double)diff / 10.0);
/*
 *			if there's any left over (or if the diff
 *			was < 10 to begin with, there's a 10%
 *			point for each difference point
 */
			int mod = diff %10;
			if(g_Dice.percent(mod * 10)) 
				inc ++;
			
			trainee[index].upd(inc);
		}

		sGirl *girl = trainee.girl();
/*
 *		need to do the  "she trained hard with ..." stuff here
 */
 		//int n_girls = set.size();
		ss.str("");
		ss << gettext("She trained during this shift in the following grouping: ");
		for(u_int i = 0; i < num_girls; i++)
		{
			ss << girls[i]->m_Realname;
			if(i == 0 && num_girls == 2)
				ss << gettext(" and ");
			else if(i < num_girls - 2)
				ss << gettext(", ");
			else if(i == num_girls - 2)
				ss << gettext(", and ");
		}
		ss << gettext(".");

		girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, DayNight);
		ss.str("");
		ss << gettext("She trained during this shift");

		bool raised = false;
		for(u_int i_idx = 0; i_idx < indices.size(); i_idx++)
		{
			int index = indices[i_idx];
			int  gain = trainee[index].gain();

			if(gain == 0) 
				continue;
			
			if(!raised)
			{
				raised = true;
				ss << gettext(" and gained ");
			}
			else 
				ss << gettext(", ");
			
			ss << gettext("+") << gain <<  gettext(" ") << trainee[index].name();
		}
		ss << gettext(".");
		girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_SUMMARY);
	}
}

void cJobManager::do_training(sBrothel* brothel, int DayNight)
{
	cTariff tariff;
	cConfig cfg;

	vector<sGirl*> t_set;
	vector<sGirl*> girls = girls_on_job(brothel, JOB_TRAINING, DayNight);

	for(u_int i = girls.size(); i --> 0; )
	{  // no girls sneaking in training if she gave birth
		if(
			(girls[i]->m_WeeksPreg > 0 && girls[i]->m_WeeksPreg+1 >= cfg.pregnancy.weeks_pregnant())
			|| (girls[i]->m_JustGaveBirth && DayNight == 1)
		)
			girls.erase(girls.begin()+i);
	}

	random_shuffle(girls.begin(),girls.end());

	for(;;)
	{
		get_training_set(girls, t_set);
		if(t_set.empty())
			break;
		do_training_set(t_set, DayNight);
	}
/*
 *	a few bookkeeping details here
 *
 *	dirt and training costs, for a start
 */
	brothel->m_Filthiness += girls.size();
	brothel->m_Finance.girl_training(
		tariff.girl_training() * girls.size()
	);
/*
 *	and then each girl gets to feel tired and horny
 *	as a result of training
 */
 	for(u_int i = 0; i < girls.size(); i++)
	{
		sGirl *girl = girls[i];
		g_Girls.AddTiredness(girl);
		g_Girls.UpdateTempStat(girl, STAT_LIBIDO, 2);
	}
}

// ----- Film & related

void cJobManager::update_film(sBrothel * brothel)
{

	if(brothel->m_CurrFilm)
	{
		if(brothel->m_CurrFilm->time)
			brothel->m_CurrFilm->time--;
		else
		{
			brothel->m_CurrFilm->final_quality=0;
			for(u_int i=0;i<brothel->m_CurrFilm->scene_quality.size();i++)
			{
				brothel->m_CurrFilm->final_quality+=(int)((float)brothel->m_CurrFilm->scene_quality[i]*brothel->m_CurrFilm->quality_multiplyer);
			}
			brothel->m_CurrFilm->total_customers=0;
			char cust_mult=0;
			for(int j=0;j<5;j++)
			{
				if(brothel->m_CurrFilm->sex_acts_flags[j])
					cust_mult++;
			}
			brothel->m_CurrFilm->total_customers=cust_mult*brothel->m_CurrFilm->final_quality*10;
			brothel->m_CurrFilm->final_quality/=brothel->m_CurrFilm->scene_quality.size();
			film_list.push_back(brothel->m_CurrFilm);
			brothel->m_CurrFilm=0;
		}
	}
}

long cJobManager::make_money_films()
{
	long income=0;
	for(u_int i=0;i<film_list.size();i++)
	{
		income+=(long)((float)film_list[i]->final_quality*(float)film_list[i]->total_customers*.85f);
		film_list[i]->total_customers=(int)((float)film_list[i]->total_customers*.85f);
	}
	return income;
}

void cJobManager::save_films(std::ofstream &ofs)
{
	ofs<<film_list.size()<<'\n';
	for(u_int i=0;i<film_list.size();i++)
	{
		ofs<<film_list[i]->final_quality<<' '<<film_list[i]->quality_multiplyer<<' '<<film_list[i]->time<<' '<<film_list[i]->total_customers<<'\n';
		for(int j=0;j<5;j++)
		{
			ofs<<film_list[i]->sex_acts_flags[j];
			if(j!=4)
			ofs<<' ';
		}
		ofs<<'\n';
		ofs<<film_list[i]->scene_quality.size()<<'\n';
			for(u_int j=0;j<film_list[i]->scene_quality.size();j++)
			ofs<<film_list[i]->scene_quality[j]<<' ';
		ofs<<'\n';
	}
}

void cJobManager::load_films(std::ifstream &ifs)
{
	u_int temp;
	ifs>>temp;
	film_list.resize(temp);
	if(ifs.peek()=='\n') ifs.ignore(1,'\n');
	for(u_int i=0;i<film_list.size();i++)
	{
		film_list[i]=new sFilm;
		ifs>>film_list[i]->final_quality;
		ifs>>film_list[i]->quality_multiplyer;
		ifs>>film_list[i]->time;
		ifs>>film_list[i]->total_customers;
		if(ifs.peek()=='\n') ifs.ignore(1,'\n');
		for(int j=0;j<5;j++)
			ifs>>film_list[i]->sex_acts_flags[j];
		if(ifs.peek()=='\n') ifs.ignore(1,'\n');
		ifs>>temp;
		film_list[i]->scene_quality.resize(temp);
		for(u_int j=0;j<temp;j++)
		{
			ifs>>film_list[i]->scene_quality[i];
		}
	}
}
