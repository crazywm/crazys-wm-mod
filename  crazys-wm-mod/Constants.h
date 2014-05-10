/*
 * Copyright 2009, 2010, The Pink Petal Development Team.
 * The Pink Petal Devloment Team are defined as the game's coders 
 * who meet on http://pinkpetal.co.cc
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
const int g_MinorVersionA   = 3;
const int g_MinorVersionB   = 0;
const int g_StableVersion   = 4;

enum {
	SHIFT_DAY	= 0,
	SHIFT_NIGHT	= 1
};

// Editable Character Stats and skills (used for traits)
const int          NUM_STATS            = 22;
const unsigned int STAT_CHARISMA        = 0;
const unsigned int STAT_HAPPINESS       = 1;
const unsigned int STAT_LIBIDO          = 2;
const unsigned int STAT_CONSTITUTION    = 3;
const unsigned int STAT_INTELLIGENCE    = 4;
const unsigned int STAT_CONFIDENCE      = 5;
const unsigned int STAT_MANA            = 6;
const unsigned int STAT_AGILITY         = 7;
const unsigned int STAT_FAME            = 8;
const unsigned int STAT_LEVEL           = 9;
const unsigned int STAT_ASKPRICE        = 10;
const unsigned int STAT_HOUSE           = 11;
const unsigned int STAT_EXP             = 12;
const unsigned int STAT_AGE             = 13;
const unsigned int STAT_OBEDIENCE       = 14;
const unsigned int STAT_SPIRIT          = 15;
const unsigned int STAT_BEAUTY          = 16;
const unsigned int STAT_TIREDNESS       = 17;
const unsigned int STAT_HEALTH          = 18;
const unsigned int STAT_PCFEAR          = 19;
const unsigned int STAT_PCLOVE          = 20;
const unsigned int STAT_PCHATE          = 21;

// Skills
const unsigned int NUM_SKILLS           = 12;
const unsigned int SKILL_ANAL           = 0;
const unsigned int SKILL_MAGIC          = 1;
const unsigned int SKILL_BDSM           = 2;
const unsigned int SKILL_NORMALSEX      = 3;
const unsigned int SKILL_BEASTIALITY    = 4;
const unsigned int SKILL_GROUP          = 5;
const unsigned int SKILL_LESBIAN        = 6;
const unsigned int SKILL_SERVICE        = 7;
const unsigned int SKILL_STRIP          = 8;
const unsigned int SKILL_COMBAT         = 9;
const unsigned int SKILL_ORALSEX		= 10;
const unsigned int SKILL_TITTYSEX		= 11;

// STATUS
const unsigned int NUM_STATUS					= 12;
const unsigned int STATUS_NONE					= 0;
const unsigned int STATUS_POISONED				= 1;
const unsigned int STATUS_BADLY_POISONED		= 2;
const unsigned int STATUS_PREGNANT				= 3;
const unsigned int STATUS_PREGNANT_BY_PLAYER	= 4;
const unsigned int STATUS_SLAVE					= 5;
const unsigned int STATUS_HAS_DAUGHTER          = 6;
const unsigned int STATUS_HAS_SON				= 7;
const unsigned int STATUS_INSEMINATED			= 8;
const unsigned int STATUS_CONTROLLED			= 9;
const unsigned int STATUS_CATACOMBS				= 10;
const unsigned int STATUS_ARENA				    = 11;

#if 0
// Girl states
enum
{
GIRL_POISONED       = 1,    // Her health will slowly decrease until better
GIRL_BPOISONED      = 2,    // Her health will quickly decrease until better
GIRL_PREGNANT       = 3,    // She is pregnant
GIRL_PREGPLAYERS    = 4,    // She is pregnant with the players child
GIRL_SLAVE          = 5,    // she is a slave and will complain and disobey less
GIRL_HASDAUGHTER    = 6,    // she has a daughter
GIRL_HASSON         = 7,    // she has a son
GIRL_INSEMINATED    = 8,    // like being pregnant except by a beast or creature
GIRL_CONTROLED      = 9,    // her obedience is always 100 while this is active, but goes straight to 0 should it be removed
GIRL_CATACOMBS      = 10    // means this girl can be found in the catacombs
};
#endif


// Jobs
const unsigned int NUMJOBTYPES          = 14;
const unsigned int NUM_JOBS             = 77;
// - General
const unsigned int JOBFILTER_GENERAL    = 0;
const unsigned int JOB_RESTING          = 0;    // relaxes and takes some time off
const unsigned int JOB_TRAINING         = 1;	// trains skills at a basic level
const unsigned int JOB_CLEANING         = 2;	// cleans the building
const unsigned int JOB_SECURITY         = 3;	// Protects the building and its occupants
const unsigned int JOB_ADVERTISING      = 4;    // Goes onto the streets to advertise the buildings services
const unsigned int JOB_CUSTOMERSERVICE  = 5;	// looks after customers needs (customers are happier when people are doing this job)
const unsigned int JOB_MATRON           = 6;	// looks after the needs of the girls (only 1 allowed)
const unsigned int JOB_TORTURER         = 7;	// Tortures the people in the dungeons to help break their will (this is in addition to player torture) (only 1 allowed)
const unsigned int JOB_EXPLORECATACOMBS = 8;	// goes adventuring in the catacombs
// - Bar
const unsigned int JOBFILTER_BAR        = 1;
const unsigned int JOB_BARMAID          = 9;		// serves at the bar (requires 1 at least)
const unsigned int JOB_WAITRESS         = 10;		// waits on the tables (requires 1 at least)
const unsigned int JOB_SINGER           = 11;		// sings in the bar (girl needs singer trait)
const unsigned int JOB_PIANO			= 12;
// - Gambling Hall
const unsigned int JOBFILTER_GAMBHALL   = 2;
const unsigned int JOB_DEALER           = 13;		// dealer for gambling tables (requires at least 3 for all jobs in this filter to work)
const unsigned int JOB_ENTERTAINMENT    = 14;		// sings, dances and other shows for patrons
const unsigned int JOB_XXXENTERTAINMENT = 15;		// naughty shows for patrons
const unsigned int JOB_WHOREGAMBHALL    = 16;		// looks after customers sexual needs
// - Sleazy Bar
const unsigned int JOBFILTER_SLEAZYBAR	= 3;
const unsigned int JOB_SLEAZYBARMAID	= 17;
const unsigned int JOB_SLEAZYWAITRESS	= 18;
const unsigned int JOB_BARSTRIPPER		= 19;
const unsigned int JOB_BARWHORE			= 20;
// - Brothel
const unsigned int JOBFILTER_BROTHEL    = 4;
const unsigned int JOB_MASSEUSE         = 21;	// gives massages to patrons and sometimes sex
const unsigned int JOB_BROTHELSTRIPPER  = 22;	// strips for customers and sometimes sex
const unsigned int JOB_WHOREBROTHEL     = 23;	// whore herself inside the building
const unsigned int JOB_WHORESTREETS     = 24;	// whore self on the city streets
const unsigned int JOB_PEEP             = 25;	// Peep show
// - Stables
const unsigned int JOBFILTER_STABLES	= 5;
const unsigned int JOB_BEASTCAPTURE		= 26;
const unsigned int JOB_BEASTCARER		= 27;
const unsigned int JOB_MILK		        = 28;
// - Arena (these jobs gain bonus if in same building as a clinic)
const unsigned int JOBFILTER_ARENA      = 6;
const unsigned int JOB_FIGHTBEASTS      = 29;	// not added yet	// customers come to place bets on who will win, girl may die (uses beasts resource)
const unsigned int JOB_FIGHTARENAGIRLS  = 30;
const unsigned int JOB_FIGHTTRAIN       = 31;	
const unsigned int JOB_DOCTORE          = 32;
const unsigned int JOB_CLEANARENA       = 33;
const unsigned int JOB_ARENAREST        = 34;
const unsigned int JOB_CITYGUARD        = 35;
// - Movie Crystal Studio
const unsigned int JOBFILTER_MOVIESTUDIO = 7;
const unsigned int JOB_FILMBEAST        = 36;		// films this sort of scene in the movie (uses beast resource)
const unsigned int JOB_FILMSEX          = 37;		// films this sort of scene in the movie
const unsigned int JOB_FILMANAL         = 38;		// films this sort of scene in the movie
const unsigned int JOB_FILMLESBIAN      = 39;		// films this sort of scene in the movie. thinking about changing to Lesbian
const unsigned int JOB_FILMBONDAGE      = 40;		// films this sort of scene in the movie
const unsigned int JOB_FILMGROUP        = 41;		// films this sort of scene in the movie
const unsigned int JOB_FILMORAL         = 42;       // films this type of scene CRAZY
const unsigned int JOB_FILMMAST         = 43;		// films this type of scene CRAZY
const unsigned int JOB_FILMTITTY        = 44;		// films this type of scene CRAZY
const unsigned int JOB_FILMSTRIP        = 45;		// films this type of scene CRAZY
const unsigned int JOB_FILMRANDOM       = 46;		// Films a random sex scene ... it does NOT work like mos jobs, see following note.
		// *****IMPORTANT**** If you add more scene types, they must go somewhere between
		// JOB_FILMBEAST and JOB_FILMRANDOM, or it will cause the random job to stop working..
		// JOB_FILMBEAST must be the first film job, JOB_FILMRANDOM must be the last one. --PP

// - Movie studio crew
const unsigned int JOBFILTER_STUDIOCREW	= 8;
const unsigned int JOB_DIRECTOR			= 47;		// Does same work as matron plus adds quality to films.
const unsigned int JOB_PROMOTER			= 48;		// Advertising -- This helps film sales after it is created.
const unsigned int JOB_CAMERAMAGE       = 49;		// Uses magic to record the scenes to crystals (requires at least 1)
const unsigned int JOB_CRYSTALPURIFIER  = 50;		// Post editing to get the best out of the film (requires at least 1)
const unsigned int JOB_FLUFFER          = 51;		// Keeps the porn stars and animals aroused
const unsigned int JOB_STAGEHAND		= 52;		// Currently does the same as a cleaner.
const unsigned int JOB_FILMFREETIME		= 53;		// Currently does the same as a cleaner.

// - Clinic
const unsigned int JOBFILTER_CLINIC     = 9;	// teaches can train girls only up to their own skill level
const unsigned int JOB_DOCTOR           = 54;	// not added yet	// becomes a doctor (requires 1) (will make some extra cash for treating locals)
const unsigned int JOB_NURSE            = 55;	// not added yet	// construct girls can get repaired quickly
const unsigned int JOB_HEALING          = 56;	// not added yet	// takes 1 days for each wound trait received.
const unsigned int JOB_GETABORT         = 57;	// not added yet	// gets an abortion (takes 2 days)
const unsigned int JOB_PHYSICALSURGERY  = 58;	// not added yet	// magical plastic surgery (takes 5 days)
const unsigned int JOB_MECHANIC         = 59;	// not added yet	// construct girls can get repaired quickly
const unsigned int JOB_REPAIRSHOP       = 60;	// not added yet	// construct girls can get repaired quickly
// - clinic staff
const unsigned int JOBFILTER_CLINICSTAFF = 10;
const unsigned int JOB_CHAIRMAN          = 61;	// not added yet	
const unsigned int JOB_JANITOR           = 62;	// not added yet	
const unsigned int JOB_CLINICREST        = 63;	// not added yet	
// - Community Centre
const unsigned int JOBFILTER_COMMUNITYCENTRE = 11;
const unsigned int JOB_FEEDPOOR         = 64;	// not added yet	// work in a soup kitchen
const unsigned int JOB_COMUNITYSERVICE  = 65;	// not added yet	// Goes around town helping where they can
const unsigned int JOB_CENTREMANAGER    = 66;	// not added yet	
const unsigned int JOB_CLEANCENTRE      = 67;	// not added yet	
const unsigned int JOB_CENTREREST       = 68;	// not added yet	
// - drug centre
const unsigned int JOBFILTER_DRUGCENTRE = 12;
const unsigned int JOB_DRUGCOUNSELOR    = 69;	// not added yet	
const unsigned int JOB_REHAB            = 70;	// not added yet	

// - House
const unsigned int JOBFILTER_HOUSE = 13;
const unsigned int JOB_PERSONALTRAINING  = 71;
const unsigned int JOB_PERSONALBEDWARMER = 72;
const unsigned int JOB_CLEANHOUSE        = 73;
const unsigned int JOB_RECRUITER         = 74;
const unsigned int JOB_HOUSEREST         = 75;
const unsigned int JOB_HEADGIRL          = 76;

#if 0
// - Community Centre
const unsigned int JOBFILTER_COMMUNITYCENTRE = 9;
const unsigned int JOB_COLLECTDONATIONS = 47;	// not added yet	// collects money to help the poor
const unsigned int JOB_FEEDPOOR         = 48;	// not added yet	// work in a soup kitchen
const unsigned int JOB_MAKEITEMS        = 49;	// not added yet	// makes cheap crappy handmade items for selling to raise money (creates handmade item resource)
const unsigned int JOB_SELLITEMS        = 50;	// not added yet // goes out and sells the made items (sells handmade item resource)
const unsigned int JOB_COMUNITYSERVICE  = 51;	// not added yet	// Goes around town helping where they can
// - Drug Lab (these jobs gain bonus if in same building as a clinic)
const unsigned int JOBFILTER_DRUGLAB    = 8;
const unsigned int JOB_VIRASPLANTFUCKER = 39;	// not added yet	// the plants need to inseminate with other humanoids to proper-gate, creates vira blood items
const unsigned int JOB_SHROUDGROWER     = 40;	// not added yet	// They require lots of care, and may explode. Produces shroud mushroom item.
const unsigned int JOB_FAIRYDUSTER      = 41;	// not added yet	// captures and pounds faries to dust, produces fairy dust item
const unsigned int JOB_DRUGDEALER       = 42;	// not added yet	// goes out onto the streets to sell the items made with the other jobs
// - Alchemist lab (these jobs gain bonus if in same building as a drug lab)
const unsigned int JOBFILTER_ALCHEMISTLAB = 9;
const unsigned int JOB_FINDREGENTS      = 43;	// not added yet	// goes around and finds ingredients for potions, produces ingredients resource.
const unsigned int JOB_BREWPOTIONS      = 44;	// not added yet	// will randomly produce any items with the word "potion" in the name, uses ingredients resource
const unsigned int JOB_POTIONTESTER     = 45;	// not added yet	// Potion tester tests potions, they may die doing this or random stuff may happen. (requires 1)
// - Arena (these jobs gain bonus if in same building as a clinic)
const unsigned int JOBFILTER_ARENA      = 10;
const unsigned int JOB_FIGHTBEASTS      = 46;	// not added yet	// customers come to place bets on who will win, girl may die (uses beasts resource)
const unsigned int JOB_WRESTLE          = 47;	// not added yet	// as above no chance of dying
const unsigned int JOB_FIGHTTODEATH     = 48;	// not added yet	// as above but against other girls (chance of dying)
const unsigned int JOB_FIGHTVOLUNTEERS  = 49;	// not added yet	// fight against customers who volunteer for prizes of gold
const unsigned int JOB_COLLECTBETS      = 50;	// not added yet	// collects the bets from customers (requires 1)
// - Skills Centre
const unsigned int JOBFILTER_TRAININGCENTRE = 11;	// teaches can train girls only up to their own skill level
const unsigned int JOB_TEACHBDSM        = 51;	// not added yet	// teaches BDSM skills
const unsigned int JOB_TEACHSEX         = 52;	// not added yet	// teaches general sex skills
const unsigned int JOB_TEACHBEAST       = 53;	// not added yet	// teaches beastiality skills
const unsigned int JOB_TEACHMAGIC       = 54;	// not added yet	// teaches magic skills
const unsigned int JOB_TEACHCOMBAT      = 55;	// not added yet	// teaches combat skills
const unsigned int JOB_DAYCARE          = 56;	// not added yet	// looks after the local kids (non sex stuff of course)
const unsigned int JOB_SCHOOLING        = 57;	// not added yet	// teaches the local kids (non sex stuff of course)
const unsigned int JOB_TEACHDANCING     = 58;	// not added yet	// teaches dancing and social skills
const unsigned int JOB_TEACHSERVICE     = 59;	// not added yet	// teaches misc skills
const unsigned int JOB_TRAIN            = 60;	// not added yet	// trains the girl in all the disicplince for which there is a teacher

#endif

// - extra unassignable
const unsigned int JOB_INDUNGEON        = 77;
const unsigned int JOB_RUNAWAY          = 78;

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
const int DUNGEON_KID               = 13;

// Town Constants
const int TOWN_NUMBUSINESSES        = 250;	// the amount of businesses in the town
const int TOWN_OFFICIALSWAGES       = 10;	// how much the authorities in the town are paid by the govenment

// Incomes
const int INCOME_BUSINESS           = 10;

// Item types
const int NUM_SHOPITEMS             = 40;	// number of items that the shop may hold at one time
const unsigned int INVRING          = 1;	// worn on fingers (max 8)
const unsigned int INVDRESS         = 2;	// Worn on body, (max 1)
const unsigned int INVSHOES         = 3;	// worn on feet, (max 1)
const unsigned int INVFOOD          = 4;	// Eaten, single use
const unsigned int INVNECKLACE      = 5;	// worn on neck, (max 1)
const unsigned int INVWEAPON        = 6;	// equipped on body, often unequipped outside of combat, (max 2)
const unsigned int INVMAKEUP        = 7;	// worn on face, single use
const unsigned int INVARMOR         = 8;	// worn on body over dresses, often unequipped outside of combat, (max 1)
const unsigned int INVMISC          = 9;    // these items don't usually do anythinig just random stuff girls might buy. The ones that do, cause a constant effect without having to be equiped
const unsigned int INVARMBAND       = 10;	// (max 2), worn around arms
const unsigned int INVSMWEAPON      = 11;   // small weapon which can be hidden on body, (max 1)
const unsigned int INVUNDERWEAR     = 12;  //CRAZY added this under wear (max 1)

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
const unsigned int OBJECTIVE_EXTORTXNEWBUSINESS    = 8;
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
const unsigned char NUM_ACTIONTYPES         = 19;

const unsigned char ACTION_COMBAT           = 0;
const unsigned char ACTION_SEX              = 1;
const unsigned char ACTION_GENERAL          = 2;
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
const unsigned char ACTION_WORKMOVIE		= 13;
const unsigned char ACTION_WORKCUSTSERV     = 14;
const unsigned char ACTION_WORKCENTRE       = 15;
const unsigned char ACTION_WORKCLUB         = 16;
const unsigned char ACTION_WORKHAREM        = 17;
const unsigned char ACTION_WORKRECRUIT      = 18;

// Character image
const int IMGTYPE_ANAL      = 0;
const int IMGTYPE_BDSM      = 1;
const int IMGTYPE_SEX       = 2;
const int IMGTYPE_BEAST     = 3;
const int IMGTYPE_GROUP     = 4;
const int IMGTYPE_LESBIAN   = 5;

/*
 * keep the images that don't change with pregnancy
 * after the ones that do. Then we can convert by adding
 * PREG_OFFSET, and if the result is > NUM_IMGTYPES
 * then it was one of the unchanging categories
 */
const int IMGTYPE_PREGNANT  = 6;
const int IMGTYPE_DEATH     = 7;
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
/*
 * now make the pregnant varieties
 */
const int PREG_OFFSET = 22;
const int IMGTYPE_PREGANAL	    = IMGTYPE_ANAL	+ PREG_OFFSET;
const int IMGTYPE_PREGBDSM	    = IMGTYPE_BDSM	+ PREG_OFFSET;
const int IMGTYPE_PREGSEX	    = IMGTYPE_SEX	+ PREG_OFFSET;
const int IMGTYPE_PREGBEAST	    = IMGTYPE_BEAST + PREG_OFFSET;
const int IMGTYPE_PREGGROUP	    = IMGTYPE_GROUP + PREG_OFFSET;
const int IMGTYPE_PREGLESBIAN	= IMGTYPE_LESBIAN + PREG_OFFSET;
const int NUM_IMGTYPES          = IMGTYPE_PREGLESBIAN + 1;



const int NUM_GIRLFLAGS         = 30;

// Maximum number of traits a girl can have
const int MAXNUM_TRAITS         = 60;

// Maximum number of items in inventory
const int MAXNUM_INVENTORY         = 300;


// Listbox Constants moved from cListBox.h
const unsigned int LISTBOX_BLUE = 0;
const unsigned int LISTBOX_RED = 1;
const unsigned int LISTBOX_DARKBLUE = 2;

//how tall (in pixels) each list item is
const int LISTBOX_ITEMHEIGHT = 20;

//how many columns are allowed
const unsigned int LISTBOX_COLUMNS = 15;

// Event constants oved from cEvents.h
const int	EVENT_DAYSHIFT		= 0;
const int	EVENT_NIGHTSHIFT	= 1;
const int	EVENT_WARNING		= 2;
const int	EVENT_DANGER		= 3;
const int	EVENT_SUMMARY		= 4;
const int	EVENT_DUNGEON		= 5;			// For torturer reports
const int	EVENT_MATRON		= 6;			// For Matron reports
const int	EVENT_GANG			= 7;
const int	EVENT_BROTHEL		= 8;
const int	EVENT_NOWORK		= 10;
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
const int SCREEN_PLAYERHOUSE		= 23; // TODO: This value might be wrong.
const int SCREEN_GALLERY2			= 24;
const int SCREEN_GETINPUT           = 25;

// The following constants are used with g_CurrBrothel to determine if we are currently working with a brothel or a new building. --PP
const int BUILDING_BROTHEL			= 0;
const int BUILDING_STUDIO			= 10;
const int BUILDING_CLINIC			= 20;
const int BUILDING_ARENA			= 30;
const int BUILDING_CENTRE			= 40;
const int BUILDING_HOUSE			= 50;
const int BUILDING_ERROR			= 60;	// This shows there was an error somehow, looking for a building that does not exist.
#endif
