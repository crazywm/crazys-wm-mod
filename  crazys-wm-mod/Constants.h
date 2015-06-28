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
  JOBFILTER_MOVIESTUDIO,
  JOBFILTER_STUDIOCREW,
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

	// `J` Job Movie Studio - Actress
	JOB_FILMBEAST,			// films this sort of scene in the movie (uses beast resource)
	JOB_FILMSEX,			// films this sort of scene in the movie
	JOB_FILMANAL,			// films this sort of scene in the movie
	JOB_FILMLESBIAN,		// films this sort of scene in the movie. thinking about changing to Lesbian
	JOB_FILMBONDAGE,		// films this sort of scene in the movie
	JOB_FILMGROUP,			// films this sort of scene in the movie
	JOB_FILMORAL,			// films this type of scene CRAZY
	JOB_FILMMAST,			// films this type of scene CRAZY
	JOB_FILMTITTY,			// films this type of scene CRAZY
	JOB_FILMSTRIP,			// films this type of scene CRAZY
	JOB_FILMHANDJOB,		// films this type of scene CRAZY
	JOB_FILMFOOTJOB,		// films this type of scene CRAZY
	JOB_FILMRANDOM,			// Films a random sex scene ... it does NOT work like most jobs, see following note.
	// *****IMPORTANT**** If you add more scene types, they must go somewhere between
	// JOB_FILMBEAST and JOB_FILMRANDOM, or it will cause the random job to stop working..
	// JOB_FILMBEAST must be the first film job, JOB_FILMRANDOM must be the last one. --PP
	// `J` Job Movie Studio - Crew
	JOB_FILMFREETIME,		// Free time
	JOB_DIRECTOR,			// Does same work as matron plus adds quality to films.
	JOB_PROMOTER,			// Advertising -- This helps film sales after it is created.
	JOB_CAMERAMAGE,			// Uses magic to record the scenes to crystals (requires at least 1)
	JOB_CRYSTALPURIFIER,	// Post editing to get the best out of the film (requires at least 1)
	JOB_FLUFFER,			// Keeps the porn stars and animals aroused
	JOB_STAGEHAND,			// Currently does the same as a cleaner.

	// `J` Job Arena - Fighting
	JOB_FIGHTBEASTS,		// customers come to place bets on who will win, girl may die (uses beasts resource)
	JOB_FIGHTARENAGIRLS,
	JOB_FIGHTTRAIN,
	//const unsigned int JOB_JOUSTING		= ;
	//const unsigned int JOB_MAGICDUEL		= ;
	//const unsigned int JOB_ARMSDUEL		= ;
	//const unsigned int JOB_FIGHTBATTLE	= ;
	//const unsigned int JOB_ATHELETE		= ;
	//const unsigned int JOB_RACING			= ;
	// `J` Job Arena - Staff
	JOB_ARENAREST,			//free time of arena
	JOB_DOCTORE,			//Matron of arena
	JOB_CITYGUARD,			//
	JOB_BLACKSMITH,			//
	JOB_COBBLER,			//
    JOB_JEWELER,
	JOB_CLEANARENA,			//
	//const unsigned int JOB_BATTLEMASTER	= ;
	//const unsigned int JOB_ARENAPROMOTER	= ;
	//const unsigned int JOB_BEASTMASTER	= ;
	//const unsigned int JOB_VENDOR			= ;
	//const unsigned int JOB_BOOKIE			= ;
	//const unsigned int JOB_GROUNDSKEEPER	= ;
	//const unsigned int JOB_MINER			= ;

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
	JOB_GETABORT,			// gets an abortion (takes 2 days)
	JOB_PHYSICALSURGERY,	// magical plastic surgery (takes 5 days)
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
	JOB_PERSONALTRAINING,	//
	JOB_PERSONALBEDWARMER,	//
	JOB_CLEANHOUSE,			//
	//const unsigned int JOB_HOUSECOOK		= ;    // cooks for the harem, (helps keep them happy, and increase job performance)
	//const unsigned int JOB_HOUSEDATE		= ;
	//const unsigned int JOB_HOUSEVAC		= ;

	// - extra unassignable
	JOB_INDUNGEON,			//
	JOB_RUNAWAY,			//

	NUM_JOBS,				//Number of Jobs
};// End JOBS enum
#if 0
// - Community Centre
const unsigned int JOBFILTER_COMMUNITYCENTRE = ;
const unsigned int JOB_COLLECTDONATIONS = ;	// not added yet	// collects money to help the poor
// - Drug Lab (these jobs gain bonus if in same building as a clinic)
const unsigned int JOBFILTER_DRUGLAB    = ;
const unsigned int JOB_VIRASPLANTFUCKER = ;	// not added yet	// the plants need to inseminate with other humanoids to proper-gate, creates vira blood items
const unsigned int JOB_SHROUDGROWER     = ;	// not added yet	// They require lots of care, and may explode. Produces shroud mushroom item.
const unsigned int JOB_FAIRYDUSTER      = ;	// not added yet	// captures and pounds faries to dust, produces fairy dust item
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
const unsigned int MISS_GUARDING        = 0;	// guarding your businesses
const unsigned int MISS_SABOTAGE        = 1;	// sabotaging rival business
const unsigned int MISS_SPYGIRLS        = 2;	// checking up on the girls while they work
const unsigned int MISS_CAPTUREGIRL     = 3;	// looking for runaway girls
const unsigned int MISS_EXTORTION       = 4;	// exthortion of local business for money in return for protection
const unsigned int MISS_PETYTHEFT       = 5;	// go out on the streets and steal from people
const unsigned int MISS_GRANDTHEFT      = 6;	// Go and rob local business while noone is there
const unsigned int MISS_KIDNAPP         = 7;	// go out and kidnap homeless or lost girls
const unsigned int MISS_CATACOMBS       = 8;	// men go down into the catacombs to find treasures
const unsigned int MISS_TRAINING        = 9;	// men improve their skills
const unsigned int MISS_RECRUIT         = 10;	// men recuit their number better

// Reasons for keeping them in the dungeon
const int DUNGEON_RELEASE           = 0;	// released from the dungeon on next update
const int DUNGEON_CUSTNOPAY         = 1;	// a customer that failed to pay or provide adiquate compensation
const int DUNGEON_GIRLCAPTURED      = 2;	// A new girl that was captured
const int DUNGEON_GIRLKIDNAPPED     = 3;	// A new girl taken against her will
const int DUNGEON_CUSTBEATGIRL      = 4;	// A customer that was found hurting a girl
const int DUNGEON_CUSTSPY           = 5;	// A customer that was found to be a spy for a rival
const int DUNGEON_RIVAL             = 6;	// a captured rival
const int DUNGEON_GIRLWHIM          = 7;	// a girl placed here on a whim
const int DUNGEON_GIRLSTEAL         = 8;	// a girl that was placed here after being found stealing extra
const int DUNGEON_DEAD              = 9;	// this person has died and will be removed next turn
const int DUNGEON_GIRLRUNAWAY       = 10;	// girl ran away but was recaptured
const int DUNGEON_NEWSLAVE          = 11;	// a newly brought slave
const int DUNGEON_NEWGIRL           = 12;	// a new girl who just joined you
const int DUNGEON_KID               = 13;	// a girl child just aged up
const int DUNGEON_NEWARENA          = 14;	// a new girl who just joined you from the arena
const int DUNGEON_RECRUITED         = 15;	// a new girl who was just recruited

// Town Constants
const int TOWN_NUMBUSINESSES        = 250;	// the amount of businesses in the town
const int TOWN_OFFICIALSWAGES       = 10;	// how much the authorities in the town are paid by the govenment

// Incomes
const int INCOME_BUSINESS           = 10;

// Item types
const int NUM_SHOPITEMS             = 40;	// number of items that the shop may hold at one time
const unsigned int INVRING          = 1;	// worn on fingers (max 8)
const unsigned int INVDRESS         = 2;	// Worn on body, (max 1)
const unsigned int INVSHOES         = 3;	// worn on feet, (max 1) often unequipped when going into combat
const unsigned int INVFOOD          = 4;	// Eaten, single use
const unsigned int INVNECKLACE      = 5;	// worn on neck, (max 1)
const unsigned int INVWEAPON        = 6;	// equipped on body, often unequipped outside of combat, (max 2)
const unsigned int INVMAKEUP        = 7;	// worn on face, single use
const unsigned int INVARMOR         = 8;	// worn on body over dresses, often unequipped outside of combat, (max 1)
const unsigned int INVMISC          = 9;    // these items don't usually do anythinig just random stuff girls might buy. The ones that do, cause a constant effect without having to be equiped
const unsigned int INVARMBAND       = 10;	// (max 2), worn around arms
const unsigned int INVSMWEAPON      = 11;   // small weapon which can be hidden on body, (max 2)
const unsigned int INVUNDERWEAR     = 12;   // CRAZY added this - underwear (max 1)
const unsigned int INVHAT			= 13;	// CRAZY added this - Noncombat worn on the head (max 1)
const unsigned int INVHELMET		= 14;	// CRAZY added this	- Combat worn on the head (max 1)
const unsigned int INVGLASSES		= 15;	// CRAZY added this	- Glasses (max 1)
const unsigned int INVSWIMSUIT		= 16;	// CRAZY added this - Swimsuit (max 1 in use but can have as many as they want)
const unsigned int INVCOMBATSHOES	= 17;	// `J`   added this - Combat Shoes (max 1) often unequipped outside of combat
const unsigned int INVSHIELD		= 18;	// `J`   added this - Shields (max 1) often unequipped outside of combat
//const unsigned int INVLEGS = ;		//CRAZY added this

// Item Rarity
const int NUM_ITEM_RARITY = 9;	// number of items that the shop may hold at one time
const unsigned int RARITYCOMMON			= 0;	// 
const unsigned int RARITYSHOP50			= 1;	// 
const unsigned int RARITYSHOP25			= 2;	// 
const unsigned int RARITYSHOP05			= 3;	// 
const unsigned int RARITYCATACOMB15		= 4;	// 
const unsigned int RARITYCATACOMB05		= 5;	// 
const unsigned int RARITYCATACOMB01		= 6;	// 
const unsigned int RARITYSCRIPTONLY		= 7;	// 
const unsigned int RARITYSCRIPTORREWARD = 8;	// 

// Random objectives (well kinda random, they appear to guide the player for the win)
const int NUM_OBJECTIVES = 10;							  // was 12 probably from when had sep Bar & Casino 
const unsigned int OBJECTIVE_REACHGOLDTARGET        = 0;
const unsigned int OBJECTIVE_GETNEXTBROTHEL         = 1;
const unsigned int OBJECTIVE_LAUNCHSUCCESSFULATTACK = 2;
const unsigned int OBJECTIVE_HAVEXGOONS             = 3;
const unsigned int OBJECTIVE_STEALXAMOUNTOFGOLD     = 4;
const unsigned int OBJECTIVE_CAPTUREXCATACOMBGIRLS  = 5;
const unsigned int OBJECTIVE_HAVEXMONSTERGIRLS      = 6;
const unsigned int OBJECTIVE_KIDNAPXGIRLS           = 7;
const unsigned int OBJECTIVE_EXTORTXNEWBUSINESS     = 8;
const unsigned int OBJECTIVE_HAVEXAMOUNTOFGIRLS     = 9;

// Objective reward types
const int          NUM_REWARDS          = 4;
const unsigned int REWARD_GOLD          = 0;
const unsigned int REWARD_GIRLS         = 1;
const unsigned int REWARD_RIVALHINDER   = 2;
const unsigned int REWARD_ITEM          = 3;

// customer fetishes (important that they are ordered from normal to weird)
const unsigned int  NUM_FETISH              = 19;
const unsigned char FETISH_TRYANYTHING      = 0;	// will like and try any form of sex (a nympho basically)
const unsigned char FETISH_SPECIFICGIRL     = 1;	// wants a particular girl
const unsigned char FETISH_BIGBOOBS         = 2;	// likes girls with big boobs
const unsigned char FETISH_SEXY             = 3;	// likes girls with lots of sex appeal
const unsigned char FETISH_CUTEGIRLS        = 4;	// Likes girls that are cute
const unsigned char FETISH_FIGURE           = 5;	// likes girls with good figures
const unsigned char FETISH_LOLITA           = 6;	// likes lolitas
const unsigned char FETISH_ARSE             = 7;	// likes girls with good arses
const unsigned char FETISH_COOLGIRLS        = 8;	// likes cool girls, may chat with them a little
const unsigned char FETISH_ELEGANT          = 9;	// likes girls with class
const unsigned char FETISH_NERDYGIRLS       = 10;	// likes nerds or clumsy girls
const unsigned char FETISH_SMALLBOOBS       = 11;	// likes girls with small boobs
const unsigned char FETISH_DANGEROUSGIRLS   = 12;	// likes girls with a bit of danger
const unsigned char FETISH_NONHUMAN         = 13;	// likes non human girls
const unsigned char FETISH_FREAKYGIRLS      = 14;	// likes freaky girls
const unsigned char FETISH_FUTAGIRLS		= 15;	// likes girls with dicks
const unsigned char FETISH_TALLGIRLS        = 16;	// likes tall girls
const unsigned char FETISH_SHORTGIRLS		= 17;	// likes short girls
const unsigned char FETISH_FATGIRLS			= 18;	// likes fat girls

// actions girls can disobey against
// `J` When modifying Action types, search for "J-Change-Action-Types"  :  found in >> Constants.h
const unsigned char ACTION_COMBAT           = 0;
const unsigned char ACTION_SEX              = 1;
const unsigned char ACTION_WORKESCORT		= 2;
const unsigned char ACTION_WORKCLEANING     = 3;
const unsigned char ACTION_WORKMATRON       = 4;
const unsigned char ACTION_WORKBAR          = 5;
const unsigned char ACTION_WORKHALL         = 6;
const unsigned char ACTION_WORKSHOW         = 7;
const unsigned char ACTION_WORKSECURITY     = 8;
const unsigned char ACTION_WORKADVERTISING  = 9;
const unsigned char ACTION_WORKTORTURER     = 10;
const unsigned char ACTION_WORKCARING       = 11;
const unsigned char ACTION_WORKDOCTOR       = 12;
const unsigned char ACTION_WORKMOVIE        = 13;
const unsigned char ACTION_WORKCUSTSERV     = 14;
const unsigned char ACTION_WORKCENTRE       = 15;
const unsigned char ACTION_WORKCLUB         = 16;
const unsigned char ACTION_WORKHAREM        = 17;
const unsigned char ACTION_WORKRECRUIT      = 18;
const unsigned char ACTION_WORKNURSE        = 19;
const unsigned char ACTION_WORKMECHANIC     = 20;
const unsigned char ACTION_WORKCOUNSELOR    = 21;
const unsigned char ACTION_WORKMUSIC		= 22;
const unsigned char ACTION_WORKSTRIP		= 23;
const unsigned char ACTION_WORKMILK 		= 24;
const unsigned char ACTION_WORKMASSUSSE		= 25;
const unsigned char ACTION_WORKFARM			= 26;
const unsigned char ACTION_WORKTRAINING		= 27;
const unsigned char ACTION_WORKREHAB		= 28;
const unsigned char ACTION_WORKMAKEPOTIONS	= 29;
const unsigned char ACTION_WORKMAKEITEMS	= 30;
const unsigned char ACTION_WORKCOOKING		= 31;
const unsigned char ACTION_WORKTHERAPY		= 32;
const unsigned char ACTION_GENERAL			= 33; // ACTION_GENERAL must be the last sction type

const unsigned char NUM_ACTIONTYPES = ACTION_GENERAL + 1;

// Character image
// `J` When modifying Image types, search for "J-Change-Image-Types"  :  found in >> Constants.h
const int IMGTYPE_ANAL      = 0;
const int IMGTYPE_BDSM      = 1;
const int IMGTYPE_SEX       = 2;
const int IMGTYPE_BEAST     = 3;
const int IMGTYPE_GROUP     = 4;
const int IMGTYPE_LESBIAN   = 5;
const int IMGTYPE_TORTURE	= 6;	// `J` added
const int IMGTYPE_DEATH		= 7;
const int IMGTYPE_PROFILE   = 8;
const int IMGTYPE_COMBAT    = 9;
const int IMGTYPE_ORAL		= 10;
const int IMGTYPE_ECCHI     = 11;
const int IMGTYPE_STRIP     = 12;
const int IMGTYPE_MAID      = 13;
const int IMGTYPE_SING      = 14;
const int IMGTYPE_WAIT      = 15;
const int IMGTYPE_CARD      = 16;
const int IMGTYPE_BUNNY     = 17;
const int IMGTYPE_NUDE      = 18;
const int IMGTYPE_MAST      = 19;
const int IMGTYPE_TITTY     = 20;
const int IMGTYPE_MILK      = 21;
const int IMGTYPE_HAND      = 22;
const int IMGTYPE_FOOT		= 23;
const int IMGTYPE_BED		= 24;
const int IMGTYPE_FARM		= 25;
const int IMGTYPE_HERD		= 26;
const int IMGTYPE_COOK		= 27;
const int IMGTYPE_CRAFT		= 28;
const int IMGTYPE_SWIM		= 29;
const int IMGTYPE_BATH		= 30;
const int IMGTYPE_NURSE		= 31;
const int IMGTYPE_FORMAL	= 32;
const int IMGTYPE_SHOP		= 33;
const int IMGTYPE_MAGIC		= 34;
const int IMGTYPE_SIGN		= 35;	// Going be used for advertising
const int IMGTYPE_PRESENTED = 36;	// Going be used for Slave Market
const int IMGTYPE_DOM		= 37;	// 
const int IMGTYPE_PREGNANT	= 38;	// IMGTYPE_PREGNANT needs to be the last of the nonpregnant image types.

/*
*	`J` All image types can have a pregnant alternative now
*	Then we can convert by adding PREG_OFFSET
*	The order and exact number of the pregnant types does not really 
*	matter as they are based off the nonpregnant types.
*/
const int PREG_OFFSET = IMGTYPE_PREGNANT + 1;
const int IMGTYPE_PREGANAL	    = IMGTYPE_ANAL			+ PREG_OFFSET;
const int IMGTYPE_PREGBDSM	    = IMGTYPE_BDSM			+ PREG_OFFSET;
const int IMGTYPE_PREGSEX	    = IMGTYPE_SEX			+ PREG_OFFSET;
const int IMGTYPE_PREGBEAST	    = IMGTYPE_BEAST			+ PREG_OFFSET;
const int IMGTYPE_PREGGROUP	    = IMGTYPE_GROUP			+ PREG_OFFSET;
const int IMGTYPE_PREGLESBIAN	= IMGTYPE_LESBIAN		+ PREG_OFFSET;
const int IMGTYPE_PREGTORTURE	= IMGTYPE_TORTURE		+ PREG_OFFSET;
const int IMGTYPE_PREGDEATH		= IMGTYPE_DEATH			+ PREG_OFFSET;
const int IMGTYPE_PREGPROFILE	= IMGTYPE_PROFILE		+ PREG_OFFSET; 
const int IMGTYPE_PREGCOMBAT	= IMGTYPE_COMBAT		+ PREG_OFFSET;
const int IMGTYPE_PREGORAL		= IMGTYPE_ORAL			+ PREG_OFFSET; 
const int IMGTYPE_PREGECCHI		= IMGTYPE_ECCHI			+ PREG_OFFSET;
const int IMGTYPE_PREGSTRIP		= IMGTYPE_STRIP			+ PREG_OFFSET;
const int IMGTYPE_PREGMAID		= IMGTYPE_MAID			+ PREG_OFFSET;
const int IMGTYPE_PREGSING		= IMGTYPE_SING			+ PREG_OFFSET;
const int IMGTYPE_PREGWAIT		= IMGTYPE_WAIT			+ PREG_OFFSET;
const int IMGTYPE_PREGCARD		= IMGTYPE_CARD			+ PREG_OFFSET;
const int IMGTYPE_PREGBUNNY		= IMGTYPE_BUNNY			+ PREG_OFFSET;
const int IMGTYPE_PREGNUDE		= IMGTYPE_NUDE			+ PREG_OFFSET;
const int IMGTYPE_PREGMAST		= IMGTYPE_MAST			+ PREG_OFFSET;
const int IMGTYPE_PREGTITTY		= IMGTYPE_TITTY			+ PREG_OFFSET;
const int IMGTYPE_PREGMILK		= IMGTYPE_MILK			+ PREG_OFFSET;
const int IMGTYPE_PREGHAND		= IMGTYPE_HAND			+ PREG_OFFSET;
const int IMGTYPE_PREGFOOT		= IMGTYPE_FOOT			+ PREG_OFFSET;
const int IMGTYPE_PREGBED		= IMGTYPE_BED			+ PREG_OFFSET;
const int IMGTYPE_PREGFARM		= IMGTYPE_FARM			+ PREG_OFFSET;
const int IMGTYPE_PREGHERD		= IMGTYPE_HERD			+ PREG_OFFSET;
const int IMGTYPE_PREGCOOK		= IMGTYPE_COOK			+ PREG_OFFSET;
const int IMGTYPE_PREGCRAFT		= IMGTYPE_CRAFT			+ PREG_OFFSET;
const int IMGTYPE_PREGSWIM		= IMGTYPE_SWIM			+ PREG_OFFSET;
const int IMGTYPE_PREGBATH		= IMGTYPE_BATH			+ PREG_OFFSET;
const int IMGTYPE_PREGNURSE		= IMGTYPE_NURSE			+ PREG_OFFSET;
const int IMGTYPE_PREGFORMAL	= IMGTYPE_FORMAL		+ PREG_OFFSET;
const int IMGTYPE_PREGSHOP		= IMGTYPE_SHOP			+ PREG_OFFSET;
const int IMGTYPE_PREGMAGIC		= IMGTYPE_MAGIC			+ PREG_OFFSET;
const int IMGTYPE_PREGSIGN		= IMGTYPE_SIGN			+ PREG_OFFSET;
const int IMGTYPE_PREGPRESENTED = IMGTYPE_PRESENTED		+ PREG_OFFSET;
const int IMGTYPE_PREGDOM		= IMGTYPE_DOM			+ PREG_OFFSET;
const int NUM_IMGTYPES			= IMGTYPE_PREGNANT		+ PREG_OFFSET; // `J` All image types can have a pregnant variation (except pregnant-pregnant)



const int NUM_GIRLFLAGS				= 30;
const int MAXNUM_TRAITS				= 60;		// Maximum number of traits a girl can have
const int MAXNUM_INVENTORY			= 3000;		// Maximum number of items in inventory
const int MAXNUM_GIRL_INVENTORY		= 40;		// Maximum number of items a Girl can have in inventory
const int MAXNUM_RIVAL_INVENTORY	= 40;		// Maximum number of items a Rival can have in inventory

const int LISTBOX_ITEMHEIGHT = 20;		//how tall (in pixels) each list item is
const unsigned int LISTBOX_COLUMNS = 15;		//how many columns are allowed

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
const int SCREEN_PROPERTYMANAGEMENT = 27; // `J` added for managing all properties on 1 page
const int SCREEN_FARM				= 28;
const int SCREEN_NEWGAME			= 29;
const int SCREEN_PREPARING			= 29;


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
