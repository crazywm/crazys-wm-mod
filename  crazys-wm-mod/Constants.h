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
#ifndef __CONSTANTS_H
#define __CONSTANTS_H
#include"cRng.h"


typedef unsigned int u_int;

// game version
const int g_MajorVersion    = 1;
const int g_MinorVersionA   = 6;
const int g_MinorVersionB   = 0;
const int g_StableVersion   = 4;

enum {
	SHIFT_DAY	= 0,
	SHIFT_NIGHT	= 1
};

// `J` added for .06.01.18 - not used for much yet 

//GENDER enum					// P T V O //
enum GENDER{
	GENDER_FEMALE/*     */ = 0,	// 0 0 1 1 //	all female
	GENDER_FEMALENEUT/* */,     // 0 0 1 0 //	female with vagina but no ovaries
	GENDER_FUTA/*       */,     // 1 0 1 1 //	female with penis but no testes
	GENDER_FUTANEUT/*   */,     // 1 0 1 0 //	female with vagina and penis but no ovaries or testes
	GENDER_FUTAFULL/*   */,     // 1 1 1 1 //	female with penis and testes
	GENDER_NONEFEMALE/* */,     // 0 0 0 0 //	no gender but more female
	GENDER_NONE/*       */,     // 0 0 0 0 //	no gender at all
	GENDER_NONEMALE/*   */,     // 0 0 0 0 //	no gender but more male
	GENDER_HERMFULL/*   */,     // 1 1 1 1 //	male with vagina and ovaries
	GENDER_HERMNEUT/*   */,     // 1 0 1 0 //	male with penis and vagina but no testes or ovaries
	GENDER_HERM/*       */,	    // 1 1 1 0 //	male with vagina but no ovaries
	GENDER_MALENEUT/*   */,	    // 1 0 0 0 //	male with penis but no testes
	GENDER_MALE/*       */,	    // 1 1 0 0 //	all male
	NUM_GENDERS/*       */      //         //   Number of different genders
};//End GENDER enum

enum FFSD		// `J` Free Fire Sell Dump
{
	FFSD_keep = 0,
	FFSD_fire,
	FFSD_free,
	FFSD_sell,
	FFSD_dump,
	FFSD_fidu,	// fire dump
	FFSD_fise,	// fire sell
	FFSD_fisd,	// fire sell dump
	FFSD_frdu,	// free dump
	FFSD_sedu,	// sell dump
	FFSD_dump1, // proper funeral
	FFSD_dump2, // dump in shollow unmarked grave
	FFSD_dump3, // dump on side of the road
	FFSD_dump4, // sell dead to highest bidder
	FFSD_frdu1,	// have your slave girls dispose of the dead then free them
	FFSD_frdu2,	// have your slave girls dispose of the dead then stay slaves
	FFSD_sedu1,	// Sell all the girls, living and dead
	FFSD_free1,	// Throw a freedom party
	FFSD_,

};

// Editable Character Stats and skills (used for traits)
//STATS enum
enum STATS{
	STAT_CHARISMA		= 0	,	//
	STAT_HAPPINESS			,	//
	STAT_LIBIDO				,	//
	STAT_CONSTITUTION		,	//
	STAT_INTELLIGENCE		,	//
	STAT_CONFIDENCE			,	//
	STAT_MANA				,	//
	STAT_AGILITY			,	//
	STAT_FAME				,	//
	STAT_LEVEL				,	//
	STAT_ASKPRICE			,	//
	STAT_HOUSE				,	//
	STAT_EXP				,	//
	STAT_AGE				,	//
	STAT_OBEDIENCE			,	//
	STAT_SPIRIT				,	//
	STAT_BEAUTY				,	//
	STAT_TIREDNESS			,	//
	STAT_HEALTH				,	//
	STAT_PCFEAR				,	//
	STAT_PCLOVE				,	//
	STAT_PCHATE				,	//
	STAT_MORALITY			,	//
	STAT_REFINEMENT			,	//
	STAT_DIGNITY			,	//
	STAT_LACTATION			,	//
	STAT_STRENGTH			,	//
	STAT_NPCLOVE			,	// will be used for when a girl has a bf/gf to do different events
	STAT_SANITY				,	//SIN: Life is hard here...
	NUM_STATS					// 1 more than the last stat
};//End STATS enum

// `J` When modifying Stats or Skills, search for "J-Change-Stats-Skills"  :  found in >> Constants.h

// Skills
//SKILLS enum
enum SKILLS{
	SKILL_ANAL			= 0	,	//
	SKILL_MAGIC				,	//
	SKILL_BDSM				,	//
	SKILL_NORMALSEX			,	//
	SKILL_BEASTIALITY		,	//
	SKILL_GROUP				,	//
	SKILL_LESBIAN			,	//
	SKILL_SERVICE			,	//
	SKILL_STRIP				,	//
	SKILL_COMBAT			,	//
	SKILL_ORALSEX			,	//
	SKILL_TITTYSEX			,	//
	SKILL_MEDICINE			,	//
	SKILL_PERFORMANCE		,	//
	SKILL_HANDJOB			,	//
	SKILL_CRAFTING			,	//
	SKILL_HERBALISM			,	//
	SKILL_FARMING			,	//
	SKILL_BREWING			,	//
	SKILL_ANIMALHANDLING	,	//
	SKILL_FOOTJOB			,	//
	SKILL_COOKING			,	//
	NUM_SKILLS				,	// 1 more than the last skill
//const unsigned int SKILL_MAST		= ;
};//End SKILLS enum

// STATUS
//STATUS enum
enum STATUS{
	STATUS_NONE					= 0	,	//
	STATUS_POISONED					,	//
	STATUS_BADLY_POISONED			,	//
	STATUS_PREGNANT					,	//
	STATUS_PREGNANT_BY_PLAYER		,	//
	STATUS_SLAVE					,	//
	STATUS_HAS_DAUGHTER				,	//
	STATUS_HAS_SON					,	//
	STATUS_INSEMINATED				,	//
	STATUS_CONTROLLED				,	//
	STATUS_CATACOMBS				,	//
	STATUS_ARENA					,	//
	STATUS_YOURDAUGHTER				,	//
	STATUS_ISDAUGHTER				,	//
	STATUS_DATING_PERV				,	//
	STATUS_DATING_MEAN				,	//
	STATUS_DATING_NICE				,	//
	NUM_STATUS						,	// 1 more than the last status
};//End STATUS enum

// Jobs
// `J` When modifying Jobs, search for "J-Change-Jobs"  :  found in >> Constants.h

// Job Filter Enum
enum JOBFILTER {
  JOBFILTER_GENERAL = 0,
  JOBFILTER_BAR,
  JOBFILTER_GAMBHALL,
  JOBFILTER_SLEAZYBAR,
  JOBFILTER_BROTHEL,

  // `J` When adding new Studio Scenes, search for "J-Add-New-Scenes"  :  found in >> Constants.h > JOBFILTER
  JOBFILTER_STUDIOCREW,
  JOBFILTER_STUDIONONSEX,
  JOBFILTER_STUDIOSOFTCORE,		
  JOBFILTER_STUDIOPORN,			
  JOBFILTER_STUDIOHARDCORE,			
  JOBFILTER_RANDSTUDIO,
  // JOBFILTER_MOVIESTUDIO,		
  // JOBFILTER_EXTREMESTUDIO,		
  // JOBFILTER_PORNSTUDIO,		

  JOBFILTER_ARENA,
  JOBFILTER_ARENASTAFF,
  JOBFILTER_COMMUNITYCENTRE,
  JOBFILTER_COUNSELINGCENTRE,
  JOBFILTER_CLINIC,	// teaches can train girls only up to their own skill level
  JOBFILTER_CLINICSTAFF,
  JOBFILTER_FARMSTAFF,
  JOBFILTER_LABORERS,
  JOBFILTER_PRODUCERS,
  JOBFILTER_HOUSE,
  JOBFILTER_HOUSETTRAINING,
  //JOBFILTER_DRUGLAB,
  JOBFILTER_NONE,
  NUMJOBTYPES
};// END JOBFILTER Enum

// Jobs Enum
enum JOBS {
	// `J` Job Brothel - General
	JOB_RESTING = 0,		// relaxes and takes some time off
	JOB_TRAINING,			// trains skills at a basic level
	JOB_CLEANING,			// cleans the building
	JOB_SECURITY,			// Protects the building and its occupants
	JOB_ADVERTISING,		// Goes onto the streets to advertise the buildings services
	JOB_CUSTOMERSERVICE,	// looks after customers needs (customers are happier when people are doing this job)
	JOB_MATRON,				// looks after the needs of the girls (only 1 allowed)
	JOB_TORTURER,			// Tortures the people in the dungeons to help break their will (this is in addition to player torture) (only 1 allowed)
	JOB_EXPLORECATACOMBS,	// goes adventuring in the catacombs
	JOB_BEASTCARER,			// takes care of beasts that are housed in the brothel.
	// `J` Job Brothel - Bar
	JOB_BARMAID,			// serves at the bar
	JOB_WAITRESS,			// waits on the tables
	JOB_SINGER,				// sings in the bar
	JOB_PIANO,				// plays the piano for customers
	JOB_ESCORT,				// High lvl whore.  Sees less clients but needs higher skill high lvl items and the such to make them happy
	JOB_BARCOOK,			// cooks at the bar
	//const unsigned int JOB_WETTSHIRT		= ;
	// `J` Job Brothel - Hall
	JOB_DEALER,				// dealer for gambling tables
	JOB_ENTERTAINMENT,		// sings, dances and other shows for patrons
	JOB_XXXENTERTAINMENT,	// naughty shows for patrons
	JOB_WHOREGAMBHALL,		// looks after customers sexual needs
	//const unsigned int JOB_FOXYBOXING		= ;
	// `J` Job Brothel - Sleazy Bar
	JOB_SLEAZYBARMAID,		//
	JOB_SLEAZYWAITRESS,		//
	JOB_BARSTRIPPER,		//
	JOB_BARWHORE,			//
	// `J` Job Brothel - Brothel
	JOB_MASSEUSE,			// gives massages to patrons and sometimes sex
	JOB_BROTHELSTRIPPER,	// strips for customers and sometimes sex
	JOB_PEEP,				// Peep show
	JOB_WHOREBROTHEL,		// whore herself inside the building
	JOB_WHORESTREETS,		// whore self on the city streets



	// `J` Job Movie Studio - Crew
	JOB_FILMFREETIME,		// Free time
	JOB_DIRECTOR,			// Does same work as matron plus adds quality to films.
	JOB_PROMOTER,			// Advertising -- This helps film sales after it is created.
	JOB_CAMERAMAGE,			// Uses magic to record the scenes to crystals (requires at least 1)
	JOB_CRYSTALPURIFIER,	// Post editing to get the best out of the film (requires at least 1)
	//JOB_SOUNDTRACK,			// Correct audio and add in music to the scenes (not required but helpful)
	JOB_FLUFFER,			// Keeps the porn stars and animals aroused
	JOB_STAGEHAND,			// Currently does the same as a cleaner.

	// `J` When adding new Studio Scenes, search for "J-Add-New-Scenes"  :  found in >> Constants.h > JOBS

	// *****IMPORTANT**** If you add more scene types, they must go somewhere between
	// JOB_STAGEHAND and JOB_FILMRANDOM, or it will cause the random job to stop working..
	// the job after JOB_STAGEHAND must be the first film job, JOB_FILMRANDOM must be the last one.

	// Studio - Non-Sex Scenes
	JOB_FILMACTION,			// For sexy combatants
	JOB_FILMCHEF,			// For sexy cooking
	//JOB_FILMCOMEDY,			// For Comedy scenes
	//JOB_FILMDRAMA,			// For Drama scenes
	//JOB_FILMHORROR,			// For Horror scenes
	//JOB_FILMIDOL,			//For cool, sexy & cute girls
	JOB_FILMMUSIC,			// For cute music videos

	// Studio - Softcore Porn
	JOB_FILMMAST,			// films this type of scene CRAZY
	JOB_FILMSTRIP,			// films this type of scene CRAZY
	JOB_FILMTEASE,			//Fex sensual and cute

	// Studio - Porn
	JOB_FILMANAL,			// films this sort of scene in the movie
	JOB_FILMFOOTJOB,		// films this type of scene CRAZY
	//JOB_FILMFUTA,			// films scenes with/as futa
	JOB_FILMHANDJOB,		// films this type of scene CRAZY
	JOB_FILMLESBIAN,		// films this sort of scene in the movie. thinking about changing to Lesbian
	JOB_FILMORAL,			// films this type of scene CRAZY
	JOB_FILMSEX,			// films this sort of scene in the movie
	JOB_FILMTITTY,			// films this type of scene CRAZY

	// Studio - Hardcore porn
	JOB_FILMBEAST,			// films this sort of scene in the movie (uses beast resource)
	JOB_FILMBONDAGE,		// films this sort of scene in the movie
	JOB_FILMBUKKAKE,		// 
	JOB_FILMFACEFUCK,		// 
	JOB_FILMGROUP,			// films this sort of scene in the movie
	JOB_FILMPUBLICBDSM,		// 
	//JOB_FILMDOM,			//

	//Must go last
	JOB_FILMRANDOM,			// Films a random sex scene ... it does NOT work like most jobs, see following note.






	// `J` Job Arena - Fighting
	JOB_FIGHTBEASTS,		// customers come to place bets on who will win, girl may die (uses beasts resource)
	JOB_FIGHTARENAGIRLS,
	JOB_FIGHTTRAIN,
	//JOB_JOUSTING		= ;
	//JOB_MAGICDUEL		= ;
	//JOB_ARMSDUEL		= ;
	//JOB_FIGHTBATTLE	= ;
	//JOB_ATHELETE		= ;
	//JOB_RACING,				//	
	// `J` Job Arena - Staff
	JOB_ARENAREST,			//free time of arena
	JOB_DOCTORE,			//Matron of arena
	JOB_CITYGUARD,			//
	JOB_BLACKSMITH,			//
	JOB_COBBLER,			//
    JOB_JEWELER,
	//JOB_BATTLEMASTER	= ;
	//JOB_ARENAPROMOTER	= ;
	//JOB_BEASTMASTER	= ;
	//JOB_VENDOR			= ;
	//JOB_BOOKIE			= ;
	//JOB_GROUNDSKEEPER	= ;
	//JOB_MINER			= ;
	JOB_CLEANARENA,			//

	// `J` Job Centre - General
	JOB_CENTREREST,			// centres free time
	JOB_CENTREMANAGER,		// matron of centre	
	JOB_FEEDPOOR,			// work in a soup kitchen
	JOB_COMUNITYSERVICE,	// Goes around town helping where they can
	JOB_CLEANCENTRE,		// 	
	// TODO ideas:Run a charity, with an option for the player to steal from charity (with possible bad outcome). Run schools/orphanages.. this should give a boost to the stats of new random girls, and possibly be a place to recruit new uniques.
	// Homeless shelter... once again a possible place to find new girls.
	// `J` Job Centre - Rehab
	JOB_COUNSELOR,			// 	
	JOB_REHAB,				// 	
	JOB_ANGER,				// 	
	JOB_EXTHERAPY,			// 	
	JOB_THERAPY,			// 		

	// `J` Job Clinic - Surgery
	JOB_GETHEALING,			// takes 1 days for each wound trait received.
	JOB_GETREPAIRS,			// construct girls can get repaired quickly
	JOB_CUREDISEASES,		// Cure diseases
	JOB_GETABORT,			// gets an abortion (takes 2 days)
	JOB_COSMETICSURGERY,	// magical plastic surgery (takes 5 days)
	JOB_LIPO,				// magical plastic surgery (takes 5 days)
	JOB_BREASTREDUCTION,	// magical plastic surgery (takes 5 days)
	JOB_BOOBJOB,			// magical plastic surgery (takes 5 days)
	JOB_VAGINAREJUV,		// magical plastic surgery (takes 5 days)
	JOB_FACELIFT,			// magical plastic surgery (takes 5 days)
	JOB_ASSJOB,				// magical plastic surgery (takes 5 days)
	JOB_TUBESTIED,			// magical plastic surgery (takes 5 days)
	JOB_FERTILITY,			// magical plastic surgery (takes 5 days)

	// `J` Job Clinic - Staff
	JOB_CLINICREST,			// Is clinics free time	
	JOB_CHAIRMAN,			// Matron of clinic	
	JOB_DOCTOR,				// becomes a doctor (requires 1) (will make some extra cash for treating locals)
	JOB_NURSE,				// helps girls recover from surgery on healing
	JOB_MECHANIC,			// construct girls can get repaired quickly
	JOB_INTERN,				// training for nurse job
	JOB_JANITOR,			// cleans clinic

	// Job Clinic - Drug lab
	//JOB_DRUGDEALER,	

	// `J` Job Farm - Staff
	JOB_FARMREST,			//farm rest
	JOB_FARMMANGER,			//matron of farm
	JOB_VETERINARIAN,		//tends to the animals to keep them from dying - full time
	JOB_MARKETER,			// buys and sells food - full time
	JOB_RESEARCH,			// potions - unlock various types of potions and garden qualities - full time
	JOB_FARMHAND,			//cleaning of the farm
	// `J` Job Farm - Laborers
	JOB_FARMER,				//tends crops
	JOB_GARDENER,			// produces herbs and potion ingredients
	JOB_SHEPHERD,			//tends food animals - 100% food
	JOB_RANCHER,			// tends animals for food or beast - % food/beast based on skills
	JOB_CATACOMBRANCHER,	//tends strange beasts - 100% beast - dangerous
	JOB_BEASTCAPTURE,		//
	JOB_MILKER,				// produces milk from animals/beasts/girls - if food animals < beasts - can be dangerous
	JOB_MILK,				//milker not required but increases yield
	// `J` Job Farm - Producers
	JOB_BUTCHER,			// produces food from animals
	JOB_BAKER,				// produces food from crops
	JOB_BREWER,				// Produces beers and wines
	JOB_TAILOR,				// Produces beers and wines
	JOB_MAKEITEM,			// produce items for sale
	JOB_MAKEPOTIONS,		// create potions with items gained from the garden

	// `J` Job House - General
	JOB_HOUSEREST,			//
	JOB_HEADGIRL,			//
	JOB_RECRUITER,			//
	JOB_PERSONALBEDWARMER,	//
	JOB_HOUSECOOK,			// cooks for the harem, (helps keep them happy, and increase job performance)
	//JOB_HOUSEDATE,
	//JOB_HOUSEVAC,
	JOB_CLEANHOUSE,			//

	// `J` Job House - Training
	JOB_PERSONALTRAINING,	//
	JOB_FAKEORGASM,			//
	JOB_SO_STRAIGHT,		//
	JOB_SO_BISEXUAL,		//
	JOB_SO_LESBIAN,			//
	JOB_HOUSEPET,			//		
	//JOB_PONYGIRL,			//		



	// - extra unassignable
	JOB_INDUNGEON,			//
	JOB_RUNAWAY,			//

	NUM_JOBS,				// number of Jobs
};// End JOBS enum
#if 0
// - Community Centre
const unsigned int JOBFILTER_COMMUNITYCENTRE = ;
const unsigned int JOB_COLLECTDONATIONS = ;	// not added yet	// collects money to help the poor
// - Drug Lab (these jobs gain bonus if in same building as a clinic)
	JOB_VIRASPLANTFUCKER,	// not added yet	// the plants need to inseminate with other humanoids to proper-gate, creates vira blood items
	JOB_SHROUDGROWER,		// not added yet	// They require lots of care, and may explode. Produces shroud mushroom item.
	JOB_FAIRYDUSTER,		// not added yet	// captures and pounds faries to dust, produces fairy dust item
const unsigned int JOB_DRUGDEALER       = ;	// not added yet	// goes out onto the streets to sell the items made with the other jobs
// - Alchemist lab (these jobs gain bonus if in same building as a drug lab)
const unsigned int JOBFILTER_ALCHEMISTLAB = ;
const unsigned int JOB_FINDREGENTS      = ;	// not added yet	// goes around and finds ingredients for potions, produces ingredients resource.
const unsigned int JOB_BREWPOTIONS      = ;	// not added yet	// will randomly produce any items with the word "potion" in the name, uses ingredients resource
const unsigned int JOB_POTIONTESTER     = ;	// not added yet	// Potion tester tests potions, they may die doing this or random stuff may happen. (requires 1)
// - Arena (these jobs gain bonus if in same building as a clinic)
const unsigned int JOBFILTER_ARENA      = ;
const unsigned int JOB_FIGHTBEASTS      = ;	// not added yet	// customers come to place bets on who will win, girl may die (uses beasts resource)
const unsigned int JOB_WRESTLE          = ;	// not added yet	// as above no chance of dying
const unsigned int JOB_FIGHTTODEATH     = ;	// not added yet	// as above but against other girls (chance of dying)
const unsigned int JOB_FIGHTVOLUNTEERS  = ;	// not added yet	// fight against customers who volunteer for prizes of gold
const unsigned int JOB_COLLECTBETS      = ;	// not added yet	// collects the bets from customers (requires 1)
// - Skills Centre
const unsigned int JOBFILTER_TRAININGCENTRE = ;	// teaches can train girls only up to their own skill level
const unsigned int JOB_TEACHBDSM        = ;	// not added yet	// teaches BDSM skills
const unsigned int JOB_TEACHSEX         = ;	// not added yet	// teaches general sex skills
const unsigned int JOB_TEACHBEAST       = ;	// not added yet	// teaches beastiality skills
const unsigned int JOB_TEACHMAGIC       = ;	// not added yet	// teaches magic skills
const unsigned int JOB_TEACHCOMBAT      = ;	// not added yet	// teaches combat skills
const unsigned int JOB_DAYCARE          = ;	// not added yet	// looks after the local kids (non sex stuff of course)
const unsigned int JOB_SCHOOLING        = ;	// not added yet	// teaches the local kids (non sex stuff of course)
const unsigned int JOB_TEACHDANCING     = ;	// not added yet	// teaches dancing and social skills
const unsigned int JOB_TEACHSERVICE     = ;	// not added yet	// teaches misc skills
const unsigned int JOB_TRAIN            = ;	// not added yet	// trains the girl in all the disicplince for which there is a teacher

#endif


// Goon missions
enum Gang_Missions{
	MISS_GUARDING = 0,						// guarding your businesses
	MISS_SABOTAGE,							// sabotaging rival business
	MISS_SPYGIRLS,							// checking up on the girls while they work
	MISS_CAPTUREGIRL,						// looking for runaway girls
	MISS_EXTORTION,							// exthortion of local business for money in return for protection
	MISS_PETYTHEFT,							// go out on the streets and steal from people
	MISS_GRANDTHEFT,						// Go and rob local business while noone is there
	MISS_KIDNAPP,							// go out and kidnap homeless or lost girls
	MISS_CATACOMBS,							// men go down into the catacombs to find treasures
	MISS_TRAINING,							// men improve their skills
	MISS_RECRUIT,							// men recuit their number better
	MISS_SERVICE,							// men will do community service - `J` added for .06.02.41
	MISS_DUNGEON,							// men will help break girls in the dungeon
};

// Reasons for keeping them in the dungeon
enum Dungeon_Reasons{
	DUNGEON_RELEASE = 0,					// released from the dungeon on next update
	DUNGEON_CUSTNOPAY,						// a customer that failed to pay or provide adiquate compensation
	DUNGEON_GIRLCAPTURED,					// A new girl that was captured
	DUNGEON_GIRLKIDNAPPED,					// A new girl taken against her will
	DUNGEON_CUSTBEATGIRL,					// A customer that was found hurting a girl
	DUNGEON_CUSTSPY,						// A customer that was found to be a spy for a rival
	DUNGEON_RIVAL,							// a captured rival
	DUNGEON_GIRLWHIM,						// a girl placed here on a whim
	DUNGEON_GIRLSTEAL,						// a girl that was placed here after being found stealing extra
	DUNGEON_DEAD,							// this person has died and will be removed next turn
	DUNGEON_GIRLRUNAWAY,					// girl ran away but was recaptured
	DUNGEON_NEWSLAVE,						// a newly brought slave
	DUNGEON_NEWGIRL,						// a new girl who just joined you
	DUNGEON_KID,							// a girl child just aged up
	DUNGEON_NEWARENA,						// a new girl who just joined you from the arena
	DUNGEON_RECRUITED,						// a new girl who was just recruited
};

// Town Constants
const int TOWN_NUMBUSINESSES        = 250;	// the amount of businesses in the town
const int TOWN_OFFICIALSWAGES       = 10;	// how much the authorities in the town are paid by the govenment

// Incomes
const int INCOME_BUSINESS           = 10;

// Item types
const int NUM_SHOPITEMS             = 40;	// number of items that the shop may hold at one time
enum Item_Type{
	INVRING = 1,							// worn on fingers (max 8)
	INVDRESS,								// Worn on body, (max 1)
	INVSHOES,								// worn on feet, (max 1) often unequipped when going into combat
	INVFOOD,								// Eaten, single use
	INVNECKLACE,							// worn on neck, (max 1)
	INVWEAPON,								// equipped on body, often unequipped outside of combat, (max 2)
	INVMAKEUP,								// worn on face, single use
	INVARMOR,								// worn on body over dresses, often unequipped outside of combat, (max 1)
	INVMISC,								// these items don't usually do anythinig just random stuff girls might buy. The ones that do, cause a constant effect without having to be equiped
	INVARMBAND,								// (max 2), worn around arms
	INVSMWEAPON,							// small weapon which can be hidden on body, (max 2)
	INVUNDERWEAR,							// CRAZY added this - underwear (max 1)
	INVHAT,									// CRAZY added this - Noncombat worn on the head (max 1)
	INVHELMET,								// CRAZY added this	- Combat worn on the head (max 1)
	INVGLASSES,								// CRAZY added this	- Glasses (max 1)
	INVSWIMSUIT,							// CRAZY added this - Swimsuit (max 1 in use but can have as many as they want)
	INVCOMBATSHOES,							// `J`   added this - Combat Shoes (max 1) often unequipped outside of combat
	INVSHIELD								// `J`   added this - Shields (max 1) often unequipped outside of combat
};
//const unsigned int INVLEGS = ;		//CRAZY added this

// Item Rarity
enum Item_Rarity{
	RARITYCOMMON = 0,						// 
	RARITYSHOP50,							// 
	RARITYSHOP25,							// 
	RARITYSHOP05,							// 
	RARITYCATACOMB15,						// 
	RARITYCATACOMB05,						// 
	RARITYCATACOMB01,						// 
	RARITYSCRIPTONLY,						// 
	RARITYSCRIPTORREWARD,					// 
	NUM_ITEM_RARITY							//
};

// Random objectives (well kinda random, they appear to guide the player for the win)
enum Objectives{
	OBJECTIVE_REACHGOLDTARGET = 0,		// 
	OBJECTIVE_GETNEXTBROTHEL,			// 
	OBJECTIVE_LAUNCHSUCCESSFULATTACK,	// 
	OBJECTIVE_HAVEXGOONS,				// 
	OBJECTIVE_STEALXAMOUNTOFGOLD,		// 
	OBJECTIVE_CAPTUREXCATACOMBGIRLS,	// 
	OBJECTIVE_HAVEXMONSTERGIRLS,		// 
	OBJECTIVE_KIDNAPXGIRLS,				// 
	OBJECTIVE_EXTORTXNEWBUSINESS,		// 
	OBJECTIVE_HAVEXAMOUNTOFGIRLS,		// 
	NUM_OBJECTIVES						// 
};

// Objective reward types
enum Rewards{
	REWARD_GOLD = 0,					//
	REWARD_GIRLS,						//
	REWARD_RIVALHINDER,					//
	REWARD_ITEM,						//
	NUM_REWARDS							//
};

// customer fetishes (important that they are ordered from normal to weird)
enum Fetishs{
	FETISH_TRYANYTHING = 0,				// will like and try any form of sex (a nympho basically)
	FETISH_SPECIFICGIRL,				// wants a particular girl
	FETISH_BIGBOOBS,					// likes girls with big boobs
	FETISH_SEXY,						// likes girls with lots of sex appeal
	FETISH_CUTEGIRLS,					// Likes girls that are cute
	FETISH_FIGURE,						// likes girls with good figures
	FETISH_LOLITA,						// likes lolitas
	FETISH_ARSE,						// likes girls with good arses
	FETISH_COOLGIRLS,					// likes cool girls, may chat with them a little
	FETISH_ELEGANT,						// likes girls with class
	FETISH_NERDYGIRLS,					// likes nerds or clumsy girls
	FETISH_SMALLBOOBS,					// likes girls with small boobs
	FETISH_DANGEROUSGIRLS,				// likes girls with a bit of danger
	FETISH_NONHUMAN,					// likes non human girls
	FETISH_FREAKYGIRLS,					// likes freaky girls
	FETISH_FUTAGIRLS,					// likes girls with dicks
	FETISH_TALLGIRLS,					// likes tall girls
	FETISH_SHORTGIRLS,					// likes short girls
	FETISH_FATGIRLS,					// likes fat girls
	NUM_FETISH							// 
};

enum Goals{								// `J` new
	GOAL_UNDECIDED = 0,					// The customer is not sure what they want to do
	GOAL_FIGHT,							// The customer wants to start a fight
	GOAL_RAPE,							// They want to rape someone?
	GOAL_SEX,							// The customer wants to get laid
	GOAL_GETDRUNK,						// The customer wants to get drunk
	GOAL_GAMBLE,						// The customer wants to gamble
	GOAL_ENTERTAINMENT,					// The customer wants to be entertained
	GOAL_XXXENTERTAINMENT,				// They want sexual entertainment
	GOAL_LONELY,						// They just want company and a friendly ear
	GOAL_MASSAGE,						// Their muscles hurt and want someone to work on it, (should Happy Ending be a separate job?)
	GOAL_STRIPSHOW,						// They want to see someone naked
	GOAL_FREAKSHOW,						// They want to see something strange, nonhuman or just different
	GOAL_CULTURALEXPLORER,				// They want to have sex with something strange, nonhuman or just different
	GOAL_OTHER,							// The customer wants to do something different
	NUM_GOALS							// 
};


// actions girls can disobey against
// `J` When modifying Action types, search for "J-Change-Action-Types"  :  found in >> Constants.h
enum Action_Types{
	ACTION_COMBAT = 0,
	ACTION_SEX,
	ACTION_WORKESCORT,
	ACTION_WORKCLEANING,
	ACTION_WORKMATRON,
	ACTION_WORKBAR,
	ACTION_WORKHALL,
	ACTION_WORKSHOW,
	ACTION_WORKSECURITY,
	ACTION_WORKADVERTISING,
	ACTION_WORKTORTURER,
	ACTION_WORKCARING,
	ACTION_WORKDOCTOR,
	ACTION_WORKMOVIE,
	ACTION_WORKCUSTSERV,
	ACTION_WORKCENTRE,
	ACTION_WORKCLUB,
	ACTION_WORKHAREM,
	ACTION_WORKRECRUIT,
	ACTION_WORKNURSE,
	ACTION_WORKMECHANIC,
	ACTION_WORKCOUNSELOR,
	ACTION_WORKMUSIC,
	ACTION_WORKSTRIP,
	ACTION_WORKMILK,
	ACTION_WORKMASSEUSE,
	ACTION_WORKFARM,
	ACTION_WORKTRAINING,
	ACTION_WORKREHAB,
	ACTION_WORKMAKEPOTIONS,
	ACTION_WORKMAKEITEMS,
	ACTION_WORKCOOKING,
	ACTION_WORKTHERAPY,
	ACTION_WORKHOUSEPET,
	
	// ACTION_GENERAL must be the last sction type
	ACTION_GENERAL,
	NUM_ACTIONTYPES
};

// training girls can under take
// When modifying Training types, search for "Change-Traning-Types"  :  found in >> Constants.h
enum Training_Types{
	TRAINING_PUPPY = 0,
	TRAINING_PONY,
	// TRAINING_GENERAL must be the last sction type
	TRAINING_GENERAL,
	NUM_TRAININGTYPES
};

// Character image
// `J` When modifying Image types, search for "J-Change-Image-Types"  :  found in >> Constants.h
enum Image_Types{
	IMGTYPE_ANAL = 0,					// 
	IMGTYPE_BDSM,						// 
	IMGTYPE_SEX,						// 
	IMGTYPE_BEAST,						// 
	IMGTYPE_GROUP,						// 
	IMGTYPE_LESBIAN,					// 
	IMGTYPE_TORTURE,					// `J` added
	IMGTYPE_DEATH,						// 
	IMGTYPE_PROFILE,					// 
	IMGTYPE_COMBAT,						// 
	IMGTYPE_ORAL,						// 
	IMGTYPE_ECCHI,						// 
	IMGTYPE_STRIP,						// 
	IMGTYPE_MAID,						// 
	IMGTYPE_SING,						// 
	IMGTYPE_WAIT,						// 
	IMGTYPE_CARD,						// 
	IMGTYPE_BUNNY,						// 
	IMGTYPE_NUDE,						// 
	IMGTYPE_MAST,						// 
	IMGTYPE_TITTY,						// 
	IMGTYPE_MILK,						// 
	IMGTYPE_HAND,						// 
	IMGTYPE_FOOT,						// 
	IMGTYPE_BED,						// 
	IMGTYPE_FARM,						// 
	IMGTYPE_HERD,						// 
	IMGTYPE_COOK,						// 
	IMGTYPE_CRAFT,						// 
	IMGTYPE_SWIM,						// 
	IMGTYPE_BATH,						// 
	IMGTYPE_NURSE,						// 
	IMGTYPE_FORMAL,						// 
	IMGTYPE_SHOP,						// 
	IMGTYPE_MAGIC,						// 
	IMGTYPE_SIGN,						// Going be used for advertising
	IMGTYPE_PRESENTED,					// Going be used for Slave Market
	IMGTYPE_DOM,						// 
	IMGTYPE_DEEPTHROAT,					// 
	IMGTYPE_EATOUT,						// 
	IMGTYPE_DILDO,						// 
	IMGTYPE_SUB,						// 
	IMGTYPE_STRAPON,					// 
	IMGTYPE_LES69ING,					// 
	IMGTYPE_LICK,						// 
	IMGTYPE_SUCKBALLS,					// 
	IMGTYPE_COWGIRL,					// 
	IMGTYPE_REVCOWGIRL,					// 
	IMGTYPE_SEXDOGGY,					// 
	IMGTYPE_JAIL,						// 
	IMGTYPE_PUPPYGIRL,					// 
	IMGTYPE_PONYGIRL,					// 
	IMGTYPE_CATGIRL,					// 
	IMGTYPE_BRAND,						// `J` new .06.03.01 for DarkArk
	IMGTYPE_RAPE,						// `J` new .06.03.01 for DarkArk
	IMGTYPE_RAPEBEAST,					// `J` new .06.03.01 for DarkArk
	IMGTYPE_BIRTHHUMAN,					// `J` new .06.03.01 for DarkArk
	IMGTYPE_BIRTHHUMANMULTIPLE,			// `J` new .06.03.01 for DarkArk
	IMGTYPE_BIRTHBEAST,					// `J` new .06.03.01 for DarkArk
	IMGTYPE_IMPREGSEX,					// `J` new .06.03.01 for DarkArk
	IMGTYPE_IMPREGGROUP,				// `J` new .06.03.01 for DarkArk
	IMGTYPE_IMPREGBDSM,					// `J` new .06.03.01 for DarkArk
	IMGTYPE_IMPREGBEAST,				// `J` new .06.03.01 for DarkArk
	IMGTYPE_VIRGINSEX,					// `J` new .06.03.01 for DarkArk
	IMGTYPE_VIRGINGROUP,				// `J` new .06.03.01 for DarkArk
	IMGTYPE_VIRGINBDSM,					// `J` new .06.03.01 for DarkArk
	IMGTYPE_VIRGINBEAST,				// `J` new .06.03.01 for DarkArk
	IMGTYPE_ESCORT,						// `J` new .06.03.02 for grishnak - clothed with a client
	IMGTYPE_SPORT,						// `J` new .06.03.02 for grishnak - for the free time job, doing sports
	IMGTYPE_STUDY,						// `J` new .06.03.02 for grishnak - reading a book or looking clever
	IMGTYPE_TEACHER,					// `J` new .06.03.02 for grishnak - dressed or acting as a teacher

	/*
IMGTYPE_WATER			- "water*."			- "Watersports"		- watersports
IMGTYPE_PETPROFILE		- "pet*."			- "Pet"				- profile, nude
IMGTYPE_PETORAL			- "petoral*."		- "PetOral"			- oral, lick, deepthroat, titty, petprofile, nude, bdsm
IMGTYPE_PETSEX			- "petsex*."		- "PetSex"			- sex, nude, anal, petprofile, bdsm
IMGTYPE_PETBEAST		- "petbeast*."		- "PetBeastiality"	- beast, sex, anal, bdsm, nude
IMGTYPE_PETFEED			- "petfeed*."		- "PetFeed"			- oral, lick, petprofile, bdsm, nude
IMGTYPE_PETPLAY			- "petplay*."		- "PetPlay"			- petprofile, bdsm, nude
IMGTYPE_PETTOY			- "pettoy*."		- "PetToy"			- dildo, petprofile, oral, mast, bdsm, nude
IMGTYPE_PETWALK			- "petwalk*."		- "PetWalk"			- petprofile, bdsm, nude
IMGTYPE_PETLICK			- "petlick*."		- "PetLick"			- lick, oral, petprofile, bdsm, nude, titty
	*/


	// IMGTYPE_PREGNANT needs to be the last of the nonpregnant image types.
	IMGTYPE_PREGNANT,					// 

	// `J` All image types can have a pregnant alternative now
	IMGTYPE_PREGANAL,					// 
	IMGTYPE_PREGBDSM,					// 
	IMGTYPE_PREGSEX,					// 
	IMGTYPE_PREGBEAST,					// 
	IMGTYPE_PREGGROUP,					// 
	IMGTYPE_PREGLESBIAN,				// 
	IMGTYPE_PREGTORTURE,				// 
	IMGTYPE_PREGDEATH,					// 
	IMGTYPE_PREGPROFILE,				// 
	IMGTYPE_PREGCOMBAT,					// 
	IMGTYPE_PREGORAL,					// 
	IMGTYPE_PREGECCHI,					// 
	IMGTYPE_PREGSTRIP,					// 
	IMGTYPE_PREGMAID,					// 
	IMGTYPE_PREGSING,					// 
	IMGTYPE_PREGWAIT,					// 
	IMGTYPE_PREGCARD,					// 
	IMGTYPE_PREGBUNNY,					// 
	IMGTYPE_PREGNUDE,					// 
	IMGTYPE_PREGMAST,					// 
	IMGTYPE_PREGTITTY,					// 
	IMGTYPE_PREGMILK,					// 
	IMGTYPE_PREGHAND,					// 
	IMGTYPE_PREGFOOT,					// 
	IMGTYPE_PREGBED,					// 
	IMGTYPE_PREGFARM,					// 
	IMGTYPE_PREGHERD,					// 
	IMGTYPE_PREGCOOK,					// 
	IMGTYPE_PREGCRAFT,					// 
	IMGTYPE_PREGSWIM,					// 
	IMGTYPE_PREGBATH,					// 
	IMGTYPE_PREGNURSE,					// 
	IMGTYPE_PREGFORMAL,					// 
	IMGTYPE_PREGSHOP,					// 
	IMGTYPE_PREGMAGIC,					// 
	IMGTYPE_PREGSIGN,					// 
	IMGTYPE_PREGPRESENTED,				// 
	IMGTYPE_PREGDOM,					// 
	IMGTYPE_PREGDEEPTHROAT,				// 
	IMGTYPE_PREGEATOUT,					// 
	IMGTYPE_PREGDILDO,					// 
	IMGTYPE_PREGSUB,					// 
	IMGTYPE_PREGSTRAPON,				// 
	IMGTYPE_PREGLES69ING,				// 
	IMGTYPE_PREGLICK,					// 
	IMGTYPE_PREGSUCKBALLS,				// 
	IMGTYPE_PREGCOWGIRL,				// 
	IMGTYPE_PREGREVCOWGIRL,				// 
	IMGTYPE_PREGSEXDOGGY,				// 
	IMGTYPE_PREGJAIL,					// 
	IMGTYPE_PREGPUPPYGIRL,				// 
	IMGTYPE_PREGPONYGIRL,				// 
	IMGTYPE_PREGCATGIRL,				// 
	IMGTYPE_PREGBRAND,					// `J` new .06.03.01 for DarkArk
	IMGTYPE_PREGRAPE,					// `J` new .06.03.01 for DarkArk
	IMGTYPE_PREGRAPEBEAST,				// `J` new .06.03.01 for DarkArk
	IMGTYPE_PREGBIRTHHUMAN,				// `J` new .06.03.01 for DarkArk - these are just in here for completion, they probably should not be used
	IMGTYPE_PREGBIRTHHUMANMULTIPLE,		// `J` new .06.03.01 for DarkArk - these are just in here for completion, they probably should not be used
	IMGTYPE_PREGBIRTHBEAST,				// `J` new .06.03.01 for DarkArk - these are just in here for completion, they probably should not be used
	IMGTYPE_PREGIMPREGSEX,				// `J` new .06.03.01 for DarkArk - these are just in here for completion, they probably should not be used
	IMGTYPE_PREGIMPREGGROUP,			// `J` new .06.03.01 for DarkArk - these are just in here for completion, they probably should not be used
	IMGTYPE_PREGIMPREGBDSM,				// `J` new .06.03.01 for DarkArk - these are just in here for completion, they probably should not be used
	IMGTYPE_PREGIMPREGBEAST,			// `J` new .06.03.01 for DarkArk - these are just in here for completion, they probably should not be used
	IMGTYPE_PREGVIRGINSEX,				// `J` new .06.03.01 for DarkArk - these are just in here for completion, they probably should not be used
	IMGTYPE_PREGVIRGINGROUP,			// `J` new .06.03.01 for DarkArk - these are just in here for completion, they probably should not be used
	IMGTYPE_PREGVIRGINBDSM,				// `J` new .06.03.01 for DarkArk - these are just in here for completion, they probably should not be used
	IMGTYPE_PREGVIRGINBEAST,			// `J` new .06.03.01 for DarkArk - these are just in here for completion, they probably should not be used
	IMGTYPE_PREGESCORT,					// `J` new .06.03.02 for grishnak - clothed with a client
	IMGTYPE_PREGSPORT,					// `J` new .06.03.02 for grishnak - for the free time job, doing sports
	IMGTYPE_PREGSTUDY,					// `J` new .06.03.02 for grishnak - reading a book or looking clever
	IMGTYPE_PREGTEACHER,				// `J` new .06.03.02 for grishnak - dressed or acting as a teacher

	NUM_IMGTYPES	// `J` All image types can have a pregnant variation (except pregnant-pregnant)
};
const int PREG_OFFSET = IMGTYPE_PREGNANT + 1;


const int NUM_GIRLFLAGS				= 30;
const int MAXNUM_TRAITS				= 60;		// Maximum number of traits a girl can have
const int MAXNUM_INVENTORY			= 3000;		// Maximum number of items in inventory
const int MAXNUM_GIRL_INVENTORY		= 40;		// Maximum number of items a Girl can have in inventory
const int MAXNUM_RIVAL_INVENTORY	= 40;		// Maximum number of items a Rival can have in inventory

const int LISTBOX_ITEMHEIGHT = 20;		//how tall (in pixels) each list item is
const unsigned int LISTBOX_COLUMNS = 25;		//how many columns are allowed

// Listbox Constants moved from cListBox.h
const unsigned int COLOR_BLUE		= 0;
const unsigned int COLOR_RED		= 1;
const unsigned int COLOR_DARKBLUE	= 2;
const unsigned int COLOR_GREEN		= 3;
const unsigned int COLOR_YELLOW		= 4;	// `J` added

// Event constants oved from cEvents.h
const int	EVENT_DAYSHIFT		= 0;
const int	EVENT_NIGHTSHIFT	= 1;
const int	EVENT_WARNING		= 2;
const int	EVENT_DANGER		= 3;
const int	EVENT_GOODNEWS		= 4;
const int	EVENT_SUMMARY		= 5;
const int	EVENT_DUNGEON		= 6;			// For torturer reports
const int	EVENT_MATRON		= 7;			// For Matron reports
const int	EVENT_GANG			= 8;
const int	EVENT_BROTHEL		= 9;
const int	EVENT_NOWORK		= 10;
const int	EVENT_BACKTOWORK	= 11;
const int	EVENT_LEVELUP		= 12;	// `J` added
const int	EVENT_DEBUG			= 99;


// Constants determining which screen is currently showing. This will help with hotkeys and help menu. --PP
// The variable that uses this constant is int g_CurrentScreen;
const int SCREEN_BROTHEL			= 0;
const int SCREEN_TURNSUMMARY		= 1;
const int SCREEN_GIRLMANAGEMENT		= 2;
const int SCREEN_GIRLDETAILS		= 3;
const int SCREEN_INVENTORY			= 4;
const int SCREEN_GALLERY			= 5;
const int SCREEN_TRANSFERGIRLS		= 6;
const int SCREEN_GANGMANAGEMENT		= 7;
const int SCREEN_BROTHELMANAGEMENT	= 8;
const int SCREEN_DUNGEON			= 9;
const int SCREEN_TOWN				= 10;
const int SCREEN_MAYOR				= 11;
const int SCREEN_BANK				= 12;
const int SCREEN_JAIL				= 13;
const int SCREEN_HOUSE				= 14;
const int SCREEN_CLINIC				= 15;
const int SCREEN_ARENA				= 16;
const int SCREEN_TRYOUTS			= 17;
const int SCREEN_CENTRE				= 18;
const int SCREEN_STUDIO				= 19;
const int SCREEN_CREATEMOVIE		= 20;
const int SCREEN_BUILDINGMANAGEMENT	= 21;
const int SCREEN_MAINMENU			= 22;
const int SCREEN_SLAVEMARKET		= 23;
const int SCREEN_PLAYERHOUSE		= 24; 
const int SCREEN_GALLERY2			= 25;
const int SCREEN_GETINPUT           = 26;
const int SCREEN_PROPERTYMANAGEMENT = 27;	// `J` added for managing all properties on 1 page
const int SCREEN_FARM				= 28;
const int SCREEN_NEWGAME			= 29;
const int SCREEN_PREPARING			= 29;
const int SCREEN_SETTINGS			= 30;	// `J` added

// The following constants are used with g_CurrBrothel to determine if we are currently working with a brothel or a new building. --PP
const int BUILDING_BROTHEL			= 0;
const int BUILDING_STUDIO			= 10;
const int BUILDING_CLINIC			= 20;
const int BUILDING_ARENA			= 30;
const int BUILDING_CENTRE			= 40;
const int BUILDING_HOUSE			= 50;
const int BUILDING_FARM				= 60;
const int BUILDING_ERROR			= 70;	// This shows there was an error somehow, looking for a building that does not exist.


// The following constants are used with counting child types for girls.
const int CHILD00_TOTAL_BIRTHS		= 0;
const int CHILD01_ALL_BEASTS		= 1;
const int CHILD02_ALL_GIRLS			= 2;
const int CHILD03_ALL_BOYS			= 3;
const int CHILD04_CUSTOMER_GIRLS	= 4;
const int CHILD05_CUSTOMER_BOYS		= 5;
const int CHILD06_YOUR_GIRLS		= 6;
const int CHILD07_YOUR_BOYS			= 7;
const int CHILD08_MISCARRIAGES		= 8;
const int CHILD09_ABORTIONS			= 9;
const int CHILD_COUNT_TYPES			= 10; // last type+1
#endif