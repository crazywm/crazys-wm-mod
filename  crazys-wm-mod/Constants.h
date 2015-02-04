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

// Editable Character Stats and skills (used for traits)
const unsigned int STAT_CHARISMA		= 0;
const unsigned int STAT_HAPPINESS		= 1;
const unsigned int STAT_LIBIDO			= 2;
const unsigned int STAT_CONSTITUTION	= 3;
const unsigned int STAT_INTELLIGENCE	= 4;
const unsigned int STAT_CONFIDENCE		= 5;
const unsigned int STAT_MANA			= 6;
const unsigned int STAT_AGILITY			= 7;
const unsigned int STAT_FAME			= 8;
const unsigned int STAT_LEVEL			= 9;
const unsigned int STAT_ASKPRICE		= 10;
const unsigned int STAT_HOUSE			= 11;
const unsigned int STAT_EXP				= 12;
const unsigned int STAT_AGE				= 13;
const unsigned int STAT_OBEDIENCE		= 14;
const unsigned int STAT_SPIRIT			= 15;
const unsigned int STAT_BEAUTY			= 16;
const unsigned int STAT_TIREDNESS		= 17;
const unsigned int STAT_HEALTH			= 18;
const unsigned int STAT_PCFEAR			= 19;
const unsigned int STAT_PCLOVE			= 20;
const unsigned int STAT_PCHATE			= 21;
const unsigned int STAT_MORALITY		= 22;
const unsigned int STAT_REFINEMENT		= 23;
const unsigned int STAT_DIGNITY			= 24;
const unsigned int STAT_LACTATION		= 25;
const int					NUM_STATS	= 26;	// 1 more than the last stat

// `J` When modifying Stats or Skills, search for "J-Change-Stats-Skills"  :  found in >> Constants.h

// Skills
const unsigned int NUM_SKILLS			= 21;
const unsigned int SKILL_ANAL			= 0;
const unsigned int SKILL_MAGIC			= 1;
const unsigned int SKILL_BDSM			= 2;
const unsigned int SKILL_NORMALSEX		= 3;
const unsigned int SKILL_BEASTIALITY	= 4;
const unsigned int SKILL_GROUP			= 5;
const unsigned int SKILL_LESBIAN		= 6;
const unsigned int SKILL_SERVICE		= 7;
const unsigned int SKILL_STRIP			= 8;
const unsigned int SKILL_COMBAT			= 9;
const unsigned int SKILL_ORALSEX		= 10;
const unsigned int SKILL_TITTYSEX		= 11;
const unsigned int SKILL_MEDICINE		= 12;
const unsigned int SKILL_PERFORMANCE	= 13;
const unsigned int SKILL_HANDJOB		= 14;
const unsigned int SKILL_CRAFTING		= 15;
const unsigned int SKILL_HERBALISM		= 16;
const unsigned int SKILL_FARMING		= 17;
const unsigned int SKILL_BREWING		= 18;
const unsigned int SKILL_ANIMALHANDLING	= 19;
const unsigned int SKILL_FOOTJOB		= 20;
//const unsigned int SKILL_COOKING		= ;
//const unsigned int SKILL_MAST		= ;

// STATUS
const unsigned int NUM_STATUS					= 14;
const unsigned int STATUS_NONE					= 0;
const unsigned int STATUS_POISONED				= 1;
const unsigned int STATUS_BADLY_POISONED		= 2;
const unsigned int STATUS_PREGNANT				= 3;
const unsigned int STATUS_PREGNANT_BY_PLAYER	= 4;
const unsigned int STATUS_SLAVE					= 5;
const unsigned int STATUS_HAS_DAUGHTER			= 6;
const unsigned int STATUS_HAS_SON				= 7;
const unsigned int STATUS_INSEMINATED			= 8;
const unsigned int STATUS_CONTROLLED			= 9;
const unsigned int STATUS_CATACOMBS				= 10;
const unsigned int STATUS_ARENA					= 11;
const unsigned int STATUS_YOURDAUGHTER			= 12;
const unsigned int STATUS_ISDAUGHTER			= 13;

// Jobs
const unsigned int NUMJOBTYPES			= 18;
const unsigned int NUM_JOBS				= 111;
// `J` Brothel Job - General
const unsigned int JOBFILTER_GENERAL	= 0;
const unsigned int JOB_RESTING			= 0;	// relaxes and takes some time off
const unsigned int JOB_TRAINING			= 1;	// trains skills at a basic level
const unsigned int JOB_CLEANING			= 2;	// cleans the building
const unsigned int JOB_SECURITY			= 3;	// Protects the building and its occupants
const unsigned int JOB_ADVERTISING		= 4;	// Goes onto the streets to advertise the buildings services
const unsigned int JOB_CUSTOMERSERVICE	= 5;	// looks after customers needs (customers are happier when people are doing this job)
const unsigned int JOB_MATRON			= 6;	// looks after the needs of the girls (only 1 allowed)
const unsigned int JOB_TORTURER			= 7;	// Tortures the people in the dungeons to help break their will (this is in addition to player torture) (only 1 allowed)
const unsigned int JOB_EXPLORECATACOMBS	= 8;	// goes adventuring in the catacombs
const unsigned int JOB_BEASTCARER		= 9;	// takes care of beasts that are housed in the brothel.
// `J` Brothel Job - Bar
const unsigned int JOBFILTER_BAR		= 1;
const unsigned int JOB_BARMAID			= 10;	// serves at the bar
const unsigned int JOB_WAITRESS			= 11;	// waits on the tables
const unsigned int JOB_SINGER			= 12;	// sings in the bar
const unsigned int JOB_PIANO			= 13;	// plays the piano for customers
const unsigned int JOB_ESCORT			= 14;	// High lvl whore.  Sees less clients but needs higher skill high lvl items and the such to make them happy
// `J` Brothel Job - Hall
const unsigned int JOBFILTER_GAMBHALL	= 2;
const unsigned int JOB_DEALER			= 15;	// dealer for gambling tables (requires at least 3 for all jobs in this filter to work)
const unsigned int JOB_ENTERTAINMENT	= 16;	// sings, dances and other shows for patrons
const unsigned int JOB_XXXENTERTAINMENT	= 17;	// naughty shows for patrons
const unsigned int JOB_WHOREGAMBHALL	= 18;	// looks after customers sexual needs
// `J` Brothel Job - Sleazy Bar
const unsigned int JOBFILTER_SLEAZYBAR	= 3;
const unsigned int JOB_SLEAZYBARMAID	= 19;
const unsigned int JOB_SLEAZYWAITRESS	= 20;
const unsigned int JOB_BARSTRIPPER		= 21;
const unsigned int JOB_BARWHORE			= 22;
// `J` Brothel Job - Brothel
const unsigned int JOBFILTER_BROTHEL	= 4;
const unsigned int JOB_MASSEUSE			= 23;	// gives massages to patrons and sometimes sex
const unsigned int JOB_BROTHELSTRIPPER	= 24;	// strips for customers and sometimes sex
const unsigned int JOB_PEEP				= 25;	// Peep show
const unsigned int JOB_WHOREBROTHEL		= 26;	// whore herself inside the building
const unsigned int JOB_WHORESTREETS		= 27;	// whore self on the city streets

// `J` Movie Studio Job - Actress
const unsigned int JOBFILTER_MOVIESTUDIO = 5;
const unsigned int JOB_FILMBEAST		= 28;	// films this sort of scene in the movie (uses beast resource)
const unsigned int JOB_FILMSEX			= 29;	// films this sort of scene in the movie
const unsigned int JOB_FILMANAL			= 30;	// films this sort of scene in the movie
const unsigned int JOB_FILMLESBIAN		= 31;	// films this sort of scene in the movie. thinking about changing to Lesbian
const unsigned int JOB_FILMBONDAGE		= 32;	// films this sort of scene in the movie
const unsigned int JOB_FILMGROUP		= 33;	// films this sort of scene in the movie
const unsigned int JOB_FILMORAL			= 34;	// films this type of scene CRAZY
const unsigned int JOB_FILMMAST			= 35;	// films this type of scene CRAZY
const unsigned int JOB_FILMTITTY		= 36;	// films this type of scene CRAZY
const unsigned int JOB_FILMSTRIP		= 37;	// films this type of scene CRAZY
const unsigned int JOB_FILMHANDJOB		= 38;	// films this type of scene CRAZY
const unsigned int JOB_FILMFOOTJOB		= 39;	// films this type of scene CRAZY
const unsigned int JOB_FILMRANDOM		= 40;	// Films a random sex scene ... it does NOT work like most jobs, see following note.
		// *****IMPORTANT**** If you add more scene types, they must go somewhere between
		// JOB_FILMBEAST and JOB_FILMRANDOM, or it will cause the random job to stop working..
		// JOB_FILMBEAST must be the first film job, JOB_FILMRANDOM must be the last one. --PP
// `J` Movie Studio Job - Crew
const unsigned int JOBFILTER_STUDIOCREW	= 6;
const unsigned int JOB_FILMFREETIME		= 41;	// Free time
const unsigned int JOB_DIRECTOR			= 42;	// Does same work as matron plus adds quality to films.
const unsigned int JOB_PROMOTER			= 43;	// Advertising -- This helps film sales after it is created.
const unsigned int JOB_CAMERAMAGE		= 44;	// Uses magic to record the scenes to crystals (requires at least 1)
const unsigned int JOB_CRYSTALPURIFIER	= 45;	// Post editing to get the best out of the film (requires at least 1)
const unsigned int JOB_FLUFFER			= 46;	// Keeps the porn stars and animals aroused
const unsigned int JOB_STAGEHAND		= 47;	// Currently does the same as a cleaner.

// `J` Arena Job - Fighting
const unsigned int JOBFILTER_ARENA		= 7;
const unsigned int JOB_FIGHTBEASTS		= 48;	// customers come to place bets on who will win, girl may die (uses beasts resource)
const unsigned int JOB_FIGHTARENAGIRLS	= 49;	
const unsigned int JOB_FIGHTTRAIN		= 50;
//const unsigned int JOB_JOUSTING		= ;
//const unsigned int JOB_MAGICDUEL		= ;
//const unsigned int JOB_ARMSDUEL		= ;
//const unsigned int JOB_FIGHTBATTLE	= ;
//const unsigned int JOB_ATHELETE		= ;
//const unsigned int JOB_RACING			= ;
// `J` Arena Job - Staff
const unsigned int JOBFILTER_ARENASTAFF	= 8;
const unsigned int JOB_ARENAREST		= 51;	//free time of arena
const unsigned int JOB_DOCTORE			= 52;	//Matron of arena
const unsigned int JOB_CITYGUARD		= 53;	
const unsigned int JOB_CLEANARENA		= 54;
//const unsigned int JOB_BATTLEMASTER	= ;
//const unsigned int JOB_ARENAPROMOTER	= ;
//const unsigned int JOB_BEASTMASTER	= ;
//const unsigned int JOB_VENDOR			= ;
//const unsigned int JOB_BOOKIE			= ;
//const unsigned int JOB_GROUNDSKEEPER	= ;
//const unsigned int JOB_MINER			= ;
//const unsigned int JOB_SMITH			= ;

// `J` Centre Job - General
const unsigned int JOBFILTER_COMMUNITYCENTRE = 9;
const unsigned int JOB_CENTREREST		= 55;	// centres free time
const unsigned int JOB_CENTREMANAGER	= 56;	// matron of centre	
const unsigned int JOB_FEEDPOOR			= 57;	// work in a soup kitchen
const unsigned int JOB_COMUNITYSERVICE	= 58;	// Goes around town helping where they can
const unsigned int JOB_CLEANCENTRE		= 59;	// 	
// TODO ideas:Run a charity, with an option for the player to steal from charity (with possible bad outcome). Run schools/orphanages.. this should give a boost to the stats of new random girls, and possibly be a place to recruit new uniques.
// Homeless shelter... once again a possible place to find new girls.
// `J` Centre Job - Rehab
const unsigned int JOBFILTER_DRUGCENTRE	= 10;
const unsigned int JOB_DRUGCOUNSELOR	= 60;	// 	
const unsigned int JOB_REHAB			= 61;	// 	
const unsigned int JOB_ANGER			= 62;	// 	
const unsigned int JOB_EXTHERAPY		= 63;	// 	
const unsigned int JOB_THERAPY			= 64;	// 		

// `J` Clinic Job - Surgery
const unsigned int JOBFILTER_CLINIC		= 11;	// teaches can train girls only up to their own skill level
const unsigned int JOB_GETHEALING		= 65;	// takes 1 days for each wound trait received.
const unsigned int JOB_GETREPAIRS		= 66;	// construct girls can get repaired quickly
const unsigned int JOB_GETABORT			= 67;	// gets an abortion (takes 2 days)
const unsigned int JOB_PHYSICALSURGERY	= 68;	// magical plastic surgery (takes 5 days)
const unsigned int JOB_LIPO				= 69;	// magical plastic surgery (takes 5 days)
const unsigned int JOB_BREASTREDUCTION	= 70;	// magical plastic surgery (takes 5 days)
const unsigned int JOB_BOOBJOB			= 71;	// magical plastic surgery (takes 5 days)
const unsigned int JOB_VAGINAREJUV		= 72;	// magical plastic surgery (takes 5 days)
const unsigned int JOB_FACELIFT			= 73;	// magical plastic surgery (takes 5 days)
const unsigned int JOB_ASSJOB			= 74;	// magical plastic surgery (takes 5 days)
const unsigned int JOB_TUBESTIED		= 75;	// magical plastic surgery (takes 5 days)
const unsigned int JOB_FERTILITY		= 76;	// magical plastic surgery (takes 5 days)

// `J` Clinic Job - Staff
const unsigned int JOBFILTER_CLINICSTAFF = 12;
const unsigned int JOB_CLINICREST		= 77;	// Is clinics free time	
const unsigned int JOB_CHAIRMAN			= 78;	// Matron of clinic	
const unsigned int JOB_DOCTOR			= 79;	// becomes a doctor (requires 1) (will make some extra cash for treating locals)
const unsigned int JOB_NURSE			= 80;	// helps girls recover from surgery on healing
const unsigned int JOB_MECHANIC			= 81;	// construct girls can get repaired quickly
const unsigned int JOB_INTERN			= 82;	// training for nurse job
const unsigned int JOB_JANITOR			= 83;	// cleans clinic	

// `J` Farm Job - Staff
const unsigned int JOBFILTER_FARMSTAFF	= 13;
const unsigned int JOB_FARMREST			= 84;  //farm rest
const unsigned int JOB_FARMMANGER		= 85;  //matron of farm
const unsigned int JOB_VETERINARIAN		= 86;  //tends to the animals to keep them from dying - full time
const unsigned int JOB_MARKETER			= 87;  // buys and sells food - full time
const unsigned int JOB_RESEARCH			= 88;  // potions - unlock various types of potions and garden qualities - full time
const unsigned int JOB_FARMHAND			= 89;  //cleaning of the farm
// `J` Farm Job - Laborers
const unsigned int JOBFILTER_LABORERS	= 14;
const unsigned int JOB_FARMER			= 90;	//tends crops
const unsigned int JOB_GARDENER			= 91;	// produces herbs and potion ingredients
const unsigned int JOB_SHEAPHERD		= 92;	//tends food animals - 100% food
const unsigned int JOB_RANCHER			= 93;	// tends animals for food or beast - % food/beast based on skills
const unsigned int JOB_CATACOMBRANCHER	= 94;	//tends strange beasts - 100% beast - dangerous
const unsigned int JOB_BEASTCAPTURE		= 95;	//
const unsigned int JOB_MILKER			= 96;	// produces milk from animals/beasts/girls - if food animals < beasts - can be dangerous
const unsigned int JOB_MILK				= 97;	//milker not required but increases yield
// `J` Farm Job - Producers
const unsigned int JOBFILTER_PRODUCERS	= 15;
const unsigned int JOB_BUTCHER			= 98;   // produces food from animals
const unsigned int JOB_BAKER			= 99;   // produces food from crops
const unsigned int JOB_BREWER			= 100;  // Produces beers and wines
const unsigned int JOB_MAKEITEM			= 101;  // produce items for sale
const unsigned int JOB_MAKEPOTIONS		= 102;  // create potions with items gained from the garden

// `J` House Job - General
const unsigned int JOBFILTER_HOUSE		= 16;
const unsigned int JOB_HOUSEREST		= 103;
const unsigned int JOB_HEADGIRL			= 104;
const unsigned int JOB_RECRUITER		= 105;
const unsigned int JOB_PERSONALTRAINING	= 106;
const unsigned int JOB_PERSONALBEDWARMER= 107;
const unsigned int JOB_CLEANHOUSE		= 108;
//const unsigned int JOB_HOUSECOOK		= ;    // cooks for the harem, (helps keep them happy, and increase job performance)
//const unsigned int JOB_HOUSEDATE		= ;
//const unsigned int JOB_HOUSEVAC		= ;

// - extra unassignable
const unsigned int JOBFILTER_NONE		= 17;
const unsigned int JOB_INDUNGEON        = 109;
const unsigned int JOB_RUNAWAY          = 110;

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
const unsigned int  NUM_FETISH              = 15;
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
const unsigned char FETISH_FREAKYGIRLS      = 14;	// likes girls with small boobs

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
const unsigned char ACTION_WORKINTERN		= 27;
const unsigned char ACTION_WORKREHAB		= 28;
const unsigned char ACTION_WORKMAKEPOTIONS	= 29;
const unsigned char ACTION_WORKMAKEITEMS	= 30;
const unsigned char ACTION_WORKCOOKING		= 31;
const unsigned char ACTION_GENERAL			= 32; // ACTION_GENERAL must be the last sction type

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
const int IMGTYPE_PREGNANT	= 35;	// IMGTYPE_PREGNANT needs to be the last of the nonpregnant image types.

/*
*	`J` All image types can have a pregnant alternative now
*	Then we can convert by adding PREG_OFFSET
*	The order and exact number of the pregnant types does not really 
*	matter as they are based off the nonpregnant types.
*/
const int PREG_OFFSET = IMGTYPE_PREGNANT + 1;
const int IMGTYPE_PREGANAL	    = IMGTYPE_ANAL		+ PREG_OFFSET;
const int IMGTYPE_PREGBDSM	    = IMGTYPE_BDSM		+ PREG_OFFSET;
const int IMGTYPE_PREGSEX	    = IMGTYPE_SEX		+ PREG_OFFSET;
const int IMGTYPE_PREGBEAST	    = IMGTYPE_BEAST		+ PREG_OFFSET;
const int IMGTYPE_PREGGROUP	    = IMGTYPE_GROUP		+ PREG_OFFSET;
const int IMGTYPE_PREGLESBIAN	= IMGTYPE_LESBIAN	+ PREG_OFFSET;
const int IMGTYPE_PREGTORTURE	= IMGTYPE_TORTURE	+ PREG_OFFSET;
const int IMGTYPE_PREGDEATH		= IMGTYPE_DEATH		+ PREG_OFFSET;
const int IMGTYPE_PREGPROFILE	= IMGTYPE_PROFILE   + PREG_OFFSET; 
const int IMGTYPE_PREGCOMBAT	= IMGTYPE_COMBAT    + PREG_OFFSET;
const int IMGTYPE_PREGORAL		= IMGTYPE_ORAL		+ PREG_OFFSET; 
const int IMGTYPE_PREGECCHI		= IMGTYPE_ECCHI     + PREG_OFFSET;
const int IMGTYPE_PREGSTRIP		= IMGTYPE_STRIP     + PREG_OFFSET;
const int IMGTYPE_PREGMAID		= IMGTYPE_MAID      + PREG_OFFSET;
const int IMGTYPE_PREGSING		= IMGTYPE_SING      + PREG_OFFSET;
const int IMGTYPE_PREGWAIT		= IMGTYPE_WAIT      + PREG_OFFSET;
const int IMGTYPE_PREGCARD		= IMGTYPE_CARD      + PREG_OFFSET;
const int IMGTYPE_PREGBUNNY		= IMGTYPE_BUNNY     + PREG_OFFSET;
const int IMGTYPE_PREGNUDE		= IMGTYPE_NUDE      + PREG_OFFSET;
const int IMGTYPE_PREGMAST		= IMGTYPE_MAST      + PREG_OFFSET;
const int IMGTYPE_PREGTITTY		= IMGTYPE_TITTY     + PREG_OFFSET;
const int IMGTYPE_PREGMILK		= IMGTYPE_MILK      + PREG_OFFSET;
const int IMGTYPE_PREGHAND		= IMGTYPE_HAND      + PREG_OFFSET;
const int IMGTYPE_PREGFOOT		= IMGTYPE_FOOT		+ PREG_OFFSET;
const int IMGTYPE_PREGBED		= IMGTYPE_BED		+ PREG_OFFSET;
const int IMGTYPE_PREGFARM		= IMGTYPE_FARM		+ PREG_OFFSET;
const int IMGTYPE_PREGHERD		= IMGTYPE_HERD		+ PREG_OFFSET;
const int IMGTYPE_PREGCOOK		= IMGTYPE_COOK		+ PREG_OFFSET;
const int IMGTYPE_PREGCRAFT		= IMGTYPE_CRAFT		+ PREG_OFFSET;
const int IMGTYPE_PREGSWIM		= IMGTYPE_SWIM		+ PREG_OFFSET;
const int IMGTYPE_PREGBATH		= IMGTYPE_BATH		+ PREG_OFFSET;
const int IMGTYPE_PREGNURSE		= IMGTYPE_NURSE		+ PREG_OFFSET;
const int IMGTYPE_PREGFORMAL	= IMGTYPE_FORMAL	+ PREG_OFFSET;
const int IMGTYPE_PREGSHOP		= IMGTYPE_SHOP		+ PREG_OFFSET;
const int IMGTYPE_PREGMAGIC		= IMGTYPE_MAGIC		+ PREG_OFFSET;

const int NUM_IMGTYPES = IMGTYPE_PREGNANT + PREG_OFFSET; // `J` All image types can have a pregnant variation (except pregnant-pregnant)



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
#endif
