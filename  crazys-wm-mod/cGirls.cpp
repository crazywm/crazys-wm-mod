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
#include <fstream>
#include <algorithm>
#include "cTariff.h"
#include "cGirls.h"
#include "cEvents.h"
#include "math.h"
#include "cBrothel.h"
#include "cMessageBox.h"
#include "cGold.h"
#include "tinyxml.h"
#include "XmlMisc.h"
#include "cGangs.h"
#include "CGraphics.h"
#include "cMovieStudio.h"
#include "cArena.h"
#include "cCentre.h"
#include "cClinic.h"
#include "cHouse.h"
#include "cBrothel.h"
#include "cFarm.h"

#ifdef LINUX
#include "linux.h"
#endif
#include "DirPath.h"
#include "FileList.h"
#include "libintl.h"

using namespace std;

extern cMessageQue g_MessageQue;
extern cTraits g_Traits;
extern cEvents g_Events;
extern cInventory g_InvManager;
extern sGirl* MarketSlaveGirls[20];
extern CGraphics g_Graphics;
extern cRng g_Dice;
extern bool g_GenGirls;
extern cGold g_Gold;
extern cGangManager g_Gangs;
extern int g_Building;
extern cGirls g_Girls;
extern sGirl *selected_girl;
extern cBrothelManager g_Brothels;
extern cMovieStudioManager  g_Studios;
extern cArenaManager g_Arena;
extern cClinicManager g_Clinic;
extern cCentreManager g_Centre;
extern cHouseManager g_House;
extern cFarmManager g_Farm;
extern cFont m_Font;

extern unsigned long    g_Year;
extern unsigned long    g_Month;
extern unsigned long    g_Day;

extern cNameList	g_GirlNameList;
extern cNameList	g_BoysNameList;
extern cSurnameList g_SurnameList;

extern cPlayer* The_Player;
extern cConfig cfg;

extern cTariff tariff;

//SIN
//SPICE = added a lot of spice (variety/trait/skill) to dialogues
//SANITY = adding a 'craziness' stat, to be used with new events and reflect the world's impact on her sanity
//1 turns them ON, 0 turns them OFF (compiles with warnings, but not errors - worth it for the easy search)
#define SPICE 1;
#define SANITY 1;

// SIN - adding raisable (only) minimum age in config.xml file, because:
//(1) makes the code easier to read and maintain (so age references don't get Ctrl+H replaced along with other number "18"s, and vice versa)
//(2) makes it possible for user to increase the min age for girls (for legal or preference reasons - some places age is 20/21)
//(3) means that age can easily be used in dialogues, etc (without search, replace and recompile if user wants to update)
//This uses a variable collected from the config.xml (via sConfig) to allow user to enter higher age (but remains hard-coded to min 18).
//const int MINAGE = max(HCM, cfg.initial.minimumgirlage());

//SIN - FMA = Fixed Min Age.
//			//Set to 1, this uses the old code with age set at different points as a magic no. throughout various functions in the code.
//			//Set to 0, this uses the above MINAGE variable in all locations (easier to see and maintain), which allows raising of the minage if user prefers.
//(compiles with warnings, but not errors)
#define FMA 0;


/*
* MOD: DocClox: Stuff for the XML loader code
*
* first: static members need declaring
*/
bool sGirl::m_maps_setup = false;
map<string, unsigned int> sGirl::stat_lookup;
map<string, unsigned int> sGirl::skill_lookup;
map<string, unsigned int> sGirl::status_lookup;
map<string, unsigned int> sGirl::enjoy_lookup;
map<string, unsigned int> sGirl::jobs_lookup;
map<string, unsigned int> sGirl::training_lookup;

const char *sGirl::stat_names[] =
{
	"Charisma", "Happiness", "Libido", "Constitution", "Intelligence", "Confidence", "Mana", "Agility",
	"Fame", "Level", "AskPrice", "House", "Exp", "Age", "Obedience", "Spirit", "Beauty", "Tiredness", "Health",
	"PCFear", "PCLove", "PCHate", "Morality", "Refinement", "Dignity", "Lactation", "Strength",
	"NPCLove", "Sanity"
};
// `J` When modifying Stats or Skills, search for "J-Change-Stats-Skills"  :  found in >> cGirls.cpp > *_names[]
const char *sGirl::skill_names[] =
{
	"Anal", "Magic", "BDSM", "NormalSex", "Beastiality", "Group", "Lesbian", "Service", "Strip", "Combat", "OralSex", "TittySex",
	"Medicine", "Performance", "Handjob", "Crafting", "Herbalism", "Farming", "Brewing", "AnimalHandling", "Footjob", "Cooking"
	, 0
};
const char *sGirl::status_names[] =
{
	"None", "Poisoned", "Badly Poisoned", "Pregnant", "Pregnant By Player", "Slave", "Has Daughter", "Has Son",
	"Inseminated", "Controlled", "Catacombs", "Arena", "Your Daughter", "Is Daughter"
};
// `J` When modifying Action types, search for "J-Change-Action-Types"  :  found in >> cGirls.cpp > enjoy_names[]
const char *sGirl::enjoy_names[] =
{
	"COMBAT", "SEX", "WORKESCORT", "WORKCLEANING", "WORKMATRON", "WORKBAR", "WORKHALL", "WORKSHOW", "WORKSECURITY",
	"WORKADVERTISING", "WORKTORTURER", "WORKCARING", "WORKDOCTOR", "WORKMOVIE", "WORKCUSTSERV", "WORKCENTRE", "WORKCLUB",
	"WORKHAREM", "WORKRECRUIT", "WORKNURSE", "WORKMECHANIC", "WORKCOUNSELOR", "WORKMUSIC", "WORKSTRIP", "WORKMILK",
	"WORKMASSEUSE", "WORKFARM", "WORKTRAINING", "WORKREHAB", "MAKEPOTIONS", "MAKEITEMS", "COOKING", "GETTHERAPY",
	"WORKHOUSEPET", "GENERAL"
};
// `J` When modifying Action types, search for "J-Change-Action-Types"  :  found in >> cGirls.cpp > enjoy_jobs[]
const char *sGirl::enjoy_jobs[] = {
	"combat",							// ACTION_COMBAT
	"working as a whore",				// ACTION_SEX
	"working as an Escort",				// ACTION_WORKESCORT
	"cleaning",							// ACTION_WORKCLEANING
	"acting as a matron",				// ACTION_WORKMATRON
	"working in the bar",				// ACTION_WORKBAR
	"working in the gambling hall",		// ACTION_WORKHALL
	"producing movies",					// ACTION_WORKSHOW
	"providing security",				// ACTION_WORKSECURITY
	"doing advertising",				// ACTION_WORKADVERTISING
	"torturing people",					// ACTION_WORKTORTURER
	"caring for beasts",				// ACTION_WORKCARING
	"working as a doctor",				// ACTION_WORKDOCTOR
	"producing movies",					// ACTION_WORKMOVIE
	"providing customer service",		// ACTION_WORKCUSTSERV
	"working in the centre",			// ACTION_WORKCENTRE
	"working in the club",				// ACTION_WORKCLUB
	"being in your harem",				// ACTION_WORKHAREM
	"being a recruiter",				// ACTION_WORKRECRUIT
	"working as a nurse",				// ACTION_WORKNURSE
	"fixing things",					// ACTION_WORKMECHANIC
	"counseling people",				// ACTION_WORKCOUNSELOR
	"performing music",					// ACTION_WORKMUSIC
	"stripping",						// ACTION_WORKSTRIP
	"having her breasts milked",		// ACTION_WORKMILK
	"working as a masseuse",			// ACTION_WORKMASSEUSE
	"working on the farm",				// ACTION_WORKFARM
	"training",							// ACTION_WORKTRAINING
	"counseling",						// ACTION_WORKREHAB
	"making potions",					// ACTION_WORKMAKEPOTIONS
	"making items",						// ACTION_WORKMAKEITEMS
	"cooking",							// ACTION_WORKCOOKING
	"therapy",							// ACTION_WORKTHERAPY
	"puppy training",					// ACTION_WORKHOUSEPET
	"doing miscellaneous tasks"			// ACTION_GENERAL
};
const char *sGirl::children_type_names[] =
{
	"Total_Births", "Beasts", "All_Girls", "All_Boys", "Customer_Girls",
	"Customer_Boys", "Your_Girls", "Your_Boys", "Miscarriages", "Abortions"
};
// When modifying Training types, search for "Change-Traning-Types"  :  found in >> cGirls.cpp > training_names[]
const char *sGirl::training_names[] =
{
	"PUPPY", "PONY", "GENERAL"
};
//  When modifying Training types, search for "Change-Traning-Types"  :  found in >> cGirls.cpp > training_jobs[]
const char *sGirl::training_jobs[] = {
	"puppy training",
	"pony training",
	"general training"
};

// calculate the max like this, and it's self-maintaining
const unsigned int sGirl::max_stats = (sizeof(sGirl::stat_names) / sizeof(sGirl::stat_names[0]));
const unsigned int sGirl::max_skills = (sizeof(sGirl::skill_names) / sizeof(sGirl::skill_names[0]));
const unsigned int sGirl::max_statuses = (sizeof(sGirl::status_names) / sizeof(sGirl::status_names[0]));
const unsigned int sGirl::max_enjoy = (sizeof(sGirl::enjoy_names) / sizeof(sGirl::enjoy_names[0]));
const unsigned int sGirl::max_jobs = (sizeof(g_Brothels.m_JobManager.JobQkNm) / sizeof(g_Brothels.m_JobManager.JobQkNm[0]));
const unsigned int sGirl::max_training = (sizeof(sGirl::training_names) / sizeof(sGirl::training_names[0]));




sGirl::sGirl()				// constructor
{
	// sGirl stuff
	m_Prev = m_Next = 0;		m_newRandomFixed = -1;

	// Names
	m_Name = m_Realname = m_FirstName = m_MiddleName = m_Surname = m_MotherName = m_FatherName = m_Desc = "";

	// Time
	BirthMonth = BirthDay = 0;		m_BDay = 0;		m_WeeksPast = 0;

	// Jobs and money
	m_DayJob = m_NightJob = m_PrevDayJob = m_PrevNightJob = m_YesterDayJob = m_YesterNightJob = 255;
	m_WorkingDay = m_PrevWorkingDay = m_SpecialJobGoal = 0;

	where_is_she = 0;
	m_InClinic = m_InStudio = m_InArena = m_InCentre = m_InHouse = m_InFarm = false;
	m_Refused_To_Work_Day = m_Refused_To_Work_Night = false;
	m_Money = m_Pay = m_Tips = 0;
	m_NumCusts = m_NumCusts_old = 0;

	// Sex
	m_Virgin = -1;
	m_UseAntiPreg = true;
	m_WeeksPreg = 0;
	m_JustGaveBirth = false;
	m_PregCooldown = 0;

	// Health and happiness
	m_Tort = false;
	m_AccLevel = 0;
	m_Withdrawals = 0;
	m_DaysUnhappy = 0;
	m_RunAway = 0;
	m_Spotted = 0;

	// Inventory
	m_NumInventory = 0;
	for (int i = 0; i < MAXNUM_GIRL_INVENTORY; i++)	{ m_EquipedItems[i] = 0; m_Inventory[i] = 0; }

	//Traits
	m_NumRememTraits = m_NumTraits = 0;
	for (int i = 0; i < MAXNUM_TRAITS; i++)			{ m_Traits[i] = 0; m_TempTrait[i] = 0; }
	for (int i = 0; i < MAXNUM_TRAITS * 2; i++)		{ m_RememTraits[i] = 0; }

	// Stats and skills
	for (u_int i = 0; i < NUM_SKILLS; i++)		// Added m_Skills here to zero out any that are not specified -- PP
		m_Skills[i] = m_SkillTemps[i] = m_SkillMods[i] = m_SkillTr[i] = 0;
	for (int i = 0; i < NUM_STATS; i++)			// Added m_Stats here to zero out any that are not specified -- PP
		m_Stats[i] = m_StatTemps[i] = m_StatMods[i] = m_StatTr[i] = 0;
	m_Stats[STAT_HEALTH] = 100;
	m_Stats[STAT_HAPPINESS] = 100;
	m_Stats[STAT_HOUSE] = 60;  // Moved from above so it is not zero'd out by above changes --PP

	// Enjoyment
	for (int i = 0; i < NUM_ACTIONTYPES; i++)	// `J` Added m_Enjoyment here to zero out any that are not specified
		m_Enjoyment[i] = m_EnjoymentTR[i] = m_EnjoymentMods[i] = m_EnjoymentTemps[i] = 0;
	for (u_int i = 0; i < NUM_ACTIONTYPES; i++)	// `J` randomize starting likes -10 to 10 most closer to 0
		m_Enjoyment[i] = (g_Dice.bell(-10, 10));

	// Training
	for (int i = 0; i < NUM_TRAININGTYPES; i++)	// Added m_Training here to zero out any that are not specified
		m_Training[i] = m_TrainingTR[i] = m_TrainingMods[i] = m_TrainingTemps[i] = 0;

	// Others
	for (int i = 0; i < NUM_GIRLFLAGS; i++)			{ m_Flags[i] = 0; }
	for (int i = 0; i < CHILD_COUNT_TYPES; i++)		{ m_ChildrenCount[i] = 0; }
	m_States = m_BaseStates = 0;
	m_FetishTypes = 0;



	// Other things that I'm not sure how their defaults would be set
	//	cEvents m_Events;
	//	cTriggerList m_Triggers;
	//	cChildList m_Children;
	//	vector<string> m_Canonical_Daughters;




	/*
	*		MOD: DocClox, Sun Nov 15 06:08:32 GMT 2009
	*		initialise maps to look up stat and skill names
	*		needed for XML loader
	*
	*		things that need to happen every time the struct
	*		is constructed need to go before this point
	*		or they'll only happen the first time around
	*/
	if (!m_maps_setup)	// only need to do this once
		setup_maps();
}
sGirl::~sGirl()		// destructor
{
	//if (m_Name)		delete[] m_Name;
	m_Name = "";
	m_Events.Free();
	if (m_Next)		delete m_Next;
	m_Next = 0;
	m_Prev = 0;

}

int sGirl::has_item(const std::string& item)
{
	for (int i = 0; i < MAXNUM_GIRL_INVENTORY; i++)
	{
		if (m_Inventory[i])
		{
			if (m_Inventory[i]->m_Name == item) return i;
		}
	}
	return -1;
}

string stringtolowerj(string name)
{
	string s = "", t = "";
	for (u_int i = 0; i < name.length(); i++)
	{
		if (tolower(name[i]) != tolower(" "[0]) && tolower(name[i]) != tolower("."[0]) && tolower(name[i]) != tolower(","[0]))
		{
			t[0] = tolower(name[i]);
			s += t[0];
		}
	}
	return s;
}

int sGirl::has_item_j(const std::string& item)
{
	string s = stringtolowerj(item);
	for (int i = 0; i < MAXNUM_GIRL_INVENTORY; i++)
	{
		if (m_Inventory[i])
		{
			string t = stringtolowerj(m_Inventory[i]->m_Name);
			if (t == s)	return i;
		}
	}
	return -1;
}

int sGirl::add_inv(sInventoryItem* item)
{
    return g_Girls.AddInv(this, item);
}

bool sGirl::disobey_check(int action, sBrothel* brothel)
{
	int diff;
	int chance_to_obey = 0;							// high value - more likely to obey
	chance_to_obey = -g_Girls.GetRebelValue(this, false);	// let's start out with the basic rebelliousness
	chance_to_obey += 100;							// make it range from 0 to 200
	chance_to_obey /= 2;							// get a conventional percentage value
	/*
	*	OK, let's factor in having a matron: normally this is done in GetRebelValue
	*	but matrons have shifts now, so really we want twice the effect for a matron
	*	on each shift as we'd get from just one. //corrected:(Either that, or we need to make this
	*	check shift dependent.)//
	*
	*	Anyway, the old version added 15 for a matron either shift. Let's add
	*	10 for each shift. Full coverage gets you 20 points
	*/
	if (brothel)
	{ // `J` added building checks
		if (brothel->matron_on_shift(SHIFT_DAY, m_InClinic, m_InStudio, m_InArena, m_InCentre, m_InHouse, m_InFarm, where_is_she)) chance_to_obey += 10;
		if (brothel->matron_on_shift(SHIFT_NIGHT, m_InClinic, m_InStudio, m_InArena, m_InCentre, m_InHouse, m_InFarm, where_is_she)) chance_to_obey += 10;
	}
	/*
	*	This is still confusing - at least it still confuses me
	*	why not normalise the rebellion -100 to 100 value so it runs
	*	0 to 100, and invert it so it's basically an obedience check
	*/

	switch (action)
	{
	case ACTION_COMBAT:
		// WD use best stat as many girls have only one stat high
		diff = max(combat(), magic()) - 50;
		diff /= 3;
		chance_to_obey += diff;
		break;
	case ACTION_SEX:
		// Let's do the same thing here
		diff = libido();
		diff /= 5;
		chance_to_obey += diff;
		break;
	case ACTION_WORKCLEANING:
		//
		break;
	default:
		break;
	}
	chance_to_obey += m_Enjoyment[action];			// add in her enjoyment level
	chance_to_obey += pclove() / 10;					// let's add in some mods for love, fear and hate
	chance_to_obey += pcfear() / 10;
	chance_to_obey -= pchate() / 10;
	chance_to_obey += 30;									// Let's add a blanket 30% to all of that
	int roll = g_Dice.d100();								// let's get a percentage roll
	diff = chance_to_obey - roll;
	bool girl_obeys = (diff >= 0);
	if (girl_obeys)											// there's a price to be paid for relying on love or fear
	{
		if (diff < (pclove() / 10)) pclove(-1);	// if the only reason she obeys is love it wears away that love
		if (diff < (pcfear() / 10)) pcfear(-1);	// just a little bit. And if she's only doing it out of fear
	}
	/*
	*	do we need any more than this, really?
	*	we can add in some shaping factors if desired
	*/

	return !girl_obeys;
}


sRandomGirl::sRandomGirl()
{
	m_Name = "";
	m_Desc = "-";

	m_newRandom = false;
	m_Human = true;
	m_Catacomb = m_Arena = m_YourDaughter = m_IsDaughter = false;

	m_NumTraits = m_NumTraitNames = 0;
	for (int i = 0; i < MAXNUM_TRAITS; i++)
	{
		m_Traits[i] = 0;
		m_TraitChance[i] = 0;
	}
	m_NumItems = m_NumItemNames = 0;
	for (int i = 0; i < MAXNUM_GIRL_INVENTORY; i++)
	{
		m_ItemChance[i] = 0;
		m_Inventory[i] = 0;
	}

	//assigning defaults
	for (int i = 0; i < NUM_STATS; i++)
	{
		// `J` When modifying Stats or Skills, search for "J-Change-Stats-Skills"  :  found in >> cGirls.h > sRandomGirl
		switch (i)
		{
		case STAT_HAPPINESS:
		case STAT_HEALTH:
			m_MinStats[i] = m_MaxStats[i] = 100;
			break;
		case STAT_TIREDNESS:
		case STAT_FAME:
		case STAT_LEVEL:
		case STAT_EXP:
		case STAT_PCFEAR:
		case STAT_PCLOVE:
		case STAT_PCHATE:
		case STAT_ASKPRICE:
		case STAT_HOUSE:
			m_MinStats[i] = m_MaxStats[i] = 0;
			break;
		case STAT_AGE:
			m_MinStats[i] = 17; m_MaxStats[i] = 25;
			break;
		case STAT_MORALITY:
		case STAT_REFINEMENT:
		case STAT_DIGNITY:
		case STAT_SANITY:
			m_MinStats[i] = -10; m_MaxStats[i] = 10;
			break;
		case STAT_LACTATION:
			m_MinStats[i] = -20; m_MaxStats[i] = 20;
			break;
		default:
			m_MinStats[i] = 30;	m_MaxStats[i] = 60;
			break;
		}
	}
	for (int i = 0; i < NUM_SKILLS; i++)// Changed from 10 to NUM_SKILLS so that it will always set the proper number of defaults --PP
	{
		m_MinSkills[i] = 0;				// Changed from 30 to 0, made no sense for all skills to be a default of 30.
		m_MaxSkills[i] = 30;
	}
	// now for a few overrides
	m_MinMoney = 0;
	m_MaxMoney = 10;
}
sRandomGirl::~sRandomGirl()
{
	if (m_Next) delete m_Next;
	m_Next = 0;
}

// ----- Lookups
void sGirl::setup_maps()
{
	g_LogFile.os() << "[sGirl::setup_maps] Setting up Stats, Skills and Status codes." << endl;
	m_maps_setup = true;
	stat_lookup["Charisma"] = STAT_CHARISMA;
	stat_lookup["Happiness"] = STAT_HAPPINESS;
	stat_lookup["Libido"] = STAT_LIBIDO;
	stat_lookup["Constitution"] = STAT_CONSTITUTION;
	stat_lookup["Intelligence"] = STAT_INTELLIGENCE;
	stat_lookup["Confidence"] = STAT_CONFIDENCE;
	stat_lookup["Mana"] = STAT_MANA;
	stat_lookup["Agility"] = STAT_AGILITY;
	stat_lookup["Fame"] = STAT_FAME;
	stat_lookup["Level"] = STAT_LEVEL;
	stat_lookup["AskPrice"] = STAT_ASKPRICE;
	stat_lookup["House"] = STAT_HOUSE;
	stat_lookup["Exp"] = STAT_EXP;
	stat_lookup["Age"] = STAT_AGE;
	stat_lookup["Obedience"] = STAT_OBEDIENCE;
	stat_lookup["Spirit"] = STAT_SPIRIT;
	stat_lookup["Beauty"] = STAT_BEAUTY;
	stat_lookup["Tiredness"] = STAT_TIREDNESS;
	stat_lookup["Health"] = STAT_HEALTH;
	stat_lookup["PCFear"] = STAT_PCFEAR;
	stat_lookup["PCLove"] = STAT_PCLOVE;
	stat_lookup["PCHate"] = STAT_PCHATE;
	stat_lookup["Morality"] = STAT_MORALITY;
	stat_lookup["Refinement"] = STAT_REFINEMENT;
	stat_lookup["Dignity"] = STAT_DIGNITY;
	stat_lookup["Lactation"] = STAT_LACTATION;
	stat_lookup["Strength"] = STAT_STRENGTH;
	stat_lookup["NPCLove"] = STAT_NPCLOVE;
	stat_lookup["Sanity"] = STAT_SANITY;

	// `J` When modifying Stats or Skills, search for "J-Change-Stats-Skills"  :  found in >> cGirls.cpp > setup_maps


	skill_lookup["Magic"] = SKILL_MAGIC;
	skill_lookup["Combat"] = SKILL_COMBAT;
	skill_lookup["Service"] = SKILL_SERVICE;
	skill_lookup["Medicine"] = SKILL_MEDICINE;
	skill_lookup["Performance"] = SKILL_PERFORMANCE;
	skill_lookup["Crafting"] = SKILL_CRAFTING;
	skill_lookup["Herbalism"] = SKILL_HERBALISM;
	skill_lookup["Farming"] = SKILL_FARMING;
	skill_lookup["Brewing"] = SKILL_BREWING;
	skill_lookup["AnimalHandling"] = SKILL_ANIMALHANDLING;
	skill_lookup["Cooking"] = SKILL_COOKING;

	skill_lookup["Anal"] = SKILL_ANAL;
	skill_lookup["BDSM"] = SKILL_BDSM;
	skill_lookup["NormalSex"] = SKILL_NORMALSEX;
	skill_lookup["OralSex"] = SKILL_ORALSEX;
	skill_lookup["TittySex"] = SKILL_TITTYSEX;
	skill_lookup["Handjob"] = SKILL_HANDJOB;
	skill_lookup["Beastiality"] = SKILL_BEASTIALITY;
	skill_lookup["Group"] = SKILL_GROUP;
	skill_lookup["Lesbian"] = SKILL_LESBIAN;
	skill_lookup["Strip"] = SKILL_STRIP;
	skill_lookup["Footjob"] = SKILL_FOOTJOB;

	status_lookup["None"] = STATUS_NONE;
	status_lookup["Poisoned"] = STATUS_POISONED;
	status_lookup["Badly Poisoned"] = STATUS_BADLY_POISONED;
	status_lookup["Pregnant"] = STATUS_PREGNANT;
	status_lookup["Pregnant By Player"] = STATUS_PREGNANT_BY_PLAYER;
	status_lookup["Slave"] = STATUS_SLAVE;
	status_lookup["Has Daughter"] = STATUS_HAS_DAUGHTER;
	status_lookup["Has Son"] = STATUS_HAS_SON;
	status_lookup["Inseminated"] = STATUS_INSEMINATED;
	status_lookup["Controlled"] = STATUS_CONTROLLED;
	status_lookup["Catacombs"] = STATUS_CATACOMBS;
	status_lookup["Arena"] = STATUS_ARENA;
	status_lookup["Your Daughter"] = STATUS_YOURDAUGHTER;
	status_lookup["Is Daughter"] = STATUS_ISDAUGHTER;

	// `J` When modifying Action types, search for "J-Change-Action-Types"  :  found in >> cGirls.cpp > setup_maps

	enjoy_lookup["COMBAT"] = ACTION_COMBAT;
	enjoy_lookup["SEX"] = ACTION_SEX;
	enjoy_lookup["WORKESCORT"] = ACTION_WORKESCORT;
	enjoy_lookup["WORKCLEANING"] = ACTION_WORKCLEANING;
	enjoy_lookup["WORKMATRON"] = ACTION_WORKMATRON;
	enjoy_lookup["WORKBAR"] = ACTION_WORKBAR;
	enjoy_lookup["WORKHALL"] = ACTION_WORKHALL;
	enjoy_lookup["WORKSHOW"] = ACTION_WORKSHOW;
	enjoy_lookup["WORKSECURITY"] = ACTION_WORKSECURITY;
	enjoy_lookup["WORKADVERTISING"] = ACTION_WORKADVERTISING;
	enjoy_lookup["WORKTORTURER"] = ACTION_WORKTORTURER;
	enjoy_lookup["WORKCARING"] = ACTION_WORKCARING;
	enjoy_lookup["WORKDOCTOR"] = ACTION_WORKDOCTOR;
	enjoy_lookup["WORKMOVIE"] = ACTION_WORKMOVIE;
	enjoy_lookup["WORKCUSTSERV"] = ACTION_WORKCUSTSERV;
	enjoy_lookup["WORKCENTRE"] = ACTION_WORKCENTRE;
	enjoy_lookup["WORKCLUB"] = ACTION_WORKCLUB;
	enjoy_lookup["WORKHAREM"] = ACTION_WORKHAREM;
	enjoy_lookup["WORKRECRUIT"] = ACTION_WORKRECRUIT;
	enjoy_lookup["WORKNURSE"] = ACTION_WORKNURSE;
	enjoy_lookup["WORKMECHANIC"] = ACTION_WORKMECHANIC;
	enjoy_lookup["WORKCOUNSELOR"] = ACTION_WORKCOUNSELOR;
	enjoy_lookup["WORKMUSIC"] = ACTION_WORKMUSIC;
	enjoy_lookup["WORKSTRIP"] = ACTION_WORKSTRIP;
	enjoy_lookup["WORKMILK"] = ACTION_WORKMILK;
	enjoy_lookup["WORKMASSEUSE"] = ACTION_WORKMASSEUSE;
	enjoy_lookup["WORKFARM"] = ACTION_WORKFARM;
	enjoy_lookup["WORKINTERN"] = ACTION_WORKTRAINING;		// `J` changed WORKINTERN to WORKTRAINING...
	enjoy_lookup["WORKTRAINING"] = ACTION_WORKTRAINING;		// to allow it to be used for any training job
	enjoy_lookup["WORKREHAB"] = ACTION_WORKREHAB;
	enjoy_lookup["MAKEPOTIONS"] = ACTION_WORKMAKEPOTIONS;
	enjoy_lookup["MAKEITEMS"] = ACTION_WORKMAKEITEMS;
	enjoy_lookup["COOKING"] = ACTION_WORKCOOKING;
	enjoy_lookup["GETTHERAPY"] = ACTION_WORKTHERAPY;
	enjoy_lookup["WORKHOUSEPET"] = ACTION_WORKHOUSEPET;
	enjoy_lookup["GENERAL"] = ACTION_GENERAL;


	// `J` When modifying Jobs, search for "J-Change-Jobs"  :  found in >> cGirls.cpp > jobs_lookup
	jobs_lookup["Adv"] = JOB_ADVERTISING;
	jobs_lookup["AMng"] = JOB_ANGER;
	jobs_lookup["TOff"] = JOB_ARENAREST;
	jobs_lookup["AssJ"] = JOB_ASSJOB;
	jobs_lookup["Bkr"] = JOB_BAKER;
	jobs_lookup["BrCk"] = JOB_BARCOOK;
	jobs_lookup["BrMd"] = JOB_BARMAID;
	jobs_lookup["SSrp"] = JOB_BARSTRIPPER;
	jobs_lookup["SWhr"] = JOB_BARWHORE;
	jobs_lookup["BCpt"] = JOB_BEASTCAPTURE;
	jobs_lookup["BstC"] = JOB_BEASTCARER;
	jobs_lookup["Bksm"] = JOB_BLACKSMITH;
	jobs_lookup["BbJb"] = JOB_BOOBJOB;
	jobs_lookup["BRS"] = JOB_BREASTREDUCTION;
	jobs_lookup["Brwr"] = JOB_BREWER;
	jobs_lookup["BStp"] = JOB_BROTHELSTRIPPER;
	jobs_lookup["Bchr"] = JOB_BUTCHER;
	jobs_lookup["CM"] = JOB_CAMERAMAGE;
	jobs_lookup["CRnc"] = JOB_CATACOMBRANCHER;
	jobs_lookup["CMgr"] = JOB_CENTREMANAGER;
	jobs_lookup["TOff"] = JOB_CENTREREST;
	jobs_lookup["Crmn"] = JOB_CHAIRMAN;
	jobs_lookup["CGrd"] = JOB_CITYGUARD;
	jobs_lookup["GKpr"] = JOB_CLEANARENA;
	jobs_lookup["ClnC"] = JOB_CLEANCENTRE;
	jobs_lookup["ClnH"] = JOB_CLEANHOUSE;
	jobs_lookup["Cln"] = JOB_CLEANING;
	jobs_lookup["TOff"] = JOB_CLINICREST;
	jobs_lookup["Cblr"] = JOB_COBBLER;
	jobs_lookup["CmSr"] = JOB_COMUNITYSERVICE;
	jobs_lookup["CosS"] = JOB_COSMETICSURGERY;
	jobs_lookup["Cnsl"] = JOB_COUNSELOR;
	jobs_lookup["CP"] = JOB_CRYSTALPURIFIER;
	jobs_lookup["Cure"] = JOB_CUREDISEASES;
	jobs_lookup["CS"] = JOB_CUSTOMERSERVICE;
	jobs_lookup["Dlr"] = JOB_DEALER;
	jobs_lookup["Dir"] = JOB_DIRECTOR;
	jobs_lookup["Dtre"] = JOB_DOCTORE;
	jobs_lookup["Doc"] = JOB_DOCTOR;
	jobs_lookup["Entn"] = JOB_ENTERTAINMENT;
	jobs_lookup["Scrt"] = JOB_ESCORT;
	jobs_lookup["ExC"] = JOB_EXPLORECATACOMBS;
	jobs_lookup["EThr"] = JOB_EXTHERAPY;
	jobs_lookup["FLft"] = JOB_FACELIFT;
	jobs_lookup["FOEx"] = JOB_FAKEORGASM;
	jobs_lookup["Frmr"] = JOB_FARMER;
	jobs_lookup["FHnd"] = JOB_FARMHAND;
	jobs_lookup["FMgr"] = JOB_FARMMANGER;
	jobs_lookup["TOff"] = JOB_FARMREST;
	jobs_lookup["Feed"] = JOB_FEEDPOOR;
	jobs_lookup["FrtT"] = JOB_FERTILITY;
	jobs_lookup["Cage"] = JOB_FIGHTARENAGIRLS;
	jobs_lookup["FiBs"] = JOB_FIGHTBEASTS;
	jobs_lookup["CT"] = JOB_FIGHTTRAIN;
	jobs_lookup["FAct"] = JOB_FILMACTION;
	jobs_lookup["FAnl"] = JOB_FILMANAL;
	jobs_lookup["FBst"] = JOB_FILMBEAST;
	jobs_lookup["FBnd"] = JOB_FILMBONDAGE;
	jobs_lookup["FBuk"] = JOB_FILMBUKKAKE;
	jobs_lookup["FChf"] = JOB_FILMCHEF;
	jobs_lookup["FFac"] = JOB_FILMFACEFUCK;
	jobs_lookup["FFJ"] = JOB_FILMFOOTJOB;
	jobs_lookup["TOff"] = JOB_FILMFREETIME;
	jobs_lookup["FGrp"] = JOB_FILMGROUP;
	jobs_lookup["FHJ"] = JOB_FILMHANDJOB;
	jobs_lookup["FLes"] = JOB_FILMLESBIAN;
	jobs_lookup["FMst"] = JOB_FILMMAST;
	jobs_lookup["FMus"] = JOB_FILMMUSIC;
	jobs_lookup["FOrl"] = JOB_FILMORAL;
	jobs_lookup["FTor"] = JOB_FILMPUBLICBDSM;
	jobs_lookup["FRnd"] = JOB_FILMRANDOM;
	jobs_lookup["FSex"] = JOB_FILMSEX;
	jobs_lookup["FStp"] = JOB_FILMSTRIP;
	jobs_lookup["FTea"] = JOB_FILMTEASE;
	jobs_lookup["FTit"] = JOB_FILMTITTY;
	jobs_lookup["Fluf"] = JOB_FLUFFER;
	jobs_lookup["Grdn"] = JOB_GARDENER;
	jobs_lookup["Abrt"] = JOB_GETABORT;
	jobs_lookup["Heal"] = JOB_GETHEALING;
	jobs_lookup["Repr"] = JOB_GETREPAIRS;
	jobs_lookup["HGrl"] = JOB_HEADGIRL;
	jobs_lookup["Hcok"] = JOB_HOUSECOOK;
	jobs_lookup["Hpet"] = JOB_HOUSEPET;
	jobs_lookup["TOff"] = JOB_HOUSEREST;
	jobs_lookup["Dngn"] = JOB_INDUNGEON;
	jobs_lookup["Ntrn"] = JOB_INTERN;
	jobs_lookup["Jntr"] = JOB_JANITOR;
	jobs_lookup["Jwlr"] = JOB_JEWELER;
	jobs_lookup["Lipo"] = JOB_LIPO;
	jobs_lookup["MkIt"] = JOB_MAKEITEM;
	jobs_lookup["MkPt"] = JOB_MAKEPOTIONS;
	jobs_lookup["Mrkt"] = JOB_MARKETER;
	jobs_lookup["Msus"] = JOB_MASSEUSE;
	jobs_lookup["Mtrn"] = JOB_MATRON;
	jobs_lookup["Mech"] = JOB_MECHANIC;
	jobs_lookup["Mlkr"] = JOB_MILKER;
	jobs_lookup["Mlkd"] = JOB_MILK;
	jobs_lookup["Nurs"] = JOB_NURSE;
	jobs_lookup["Peep"] = JOB_PEEP;
	jobs_lookup["BdWm"] = JOB_PERSONALBEDWARMER;
	jobs_lookup["PTrn"] = JOB_PERSONALTRAINING;
	jobs_lookup["Pno"] = JOB_PIANO;
	jobs_lookup["Prmt"] = JOB_PROMOTER;
	jobs_lookup["Rncr"] = JOB_RANCHER;
	jobs_lookup["Rcrt"] = JOB_RECRUITER;
	jobs_lookup["Rehb"] = JOB_REHAB;
	jobs_lookup["Rsrc"] = JOB_RESEARCH;
	jobs_lookup["TOff"] = JOB_RESTING;
	jobs_lookup["RunA"] = JOB_RUNAWAY;
	jobs_lookup["Sec"] = JOB_SECURITY;
	jobs_lookup["SHrd"] = JOB_SHEPHERD;
	jobs_lookup["Sngr"] = JOB_SINGER;
	jobs_lookup["SBmd"] = JOB_SLEAZYBARMAID;
	jobs_lookup["SWtr"] = JOB_SLEAZYWAITRESS;
	jobs_lookup["SOBi"] = JOB_SO_BISEXUAL;
	jobs_lookup["SOLe"] = JOB_SO_LESBIAN;
	jobs_lookup["SOSt"] = JOB_SO_STRAIGHT;
	jobs_lookup["StgH"] = JOB_STAGEHAND;
	jobs_lookup["Talr"] = JOB_TAILOR;
	jobs_lookup["Thrp"] = JOB_THERAPY;
	jobs_lookup["Trtr"] = JOB_TORTURER;
	jobs_lookup["Prtc"] = JOB_TRAINING;
	jobs_lookup["TTid"] = JOB_TUBESTIED;
	jobs_lookup["VagR"] = JOB_VAGINAREJUV;
	jobs_lookup["Vet"] = JOB_VETERINARIAN;
	jobs_lookup["Wtrs"] = JOB_WAITRESS;
	jobs_lookup["BWhr"] = JOB_WHOREBROTHEL;
	jobs_lookup["HWhr"] = JOB_WHOREGAMBHALL;
	jobs_lookup["StWr"] = JOB_WHORESTREETS;
	jobs_lookup["XXXE"] = JOB_XXXENTERTAINMENT;
	jobs_lookup["255"] = 255;


	// When modifying Training types, search for "Change-Traning-Types"  :  found in >> cGirls.cpp > setup_maps

	training_lookup["PUPPY"] = TRAINING_PUPPY;
	training_lookup["GENERAL"] = TRAINING_GENERAL;


}

int sGirl::lookup_skill_code(string s)
{
	// be useful to be able to log unrecognised type names here
	if (skill_lookup.find(s) == skill_lookup.end())
	{
		g_LogFile.os() << "[sGirl::lookup_skill_code] Error: unknown Skill: " << s << endl;
		return -1;
	}
	return skill_lookup[s];
}

int sGirl::lookup_status_code(string s)
{
	// be useful to be able to log unrecognised type names here
	if (status_lookup.find(s) == status_lookup.end())
	{
		g_LogFile.os() << "[sGirl::lookup_status_code] Error: unknown Status: " << s << endl;
		return -1;
	}
	return status_lookup[s];
}

int sGirl::lookup_stat_code(string s)
{
	// be useful to be able to log unrecognised type names here
	if (stat_lookup.find(s) == stat_lookup.end())
	{
		g_LogFile.os() << "[sGirl::lookup_stat_code] Error: unknown Stat: " << s << endl;
		return -1;
	}
	return stat_lookup[s];
}

int sGirl::lookup_enjoy_code(string s)
{
	// be useful to be able to log unrecognised type names here
	if (enjoy_lookup.find(s) == enjoy_lookup.end())
	{
		g_LogFile.os() << "[sGirl::lookup_enjoy_code] Error: unknown Enjoy: " << s << endl;
		return -1;
	}
	return enjoy_lookup[s];
}

int sGirl::lookup_jobs_code(string s)
{
	// be useful to be able to log unrecognised type names here
	if (jobs_lookup.find(s) == jobs_lookup.end())
	{
		// `J` added check for missing jobs_lookup
		for (int i = 0; i < NUM_JOBS; i++)
		{
			if (g_Brothels.m_JobManager.JobQkNm[i] == s)
				return i;
		}
		// if still not found, send original error message
		g_LogFile.os() << "[sGirl::jobs_enjoy_code] Error: unknown Job: " << s << endl;
		return -1;
	}
	return jobs_lookup[s];
}

int sGirl::lookup_training_code(string s)
{
	// be useful to be able to log unrecognised type names here
	if (training_lookup.find(s) == training_lookup.end())
	{
		g_LogFile.os() << "[sGirl::lookup_training_code] Error: unknown Training: " << s << endl;
		return -1;
	}
	return training_lookup[s];
}

string sGirl::lookup_where_she_is()
{
	if (this->m_InStudio)	return "Studio";
	if (this->m_InArena)	return "Arena";
	if (this->m_InCentre)	return "Centre";
	if (this->m_InClinic)	return "Clinic";
	if (this->m_InFarm)		return "Farm";
	if (this->m_InHouse)	return "House";
	return g_Brothels.GetName(where_is_she);
}

// END MOD

class GirlPredicate_GRG : public GirlPredicate
{
	bool m_slave = false;
	bool m_catacomb = false;
	bool m_arena = false;
	bool m_yourdaughter = false;
	bool m_isdaughter = false;
public:
	GirlPredicate_GRG(bool slave, bool catacomb, bool arena, bool daughter, bool isdaughter)
	{
		m_slave = slave;
		m_catacomb = catacomb;
		m_arena = arena;
		m_yourdaughter = daughter;
		m_isdaughter = isdaughter;
	}
	virtual bool test(sGirl *girl)
	{
		return  girl->is_slave() == m_slave
			&&	girl->is_monster() == m_catacomb
			&&	girl->is_arena() == m_arena
			&&	girl->is_yourdaughter() == m_yourdaughter
			&&	girl->is_isdaughter() == m_isdaughter;
	}
};

// ----- Create / destroy

cGirls::cGirls()
{
	m_Parent = 0;
	m_Last = 0;
	m_NumRandomGirls = m_NumGirls =
		m_NumHumanRandomGirls =
		m_NumNonHumanRandomGirls =
		m_NumRandomYourDaughterGirls =
		m_NumHumanRandomYourDaughterGirls =
		m_NumNonHumanRandomYourDaughterGirls = 0;
	m_RandomGirls = 0;
	m_LastRandomGirls = 0;
}

cGirls::~cGirls()
{
	Free();
}

void cGirls::Free()
{
	if (m_Parent) delete m_Parent;
	m_Parent = m_Last = 0;
	m_NumGirls = 0;
	g_GenGirls = false;
	if (m_RandomGirls) delete m_RandomGirls;
	m_RandomGirls = 0;
	m_LastRandomGirls = 0;
	m_NumRandomGirls =
	m_NumHumanRandomGirls =
		m_NumNonHumanRandomGirls =
		m_NumRandomYourDaughterGirls =
		m_NumHumanRandomYourDaughterGirls =
		m_NumNonHumanRandomYourDaughterGirls = 0;

}

// ----- Utility

static char *n_strdup(const char *s)
{
	return strcpy(new char[strlen(s) + 1], s);
}

sGirl *sRandomGirl::lookup = new sGirl();  // used to look up stat and skill IDs

// ----- Misc

void cGirls::CalculateGirlType(sGirl* girl)
{
	// `J` When adding new traits, search for "J-Add-New-Traits"  :  found in >> CalculateGirlType
	int BigBoobs = 0, SmallBoobs = 0, CuteGirl = 0, Dangerous = 0, Cool = 0, Nerd = 0, NonHuman = 0, Lolita = 0,
		Elegant = 0, Sexy = 0, NiceFigure = 0, NiceArse = 0, Freak = 0, Tall = 0, Short = 0, Fat = 0;

	girl->m_FetishTypes = 0;

	// template
	if (girl->has_trait(""))		{
		BigBoobs += 0;		SmallBoobs += 0;	CuteGirl += 0;		Dangerous += 0;
		Cool += 0;			Nerd += 0;			NonHuman += 0;		Lolita += 0;
		Elegant += 0;		Sexy += 0;			NiceFigure += 0;	NiceArse += 0;
		Freak += 0;			Tall += 0;			Short += 0;			Fat += 0;
	}


	//SIN: sorted ALL traits and included ALL new traits from current CoreTraits.traitsx file (v126)
	// Included code for traits that don't do anything for completeness, but these are commented out for performance.
	/****** Physical Traits ******/
	// Breasts
	/* */if (girl->has_trait("Flat Chest"))				{ BigBoobs -= 120;	SmallBoobs += 80;	CuteGirl += 20;	Lolita += 30;	Sexy -= 10;	NiceFigure -= 10; }
	else if (girl->has_trait("Petite Breasts"))			{ BigBoobs -= 80;	SmallBoobs += 100;	CuteGirl += 15;	Lolita += 20;	Sexy -= 5;	NiceFigure -= 5; }
	else if (girl->has_trait("Small Boobs"))				{ BigBoobs -= 40;	SmallBoobs += 50;	CuteGirl += 10;	Lolita += 10;	Sexy += 5;	NiceFigure -= 0; }
	else if (girl->has_trait("Busty Boobs"))				{ BigBoobs += 20;	SmallBoobs -= 20;	CuteGirl += 5;	Lolita += 0; 	Sexy += 10;	NiceFigure += 5; }
	else if (girl->has_trait("Big Boobs"))				{ BigBoobs += 40;	SmallBoobs -= 40;	CuteGirl -= 5;	Lolita -= 20;	Sexy += 20;	NiceFigure += 10; }
	else if (girl->has_trait("Giant Juggs"))				{ BigBoobs += 60;	SmallBoobs -= 60;	CuteGirl -= 25;	Lolita -= 40;	Sexy += 20;	NiceFigure += 5; }
	else if (girl->has_trait("Massive Melons"))			{ BigBoobs += 80;	SmallBoobs -= 80;	CuteGirl -= 25;	Lolita -= 60;	Sexy += 10;	NiceFigure -= 5;	Freak += 5; }
	else if (girl->has_trait("Abnormally Large Boobs"))	{ BigBoobs += 100;	SmallBoobs -= 100;	CuteGirl -= 15;	Lolita -= 80;	Sexy += 0;	NiceFigure -= 10;	Freak += 10;	NonHuman += 5; }
	else if (girl->has_trait("Titanic Tits"))			{ BigBoobs += 120;	SmallBoobs -= 120;	CuteGirl -= 25;	Lolita -= 100;	Sexy -= 10;	NiceFigure -= 20;	Freak += 20;	NonHuman += 10; }

	// Nipples
	if (girl->has_trait("Inverted Nipples"))				{ BigBoobs -= 5;	SmallBoobs += 5;	CuteGirl -= 5;	Freak += 5; }
	if (girl->has_trait("Perky Nipples"))				{ BigBoobs += 10;	SmallBoobs += 5;	CuteGirl += 5;	Lolita += 5; }
	if (girl->has_trait("Puffy Nipples"))				{ BigBoobs += 10;	SmallBoobs -= 5;	CuteGirl += 5; }

	// Milk
	if (girl->has_trait("Dry Milk"))						{ BigBoobs -= 10;	SmallBoobs += 10;	CuteGirl += 5;	Lolita += 10; }
	if (girl->has_trait("Scarce Lactation"))				{ BigBoobs -= 5;	SmallBoobs += 5;	CuteGirl += 0;	Lolita += 5; }
	if (girl->has_trait("Abundant Lactation"))			{ BigBoobs += 10;	SmallBoobs -= 10;	CuteGirl -= 0;	Lolita -= 10; }
	if (girl->has_trait("Cow Tits"))						{ BigBoobs += 20;	SmallBoobs -= 20;	CuteGirl -= 5;	Lolita -= 20;	Freak += 5; }

	//Body
	if (girl->has_trait("Fleet of Foot"))				{ Dangerous += 10; Sexy += 20; }
	if (girl->has_trait("Fragile"))						{ CuteGirl += 10; Nerd += 5; Freak += 10; Lolita += 10; }
	if (girl->has_trait("Long Legs"))					{ Sexy += 20; NiceFigure += 20; }
	if (girl->has_trait("Strong"))						{ Dangerous += 20; Cool += 20; Nerd -= 30; NiceFigure += 20; NiceArse += 20; Lolita -= 5; }
	if (girl->has_trait("Tough"))						{ CuteGirl -= 5; Dangerous += 10; Cool += 10; Nerd -= 5; Elegant -= 5; }
	if (girl->has_trait("Agile"))						{ Dangerous += 20; Sexy += 5; Freak += 10; }
	if (girl->has_trait("Delicate"))						{ CuteGirl += 5; Nerd += 5; Freak += 5; Lolita += 10; }
	if (girl->has_trait("Dick-Sucking Lips"))			{ Sexy += 20; CuteGirl += 20; }
	if (girl->has_trait("Exotic"))						{ Sexy += 10; CuteGirl += 10; Freak += 10; }
	if (girl->has_trait("Large Hips"))					{ Sexy += 15; Lolita -= 20; NiceArse += 20; }
	if (girl->has_trait("Mature Body"))					{ Lolita -= 50; CuteGirl -= 20; Sexy += 10; }
	if (girl->has_trait("Muscular"))						{ Lolita -= 30; Sexy -= 30; Dangerous += 25; Freak += 25; }
	if (girl->has_trait("Old"))							{ Lolita -= 75; CuteGirl -= 40; Sexy -= 20; Freak += 30; Elegant += 15; }
	if (girl->has_trait("Whore"))						{ Lolita -= 50; CuteGirl -= 40; Cool += 10; Sexy += 10; }

	//Figure
	if (girl->has_trait("Great Figure"))					{ BigBoobs += 10; Sexy += 10; NiceFigure += 60; Fat -= 100;}
	if (girl->has_trait("Hourglass Figure"))				{ Sexy += 25; Lolita -= 30; NiceFigure += 20; Fat -= 50;}
	if (girl->has_trait("Plump"))						{ Lolita -= 10; NiceFigure -= 20; Freak += 20; Fat += 50;}
	if (girl->has_trait("Fat"))							{ Lolita -= 20; NiceFigure -= 40; Freak += 25; Fat += 100;}

	//Height
	if (girl->has_trait("Giant"))						{ Freak += 20; Lolita -= 60; Tall += 75; Short -= 100;}
	if (girl->has_trait("Tall"))							{ Lolita -= 30; Tall += 50; Short -= 50;}
	if (girl->has_trait("Short"))						{ Lolita += 15; Tall -= 50; Short += 50;}
	if (girl->has_trait("Dwarf"))						{ Freak += 20; Lolita += 10; Tall -= 75; Short += 100;}

	//Ass
	if (girl->has_trait("Great Arse"))					{ Sexy += 10; NiceArse += 60; }
	if (girl->has_trait("Tight Butt"))					{ Lolita += 5; Sexy += 20; NiceArse += 40; }
	if (girl->has_trait("Phat Booty"))					{ Lolita -= 20; Sexy += 15; NiceArse += 30; }
	if (girl->has_trait("Deluxe Derriere"))				{ Lolita -= 25; Sexy += 20; NiceArse += 40; }
	if (girl->has_trait("Wide Bottom"))					{ Lolita -= 20; Sexy += 15; NiceArse += 20; }
	if (girl->has_trait("Plump Tush"))					{ Lolita -= 20; Sexy += 30; NiceArse += 10; }
	if (girl->has_trait("Flat Ass"))						{ Lolita += 20; Sexy -= 20; NiceArse -= 50; }

	//Piercings, Brandings & Tattoos
	if (girl->has_trait("Pierced Clit"))					{ Elegant -= 5; Sexy += 20; Freak += 15; }
	if (girl->has_trait("Pierced Nipples"))				{ Elegant -= 10; Sexy += 20; Freak += 15; }
	if (girl->has_trait("Pierced Tongue"))				{ Elegant -= 20; Sexy += 10; Freak += 15; }
	if (girl->has_trait("Pierced Navel"))				{ Elegant -= 5; Sexy += 5; Freak += 15; }
	if (girl->has_trait("Pierced Nose"))					{ Elegant -= 30; Sexy += 5; Dangerous += 5; Freak += 15; }
	if (girl->has_trait("Tattooed"))						{ Cool += 10; Dangerous += 5; }
	if (girl->has_trait("Small Tattoos"))				{ Cool += 25; Elegant -= 5; Dangerous += 5; }
	if (girl->has_trait("Heavily Tattooed"))				{ Cool += 30; Dangerous += 15; Elegant -= 20; }
	if (girl->has_trait("Branded on the Ass"))			{ Freak += 25; Elegant -= 20; NiceArse -= 10; }
	if (girl->has_trait("Branded on the Forehead"))		{ Cool -= 30; Elegant -= 30; Freak += 25; }


	/*********  Scars, Mutilations, Amputations, Disabilities, etc.  ***************/
	//Born this way...
	if (girl->has_trait("Malformed"))					{ NonHuman += 10; Freak += 50; }
	if (girl->has_trait("Beauty Mark"))					{ Cool += 5; Freak += 5; Sexy += 5; }
	if (girl->has_trait("Blind"))						{ Freak += 10; Dangerous -= 15; }
	if (girl->has_trait("Deaf"))							{ Freak += 10; Dangerous -= 5; }
	if (girl->has_trait("Mute"))							{ Freak += 15; Dangerous -= 5; }
	//	if (girl->has_trait("Bad Eyesight"))				{} // Not visible to customer
	//	if (girl->has_trait("Sharp-Eyed"))				{} // Not visible to customer
	//	if (girl->has_trait("Incest"))					{} // Not visible to customer
	//Scars and wounds
	if (girl->has_trait("Small Scars"))					{ CuteGirl -= 5; Dangerous += 5; Cool += 2; Freak += 2; }
	if (girl->has_trait("Cool Scars"))					{ CuteGirl -= 10; Dangerous += 20; Cool += 30; Freak += 5; }
	if (girl->has_trait("Horrific Scars"))				{ CuteGirl -= 15; Dangerous += 30; Freak += 20; }
	//Missing Parts
	if (girl->has_trait("One Eye"))						{ CuteGirl -= 20; Cool += 5; Dangerous += 10; Sexy -= 20; NiceFigure -= 10; Freak += 20; }
	if (girl->has_trait("Eye Patch"))					{ CuteGirl -= 5; Dangerous += 5; Cool += 20; Sexy -= 5; Freak += 20; }
	if (girl->has_trait("Missing Finger"))				{ Freak += 5; }
	if (girl->has_trait("Missing Fingers"))				{ Freak += 10; }
	if (girl->has_trait("One Hand"))						{ Freak += 20; Dangerous -= 10; }
	if (girl->has_trait("No Hands"))						{ Freak += 40; Dangerous -= 20; }
	if (girl->has_trait("One Arm"))						{ Freak += 40; Dangerous -= 20; }
	if (girl->has_trait("No Arms"))						{ Freak += 60; Dangerous -= 30; }
	if (girl->has_trait("Missing Toe"))					{ Freak += 5; }
	if (girl->has_trait("Missing Toes"))					{ Freak += 10; }
	if (girl->has_trait("One Foot"))						{ Freak += 20; Dangerous -= 10; }
	if (girl->has_trait("No Feet"))						{ Freak += 40; Dangerous -= 20; }
	if (girl->has_trait("One Leg"))						{ Freak += 40; Dangerous -= 20; }
	if (girl->has_trait("No Legs"))						{ Freak += 60; Dangerous -= 30;  Tall -= 20; Short += 10; }
	if (girl->has_trait("Clipped Tendons"))				{ Freak += 40; Dangerous -= 20; CuteGirl -= 10; Elegant -= 50; Tall -= 5; Short += 5; }
	if (girl->has_trait("No Clit"))						{ Freak += 5; Sexy -= 10; }
	if (girl->has_trait("Missing Teeth"))				{ Freak += 5; Elegant -= 10; Dangerous += 10; Sexy -= 5; }
	if (girl->has_trait("No Teeth"))						{ Freak += 40; Elegant -= 40; Dangerous -= 5; Sexy -= 15; }
	if (girl->has_trait("No Tongue"))					{ Freak += 20; Dangerous -= 5; }
	if (girl->has_trait("Missing Nipple"))				{ Freak += 20; Dangerous += 10; Sexy -= 10; }
	if (girl->has_trait("No Nipples"))					{ Freak += 20; NonHuman += 10; }
	if (girl->has_trait("Cyclops"))						{ CuteGirl -= 20; Sexy -= 10; Freak += 30; }


	/****** Magic Traits ******/
	if (girl->has_trait("Psychic"))						{ Dangerous += 10; Nerd += 10; NonHuman += 10; Freak += 10; }
	if (girl->has_trait("Strong Magic"))					{ Dangerous += 20; Nerd += 5; NonHuman += 5; Freak += 20; }
	if (girl->has_trait("Muggle"))						{ Dangerous -= 5; Nerd += 5; }
	if (girl->has_trait("Weak Magic"))					{ Dangerous += 5; Freak += 5; }
	if (girl->has_trait("Powerful Magic"))				{ Dangerous += 25; Nerd += 15; NonHuman += 10; Freak += 35; }
	if (girl->has_trait("Natural Pheromones"))			{ NonHuman += 10; Sexy += 40; }  // they'll see her as far more sexy for this
	if (girl->has_trait("Flight"))						{ Dangerous += 15; NonHuman += 20; Freak += 20; }


	/****** Sexual Traits ******/
	// Performance
	if (girl->has_trait("Strong Gag Reflex"))			{ Elegant += 20; Sexy -= 20; Freak += 5;	Lolita += 20; }
	if (girl->has_trait("Gag Reflex"))					{ Elegant += 10; Sexy -= 10; Freak += 0;	Lolita += 10; }
	if (girl->has_trait("No Gag Reflex"))				{ Elegant -= 10; Sexy += 10; Freak += 15;	Lolita -= 5; }
	if (girl->has_trait("Deep Throat"))					{ Elegant -= 20; Sexy += 20; Freak += 30;	Lolita -= 10; }
	if (girl->has_trait("Slow Orgasms"))					{ CuteGirl -= 5; Cool -= 5; Elegant += 5; Sexy -= 10; Freak += 5; }
	if (girl->has_trait("Fake Orgasm Expert"))			{ Sexy += 5; }
	if (girl->has_trait("Fast Orgasms"))					{ Cool += 10; Sexy += 30; }
	if (girl->has_trait("Good Kisser"))					{ Cool += 10; Sexy += 20; }

	if (girl->has_trait("Dominatrix"))					{ Dangerous += 30; Elegant += 5; Freak += 40; }
	if (girl->has_trait("Nimble Tongue"))				{ Sexy += 25; }
	if (girl->has_trait("Open Minded"))					{ Sexy += 30; Cool += 20; Elegant -= 10; }
	if (girl->check_virginity())						{ Lolita += 15; }//Plus 50 for begin a virgin?  Seems odd to me so changed it CRAZY


	/****** Social Traits ******/
	//Types
	if (girl->has_trait("Aggressive"))					{ CuteGirl -= 15; Dangerous += 20; Lolita -= 5; Elegant -= 10; Freak += 10; }
	if (girl->has_trait("Adventurer"))					{ Dangerous += 20; Cool += 10; Nerd -= 20; Elegant -= 5; }
	if (girl->has_trait("Charismatic"))					{ Elegant += 30; Sexy += 30; Freak -= 20; }
	if (girl->has_trait("Charming"))						{ Elegant += 20; Sexy += 20; Freak -= 15; }
	if (girl->has_trait("Clumsy"))						{ CuteGirl += 10; Dangerous -= 20; Cool -= 10; Nerd += 20; Freak += 5; }
	if (girl->has_trait("Cool Person"))					{ Dangerous += 5; Cool += 60; Nerd -= 10; }
	if (girl->has_trait("Cute"))							{ CuteGirl += 60; Lolita += 20; SmallBoobs += 5; }
	if (girl->has_trait("Elegant"))						{ Dangerous -= 30; Nerd -= 20; NonHuman -= 20; Elegant += 60; Freak -= 30; }
	if (girl->has_trait("Lolita"))						{ BigBoobs -= 30; CuteGirl += 30; Dangerous -= 5; Lolita += 60; SmallBoobs += 15; }
	if (girl->has_trait("Manly"))						{ CuteGirl -= 15; Dangerous += 5; Elegant -= 20; Sexy -= 20; NiceFigure -= 20; SmallBoobs += 10; Freak += 5; Lolita -= 10; }
	if (girl->has_trait("Meek"))							{ CuteGirl += 15; Dangerous -= 30; Cool -= 30; Nerd += 30; Lolita += 10; }
	if (girl->has_trait("MILF"))							{ Freak += 15; Lolita -= 50; }
	if (girl->has_trait("Nerd"))							{ CuteGirl += 10; Dangerous -= 30; Cool -= 30; Nerd += 60; SmallBoobs += 5; }
	if (girl->has_trait("Nervous"))						{ CuteGirl += 10; Nerd += 15; }
	if (girl->has_trait("Optimist"))						{ Elegant += 5; }
	if (girl->has_trait("Pessimist"))					{ Elegant -= 5; }
	if (girl->has_trait("Princess"))						{ Elegant += 40; Sexy += 20; Freak -= 15; }
	if (girl->has_trait("Queen"))						{ Elegant += 60; Sexy += 20; Freak -= 15; }
	if (girl->has_trait("Sexy Air"))						{ Cool += 5; Elegant -= 5; Sexy += 10; }
	if (girl->has_trait("Audacity"))						{ Dangerous += 20; Cool += 25; Nerd -= 10; }
	if (girl->has_trait("Brawler"))						{ Dangerous += 30; Freak += 10; }
	if (girl->has_trait("Dojikko"))						{ CuteGirl += 20; Dangerous -= 20; Nerd += 10; Freak += 5; }
	if (girl->has_trait("Exhibitionist"))				{ CuteGirl += 20; Freak += 20; Sexy += 10; }
	if (girl->has_trait("Idol"))							{ Cool += 30; CuteGirl += 10; }
	if (girl->has_trait("Noble"))						{ Elegant += 15; Sexy += 5; Freak -= 5; }
	if (girl->has_trait("Shy"))							{ Cool -= 20; Nerd += 10; }
	if (girl->has_trait("Slut"))							{ Sexy += 25; Cool += 15; Elegant -= 30; }
	if (girl->has_trait("Social Drinker"))				{ Elegant -= 5; Cool += 15; Sexy += 10; }
	if (girl->has_trait("Tomboy"))						{ CuteGirl -= 15; Elegant -= 20; Sexy -= 10; SmallBoobs += 10; Freak += 5; Lolita -= 10; }


	/****** Mental Traits ******/
	//Learning
	if (girl->has_trait("Quick Learner"))				{ Cool -= 20; Nerd += 30; }
	if (girl->has_trait("Slow Learner"))					{ CuteGirl += 10; Cool += 10; Nerd -= 20; }
	if (girl->has_trait("Retarded"))						{ NonHuman += 2; Freak += 45; }
	if (girl->has_trait("Bimbo"))						{ CuteGirl += 10; Cool += 10; Nerd -= 20; }
	if (girl->has_trait("Idiot Savant"))					{ Freak += 10; Nerd += 20; Cool -= 30; Sexy -= 20; }

	//Weak Mind
	if (girl->has_trait("Mind Fucked"))					{ CuteGirl -= 60; Dangerous -= 60; Cool -= 60; Nerd -= 60; Elegant -= 60; Sexy -= 60; Freak += 40; }
	if (girl->has_trait("Broken Will"))					{ Cool -= 40; Nerd -= 40; Elegant -= 40; Sexy -= 40; Freak += 40; }
	if (girl->has_trait("Dependant"))					{ CuteGirl += 5; Dangerous -= 20; Cool -= 5; Nerd += 5; Elegant -= 20; Sexy -= 20; Freak += 10; }

	//Strong Mind
	if (girl->has_trait("Merciless"))					{ CuteGirl -= 20; Dangerous += 20; Nerd -= 10; Elegant -= 5; Lolita -= 10; }
	if (girl->has_trait("Fearless"))						{ Dangerous += 20; Cool += 15; Nerd -= 10; Elegant -= 10; Lolita -= 5; }
	if (girl->has_trait("Iron Will"))					{ Dangerous += 10; Cool += 10; Nerd -= 5; Elegant -= 10; }

	//Messed up mind
	if (girl->has_trait("Twisted"))						{ CuteGirl -= 40; Dangerous += 30; Elegant -= 30; Sexy -= 20; Freak += 40; }
	if (girl->has_trait("Masochist"))					{ CuteGirl -= 10; Nerd -= 10; CuteGirl -= 15; Dangerous += 10; Elegant -= 10; Freak += 30; }
	if (girl->has_trait("Sadistic"))						{ CuteGirl -= 20; Dangerous += 15; Nerd -= 10; Elegant -= 30; Sexy -= 10; Freak += 30; }
	if (girl->has_trait("Tsundere"))						{ Dangerous += 5; Cool += 5; Nerd -= 5; Elegant -= 20; Freak += 10; }
	if (girl->has_trait("Yandere"))						{ Dangerous += 5; Cool += 5; Nerd -= 5; Elegant -= 20; Freak += 10; }

	// Addictions
	if (girl->has_trait("Shroud Addict"))				{ Dangerous += 5; Cool += 15; Nerd -= 10; Elegant -= 20; Sexy -= 20; Freak += 10; }
	if (girl->has_trait("Fairy Dust Addict"))			{ Dangerous += 10; Cool += 20; Nerd -= 15; Elegant -= 25; Sexy -= 25; Freak += 15; }
	if (girl->has_trait("Viras Blood Addict"))			{ Dangerous += 15; Cool += 25; Nerd -= 20; Elegant -= 30; Sexy -= 30; Freak += 20; }
	if (girl->has_trait("Nymphomaniac"))					{ Sexy += 15; Freak += 20; Elegant -= 5; } //kind of an addiction!
	if (girl->has_trait("Chaste"))						{ Sexy -= 10; Freak += 10; Elegant += 5; Nerd += 5; }
	if (girl->has_trait("Smoker"))						{ Elegant -= 5; Cool += 5; }
	if (girl->has_trait("Alchoholic"))					{ Elegant -= 15; Cool += 5; Dangerous += 5; }
	if (girl->has_trait("Cum Addict"))					{ Sexy += 10; Freak += 20; Elegant -= 20; }


	/****** Job Skills ******/
	if (girl->has_trait("Assassin"))						{ Dangerous += 25; Cool += 15; Nerd -= 25; Freak += 10; }
	if (girl->has_trait("Actress"))						{ Sexy += 10; Cool += 10; }
	if (girl->has_trait("Porn Star"))					{ Sexy += 40; Cool += 40; Elegant -= 40; }
	//	if (girl->has_trait("Chef"))						{}								// Not visible to customer
	if (girl->has_trait("City Girl"))					{ Cool += 5; Freak -= 5; }
	if (girl->has_trait("Farmer"))						{ Cool -= 5; Elegant -= 5; }
	if (girl->has_trait("Hunter"))						{ Dangerous += 10; Cool += 5; Sexy += 2; NiceFigure += 2; Freak += 5; }
	if (girl->has_trait("Country Gal"))					{ Sexy += 10; Cool -= 10; }		//simple country charm!
	if (girl->has_trait("Director"))						{ Elegant += 10; }				//Composure under pressure
	if (girl->has_trait("Doctor"))						{ Elegant += 5; }	//rubber gloves
	if (girl->has_trait("Farmers Daughter"))				{ Sexy += 5; Cool -= 5; }		//simple country charm!
	if (girl->has_trait("Former Official"))				{ Elegant += 10; Freak += 5; } //F* the government!
	if (girl->has_trait("Handyman"))						{ Nerd += 10; }
	if (girl->has_trait("Heroine"))						{ Dangerous += 50; Sexy += 10; Cool += 10; }
	if (girl->has_trait("Homeless"))						{ Dangerous += 10; Elegant -= 10; Cool -= 5; }
	if (girl->has_trait("Maid"))							{ Sexy += 10; Freak -= 10; Elegant += 10; Cool -= 5; }
	if (girl->has_trait("Mixologist"))					{ Nerd += 5; Cool += 5; }
	if (girl->has_trait("Priestess"))					{ Freak += 30; Elegant += 10; Sexy -= 5; }
	if (girl->has_trait("Singer"))						{ Elegant += 10; Sexy += 5; Cool += 5; }
	//	if (girl->has_trait("Tone Deaf"))				{}								// Not visible to customer
	if (girl->has_trait("Teacher"))						{ Elegant += 5; Nerd += 20; }
	if (girl->has_trait("Waitress"))						{ Elegant += 5; }


	/****** Species Traits ******/
	if (girl->has_trait("Cat Girl"))						{ CuteGirl += 20; NonHuman += 60; Freak += 5; }
	if (girl->has_trait("Construct"))					{ Dangerous += 10; NonHuman += 60; Freak += 20; }
	if (girl->has_trait("Angel"))						{ CuteGirl += 10; NonHuman += 60; Freak += 5; }
	if (girl->has_trait("Demon"))						{ Dangerous += 10; NonHuman += 60; Freak += 5; }
	if (girl->has_trait("Different Colored Eyes"))		{ NonHuman += 5; Freak += 10; }
	if (girl->has_trait("Futanari"))						{ CuteGirl -= 15; NonHuman += 10; Freak += 30; }
	if (girl->has_trait("Half-Construct"))				{ Dangerous += 5; NonHuman += 20; Freak += 20; }
	if (girl->has_trait("Incorporeal"))					{ NonHuman += 60; Freak += 40; }
	if (girl->has_trait("Not Human"))					{ NonHuman += 60; Freak += 10; }
	if (girl->has_trait("Shape Shifter"))				{ NonHuman += 35; Sexy += 20; NiceFigure += 40; Freak += 40; }
	if (girl->has_trait("Strange Eyes"))					{ NonHuman += 10; Freak += 15; }

	if (girl->has_trait("Skeleton"))						{ BigBoobs -= 1000; SmallBoobs -= 1000; CuteGirl -= 1000; Dangerous += 1000; Cool += 10; NonHuman += 1000; Elegant -= 100; Sexy -= 100; NiceFigure -= 1000; NiceArse -= 1000; Freak += 1000; }
	if (girl->has_trait("Undead"))						{ Dangerous += 10; Cool += 5; NonHuman += 10; Sexy -= 5; Freak += 20; }
	if (girl->has_trait("Vampire"))						{ CuteGirl += 10; Dangerous += 10; Cool += 10; NonHuman += 10; Elegant += 10; Sexy += 10; Freak += 10; }
	if (girl->has_trait("Zombie"))						{ NonHuman += 100; Freak += 100; CuteGirl -= 50; Dangerous += 100; Elegant -= 50; }

	if (girl->has_trait("Battery Operated"))				{ NonHuman += 20; Freak += 5; }
	if (girl->has_trait("Canine"))						{ NonHuman += 60; CuteGirl += 10; Freak += 15; }
	if (girl->has_trait("Cow Girl"))						{ NonHuman += 60; CuteGirl -= 20; Freak += 25; BigBoobs += 20; SmallBoobs -= 20; }
	if (girl->has_trait("Dryad"))						{ NonHuman += 50; CuteGirl -= 10; Freak += 20; }
	if (girl->has_trait("Egg Layer"))					{ NonHuman += 30; Sexy -= 20; Freak += 40; }
	if (girl->has_trait("Elf"))							{ NonHuman += 30; Elegant += 10; Lolita += 10; Freak += 10; }
	if (girl->has_trait("Equine"))						{ NonHuman += 80; Freak += 40; }
	if (girl->has_trait("Fallen Goddess"))				{ NonHuman += 20; Elegant += 30; Freak -= 10; Sexy += 25; }
	if (girl->has_trait("Furry"))						{ NonHuman += 40; Elegant -= 5; Sexy += 15; Freak += 30; }
	if (girl->has_trait("Goddess"))						{ NonHuman += 20; Elegant += 50; Freak -= 20; Sexy += 20; }
	if (girl->has_trait("Half-Breed"))					{ NonHuman += 20; Freak += 20; }
	if (girl->has_trait("Playful Tail"))					{ NonHuman += 10; Freak += 25; }
	if (girl->has_trait("Prehensile Tail"))				{ NonHuman += 20; Freak += 40; }
	if (girl->has_trait("Reptilian"))					{ NonHuman += 50; Freak += 20; Dangerous += 10; CuteGirl -= 20; }
	if (girl->has_trait("Slitherer"))					{ NonHuman += 60; Freak += 25; CuteGirl -= 30; }
	if (girl->has_trait("Solar Powered"))				{ NonHuman += 20; Freak += 5; }
	if (girl->has_trait("Succubus"))						{ NonHuman += 40; Freak += 30; Sexy += 30; }
	if (girl->has_trait("Wings"))						{ NonHuman += 50; Freak += 20; }


	// Other Invisible Traits (which do not affect customer decision, but are here for completeness)
#if 0
	//Fertility
	if (girl->has_trait("Sterile"))					{}
	if (girl->has_trait("Fertile"))					{}
	if (girl->has_trait("Broodmother"))				{}
	// Sexuality
	if (girl->has_trait("Straight"))					{}
	if (girl->has_trait("Bisexual"))					{}
	if (girl->has_trait("Lesbian"))					{}
	//STDs
	if (girl->has_trait("AIDS"))						{}
	if (girl->has_trait("Chlamydia"))				{}
	if (girl->has_trait("Herpes"))					{}
	if (girl->has_trait("Syphilis"))					{}
	//Other
	if (girl->has_trait("Has Boy Friend"))			{}
	if (girl->has_trait("Has Girl Friend"))			{}
	if (girl->has_trait("Your Daughter"))			{}
	if (girl->has_trait("Your Wife"))				{}

	//Temporary Traits
	//Have ignored these four temp traits for now - not yet familiar with how/when applied
	if (girl->has_trait("Botox Treatment"))			{}
	if (girl->has_trait("Bruises"))					{}
	if (girl->has_trait("Demon Possessed"))			{}
	if (girl->has_trait("Out-Patient Surgery"))		{}
	if (girl->has_trait("Spirit Possessed"))			{}
	if (girl->has_trait("Kidnapped"))				{}
	if (girl->has_trait("Emprisoned Customer"))		{}

#endif	// end unused traits


	if (BigBoobs > SmallBoobs)
	{
		if (BigBoobs > 50)			girl->m_FetishTypes |= (1 << FETISH_BIGBOOBS);
	}
	else
	{
		if (SmallBoobs > 50)		girl->m_FetishTypes |= (1 << FETISH_SMALLBOOBS);
	}
	if (CuteGirl > 50)				girl->m_FetishTypes |= (1 << FETISH_CUTEGIRLS);
	if (Dangerous > 50)				girl->m_FetishTypes |= (1 << FETISH_DANGEROUSGIRLS);
	if (Cool > 50)					girl->m_FetishTypes |= (1 << FETISH_COOLGIRLS);
	if (Nerd > 50)					girl->m_FetishTypes |= (1 << FETISH_NERDYGIRLS);
	if (NonHuman > 50)				girl->m_FetishTypes |= (1 << FETISH_NONHUMAN);
	if (Lolita > 50)				girl->m_FetishTypes |= (1 << FETISH_LOLITA);
	if (Elegant > 50)				girl->m_FetishTypes |= (1 << FETISH_ELEGANT);
	if (Sexy > 50)					girl->m_FetishTypes |= (1 << FETISH_SEXY);
	if (NiceFigure > 50)			girl->m_FetishTypes |= (1 << FETISH_FIGURE);
	if (NiceArse > 50)				girl->m_FetishTypes |= (1 << FETISH_ARSE);
	if (Freak > 50)					girl->m_FetishTypes |= (1 << FETISH_FREAKYGIRLS);
	if (girl->has_trait("Futanari"))	girl->m_FetishTypes |= (1 << FETISH_FUTAGIRLS);
	if (Tall > 50)					girl->m_FetishTypes |= (1 << FETISH_TALLGIRLS);
	if (Short > 50)					girl->m_FetishTypes |= (1 << FETISH_SHORTGIRLS);
	if (Fat > 50)					girl->m_FetishTypes |= (1 << FETISH_FATGIRLS);
}

bool cGirls::CheckGirlType(sGirl* girl, int type)
{
	if (type == FETISH_TRYANYTHING || girl->m_FetishTypes&(1 << type))	return true;
	return false;
}

sGirl *sGirl::run_away()
{
	m_RunAway = 6;		// player has 6 weeks to retreive
	m_NightJob = m_DayJob = JOB_RUNAWAY;
	g_Brothels.AddGirlToRunaways(this);
	return 0;
}

void cGirls::CalculateAskPrice(sGirl* girl, bool vari)
{
	int askPrice = (int)(((girl->beauty() + girl->charisma()) / 2)*0.6f);	// Initial price
	askPrice += girl->confidence() / 10;		// their confidence will make them think they are worth more
	askPrice += girl->intelligence() / 10;		// if they are smart they know they can get away with a little more
	askPrice += girl->fame() / 2;				// And lastly their fame can be quite useful too
	if (girl->level() > 0)	askPrice += girl->level() * 10;  // MYR: Was * 1

	if (askPrice > 100) askPrice = 100;
	if (askPrice < 0) askPrice = 0;
	SetStat(girl, STAT_ASKPRICE, askPrice);
	int baseask = askPrice;
	if (vari)	// vari is used when calculating for jobs so she can charge customers a little more
	{
		// `J` changed so there is better variation but with a chance of negatives
#if 0	// old code
		int minVariance = 0 - (g_Dice % 10) + 1;
		int maxVariance = (g_Dice % 10) + 1;
		int variance = ((g_Dice % 10) + maxVariance) - minVariance;
		askPrice += variance;
#else	// new code

		int lowend = 0;	int highend = 0;
		// dumb girls will charge less, smart girls will charge more
		lowend	+= (girl->intelligence() / 10) - 7;	// -7 to +3
		highend	+= (girl->intelligence() / 8);		// +0 to +12
		// timid girls will charge less, confident girls will charge more
		lowend	+= (girl->confidence() / 10) - 7;	// -7 to +3
		highend	+= (girl->confidence() / 7);		// +0 to +14

		askPrice += g_Dice.bell(lowend, highend);

		if (askPrice < 1) askPrice = 1;
		girl->m_Stats[STAT_ASKPRICE] = askPrice;

		if (cfg.debug.log_extradetails())
		{
			g_LogFile.ss() << "\n" << girl->m_Realname << " Ask Price: " << baseask << " : " << askPrice << "\n"; g_LogFile.ssend();
		}

#endif
	}

}

sRandomGirl* cGirls::random_girl_at(int n)
{
	int i;
	sRandomGirl* current = m_RandomGirls;
	/*
	*	if we try and get a random girl template
	*	that's numbered higher than the number of
	*	templates... well we're not going to find it.
	*
	*	So let's cut to the chase
	*/
	if (n >= m_NumRandomGirls)	return 0;
	// loop through the linked list n times
	for (i = 0; i < n; i++)
	{
		current = current->m_Next;
		/*
		*		current should only be null at the end
		*		of the chain. Which we shouldn't get to
		*		since we know n < m_NumRandomGirls from
		*		the above.
		*
		*		so if it IS null, we have an integrity error
		*		in the pointer chain.
		*
		*		is it too late to rewrite this using vector?
		*/
		if (current == 0)
		{
			g_LogFile.os() << "broken chain in cGirls::random_girl_at" << endl;
			return 0;
		}
	}
	return current;		// and there we (hopefully) are
}

sGirl* cGirls::CreateRandomGirl(int age, bool addToGGirls, bool slave, bool undead, bool Human0Monster1,
								bool childnaped, bool arena, bool daughter, bool isdaughter, string findbyname)
{
	sRandomGirl* current = 0;
	// 1. The most direct check is to try to find a girl by name.
	if (findbyname != "")
	{
		current = find_random_girl_by_name(findbyname, 0);
	}

	// 2. Next we see if you are looking for your own daughter
	if (current == 0 && daughter &&	m_NumRandomYourDaughterGirls > 0)
	{
		bool girlfound = false;
		bool monstergirl = Human0Monster1;
		if (m_NumNonHumanRandomYourDaughterGirls < 1)
			monstergirl = false;
		// if there are no monster girls we will accept a human

		int i = 0;
		int random_girl_index = g_Dice%m_NumRandomGirls;	// pick a number between 0 and m_NumRandomGirls as the stating point
		while (i < (int)m_NumRandomGirls)					// loop until we find a human/non-human template as required
		{
			current = random_girl_at(random_girl_index);
			if (current != 0 && current->m_YourDaughter)
			{
				if (monstergirl)
				{
					if (current->m_Human == 0)
					{
						girlfound = true;
						break;	// nonhuman
					}
				}
				else
				{
					if (current->m_Human == 1)
					{
						girlfound = true;
						break;
					}
				}
			}
			i++; random_girl_index++;	// `J` check all random girls then if not found return the last random girl checked
			if (random_girl_index >(int)m_NumRandomGirls) random_girl_index = 0;
		}
		if (!girlfound) current = 0;	// if no your daughter was found allow for random check.
	}

	if (current){}	// if the above checks succeded - skip random check
	else if (m_NumRandomGirls == 0)	current = 0;
	else
	{
		int i = 0;
		int random_girl_index = g_Dice%m_NumRandomGirls;	// pick a number between 0 and m_NumRandomGirls as the stating point
		while (i < (int)m_NumRandomGirls)	// loop until we find a human/non-human template as required
		{
			current = random_girl_at(random_girl_index);
			if (current != 0 && Human0Monster1 == (current->m_Human == 0))				// test for humanity - or lack of it as the case may be
			{
				break;
			}
			//	She's either human when we wanted non-human or non-human when we wanted human
			//	Either way, try again...
			i++; random_girl_index++;	// `J` check all random girls then if not found return the last random girl checked
			if (random_girl_index >m_NumRandomGirls) random_girl_index = 0;
		}
		if (!current)
		{
			g_LogFile.os() << "There was an error in CreateRandomGirl code, using hard coded random girl" << endl;
		}
	}

	if (!current)	// `J` added hard coded random girl if there are no rgirlsx files
	{
		current = new sRandomGirl();
		current->m_newRandom = false;
		current->m_Desc = "Hard Coded Random Girl\n(The game did not find any .rgirlsx files)\n(or there was an error in an .rgirlsx file)";
		current->m_Name = "Default";
		current->m_Human = (Human0Monster1 == 0);
		current->m_Arena = arena;
		current->m_YourDaughter = daughter;

		for (int i = 0; i < NUM_STATS; i++)
		{
			if (age != 0 && i == STAT_AGE)	{ current->m_MinStats[i] = age;	current->m_MaxStats[i] = age; }

			if (arena)
			{
				if (i == STAT_CHARISMA)		{ current->m_MinStats[i] += 0;	current->m_MaxStats[i] += 10; }
				if (i == STAT_CONSTITUTION)	{ current->m_MinStats[i] += 20;	current->m_MaxStats[i] += 20; }
				if (i == STAT_INTELLIGENCE)	{ current->m_MinStats[i] += 10;	current->m_MaxStats[i] += 10; }
				if (i == STAT_CONFIDENCE)	{ current->m_MinStats[i] += 10;	current->m_MaxStats[i] += 20; }
				if (i == STAT_MANA)			{ current->m_MinStats[i] += 10;	current->m_MaxStats[i] += 40; }
				if (i == STAT_AGILITY)		{ current->m_MinStats[i] += 10;	current->m_MaxStats[i] += 20; }
				if (i == STAT_OBEDIENCE)	{ current->m_MinStats[i] -= 10;	current->m_MaxStats[i] -= 10; }
				if (i == STAT_SPIRIT)		{ current->m_MinStats[i] += 10;	current->m_MaxStats[i] += 20; }
			}
			if (Human0Monster1)
			{
				if (i == STAT_CHARISMA)		{ current->m_MinStats[i] -= 10;	current->m_MaxStats[i] += 10; }
				if (i == STAT_CONSTITUTION)	{ current->m_MinStats[i] -= 10;	current->m_MaxStats[i] += 20; }
				if (i == STAT_INTELLIGENCE)	{ current->m_MinStats[i] -= 10;	current->m_MaxStats[i] += 10; }
				if (i == STAT_CONFIDENCE)	{ current->m_MinStats[i] -= 10;	current->m_MaxStats[i] += 20; }
				if (i == STAT_MANA)			{ current->m_MinStats[i] -= 10;	current->m_MaxStats[i] += 40; }
				if (i == STAT_AGILITY)		{ current->m_MinStats[i] -= 10;	current->m_MaxStats[i] += 10; }
				if (i == STAT_OBEDIENCE)	{ current->m_MinStats[i] -= 10;	current->m_MaxStats[i] += 10; }
				if (i == STAT_SPIRIT)		{ current->m_MinStats[i] -= 10;	current->m_MaxStats[i] += 10; }
			}
		}
		// normalize stats
		for (int i = 0; i < NUM_STATS; i++)
		{
			int min = 0, max = 100;
			/* */if (i == STAT_AGE) min = 18;	// `J` Legal Note: 18 is the Legal Age of Majority for the USA where I live
			else if (i == STAT_EXP) max = 32000;
			else if (i == STAT_LEVEL) max = 255;
			else if (i == STAT_PCLOVE || i == STAT_PCFEAR || i == STAT_PCHATE || i == STAT_MORALITY ||
				i == STAT_REFINEMENT || i == STAT_DIGNITY || i == STAT_LACTATION) min = -100;

			if (current->m_MinStats[i] < min) current->m_MinStats[i] = min; if (current->m_MinStats[i] > max) current->m_MinStats[i] = max;
			if (current->m_MaxStats[i] < min) current->m_MaxStats[i] = min; if (current->m_MaxStats[i] > max) current->m_MaxStats[i] = max;
			if (current->m_MinStats[i] > current->m_MaxStats[i])
			{
				int a = current->m_MinStats[i];
				current->m_MinStats[i] = current->m_MaxStats[i];
				current->m_MaxStats[i] = a;
			}
		}

		for (int i = 0; i < NUM_SKILLS; i++)
		{	// base for all skills is 0-30
			if (arena)
			{
				if (i == SKILL_MAGIC || i == SKILL_COMBAT)
				{
					current->m_MinSkills[i] += 50; current->m_MaxSkills[i] += 100;
				}
				if (i == SKILL_ANAL || i == SKILL_NORMALSEX || i == SKILL_GROUP || i == SKILL_LESBIAN ||
					i == SKILL_STRIP || i == SKILL_ORALSEX || i == SKILL_TITTYSEX || i == SKILL_HANDJOB || SKILL_FOOTJOB)
				{
					current->m_MaxSkills[i] -= 10; current->m_MaxSkills[i] -= 20;
				}
				if (i == SKILL_BDSM || i == SKILL_BEASTIALITY)				current->m_MaxSkills[i] -= 10;
				if (i == SKILL_PERFORMANCE || i == SKILL_ANIMALHANDLING)	current->m_MaxSkills[i] += 20;
				if (i == SKILL_MEDICINE)									current->m_MaxSkills[i] += 10;
			}
			if (Human0Monster1)
			{
				if (i == SKILL_MAGIC)			{ current->m_MaxSkills[i] -= 10; current->m_MaxSkills[i] += 40; }
				if (i == SKILL_COMBAT)			{ current->m_MaxSkills[i] -= 10; current->m_MaxSkills[i] += 40; }
				if (i == SKILL_SERVICE)			{ current->m_MaxSkills[i] -= 20; current->m_MaxSkills[i] -= 10; }
				if (i == SKILL_BEASTIALITY)		{ current->m_MaxSkills[i] += 20; }
				if (i == SKILL_CRAFTING || i == SKILL_HERBALISM || i == SKILL_FARMING)
				{
					current->m_MaxSkills[i] -= 10; current->m_MaxSkills[i] += 10;
				}
				if (i == SKILL_ANIMALHANDLING)	{ current->m_MaxSkills[i] -= 10; current->m_MaxSkills[i] += 40; }
			}
			// normalize
			if (current->m_MinSkills[i] < 0)current->m_MinSkills[i] = 0; if (current->m_MinSkills[i] > 100)current->m_MinSkills[i] = 100;
			if (current->m_MaxSkills[i] < 0)current->m_MaxSkills[i] = 0; if (current->m_MaxSkills[i] > 100)current->m_MaxSkills[i] = 100;
			if (current->m_MinSkills[i] > current->m_MaxSkills[i]) { int a = current->m_MinSkills[i]; current->m_MinSkills[i] = current->m_MaxSkills[i]; current->m_MaxSkills[i] = a; }
		}

		// `J` When adding new traits, search for "J-Add-New-Traits"  :  found in >> CreateRandomGirl > hardcoded rgirl
		current->m_NumTraits = 0;
		current->m_NumTraitNames = 0;
		for(const auto& trait : g_Traits.all_traits())
        {
		    if(current->m_NumTraits >= MAXNUM_TRAITS - 10)
		        break;

            int c = trait->random_chance();
            string test = trait->name();

			// first check if it is a daughter or Human0Monster1 trait
			if (test == "Your Daughter")	c = (daughter) ? 100 : 0;
			if (test == "Not Human")		c = (Human0Monster1) ? 100 : 0;
			if (test == "Incorporeal")		c = (Human0Monster1) ? c+1 : 0;
			if (test == "Futanari")			c = (Human0Monster1) ? c+5 : 0;
			if (test == "Construct")		c = (Human0Monster1) ? c+5 : 0;
			if (test == "Half-Construct")	c = (Human0Monster1) ? c+10 : 0;
			if (test == "Strange Eyes")		c = (Human0Monster1) ? c+10 : 0;
			if (test == "Cat Girl")			c = (Human0Monster1) ? c+10 : 0;
			if (test == "Demon")			c = (Human0Monster1) ? c+10 : 0;
			if (test == "Shape Shifter")	c = (Human0Monster1) ? c+1 : 0;

			// virgin is handled differently
			if (test == "Virgin")			c = (age < 21) ? max(min(100 - ((age - 17) * 10), 100), 0) : max(70 - ((age - 20) * 2), 0);
			// so under 18 is 100%, 18=90%, 19=80%, 20=70%, then 21=68%, 22=66% and so on until age 55 where it is 0%


			if (c != -1 && arena)	// if .traitsx has m_RandomChance in it adjust some traits for arena girls
			{
				// some traits are more or less common for arena girls
				if (test == "Adventurer")			c += 30;
				if (test == "Fearless")				c += 30;
				if (test == "Iron Will")			c += 30;
				if (test == "Small Scars")			c += 20;
				if (test == "Cool Scars")			c += 20;
				if (test == "Horrific Scars")		c += 20;
				if (test == "Strong")				c += 20;
				if (test == "Strong Magic")			c += 20;
				if (test == "Aggressive")			c += 20;
				if (test == "Great Figure")			c += 20;
				if (test == "Sadistic")				c += 20;
				if (test == "Tsundere")				c += 20;
				if (test == "Yandere")				c += 20;
				if (test == "Fleet of Foot")		c += 20;
				if (test == "Tough")				c += 20;
				if (test == "Merciless")			c += 20;
				if (test == "Assassin")				c += 10;
				if (test == "Great Arse")			c += 10;
				if (test == "Masochist")			c += 10;
				if (test == "Manly")				c += 10;
				if (test == "Twisted")				c += 10;
				if (test == "Cool Person")			c += 5;
				if (test == "One Eye")				c += 5;
				if (test == "Eye Patch")			c += 5;
				if (test == "Malformed")			c += 5;
				if (test == "Chaste")				c += 5;

				if (test == "Nymphomaniac")			c -= 5;
				if (test == "Mind Fucked")			c += -5;
				if (test == "Retarded")				c += -5;
				if (test == "Dependant")			c += -10;
				if (test == "Fragile")				c += -10;
				if (test == "Cute")					c += -10;
				if (test == "Lolita")				c += -10;
				if (test == "Nervous")				c += -10;
				if (test == "Elegant")				c += -10;
				if (test == "Broken Will")			c += -10;
				if (test == "Nerd")					c += -15;
				if (test == "Clumsy")				c += -15;
				if (test == "Meek")					c += -20;

				if (c < 0)	c = 0;
			}

			// all other traits have a 1 in 3 chance of making the cut
			if (c == -1 && g_Dice % 3 == 1)
			{
				// now, certain traits are more or less common than others
				if (test == "Shroud Addict")		c = 5;
				if (test == "Fairy Dust Addict")	c = 5;
				if (test == "Viras Blood Addict")	c = 5;
				if (test == "AIDS")					c = 2;
				if (test == "Chlamydia")			c = 3;
				if (test == "Syphilis")				c = 4;
				if (test == "Herpes")				c = 5;
				if (test == "Queen")				c = 1;
				if (test == "Princess")				c = 5;
				if (test == "Sterile")				c = 5;
				if (test == "Incest")				c = 5;

				// some traits are more or less common for arena girls
				if (test == "Small Scars")			c = (arena) ? 40 : 20;
				if (test == "Cool Scars")			c = (arena) ? 40 : 20;
				if (test == "Horrific Scars")		c = (arena) ? 30 : 10;
				if (test == "Cool Person")			c = (arena) ? 25 : 20;
				if (test == "Nerd")					c = (arena) ? 5 : 20;
				if (test == "Clumsy")				c = (arena) ? 5 : 20;
				if (test == "Cute")					c = (arena) ? 10 : 20;
				if (test == "Strong")				c = (arena) ? 40 : 20;
				if (test == "Strong Magic")			c = (arena) ? 30 : 10;
				if (test == "Aggressive")			c = (arena) ? 40 : 20;
				if (test == "Adventurer")			c = (arena) ? 50 : 20;
				if (test == "Assassin")				c = (arena) ? 30 : 20;
				if (test == "Lolita")				c = (arena) ? 10 : 20;
				if (test == "Nervous")				c = (arena) ? 10 : 20;
				if (test == "Elegant")				c = (arena) ? 10 : 20;
				if (test == "Great Figure")			c = (arena) ? 50 : 30;
				if (test == "Great Arse")			c = (arena) ? 40 : 30;
				if (test == "Broken Will")			c = (arena) ? 0 : 5;
				if (test == "Masochist")			c = (arena) ? 20 : 10;
				if (test == "Sadistic")				c = (arena) ? 30 : 10;
				if (test == "Tsundere")				c = (arena) ? 30 : 10;
				if (test == "Yandere")				c = (arena) ? 30 : 10;
				if (test == "Meek")					c = (arena) ? 0 : 10;
				if (test == "Manly")				c = (arena) ? 20 : 10;
				if (test == "Merciless")			c = (arena) ? 30 : 10;
				if (test == "Fearless")				c = (arena) ? 40 : 10;
				if (test == "Iron Will")			c = (arena) ? 40 : 10;
				if (test == "Twisted")				c = (arena) ? 20 : 10;
				if (test == "Dependant")			c = (arena) ? 0 : 10;
				if (test == "Fleet of Foot")		c = (arena) ? 40 : 20;
				if (test == "Tough")				c = (arena) ? 40 : 20;
				if (test == "One Eye")				c = (arena) ? 10 : 5;
				if (test == "Eye Patch")			c = (arena) ? 10 : 5;
				if (test == "Fragile")				c = (arena) ? 0 : 5;
				if (test == "Mind Fucked")			c = (arena) ? 0 : 5;
				if (test == "Malformed")			c = (arena) ? 5 : 5;
				if (test == "Retarded")				c = (arena) ? 2 : 5;

				if (c == -1) c = 1; // set all unlisted to base 1%
			}

			if (c > 0)	// after the checks, if c was set, add the trait to the girl
			{
				if (current->m_NumTraitNames < MAXNUM_TRAITS)	// first 40
				{
                    current->m_Traits[current->m_NumTraits] = trait.get();
					current->m_TraitChance[current->m_NumTraits] = c;
				}
				current->m_TraitNames[current->m_NumTraitNames] = test;
				current->m_TraitChanceB[current->m_NumTraitNames] = c;

				current->m_NumTraitNames++;											// and whack up the trait count.
				current->m_NumTraits++;
			}
		}
	}

	sGirl* newGirl = new sGirl();
	newGirl->m_Next = 0;
	newGirl->m_AccLevel = cfg.initial.girls_accom();
	newGirl->m_States = 0;
	newGirl->m_NumTraits = 0;

	newGirl->m_Desc = current->m_Desc;		// Bugfix.. was populating description with name.
	newGirl->m_Name = current->m_Name.c_str();


	// set all jobs to null
	newGirl->m_DayJob = newGirl->m_NightJob = newGirl->m_YesterDayJob = newGirl->m_YesterNightJob = newGirl->m_PrevDayJob = newGirl->m_PrevNightJob = 255;
	newGirl->m_WorkingDay = newGirl->m_PrevWorkingDay = newGirl->m_SpecialJobGoal = 0;

	newGirl->m_Money = (g_Dice % (current->m_MaxMoney - current->m_MinMoney)) + current->m_MinMoney;	// money

	// skills
	for (u_int i = 0; i < NUM_SKILLS; i++)
	{
		if (current->m_MaxSkills[i] == current->m_MinSkills[i])		newGirl->m_Skills[i] = current->m_MaxSkills[i];
		else if (current->m_MaxSkills[i] < current->m_MinSkills[i])	newGirl->m_Skills[i] = g_Dice % 101;
		else	newGirl->m_Skills[i] = (int)(g_Dice % (current->m_MaxSkills[i] - current->m_MinSkills[i])) + current->m_MinSkills[i];
	}

	// stats
	for (int i = 0; i < NUM_STATS; i++)
	{
		if (current->m_MaxStats[i] == current->m_MinStats[i])		newGirl->m_Stats[i] = current->m_MaxStats[i];
		else if (current->m_MaxStats[i] < current->m_MinStats[i])	newGirl->m_Stats[i] = g_Dice % 101;
		else	newGirl->m_Stats[i] = (g_Dice % (current->m_MaxStats[i] - current->m_MinStats[i])) + current->m_MinStats[i];
	}

	for (int i = 0; i < current->m_NumTraitNames && newGirl->m_NumTraits < MAXNUM_TRAITS; i++)	// add the traits
	{
		if (g_Dice.percent(current->m_TraitChanceB[i]))
		{
			string name = current->m_TraitNames[i];
			if (g_Traits.GetTrait(current->m_Traits[i]->display_name())) // `J` added translation check
			{
				if (name == "Virgin") newGirl->m_Virgin = 1;
				if (!newGirl->has_trait(name))
					newGirl->add_trait(name);
			}
			else
			{
				stringstream ss;
				ss << "cGirls::CreateRandomGirl: ERROR: Trait '" << name << "' from girl template " << current->m_Name << " doesn't exist or is spelled incorrectly.";
				g_MessageQue.AddToQue(ss.str(), COLOR_RED);
			}
		}
	}

	if (current->m_NumItemNames > 0)
	{
		for (int i = 0; i < current->m_NumItemNames && newGirl->m_NumInventory < MAXNUM_GIRL_INVENTORY; i++)
		{
			if (g_Dice.percent(current->m_ItemChanceB[i]))
			{
				sInventoryItem* item = current->m_Inventory[i];
				if (item)
				{
					newGirl->m_Inventory[newGirl->m_NumInventory] = item;
					if (item->m_Type != INVFOOD && item->m_Type != INVMAKEUP)
					{
						g_Girls.EquipItem(newGirl, newGirl->m_NumInventory, false);
					}
					newGirl->m_NumInventory++;
				}
				else
				{
					stringstream ss;
					ss << "cGirls::CreateRandomGirl: ERROR: Item '" << current->m_ItemNames[i] << "' from girl template "
						<< current->m_Name << " doesn't exist or is spelled incorrectly.";
					g_MessageQue.AddToQue(ss.str(), COLOR_RED);
				}
			}
		}
	}

	if (current->m_Human == 0)			newGirl->add_trait("Not Human");
	if (current->m_YourDaughter == 1)	newGirl->add_trait("Your Daughter");

	newGirl->m_Stats[STAT_FAME] = 0;
	if (age != 0)	newGirl->m_Stats[STAT_AGE] = age;
	newGirl->m_Stats[STAT_HEALTH] = 100;
	newGirl->m_Stats[STAT_HAPPINESS] = 100;
	newGirl->m_Stats[STAT_TIREDNESS] = 0;

	if (childnaped)	// this girl has been taken against her will so make her rebelious
	{
		newGirl->add_trait("Kidnapped", max(5, g_Dice.bell(0, 25)));		// 5-25 turn temp trait
		int spirit = g_Dice.bell(50, 125);
		int conf = g_Dice.bell(50, 125);
		int obey = g_Dice.bell(-50, 50);
		int hate = g_Dice.bell(0, 100);

		newGirl->m_Stats[STAT_SPIRIT] = min(100, spirit);
		newGirl->m_Stats[STAT_CONFIDENCE] = min(100, conf);
		newGirl->m_Stats[STAT_OBEDIENCE] = max(0, obey);
		newGirl->m_Stats[STAT_PCHATE] = hate;
	}

	if (newGirl->check_virginity())	// `J` check random girl's virginity
	{
		newGirl->m_Virgin = 1;
		newGirl->add_trait("Virgin");
	}
	else
	{
		newGirl->m_Virgin = 0;
		newGirl->remove_trait("Virgin");
	}
	if (newGirl->m_Stats[STAT_AGE] < 18) newGirl->m_Stats[STAT_AGE] = 18;	// `J` Legal Note: 18 is the Legal Age of Majority for the USA where I live
	if (g_Dice.percent(5))		newGirl->add_trait("Former Addict");
	else
	{
		if (g_Dice.percent(5))		newGirl->add_trait("Smoker");
		if (g_Dice.percent(4))		newGirl->add_trait("Alcoholic");
		if (g_Dice.percent(2))		newGirl->add_trait("Fairy Dust Addict");
		if (g_Dice.percent(1))		newGirl->add_trait("Shroud Addict");
		if (g_Dice.percent(0.5))	newGirl->add_trait("Viras Blood Addict");
	}

	if (daughter)				newGirl->m_Stats[STAT_HOUSE] = 0;	// your daughter gets to keep all she gets
	else if (!slave && !arena)	newGirl->m_Stats[STAT_HOUSE] = cfg.initial.girls_house_perc();	// 60% is the norm
	else newGirl->m_Stats[STAT_HOUSE] = cfg.initial.slave_house_perc();	// 100% is the norm

	// If the girl is a slave or arena.. then make her more obedient.
	if (slave || newGirl->m_States&(1 << STATUS_SLAVE))
	{
		newGirl->m_States |= (1 << STATUS_SLAVE);
		newGirl->m_AccLevel = cfg.initial.slave_accom();
		newGirl->m_Money = 0;
		newGirl->m_Stats[STAT_OBEDIENCE] = min(newGirl->m_Stats[STAT_OBEDIENCE] + 20, 100);
	}
	if (arena || newGirl->m_States&(1 << STATUS_ARENA))
	{
		newGirl->m_States |= (1 << STATUS_ARENA);
		newGirl->m_AccLevel = cfg.initial.slave_accom();
		newGirl->m_Money = 0;
		newGirl->m_Stats[STAT_OBEDIENCE] = min(newGirl->m_Stats[STAT_OBEDIENCE] + 20, 100);
	}
	if (daughter || newGirl->m_States&(1 << STATUS_YOURDAUGHTER))	// `J` if she is your daughter...
	{
		newGirl->m_States |= (1 << STATUS_YOURDAUGHTER);
		newGirl->m_AccLevel = 9;			// pamper her
		newGirl->m_Money = 1000;
		newGirl->m_Stats[STAT_HOUSE] = 0;	// your daughter gets to keep all she gets
		newGirl->add_trait("Your Daughter");
		newGirl->m_Stats[STAT_OBEDIENCE] = max(newGirl->m_Stats[STAT_OBEDIENCE], 80);	// She starts out obedient
		if (newGirl->check_virginity())
		{		// you made sure she stayed pure
			// `J` needs work
		}
	}
	if (isdaughter || newGirl->m_States&(1 << STATUS_ISDAUGHTER))
	{
		newGirl->m_States |= (1 << STATUS_ISDAUGHTER);
		newGirl->m_Money = 0;
		newGirl->m_Stats[STAT_OBEDIENCE] = min(newGirl->m_Stats[STAT_OBEDIENCE] + 20, 100);
	}

	g_Girls.MutuallyExclusiveTraits(newGirl, 1);	// make sure all the trait effects are applied
	g_Girls.ApplyTraits(newGirl);
	RemoveAllRememberedTraits(newGirl);	// WD: remove any rembered traits created from trait incompatibilities

	/*
	*	Now that everything is in there, time to give her a random name
	*
	*	we'll try five times for a unique name
	*	if that fails, we'll give her the last one found
	*	this should be ok - assuming that names don't have to be unique
	*
	*	If they do need to be unique, the game will slow drastically as
	*	the number of girls approaches the limit, and hang once it is surpassed.
	*
	*	So I'm assuming non-unique names are ok
	*/
	string name = "", name1 = "", name2 = "";
	for (int i = 0; i < 5; i++)
	{
		/* `J` Added g_BoysNameList for .06.03.00
			for now just using true to force girls names but when male slaves are added
			some way to choose random boys names will be added
		*/ 
		name = name1 = (true ? g_GirlNameList.random() : g_BoysNameList.random());
		if (i > 3)
		{
			name2 = (true ? g_GirlNameList.random() : g_BoysNameList.random());
			name = name1 + " " + name2; // `J` added second name to further reduce chance of multiple names
		}
		if (NameExists(name)) continue;
		break;
	}

	string surname;
	if (daughter || newGirl->m_States&(1 << STATUS_YOURDAUGHTER))	// `J` if she is your daughter...
	{
		surname = The_Player->Surname();	// give her your last name
	}
	else if (g_Dice.percent(90))		// 10% chance of no last name
	{
		for (int i = 0; i < 5; i++)
		{
			surname = g_SurnameList.random();
			if (i>3) surname = surname + "-" + g_SurnameList.random(); // `J` added second name to further reduce chance of multiple names
			if (SurnameExists(surname)) continue;
			break;
		}
	}
	else surname = "";
	newGirl->m_FirstName = name1;
	newGirl->m_MiddleName = name2;
	newGirl->m_Surname = surname;
	CreateRealName(newGirl);

	DirPath dp = DirPath(cfg.folders.characters().c_str()) << newGirl->m_Name << "triggers.xml";
	newGirl->m_Triggers.LoadList(dp);
	newGirl->m_Triggers.SetGirlTarget(newGirl);

	// `J` more usefull log for rgirl
	g_LogFile.os() << "Random girl " << newGirl->m_Realname << " created from template " << newGirl->m_Name << ".rgirlsx" << endl;

	if (addToGGirls)	AddGirl(newGirl);

	CalculateGirlType(newGirl);
	return newGirl;
}

bool cGirls::NameExists(string name)
{
	sGirl* current = m_Parent;
	while (current)
	{
		if (current->m_Realname == name)	return true;
		current = current->m_Next;
	}
	if (g_Brothels.NameExists(name))		return true;
	if (g_Studios.NameExists(name))			return true;
	if (g_Arena.NameExists(name))			return true;
	if (g_Centre.NameExists(name))			return true;
	if (g_Clinic.NameExists(name))			return true;
	if (g_Farm.NameExists(name))			return true;
	if (g_House.NameExists(name))			return true;
	for (int i = 0; i < 20; i++)
	{
		if (MarketSlaveGirls[i])
		{
			if (MarketSlaveGirls[i]->m_Realname == name)	return true;
		}
	}
	return false;
}
bool cGirls::SurnameExists(string name)
{
	sGirl* current = m_Parent;
	while (current)
	{
		if (current->m_Surname == name)	return true;
		current = current->m_Next;
	}
	if (g_Brothels.SurnameExists(name))		return true;
	if (g_Studios.SurnameExists(name))		return true;
	if (g_Arena.SurnameExists(name))		return true;
	if (g_Centre.SurnameExists(name))		return true;
	if (g_Clinic.SurnameExists(name))		return true;
	if (g_Farm.SurnameExists(name))			return true;
	if (g_House.SurnameExists(name))		return true;
	for (int i = 0; i < 20; i++)
	{
		if (MarketSlaveGirls[i])
		{
			if (MarketSlaveGirls[i]->m_Surname == name)	return true;
		}
	}
	return false;
}

// `J` added to simplify first, middle and surname combining into realname
string cGirls::CreateRealName(string first, string middle, string last)
{
	stringstream ss;
	int numnames = 0;
	if (first.size() > 0)	numnames++;
	if (middle.size() > 0)	numnames++;
	if (last.size() > 0)	numnames++;

	if (numnames == 0) return "";
	else if (numnames == 1) ss << first << middle << last;
	else if (numnames == 2)
	{
		if (first.size() > 0) ss << first << " " << middle << last;
		else ss << middle << " " << last;
	}
	else  ss << first << " " << middle << " " << last;

	return ss.str();
}
bool cGirls::CreateRealName(sGirl* girl)
{
	if (!girl) return false;
	string rn = CreateRealName(girl->m_FirstName, girl->m_MiddleName, girl->m_Surname);
	if (rn.length() > 0)
	{
		girl->m_Realname = rn;
		return true;
	}
	return false;
}
void cGirls::DivideName(sGirl* girl)
{
	string test = girl->m_Realname;
	int posspace = 0;
	posspace = test.find(' ');
	if (posspace < 0)		// only 1 name found so making it the first name
	{
		girl->m_FirstName = test;
	}
	else					// Set the first name found and test the rest
	{
		girl->m_FirstName = test.substr(0, posspace);
		test = test.substr(posspace + 1);
	}
	posspace = test.find_last_of(' ');
	if (posspace < 0)		// only 1 name found so making it the last name
	{
		girl->m_Surname = test;
	}
	else					// Set the middle and last names
	{
		girl->m_MiddleName = test.substr(0, posspace);
		girl->m_Surname = test.substr(posspace + 1);
	}
}
bool cGirls::BuildName(sGirl* girl)
{
	if (girl->m_Realname == "")				// if m_Realname is empty...
	{
		if (CreateRealName(girl))			// check if the F+M+S names are also empty
		{
			return true;					// if F+M+S exist, just use them to build m_Realname
		}
		girl->m_Realname = girl->m_Name;	// otherwise, set m_Realname as m_Name
	}
	DivideName(girl);						// divide the name
	return true;
}


// `J` moved exp check into levelup to reduce coding
void cGirls::LevelUp(sGirl* girl)
{
	int level = girl->level();
	int xp = girl->exp();
	int xpneeded = min(32000, (level + 1) * 125);

	if (xp < xpneeded) return;

	SetStat(girl, STAT_EXP, xp - xpneeded);
	girl->upd_stat(STAT_LEVEL, 1);

	if (girl->level() <= 20)	LevelUpStats(girl);

	stringstream ss;
	ss << girl->m_Realname << " levelled up to " << girl->level() << ".";
	girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_LEVELUP);
	ss.str("");

	// add traits
	// MYR: One chance to get a new trait every five levels.
	if (girl->level() % 5 == 0)
	{
		int addedtrait = girl->level() + 5;
		while (addedtrait > 0)
		{
			int chance = g_Dice % 12;
			string trait = "";
			switch (chance)
			{
			case 1:		trait = "Agile";				break;
			case 2:		trait = "Charismatic";			break;
			case 3:		trait = "Charming";				break;
			case 4:		trait = "Cool Person";			break;
			case 5:		trait = "Fake Orgasm Expert";	break;
			case 6:		trait = "Fleet of Foot";		break;
			case 7:		trait = "Good Kisser";			break;
			case 8:		trait = "Nimble Tongue";		break;
			case 9:		trait = "Nymphomaniac";			break;
			case 10:	trait = "Open Minded";			break;
			case 11:	trait = "Sexy Air";				break;
			default: break;
			}
			if (trait != "" && !girl->has_trait(trait))
			{
				addedtrait = 0;
				girl->add_trait(trait);
				ss << " She has gained the " << trait << " trait.";
				girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_GOODNEWS);
			}
			addedtrait--;
		}
	}
}

void cGirls::LevelUpStats(sGirl* girl)
{
	int DiceSize = 3;
	if (girl->has_trait("Quick Learner"))		DiceSize = 4;
	else if (girl->has_trait("Slow Learner"))	DiceSize = 2;

	// level up stats (only first 8 advance in levelups)
	for (int i = 0; i < 8; i++)	girl->upd_stat(i, g_Dice%DiceSize);

	// level up skills
	for (u_int i = 0; i < NUM_SKILLS; i++)	girl->upd_skill(i, g_Dice%DiceSize);
}

/*
*   `J` degrade the girls skills at the end of the turn.
*	if a sex type is banned, 10% chance she will lose 1 point in it
*   all other skills have a 5% chance to lose 1 point
*/
void cGirls::EndDayGirls(sBrothel* brothel, sGirl* girl)
{
	cJobManager m_JobManager;

	stringstream goodnews;
	/* */if (girl->m_NumCusts == girl->m_NumCusts_old)	{}	// no customers
	else if (girl->m_NumCusts < girl->m_NumCusts_old)	{}	// lost customers??
	else if (girl->m_NumCusts_old == 0 && girl->m_NumCusts > 0)
	{
		goodnews << girl->m_Realname << " has serviced her first ";
		/* */if (girl->m_NumCusts == 1)			goodnews << " customer.";
		else if (girl->m_NumCusts == 2)		goodnews << " pair of customers.";
		else if (girl->m_NumCusts == 12)	goodnews << " dozen customers.";
		else		goodnews << (int)girl->m_NumCusts << " customers. ";
		goodnews << " She is sure to service more as long as she works for you.";
	}
	else if (girl->m_NumCusts_old < 100 && girl->m_NumCusts >= 100)
	{
		goodnews << girl->m_Realname << " serviced her first hundred customers.";
		if (girl->has_trait("Optimist") && girl->happiness() > 80) goodnews << " She seems pleased with her accomplishment and looks forward to reaching the next level.";
		else goodnews << " You see great potential in this one.";
		girl->fame(1);
	}
	else if (girl->m_NumCusts_old < 500 && girl->m_NumCusts >= 500)
	{
		goodnews << girl->m_Realname << " serviced five hundred customers.";
		if (girl->has_trait("Optimist") && girl->happiness() > 80) goodnews << " She seems pleased with her accomplishment and looks forward to reaching the next level.";
		girl->add_trait("Whore");
		girl->fame(5);
	}
	else if (girl->m_NumCusts_old < 1000 && girl->m_NumCusts >= 1000)
	{
		goodnews << girl->m_Realname << " has slept with 1000 people.. You gotta wonder if its like throwing a hot dog down a hallway at this point.";
		girl->fame(10);

	}
	if (goodnews.str().length() > 2)	girl->m_Events.AddMessage(goodnews.str(), IMGTYPE_PROFILE, EVENT_GOODNEWS);


	girl->m_NumCusts_old = girl->m_NumCusts;			// prepare for next week

	int E_mana = 0, E_libido = 0, E_lactation = 0;

	/* */if (girl->has_trait("Muggle"))			E_mana = girl->magic() / 50;	// max 2 per day
	else if (girl->has_trait("Weak Magic"))		E_mana = girl->magic() / 20;	// max 5 per day
	else if (girl->has_trait("Strong Magic"))	E_mana = girl->magic() / 5;		// max 20 per day
	else if (girl->has_trait("Powerful Magic"))	E_mana = girl->magic() / 2;		// max 50 per day
	else /*                                 */	E_mana = girl->magic() / 10;	// max 10 per day
	girl->upd_stat(STAT_MANA, E_mana);


	// `J` update the girls base libido
	int total_libido = girl->libido();				// total_libido
	int base_libido = girl->m_Stats[STAT_LIBIDO];	// base_libido
	if (total_libido > (base_libido*1.5)) E_libido++;
	if (total_libido > 90)	E_libido++;
	if (total_libido < 10)	E_libido--;
	if (total_libido < (base_libido / 3)) E_libido--;
	girl->upd_stat(STAT_LIBIDO, E_libido);


	/* `J` lactation is not really thought out fully
	*	lactation traits affect how quickly she refills
	*	breast size affects how much she produces
	*	pregnancy doubles lactation
	*	pregnant cow girl will alwasy be ready to milk
	//*/
	if (!girl->has_trait("No Nipples"))	// no nipples = no lactation
	{
		/* */if (girl->has_trait("Dry Milk"))			E_lactation = 1;
		else if (girl->has_trait("Scarce Lactation"))	E_lactation = 5;
		else if (girl->has_trait("Abundant Lactation"))	E_lactation = 25;
		else if (girl->has_trait("Cow Tits"))			E_lactation = 50;
		else /*                                     */	E_lactation = 10;
		/* */if (girl->is_pregnant())					E_lactation *= 2;
		else if (girl->m_PregCooldown>0)				E_lactation = int((float)E_lactation * 2.5f);
		girl->upd_stat(STAT_LACTATION, E_lactation);
	}



	int a = g_Dice.d100();	if (a < 5 || (a < 10 && !m_JobManager.is_sex_type_allowed(SKILL_BEASTIALITY, brothel)))	girl->upd_skill(SKILL_BEASTIALITY, -1);
	int b = g_Dice.d100();	if (b < 5 || (b < 10 && !m_JobManager.is_sex_type_allowed(SKILL_BDSM, brothel)))		girl->upd_skill(SKILL_BDSM, -1);
	int c = g_Dice.d100();	if (c < 5 || (c < 10 && !m_JobManager.is_sex_type_allowed(SKILL_GROUP, brothel)))		girl->upd_skill(SKILL_GROUP, -1);
	int d = g_Dice.d100();	if (d < 5 || (d < 10 && !m_JobManager.is_sex_type_allowed(SKILL_NORMALSEX, brothel)))	girl->upd_skill(SKILL_NORMALSEX, -1);
	int e = g_Dice.d100();	if (e < 5 || (e < 10 && !m_JobManager.is_sex_type_allowed(SKILL_ANAL, brothel)))		girl->upd_skill(SKILL_ANAL, -1);
	int f = g_Dice.d100();	if (f < 5 || (f < 10 && !m_JobManager.is_sex_type_allowed(SKILL_LESBIAN, brothel)))		girl->upd_skill(SKILL_LESBIAN, -1);
	int g = g_Dice.d100();	if (g < 5 || (g < 10 && !m_JobManager.is_sex_type_allowed(SKILL_FOOTJOB, brothel)))		girl->upd_skill(SKILL_FOOTJOB, -1);
	int h = g_Dice.d100();	if (h < 5 || (h < 10 && !m_JobManager.is_sex_type_allowed(SKILL_HANDJOB, brothel)))		girl->upd_skill(SKILL_HANDJOB, -1);
	int i = g_Dice.d100();	if (i < 5 || (i < 10 && !m_JobManager.is_sex_type_allowed(SKILL_ORALSEX, brothel)))		girl->upd_skill(SKILL_ORALSEX, -1);
	int j = g_Dice.d100();	if (j < 5 || (j < 10 && !m_JobManager.is_sex_type_allowed(SKILL_TITTYSEX, brothel)))	girl->upd_skill(SKILL_TITTYSEX, -1);
	int k = g_Dice.d100();	if (k < 5 || (k < 10 && !m_JobManager.is_sex_type_allowed(SKILL_STRIP, brothel)))		girl->upd_skill(SKILL_STRIP, -1);

	if (g_Dice.percent(5))	girl->upd_skill(SKILL_MAGIC, -1);
	if (g_Dice.percent(5))	girl->upd_skill(SKILL_SERVICE, -1);
	if (g_Dice.percent(5))	girl->upd_skill(SKILL_COMBAT, -1);
	if (g_Dice.percent(5))	girl->upd_skill(SKILL_MEDICINE, -1);
	if (g_Dice.percent(5))	girl->upd_skill(SKILL_PERFORMANCE, -1);
	if (g_Dice.percent(5))	girl->upd_skill(SKILL_CRAFTING, -1);
	if (g_Dice.percent(5))	girl->upd_skill(SKILL_HERBALISM, -1);
	if (g_Dice.percent(5))	girl->upd_skill(SKILL_FARMING, -1);
	if (g_Dice.percent(5))	girl->upd_skill(SKILL_BREWING, -1);
	if (g_Dice.percent(5))	girl->upd_skill(SKILL_ANIMALHANDLING, -1);
	if (g_Dice.percent(5))	girl->upd_skill(SKILL_COOKING, -1);
}

// ----- Add remove

void cGirls::AddRandomGirl(sRandomGirl* girl)
{
	girl->m_Next = 0;
	if (m_RandomGirls)	m_LastRandomGirls->m_Next = girl;
	else				m_RandomGirls = girl;
	m_LastRandomGirls = girl;
	m_NumRandomGirls++;
	if (girl->m_YourDaughter)
	{
		m_NumRandomYourDaughterGirls++;
		if (girl->m_Human)		m_NumHumanRandomYourDaughterGirls++;
		if (!girl->m_Human)		m_NumNonHumanRandomYourDaughterGirls++;
	}
	else
	{
		if (girl->m_Human)		m_NumHumanRandomGirls++;
		if (!girl->m_Human)		m_NumNonHumanRandomGirls++;
	}
}

void cGirls::AddGirl(sGirl* girl)
{
	girl->m_Prev = girl->m_Next = 0;
	if (m_Parent)
	{
		girl->m_Prev = m_Last;
		m_Last->m_Next = girl;
		m_Last = girl;
	}
	else	m_Last = m_Parent = girl;
	m_NumGirls++;
}

void cGirls::RemoveGirl(sGirl* girl, bool deleteGirl)
{
	if (m_Parent == 0)	return;
	bool match = false;
	sGirl* currGirl = m_Parent;
	while (currGirl)
	{
		if (currGirl == girl)
		{
			match = true;
			break;
		}
		currGirl = currGirl->m_Next;
	}
	if (match)
	{
		if (deleteGirl)
		{
			if (girl->m_Prev)		girl->m_Prev->m_Next = girl->m_Next;
			if (girl->m_Next)		girl->m_Next->m_Prev = girl->m_Prev;
			if (girl == m_Parent)	m_Parent = girl->m_Next;
			if (girl == m_Last)		m_Last = girl->m_Prev;
			girl->m_Next = girl->m_Prev = 0;
			delete girl;
			girl = 0;
		}
		else
		{
			if (girl->m_Prev)		girl->m_Prev->m_Next = girl->m_Next;
			if (girl->m_Next)		girl->m_Next->m_Prev = girl->m_Prev;
			if (girl == m_Parent)	m_Parent = girl->m_Next;
			if (girl == m_Last)		m_Last = girl->m_Prev;
			girl->m_Next = girl->m_Prev = 0;
		}
		m_NumGirls--;
	}
}

void cGirls::AddTiredness(sGirl* girl)
{
	if (girl->has_trait( "Incorporeal") || // Sanity check
		girl->has_trait( "Skeleton") ||
		girl->has_trait( "Zombie"))
	{
		g_Girls.SetStat(girl, STAT_TIREDNESS, 0);	return;
	}
	int tiredness = 10;
	if (girl->constitution() > 0)
		tiredness -= (girl->constitution()) / 10;
	if (tiredness <= 0)	tiredness = 0;
	girl->upd_stat(STAT_TIREDNESS, tiredness, false);
	if (girl->tiredness() == 100)
	{
		girl->upd_stat(STAT_HAPPINESS, -1, false);
		girl->upd_stat(STAT_HEALTH, -1, false);
	}
}

// ----- Get

int cGirls::GetSlaveGirl(int from)
{
	sGirl* current = m_Parent;
	int num = 0; int girlnum = 0;
	bool found = false;
	while (current)
	{
		if (current->m_States&(1 << STATUS_SLAVE))
		{
			if (num == from)
			{
				found = true;
				break;
			}
			else	num++;
		}
		girlnum++;
		current = current->m_Next;
	}
	if (found == false)	return -1;
	return girlnum;
}

vector<sGirl *>  cGirls::get_girls(GirlPredicate* pred)
{
	sGirl *girl;
	vector<sGirl *> v;
	for (girl = m_Parent; girl; girl = girl->m_Next)
	{
		if (pred->test(girl))	v.push_back(girl);
	}
	return v;
}

string cGirls::GetGirlMood(sGirl* girl)
{
	stringstream ss;
	ss << girl->m_Realname;
	int variable = 0;

	int HateLove = girl->pclove() - girl->pchate();
	ss << " feels the player ";

	if (girl->has_trait( "Your Daughter"))
	{
		/* */if (HateLove <= -80)	ss << "should die ";
		else if (HateLove <= -60)	ss << "is better off dead ";
		else if (HateLove <= -40)	ss << "is mean ";
		else if (HateLove <= -20)	ss << "isn't nice ";
		else if (HateLove <= 0)		ss << "is annoying ";
		else if (HateLove <= 20)	ss << "is ok ";
		else if (HateLove <= 40)	ss << "is easy going ";
		else if (HateLove <= 60)	ss << "is a good dad ";
		else if (HateLove <= 80)	ss << "is a great dad ";
		else 						ss << "is an awesome daddy ";
	}
	else if (girl->has_trait( "Lesbian"))//lesbian shouldn't fall in love with you
	{
		/* */if (HateLove <= -80)	ss << "should die ";
		else if (HateLove <= -60)	ss << "is better off dead ";
		else if (HateLove <= -40)	ss << "is mean ";
		else if (HateLove <= -20)	ss << "isn't nice ";
		else if (HateLove <= 0)		ss << "is annoying ";
		else if (HateLove <= 20)	ss << "is ok ";
		else if (HateLove <= 40)	ss << "is easy going ";
		else if (HateLove <= 60)	ss << "is good ";
		else if (HateLove <= 80)	ss << "is a nice guy ";
		else 						ss << "is an awesome guy ";
	}
	else
	{
		/* */if (HateLove <= -80)	ss << "should die ";
		else if (HateLove <= -60)	ss << "is better off dead ";
		else if (HateLove <= -40)	ss << "is mean ";
		else if (HateLove <= -20)	ss << "isn't nice ";
		else if (HateLove <= 0)		ss << "is annoying ";
		else if (HateLove <= 20)	ss << "is ok ";
		else if (HateLove <= 40)	ss << "is easy going ";
		else if (HateLove <= 60)	ss << "is good ";
		else if (HateLove <= 80)	ss << "is attractive ";
		else 						ss << "is her true love ";
	}

	if (girl->pcfear() > 20)
	{
		if (HateLove > 0)	ss << "but she is also ";
		else				ss << "and she is ";
		/* */if (girl->pcfear() < 40)	ss << "afraid of him." << (girl->is_dead() ? " (for good reasons)." : ".");
		else if (girl->pcfear() < 60)	ss << "fearful of him." << (girl->is_dead() ? " (for good reasons)." : ".");
		else if (girl->pcfear() < 80)	ss << "afraid he will hurt her" << (girl->is_dead() ? " (and she was right)." : ".");
		else										ss << "afraid he will kill her" << (girl->is_dead() ? " (and she was right)." : ".");

	}
	else	ss << "and he isn't scary.";

	int happy = girl->happiness();
	ss << "\nShe is ";
	if (girl->health() < 1)	ss << "dead.";
	else if (happy > 90)	ss << "happy.";
	else if (happy > 80)	ss << "joyful.";
	else if (happy > 60)	ss << "reasonably happy.";
	else if (happy > 40)	ss << "unhappy.";
	else		ss << "showing signs of depression.";


	int morality = girl->morality(); //zzzzz FIXME needs better text
	ss << "\nShe " << (girl->health() < 1 ? "was " : "is ");
	/* */if (morality <= -80)	ss << "pure evil";
	else if (morality <= -60)	ss << "evil";
	else if (morality <= -40)	ss << "mean";
	else if (morality <= -20)	ss << "not nice";
	else if (morality <= 0)		ss << "neutral";
	else if (morality <= 20)	ss << "lawful";
	else if (morality <= 40)	ss << "nice";
	else if (morality <= 60)	ss << "good";
	else if (morality <= 80)	ss << "very good";
	else 						ss << "holy";
	ss << ".";

	int sanity = girl->sanity();	// `bsin` added - `J` adjusted
	ss << "\nShe " << (girl->health() < 1 ? "was " : "is ");

	/* */if (sanity <= -75)
	{
		/* */if (morality < -66)		ss << "psychopathic";
		else if (morality < -33)		ss << "a total bunny-boiler";
		else if (morality <	 33)		ss << "dangerous";
		else if (morality <	 66)		ss << "a raving fundamentalist";
		else							ss << "a religious extremist";
	}
	else if (sanity <= -45)
	{
		/* */if (morality < -33)		ss << "a bit of a psycho";
		else if (morality <	 33)		ss << "scarily out-there";
		else							ss << "scarily over-zealous";
	}
	else if (sanity <= -15)
	{
		if (morality < 0)		ss << "disturbed";
		else					ss << "nuts";
	}
	else if (sanity <= 15)	ss << "mentally stable";
	else if (sanity <= 45)	ss << "well-adjusted";
	else if (sanity <= 75)	ss << "completely rational";
	else ss << "rational to the extreme";

	ss << ".";

	return ss.str();
}

string cGirls::GetDetailsString(sGirl* girl, bool purchase)
{
	if (girl == 0)	return string("");
	//cTariff tariff;
	cFont check; int w, h, size = 0;
	check.LoadFont(cfg.fonts.normal(), cfg.fonts.detailfontsize());
	stringstream ss;
	string sper = ""; if (cfg.fonts.showpercent()) sper = " %";

	// `J` When modifying Stats or Skills, search for "J-Change-Stats-Skills"  :  found in >> cGirls.cpp > GetDetailsString
	string basestr[] = { "Age : ", "Rebelliousness : ", "Looks : ", "Constitution : ", "Health : ", "Happiness : ", "Tiredness : ", "Worth : " };

	int skillnum[] = { SKILL_MAGIC, SKILL_COMBAT, SKILL_SERVICE, SKILL_MEDICINE, SKILL_PERFORMANCE, SKILL_CRAFTING, SKILL_HERBALISM, SKILL_FARMING, SKILL_BREWING, SKILL_ANIMALHANDLING, SKILL_COOKING, SKILL_ANAL, SKILL_BDSM, SKILL_NORMALSEX, SKILL_BEASTIALITY, SKILL_GROUP, SKILL_LESBIAN, SKILL_ORALSEX, SKILL_TITTYSEX, SKILL_HANDJOB, SKILL_STRIP, SKILL_FOOTJOB };
	string skillstr[] = { "Magic Ability : ", "Combat Ability : ", "Service Skills : ", "Medicine Skill : ", "Performance Skill : ", "Crafting Skill : ", "Herbalism Skill : ", "Farming Skill : ", "Brewing Skill : ", "Animal Handling : ", "Cooking : ", "Anal Sex : ", "BDSM Sex : ", "Normal Sex : ", "Bestiality Sex : ", "Group Sex : ", "Lesbian Sex : ", "Oral Sex : ", "Titty Sex : ", "Hand Job : ", "Stripping : ", "Foot Job : " };

	if (cfg.fonts.normal() == "segoeui.ttf" && cfg.fonts.detailfontsize() == 9) // `J` if already set to my default
	{
		string basesegoeuistr[] = { "Age :                                   ", "Rebelliousness :         ", "Looks :                              ", "Constitution :               ", "Health :                            ", "Happiness :                    ", "Tiredness :                     ", "Worth :                             " };
		for (int i = 0; i < 8; i++) basestr[i] = basesegoeuistr[i];
		string skillsegoeuistr[] = { "Magic Ability :              ", "Combat Ability :           ", "Service Skills :              ", "Medicine Skill :            ", "Performance Skill :    ", "Crafting Skill :              ", "Herbalism Skill :         ", "Farming Skill :             ", "Brewing Skill :               ", "Animal Handling :    ", "Cooking :                        ", "Anal Sex :                        ", "BDSM Sex :                      ", "Normal Sex :                 ", "Bestiality Sex :              ", "Group Sex :                     ", "Lesbian Sex :                 ", "Oral Sex :                         ", "Titty Sex :                         ", "Hand Job :                      ", "Stripping :                       ", "Foot Job :                         " };
		for (int i = 0; i < 22; i++) skillstr[i] = skillsegoeuistr[i];
		size = 90;
	}
	else		// `J` otherwise try to align the numbers
	{
		// get the widest
		for (int i = 0; i < 8; i++) { check.GetSize(basestr[i], w, h); if (w > size) size = w; }
		for (int i = 0; i < 22; i++) { check.GetSize(skillstr[i], w, h); if (w > size) size = w; }
		size += 5; // add a little padding
		// then add extra spaces until it is longer that the widest
		for (int i = 0; i < 8; i++)
		{
			check.GetSize(basestr[i], w, h);
			while (w < size)
			{
				basestr[i] += " ";
				check.GetSize(basestr[i], w, h);
			}
		}
		for (int i = 0; i < 22; i++)
		{
			check.GetSize(skillstr[i], w, h);
			while (w < size)
			{
				skillstr[i] += " ";
				check.GetSize(skillstr[i], w, h);
			}
		}
	}

	string levelstr[] = { "Level : ", "Exp : ", "Exp to level : ", "Needs : " };

	int level = girl->level();
	int exp = girl->exp();
	int exptolv = min(32000, (level + 1) * 125);
	int expneed = exptolv - exp;

	check.GetSize(levelstr[0], w, h);
	if (!purchase)
	{
		while (w < size - 5)
		{
			levelstr[0] += " ";
			stringstream levelnumstr; levelnumstr << level;
			check.GetSize(levelstr[0] + levelnumstr.str(), w, h);
		}
	}
	else
	{
		while (w < size)
		{
			levelstr[0] += " ";
			check.GetSize(levelstr[0], w, h);
		}

	}

	// display looks
	ss << basestr[2] << (girl->beauty() + girl->charisma()) / 2 << sper;

	// display level and exp
	ss << "\n" << levelstr[0] << level;
	if (!purchase)
	{
		check.GetSize(levelstr[2], w, h);
		while (w < size - 5)
		{
			levelstr[2] += " ";
			stringstream levelnumstr; levelnumstr << exptolv;
			check.GetSize(levelstr[2] + levelnumstr.str(), w, h);
		}
		ss << "  |  " << levelstr[1] << exp;
		ss << "\n" << levelstr[2] << exptolv;
		ss << "  |  " << levelstr[3] << expneed;
	}

	// display Age
	ss << "\n" << basestr[0]; if (girl->age() == 100) ss << "Unknown"; else ss << girl->age();
	// display rebel
	ss << "\n" << basestr[1] << girl->rebel();
	// display Constitution
	ss << "\n" << basestr[3] << girl->constitution() << sper;

	// display HHT and money
	if (!purchase)
	{
		ss << "\n" << basestr[4] << girl->health() << sper;
		ss << "\n" << basestr[5] << girl->happiness() << sper;
		ss << "\n" << basestr[6] << girl->tiredness() << sper;
	}
	int cost = int(tariff.slave_price(girl, purchase));
	g_LogFile.ss() << "slave " << (purchase ? "buy" : "sell") << "price = " << cost;
	g_LogFile.ssend();
	ss << "\n" << basestr[7] << cost << " Gold";
	CalculateAskPrice(girl, false);
	cost = girl->askprice();
	ss << "\nAvg Pay per Customer : " << cost << " gold\n";

	// display status
	/* */if (girl->m_States&(1 << STATUS_SLAVE))	ss << "Is Branded a Slave\n";
	else if (cfg.debug.log_extradetails())			ss << "( She Is Not a Slave )\n";
	else ss << "\n";

	/* */if (girl->check_virginity())			ss << "She is a Virgin\n";
	else if (cfg.debug.log_extradetails())			ss << "( She Is Not a Virgin )\n";
	else ss << "\n";

	if (!purchase)
	{
		int to_go = (girl->m_States&(1 << STATUS_INSEMINATED) ? cfg.pregnancy.weeks_monster_p() : cfg.pregnancy.weeks_pregnant()) - girl->m_WeeksPreg;
		/* */if (girl->m_States&(1 << STATUS_PREGNANT))				{ ss << "Is pregnant, due: " << to_go << " weeks\n"; }
		else if (girl->m_States&(1 << STATUS_PREGNANT_BY_PLAYER))	{ ss << "Is pregnant with your child, due: " << to_go << " weeks\n"; }
		else if (girl->m_States&(1 << STATUS_INSEMINATED))			{ ss << "Is inseminated, due: " << to_go << " weeks\n"; }
		else if (girl->m_PregCooldown != 0)							{ ss << "Cannot get pregnant for: " << girl->m_PregCooldown << " weeks\n"; }
		else if (cfg.debug.log_extradetails())						{ ss << "( She Is not Pregnant )\n"; }
		else ss << "\n";
		// `J` moved the rest of children lines to second detail list
	}

	/* */if (girl->is_addict() && !girl->has_disease()) ss << "Has an addiciton\n";
	else if (!girl->is_addict() && girl->has_disease())	ss << "Has a disease\n";
	else if (girl->is_addict() && girl->has_disease())	ss << "Has an addiciton and a disease\n";
	else if (cfg.debug.log_extradetails())				ss << "( She Has No Addicitons or Diseases )\n";
	else												ss << "\n";

	if (!purchase)
	{
		if (girl->m_States&(1 << STATUS_BADLY_POISONED))ss << "Is badly poisoned\n";
		else if (girl->m_States&(1 << STATUS_POISONED))	ss << "Is poisoned\n";
		else if (cfg.debug.log_extradetails())			ss << "( She Is Not Poisoned )\n";
		else											ss << "\n";
	}

	if (!purchase)
	{
		int cust = girl->m_NumCusts;
		ss << "\nShe has slept with " << cust << " Customers.";
	}

	// display Skills
	ss << "\n \nSKILLS";
	if (cfg.debug.log_extradetails() && !purchase) ss << "           (base+temp+item+trait)";

	for (int i = 0; i < 22; i++)
	{
		if (i == 11)
		{
			ss << "\n \nSEX SKILLS";
			if (cfg.debug.log_extradetails() && !purchase) ss << "           (base+temp+item+trait)";
		}
		ss << "\n" << skillstr[i] << girl->get_skill(skillnum[i]) << sper;
		if (cfg.debug.log_extradetails() && !purchase) ss << "    (" << girl->m_Skills[skillnum[i]] << "+" << girl->m_SkillTemps[skillnum[i]] << "+" << girl->m_SkillMods[skillnum[i]] << "+" << girl->m_SkillTr[skillnum[i]] << ")";
	}
	return ss.str();
}

string cGirls::GetMoreDetailsString(sGirl* girl, bool purchase)
{
	if (girl == 0)	return "";
	string sper = ""; if (cfg.fonts.showpercent()) sper = " %";
	stringstream ss;
	cFont check; int w, h, size = 0;
	check.LoadFont(cfg.fonts.normal(), cfg.fonts.detailfontsize());

	// `J` When modifying Stats or Skills, search for "J-Change-Stats-Skills"  :  found in >> cGirls.cpp > GetMoreDetailsString
	ss << "STATS";
	if (cfg.debug.log_extradetails() && !purchase) ss << "           (base+temp+item+trait)";
	int statnum[] = { STAT_CHARISMA, STAT_BEAUTY, STAT_LIBIDO, STAT_MANA, STAT_INTELLIGENCE, STAT_CONFIDENCE, STAT_OBEDIENCE, STAT_SPIRIT, STAT_AGILITY, STAT_STRENGTH, STAT_FAME, STAT_LACTATION ,STAT_PCFEAR, STAT_PCLOVE, STAT_PCHATE };
	int statnumsize = 15;
	string statstr[] = { "Charisma : ", "Beauty : ", "Libido : ", "Mana : ", "Intelligence : ", "Confidence : ", "Obedience : ", "Spirit : ", "Agility : ", "Strength : ", "Fame : ", "Lactation : ", "PCFear : ", "PCLove : ", "PCHate : ", "Gold : " };

	int show = (cfg.debug.log_extradetails() && !purchase) ? statnumsize : statnumsize - 3;

	if (cfg.fonts.normal() == "segoeui.ttf" && cfg.fonts.detailfontsize() == 9) // `J` if already set to my default
	{
		string statsegoeuistr[] = { "Charisma :          ", "Beauty :                  ", "Libido :                   ", "Mana :                   ", "Intelligence :      ", "Confidence :       ", "Obedience :         ", "Spirit :                     ", "Agility :                  ", "Strength :             ", "Fame :                    ", "Lactation :           ", "PCFear :                 ", "PCLove :                 ", "PCHate :                 ", "Gold :                     " };
		for (u_int i = 0; i < 16; i++) statstr[i] = statsegoeuistr[i];
		size = 70;
	}
	else		// `J` otherwise try to align the numbers
	{
		// get the widest
		for (u_int i = 0; i < 16; i++) { check.GetSize(statstr[i], w, h); if (w > size) size = w; }
		size += 10; // add some padding
		// then add extra spaces to the statstr until it is longer that the widest
		for (int i = 0; i < show; i++)
		{
			check.GetSize(statstr[i], w, h);
			while (w < size)
			{
				statstr[i] += " ";
				check.GetSize(statstr[i], w, h);
			}
		}
	}

	for (int i = 0; i < show; i++)
	{
		ss << "\n" << statstr[i] << girl->get_stat(statnum[i]) << sper;
		if (cfg.debug.log_extradetails() && !purchase) ss << "    (" << girl->m_Stats[statnum[i]] << "+" << girl->m_StatTemps[statnum[i]] << "+" << girl->m_StatMods[statnum[i]] << "+" << girl->m_StatTr[statnum[i]] << ")";
	}
	if (!purchase)
	{
		ss << "\n" << statstr[15];
		if (g_Gangs.GetGangOnMission(MISS_SPYGIRLS))
		{
			ss << girl->m_Money;
		}
		else
		{
			ss << "Unknown";
		}
	}
	else ss << "\n";

	if (!purchase)
	{
		ss << "\n \nAccommodation: ";
		if (cfg.debug.log_extradetails()) ss << "( " << girl->m_AccLevel << " ) ";
		ss << Accommodation(girl->m_AccLevel);
		if (cfg.debug.log_extradetails())
		{
			ss << "\n" << (girl->is_free() ? "Preferred  Accom:" : "Expected Accom: ")
				<< " ( " << PreferredAccom(girl) << " ) " << Accommodation(PreferredAccom(girl));
		}
		ss << "\nCost per turn: " << ((girl->is_slave() ? 5 : 20) * (girl->m_AccLevel + 1)) << " gold.\n";

		//ss << "\nDetails:\n";
		//ss << AccommodationDetails(girl, girl->m_AccLevel);
		//ss << g_Girls.AccommodationDetails(girl, girl->m_AccLevel);

		// added from Dagoth
		if (girl->is_resting() && !girl->was_resting() && girl->m_PrevDayJob != 255 && girl->m_PrevNightJob != 255)
		{
			ss << "\n \nOFF WORK, RESTING DUE TO TIREDNESS.";
			ss << "\nStored Day Job:   " << g_Brothels.m_JobManager.JobName[girl->m_PrevDayJob];
			ss << "\nStored Night Job: " << g_Brothels.m_JobManager.JobName[girl->m_PrevNightJob];
			ss << "\n";
		}
		int to_go = (girl->m_States&(1 << STATUS_INSEMINATED) ? cfg.pregnancy.weeks_monster_p() : cfg.pregnancy.weeks_pregnant()) - girl->m_WeeksPreg;
		// first line is current pregnancy
		/* */if (girl->m_States&(1 << STATUS_PREGNANT))				{ ss << "Is pregnant, due: " << to_go << " weeks\n"; }
		else if (girl->m_States&(1 << STATUS_PREGNANT_BY_PLAYER))	{ ss << "Is pregnant with your child, due: " << to_go << " weeks\n"; }
		else if (girl->m_States&(1 << STATUS_INSEMINATED))			{ ss << "Is inseminated, due: " << to_go << " weeks\n"; }
		else if (girl->m_PregCooldown != 0)							{ ss << "Cannot get pregnant for: " << girl->m_PregCooldown << " weeks\n"; }
		else if (cfg.debug.log_extradetails())						{ ss << "( She Is not Pregnant )\n"; }
		else ss << "\n";
		// count the total births
		if (girl->m_ChildrenCount[CHILD00_TOTAL_BIRTHS] > 0)
			ss << "She has given birth to " << girl->m_ChildrenCount[CHILD00_TOTAL_BIRTHS]
				<< " child" << (girl->m_ChildrenCount[CHILD00_TOTAL_BIRTHS] > 1 ? "ren" : "") << ":\n";
		// count the girls born
		if (girl->m_ChildrenCount[CHILD02_ALL_GIRLS] > 0)
		{
			ss << girl->m_ChildrenCount[CHILD02_ALL_GIRLS] << " girl" << (girl->m_ChildrenCount[CHILD02_ALL_GIRLS] > 1 ? "s" : "") << "\n   ";
			if (girl->m_ChildrenCount[CHILD02_ALL_GIRLS] == girl->m_ChildrenCount[CHILD06_YOUR_GIRLS])
			{
				if (girl->m_ChildrenCount[CHILD02_ALL_GIRLS] == 1)
					ss << "She is your daughter.";
				else ss << "They are all yours.";
			}
			else if (girl->m_ChildrenCount[CHILD02_ALL_GIRLS] == girl->m_ChildrenCount[CHILD04_CUSTOMER_GIRLS])
			{
				if (girl->m_ChildrenCount[CHILD02_ALL_GIRLS] == 1)
					ss << "She is not your daughter.";
				else ss << "They are all from other men.";
			}
			else
			{
				if (girl->m_ChildrenCount[CHILD06_YOUR_GIRLS] == 1)
					ss << "One is yours and ";
				else ss << girl->m_ChildrenCount[CHILD06_YOUR_GIRLS] << " of them are yours and ";
				ss << "\n   ";
				if (girl->m_ChildrenCount[CHILD04_CUSTOMER_GIRLS] == 1)
					ss << "One is from another man.";
				else ss << girl->m_ChildrenCount[CHILD04_CUSTOMER_GIRLS] << " of them are from other men.";
			}
			ss << "\n";
		}
		else if (girl->m_States&(1 << STATUS_HAS_DAUGHTER))		ss << "at least one Daughter (old code)\n";
		else if (cfg.debug.log_extradetails())					ss << "( She Has No Daughters )\n";

		// count the boys born
		if (girl->m_ChildrenCount[CHILD03_ALL_BOYS] > 0)
		{
			ss << girl->m_ChildrenCount[CHILD03_ALL_BOYS] << " boy" << (girl->m_ChildrenCount[CHILD03_ALL_BOYS] > 1 ? "s" : "") << "\n   ";
			if (girl->m_ChildrenCount[CHILD03_ALL_BOYS] == girl->m_ChildrenCount[CHILD07_YOUR_BOYS])
			{
				if (girl->m_ChildrenCount[CHILD03_ALL_BOYS] == 1)
					ss << "He is your son.";
				else ss << "They are all yours.";
			}
			else if (girl->m_ChildrenCount[CHILD03_ALL_BOYS] == girl->m_ChildrenCount[CHILD05_CUSTOMER_BOYS])
			{
				if (girl->m_ChildrenCount[CHILD03_ALL_BOYS] == 1)
					ss << "He is not your son.";
				else ss << "They are all from other men.";
			}
			else
			{
				if (girl->m_ChildrenCount[CHILD07_YOUR_BOYS] == 1)
					ss << "One is yours and ";
				else ss << girl->m_ChildrenCount[CHILD07_YOUR_BOYS] << " of them are yours and ";
				ss << "\n   ";
				if (girl->m_ChildrenCount[CHILD05_CUSTOMER_BOYS] == 1)
					ss << "One is from another man.";
				else ss << girl->m_ChildrenCount[CHILD05_CUSTOMER_BOYS] << " of them are from other men.";
			}
			ss << "\n";
		}
		else if (girl->m_States&(1 << STATUS_HAS_SON))			ss << "at least one Son (old code)\n";
		else if (cfg.debug.log_extradetails())					ss << "( She Has No Sons )\n";

		if (girl->m_ChildrenCount[CHILD01_ALL_BEASTS] > 0)		ss << "She has given birth to " << girl->m_ChildrenCount[CHILD01_ALL_BEASTS] << " Beast" << (girl->m_ChildrenCount[CHILD01_ALL_BEASTS] > 1 ? "s" : "") << ".\n";
		if (girl->m_ChildrenCount[CHILD08_MISCARRIAGES] > 0)	ss << "She has had " << girl->m_ChildrenCount[CHILD08_MISCARRIAGES] << " Miscarriage" << (girl->m_ChildrenCount[CHILD08_MISCARRIAGES] > 1 ? "s" : "") << ".\n";
		if (girl->m_ChildrenCount[CHILD09_ABORTIONS] > 0)		ss << "She has had " << girl->m_ChildrenCount[CHILD09_ABORTIONS] << " Abortion" << (girl->m_ChildrenCount[CHILD09_ABORTIONS] > 1 ? "s" : "") << ".\n";
	}

	ss << "\n \nFETISH CATEGORIES\n";
	if (CheckGirlType(girl, FETISH_BIGBOOBS))		ss << " |Big Boobs| ";
	if (CheckGirlType(girl, FETISH_CUTEGIRLS))		ss << " |Cute Girl| ";
	if (CheckGirlType(girl, FETISH_DANGEROUSGIRLS))	ss << " |Dangerous| ";
	if (CheckGirlType(girl, FETISH_COOLGIRLS))		ss << " |Cool| ";
	if (CheckGirlType(girl, FETISH_NERDYGIRLS))		ss << " |Nerd| ";
	if (CheckGirlType(girl, FETISH_NONHUMAN))		ss << " |Non or part human| ";
	if (CheckGirlType(girl, FETISH_LOLITA))			ss << " |Lolita| ";
	if (CheckGirlType(girl, FETISH_ELEGANT))		ss << " |Elegant| ";
	if (CheckGirlType(girl, FETISH_SEXY))			ss << " |Sexy| ";
	if (CheckGirlType(girl, FETISH_FIGURE))			ss << " |Nice Figure| ";
	if (CheckGirlType(girl, FETISH_ARSE))			ss << " |Nice Arse| ";
	if (CheckGirlType(girl, FETISH_SMALLBOOBS))		ss << " |Small Boobs| ";
	if (CheckGirlType(girl, FETISH_FREAKYGIRLS))	ss << " |Freaky| ";
	if (CheckGirlType(girl, FETISH_FUTAGIRLS))		ss << " |Futa| ";
	if (CheckGirlType(girl, FETISH_TALLGIRLS))		ss << " |Tall| ";
	if (CheckGirlType(girl, FETISH_SHORTGIRLS))		ss << " |Short| ";
	if (CheckGirlType(girl, FETISH_FATGIRLS))		ss << " |Fat| ";
//	if (CheckGirlType(girl, FETISH_OBEDIENCE))		ss << " |Obedient| ";
//	if (CheckGirlType(girl, FETISH_REBELLIOUS))		ss << " |Rebellious| ";
//	if (CheckGirlType(girl, FETISH_DEGRADATION))	ss << " |Degraded| ";
//	if (CheckGirlType(girl, FETISH_MUTILATION))		ss << " |Mutilated| ";
//	if (CheckGirlType(girl, FETISH_BONDAGE))		ss << " |Restrained| ";


	if (!purchase)
	{
		ss << "\n \nJOB PREFERENCES";
		if (cfg.debug.log_extradetails() && !purchase) ss << "\n    (base+temp+item+trait)";
		ss << "\n";
		string base = "She";
		string text;
		int enjcount = 0;
		for (int i = 0; i < NUM_ACTIONTYPES; ++i)
		{
			if (sGirl::enjoy_jobs[i] == "")			continue;
			int e = girl->get_enjoyment(i);
			/* */if (e < -70)	{ text = " hates "; }
			else if (e < -50)	{ text = " really dislikes "; }
			else if (e < -30)	{ text = " dislikes "; }
			else if (e < -20)	{ text = " doesn't particularly enjoy "; }
			// if she's indifferent, why specify it? Let's instead skip it.
			else if (e < 15)	{ if (cfg.debug.log_extradetails())	{ text = " is indifferent to "; } else continue; }
			else if (e < 30)	{ text = " is happy enough with "; }
			else if (e < 50)	{ text = " likes "; }
			else if (e < 70)	{ text = " really enjoys "; }
			else				{ text = " loves "; }
			ss << base << text << sGirl::enjoy_jobs[i] << ".";
			if (cfg.debug.log_extradetails() || cfg.debug.log_show_numbers())
			{
				if (cfg.debug.log_extradetails()) ss << "\n";
				ss << "    ( " << girl->m_Enjoyment[i];
				if (cfg.debug.log_extradetails())
					ss << " + " << girl->m_EnjoymentTemps[i] << " + " << girl->m_EnjoymentMods[i] << " + " << girl->m_EnjoymentTR[i];
				ss << " )";
			}
			ss << "\n";
			enjcount++;
		}
		if (cfg.debug.log_extradetails())			{ ss << "\n"; }
		else if (enjcount > 0)						{ ss << "\nShe is indifferent to all other tasks.\n \n"; }
		else										{ ss << "At the moment, she is indifferent to all tasks.\n \n"; }

		int tricount = 0;
		for (int i = 0; i < NUM_TRAININGTYPES; ++i)
		{
			if (sGirl::training_jobs[i] == "")			continue;
			int e = girl->get_training(i);
			/* */if (e < 0)	{ text = " hasn't started "; }
			// if she's indifferent, why specify it? Let's instead skip it.
			else if (e < 15)	{ if (cfg.debug.log_extradetails())	{ text = " is indifferent to "; } else continue; }
			else if (e < 30)	{ text = " has started "; }
			else if (e < 50)	{ text = " knows the basics of "; }
			else if (e < 70)	{ text = " has knowledge of "; }
			else				{ text = " performs well in"; }
			ss << base << text << sGirl::training_jobs[i] << ".";
			if (cfg.debug.log_extradetails() || cfg.debug.log_show_numbers())
			{
				if (cfg.debug.log_extradetails()) ss << "\n";
				ss << "    ( " << girl->m_Training[i];
				if (cfg.debug.log_extradetails())
					ss << " + " << girl->m_TrainingTemps[i] << " + " << girl->m_TrainingMods[i] << " + " << girl->m_TrainingTR[i];
				ss << " )";
			}
			ss << "\n";
			tricount++;
		}
		if (cfg.debug.log_extradetails())			{ ss << "\n"; }
		else if (tricount > 0)						{ ss << "\nShe hasn't started any other training.\n \n"; }
		else										{ ss << "At the moment, she hasn't started any special training.\n \n"; }
	}

	ss << "\n \n\nBased on:  " << girl->m_Name;

	return ss.str();
}

string cGirls::GetThirdDetailsString(sGirl* girl)	// `J` bookmark - Job ratings
{
	cJobManager m_JobManager;
	// `J` bookmark - Job Ratings list

	// `J` When modifying Jobs, search for "J-Change-Jobs"  :  found in >> cGirls.cpp > GetThirdDetailsString

	// `J` spiltting the buildings so they can be sorted
	string div = "\n------------------------------------\n\n";
	string Brothel_Data = "";
	Brothel_Data += "Brothel Job Ratings\n";
	Brothel_Data += girl->JobRating(m_JobManager.JP_Matron(girl, true), "-", "Matron");
	Brothel_Data += girl->JobRating(m_JobManager.JP_Security(girl, true), "-", "Security");
	Brothel_Data += girl->JobRating(m_JobManager.JP_ExploreCatacombs(girl, true), "-", "Explore Catacombs");
	Brothel_Data += girl->JobRating(m_JobManager.JP_Advertising(girl, true), "-", "Advertising");
	Brothel_Data += girl->JobRating(m_JobManager.JP_CustService(girl, true), "-", "Customer Service");
	Brothel_Data += girl->JobRating(m_JobManager.JP_BeastCare(girl, true), "-", "Beast Care");
	Brothel_Data += girl->JobRating(m_JobManager.JP_Training(girl, true), "!", "Training");
	// Brothel_Data += girl->JobRating(m_JobManager.JP_Torturer(girl, true), "* Torturer");
	Brothel_Data += girl->JobRating(m_JobManager.JP_Cleaning(girl, true), "?", "Cleaning");
	Brothel_Data += "\n";
	Brothel_Data += girl->JobRating(m_JobManager.JP_Barmaid(girl, true), "-", "Barmaid");
	Brothel_Data += girl->JobRating(m_JobManager.JP_BarWaitress(girl, true), "-", "Bar Waitress");
	Brothel_Data += girl->JobRating(m_JobManager.JP_BarSinger(girl, true), "-", "Singer");
	Brothel_Data += girl->JobRating(m_JobManager.JP_BarPiano(girl, true), "-", "Piano");
	Brothel_Data += girl->JobRating(m_JobManager.JP_Escort(girl, true), "?", "Escort");
	Brothel_Data += "\n";
	Brothel_Data += girl->JobRating(m_JobManager.JP_HallDealer(girl, true), "-", "Dealer");
	Brothel_Data += girl->JobRating(m_JobManager.JP_HallEntertainer(girl, true), "-", "Entertainer");
	Brothel_Data += girl->JobRating(m_JobManager.JP_HallXXXEntertainer(girl, true), "-", "XXX Entertainer");
	Brothel_Data += girl->JobRating(m_JobManager.JP_HallWhore(girl, true), "?", "Hall Whore");
	Brothel_Data += "\n";
	Brothel_Data += girl->JobRating(m_JobManager.JP_SleazyBarmaid(girl, true), "-", "Club Barmaid");
	Brothel_Data += girl->JobRating(m_JobManager.JP_SleazyWaitress(girl, true), "-", "Club Waitress");
	Brothel_Data += girl->JobRating(m_JobManager.JP_BarStripper(girl, true), "-", "Stripper");
	Brothel_Data += girl->JobRating(m_JobManager.JP_BarWhore(girl, true), "?", "Club Whore");
	Brothel_Data += "\n";
	Brothel_Data += girl->JobRating(m_JobManager.JP_BrothelMasseuse(girl, true), "-", "Masseuse");
	Brothel_Data += girl->JobRating(m_JobManager.JP_BrothelStripper(girl, true), "-", "Brothel Stripper");
	Brothel_Data += girl->JobRating(m_JobManager.JP_PeepShow(girl, true), "-", "Peep Show");
	Brothel_Data += girl->JobRating(m_JobManager.JP_Whore(girl, true), "?", "Whore");
	Brothel_Data += girl->JobRating(m_JobManager.JP_WhoreStreets(girl, true), "?", "Whore on Streets");
	Brothel_Data += div;

	//STUDIO
	string Studio_Data = "";
	if (g_Studios.GetNumBrothels() > 0)
	{
		Studio_Data += "Studio Job Ratings\n";
		Studio_Data += girl->JobRating(m_JobManager.JP_FilmDirector(girl, true), "?", "Director");
		Studio_Data += girl->JobRating(m_JobManager.JP_FilmPromoter(girl, true), "?", "Promoter");
		Studio_Data += girl->JobRating(m_JobManager.JP_CameraMage(girl, true), "?", "Camera Mage");
		Studio_Data += girl->JobRating(m_JobManager.JP_CrystalPurifier(girl, true), "?", "Crystal Purifier");
		Studio_Data += girl->JobRating(m_JobManager.JP_Fluffer(girl, true), "?", "Fluffer");
		Studio_Data += girl->JobRating(m_JobManager.JP_FilmStagehand(girl, true), "?", "FilmStagehand");
		Studio_Data += "\n";
		//Studio_Data += girl->JobRating(m_JobManager.JP_FilmIdol(girl, true), "Idol Tape");
		Studio_Data += girl->JobRating(m_JobManager.JP_FilmAction(girl, true), "Action Film");
		Studio_Data += girl->JobRating(m_JobManager.JP_FilmMusic(girl, true), "Music Video");
		Studio_Data += girl->JobRating(m_JobManager.JP_FilmChef(girl, true), "Cookery Show");
		Studio_Data += girl->JobRating(m_JobManager.JP_FilmTease(girl, true), "Teaser Scene");
		Studio_Data += "\n";
		// Studio_Data += girl->JobRating(m_JobManager.JP_FilmStrip(girl, true), "Stripping Scene");
		// Studio_Data += girl->JobRating(m_JobManager.JP_FilmSex(girl, true), "* Film Sex");
		// Studio_Data += girl->JobRating(m_JobManager.JP_FilmAnal(girl, true), "* Film Anal");
		// Studio_Data += girl->JobRating(m_JobManager.JP_FilmLesbian(girl, true), "* Film Lesbian");
		// Studio_Data += girl->JobRating(m_JobManager.JP_FilmGroup(girl, true), "* Film Group");
		Studio_Data += girl->JobRating(m_JobManager.JP_FilmOral(girl, true), "Film Oral");
		// Studio_Data += girl->JobRating(m_JobManager.JP_FilmMast(girl, true), "* Film Mast");
		// Studio_Data += girl->JobRating(m_JobManager.JP_FilmTitty(girl, true), "* Film Titty");
		// Studio_Data += girl->JobRating(m_JobManager.JP_FilmHandJob(girl, true), "* Film HandJob");
		// Studio_Data += girl->JobRating(m_JobManager.JP_FilmFootJob(girl, true), "* Film FootJob");
		Studio_Data += "\n";
		Studio_Data += girl->JobRating(m_JobManager.JP_FilmThroat(girl, true), "FaceFuck");
		Studio_Data += girl->JobRating(m_JobManager.JP_FilmBuk(girl, true), "Bukkake Scene");
		Studio_Data += girl->JobRating(m_JobManager.JP_FilmBondage(girl, true), "Bondage Scene");
		Studio_Data += girl->JobRating(m_JobManager.JP_FilmPublicBDSM(girl, true), "Public Bondage");
		Studio_Data += girl->JobRating(m_JobManager.JP_FilmBeast(girl, true), "BeastPlay");

		// Studio_Data += girl->JobRating(m_JobManager.JP_FilmRandom(girl, true), "* Film Random");
		Studio_Data += div;
	}
	//ARENA
	string Arena_Data = "";
	if (g_Arena.GetNumBrothels() > 0)
	{
		Arena_Data += "Arena Job Ratings\n";
		Arena_Data += girl->JobRating(m_JobManager.JP_Doctore(girl, true), "-", "Doctore");
		Arena_Data += girl->JobRating(m_JobManager.JP_CityGuard(girl, true), "?", "City Guard");
		Arena_Data += girl->JobRating(m_JobManager.JP_Blacksmith(girl, true), "-", "Blacksmith");
		Arena_Data += girl->JobRating(m_JobManager.JP_Cobbler(girl, true), "-", "Cobbler");
		Arena_Data += girl->JobRating(m_JobManager.JP_Jeweler(girl, true), "-", "Jeweler");
		Arena_Data += "\n";
		Arena_Data += girl->JobRating(m_JobManager.JP_FightBeast(girl, true), "-", "Fight Beast");
		Arena_Data += girl->JobRating(m_JobManager.JP_FightArenaGirls(girl, true), "-", "Cage Match");
		Arena_Data += girl->JobRating(m_JobManager.JP_CombatTraining(girl, true), "!", "Combat Training");
		Arena_Data += div;
	}
	//CENTRE
	string Centre_Data = "";
	if (g_Centre.GetNumBrothels() > 0)
	{
		Centre_Data += "Centre Job Ratings\n";
		Centre_Data += girl->JobRating(m_JobManager.JP_CentreManager(girl, true), "-", "Centre Manager");
		Centre_Data += girl->JobRating(m_JobManager.JP_FeedPoor(girl, true), "-", "Feed Poor");
		Centre_Data += girl->JobRating(m_JobManager.JP_ComunityService(girl, true), "-", "Comunity Service");
		Centre_Data += "\n";
		Centre_Data += girl->JobRating(m_JobManager.JP_Counselor(girl, true), "-", "Counselor");
		Centre_Data += girl->JobRating(m_JobManager.JP_Rehab(girl, true), "!", "Rehab");
		Centre_Data += girl->JobRating(m_JobManager.JP_CentreTherapy(girl, true), "!", "Therapy");
		Centre_Data += girl->JobRating(m_JobManager.JP_CentreExTherapy(girl, true), "!", "Extreme Therapy");
		Centre_Data += girl->JobRating(m_JobManager.JP_CentreAngerManagement(girl, true), "!", "Anger Management");
		Centre_Data += div;
	}
	//CLINIC
	string Clinic_Data = "";
	if (g_Clinic.GetNumBrothels() > 0)
	{
		Clinic_Data += "Clinic Job Ratings\n";
		Clinic_Data += girl->JobRating(m_JobManager.JP_ChairMan(girl, true), "-", "Chairman");
		Clinic_Data += girl->JobRating(m_JobManager.JP_Doctor(girl, true), "-", "Doctor");
		Clinic_Data += girl->JobRating(m_JobManager.JP_Nurse(girl, true), "-", "Nurse");
		Clinic_Data += girl->JobRating(m_JobManager.JP_Mechanic(girl, true), "-", "Mechanic");
		Clinic_Data += girl->JobRating(m_JobManager.JP_Intern(girl, true), "!", "Intern");
		Clinic_Data += "\n";
		Clinic_Data += girl->JobRating(m_JobManager.JP_Healing(girl, true), "!", "Healing");
		Clinic_Data += girl->JobRating(m_JobManager.JP_RepairShop(girl, true), "!", "Repair Shop");
		Clinic_Data += girl->JobRating(m_JobManager.JP_CureDiseases(girl, true), "!", "Cure Diseases");
		Clinic_Data += girl->JobRating(m_JobManager.JP_GetAbort(girl, true), "!", "Get Abortion");
		Clinic_Data += girl->JobRating(m_JobManager.JP_CosmeticSurgery(girl, true), "!", "Cosmetic Surgery");
		Clinic_Data += girl->JobRating(m_JobManager.JP_Liposuction(girl, true), "!", "Liposuction");
		Clinic_Data += girl->JobRating(m_JobManager.JP_BreastReduction(girl, true), "!", "Breast Reduction");
		Clinic_Data += girl->JobRating(m_JobManager.JP_BoobJob(girl, true), "!", "Boob Job");
		Clinic_Data += girl->JobRating(m_JobManager.JP_GetVaginalRejuvenation(girl, true), "!", "Vaginal Rejuvenation");
		Clinic_Data += girl->JobRating(m_JobManager.JP_GetFacelift(girl, true), "!", "Facelift");
		Clinic_Data += girl->JobRating(m_JobManager.JP_GetAssJob(girl, true), "!", "Ass Job");
		Clinic_Data += girl->JobRating(m_JobManager.JP_GetTubesTied(girl, true), "!", "Tubes Tied");
		Clinic_Data += girl->JobRating(m_JobManager.JP_GetFertility(girl, true), "!", "Fertility");
		Clinic_Data += div;
	}
	//FARM
	string Farm_Data = "";
	if (g_Farm.GetNumBrothels() > 0)
	{
		Farm_Data += "Farm Job Ratings\n";
		Farm_Data += girl->JobRating(m_JobManager.JP_FarmManager(girl, true), "-", "Farm Manger");
		Farm_Data += girl->JobRating(m_JobManager.JP_FarmVeterinarian(girl, true), "-", "Veterinarian");
		Farm_Data += girl->JobRating(m_JobManager.JP_FarmMarketer(girl, true), "-", "Marketer");
		Farm_Data += girl->JobRating(m_JobManager.JP_FarmResearch(girl, true), "!", "Researcher");
		Farm_Data += girl->JobRating(m_JobManager.JP_FarmHand(girl, true), "-", "FarmHand");
		Farm_Data += "\n";
		Farm_Data += girl->JobRating(m_JobManager.JP_Farmer(girl, true), "-", "Farmer");
		Farm_Data += girl->JobRating(m_JobManager.JP_Gardener(girl, true), "-", "Gardener");
		Farm_Data += girl->JobRating(m_JobManager.JP_Shepherd(girl, true), "-", "Shepherd");
		Farm_Data += girl->JobRating(m_JobManager.JP_Rancher(girl, true), "-", "Rancher");
		Farm_Data += girl->JobRating(m_JobManager.JP_CatacombRancher(girl, true), "-", "Catacombs Rancher");
		Farm_Data += girl->JobRating(m_JobManager.JP_BeastCapture(girl, true), "-", "Beast Capture");
		Farm_Data += girl->JobRating(m_JobManager.JP_Milker(girl, true), "-", "Milker");
		Farm_Data += girl->JobRating(m_JobManager.JP_Milk(girl, true), "?", "Get Milked");
		Farm_Data += "\n";
		Farm_Data += girl->JobRating(m_JobManager.JP_Butcher(girl, true), "-", "Butcher");
		Farm_Data += girl->JobRating(m_JobManager.JP_Baker(girl, true), "-", "Baker");
		Farm_Data += girl->JobRating(m_JobManager.JP_Brewer(girl, true), "-", "Brewer");
		Farm_Data += girl->JobRating(m_JobManager.JP_Tailor(girl, true), "-", "Tailor");
		Farm_Data += girl->JobRating(m_JobManager.JP_MakeItem(girl, true), "-", "Make Item");
		Farm_Data += girl->JobRating(m_JobManager.JP_MakePotions(girl, true), "-", "Make Potion");
		Farm_Data += div;
	}
	//HOUSE
	string House_Data = "";
	House_Data += "House Job Ratings\n";
	House_Data += girl->JobRating(m_JobManager.JP_HeadGirl(girl, true), "-", "Head Girl");
	House_Data += girl->JobRating(m_JobManager.JP_Recruiter(girl, true), "-", "Recruiter");
	House_Data += girl->JobRating(m_JobManager.JP_PersonalTraining(girl, true), "!", "PersonalTraining");
	House_Data += girl->JobRating(m_JobManager.JP_FakeOrgasm(girl, true), "!", "Fake Orgasm");
	House_Data += girl->JobRating(m_JobManager.JP_SOStraight(girl, true), "!", "SO Straight");
	House_Data += girl->JobRating(m_JobManager.JP_SOBisexual(girl, true), "!", "SO Bisexual");
	House_Data += girl->JobRating(m_JobManager.JP_SOLesbian(girl, true), "!", "SO Lesbian");


	// House_Data += girl->JobRating(m_JobManager.JP_PersonalBedWarmer(girl, true), "* PersonalBedWarmer");
	House_Data += div;

	// `J` Show the current building first
	string data = "";
	/* */if (girl->m_InArena)		data += Arena_Data;
	else if (girl->m_InStudio)		data += Studio_Data;
	else if (girl->m_InCentre)		data += Centre_Data;
	else if (girl->m_InClinic)		data += Clinic_Data;
	else if (girl->m_InFarm)		data += Farm_Data;
	else if (girl->m_InHouse)		data += House_Data;
	else /*                    */	data += Brothel_Data;

	// `J` show all the other buildings
	data += div;
	if (girl->m_InStudio || girl->m_InArena || girl->m_InCentre || girl->m_InClinic || girl->m_InFarm || girl->m_InHouse)
		/*                       */	data += Brothel_Data;
	if (!girl->m_InStudio)	data += Studio_Data;
	if (!girl->m_InArena)		data += Arena_Data;
	if (!girl->m_InCentre)		data += Centre_Data;
	if (!girl->m_InClinic)		data += Clinic_Data;
	if (!girl->m_InFarm)		data += Farm_Data;
	if (!girl->m_InHouse)		data += House_Data;

	// `J` finish with the explanation
	data += div;
	data += "Job Ratings range from\n'I' The absolute best, 'S' Superior,\n";
	data += "Then 'A'-'E' with 'E' being the worst.\n'X' means they can not do the job.\n \n";
	data += "Jobs marked with ? do not really use job performace directly and is an estimate.\n";
	data += "Jobs marked with ! are how much the girl is in need of the service of that job.\n";
	data += "Jobs marked with * do not use job performace at all and are just in for completion.\n";
	return data;

}

string cGirls::GetSimpleDetails(sGirl* girl, int fontsize)
{
	if (girl == 0) return "";
	if (fontsize < 8) fontsize = 8;
	stringstream ss;
	cFont check; int w, h, size = 0;
	check.LoadFont(cfg.fonts.normal(), fontsize);
	string sper = ""; if (cfg.fonts.showpercent()) sper = " %";

	// `J` When modifying Stats or Skills, search for "J-Change-Stats-Skills"  :  found in >> cGirls.cpp > GetDetailsString
	string basestr[] = { "Age : ", "Rebelliousness : ", "Looks : ", "Constitution : ", "Health : ", "Happiness : ", "Tiredness : ", "Level : ", "Exp : ", "Location : ", "Day Job : ", "Night Job : " };
	int basecount = 12;
	int skillnum[] = { SKILL_MAGIC, SKILL_COMBAT, SKILL_SERVICE, SKILL_MEDICINE, SKILL_PERFORMANCE, SKILL_CRAFTING, SKILL_HERBALISM, SKILL_FARMING, SKILL_BREWING, SKILL_ANIMALHANDLING, SKILL_COOKING, SKILL_ANAL, SKILL_BDSM, SKILL_NORMALSEX, SKILL_BEASTIALITY, SKILL_GROUP, SKILL_LESBIAN, SKILL_ORALSEX, SKILL_TITTYSEX, SKILL_HANDJOB, SKILL_STRIP, SKILL_FOOTJOB };
	string skillstr[] = { "Magic : ", "Combat : ", "Service : ", "Medicine : ", "Performance : ", "Crafting : ", "Herbalism : ", "Farming : ", "Brewing : ", "Animal Handling : ", "Cooking : ", "Anal : ", "BDSM : ", "Normal : ", "Bestiality : ", "Group : ", "Lesbian : ", "Oral : ", "Titty : ", "Hand Job : ", "Stripping : ", "Foot Job : " };
	int skillcount = 22;
	int statnum[] = { STAT_CHARISMA, STAT_BEAUTY, STAT_LIBIDO, STAT_MANA, STAT_INTELLIGENCE, STAT_CONFIDENCE, STAT_OBEDIENCE, STAT_SPIRIT, STAT_AGILITY, STAT_STRENGTH, STAT_FAME, STAT_LACTATION };
	string statstr[] = { "Charisma : ", "Beauty : ", "Libido : ", "Mana : ", "Intelligence : ", "Confidence : ", "Obedience : ", "Spirit : ", "Agility : ", "Strength : ", "Fame : ", "Lactation : " };
	int statcount = 12;

	// get the widest
	for (int i = 0; i < basecount; i++)		{ check.GetSize(basestr[i], w, h);	if (w > size) size = w; }
	for (int i = 0; i < skillcount; i++)	{ check.GetSize(skillstr[i], w, h);	if (w > size) size = w; }
	for (int i = 0; i < statcount; i++)		{ check.GetSize(statstr[i], w, h);	if (w > size) size = w; }
	size += 5; // add a little padding
	// then add extra spaces until it is longer than the widest
	for (int i = 0; i < basecount; i++)		{ check.GetSize(basestr[i], w, h);	while (w < size)	{ basestr[i] += " "; check.GetSize(basestr[i], w, h); } }
	for (int i = 0; i < skillcount; i++)	{ check.GetSize(skillstr[i], w, h);	while (w < size)	{ skillstr[i] += " "; check.GetSize(skillstr[i], w, h); } }
	for (int i = 0; i < statcount; i++)		{ check.GetSize(statstr[i], w, h);	while (w < size)	{ statstr[i] += " "; check.GetSize(statstr[i], w, h); } }


	ss << basestr[9] << girl->lookup_where_she_is();
	ss << "\n" << basestr[10] << g_Brothels.m_JobManager.JobName[girl->m_DayJob];
	ss << "\n" << basestr[11] << g_Brothels.m_JobManager.JobName[girl->m_NightJob];
	ss << "\n" << basestr[2] << (girl->beauty() + girl->charisma()) / 2 << sper;
	ss << "\n" << statstr[0] << girl->charisma() << sper;
	ss << "\n" << statstr[1] << girl->beauty() << sper;
	ss << "\n" << basestr[7] << girl->level();
	ss << "\n" << basestr[8] << girl->exp();
	ss << "\n" << basestr[0]; if (girl->age() == 100) ss << "Unknown"; else ss << girl->age();
	ss << "\n" << basestr[1] << girl->rebel();
	ss << "\n" << basestr[3] << girl->constitution() << sper;
	ss << "\n" << basestr[4] << girl->health() << sper;
	ss << "\n" << basestr[5] << girl->happiness() << sper;
	ss << "\n" << basestr[6] << girl->tiredness() << sper;
	for (int i = 2; i < statcount; i++)	{ ss << "\n" << statstr[i] << girl->get_stat(statnum[i]) << sper; }
	ss << "\n";	if (girl->is_slave())				{ ss << "Is Branded a Slave"; }
	ss << "\n";	if (girl->check_virginity())	{ ss << "She is a Virgin"; }
	int to_go = (girl->carrying_monster() ? cfg.pregnancy.weeks_monster_p() : cfg.pregnancy.weeks_pregnant()) - girl->m_WeeksPreg;
	ss << "\n";	if (girl->m_States&(1 << STATUS_PREGNANT))		{ ss << "Is pregnant " << "(" << to_go << ")"; }
	else if (girl->m_States&(1 << STATUS_PREGNANT_BY_PLAYER))	{ ss << "Is pregnant with your child " << "(" << to_go << ")"; }
	else if (girl->m_States&(1 << STATUS_INSEMINATED))			{ ss << "Is inseminated " << "(" << to_go << ")"; }
	ss << "\n";	if (girl->is_addict() && !girl->has_disease())	{ ss << "Has an addiciton"; }
	else if (!girl->is_addict() && girl->has_disease())			{ ss << "Has a disease"; }
	else if (girl->is_addict() && girl->has_disease())			{ ss << "Has an addiciton and a disease"; }
	for (int i = 0; i < skillcount; i++)	{ ss << "\n" << skillstr[i] << girl->get_skill(skillnum[i]) << sper; }
	ss << "\n \n";	int trait_count = 0;
	for (int i = 0; i < MAXNUM_TRAITS; i++)
	{
		if (!girl->m_Traits[i]) continue;
		trait_count++;
		if (trait_count > 1) ss << ",   ";
		ss << girl->m_Traits[i]->display_name();
		if (girl->m_TempTrait[i] > 0) ss << " (" << girl->m_TempTrait[i] << ")";
	}
	return ss.str();
}

// added human check: -1 does not matter, 0 not human, 1 human
sGirl* cGirls::GetUniqueYourDaughterGirl(int Human0Monster1)
{
	if (GetNumYourDaughterGirls() == 0) return 0;
	sGirl *girl;
	vector<sGirl *> v;
	for (girl = m_Parent; girl; girl = girl->m_Next)
	{
		if (girl->is_yourdaughter())
			if (Human0Monster1 == -1 ||
				(Human0Monster1 == 1 && !girl->is_human()) ||
				(Human0Monster1 == 0 && girl->is_human()))
				v.push_back(girl);
	}

	return v[g_Dice%v.size()];
}

sGirl* cGirls::GetRandomGirl(bool slave, bool catacomb, bool arena, bool daughter, bool isdaughter)
{
	int num_girls = m_NumGirls;
	if ((num_girls == GetNumSlaveGirls() + GetNumCatacombGirls() + GetNumArenaGirls() + GetNumYourDaughterGirls() + GetNumIsDaughterGirls()) || num_girls == 0)
	{
		int r = 3;
		while (r)
		{
			CreateRandomGirl(0, true);
			r--;
		}
	}
	GirlPredicate_GRG pred(slave, catacomb, arena, daughter, isdaughter);
	vector<sGirl *> girls = get_girls(&pred);
	if (girls.size() == 0) return 0;
	return girls[g_Dice.random(girls.size())];
}

sGirl* cGirls::GetGirl(int girl)
{
	int count = 0;
	sGirl* current = m_Parent;
	if (girl < 0 || (unsigned int)girl >= m_NumGirls)		return 0;
	while (current)
	{
		if (count == girl)	return current;
		count++;
		current = current->m_Next;
	}
	return 0;
}

int cGirls::GetRebelValue(sGirl* girl, bool matron)
{
	/*
	*	WD:	Added test to ingnore STAT_HOUSE value
	*	if doing a job that the palyer is paying
	*	only when processing Day or Night Shift
	*
	*	This is to make it so that the jobs that
	*	cost the player support where the hosue take
	*	has no effect has no impact on the chance of
	*	refusal.
	*/

	if (girl->has_trait("Broken Will"))	return -100;
	int chanceNo = 0;
	int houseStat = girl->house();
	int happyStat = girl->happiness();
	bool girlIsSlave = girl->is_slave();

	// a matron (or torturer in dungeon) will help convince a girl to obey
	if (matron)	chanceNo -= 15;

	chanceNo -= girl->pclove() / 5;
	chanceNo += girl->spirit() / 2;
	chanceNo -= girl->obedience() / 5;

	// having a guarding gang will enforce order
	sGang* gang = g_Gangs.GetGangOnMission(MISS_GUARDING);
	if (gang)	chanceNo -= 10;

	chanceNo += girl->tiredness() / 10;	// Tired girls increase Rebel

	if (happyStat < 50)								// Unhappy girls increase Rebel
	{
		chanceNo += (50 - happyStat) / 5;
		if (happyStat < 10)							// WD:	Fixed missing case Happiness < 10
			chanceNo += 10 - happyStat;				// WD:	Rebel increases by an additional point if happy < 10
	}
	else	chanceNo -= (happyStat - 50) / 10;		// happy girls are less cranky, less Rebel

	// House Take has no effect on slaves
	if (girlIsSlave)	chanceNo -= 15;				// Slave Girl lowers rebelinous of course
	else
	{
		chanceNo += 15;								// Free girls are a little more rebelious
		// WD	House take of gold has no affect on rebellion if
		//		job is paid by player. eg Matron / cleaner
		if ((g_Brothels.is_Dayshift_Processing() && g_Brothels.m_JobManager.is_job_Paid_Player(girl->m_DayJob)) ||
			(g_Brothels.is_Nightshift_Processing() && g_Brothels.m_JobManager.is_job_Paid_Player(girl->m_NightJob)))
			houseStat = 0;

		if (houseStat < 60)							// Take less money than normal, lower Rebel
			chanceNo -= (60 - houseStat) / 2;
		else
		{
			chanceNo += (houseStat - 60) / 2;		// Take more money than normal, more Rebel
			if (houseStat >= 100) chanceNo += 10;	// Take all the money, more Rebel
		}
	}

	/*
	*	`J` "Kidnapped" and "Emprisoned Customer" are factored in twice, before and after mental trait modifiers
	*	This will allow them to have at least some effect on "Mind Fucked", "Dependant" or "Meek" girls
	*/

	// these are factoring in twice before and after mental trait modifiers
	if (girl->has_trait("Kidnapped") || girl->has_trait("Emprisoned Customer"))	chanceNo += 10;
	int kep = HasTempTrait(girl, "Kidnapped") + HasTempTrait(girl, "Emprisoned Customer");
	if (kep > 20) kep += 20; else if (kep > 10) kep += 10;

	// guarantee certain rebelliousness values for specific traits
	if (girl->has_trait("Retarded"))	chanceNo -= 30;
	if (girl->has_trait("Mind Fucked") && chanceNo > -50)	chanceNo = -50;
	if (girl->has_trait("Dependant") && chanceNo > -40)		chanceNo = -40;
	if (girl->has_trait("Meek") && chanceNo > 20)			chanceNo = 20;

	chanceNo += kep;

	// `J` What type of accommodations she is held in will affect her a lot.
	int accommod = girl->m_AccLevel - g_Girls.PreferredAccom(girl);
	if (accommod < -4)
	{
		chanceNo -= accommod * 2;
	}
	else if (accommod < -1)
	{
		chanceNo -= accommod;
	}
	else if (accommod > 1)
	{
		chanceNo -= accommod * 2;
	}


	// Normalise
	if (chanceNo < -100)		chanceNo = -100;
	else if (chanceNo > 100)	chanceNo = 100;
	return chanceNo;
}

int cGirls::GetNumCatacombGirls()
{
	int number = 0;
	sGirl* current = m_Parent;
	while (current)
	{
		if (current->m_States&(1 << STATUS_CATACOMBS))
			number++;
		current = current->m_Next;
	}
	return number;
}

int cGirls::GetNumSlaveGirls()
{
	int number = 0;
	sGirl* current = m_Parent;
	while (current)
	{
		if (current->m_States&(1 << STATUS_SLAVE))
			number++;
		current = current->m_Next;
	}
	return number;
}

int cGirls::GetNumArenaGirls()
{
	int number = 0;
	sGirl* current = m_Parent;
	while (current)
	{
		if (current->m_States&(1 << STATUS_ARENA))
			number++;
		current = current->m_Next;
	}
	return number;
}

int cGirls::GetNumYourDaughterGirls()
{
	int number = 0;
	sGirl* current = m_Parent;
	while (current)
	{
		if (current->m_States&(1 << STATUS_YOURDAUGHTER))
			number++;
		current = current->m_Next;
	}
	return number;
}

int cGirls::GetNumIsDaughterGirls()
{
	int number = 0;
	sGirl* current = m_Parent;
	while (current)
	{
		if (current->m_States&(1 << STATUS_ISDAUGHTER))
			number++;
		current = current->m_Next;
	}
	return number;
}

// ----- Stat

void cGirls::SetStat(sGirl* girl, int a_stat, int amount)
{
	u_int stat = a_stat;
	int amt = amount;       // Modifying amount directly isn't a good idea
	int min = 0, max = 100;
	switch (stat)
	{
	case STAT_AGE:		// age is a special case so we start with that
		if (girl->m_Stats[STAT_AGE] > 99)		girl->m_Stats[stat] = 100;
		else if (girl->m_Stats[stat] > 80)		girl->m_Stats[stat] = 80;
		else if (girl->m_Stats[stat] < 18)		girl->m_Stats[stat] = 18;	// `J` Legal Note: 18 is the Legal Age of Majority for the USA where I live
		else									girl->m_Stats[stat] = amt;
		return; break;	// and just return instead of going to the end
	case STAT_HEALTH:
		if (girl->has_trait( "Incorporeal"))	// Health and tiredness need the incorporeal sanity check
		{
			girl->m_Stats[stat] = 100;
			return;
		}
		break;
	case STAT_TIREDNESS:
		if (girl->has_trait( "Incorporeal") ||	// Health and tiredness need the incorporeal sanity check
			girl->has_trait( "Skeleton") ||
			girl->has_trait( "Zombie"))
		{
			girl->m_Stats[stat] = 0;
			return;
		}
		break;
	case STAT_EXP:
		max = 32000;
		break;
	case STAT_LEVEL:
		max = 255;
		break;
	case STAT_PCFEAR:
	case STAT_PCHATE:
	case STAT_PCLOVE:
	case STAT_MORALITY:
	case STAT_REFINEMENT:
	case STAT_DIGNITY:
	case STAT_LACTATION:
		min = -100;
		break;
	default:
		break;
	}
	if (amt > max) amt = max;
	else if (amt < min) amt = min;
	girl->m_Stats[stat] = amt;
}

void cGirls::UpdateStatMod(sGirl* girl, int stat, int amount)
{
	if (stat == STAT_HEALTH || stat == STAT_HAPPINESS || stat == STAT_TIREDNESS || stat == STAT_EXP ||
		stat == STAT_LEVEL || stat == STAT_HOUSE || stat == STAT_ASKPRICE)
	{
		girl->upd_stat(stat, amount);
		return;
	}
	girl->m_StatMods[stat] += amount;
}

void cGirls::UpdateStatTr(sGirl* girl, int stat, int amount)
{
	if (stat == STAT_HEALTH || stat == STAT_HAPPINESS || stat == STAT_TIREDNESS || stat == STAT_EXP ||
		stat == STAT_LEVEL || stat == STAT_HOUSE || stat == STAT_ASKPRICE)
	{
		girl->upd_stat(stat, amount);
		return;
	}
	girl->m_StatTr[stat] += amount;
}

void cGirls::updateTemp(sGirl* girl)	// `J` group all the temp updates into one area
{
	updateTempStats(girl);		// update temp stats
	updateTempSkills(girl);		// update temp skills
	updateTempTraits(girl);		// update temp traits
	updateTempEnjoyment(girl);		// update temp enjoyment
}

void cGirls::updateTempStats(sGirl* girl)	// Normalise to zero by 30% each week
{
	if (girl->is_dead()) return;		// Sanity check. Abort on dead girl
	for (int i = 0; i < NUM_STATS; i++)
	{
		if (girl->m_StatTemps[i] != 0)				// normalize towards 0 by 30% each week
		{
			int newStat = (int)(float(girl->m_StatTemps[i]) * 0.7);
			if (newStat != girl->m_StatTemps[i]) girl->m_StatTemps[i] = newStat;
			else if (girl->m_StatTemps[i] > 0)	girl->m_StatTemps[i]--;	// if 30% did nothing, go with 1 instead
			else if (girl->m_StatTemps[i] < 0)	girl->m_StatTemps[i]++;
		}
	}
}

// ----- Skill

double cGirls::GetAverageOfAllSkills(sGirl* girl)
{
	return ((girl->anal() + girl->animalhandling() + girl->bdsm() + girl->beastiality() + girl->brewing()
		+ girl->combat() + girl->cooking()+ girl->crafting() + girl->farming() + girl->footjob() + girl->group() + girl->handjob()
		+ girl->herbalism() + girl->lesbian() + girl->magic() + girl->medicine() + girl->normalsex() + girl->oralsex()
		+ girl->performance() + girl->service() + girl->strip() + girl->tittysex()) / 22.0);
}
double cGirls::GetAverageOfNSxSkills(sGirl* girl)
{
	return ((girl->animalhandling() + girl->brewing() + girl->combat() + girl->cooking() + girl->crafting() + girl->farming()
		+ girl->herbalism() + girl->magic() + girl->medicine() + girl->performance() + girl->service()
		) / 11.0);
}
double cGirls::GetAverageOfSexSkills(sGirl* girl)
{
	return ((girl->anal() + girl->bdsm() + girl->beastiality() + girl->footjob() + girl->group() + girl->handjob()
		+ girl->lesbian() + girl->normalsex() + girl->oralsex() + girl->strip() + girl->tittysex()) / 11.0);
}


// set the skill to amount
void cGirls::SetSkill(sGirl* girl, int skill, int amount)
{
	girl->m_Skills[skill] = amount;
}
// total of all skills
int cGirls::GetSkillWorth(sGirl* girl)
{
	int num = 0;
	for (u_int i = 0; i < NUM_SKILLS; i++) num += (int)girl->m_Skills[i];
	return num;
}

// add amount to skillmod
void cGirls::UpdateSkillMod(sGirl* girl, int skill, int amount)
{
	girl->m_SkillMods[skill] += amount;
}
// add amount to skillTr
void cGirls::UpdateSkillTr(sGirl* girl, int skill, int amount)
{
	girl->m_SkillTr[skill] += amount;
}

// Normalise to zero by 30%
void cGirls::updateTempSkills(sGirl* girl)
{
	// Sanity check. Abort on dead girl
	if (girl->is_dead()) return;

	for (u_int i = 0; i < NUM_SKILLS; i++)
	{
		if (girl->m_SkillTemps[i] != 0)
		{											// normalize towards 0 by 30% each week
			int newSkill = (int)(float(girl->m_SkillTemps[i]) * 0.7);
			if (newSkill != girl->m_SkillTemps[i])
				girl->m_SkillTemps[i] = newSkill;
			else
			{										// if 30% did nothing, go with 1 instead
				if (girl->m_SkillTemps[i] > 0)
					girl->m_SkillTemps[i]--;
				else if (girl->m_SkillTemps[i] < 0)
					girl->m_SkillTemps[i]++;
			}
		}
	}
}

// ----- Load save

// This load

bool sGirl::LoadGirlXML(TiXmlHandle hGirl)
{
	//this is always called after creating a new girl, so let's not init sGirl again
	TiXmlElement* pGirl = hGirl.ToElement();
	if (pGirl == 0) return false;
	int tempInt = 0;

	// load the name
	m_Name = pGirl->Attribute("Name");		// the name the girl is based on, also the name of the image folder

	// m_Realname = the name the girl is called in the game // `J` used to set to m_Name but now gets built from F+M+S names
	m_Realname = (pGirl->Attribute("Realname") ? pGirl->Attribute("Realname") : "");
	m_FirstName = (pGirl->Attribute("FirstName") ? pGirl->Attribute("FirstName") : "");	// `J` New
	m_MiddleName = (pGirl->Attribute("MiddleName") ? pGirl->Attribute("MiddleName") : "");	// `J` New
	m_Surname = (pGirl->Attribute("Surname") ? pGirl->Attribute("Surname") : "");	// `J` New
	if (m_Realname == "" || (m_FirstName == "" && m_MiddleName == "" && m_Surname == "")) g_Girls.BuildName(this);

	m_Desc = (pGirl->Attribute("Desc") ? pGirl->Attribute("Desc") : "-");	// get the description

	// load the amount of days they are unhappy in a row
	pGirl->QueryIntAttribute("DaysUnhappy", &tempInt); m_DaysUnhappy = tempInt; tempInt = 0;

	// Load their traits
	LoadTraitsXML(hGirl.FirstChild("Traits"), m_NumTraits, m_Traits, m_TempTrait);
	if (m_NumTraits > MAXNUM_TRAITS)
		g_LogFile.write("--- ERROR - Loaded more traits than girls can have??");

	g_Girls.MutuallyExclusiveTraits(this, 1);	// cleanup traits
	g_Girls.RemoveAllRememberedTraits(this);	// and clear any thing left after the cleanup

	// Load their remembered traits
	LoadTraitsXML(hGirl.FirstChild("Remembered_Traits"), m_NumRememTraits, m_RememTraits);
	if (m_NumRememTraits > MAXNUM_TRAITS * 2)
		g_LogFile.write("--- ERROR - Loaded more remembered traits than girls can have??");

	// Load inventory items
	LoadInventoryXML(hGirl.FirstChild("Inventory"), m_Inventory, m_NumInventory, m_EquipedItems);

	// load their states
	pGirl->QueryValueAttribute<long>("States", &m_States);

	// load their stats
	LoadStatsXML(hGirl.FirstChild("Stats"), m_Stats, m_StatMods, m_StatTemps);

	// load their skills
	LoadSkillsXML(hGirl.FirstChild("Skills"), m_Skills, m_SkillMods, m_SkillTemps);

	// load enjoyment values
	LoadActionsXML(hGirl.FirstChildElement("Actions"), m_Enjoyment, m_EnjoymentMods, m_EnjoymentTemps);

	// load training values
	LoadTrainingXML(hGirl.FirstChildElement("Training"), m_Training, m_TrainingMods, m_TrainingTemps);

	// load virginity
	pGirl->QueryIntAttribute("Virgin", &m_Virgin);

	// load using antipreg
	pGirl->QueryValueAttribute<bool>("UseAntiPreg", &m_UseAntiPreg);

	// load withdrawals
	pGirl->QueryIntAttribute("Withdrawals", &tempInt); m_Withdrawals = tempInt; tempInt = 0;

	// load money
	pGirl->QueryIntAttribute("Money", &m_Money);

	// load working day counter
	pGirl->QueryIntAttribute("WorkingDay", &m_WorkingDay);
	pGirl->QueryIntAttribute("PrevWorkingDay", &m_PrevWorkingDay);	// `J` added
	pGirl->QueryIntAttribute("SpecialJobGoal", &m_SpecialJobGoal);	// `J` added
	if (m_WorkingDay < 0)		m_WorkingDay = 0;
	if (m_PrevWorkingDay < 0)	m_PrevWorkingDay = 0;
	if (m_SpecialJobGoal < 0)	m_SpecialJobGoal = 0;

	// load acom level
	pGirl->QueryIntAttribute("AccLevel", &tempInt); m_AccLevel = tempInt; tempInt = 0;

	// `J` changeing jobs to save as quick codes in stead of numbers so if new jobs are added they don't shift jobs
	string tempst = pGirl->Attribute("DayJob");			m_DayJob = lookup_jobs_code(tempst);
	tempst = pGirl->Attribute("NightJob");				m_NightJob = lookup_jobs_code(tempst);
	tempst = pGirl->Attribute("PrevDayJob");			m_PrevDayJob = lookup_jobs_code(tempst);
	tempst = pGirl->Attribute("PrevNightJob");			m_PrevNightJob = lookup_jobs_code(tempst);
	tempst = pGirl->Attribute("YesterDayJob");			m_YesterDayJob = lookup_jobs_code(tempst);
	tempst = pGirl->Attribute("YesterNightJob");		m_YesterNightJob = lookup_jobs_code(tempst);

	// `J` now check if any of the jobs failed the lookup and see if they are numbers - aka use the old code
	// load day/night jobs
	if (m_DayJob < 0 || m_DayJob > 255)					{ pGirl->QueryIntAttribute("DayJob", &tempInt);			m_DayJob = tempInt; tempInt = 0; }
	if (m_NightJob < 0 || m_NightJob >255)				{ pGirl->QueryIntAttribute("NightJob", &tempInt);		m_NightJob = tempInt; tempInt = 0; }
	if (m_PrevDayJob < 0 || m_PrevDayJob >255)			{ pGirl->QueryIntAttribute("PrevDayJob", &tempInt);		m_PrevDayJob = tempInt; tempInt = 0; }
	if (m_PrevNightJob < 0 || m_PrevNightJob >255)		{ pGirl->QueryIntAttribute("PrevNightJob", &tempInt);	m_PrevNightJob = tempInt; tempInt = 0; }
	if (m_YesterDayJob < 0 || m_YesterDayJob >255)		{ pGirl->QueryIntAttribute("YesterDayJob", &tempInt);	m_YesterDayJob = tempInt; tempInt = 0; }
	if (m_YesterNightJob < 0 || m_YesterNightJob >255)	{ pGirl->QueryIntAttribute("YesterNightJob", &tempInt);	m_YesterNightJob = tempInt; tempInt = 0; }

	if (m_YesterDayJob < 0)		m_YesterDayJob = 255;
	if (m_YesterNightJob < 0)	m_YesterNightJob = 255;

	// load runnayway value
	pGirl->QueryIntAttribute("RunAway", &tempInt); m_RunAway = tempInt; tempInt = 0;

	// load spotted
	pGirl->QueryIntAttribute("Spotted", &tempInt); m_Spotted = tempInt; tempInt = 0;

	// load newRandomFixed
	tempInt = -1;
	pGirl->QueryIntAttribute("NewRandomFixed", &tempInt); m_newRandomFixed = tempInt; tempInt = 0;

	// load weeks past, birth day, and pregant time
	pGirl->QueryValueAttribute<unsigned long>("WeeksPast", &m_WeeksPast);
	pGirl->QueryValueAttribute<unsigned int>("BDay", &m_BDay);
	pGirl->QueryIntAttribute("WeeksPreg", &tempInt); m_WeeksPreg = tempInt; tempInt = 0;

	// load number of customers slept with
	pGirl->QueryValueAttribute<unsigned long>("NumCusts", &m_NumCusts);
	m_NumCusts_old = m_NumCusts;

	// load girl flags
	TiXmlElement* pFlags = pGirl->FirstChildElement("Flags");
	if (pFlags)
	{
		std::string flagNumber;
		for (int i = 0; i < NUM_GIRLFLAGS; i++)
		{
			flagNumber = "Flag_";
			std::stringstream stream;
			stream << i;
			flagNumber.append(stream.str());
			pFlags->QueryIntAttribute(flagNumber, &tempInt); m_Flags[i] = tempInt; tempInt = 0;
		}
	}

	// load their torture value
	pGirl->QueryValueAttribute<bool>("Tort", &m_Tort);

	// Load their children
	pGirl->QueryIntAttribute("PregCooldown", &tempInt); m_PregCooldown = tempInt; tempInt = 0;

	// load number of children
	TiXmlElement* pChildren = pGirl->FirstChildElement("Children");
	for (int i = 0; i < CHILD_COUNT_TYPES; i++)		// `J` added
	{
		pChildren->QueryIntAttribute(children_type_names[i], &tempInt);
		if (tempInt < 0 || tempInt>1000) tempInt = 0;
		m_ChildrenCount[i] = tempInt;
		tempInt = 0;
	}
	if (pChildren)
	{
		for (TiXmlElement* pChild = pChildren->FirstChildElement("Child"); pChild != 0; pChild = pChild->NextSiblingElement("Child"))
		{
			sChild* child = new sChild(0, sChild::Girl, 0);				// `J` prepare a minimal new child
			bool success = child->LoadChildXML(TiXmlHandle(pChild));	// because this will load over top of it
			if (success == true) { m_Children.add_child(child); }		// add it if it loaded
			else { delete child; continue; }							// or delete the failed load
		}
	}


	// load their triggers
	m_Triggers.LoadTriggersXML(hGirl.FirstChildElement("Triggers"));
	m_Triggers.SetGirlTarget(this);



	g_Girls.ApplyTraits(this);
	if (m_Stats[STAT_AGE] < 18) m_Stats[STAT_AGE] = 18;	// `J` Legal Note: 18 is the Legal Age of Majority for the USA where I live

	g_Girls.CalculateGirlType(this);

	// get the number of daugher names
	/*ifs  >>  temp >> ws;
	for(int i = 0; i < temp; i++) {
	string s;

	ifs >> s >> ws;
	current->m_Canonical_Daughters.push_back(s);*/

	return true;
}

// This save

TiXmlElement* sGirl::SaveGirlXML(TiXmlElement* pRoot)
{
	TiXmlElement* pGirl = new TiXmlElement("Girl");
	pRoot->LinkEndChild(pGirl);
	pGirl->SetAttribute("Name", m_Name);						// save the name
	pGirl->SetAttribute("Realname", m_Realname);				// save the real name
	pGirl->SetAttribute("FirstName", m_FirstName);				// save the first name
	pGirl->SetAttribute("MiddleName", m_MiddleName);			// save the middle name
	pGirl->SetAttribute("Surname", m_Surname);					// save the surname
	pGirl->SetAttribute("Desc", m_Desc);						// save the description
	pGirl->SetAttribute("DaysUnhappy", m_DaysUnhappy);			// save the amount of days they are unhappy

	// Save their traits
	if (m_NumTraits > MAXNUM_TRAITS) g_LogFile.write("---- ERROR - Saved more traits then girls can have");
	SaveTraitsXML(pGirl, "Traits", MAXNUM_TRAITS, m_Traits, m_TempTrait);

	// Save their remembered traits
	if (m_NumRememTraits > MAXNUM_TRAITS * 2) g_LogFile.write("---- ERROR - Saved more remembered traits then girls can have");
	SaveTraitsXML(pGirl, "Remembered_Traits", MAXNUM_TRAITS * 2, m_RememTraits, 0);

	// Save inventory items
	TiXmlElement* pInventory = new TiXmlElement("Inventory");
	pGirl->LinkEndChild(pInventory);
	SaveInventoryXML(pInventory, m_Inventory, 40, m_EquipedItems);

	pGirl->SetAttribute("States", m_States);					// save their states
	SaveStatsXML(pGirl, m_Stats, m_StatMods, m_StatTemps);		// Save their stats
	SaveSkillsXML(pGirl, m_Skills, m_SkillMods, m_SkillTemps);	// save their skills

	pGirl->SetAttribute("Virgin", m_Virgin);					// save virginity
	pGirl->SetAttribute("UseAntiPreg", m_UseAntiPreg);			// save using antipreg
	pGirl->SetAttribute("Withdrawals", m_Withdrawals);			// save withdrawals
	pGirl->SetAttribute("Money", m_Money);						// save money
	pGirl->SetAttribute("AccLevel", m_AccLevel);				// save acom level

	// save working day counter
	pGirl->SetAttribute("WorkingDay", m_WorkingDay);
	pGirl->SetAttribute("PrevWorkingDay", m_PrevWorkingDay);	// `J` added
	pGirl->SetAttribute("SpecialJobGoal", m_SpecialJobGoal);	// `J` added

	// `J` changed jobs to save as quick codes in stead of numbers so if new jobs are added they don't shift jobs
	// save day/night jobs
	if (m_DayJob < 0 || m_DayJob > NUM_JOBS) pGirl->SetAttribute("DayJob", "255");
	else pGirl->SetAttribute("DayJob", g_Brothels.m_JobManager.JobQkNm[m_DayJob]);
	if (m_NightJob < 0 || m_NightJob > NUM_JOBS) pGirl->SetAttribute("NightJob", "255");
	else pGirl->SetAttribute("NightJob", g_Brothels.m_JobManager.JobQkNm[m_NightJob]);

	// save prev day/night jobs
	if (m_PrevDayJob < 0 || m_PrevDayJob > NUM_JOBS) pGirl->SetAttribute("PrevDayJob", "255");
	else pGirl->SetAttribute("PrevDayJob", g_Brothels.m_JobManager.JobQkNm[m_PrevDayJob]);
	if (m_PrevNightJob < 0 || m_PrevNightJob > NUM_JOBS) pGirl->SetAttribute("PrevNightJob", "255");
	else pGirl->SetAttribute("PrevNightJob", g_Brothels.m_JobManager.JobQkNm[m_PrevNightJob]);

	// save prev day/night jobs
	if (m_YesterDayJob < 0 || m_YesterDayJob > NUM_JOBS) pGirl->SetAttribute("YesterDayJob", "255");
	else pGirl->SetAttribute("YesterDayJob", g_Brothels.m_JobManager.JobQkNm[m_YesterDayJob]);
	if (m_YesterNightJob < 0 || m_YesterNightJob > NUM_JOBS) pGirl->SetAttribute("YesterNightJob", "255");
	else pGirl->SetAttribute("YesterNightJob", g_Brothels.m_JobManager.JobQkNm[m_YesterNightJob]);

	pGirl->SetAttribute("RunAway", m_RunAway);					// save runnayway vale
	pGirl->SetAttribute("Spotted", m_Spotted);					// save spotted

	if (m_newRandomFixed >= 0)	pGirl->SetAttribute("NewRandomFixed", m_newRandomFixed);

	// save weeks past, birth day, and pregant time
	pGirl->SetAttribute("WeeksPast", m_WeeksPast);
	pGirl->SetAttribute("BDay", m_BDay);
	pGirl->SetAttribute("WeeksPreg", m_WeeksPreg);

	pGirl->SetAttribute("NumCusts", m_NumCusts);				// number of customers slept with

	// girl flags
	TiXmlElement* pFlags = new TiXmlElement("Flags");
	pGirl->LinkEndChild(pFlags);
	std::string flagNumber;
	for (int i = 0; i < NUM_GIRLFLAGS; i++)
	{
		flagNumber = "Flag_";
		std::stringstream stream;
		stream << i;
		flagNumber.append(stream.str());
		pFlags->SetAttribute(flagNumber, m_Flags[i]);
	}

	pGirl->SetAttribute("Tort", m_Tort);						// save their torture value

	// save their children
	pGirl->SetAttribute("PregCooldown", m_PregCooldown);
	TiXmlElement* pChildren = new TiXmlElement("Children");
	for (int i = 0; i < CHILD_COUNT_TYPES; i++)
	{
		if (m_ChildrenCount[i] < 0 || m_ChildrenCount[i] > 1000)	m_ChildrenCount[i] = 0;		// to correct girls without these
		pChildren->SetAttribute(children_type_names[i], m_ChildrenCount[i]);
	}

	pGirl->LinkEndChild(pChildren);
	sChild* child = m_Children.m_FirstChild;
	while (child)
	{
		child->SaveChildXML(pChildren);
		child = child->m_Next;
	}

	SaveActionsXML(pGirl, m_Enjoyment);							// save their enjoyment values
	SaveTrainingXML(pGirl, m_Training, m_TrainingMods, m_TrainingTemps);						// save their training values
	m_Triggers.SaveTriggersXML(pGirl);							// save their triggers

	return pGirl;

	/*unsigned int lim = current->m_Canonical_Daughters.size();
	ofs << lim << "\n";
	for(unsigned int i = 0; i < lim; i++) {
	ofs << current->m_Canonical_Daughters[i] << "\n";*/
}

bool sChild::LoadChildXML(TiXmlHandle hChild)
{
	TiXmlElement* pChild = hChild.ToElement();
	if (pChild == 0)
	{
		return false;
	}

	int tempInt = 0;
	pChild->QueryIntAttribute("Age", &tempInt); m_Age = tempInt; tempInt = 0;
	pChild->QueryValueAttribute<bool>("IsPlayers", &m_IsPlayers);
	pChild->QueryIntAttribute("Sex", &tempInt);	m_Sex = sChild::Gender(tempInt); tempInt = 0;
	pChild->QueryIntAttribute("Unborn", &tempInt); m_Unborn = tempInt; tempInt = 0;
	pChild->QueryIntAttribute("MultiBirth", &tempInt); m_MultiBirth = tempInt; tempInt = 0;	// `J` added
	if (m_MultiBirth < 1) m_MultiBirth = 1; if (m_MultiBirth > 5) m_MultiBirth = 5;			// `J` limited
	pChild->QueryIntAttribute("GirlsBorn", &tempInt); m_GirlsBorn = tempInt; tempInt = 0;	// `J` added
	if (m_GirlsBorn > m_MultiBirth) m_GirlsBorn = m_MultiBirth;								// `J` limited
	if (m_GirlsBorn < 0)	m_GirlsBorn = 0;												// `J` limited

	LoadStatsXML(hChild.FirstChild("Stats"), m_Stats);		// load their stats
	LoadSkillsXML(hChild.FirstChild("Skills"), m_Skills);	// load their skills
	return true;
}

TiXmlElement* sChild::SaveChildXML(TiXmlElement* pRoot)
{
	TiXmlElement* pChild = new TiXmlElement("Child");
	pRoot->LinkEndChild(pChild);
	pChild->SetAttribute("Age", m_Age);
	pChild->SetAttribute("IsPlayers", m_IsPlayers);
	pChild->SetAttribute("Sex", m_Sex);
	pChild->SetAttribute("Unborn", m_Unborn);
	pChild->SetAttribute("MultiBirth", m_MultiBirth);	// `J` added
	pChild->SetAttribute("GirlsBorn", m_GirlsBorn);		// `J` added
	SaveStatsXML(pChild, m_Stats);						// Save their stats
	SaveSkillsXML(pChild, m_Skills);					// save their skills
	return pChild;
}

// and an xml loader for sGirl
void sGirl::load_from_xml(TiXmlElement *el)
{
	int ival;
	const char *pt;
	// get the simple fields
	if (pt = el->Attribute("Name"))
	{
		m_Name = n_strdup(pt);
		m_Realname = pt;
		g_LogFile.os() << "Loading Girl : " << m_Realname << endl;
	}
	else
	{
		g_LogFile.os() << "Error: can't find name when loading girl." << "XML = " << (*el) << endl;
		return;
	}
	m_newRandomFixed = -1;

	if (pt = el->Attribute("Desc"))			m_Desc = n_strdup(pt);
	if (pt = el->Attribute("Gold", &ival))	m_Money = ival;
	if (pt = el->Attribute("Virgin"))		m_Virgin = (strcmp(pt, "Yes") == 0 || strcmp(pt, "1") == 0) ? 1 : 0;
	if (pt = el->Attribute("Catacombs"))	m_States |= (strcmp(pt, "Yes") == 0 || strcmp(pt, "1") == 0) ? (1 << STATUS_CATACOMBS) : (0 << STATUS_CATACOMBS);
	if (pt = el->Attribute("Slave"))		m_States |= (strcmp(pt, "Yes") == 0 || strcmp(pt, "1") == 0) ? (1 << STATUS_SLAVE) : (0 << STATUS_SLAVE);
	if (pt = el->Attribute("Arena"))		m_States |= (strcmp(pt, "Yes") == 0 || strcmp(pt, "1") == 0) ? (1 << STATUS_ARENA) : (0 << STATUS_ARENA);
	if (pt = el->Attribute("YourDaughter"))	m_States |= (strcmp(pt, "Yes") == 0 || strcmp(pt, "1") == 0) ? (1 << STATUS_YOURDAUGHTER) : (0 << STATUS_YOURDAUGHTER);
	if (pt = el->Attribute("IsDaughter"))	m_States |= (strcmp(pt, "Yes") == 0 || strcmp(pt, "1") == 0) ? (1 << STATUS_ISDAUGHTER) : (0 << STATUS_ISDAUGHTER);

	for (int i = 0; i < NUM_STATS; i++) // loop through stats
	{
		int ival;
		const char *stat_name = sGirl::stat_names[i];
		pt = el->Attribute(stat_name, &ival);

		ostream& os = g_LogFile.os();
		if (pt == 0)
		{
			os << "Error: Can't find stat '" << stat_name << "' for girl '" << m_Realname << "' - Setting it to default." << endl;
			continue;
		}
		m_Stats[i] = ival;
		if (cfg.debug.log_girls())	os << "Debug: Girl='" << m_Realname << "'; Stat='" << stat_name << "'; Value='" << pt << "'; Ival = " << int(m_Stats[i]) << "'" << endl;
	}

	for (u_int i = 0; i < NUM_SKILLS; i++)	//	loop through skills
	{
		int ival;
		if (pt = el->Attribute(sGirl::skill_names[i], &ival))	m_Skills[i] = ival;
	}

	if (pt = el->Attribute("Status"))
	{
		/* */if (strcmp(pt, "Catacombs") == 0)		m_States |= (1 << STATUS_CATACOMBS);
		else if (strcmp(pt, "Slave") == 0)			m_States |= (1 << STATUS_SLAVE);
		else if (strcmp(pt, "Arena") == 0)			m_States |= (1 << STATUS_ARENA);
		else if (strcmp(pt, "Your Daughter") == 0)	m_States |= (1 << STATUS_YOURDAUGHTER);
		else if (strcmp(pt, "Is Daughter") == 0)	m_States |= (1 << STATUS_ISDAUGHTER);
		//		else	m_States = 0;
	}

	TiXmlElement * child;
	for (child = el->FirstChildElement(); child; child = child->NextSiblingElement())
	{
		if (child->ValueStr() == "Canonical_Daughters")
		{
			string s = child->Attribute("Name");
			m_Canonical_Daughters.push_back(s);
		}
		if (child->ValueStr() == "Trait")	//get the trait name
		{
			pt = child->Attribute("Name");
			m_Traits[m_NumTraits] = g_Traits.GetTrait(pt);
			m_NumTraits++;
		}
		if (child->ValueStr() == "Item")	//get the item name
		{
			pt = child->Attribute("Name");
			sInventoryItem* item = g_InvManager.GetItem(n_strdup(pt));
			if (item)
			{
				m_Inventory[m_NumInventory] = item;
				if (item->m_Type != INVFOOD && item->m_Type != INVMAKEUP)
				{
					g_Girls.EquipItem(this, m_NumInventory, false);
				}
				m_NumInventory++;
			}
			else
			{
				g_LogFile.os() << "Error: Can't find Item: '" << pt << "' - skipping it." << endl;
				return;		// do as much as we can without crashing
			}

		}
	}

	if (pt = el->Attribute("Accomm", &ival)) m_AccLevel = ival;
	else m_AccLevel = (is_slave() ? cfg.initial.slave_accom() : cfg.initial.girls_accom());

}

void sRandomGirl::load_from_xml(TiXmlElement *el)
{
	const char *pt;
	m_NumTraits = 0; m_NumTraitNames = 0;
	m_NumItems = 0; m_NumItemNames = 0;
	// name and description are easy
	if (pt = el->Attribute("Name")) 		m_Name = pt;
	g_LogFile.os() << "Loading Rgirl : " << pt << endl;
	if (pt = el->Attribute("Desc"))			m_Desc = pt;

	// DQ - new random type ...
	m_newRandom = false;
	m_newRandomTable = 0;
	if (pt = el->Attribute("NewRandom"))		m_newRandom = (strcmp(pt, "Yes") == 0 || strcmp(pt, "1") == 0) ? 1 : 0;
	if (pt = el->Attribute("Human"))			m_Human = (strcmp(pt, "Yes") == 0 || strcmp(pt, "1") == 0) ? 1 : 0;
	if (pt = el->Attribute("Catacomb"))			m_Catacomb = (strcmp(pt, "Yes") == 0 || strcmp(pt, "1") == 0) ? 1 : 0;
	if (pt = el->Attribute("Arena"))			m_Arena = (strcmp(pt, "Yes") == 0 || strcmp(pt, "1") == 0) ? 1 : 0;
	if (pt = el->Attribute("Your Daughter"))	m_YourDaughter = (strcmp(pt, "Yes") == 0 || strcmp(pt, "1") == 0) ? 1 : 0;
	if (pt = el->Attribute("Is Daughter"))		m_IsDaughter = (strcmp(pt, "Yes") == 0 || strcmp(pt, "1") == 0) ? 1 : 0;

	// loop through children
	TiXmlElement *child;
	for (child = el->FirstChildElement(); child; child = child->NextSiblingElement())
	{
		/*
		*		now: what we do depends on the tag string
		*		which we can get from the ValueStr() method
		*
		*		Let's process each tag type in its own method.
		*		Keep things cleaner that way.
		*/
		if (child->ValueStr() == "Gold")
		{
			process_cash_xml(child);
			continue;
		}
		// OK: is it a stat?
		if (child->ValueStr() == "Stat")
		{
			process_stat_xml(child);
			continue;
		}
		// How about a skill?
		if (child->ValueStr() == "Skill")
		{
			process_skill_xml(child);
			continue;
		}
		// surely a trait then?
		if (child->ValueStr() == "Trait")
		{
			process_trait_xml(child);
			continue;
		}
		// surely a item then?
		if (child->ValueStr() == "Item")
		{
			process_item_xml(child);
			continue;
		}
		// None of the above? Better ask for help then.
		g_LogFile.os() << "Unexpected tag: " << child->ValueStr() << endl;
		g_LogFile.os() << "	don't know what do to, ignoring" << endl;
	}
}

void cGirls::LoadRandomGirl(string filename)
{
	/*
	*	before we go any further: files that end in "x" are
	*	in XML format. Get the last char of the filename.
	*/
	char c = filename.at(filename.length() - 1);
	/*
	*	now decide how we want to really load the file
	*/
	if (c == 'x')
	{
		cerr << "loading " << filename << " as XML" << endl;
		LoadRandomGirlXML(filename);
	}
	else
	{
		cerr << ".06 no longer supports Legacy Girls. Use the Whore Master Editor to update '" << filename << "'" << endl;
	}
}

void cGirls::LoadRandomGirlXML(string filename)
{
	TiXmlDocument doc(filename);
	if (!doc.LoadFile())
	{
		g_LogFile.os() << "can't load random XML girls " << filename << endl;
		g_LogFile.os() << "Error: line " << doc.ErrorRow() << ", col " << doc.ErrorCol() << ": " << doc.ErrorDesc() << endl;
		return;
	}
	g_LogFile.os() << endl << "Loading File ::: " << filename << endl;
	TiXmlElement *el, *root_el = doc.RootElement();	// get the docuement root

	for (el = root_el->FirstChildElement(); el; el = el->NextSiblingElement())
	{	// loop over the elements attached to the root
		sRandomGirl* girl = new sRandomGirl;		// walk the XML DOM to get the girl data
		girl->load_from_xml(el);					// uses sRandomGirl::load_from_xml
		AddRandomGirl(girl);						// add the girl to the list
	}
}

void cGirls::LoadGirlsDecider(string filename)
{
	/*
	*	before we go any further: files that end in "x" are
	*	in XML format. Get the last char of the filename.
	*/
	char c = filename.at(filename.length() - 1);
	/*
	*	now decide how we want to really load the file
	*/
	if (c == 'x')
	{
		cerr << "loading " << filename << " as XML" << endl;
		LoadGirlsXML(filename);
	}
	else
	{
		cerr << ".06 no longer supports Legacy Girls. Use the Whore Master Editor to update '" << filename << "'" << endl;
	}
}

void cGirls::LoadGirlsXML(string filename)
{
	TiXmlDocument doc(filename);
	if (!doc.LoadFile())
	{
		g_LogFile.ss() << "can't load XML girls " << filename << endl;
		g_LogFile.ss() << "Error: line " << doc.ErrorRow() << ", col " << doc.ErrorCol() << ": " << doc.ErrorDesc() << endl;
		g_LogFile.ssend();
		return;
	}
	g_LogFile.os() << endl << "Loading File ::: " << filename << endl;
	// get the docuement root
	TiXmlElement *el, *root_el = doc.RootElement();
	// loop over the elements attached to the root
	for (el = root_el->FirstChildElement(); el; el = el->NextSiblingElement())
	{

		sGirl* girl = new sGirl;			// walk the XML DOM to get the girl data
		girl->load_from_xml(el);			// uses sGirl::load_from_xml
		if (cfg.debug.log_girls() && cfg.debug.log_extradetails()) g_LogFile.os() << *girl << endl;

		if (girl->check_virginity())			// `J` check girl's virginity
		{
			girl->m_Virgin = 1; girl->add_trait("Virgin");
		}
		else
		{
			girl->m_Virgin = 0;
			girl->remove_trait("Virgin");
		}
		if (girl->m_Stats[STAT_AGE] < 18) girl->m_Stats[STAT_AGE] = 18;	// `J` Legal Note: 18 is the Legal Age of Majority for the USA where I live
		MutuallyExclusiveTraits(girl, 1);	// make sure all the trait effects are applied
		RemoveAllRememberedTraits(girl);	// WD: For new girls remove any remembered traits from trait incompatibilities
		ApplyTraits(girl);

		// load triggers if the girl has any
		DirPath dp = DirPath(cfg.folders.characters().c_str()) << girl->m_Name << "triggers.xml";
		girl->m_Triggers.LoadList(dp);
		girl->m_Triggers.SetGirlTarget(girl);
		AddGirl(girl);						// add the girl to the list
		CalculateGirlType(girl);			// Fetish list for customer happiniess
	}
}

bool cGirls::LoadGirlsXML(TiXmlHandle hGirls)
{
	TiXmlElement* pGirls = hGirls.ToElement();
	if (pGirls == 0) return false;
	sGirl* current = 0;					// load the number of girls
	for (TiXmlElement* pGirl = pGirls->FirstChildElement("Girl"); pGirl != 0; pGirl = pGirl->NextSiblingElement("Girl"))
	{
		current = new sGirl();			// load each girl and add her
		bool success = current->LoadGirlXML(TiXmlHandle(pGirl));
		if (cfg.debug.log_girls())
		{
			g_LogFile.ss() << "Loading girl: " << current->m_Realname << " | (" << current->m_Name << ") | " << (success ? "Done" : "Failed");
			g_LogFile.ssend();
		}
		if (success == true) AddGirl(current);
		else { delete current; continue; }
	}
	return true;
}

TiXmlElement* cGirls::SaveGirlsXML(TiXmlElement* pRoot)
{
	TiXmlElement* pGirls = new TiXmlElement("Girls");
	pRoot->LinkEndChild(pGirls);
	sGirl* current = m_Parent;
	int numgirls=0;
	while (current)					// save the number of girls
	{
		current->SaveGirlXML(pGirls);
		current = current->m_Next;
		numgirls++;
	}
	pGirls->SetAttribute("NumberofGirls", numgirls);
	return pGirls;
}

// ----- Tag processing

void sRandomGirl::process_trait_xml(TiXmlElement *el)
{
	int ival; const char *pt;
	if (!(pt = el->Attribute("Name"))) // `J` if there is no name why continue?
	    return;

    TraitSpec* trait = new TraitSpec(pt, "", "", -1, -1);       // we need to allocate a new sTrait scruct,
    stringstream ss;
    ss << trait->name();
    m_TraitNames[m_NumTraitNames] = ss.str();


	if (m_NumTraitNames<MAXNUM_TRAITS) m_Traits[m_NumTraits] = trait;					// store that in the next free index slot
	if ((pt = el->Attribute("Percent", &ival)))							// get the percentage chance
	{
		if (m_NumTraitNames<MAXNUM_TRAITS)	m_TraitChance[m_NumTraits] = ival;
		m_TraitChanceB[m_NumTraitNames] = ival;
	}
	if (m_NumTraitNames<MAXNUM_TRAITS) m_NumTraits++;
	m_NumTraitNames++;											// and whack up the trait count.
}

void sRandomGirl::process_item_xml(TiXmlElement *el)
{
	int ival; const char *pt;
	sInventoryItem *item = 0;
	if ((pt = el->Attribute("Name")))
	{
		string finditem = n_strdup(pt);
		item = g_InvManager.GetItem(finditem);
		if (!item)
		{
			g_LogFile.os() << "Error: Can't find Item: '" << finditem << "' - skipping it." << endl;
			return;		// do as much as we can without crashing
		}
		m_ItemNames[m_NumItemNames] = item->m_Name;
	}
	if (m_NumItemNames<MAXNUM_INVENTORY) m_Inventory[m_NumItems] = item;
	if ((pt = el->Attribute("Percent", &ival)))
	{
		if (m_NumItemNames<MAXNUM_INVENTORY)	m_ItemChance[m_NumItems] = ival;
		m_ItemChanceB[m_NumItemNames] = ival;
	}
	if (m_NumItemNames<MAXNUM_INVENTORY) m_NumItems++;
	m_NumItemNames++;
}

void sRandomGirl::process_stat_xml(TiXmlElement *el)
{
	int ival, index; const char *pt;
	if ((pt = el->Attribute("Name"))) index = lookup->stat_lookup[pt];
	else
	{
		g_LogFile.os() << "can't find 'Name' attribute - can't process stat" << endl;
		return;		// do as much as we can without crashing
	}
	if ((pt = el->Attribute("Min", &ival))) m_MinStats[index] = ival;
	if ((pt = el->Attribute("Max", &ival))) m_MaxStats[index] = ival;
}

void sRandomGirl::process_skill_xml(TiXmlElement *el)
{
	int ival, index;
	const char *pt;
	/*
	*	Strictly, I should use something that lets me
	*	test for absence. This won't catch typos in the
	*	XML file
	*/
	if ((pt = el->Attribute("Name"))) index = lookup->skill_lookup[pt];
	else
	{
		g_LogFile.os() << "can't find 'Name' attribute - can't process skill" << endl;
		return;		// do as much as we can without crashing
	}
	if ((pt = el->Attribute("Min", &ival))) m_MinSkills[index] = ival;
	if ((pt = el->Attribute("Max", &ival))) m_MaxSkills[index] = ival;
}

void sRandomGirl::process_cash_xml(TiXmlElement *el)
{
	int ival; const char *pt;
	if ((pt = el->Attribute("Min", &ival)))
	{
		if (cfg.debug.log_girls()) g_LogFile.os() << " min money = " << ival << endl;
		m_MinMoney = ival;
	}
	if ((pt = el->Attribute("Max", &ival)))
	{
		if (cfg.debug.log_girls()) g_LogFile.os() << " max money = " << ival << endl;
		m_MaxMoney = ival;
	}
}

// ----- Equipment & inventory

string stringtolower(string name)
{
	string s = name;
	for (u_int i = 0; i < name.length(); i++)
	{
		s[i] = tolower(name[i]);
	}
	return s;
}

/*	`J` Updated from Akia's suggestion and expanded return values
	returns 0 if she has no weapons or armor equipped
	returns 6 if she has one of each: weapon, armor, helmet, boots, shield
	returns 5 if she has weapons and armor (others ignored)
	returns 4 if she has weapons and no armor but at least one of helmet, boots or shield
	returns 3 if she has no weapons or armor but at least one of helmet, boots or shield
	returns 2 if she has only armor but no weapon (others ignored)
	returns 1 if she has only weapons
	returns -1 for errors
	*	Small weapons are not counted
	*/
int cGirls::CheckEquipment(sGirl* girl)
{
	if (girl->m_NumInventory <= 0) return 0;	// she has no items
	int foundw = 0;	int founda = 0;	int foundh = 0;	int foundc = 0;	int founds = 0;
	for (int i = 0; i < MAXNUM_GIRL_INVENTORY; i++)
	{
		if (girl->m_Inventory[i] != 0 && girl->m_EquipedItems[i] == 1)
		{
			if (girl->m_Inventory[i]->m_Type == INVWEAPON)			foundw = 1;
			if (girl->m_Inventory[i]->m_Type == INVARMOR)			founda = 1;
			if (girl->m_Inventory[i]->m_Type == INVHELMET)			foundh = 1;
			if (girl->m_Inventory[i]->m_Type == INVCOMBATSHOES)		foundc = 1;
			if (girl->m_Inventory[i]->m_Type == INVSHIELD)			founds = 1;
		}
	}

	if (foundw + founda + foundh + foundc + founds <= 0)			return 0;	// has nothing
	if (foundw + founda + foundh + foundc + founds >= 5)			return 6;	// has everything
	if (foundw == 1 && founda == 1)									return 5;	// has weapon and armor (others ignored)
	if (foundw == 1 && founda == 0 && foundh + foundc + founds > 0)	return 4;	// has weapon, no armor but at least one of helmet, boots or shield
	if (foundw == 0 && founda == 0 && foundh + foundc + founds > 0)	return 3;	// no weapon or armor but at least one of helmet, boots or shield
	if (foundw == 0 && founda == 1)									return 2;	// has armor but no weapons (others ignored)
	if (foundw == 1 && founda + foundh + foundc + founds <= 0)		return 1;	// has weapon only
	return -1;
}

void cGirls::EquipCombat(sGirl* girl)
{
	// girl makes sure best armor and weapons are equipped, ready for combat
	if (!cfg.initial.auto_combat_equip()) return;	// is this feature disabled in config?
	int refusal = 0;
	if (girl->has_trait("Retarded")) refusal += 30;	// if she's retarded, she might refuse or forget
	if (g_Dice.percent(refusal)) return;

	int found = 0;
	int Armor = -1, Weap1 = -1, Weap2 = -1, Helm = -1, Shield = -1, Boot = -1;
	for (int i = 0; i < MAXNUM_GIRL_INVENTORY && found<girl->m_NumInventory; i++)
	{
		if (girl->m_Inventory[i] != 0)
		{
			found++;
			if (girl->m_Inventory[i]->m_Type == INVWEAPON)
			{
				g_InvManager.Unequip(girl, i);
				if (Weap1 == -1) Weap1 = i;
				else if (Weap2 == -1) Weap2 = i;
				else if (girl->m_Inventory[i]->m_Cost > girl->m_Inventory[Weap1]->m_Cost)
				{
					Weap2 = Weap1;
					Weap1 = i;
				}
				else if (girl->m_Inventory[i]->m_Cost > girl->m_Inventory[Weap2]->m_Cost)
					Weap2 = i;
			}
			if (girl->m_Inventory[i]->m_Type == INVARMOR)
			{
				g_InvManager.Unequip(girl, i);
				if (Armor == -1) Armor = i;
				else if (girl->m_Inventory[i]->m_Cost > girl->m_Inventory[Armor]->m_Cost) Armor = i;
			}
			if (girl->m_Inventory[i]->m_Type == INVHELMET)
			{
				g_InvManager.Unequip(girl, i);
				if (Helm == -1) Helm = i;
				else if (girl->m_Inventory[i]->m_Cost > girl->m_Inventory[Helm]->m_Cost) Helm = i;
			}
			if (girl->m_Inventory[i]->m_Type == INVCOMBATSHOES)
			{
				g_InvManager.Unequip(girl, i);
				if (Boot == -1) Boot = i;
				else if (girl->m_Inventory[i]->m_Cost > girl->m_Inventory[Boot]->m_Cost) Boot = i;
			}
			if (girl->m_Inventory[i]->m_Type == INVSHIELD)
			{
				g_InvManager.Unequip(girl, i);
				if (Shield == -1) Shield = i;
				else if (girl->m_Inventory[i]->m_Cost > girl->m_Inventory[Shield]->m_Cost) Shield = i;
			}
		}
	}

	// unequip hats and shoes if boots and helms were found
	if (Helm > -1 || Boot > -1)
	{
		found = 0;
		for (int i = 0; i < MAXNUM_GIRL_INVENTORY && found<girl->m_NumInventory; i++)
		{
			if (girl->m_Inventory[i] != 0)
			{
				found++;
				sInventoryItem* curItem = girl->m_Inventory[i];
				if (Helm > -1 && curItem->m_Type == INVHAT)		g_InvManager.Unequip(girl, i);
				if (Boot > -1 && curItem->m_Type == INVSHOES)	g_InvManager.Unequip(girl, i);
			}
		}
	}

	if (Armor > -1)		g_InvManager.Equip(girl, Armor, false);
	if (Weap1 > -1)		g_InvManager.Equip(girl, Weap1, false);
	if (Weap2 > -1)		g_InvManager.Equip(girl, Weap2, false);
	if (Helm > -1)		g_InvManager.Equip(girl, Helm, false);
	if (Boot > -1)		g_InvManager.Equip(girl, Boot, false);
	if (Shield > -1)	g_InvManager.Equip(girl, Shield, false);
}

void cGirls::UnequipCombat(sGirl* girl)
{  // girl unequips armor and weapons, ready for brothel work or other non-aggressive jobs
	if (!cfg.initial.auto_combat_equip()) return; // is this feature disabled in config?
	// if she's a really rough or crazy bitch, she might just keep combat gear equipped
	int refusal = 0;
	if (girl->has_trait("Aggressive"))	refusal += 30;
	if (girl->has_trait("Yandere"))		refusal += 30;
	if (girl->has_trait("Twisted"))		refusal += 30;
	if (girl->has_trait("Retarded"))	refusal += 30;
	if (g_Dice.percent(refusal))			return;

	int found = 0;
	for (int i = 0; i < MAXNUM_GIRL_INVENTORY && found<girl->m_NumInventory; i++)
	{
		if (girl->m_Inventory[i] != 0)
		{
			found++;
			sInventoryItem* curItem = girl->m_Inventory[i];
			if (curItem->m_Type == INVWEAPON
				|| curItem->m_Type == INVARMOR
				|| curItem->m_Type == INVHELMET
				|| curItem->m_Type == INVCOMBATSHOES
				|| curItem->m_Type == INVSHIELD)
				g_InvManager.Unequip(girl, i);
		}
	}
	found = 0;
	// reequip shoes and hats
	for (int i = 0; i < MAXNUM_GIRL_INVENTORY && found<girl->m_NumInventory; i++)
	{
		if (girl->m_Inventory[i] != 0)
		{
			found++;
			sInventoryItem* curItem = girl->m_Inventory[i];
			if (curItem->m_Type == INVSHOES || curItem->m_Type == INVHAT)
				g_InvManager.Equip(girl, i, false);
		}
	}


}

void cGirls::UseItems(sGirl* girl)
{
	bool withdraw = false;
	// uses drugs first
	if (girl->has_trait("Viras Blood Addict"))
	{
		int temp = girl->has_item("Vira Blood");
		if (temp == -1)	// withdrawals for a week
		{
			if (girl->m_Withdrawals >= 30)
			{
				girl->remove_trait("Viras Blood Addict", true);
				girl->add_trait("Former Addict");
				stringstream goodnews;
				goodnews << "Good News, " << girl->m_Realname << " has overcome her addiction to Viras Blood.";
				girl->m_Events.AddMessage(goodnews.str(), IMGTYPE_PROFILE, EVENT_GOODNEWS);
			}
			else
			{
				girl->upd_stat(STAT_HAPPINESS, -30);
				girl->upd_stat(STAT_OBEDIENCE, -30);
				girl->upd_stat(STAT_HEALTH, -4);
				if (!withdraw)
				{
					girl->m_Withdrawals++;
					withdraw = true;
				}
			}
		}
		else
		{
			girl->upd_stat(STAT_HAPPINESS, 10);
			girl->upd_temp_stat(STAT_LIBIDO, 10, true);
			g_InvManager.Equip(girl, temp, false);
			girl->m_Withdrawals = 0;
		}
	}
	if (girl->has_trait("Fairy Dust Addict"))
	{
		int temp = girl->has_item("Fairy Dust");
		if (temp == -1)	// withdrawals for a week
		{
			if (girl->m_Withdrawals >= 20)
			{
				girl->remove_trait("Fairy Dust Addict", true);
				girl->add_trait("Former Addict");
				stringstream goodnews;
				goodnews << "Good News, " << girl->m_Realname << " has overcome her addiction to Fairy Dust.";
				girl->m_Events.AddMessage(goodnews.str(), IMGTYPE_PROFILE, EVENT_GOODNEWS);
			}
			else
			{
				girl->upd_stat(STAT_HAPPINESS, -30);
				girl->upd_stat(STAT_OBEDIENCE, -30);
				girl->upd_stat(STAT_HEALTH, -4);
				if (!withdraw)
				{
					girl->m_Withdrawals++;
					withdraw = true;
				}
			}
		}
		else
		{
			girl->upd_stat(STAT_HAPPINESS, 10);
			girl->upd_temp_stat(STAT_LIBIDO, 5, true);
			g_InvManager.Equip(girl, temp, false);
			girl->m_Withdrawals = 0;
		}
	}
	if (girl->has_trait("Shroud Addict"))
	{
		int temp = girl->has_item("Shroud Mushroom");
		if (temp == -1)	// withdrawals for a week
		{
			if (girl->m_Withdrawals >= 20)
			{
				girl->remove_trait("Shroud Addict", true);
				girl->add_trait("Former Addict");
				stringstream goodnews;
				goodnews << "Good News, " << girl->m_Realname << " has overcome her addiction to Shroud Mushrooms.";
				girl->m_Events.AddMessage(goodnews.str(), IMGTYPE_PROFILE, EVENT_GOODNEWS);
			}
			else
			{
				girl->upd_stat(STAT_HAPPINESS, -30);
				girl->upd_stat(STAT_OBEDIENCE, -30);
				girl->upd_stat(STAT_HEALTH, -4);
				if (!withdraw)
				{
					girl->m_Withdrawals++;
					withdraw = true;
				}
			}
		}
		else
		{
			girl->upd_stat(STAT_HAPPINESS, 10);
			girl->upd_temp_stat(STAT_LIBIDO, 2, true);
			g_InvManager.Equip(girl, temp, false);
			girl->m_Withdrawals = 0;
		}
	}
	if (girl->has_trait("Alcoholic"))
	{
		int temp = girl->has_item("Alcohol");
		if (temp == -1)	// withdrawals for a week
		{
			if (girl->m_Withdrawals >= 15)
			{
				girl->remove_trait("Alcoholic", true);
				girl->add_trait("Former Addict");
				stringstream goodnews;
				goodnews << "Good News, " << girl->m_Realname << " has overcome her addiction to Alcohol.";
				girl->m_Events.AddMessage(goodnews.str(), IMGTYPE_PROFILE, EVENT_GOODNEWS);
			}
			else
			{
				girl->upd_stat(STAT_HAPPINESS, -10);
				girl->upd_stat(STAT_OBEDIENCE, -10);

				// `J` alchohol withdrawl should not really harm her health, changed it to (-2, -1, 0 or +1) instead
				girl->upd_stat(STAT_HEALTH, g_Dice % 4 - 2);
				if (!withdraw)
				{
					girl->m_Withdrawals++;
					withdraw = true;
				}
			}
		}
		else
		{
			girl->upd_stat(STAT_HAPPINESS, 10);
			girl->upd_temp_stat(STAT_LIBIDO, 2, true);
			g_InvManager.Equip(girl, temp, false);
			girl->m_Withdrawals = 0;
		}
	}
	if (girl->has_trait("Smoker")) // `Gondra` added this since this seemed to be missing IMPORTANT: requires the item
	{
		if (girl->has_item_j("Stop Smoking Now Patch") > -1)
		{
			g_InvManager.Equip(girl, girl->has_item_j("Stop Smoking Now Patch"), false);
			girl->m_Withdrawals = 0;
		}
		else if (girl->has_item_j("Stop Smoking Patch") > -1)
		{
			g_InvManager.Equip(girl, girl->has_item_j("Stop Smoking Patch"), false);
			girl->m_Withdrawals = 0;
		}
		else if (girl->has_item_j("Cigarette") > -1 ||
			girl->has_item_j("Small pack of Cigarettes") > -1 ||
			girl->has_item_j("Pack of Cigarettes") > -1 ||
			girl->has_item_j("Carton of Cigarettes") > -1 ||
			girl->has_item_j("Magic Pack of Cigarettes") > -1 ||
			girl->has_item_j("Magic Carton of Cigarettes") > -1)
		{
			int temp = -1; int happy = 0; int health = 0; int libido = 0; int mana = 0;
			// `J` go through the list of available items and if she has more than one of them use only the "best"
			if (girl->has_item_j("Cigarette") > -1)
			{
				temp = girl->has_item_j("Cigarette");
				happy += g_Dice % 2; health = 0; libido += g_Dice % 2;
			}
			if (girl->has_item_j("Small pack of Cigarettes") > -1)
			{
				temp = girl->has_item_j("Small pack of Cigarettes");
				happy += g_Dice % 4; health -= g_Dice % 2; libido += g_Dice % 3;
			}
			if (girl->has_item_j("Pack of Cigarettes") > -1)
			{
				temp = girl->has_item_j("Pack of Cigarettes");
				happy += g_Dice % 5 + 1; health -= g_Dice % 3; libido += g_Dice % 4;
			}
			if (girl->has_item_j("Carton of Cigarettes") > -1)
			{
				temp = girl->has_item_j("Carton of Cigarettes");
				happy += g_Dice % 6 + 3; health -= g_Dice % 3 + 1; libido += g_Dice % 5 + 1;
			}
			if (girl->has_item_j("Magic Pack of Cigarettes") > -1)
			{
				temp = girl->has_item_j("Magic Pack of Cigarettes");
				happy += g_Dice % 6 + 4; health -= g_Dice % 4; libido += g_Dice % 4 + 2; mana -= 1;
			}
			if (girl->has_item_j("Magic Carton of Cigarettes") > -1)
			{
				temp = girl->has_item_j("Magic Carton of Cigarettes");
				happy += g_Dice % 11 + 5; health -= g_Dice % 6 + 1; libido += g_Dice % 8 + 4; mana -= 2;
			}
			if (temp > -1)
			{
				girl->upd_stat(STAT_HAPPINESS, happy);
				girl->upd_stat(STAT_HEALTH, health);
				girl->upd_stat(STAT_MANA, mana);
				girl->upd_temp_stat(STAT_LIBIDO, libido, true);
				g_InvManager.Equip(girl, temp, false);
				girl->m_Withdrawals = 0;
			}
			if (girl->is_dead())
			{
				stringstream cancer;
				cancer << girl->m_Realname << " has died of cancer from smoking.";
				girl->m_Events.AddMessage(cancer.str(), IMGTYPE_PROFILE, EVENT_WARNING);
				return;
			}
		}
		else if (girl->m_Withdrawals >= 15)
		{
			girl->m_Withdrawals = 0;
			girl->remove_trait("Smoker", true);
			girl->add_trait("Former Addict");
			stringstream goodnews;
			goodnews << "Good News, " << girl->m_Realname << " has overcome her addiction to Nicotine.";
			girl->m_Events.AddMessage(goodnews.str(), IMGTYPE_PROFILE, EVENT_GOODNEWS);
		}
		else
		{
			girl->upd_stat(STAT_HAPPINESS, -10);
			girl->upd_stat(STAT_OBEDIENCE, -5);

			// `Gondra` not sure if Nicotine withdrawal should harm her health, left it at (-2, -1, 0 or +1) like alcohol
			girl->upd_stat(STAT_HEALTH, g_Dice % 4 - 2);
			// `Gondra` nicotine withdrawal includes as symptoms difficulty to concentrate and fatigue
			girl->upd_temp_stat(STAT_INTELLIGENCE, -2);
			girl->upd_stat(STAT_TIREDNESS, 5);
			if (!withdraw)
			{
				girl->m_Withdrawals++;
				withdraw = true;
			}
		}
	}

	// sell crapy items
	for (int i = 0; i < girl->m_NumInventory; i++)	// use a food item if it is in stock, and remove any bad things if disobedient
	{
		if (girl->m_Inventory[i] != 0)
		{
			int max = 0;
			switch ((int)girl->m_Inventory[i]->m_Type)
			{
			case INVRING:				if (max == 0)	max = 8;
			case INVDRESS:				if (max == 0)	max = 1;
			case INVUNDERWEAR:			if (max == 0)	max = 1;
			case INVSHOES:				if (max == 0)	max = 1;
			case INVNECKLACE:			if (max == 0)	max = 1;
			case INVWEAPON:				if (max == 0)	max = 2;
			case INVSMWEAPON:			if (max == 0)	max = 2;
			case INVARMOR:				if (max == 0)	max = 1;
			case INVARMBAND:			if (max == 0)	max = 2;
			case INVHAT:				if (max == 0)	max = 1;
			case INVHELMET:				if (max == 0)	max = 1;
			case INVGLASSES:			if (max == 0)	max = 1;
			case INVCOMBATSHOES:		if (max == 0)	max = 1;
			case INVSHIELD:				if (max == 0)	max = 1;
				if (g_Girls.GetNumItemType(girl, girl->m_Inventory[i]->m_Type) > max) // MYR: Bug fix, was >=
				{
					int nicerThan = g_Girls.GetWorseItem(girl, girl->m_Inventory[i]->m_Type, girl->m_Inventory[i]->m_Cost);	// find a worse item of the same type
					if (nicerThan != -1)
						// `J` zzzzzzzz Add an option to have the girls put the item into "Store Room" instead of selling it
						g_Girls.SellInvItem(girl, nicerThan);
				}
				break;
			default:
				break;
			}
		}
	}

	int usedFood = (g_Dice % 3) + 1;
	int usedFoodCount = 0;
	for (int i = 0; i < girl->m_NumInventory; i++)	// use a food item if it is in stock, and remove any bad things if disobedient
	{
		sInventoryItem* curItem = girl->m_Inventory[i];
		if (curItem != 0)
		{
			if ((curItem->m_Type == INVFOOD || curItem->m_Type == INVMAKEUP) && usedFoodCount < usedFood)
			{
				if (!g_Dice.percent(curItem->m_GirlBuyChance)) continue;   // make sure she'd want it herself

				bool useful = false;					// make sure there's some reason for her to use it
				int checktouseit = curItem->m_Effects.size();
				for (u_int j = 0; j < curItem->m_Effects.size(); j++)
				{
					sEffect* curEffect = &curItem->m_Effects[j];
					if (curEffect->m_Affects == sEffect::Nothing)
					{  // really? it does nothing? sure, just for the hell of it
						checktouseit--;
					}
					else if (curEffect->m_Affects == sEffect::GirlStatus)
					{
						switch (curEffect->m_EffectID)
						{
							// these should not be used for items so skip them
						case STATUS_NONE:
						case STATUS_CATACOMBS:
						case STATUS_ARENA:
						case STATUS_ISDAUGHTER:
							break;
						case STATUS_HAS_DAUGHTER:
						case STATUS_HAS_SON:
							if (curEffect->m_Amount == 1)	// adopt a child item?
							{
								if (g_Dice.percent(25)) checktouseit--;		// 25% chance she wants adopt
							}
							break;
							// these statuses need to be tested individually
						case STATUS_YOURDAUGHTER:
							if (curEffect->m_Amount == 0 && girl->m_States&(1 << STATUS_YOURDAUGHTER))
							{
								if (girl->pchate()>90 && girl->pclove()<10)	// she hates you and does not want to be your daughter
									checktouseit--;
							}
							break;
						case STATUS_PREGNANT:
							if (curEffect->m_Amount == 0 && girl->m_States&(1 << STATUS_PREGNANT))
							{
								if (g_Dice.percent(5)) checktouseit--;		// 5% chance she wants to get rid of the baby
							}
							if (curEffect->m_Amount == 1 && !girl->is_pregnant())
							{
								if (g_Dice.percent(50)) checktouseit--;		// you gave it to her so she will consider using it
							}
							break;
						case STATUS_PREGNANT_BY_PLAYER:
							if (curEffect->m_Amount == 1 && !girl->is_pregnant())
							{
								if (girl->pchate()<10 && girl->pclove()>80)	// she love you and wants to have your child
									checktouseit--;
							}
							if (curEffect->m_Amount == 0 && girl->m_States&(1 << STATUS_PREGNANT_BY_PLAYER))
							{
								if (girl->pchate()>90 && girl->pclove()<10)	// she hates you and doesn't want to have your child
									checktouseit--;
							}
							break;
						case STATUS_INSEMINATED:
							if (curEffect->m_Amount == 0 && girl->m_States&(1 << curEffect->m_EffectID))
							{
								if (g_Dice.percent(50)) checktouseit--;	// she might not want give birth to a beast
							}
							if (curEffect->m_Amount == 1 && !girl->is_pregnant())
							{
								if (g_Dice.percent(50)) checktouseit--;	// she might want give birth to a beast
							}
							break;
							// if she has these statuses and the item removes it she will use it
						case STATUS_POISONED:
						case STATUS_BADLY_POISONED:
						case STATUS_SLAVE:
						case STATUS_CONTROLLED:
							if (curEffect->m_Amount == 0 && girl->m_States&(1 << curEffect->m_EffectID))
							{
								checktouseit--;
							}
							break;
						}

					}
					else if (curEffect->m_Affects == sEffect::Trait)
					{
						if ((curEffect->m_Amount >= 1) != girl->has_trait(curEffect->m_Trait))
						{  // girl has trait and item removes it, or doesn't have trait and item adds it
							checktouseit--;
						}
					}
					else if (curEffect->m_Affects == sEffect::Stat)
					{
						unsigned int Stat = curEffect->m_EffectID;

						if ((curEffect->m_Amount > 0) &&
							(
							Stat == STAT_CHARISMA
							|| Stat == STAT_HAPPINESS
							|| Stat == STAT_FAME
							|| Stat == STAT_LEVEL
							|| Stat == STAT_ASKPRICE
							|| Stat == STAT_EXP
							|| Stat == STAT_BEAUTY
							)
							)
						{  // even if this stat can't be increased further, she still wants it (call it vanity, greed, whatever)
							checktouseit--;
						}
						if ((curEffect->m_Amount > 0) && (girl->m_Stats[Stat] < 100) &&
							(
							Stat == STAT_LIBIDO
							|| Stat == STAT_CONSTITUTION
							|| Stat == STAT_INTELLIGENCE
							|| Stat == STAT_CONFIDENCE
							|| Stat == STAT_MANA
							|| Stat == STAT_AGILITY
							|| Stat == STAT_SPIRIT
							|| Stat == STAT_HEALTH
							)
							)
						{  // this stat increase would be good
							checktouseit--;
						}
						if ((curEffect->m_Amount < 0) && (girl->m_Stats[Stat] > 0) &&
							(
							Stat == STAT_AGE
							|| Stat == STAT_TIREDNESS
							)
							)
						{  // decreasing this stat would actually be good
							checktouseit--;
						}
					}
					else if (curEffect->m_Affects == sEffect::Skill)
					{
						if ((curEffect->m_Amount > 0) && (girl->m_Stats[curEffect->m_EffectID] < 100))
						{  // skill would actually increase (wouldn't want to lose any skills)
							checktouseit--;
						}
					}
					else if (curEffect->m_Affects == sEffect::Enjoy)
					{
						if ((curEffect->m_Amount > 0) && (girl->get_enjoyment(curEffect->m_EffectID) < 100))
						{  // enjoyment would actually increase (wouldn't want to lose any enjoyment)
							checktouseit--;
						}
					}
				}

				if (checktouseit < (int)curItem->m_Effects.size() / 2) // if more than half of the effects are useful, use it
				{  // hey, this consumable item might actually be useful... gobble gobble gobble
					g_InvManager.Equip(girl, i, false);
					usedFoodCount++;
				}
			}

			// MYR: Girls shouldn't be able (IMHO) to take off things like control bracelets
			//else if(curItem->m_Badness > 20 && DisobeyCheck(girl, ACTION_GENERAL) && girl->m_EquipedItems[i] == 1)
			//{
			//	g_InvManager.Unequip(girl, i);
			//}
		}
	}

	// add the selling of items that are no longer needed here
}

bool cGirls::CanEquip(sGirl* girl, int num, bool force)
{
	if (force) return true;
	switch (girl->m_Inventory[num]->m_Type)
	{
	case INVRING:			// worn on fingers (max 8)
		if (GetNumItemEquiped(girl, girl->m_Inventory[num]->m_Type) >= 8) return false;
		break;

	case INVWEAPON:			// equiped on body, (max 2)
	case INVSMWEAPON:		// hidden on body, (max 2)
	case INVARMBAND:		// (max 2), worn around arms
		if (GetNumItemEquiped(girl, girl->m_Inventory[num]->m_Type) >= 2) return false;
		break;

	case INVDRESS:			// Worn on body, (max 1)
	case INVUNDERWEAR:		// Worn on body, (max 1)
	case INVSHOES:			// worn on feet, (max 1)
	case INVNECKLACE:		// worn on neck, (max 1)
	case INVARMOR:			// worn on body over dresses (max 1)
	case INVHAT:			//
	case INVHELMET:			//
	case INVGLASSES:		//
	case INVSWIMSUIT:		//
	case INVCOMBATSHOES:	//
	case INVSHIELD:			//
		if (GetNumItemEquiped(girl, girl->m_Inventory[num]->m_Type) >= 1) return false;
		break;

	case INVFOOD:	// Eaten, single use
	case INVMAKEUP:	// worn on face, single use
	case INVMISC:	// these items don't usually do anything just random stuff girls might buy. The ones that do, cause a constant effect without having to be equiped
	default:
		return true;
		break;
	}
	return true;
}

bool cGirls::IsItemEquipable(sGirl* girl, int num)
{
	switch (girl->m_Inventory[num]->m_Type)
	{
	case sInventoryItem::Ring:
	case sInventoryItem::Dress:
	case sInventoryItem::Underwear:
	case sInventoryItem::Shoes:
	case sInventoryItem::Necklace:
	case sInventoryItem::Weapon:
	case sInventoryItem::Armor:
	case sInventoryItem::Armband:
	case sInventoryItem::SmWeapon:
	case sInventoryItem::Hat:
	case sInventoryItem::Helmet:
	case sInventoryItem::Glasses:
	case sInventoryItem::Swimsuit:
	case sInventoryItem::Combatshoes:
	case sInventoryItem::Shield:
		return true;
	case sInventoryItem::Food:
	case sInventoryItem::Makeup:
	case sInventoryItem::Misc:
		return false;
	}
	return false;
}

bool cGirls::EquipItem(sGirl* girl, int num, bool force)
{
	if (CanEquip(girl, num, force))
	{
		g_InvManager.Equip(girl, num, force);
		return true;
	}
	return false;
}

bool cGirls::IsInvFull(sGirl* girl)
{
	bool full = true;
	if (girl)
	{
		for (int i = 0; i < MAXNUM_GIRL_INVENTORY; i++)
		{
			if (girl->m_Inventory[i] == 0)
			{
				full = false;
				break;
			}
		}
	}
	return full;
}

int cGirls::AddInv(sGirl* girl, sInventoryItem* item)
{

	if (!girl || !item)	return -1;
	int i;
	for (i = 0; i < MAXNUM_GIRL_INVENTORY; i++)
	{
		if (girl->m_Inventory[i] == 0)
		{
			girl->m_Inventory[i] = item;
			girl->m_NumInventory++;
			if (item->m_Type == INVMISC) EquipItem(girl, i, true);
			return i;  // MYR: return i for success, -1 for failure
		}
	}
	return -1;
}

bool cGirls::RemoveInvByNumber(sGirl* girl, int Pos)
{
	// Girl inventories don't stack items
	if (girl->m_Inventory[Pos] != 0)
	{
		g_InvManager.Unequip(girl, Pos);
		girl->m_Inventory[Pos] = 0;
		girl->m_NumInventory--;
		return true;
	}
	return false;
}

void cGirls::SellInvItem(sGirl* girl, int num)
{
	girl->m_Money += (int)((float)girl->m_Inventory[num]->m_Cost*0.5f);
	girl->m_NumInventory--;
	g_InvManager.Unequip(girl, num);
	girl->m_Inventory[num] = 0;
}

int cGirls::GetWorseItem(sGirl* girl, int type, int cost)
{
	int ret = -1;
	if (girl->m_NumInventory == 0) return -1;
	for (int i = 0; i < MAXNUM_GIRL_INVENTORY; i++)
	{
		if (girl->m_Inventory[i])
		{
			if (girl->m_Inventory[i]->m_Type == type && girl->m_Inventory[i]->m_Cost < cost)
			{
				ret = i;
				break;
			}
		}
	}
	return ret;
}

int cGirls::GetNumItemType(sGirl* girl, int Type, bool splitsubtype)
{
	if (girl->m_NumInventory == 0) return 0;
	int num = 0;
	int found = 0;
	for (int i = 0; i < MAXNUM_GIRL_INVENTORY && found<girl->m_NumInventory; i++)
	{
		if (girl->m_Inventory[i])
		{
			found++;
			if (girl->m_Inventory[i]->m_Type == Type)
				num++;
			// if we are looking for consumables (INVFOOD) but we are not splitting subtypes, accept INVMAKEUP as INVFOOD.
			if (Type == INVFOOD && !splitsubtype && girl->m_Inventory[i]->m_Type == INVMAKEUP)
				num++;
		}
	}
	return num;
}

int cGirls::GetNumItemEquiped(sGirl* girl, int Type)
{
	if (girl->m_NumInventory == 0) return 0;
	int num = 0;
	for (int i = 0; i < MAXNUM_GIRL_INVENTORY; i++)
	{
		if (girl->m_Inventory[i])
		{
			if (girl->m_Inventory[i]->m_Type == Type && girl->m_EquipedItems[i] == 1) num++;
		}
	}
	return num;
}

// ----- Traits

// If a girl enjoys a job enough, she has a chance of gaining traits associated with it
bool cGirls::PossiblyGainNewTrait(sGirl* girl, string Trait, int Threshold, int ActionType, string Message, bool Day0Night1, int eventtype)
{
	if (girl->m_Enjoyment[ActionType] > Threshold && !girl->has_trait(Trait))
	{
		int chance = (girl->m_Enjoyment[ActionType] - Threshold);
		if (g_Dice.percent(chance))
		{
			girl->add_trait(Trait, false);
			girl->m_Events.AddMessage(Message, IMGTYPE_PROFILE, eventtype);
			return true;
		}
	}
	return false;
}

// If a girl enjoys a job enough, she has a chance of losing bad traits associated with it
bool cGirls::PossiblyLoseExistingTrait(sGirl* girl, string Trait, int Threshold, int ActionType, string Message, bool Day0Night1)
{
	if (girl->m_Enjoyment[ActionType] > Threshold && girl->has_trait(Trait))
	{
		int chance = (girl->m_Enjoyment[ActionType] - Threshold);
		if (g_Dice.percent(chance))
		{
			girl->remove_trait(Trait);
			girl->m_Events.AddMessage(Message, IMGTYPE_PROFILE, EVENT_GOODNEWS);
			return true;
		}
	}
	return false;
}

// `J` When adding new traits, search for "J-Add-New-Traits"  :  found in >> cGirls.cpp > AdjustTraitGroup

// `J` adding these to allow single step adjustment of linked traits
string cGirls::AdjustTraitGroupGagReflex(sGirl* girl, int adjustment, bool showmessage, bool Day0Night1)
{
	if (girl == 0 || adjustment == 0) return "";	// no girl or not changing anything so quit
	int newGR = 0;
	stringstream ss;
	ss << girl->m_Realname;

	// zzzzzz - These need better texts


	// first we check what she has and remove it if it is changing
	if (girl->has_trait( "Strong Gag Reflex"))	// step -2
	{
		if (adjustment < 0) return "";	// can't go lower
		newGR = -2 + adjustment;
		girl->remove_trait("Strong Gag Reflex", true, true);
		ss << " has lost the trait 'Strong Gag Reflex' ";
	}
	else if (girl->has_trait( "Gag Reflex"))		// step -1
	{
		newGR = -1 + adjustment;
		girl->remove_trait("Gag Reflex", true, true);
		ss << " has lost the trait 'Gag Reflex' ";
	}
	else if (girl->has_trait( "No Gag Reflex"))	// step +1
	{
		newGR = 1 + adjustment;
		girl->remove_trait("No Gag Reflex", true, true);
		ss << " has lost the trait 'No Gag Reflex' ";
	}
	else if (girl->has_trait( "Deep Throat"))		// step +2
	{
		if (adjustment > 0) return "";	// can't go higher
		newGR = 2 + adjustment;
		girl->remove_trait("Deep Throat", true, true);
		ss << " has lost the trait 'Deep Throat' ";
	}
	else /* No trait                              */	// step 0
	{
		newGR = adjustment;
	}

	// then we add the new trait if it has changed
	/* */if (newGR <= -2)
	{
		newGR = -2;
		girl->add_trait("Strong Gag Reflex");
		ss << " has gained the trait 'Strong Gag Reflex'";
	}
	else if (newGR == -1)
	{
		girl->add_trait("Gag Reflex");
		ss << " has gained the trait 'Gag Reflex'";
	}
	else if (newGR == 0)
	{

	}
	else if (newGR == 1)
	{
		girl->add_trait("No Gag Reflex");
		ss << " has gained the trait 'No Gag Reflex'";
	}
	else // if (newGR >= 2)
	{
		newGR = 2;
		girl->add_trait("Deep Throat");
		ss << " has gained the trait 'Deep Throat'";
	}

	// only send a message if called for
	if (showmessage)	girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_GOODNEWS);
	return ss.str();
}

string cGirls::AdjustTraitGroupBreastSize(sGirl* girl, int adjustment, bool showmessage, bool Day0Night1)
{
	if (girl == 0 || adjustment == 0) return "";	// no girl or not changing anything so quit
	int newGR = 0;
	stringstream ss;
	ss << girl->m_Realname;

	// zzzzzz - These need better texts

	if (girl->has_trait( "Flat Chest"))						// step -3
	{
		if (adjustment < 0) return "";	// can't go lower
		newGR = -3 + adjustment;
		girl->remove_trait("Flat Chest", true, true);
		ss << " breast size has changed from 'Flat Chest' ";
	}
	else if (girl->has_trait( "Petite Breasts"))				// step -2
	{
		newGR = -2 + adjustment;
		girl->remove_trait("Petite Breasts", true, true);
		ss << " breast size has changed from 'Petite Breasts' ";
	}
	else if (girl->has_trait( "Small Boobs"))					// step -1
	{
		newGR = -1 + adjustment;
		girl->remove_trait("Small Boobs", true, true);
		ss << " breast size has changed from 'Small Boobs' ";
	}
	else if (girl->has_trait( "Busty Boobs"))					// step +1
	{
		newGR = 1 + adjustment;
		girl->remove_trait("Busty Boobs", true, true);
		ss << " breast size has changed from 'Busty Boobs' ";
	}
	else if (girl->has_trait( "Big Boobs"))					// step +2
	{
		newGR = 2 + adjustment;
		girl->remove_trait("Big Boobs", true, true);
		ss << " breast size has changed from 'Big Boobs' ";
	}
	else if (girl->has_trait( "Giant Juggs"))					// step +3
	{
		newGR = 3 + adjustment;
		girl->remove_trait("Giant Juggs", true, true);
		ss << " breast size has changed from 'Giant Juggs' ";
	}
	else if (girl->has_trait( "Massive Melons"))				// step +4
	{
		newGR = 4 + adjustment;
		girl->remove_trait("Massive Melons", true, true);
		ss << " breast size has changed from 'Massive Melons' ";
	}
	else if (girl->has_trait( "Abnormally Large Boobs"))		// step +5
	{
		newGR = 5 + adjustment;
		girl->remove_trait("Abnormally Large Boobs", true, true);
		ss << " breast size has changed from 'Abnormally Large Boobs' ";
	}
	else if (girl->has_trait( "Titanic Tits"))				// step +6
	{
		if (adjustment > 0) return "";	// can't go higher
		newGR = 6 + adjustment;
		girl->remove_trait("Titanic Tits", true, true);
		ss << " breast size has changed from 'Titanic Tits' ";
	}
	else /* No trait                                          */	// step 0
	{
		newGR = adjustment;
		ss << " breast size has changed from 'Average' ";

	}

	// then we add the new trait if it has changed
	if (newGR <= -3)
	{
		newGR = -3;
		girl->add_trait("Flat Chest");
		ss << " to 'Flat Chest'";
	}
	else if (newGR == -2)
	{
		girl->add_trait("Petite Breasts");
		ss << " to 'Petite Breasts'";
	}
	else if (newGR == -1)
	{
		girl->add_trait("Small Boobs");
		ss << " to 'Small Boobs'";
	}
	else if (newGR == 0)
	{
		girl->add_trait("Average");
		ss << " to 'Average'";
	}
	else if (newGR == +1)
	{
		girl->add_trait("Busty Boobs");
		ss << " to 'Busty Boobs'";
	}
	else if (newGR == +2)
	{
		girl->add_trait("Big Boobs");
		ss << " to 'Big Boobs'";
	}
	else if (newGR == +3)
	{
		girl->add_trait("Giant Juggs");
		ss << " to 'Giant Juggs'";
	}
	else if (newGR == +4)
	{
		girl->add_trait("Massive Melons");
		ss << " to 'Massive Melons'";
	}
	else if (newGR == +5)
	{
		girl->add_trait("Abnormally Large Boobs");
		ss << " to 'Abnormally Large Boobs'";
	}
	else if (newGR >= +6)
	{
		newGR = 6;
		girl->add_trait("Titanic Tits");
		ss << " to 'Titanic Tits'";
	}


	// only send a message if called for
	if (showmessage)	girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_GOODNEWS);
	return ss.str();
}

string cGirls::AdjustTraitGroupFertility(sGirl* girl, int adjustment, bool showmessage, bool Day0Night1)
{
	if (girl == 0 || adjustment == 0) return "";	// no girl or not changing anything so quit
	int newGR = 0;
	stringstream ss;
	ss << girl->m_Realname;

	// zzzzzz - These need better texts

	// first we check what she has and remove it if it is changing
	/* */if (girl->has_trait( "Sterile")) {
		if (adjustment < 0) return "";	// can't go lower
		girl->remove_trait("Sterile", true, true);
		ss << " has lost the trait 'Sterile' ";
		newGR = -1 + adjustment;
	}
	else if (girl->has_trait( "Fertile")) {	// step +1
		girl->remove_trait("Fertile", true, true);
		ss << " has lost the trait 'Fertile' ";
		newGR = 1 + adjustment;
	}
	else if (girl->has_trait( "Broodmother")) {
		if (adjustment > 0) return "";	// can't go higher
		girl->remove_trait("Broodmother", true, true);
		ss << " has lost the trait 'Broodmother' ";
		newGR = 2 + adjustment;
	}
	else /* No trait                              */	// step 0
	{
		newGR = adjustment;
	}

	// then we add the new trait if it has changed
	if (newGR <= -1)
	{
		newGR = -1;
		girl->add_trait("Sterile");
		ss << " has gained the trait 'Sterile'";
	}
	else if (newGR == 0)
	{

	}
	else if (newGR == 1)
	{
		girl->add_trait("Fertile");
		ss << " has gained the trait 'Fertile'";
	}
	else // if (newGR >= 2)
	{
		newGR = 2;
		girl->add_trait("Broodmother");
		ss << " has gained the trait 'Broodmother'";
	}

	// only send a message if called for
	if (showmessage)	girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_GOODNEWS);
	return ss.str();
}

void cGirls::ApplyTraits(sGirl* girl, TraitSpec* trait)
{
	// `J` When adding new traits, search for "J-Add-New-Traits"  :  found in > ApplyTraits
	/* WD:
	*	Added doOnce = false; to end of fn
	*	else the fn will allways abort
	*/
	bool doOnce = false;
	if (trait) doOnce = true;	// only adding one trait
	else						// otherwise reset StatTR/SkillTR/EnjoymentTR to 0 and reload them
	{
		for (int r = 0; r < NUM_STATS; r++)			girl->m_StatTr[r] = 0;
		for (int r = 0; r < NUM_SKILLS; r++)		girl->m_SkillTr[r] = 0;
		for (int r = 0; r < NUM_ACTIONTYPES; r++)	girl->m_EnjoymentTR[r] = 0;

	}
	for (int i = 0; i < girl->m_NumTraits || doOnce; i++)
	{
		TraitSpec* tr = nullptr;
		tr = (doOnce) ? trait : girl->m_Traits[i];
		if (tr == 0) continue;

		tr->apply_effects(girl);

		if (doOnce)
		{
			// WD: 	Added to stop fn from aborting
			// doOnce = false;
			break;
		}
	}
}

void cGirls::MutuallyExclusiveTraits(sGirl* girl, bool apply)
{
	for(int i = 0; i < girl->m_NumTraits; ++i) {
		if(girl->m_Traits[i])
		{
			MutuallyExclusiveTrait(girl, apply, girl->m_Traits[i], false);
		}
	}
}

void cGirls::MutuallyExclusiveTrait(sGirl* girl, bool apply, TraitSpec* trait, bool rememberflag)
{
	string name = trait->name();
	if (name == "") return;

		// `J` base for adding new mutually exclusive traits

		// no need to comment this out because it will always fail to be true because of the first if check

	else if (	// Check _type_ Traits
			name == "" ||
			name == "" ||
			name == "" ||
			name == "")
	{
		if (apply)
		{
			if (name != "")			girl->remove_trait("", rememberflag, true);
			if (name != "")			girl->remove_trait("", rememberflag, true);
			if (name != "")			girl->remove_trait("", rememberflag, true);
			if (name != "")			girl->remove_trait("", rememberflag, true);
		}
		else
		{
			/* */if (name != "" && HasRememberedTrait(girl, ""))	RestoreRememberedTrait(girl, "");
			else if (name != "" && HasRememberedTrait(girl, ""))	RestoreRememberedTrait(girl, "");
			else if (name != "" && HasRememberedTrait(girl, ""))	RestoreRememberedTrait(girl, "");
			else if (name != "" && HasRememberedTrait(girl, ""))	RestoreRememberedTrait(girl, "");
		}
	}

		//	`J` end of base for adding new mutually exclusive traits

#if 1	// Start of Breast traits

	else if (	// Check Breast size traits
			name == "Flat Chest" ||
			name == "Petite Breasts" ||
			name == "Small Boobs" ||
			name == "Busty Boobs" ||
			name == "Big Boobs" ||
			name == "Giant Juggs" ||
			name == "Massive Melons" ||
			name == "Abnormally Large Boobs" ||
			name == "Titanic Tits")
	{
		if (apply)
		{
			if (name != "Flat Chest")				girl->remove_trait("Flat Chest", rememberflag, true);
			if (name != "Petite Breasts")			girl->remove_trait("Petite Breasts", rememberflag, true);
			if (name != "Small Boobs")				girl->remove_trait("Small Boobs", rememberflag, true);
			if (name != "Busty Boobs")				girl->remove_trait("Busty Boobs", rememberflag, true);
			if (name != "Big Boobs")				girl->remove_trait("Big Boobs", rememberflag, true);
			if (name != "Giant Juggs")				girl->remove_trait("Giant Juggs", rememberflag, true);
			if (name != "Massive Melons")			girl->remove_trait("Massive Melons", rememberflag, true);
			if (name != "Abnormally Large Boobs")	girl->remove_trait("Abnormally Large Boobs", rememberflag, true);
			if (name != "Titanic Tits")				girl->remove_trait("Titanic Tits", rememberflag, true);
		}
		else
		{
			/* */if (name != "Flat Chest" && HasRememberedTrait(girl, "Flat Chest"))							RestoreRememberedTrait(girl, "Flat Chest");
			else if (name != "Petite Breasts" && HasRememberedTrait(girl, "Petite Breasts"))					RestoreRememberedTrait(girl, "Petite Breasts");
			else if (name != "Small Boobs" && HasRememberedTrait(girl, "Small Boobs"))							RestoreRememberedTrait(girl, "Small Boobs");
			else if (name != "Busty Boobs" && HasRememberedTrait(girl, "Busty Boobs"))							RestoreRememberedTrait(girl, "Busty Boobs");
			else if (name != "Big Boobs" && HasRememberedTrait(girl, "Big Boobs"))								RestoreRememberedTrait(girl, "Big Boobs");
			else if (name != "Giant Juggs" && HasRememberedTrait(girl, "Giant Juggs"))							RestoreRememberedTrait(girl, "Giant Juggs");
			else if (name != "Massive Melons" && HasRememberedTrait(girl, "Massive Melons"))					RestoreRememberedTrait(girl, "Massive Melons");
			else if (name != "Abnormally Large Boobs" && HasRememberedTrait(girl, "Abnormally Large Boobs"))	RestoreRememberedTrait(girl, "Abnormally Large Boobs");
			else if (name != "Titanic Tits" && HasRememberedTrait(girl, "Titanic Tits"))						RestoreRememberedTrait(girl, "Titanic Tits");
		}
	}
	else if (	// Check Lactation Traits
			name == "Dry Milk" ||
			name == "Scarce Lactation" ||
			name == "Abundant Lactation" ||
			name == "Cow Tits")
	{

		if (apply)
		{
			if (girl->has_trait("No Nipples"))
			{
				// if she has no nipples she can not produce milk, but remember that is has changed in case she grows nipples
				girl->remove_trait("Dry Milk", false, true, name == "Dry Milk");
				girl->remove_trait("Scarce Lactation", false, true, name == "Scarce Lactation");
				girl->remove_trait("Abundant Lactation", false, true, name == "Abundant Lactation");
				girl->remove_trait("Cow Tits", false, true, name == "Cow Tits");
			}
			else
			{
				if (name != "Dry Milk")				girl->remove_trait("Dry Milk", rememberflag, true);
				if (name != "Scarce Lactation")		girl->remove_trait("Scarce Lactation", rememberflag, true);
				if (name != "Abundant Lactation")	girl->remove_trait("Abundant Lactation", rememberflag, true);
				if (name != "Cow Tits")				girl->remove_trait("Cow Tits", rememberflag, true);
			}
		}
		else
		{
			/* */if (name != "Dry Milk" && HasRememberedTrait(girl, "Dry Milk"))						RestoreRememberedTrait(girl, "Dry Milk");
			else if (name != "Scarce Lactation" && HasRememberedTrait(girl, "Scarce Lactation"))		RestoreRememberedTrait(girl, "Scarce Lactation");
			else if (name != "Abundant Lactation" && HasRememberedTrait(girl, "Abundant Lactation"))	RestoreRememberedTrait(girl, "Abundant Lactation");
			else if (name != "Cow Tits" && HasRememberedTrait(girl, "Cow Tits"))						RestoreRememberedTrait(girl, "Cow Tits");

			// make it easy by adding the trait as usual and then move it to remembered if she has no nipples
			if (girl->has_trait("No Nipples"))
			{
				// if she has no nipples she can not produce milk, but remember that is has changed in case she grows nipples
				girl->remove_trait("Dry Milk", true, true, girl->has_trait("Dry Milk"));
				girl->remove_trait("Scarce Lactation", true, true, girl->has_trait("Scarce Lactation"));
				girl->remove_trait("Abundant Lactation", true, true, girl->has_trait("Abundant Lactation"));
				girl->remove_trait("Cow Tits", true, true, girl->has_trait("Cow Tits"));
			}
		}
	}
	else if (	// Check Nipple Traits
			name == "No Nipples" ||
			name == "Missing Nipple" ||
			name == "Inverted Nipples" ||
			name == "Perky Nipples" ||
			name == "Puffy Nipples")
	{
		if (apply)
		{
			// if adding "No Nipples" remove all other nipple traits but remember them
			if (name == "No Nipples" || girl->has_trait("No Nipples"))
			{
				if (girl->has_trait("Dry Milk"))				girl->remove_trait("Dry Milk", true, true, true);
				if (girl->has_trait("Scarce Lactation"))		girl->remove_trait("Scarce Lactation", true, true, true);
				if (girl->has_trait("Abundant Lactation"))	girl->remove_trait("Abundant Lactation", true, true, true);
				if (girl->has_trait("Cow Tits"))				girl->remove_trait("Cow Tits", true, true, true);

				if (name == "Missing Nipple" || girl->has_trait("Missing Nipple"))	girl->remove_trait("Missing Nipple", rememberflag, true, true);
				if (name == "Puffy Nipples" || girl->has_trait("Puffy Nipples"))		girl->remove_trait("Puffy Nipples", rememberflag, true, true);
				if (name == "Inverted Nipples" || girl->has_trait("Perky Nipples"))	girl->remove_trait("Perky Nipples", rememberflag, true, true);
				if (name == "Perky Nipples" || girl->has_trait("Inverted Nipples"))	girl->remove_trait("Inverted Nipples", rememberflag, true, true);
			}
			else
			{
				// she can not have both Inverted and Perky but Puffy can go with either
				if (name == "Inverted Nipples")		girl->remove_trait("Perky Nipples", rememberflag, true);
				if (name == "Perky Nipples")		girl->remove_trait("Inverted Nipples", rememberflag, true);
			}
		}
		else
		{
			// if removing "No Nipples" try adding back the others
			/* */if (name == "No Nipples")
			{
				/* */if (HasRememberedTrait(girl, "Dry Milk"))				RestoreRememberedTrait(girl, "Dry Milk");
				else if (HasRememberedTrait(girl, "Scarce Lactation"))		RestoreRememberedTrait(girl, "Scarce Lactation");
				else if (HasRememberedTrait(girl, "Abundant Lactation"))	RestoreRememberedTrait(girl, "Abundant Lactation");
				else if (HasRememberedTrait(girl, "Cow Tits"))				RestoreRememberedTrait(girl, "Cow Tits");
				/* */if (HasRememberedTrait(girl, "Puffy Nipples"))			RestoreRememberedTrait(girl, "Puffy Nipples");
			}
			/* */if ((name == "No Nipples" || (!girl->has_trait("No Nipples") && name == "Inverted Nipples"))
					 && HasRememberedTrait(girl, "Perky Nipples"))
				RestoreRememberedTrait(girl, "Perky Nipples");
			else if ((name == "No Nipples" || (!girl->has_trait("No Nipples") && name == "Perky Nipples"))
					 && HasRememberedTrait(girl, "Inverted Nipples"))
				RestoreRememberedTrait(girl, "Inverted Nipples");
		}
	}

#endif	// End of Breast traits

#if 1	// Start of Traits that affect sex

	else if (	// Check Fertility Traits
			name == "Sterile" ||
			name == "Broodmother" ||
			name == "Fertile")
	{
		if (apply)
		{
			if (name != "Sterile")		girl->remove_trait("Sterile", rememberflag, true);
			if (name != "Broodmother")	girl->remove_trait("Broodmother", rememberflag, true);
			if (name != "Fertile")		girl->remove_trait("Fertile", rememberflag, true);
		}
		else
		{
			/* */if (name != "Sterile" && HasRememberedTrait(girl, "Sterile"))			RestoreRememberedTrait(girl, "Sterile");
			else if (name != "Broodmother" && HasRememberedTrait(girl, "Broodmother"))	RestoreRememberedTrait(girl, "Broodmother");
			else if (name != "Fertile" && HasRememberedTrait(girl, "Fertile"))			RestoreRememberedTrait(girl, "Fertile");
		}
	}
	else if (	// Check Sexuality Traits
			name == "Bisexual" ||
			name == "Lesbian" ||
			name == "Straight")
	{
		if (apply)
		{
			if (name != "Bisexual")		girl->remove_trait("Bisexual", rememberflag, true);
			if (name != "Lesbian")		girl->remove_trait("Lesbian", rememberflag, true);
			if (name != "Straight")		girl->remove_trait("Straight", rememberflag, true);
		}
		else
		{
			/* */if (name != "Bisexual" && HasRememberedTrait(girl, "Bisexual"))	RestoreRememberedTrait(girl, "Bisexual");
			else if (name != "Lesbian" && HasRememberedTrait(girl, "Lesbian"))		RestoreRememberedTrait(girl, "Lesbian");
			else if (name != "Straight" && HasRememberedTrait(girl, "Straight"))	RestoreRememberedTrait(girl, "Straight");
		}
	}
	else if (	// Check Gag Reflex Traits
			name == "Strong Gag Reflex" ||
			name == "Gag Reflex" ||
			name == "No Gag Reflex" ||
			name == "Deep Throat")
	{
		if (apply)
		{
			if (name != "Strong Gag Reflex")		girl->remove_trait("Strong Gag Reflex", rememberflag, true);
			if (name != "Gag Reflex")				girl->remove_trait("Gag Reflex", rememberflag, true);
			if (name != "No Gag Reflex")			girl->remove_trait("No Gag Reflex", rememberflag, true);
			if (name != "Deep Throat")				girl->remove_trait("Deep Throat", rememberflag, true);
		}
		else
		{
			/* */if (name != "Strong Gag Reflex" && HasRememberedTrait(girl, "Strong Gag Reflex"))	RestoreRememberedTrait(girl, "Strong Gag Reflex");
			else if (name != "Gag Reflex" && HasRememberedTrait(girl, "Gag Reflex"))				RestoreRememberedTrait(girl, "Gag Reflex");
			else if (name != "No Gag Reflex" && HasRememberedTrait(girl, "No Gag Reflex"))			RestoreRememberedTrait(girl, "No Gag Reflex");
			else if (name != "Deep Throat" && HasRememberedTrait(girl, "Deep Throat"))				RestoreRememberedTrait(girl, "Deep Throat");
		}
	}
	else if (	// Check need sex Traits
			name == "Nymphomaniac" ||
			name == "Chaste")
	{
		if (apply)
		{
			if (name != "Nymphomaniac")		girl->remove_trait("Nymphomaniac", rememberflag, true);
			if (name != "Chaste")			girl->remove_trait("Chaste", rememberflag, true);
		}
		else
		{
			/* */if (name != "Nymphomaniac" && HasRememberedTrait(girl, "Nymphomaniac"))	RestoreRememberedTrait(girl, "Nymphomaniac");
			else if (name != "Chaste" && HasRememberedTrait(girl, "Chaste"))				RestoreRememberedTrait(girl, "Chaste");
		}
	}
	else if (	// Check Orgasm Traits
			name == "Fake Orgasm Expert" ||
			name == "Fast Orgasms" ||
			name == "Slow Orgasms")
	{
		if (apply)
		{
			// "Fake Orgasm Expert" overrides the other 2 so if adding it, remove the others but if adding the others don't remove it
			if (name != "Fast Orgasms")			girl->remove_trait("Fast Orgasms", rememberflag, true);
			if (name != "Slow Orgasms")			girl->remove_trait("Slow Orgasms", rememberflag, true);
		}
		else
		{
			/* */if (name != "Fake Orgasm Expert" && HasRememberedTrait(girl, "Fake Orgasm Expert"))	RestoreRememberedTrait(girl, "Fake Orgasm Expert");
			else if (name != "Fast Orgasms" && HasRememberedTrait(girl, "Fast Orgasms"))				RestoreRememberedTrait(girl, "Fast Orgasms");
			else if (name != "Slow Orgasms" && HasRememberedTrait(girl, "Slow Orgasms"))				RestoreRememberedTrait(girl, "Slow Orgasms");
		}
	}

#endif	// End of Traits that affect sex

#if 1	// Start of Physical traits

	else if (	// Check Fragile/Tough Traits
			name == "Fragile" ||
			name == "Tough")
	{
		if (apply)
		{
			if (name != "Fragile")			girl->remove_trait("Fragile", rememberflag, true);
			if (name != "Tough")			girl->remove_trait("Tough", rememberflag, true);
		}
		else
		{
			/* */if (name != "Fragile" && HasRememberedTrait(girl, "Fragile"))	RestoreRememberedTrait(girl, "Fragile");
			else if (name != "Tough" && HasRememberedTrait(girl, "Tough"))		RestoreRememberedTrait(girl, "Tough");
		}
	}
	else if (	// Check Construct Traits
			name == "Construct" ||
			name == "Half-Construct")
	{
		if (apply)
		{
			if (name != "Construct")			girl->remove_trait("Construct", rememberflag, true);
			if (name != "Half-Construct")		girl->remove_trait("Half-Construct", rememberflag, true);
		}
		else
		{
			/* */if (name != "Construct" && HasRememberedTrait(girl, "Construct"))				RestoreRememberedTrait(girl, "Construct");
			else if (name != "Half-Construct" && HasRememberedTrait(girl, "Half-Construct"))	RestoreRememberedTrait(girl, "Half-Construct");
		}
	}
	else if (	// Check Ass Traits
			name == "Flat Ass" ||
			name == "Tight Butt" ||
			name == "Plump Tush" ||
			name == "Great Arse" ||
			name == "Phat Booty" ||
			name == "Wide Bottom" ||
			name == "Deluxe Derriere")
	{
		if (apply)
		{
			if (name != "Flat Ass")				girl->remove_trait("Flat Ass", rememberflag, true);
			if (name != "Tight Butt")			girl->remove_trait("Tight Butt", rememberflag, true);
			if (name != "Plump Tush")			girl->remove_trait("Plump Tush", rememberflag, true);
			if (name != "Great Arse")			girl->remove_trait("Great Arse", rememberflag, true);
			if (name != "Phat Booty")			girl->remove_trait("Phat Booty", rememberflag, true);
			if (name != "Wide Bottom")			girl->remove_trait("Wide Bottom", rememberflag, true);
			if (name != "Deluxe Derriere")		girl->remove_trait("Deluxe Derriere", rememberflag, true);
		}
		else
		{
			/* */if (name != "Flat Ass" && HasRememberedTrait(girl, "Flat Ass"))				RestoreRememberedTrait(girl, "Flat Ass");
			else if (name != "Tight Butt" && HasRememberedTrait(girl, "Tight Butt"))			RestoreRememberedTrait(girl, "Tight Butt");
			else if (name != "Plump Tush" && HasRememberedTrait(girl, "Plump Tush"))			RestoreRememberedTrait(girl, "Plump Tush");
			else if (name != "Great Arse" && HasRememberedTrait(girl, "Great Arse"))			RestoreRememberedTrait(girl, "Great Arse");
			else if (name != "Phat Booty" && HasRememberedTrait(girl, "Phat Booty"))			RestoreRememberedTrait(girl, "Phat Booty");
			else if (name != "Wide Bottom" && HasRememberedTrait(girl, "Wide Bottom"))			RestoreRememberedTrait(girl, "Wide Bottom");
			else if (name != "Deluxe Derriere" && HasRememberedTrait(girl, "Deluxe Derriere"))	RestoreRememberedTrait(girl, "Deluxe Derriere");
		}
	}
	else if (	// Check Teeth Traits
			name == "Missing Teeth" ||
			name == "No Teeth")
	{
		if (apply)
		{
			if (name != "Missing Teeth")	girl->remove_trait("Missing Teeth", rememberflag, true);
			if (name != "No Teeth")			girl->remove_trait("No Teeth", rememberflag, true);
		}
		else
		{
			/* */if (name != "Missing Teeth" && HasRememberedTrait(girl, "Missing Teeth"))	RestoreRememberedTrait(girl, "Missing Teeth");
			else if (name != "No Teeth" && HasRememberedTrait(girl, "No Teeth"))			RestoreRememberedTrait(girl, "No Teeth");
		}
	}
	else if (	// Check Height
			name == "Giant" ||
			name == "Tall" ||
			name == "Short" ||
			name == "Dwarf")
	{
		if (apply)
		{
			if (name != "Giant")		girl->remove_trait("Giant", rememberflag, true);
			if (name != "Tall")			girl->remove_trait("Tall", rememberflag, true);
			if (name != "Short")		girl->remove_trait("Short", rememberflag, true);
			if (name != "Dwarf")		girl->remove_trait("Dwarf", rememberflag, true);
		}
		else
		{
			/* */if (name != "Giant" && HasRememberedTrait(girl, "Giant"))			RestoreRememberedTrait(girl, "Giant");
			else if (name != "Tall" && HasRememberedTrait(girl, "Tall"))			RestoreRememberedTrait(girl, "Tall");
			else if (name != "Short" && HasRememberedTrait(girl, "Short"))			RestoreRememberedTrait(girl, "Short");
			else if (name != "Dwarf" && HasRememberedTrait(girl, "Dwarf"))			RestoreRememberedTrait(girl, "Dwarf");
		}
	}
	else if (	// Check Figure
			name == "Great Figure" ||
			name == "Hourglass Figure" ||
			name == "Plump" ||
			name == "Fat")
	{
		if (apply)
		{
			if (name != "Great Figure")		girl->remove_trait("Great Figure", rememberflag, true);
			if (name != "Hourglass Figure")	girl->remove_trait("Hourglass Figure", rememberflag, true);
			if (name != "Plump")			girl->remove_trait("Plump", rememberflag, true);
			if (name != "Fat")				girl->remove_trait("Fat", rememberflag, true);
		}
		else
		{
			/* */if (name != "Great Figure" && HasRememberedTrait(girl, "Great Figure"))			RestoreRememberedTrait(girl, "Great Figure");
			else if (name != "Hourglass Figure" && HasRememberedTrait(girl, "Hourglass Figure"))	RestoreRememberedTrait(girl, "Hourglass Figure");
			else if (name != "Plump" && HasRememberedTrait(girl, "Plump"))							RestoreRememberedTrait(girl, "Plump");
			else if (name != "Fat" && HasRememberedTrait(girl, "Fat"))								RestoreRememberedTrait(girl, "Fat");
		}
	}


#endif	// End of Physical traits

#if 1	// Start of Appearance traits

	else if (	// Check Tattoo Traits
			name == "Small Tattoos" ||
			name == "Tattooed" ||
			name == "Heavily Tattooed")
	{
		if (apply)
		{
			if (name != "Small Tattoos")		girl->remove_trait("Small Tattoos", rememberflag, true);
			if (name != "Tattooed")				girl->remove_trait("Tattooed", rememberflag, true);
			if (name != "Heavily Tattooed")		girl->remove_trait("Heavily Tattooed", rememberflag, true);
		}
		else
		{
			/* */if (name != "Small Tattoos" && HasRememberedTrait(girl, "Small Tattoos"))			RestoreRememberedTrait(girl, "Small Tattoos");
			else if (name != "Tattooed" && HasRememberedTrait(girl, "Tattooed"))					RestoreRememberedTrait(girl, "Tattooed");
			else if (name != "Heavily Tattooed" && HasRememberedTrait(girl, "Heavily Tattooed"))	RestoreRememberedTrait(girl, "Heavily Tattooed");
		}
	}

#endif	// End of Appearance traits

#if 1	// Start of Magical traits

	else if (	// Check Magic Strength Traits
			name == "Muggle" ||
			name == "Weak Magic" ||
			name == "Strong Magic" ||
			name == "Powerful Magic")
	{
		if (apply)
		{
			if (name != "Muggle")			girl->remove_trait("Muggle", rememberflag, true);
			if (name != "Weak Magic")		girl->remove_trait("Weak Magic", rememberflag, true);
			if (name != "Strong Magic")		girl->remove_trait("Strong Magic", rememberflag, true);
			if (name != "Powerful Magic")	girl->remove_trait("Powerful Magic", rememberflag, true);
		}
		else
		{
			/* */if (name != "Muggle" && HasRememberedTrait(girl, "Muggle"))					RestoreRememberedTrait(girl, "Muggle");
			else if (name != "Weak Magic" && HasRememberedTrait(girl, "Weak Magic"))			RestoreRememberedTrait(girl, "Weak Magic");
			else if (name != "Strong Magic" && HasRememberedTrait(girl, "Strong Magic"))		RestoreRememberedTrait(girl, "Strong Magic");
			else if (name != "Powerful Magic" && HasRememberedTrait(girl, "Powerful Magic"))	RestoreRememberedTrait(girl, "Powerful Magic");
		}
	}

#endif	// End of Magical traits

#if 1	// Start of Social Traits

	else if (	// Check Princess/Queen Traits
			name == "Princess" ||
			name == "Queen")
	{
		if (apply)
		{
			if (name != "Princess")			girl->remove_trait("Princess", rememberflag, true);
			if (name != "Queen")			girl->remove_trait("Queen", rememberflag, true);
		}
		else
		{
			/* */if (name != "Princess" && HasRememberedTrait(girl, "Princess"))	RestoreRememberedTrait(girl, "Princess");
			else if (name != "Queen" && HasRememberedTrait(girl, "Queen"))			RestoreRememberedTrait(girl, "Queen");
		}
	}
	else if (	// Check Lolita/MILF Traits
			name == "Lolita" ||
			name == "MILF" ||
			name == "Middle Aged" ||//zzzzz FIXME
			name == "Old")
	{
		if (apply)
		{
			// If adding Lolita remove the others but if adding the others only remove Lolita
			if (name == "Lolita")
			{
				girl->remove_trait("MILF", rememberflag, true);
				girl->remove_trait("Old", rememberflag, true);
				girl->remove_trait("Middle Aged", rememberflag, true);
			}
			if (name == "MILF")			girl->remove_trait("Lolita", rememberflag, true);
			if (name == "Old")			girl->remove_trait("Lolita", rememberflag, true);
			if (name == "Middle Aged")	girl->remove_trait("Lolita", rememberflag, true);
		}
		else
		{
			// if removing Lolita add back both Old and MILF
			if (name == "Lolita")
			{
				RestoreRememberedTrait(girl, "Old");
				RestoreRememberedTrait(girl, "MILF");
			}
				// if removing Old or MILF try to add the other one and if neither are there then try adding Lolita
			else
			{
				if (name != "Old" && HasRememberedTrait(girl, "Old"))	RestoreRememberedTrait(girl, "Old");
				if (name != "MILF" && HasRememberedTrait(girl, "MILF"))	RestoreRememberedTrait(girl, "MILF");
				if (!girl->has_trait("Old") && !girl->has_trait("MILF"))			RestoreRememberedTrait(girl, "Lolita");
			}
		}
	}
	else if (	// Check Shy/Exhibitionist Traits
			name == "Shy" ||
			name == "Exhibitionist")
	{
		if (apply)
		{
			if (name != "Shy")				girl->remove_trait("Shy", rememberflag, true);
			if (name != "Exhibitionist")	girl->remove_trait("Exhibitionist", rememberflag, true);
		}
		else
		{
			/* */if (name != "Shy" && HasRememberedTrait(girl, "Shy"))						RestoreRememberedTrait(girl, "Shy");
			else if (name != "Exhibitionist" && HasRememberedTrait(girl, "Exhibitionist"))	RestoreRememberedTrait(girl, "Exhibitionist");
		}
	}

#endif	// End of Social Traits

#if 1	// Start of Mental Traits

	else if (	// Check Optimist/Pessimist Traits
			name == "Optimist" ||
			name == "Pessimist")
	{
		if (apply)
		{
			if (name != "Optimist")			girl->remove_trait("Optimist", rememberflag, true);
			if (name != "Pessimist")		girl->remove_trait("Pessimist", rememberflag, true);
		}
		else
		{
			/* */if (name != "Optimist" && HasRememberedTrait(girl, "Optimist"))	RestoreRememberedTrait(girl, "Optimist");
			else if (name != "Pessimist" && HasRememberedTrait(girl, "Pessimist"))	RestoreRememberedTrait(girl, "Pessimist");
		}
	}
	else if (	// singer/tone deaf
			name == "Singer" ||
			name == "Tone Deaf")
	{
		if (apply)
		{
			if (name != "Singer")			girl->remove_trait("Singer", rememberflag, true);
			if (name != "Tone Deaf")		girl->remove_trait("Tone Deaf", rememberflag, true);
		}
		else
		{
			/* */if (name != "Singer" && HasRememberedTrait(girl, "Singer"))	RestoreRememberedTrait(girl, "Singer");
			else if (name != "Tone Deaf" && HasRememberedTrait(girl, "Tone Deaf"))	RestoreRememberedTrait(girl, "Tone Deaf");
		}
	}
	else if (	// Check Willpower Traits
			name == "Broken Will" ||
			name == "Iron Will")
	{
		if (apply)
		{
			if (name != "Broken Will")			girl->remove_trait("Broken Will", rememberflag, true);
			if (name != "Iron Will")			girl->remove_trait("Iron Will", rememberflag, true);
		}
		else
		{
			/* */if (name != "Broken Will" && HasRememberedTrait(girl, "Broken Will"))	RestoreRememberedTrait(girl, "Broken Will");
			else if (name != "Iron Will" && HasRememberedTrait(girl, "Iron Will"))		RestoreRememberedTrait(girl, "Iron Will");
		}
	}
	else if (	// Check Learning Traits
			name == "Slow Learner" ||
			name == "Quick Learner")
	{
		if (apply)
		{
			if (name != "Slow Learner")			girl->remove_trait("Slow Learner", rememberflag, true);
			if (name != "Quick Learner")		girl->remove_trait("Quick Learner", rememberflag, true);
		}
		else
		{
			/* */if (name != "Slow Learner" && HasRememberedTrait(girl, "Slow Learner"))	RestoreRememberedTrait(girl, "Slow Learner");
			else if (name != "Quick Learner" && HasRememberedTrait(girl, "Quick Learner"))	RestoreRememberedTrait(girl, "Quick Learner");
		}
	}
	else if (	// Check Social Force Traits
			name == "Audacity" ||
			name == "Fearless" ||
			name == "Aggressive" ||
			name == "Meek" ||
			name == "Dependant" ||
			name == "Nervous")
	{
		if (apply)
		{
			if (name == "Audacity")
			{
				girl->remove_trait("Meek", rememberflag, true);
				girl->remove_trait("Nervous", rememberflag, true);
				girl->remove_trait("Dependant", rememberflag, true);
			}
			if (name == "Fearless")
			{
				girl->remove_trait("Meek", rememberflag, true);
				girl->remove_trait("Nervous", rememberflag, true);
			}
			if (name == "Aggressive")
			{
				girl->remove_trait("Meek", rememberflag, true);
			}
			if (name == "Meek")
			{
				girl->remove_trait("Aggressive", rememberflag, true);
				girl->remove_trait("Fearless", rememberflag, true);
				girl->remove_trait("Audacity", rememberflag, true);
			}
			if (name == "Nervous")
			{
				girl->remove_trait("Fearless", rememberflag, true);
				girl->remove_trait("Audacity", rememberflag, true);
			}
			if (name == "Dependant")
			{
				girl->remove_trait("Audacity", rememberflag, true);
			}
		}
		else
		{
			// if removing a trait from group a try to add back the others from that
			if ((name == "Audacity" || name == "Fearless" || name == "Aggressive") &&
				!girl->has_trait("Audacity") && !girl->has_trait("Fearless") && !girl->has_trait("Aggressive"))
			{
				RestoreRememberedTrait(girl, "Meek");
			}
			if ((name == "Audacity" || name == "Fearless") &&
				!girl->has_trait("Audacity") && !girl->has_trait("Fearless"))
			{
				RestoreRememberedTrait(girl, "Nervous");
			}
			if (name == "Audacity")
			{
				RestoreRememberedTrait(girl, "Dependant");
			}
			if ((name == "Meek" || name == "Nervous" || name == "Dependant") &&
				!girl->has_trait("Meek") && !girl->has_trait("Nervous") && !girl->has_trait("Dependant"))
			{
				RestoreRememberedTrait(girl, "Audacity");
			}
			if ((name == "Meek" || name == "Nervous") &&
				!girl->has_trait("Meek") && !girl->has_trait("Nervous"))
			{
				RestoreRememberedTrait(girl, "Fearless");
			}
			if (name == "Meek")
			{
				RestoreRememberedTrait(girl, "Aggressive");
			}
		}
	}

#endif	// End of Mental Traits

#if 1	// Start of Action Traits

	else if (	// Check Eyesight Traits
			name == "Blind" ||
			name == "Bad Eyesight" ||
			name == "Sharp-Eyed")
	{
		if (apply)
		{
			if (name != "Blind")			girl->remove_trait("Blind", rememberflag, true);
			if (name != "Bad Eyesight")		girl->remove_trait("Bad Eyesight", rememberflag, true);
			if (name != "Sharp-Eyed")		girl->remove_trait("Sharp-Eyed", rememberflag, true);
		}
		else
		{
			/* */if (name != "Blind" && HasRememberedTrait(girl, "Blind"))					RestoreRememberedTrait(girl, "Blind");
			else if (name != "Bad Eyesight" && HasRememberedTrait(girl, "Bad Eyesight"))	RestoreRememberedTrait(girl, "Bad Eyesight");
			else if (name != "Sharp-Eyed" && HasRememberedTrait(girl, "Sharp-Eyed"))		RestoreRememberedTrait(girl, "Sharp-Eyed");
		}
	}

#endif	// End of Action Traits

		// These Trait groups will be handled differently
#if 1	// Start of Special Group Traits


	else if (	// Check Eye Traits
			name == "Different Colored Eyes" ||
			name == "Eye Patch" ||
			name == "One Eye" ||
			name == "Cyclops")
	{
		if (apply)
		{
			// If she becomes a Cyclops, remove all the other traits and forget them, if she changes back later we assume the new eyes are good and the same color
			if (name == "Cyclops")
			{
				girl->remove_trait("Different Colored Eyes", false, true);
				girl->remove_trait("Eye Patch", false, true);
				girl->remove_trait("One Eye", false, true);
			}
			else if (name == "Different Colored Eyes")	// If something tries to give a girl DCE and she only has 1 eye...
			{
				// if she only has 1 eye because she is a Cyclops (naturally only has space on her face for 1 eye)...
				if (girl->has_trait("Cyclops"))
					girl->remove_trait("Different Colored Eyes", false, true);	// Forget having tried to get it.
					// but if she had 2 eyes but lost 1...
				else if (girl->has_trait("Eye Patch") || girl->has_trait("One Eye"))
					girl->remove_trait("Different Colored Eyes", true, true);	// Remember having tried to get it.
			}
			// If something tries to give her the "One Eye" trait but she is a Cyclops, don't give her "One Eye"
			if (name == "One Eye" && girl->has_trait("Cyclops"))
			{
				girl->remove_trait("One Eye", false, true);
			}
			// If she loses an eye or covers it up with an eye patch and she is not a Cyclops, remove DCE but remember it
			if ((name == "Eye Patch" || name == "One Eye") &&
				(girl->has_trait("Eye Patch") || girl->has_trait("One Eye")) && !girl->has_trait("Cyclops"))
			{
				girl->remove_trait("Different Colored Eyes", true, true);
			}
		}
		else
		{
			// If she removes Cyclops, basically growing a second eye, we assume the new eye is good and the same color as the original one

			// If removing "Different Colored Eyes", we assume this means her eyes have become the same color

			// If removing "Eye Patch" or "One Eye", this may give back DCE if she does not have the others
			if ((name == "Eye Patch" || name == "One Eye") &&
				!girl->has_trait("Eye Patch") && !girl->has_trait("One Eye") && !girl->has_trait("Cyclops"))
			{
				RestoreRememberedTrait(girl, "Different Colored Eyes");
			}
		}
	}

#endif	// End of Eye Traits

#if 0	// Start of Species Traits

	else if (	// Check Eyesight Traits
		name == "Succubus" ||
		name == "Angel" ||
		name == "Battery Operated" ||
		name == "Canine" ||
		name == "Cat Girl" ||
		name == "Cow Girl" ||
		name == "Demon" ||
		name == "Dryad" ||
		name == "Elf" ||
		name == "Equine" ||
		name == "Fallen Goddess" ||
		name == "Furry" ||
		name == "Goddess" ||
		name == "Half-Breed" ||
		name == "Not Human" ||
		name == "Reptilian" ||
		name == "Slitherer" ||
		name == "Solar Powered")
	{
		if (apply)
		{
			if (name != "Succubus")			girl->remove_trait("Succubus", rememberflag, true);
			if (name != "Angel")			girl->remove_trait("Angel", rememberflag, true);
			if (name != "Battery Operated")	girl->remove_trait("Battery Operated", rememberflag, true);
			if (name != "Canine")			girl->remove_trait("Canine", rememberflag, true);
			if (name != "Cat Girl")			girl->remove_trait("Cat Girl", rememberflag, true);
			if (name != "Cow Girl")			girl->remove_trait("Cow Girl", rememberflag, true);
			if (name != "Demon")			girl->remove_trait("Demon", rememberflag, true);
			if (name != "Dryad")			girl->remove_trait("Dryad", rememberflag, true);
			if (name != "Elf")				girl->remove_trait("Elf", rememberflag, true);
			if (name != "Equine")			girl->remove_trait("Equine", rememberflag, true);
			if (name != "Fallen Goddess")	girl->remove_trait("Fallen Goddess", rememberflag, true);
			if (name != "Furry")			girl->remove_trait("Furry", rememberflag, true);
			if (name != "Goddess")			girl->remove_trait("Goddess", rememberflag, true);
			if (name != "Half-Breed")		girl->remove_trait("Half-Breed", rememberflag, true);
			if (name != "Not Human")		girl->remove_trait("Not Human", rememberflag, true);
			if (name != "Reptilian")		girl->remove_trait("Reptilian", rememberflag, true);
			if (name != "Slitherer")		girl->remove_trait("Slitherer", rememberflag, true);
			if (name != "Solar Powered")	girl->remove_trait("Solar Powered", rememberflag, true);
		}
		else
		{
			/* */if (name != "Succubus" && HasRememberedTrait(girl, "Succubus"))					RestoreRememberedTrait(girl, "Succubus");
			else if (name != "Angel" && HasRememberedTrait(girl, "Angel"))							RestoreRememberedTrait(girl, "Angel");
			else if (name != "Battery Operated" && HasRememberedTrait(girl, "Battery Operated"))	RestoreRememberedTrait(girl, "Battery Operated");
			else if (name != "Canine" && HasRememberedTrait(girl, "Canine"))						RestoreRememberedTrait(girl, "Canine");
			else if (name != "Cat Girl" && HasRememberedTrait(girl, "Cat Girl"))					RestoreRememberedTrait(girl, "Cat Girl");
			else if (name != "Cow Girl" && HasRememberedTrait(girl, "Cow Girl"))					RestoreRememberedTrait(girl, "Cow Girl");
			else if (name != "Demon" && HasRememberedTrait(girl, "Demon"))							RestoreRememberedTrait(girl, "Demon");
			else if (name != "Dryad" && HasRememberedTrait(girl, "Dryad"))							RestoreRememberedTrait(girl, "Dryad");
			else if (name != "Elf" && HasRememberedTrait(girl, "Elf"))								RestoreRememberedTrait(girl, "Elf");
			else if (name != "Equine" && HasRememberedTrait(girl, "Equine"))						RestoreRememberedTrait(girl, "Equine");
			else if (name != "Fallen Goddess" && HasRememberedTrait(girl, "Fallen Goddess"))		RestoreRememberedTrait(girl, "Fallen Goddess");
			else if (name != "Furry" && HasRememberedTrait(girl, "Furry"))							RestoreRememberedTrait(girl, "Furry");
			else if (name != "Goddess" && HasRememberedTrait(girl, "Goddess"))						RestoreRememberedTrait(girl, "Goddess");
			else if (name != "Half-Breed" && HasRememberedTrait(girl, "Half-Breed"))				RestoreRememberedTrait(girl, "Half-Breed");
			else if (name != "Not Human" && HasRememberedTrait(girl, "Not Human"))					RestoreRememberedTrait(girl, "Not Human");
			else if (name != "Reptilian" && HasRememberedTrait(girl, "Reptilian"))					RestoreRememberedTrait(girl, "Reptilian");
			else if (name != "Slitherer" && HasRememberedTrait(girl, "Slitherer"))					RestoreRememberedTrait(girl, "Slitherer");
			else if (name != "Solar Powered" && HasRememberedTrait(girl, "Solar Powered"))			RestoreRememberedTrait(girl, "Solar Powered");
		}
	}

#endif	// End of Species Traits

#if 1	// Start of Unsorted Traits



#endif	// End of Unsorted Traits
}



bool cGirls::HasRememberedTrait(sGirl* girl, string trait)
{
	for (int i = 0; i < MAXNUM_TRAITS * 2; i++)
	{
		if (girl->m_RememTraits[i])
		{
			if (trait.compare(girl->m_RememTraits[i]->name()) == 0) return true;
		}
	}
	return false;
}
bool cGirls::RestoreRememberedTrait(sGirl* girl, string trait)
{
	if (HasRememberedTrait(girl, trait)) RemoveRememberedTrait(girl, trait);
	else return false;							//	WD:	No trait to add

	for (int i = 0; i < MAXNUM_TRAITS; i++)				// add the trait
	{
		if (girl->m_Traits[i] == 0)
		{
			girl->m_NumTraits++;
			TraitSpec *addthistrait = g_Traits.GetTrait(trait);
			girl->m_Traits[i] = addthistrait;

			return true;
		}
	}
	return false;

}


// `J` returns the number of turns left on a temp trait or 0 if is not temporary
int cGirls::HasTempTrait(sGirl* girl, string trait)
{
	if (girl)
	{
		for (int i = 0; i < MAXNUM_TRAITS; i++)
		{
			if (girl->m_Traits[i] && girl->m_TempTrait[i] > 0)
			{
				if (trait.compare(girl->m_Traits[i]->name()) == 0)
				{
					return girl->m_TempTrait[i];
				}
			}
		}
	}
	else
	{
		if (cfg.debug.log_debug()) { g_LogFile.ss() << "Debug HasTempTrait || Girl is NULL"; g_LogFile.ssend(); }
	}
	return 0;
}

void cGirls::RemoveRememberedTrait(sGirl* girl, string name)
{
	if (girl)
	{
		TraitSpec* trait = g_Traits.GetTrait(name);
		for (int i = 0; i < MAXNUM_TRAITS * 2; i++)	// remove the traits
		{
			if (girl->m_RememTraits[i])
			{
				if (girl->m_RememTraits[i] == trait)
				{
					girl->m_NumRememTraits--;
					girl->m_RememTraits[i] = 0;
					return;
				}
			}
		}
	}
	else
	{
		if (cfg.debug.log_debug()) { g_LogFile.ss() << "Debug RemoveRememberedTrait || Girl is NULL"; g_LogFile.ssend(); }
	}
}

void cGirls::RemoveAllRememberedTraits(sGirl* girl)
{
	/*
	*	WD: Remove all traits to for new girl creation
	*		is required after using ApplyTraits() & UnApplyTraits()
	*		can create remembered traits especially random girls
	*/
	for (int i = 0; i < MAXNUM_TRAITS * 2; i++)
	{
		girl->m_RememTraits[i] = 0;
	}
	girl->m_NumRememTraits = 0;
}

bool sGirl::lose_virginity()	{
	/*  Very similar to (and uses) RemoveTrait(). Added since trait "Virgin" created 04/14/2013.
	*	This includes capability for items, magic or other processes
	*	to have a "remove but remember" effect, like a "Belt of False Defloration"
	*	that provides a magical substitute vagina, preserving the original while worn.
	*	Well, why not?		DustyDan
	*/

	bool traitOpSuccess = false;
	m_Virgin = 0;
	traitOpSuccess = remove_trait("Virgin");
	return traitOpSuccess;
}

void cGirls::AddRememberedTrait(sGirl* girl, string name)
{
	for (int i = 0; i < MAXNUM_TRAITS * 2; i++)	// add the traits
	{
		if (girl->m_RememTraits[i] == 0)
		{
			girl->m_NumRememTraits++;
			girl->m_RememTraits[i] = g_Traits.GetTrait(name);
			return;
		}
	}
}

// Update temp traits and remove expired traits
void cGirls::updateTempTraits(sGirl* girl)
{
	if (girl->is_dead()) return;		// Sanity check. Abort on dead girl
	for (int i = 0; i < MAXNUM_TRAITS; i++)
	{
		if (girl->m_Traits[i] && girl->m_TempTrait[i] > 0)
		{
			girl->m_TempTrait[i]--;
			if (girl->m_TempTrait[i] == 0)
				girl->remove_trait(girl->m_Traits[i]->name());
		}
	}
}

// Update individual temp trait and remove expired trait - can not make nontemp traits temp
void cGirls::updateTempTraits(sGirl* girl, string trait, int amount)
{
	if (girl->is_dead()) return;									// Sanity check. Abort on dead girl

	if (!girl->has_trait( trait))									// first check if she does not have the trait already
	{
		if (amount > 0)													// add it if modifier is positive
			girl->add_trait(trait, amount);
		return;
	}
	else																// if she does have it, check if it is permanent or temp
	{
		for (int i = 0; i < MAXNUM_TRAITS; i++)							// go through her traits
		{
			if (girl->m_Traits[i] && trait.compare(girl->m_Traits[i]->name()) == 0)			// until you find the trait
			{
				if (girl->m_TempTrait[i] > 0)							// if the trait is temp ...
				{
					girl->m_TempTrait[i] += amount;						// ... adjust the temp time
					if (girl->m_TempTrait[i] <= 0)						// if the temp trait is reduced below 1 ...
						girl->remove_trait(girl->m_Traits[i]->name());	// ... remove it
					if (girl->m_TempTrait[i] > 100)						// if the temp trait goes above 100 ...
						girl->m_TempTrait[i] = 0;						// ... make it permanet
				}
				return;
			}
		}
	}
}

// Update happiness for trait affects
void cGirls::updateHappyTraits(sGirl* girl)
{
	if (girl->is_dead()) return;	// Sanity check. Abort on dead girl
	if (girl->has_trait("Optimist")) girl->happiness(5);

	if (girl->has_trait("Pessimist"))
	{
		girl->happiness(-5);
		if (girl->happiness() <= 0 && g_Dice.percent(50))
		{
			stringstream ss;
			string stopper = "";
			if (girl->m_InArena && g_Arena.GetNumGirlsOnJob(0, JOB_DOCTORE, 0) > 0)
				stopper = "the Doctore";
			else if (girl->m_InStudio && g_Studios.GetNumGirlsOnJob(0, JOB_DIRECTOR, 1) > 0)
				stopper = "the Director";
			else if (girl->m_InClinic && g_Clinic.GetNumGirlsOnJob(0, JOB_CHAIRMAN, 0) > 0)
				stopper = "the Chairman";
			else if (girl->m_InCentre && (girl->m_DayJob == JOB_REHAB || girl->m_PrevDayJob == JOB_REHAB) && g_Centre.GetNumGirlsOnJob(0, JOB_COUNSELOR, 0) > 0)
				stopper = "her Counselor";
			else if (girl->m_InCentre && g_Centre.GetNumGirlsOnJob(0, JOB_CENTREMANAGER, 0) > 0)
				stopper = "the Centre Manager";
			else if (girl->m_InHouse && g_Dice.percent(50))
				stopper = "You";
			else if (girl->m_InHouse && g_House.GetNumGirlsOnJob(0, JOB_HEADGIRL, 0) > 0)
				stopper = "your Head Girl";
			else if (g_Brothels.GetNumGirlsOnJob(girl->where_is_she, JOB_MATRON, 0) > 0)
				stopper = "the Matron";
			else if (girl->m_InFarm && g_Farm.GetNumGirlsOnJob(0, JOB_FARMMANGER, 0) > 0)
				stopper = "the Farm Manger";

			int Schance = g_Dice.d100();
			if (stopper != "")
			{
				ss << girl->m_Realname << " tried to killed herself but " << stopper;
				if (Schance < 50)		{ ss << " talked her out of it."; }
				else if (Schance < 90)	{ ss << " stopped her."; }
				else	{ girl->m_Stats[STAT_HEALTH] = 1;	ss << " revived her."; }
				girl->m_Events.AddMessage(ss.str(), IMGTYPE_DEATH, EVENT_DANGER);
			}
			else
			{
				string msg = girl->m_Realname + " has killed herself since she was unhappy and depressed.";
				girl->m_Events.AddMessage(ss.str(), IMGTYPE_DEATH, EVENT_DANGER);
				g_MessageQue.AddToQue(ss.str(), COLOR_RED);
				girl->health(-1000);
			}
		}
	}
}

// ----- Sex

void cGirls::GirlFucks(sGirl* girl, bool Day0Night1, sCustomer* customer, bool group, string& message, u_int& SexType)
{
	int check = girl->get_skill(SexType);
	string girlName = girl->m_Realname;

	if (cfg.debug.log_extradetails())
	{
		message += "\n(Debug: Customer wants ";
		/* */if (SexType == SKILL_ANAL)			message += "anal";
		else if (SexType == SKILL_BDSM)			message += "bondage";
		else if (SexType == SKILL_NORMALSEX)	message += "sex";
		else if (SexType == SKILL_BEASTIALITY)	message += "beast";
		else if (SexType == SKILL_GROUP)		message += "gangbang";
		else if (SexType == SKILL_LESBIAN)		message += "lesbian";
		else if (SexType == SKILL_STRIP)		message += "a strip";
		else if (SexType == SKILL_ORALSEX)		message += "oral";
		else if (SexType == SKILL_TITTYSEX)		message += "titty sex";
		else if (SexType == SKILL_HANDJOB)		message += "a handjob";
		else if (SexType == SKILL_FOOTJOB)		message += "a footjob";
		message += ").\n";
	}

	//SIN: let's add a problem...
	if (g_Dice.percent(33) && (girl->happiness() < 40) && (girl->intelligence() < 50)
		&& (girl->has_trait("Shroud Addict") || girl->has_trait("Fairy Dust Addict") || girl->has_trait("Viras Blood Addict")))
	{
		stringstream runawaymsg;
		if (girl->is_yourdaughter()) runawaymsg << "Your daughter ";
		runawaymsg << girlName << " ran away with your rival!\nExploiting her desperate drug cravings, he claimed to be ";

		//the con
		switch (check % 6) //avoids a dice roll
		{
		case 0:
			runawaymsg << "her true love, ";
			break;
		case 1:
			runawaymsg << "a Prince, ";
			break;
		case 2:
			runawaymsg << "the Spy who Loves her, ";
			break;
		case 3:
			runawaymsg << "her long-lost brother, ";
			break;
		case 4:
			runawaymsg << "a Priest, ";
			break;
		case 5:
			runawaymsg << "her old school friend, ";
			break;
		default:
			runawaymsg << "a h(E)ro ";
			break;
		}
		runawaymsg << "promising her a better life and everything she needs if she escaped with him.\nShe did. By now she will probably be ";

		//speculate
		switch (g_Dice % 6)
		{
		case 0:
			runawaymsg << "tied over a park-bench being gang-raped by ";
			break;
		case 1:
			runawaymsg << "chained to a rack being ram-raided by ";
			break;
		case 2:
			runawaymsg << "tied up in a dumpster giving blowjobs to ";
			break;
		case 3:
			runawaymsg << "folded over a fence being 'used' by ";
			break;
		case 4:
			runawaymsg << "naked in the town arena being 'conquered' by ";
			break;
		case 5:
			runawaymsg << "stripped and locked in the town's public-stocks, being 'punished' by ";
			break;
		default:
			runawaymsg << "being abus(E)d by ";
			break;
		}
		switch (g_Dice % 8)
		{
		case 0:
			runawaymsg << "rival gang-members.";
			break;
		case 1:
			runawaymsg << "bums.";
			break;
		case 2:
			runawaymsg << "horny street kids.";
			break;
		case 3:
			runawaymsg << "mistreated slaves.";
			break;
		case 4:
			runawaymsg << "wild animals.";
			break;
		case 5:
			runawaymsg << "pumped-up gladiators.";
			break;
		case 6:
			runawaymsg << "the town's gentlemen.";
			break;
		case 7:
			runawaymsg << "aristocrats.";
			break;
		default:
			runawaymsg << "(E)veryone.";
			break;
		}
		runawaymsg << "\n";

		//What do you do...
		/* */if (The_Player->disposition() < -33) runawaymsg << "She's where she deserves. Why waste a gang's time going to fetch her? Unless you want to punish personally?";
		else if (The_Player->disposition() < 33) runawaymsg << "You could send a gang to retrieve her. Or you could leave her. No hurry.";
		else runawaymsg << "You should send a gang right away to rescue the poor girl.";

		runawaymsg << "(When you find her, she may be... changed.)";

		//If she was a virgin, she won't be now...
		girl->lose_virginity();

		//What damage?
		int harm = g_Dice.d100();
		if (harm > 95) //5% multi STDS
		{
			harm = g_Dice.d100();
			if (harm == 100) girl->add_trait("AIDS"), girl->add_trait("Syphilis"), girl->add_trait("Herpes"), girl->add_trait("Chlamydia");
			else if (harm > 95) girl->add_trait("AIDS"),		girl->add_trait("Syphilis");
			else if (harm > 85) girl->add_trait("AIDS"),		girl->add_trait("Herpes");
			else if (harm > 70) girl->add_trait("Syphilis"),	girl->add_trait("Herpes");
			else if (harm > 50) girl->add_trait("Syphilis"),	girl->add_trait("Chlamydia");
			else				girl->add_trait("Herpes"),	girl->add_trait("Chlamydia");
		}
		else if (harm > 90)  //5% an STD
		{
			harm = g_Dice.d100();
			if (harm > 95)		girl->add_trait("AIDS");
			else if (harm > 80)	girl->add_trait("Syphilis");
			else if (harm > 50) girl->add_trait("Herpes");
			else				girl->add_trait("Chlamydia");
		}
		else if (harm > 85)  //10% scars
		{
			if (!girl->has_trait("Small Scars") && !girl->has_trait("Cool Scars") && !girl->has_trait("Horrific Scars")) girl->add_trait("Small Scars");
			else if (girl->has_trait("Small Scars")) girl->add_trait("Cool Scars");
			else if (girl->has_trait("Cool Scars")) girl->add_trait("Horrific Scars");
		}
		else if (harm > 75)  //10% traumatised
		{
			girl->add_trait("Mind Fucked");
		}
		else if (harm > 50)  //25% chance
		{
			//overused face
			girl->add_trait("Missing Teeth");
			AdjustTraitGroupGagReflex(girl, +1);

			girl->remove_trait("Optimist");
			girl->upd_stat(STAT_DIGNITY, -10);
			girl->upd_stat(STAT_SPIRIT, -10);
			girl->upd_skill(SKILL_ORALSEX, 5);
		}
		else if (harm > 25)  //25% chance
		{
			//overused behind
			girl->add_trait("Whore");

			girl->remove_trait("Optimist");
			girl->upd_stat(STAT_HEALTH, -5);
			girl->upd_stat(STAT_SPIRIT, -5);
			girl->upd_temp_stat(STAT_LIBIDO, -50, true);
			girl->upd_skill(SKILL_NORMALSEX, 5);
		}
		else if (harm > 15)  //10% chance
		{
			girl->add_trait("Broken Will");
			girl->add_trait("Branded on the Ass");
		}
		else //15% no damage
		{

		}


		//does she get knocked up?
		bool antiPregStatus = girl->m_UseAntiPreg;
		girl->m_UseAntiPreg = false;					//won't have access to this
		girl->calc_group_pregnancy(*customer, false, 2);
		girl->m_UseAntiPreg = antiPregStatus;			//afterwards she'll go back to normal

		// player has 6 weeks to retreive
		girl->m_RunAway = 6;
		g_Brothels.RemoveGirl(girl->where_is_she, girl, false);
		girl->m_NightJob = girl->m_DayJob = JOB_RUNAWAY;
		g_Brothels.AddGirlToRunaways(girl);

		//Warn the user
		g_MessageQue.AddToQue(runawaymsg.str(), COLOR_RED);
		return;
	}


	bool good = false;
	bool contraception = false;
	double STDchance = 0.001;		// `J` added new percent that allows 3 decimal check so setting a 0.001% base chance
	int happymod = 0;	// Start the customers unhappiness/happiness bad sex decreases, good sex inceases
	if (girl->has_trait("Fake Orgasm Expert"))		happymod += 20;
	else if (girl->has_trait("Fast Orgasms"))		happymod += 10;
	else if (girl->has_trait("Slow Orgasms"))		happymod -= 10;
	if (girl->has_trait("Psychic"))					happymod += 10;	// she knows what the customer wants
	if (girl->has_trait("Shape Shifter"))			happymod += 10;	// she can be anything the customer wants

	if (customer->m_Fetish == FETISH_FUTAGIRLS)
	{
		if (girl->has_trait("Futanari"))				happymod += 50;
		else										happymod -= 10;
	}

	if (customer->m_Fetish == FETISH_BIGBOOBS)
	{
		/* */if (girl->has_trait("Flat Chest"))				happymod -= 15;
		else if (girl->has_trait("Petite Breasts"))			happymod -= 10;
		else if (girl->has_trait("Small Boobs"))				happymod -= 5;
		else if (girl->has_trait("Busty Boobs"))				happymod += 4;
		else if (girl->has_trait("Big Boobs"))				happymod += 8;
		else if (girl->has_trait("Giant Juggs"))				happymod += 12;
		else if (girl->has_trait("Massive Melons"))			happymod += 16;
		else if (girl->has_trait("Abnormally Large Boobs"))	happymod += 20;
		else if (girl->has_trait("Titanic Tits"))			happymod += 25;
	}
	else if (customer->m_Fetish == FETISH_SMALLBOOBS)
	{
		/* */if (girl->has_trait("Flat Chest"))				happymod += 15;
		else if (girl->has_trait("Petite Breasts"))			happymod += 20;
		else if (girl->has_trait("Small Boobs"))				happymod += 10;
		else if (girl->has_trait("Busty Boobs"))				happymod -= 2;
		else if (girl->has_trait("Big Boobs"))				happymod -= 5;
		else if (girl->has_trait("Giant Juggs"))				happymod -= 10;
		else if (girl->has_trait("Massive Melons"))			happymod -= 15;
		else if (girl->has_trait("Abnormally Large Boobs"))	happymod -= 20;
		else if (girl->has_trait("Titanic Tits"))			happymod -= 30;
	}
	else
	{
		/* */if (girl->has_trait("Flat Chest"))				happymod -= 2;
		else if (girl->has_trait("Petite Breasts"))			happymod -= 1;
		else if (girl->has_trait("Small Boobs"))				happymod += 0;
		else if (girl->has_trait("Busty Boobs"))				happymod += 1;
		else if (girl->has_trait("Big Boobs"))				happymod += 2;
		else if (girl->has_trait("Giant Juggs"))				happymod += 1;
		else if (girl->has_trait("Massive Melons"))			happymod += 0;
		else if (girl->has_trait("Abnormally Large Boobs"))	happymod -= 1;
		else if (girl->has_trait("Titanic Tits"))			happymod -= 2;
	}

	if (customer->m_Fetish == FETISH_ARSE)
	{
		/* */if (girl->has_trait("Great Arse"))				happymod += 25;
		else if (girl->has_trait("Deluxe Derriere"))			happymod += 25;
		else if (girl->has_trait("Tight Butt"))				happymod += 10;
		else if (girl->has_trait("Phat Booty"))				happymod += 15;
		else if (girl->has_trait("Wide Bottom"))				happymod += 10;
		else if (girl->has_trait("Plump Tush"))				happymod += 5;
		else if (girl->has_trait("Flat Ass"))				happymod -= 30;
	}
	else
	{
		/* */if (girl->has_trait("Great Arse"))				happymod += 3;
		else if (girl->has_trait("Deluxe Derriere"))			happymod += 3;
		else if (girl->has_trait("Tight Butt"))				happymod += 2;
		else if (girl->has_trait("Phat Booty"))				happymod += 1;
		else if (girl->has_trait("Wide Bottom"))				happymod += 0;
		else if (girl->has_trait("Plump Tush"))				happymod += 0;
		else if (girl->has_trait("Flat Ass"))				happymod -= 2;
	}

	girl->m_NumCusts += (int)customer->m_Amount;
	if (group && (customer->m_SexPref != SKILL_GROUP || customer->m_SexPref != SKILL_STRIP))
	{
		// the customer will be an average in all skills for the customers involved in the sex act
		SexType = SKILL_GROUP;
	}

	// If the girls skill < 50 then it will be unsatisfying otherwise it will be satisfying
	happymod += (girl->get_skill(SexType) - 50) / 5;
	// If the girl is famous then he will be slightly happier
	happymod += girl->fame() / 5;

	// her service ability will also make him happier (I.e. does she help clean him well)
	happymod += girl->service() / 10;

	int value = customer->m_Stats[STAT_HAPPINESS] + happymod;			// `J` now set customers happiness
	if (value > 100)	{ customer->m_Stats[STAT_HAPPINESS] = 100; }
	else if (value < 0)	{ customer->m_Stats[STAT_HAPPINESS] = 0; }
	else				{ customer->m_Stats[STAT_HAPPINESS] += happymod; }

	// her magic ability can make him think he enjoyed it more if she has mana

	int happycost = 3 - int(girl->magic() / 40);	// `J` how many mana will each point of happy cost her
	if (happycost < 1) happycost = 1;		// so [magic:cost] [<10:can't] [10-39:3] [40-79:2] [80+:1] (probably, I hate math)
	if (customer->m_Stats[STAT_HAPPINESS] < 100 &&			// If they are not fully happy
		girl->mana() >= happycost &&		// If she has enough mana to actually try
		girl->magic() > 9)				// If she has at least 10 magic
	{
		int happymana = girl->mana();					// check her mana
		if (happymana > 20) happymana = 20;							// and only max of 20 will be used
		int happygain = happymana / happycost;						// check how much she can increase happiness with 20 mana
		if (happygain > 10) happygain = 10;							// can only increase happy by 10
		int lesshappy = 100 - customer->m_Stats[STAT_HAPPINESS];	// how much can she charm him before it is wasted?
		if (happygain > lesshappy) happygain = lesshappy;			// can only increase happy by 10
		happymana = happygain * happycost;							// check how much mana she actually spends
		if (happymana > 20) happymana = 20;							// correct incase more than 20
		if (happymana < 0) happymana = 1;							// will spend at least 1 mana just for trying

		customer->m_Stats[STAT_HAPPINESS] += happygain;				// now apply happy
		girl->upd_stat(STAT_MANA, -happymana);					// and apply mana
	}


	string introtext = girlName;
	// `J` not sure if all of the options will come up in appropriate order but it is a good start.
	int intro = g_Dice % 15;
	intro += girl->libido() / 20;
	intro += check / 20;

	bool z = false;
	if (girl->has_trait("Zombie"))
	{
		z = true;
		introtext += " follows";
	}
	else
	{
		// need to add more traits
		if (girl->has_trait("Succubus"))		intro += 5;
		if (!customer->m_IsWoman && girl->has_trait("Cum Addict")) intro += 4;
		if (customer->m_IsWoman && girl->has_trait("Lesbian"))	intro += 3;
		if (girl->has_trait("Nymphomaniac"))	intro += 4;
		if (girl->has_trait("Fast Orgasms"))	intro += 3;
		if (girl->has_trait("Whore"))		intro += 2;
		if (girl->has_trait("Aggressive"))	intro += 1;
		if (girl->has_trait("Open Minded"))	intro += 1;
		if (girl->has_trait("Slut"))			intro += 1;
		if (girl->has_trait("Homeless"))		intro += 1; //Just happy to be off the street?  CRAZY
		if (girl->has_trait("Optimist"))		intro += 1;
		if (girl->has_trait("Pessimist"))	intro -= 1;
		if (girl->has_trait("Meek"))			intro -= 1;
		if (girl->has_trait("Noble"))		intro -= 1;//maybe again CRAZY
		if (girl->has_trait("Nervous"))		intro -= 2;
		if (girl->has_trait("Chaste"))		intro -= 2;
		if (girl->has_trait("Shy"))			intro -= 2;
		if (girl->has_trait("Princess"))		intro -= 2;//maybe again CRAZY
		if (girl->has_trait("Priestess"))	intro -= 2;//maybe again CRAZY guess it would depend on the type of priestess
		if (girl->has_trait("Slow Orgasms"))	intro -= 3;
		if (girl->has_trait("Queen"))		intro -= 3; //maybe again CRAZY
		if (customer->m_IsWoman && girl->has_trait("Straight"))	intro -= 3;
		if (girl->has_trait("Your Wife"))	intro -= 3; //maybe this idk CRAZY might need a love check also
		if (girl->check_virginity())		intro -= 5;
		if (girl->has_trait("Kidnapped"))	intro -= 5;
		if (girl->has_trait("Emprisoned Customer"))	intro -= 5;


		//SIN: Fix ordering and wording - delete old if this okay
		/* */if (intro < 2)		introtext += " reluctantly leads";
		else if (intro < 4)		introtext += " hesitantly leads";
		else if (intro < 8)		introtext += " leads";
		else if (intro < 12)	introtext += " quickly leads";
		else if (intro < 18)	introtext += " eagerly leads";
		else if (intro < 22)	introtext += " excitedly leads";
		else if (intro < 25)	introtext += " almost drags";
		else					introtext += " eagerly drags";
	}

	if (SexType == SKILL_GROUP)	introtext += " her customers ";
	else introtext += " her customer ";

	int currentjob = (Day0Night1 ? girl->m_NightJob : girl->m_DayJob);
	if (currentjob == JOB_WHOREBROTHEL || currentjob == JOB_BARWHORE || currentjob == JOB_WHOREGAMBHALL)
	{
		//SIN - added some variety here
		//How many options do we need for a litttle randomness below? State it here.
		const int OPTS = 4;
		int RANDROLL = g_Dice%OPTS;

		if (SexType == SKILL_GROUP && g_Dice.percent(40))
		{
			introtext += "to the ";
			switch (RANDROLL)
			{
			case 0:
				introtext += "pirate-themed 'Cap'n's Orrrrgy Cabin.'\n";
				break;
			case 1:
				introtext += "college-themed dorm room.\n";
				break;
			case 2:
				introtext += "prison-themed communal shower.\n";
				break;
			case 3:
				introtext += "secluded orgy garden.\n";
				break;
			default:
				introtext += "gen(E)ric orgy place.\n";
				break;
			}
		}
		else if (SexType == SKILL_BDSM && g_Dice.percent(40))
		{
			// now, what kind of person are you?
			if (The_Player->disposition() < -40)	//You're related to satan, right?
			{
				introtext += "deep underground to your infamous dungeons.\n";
				switch (RANDROLL)
				{
				case 0:
					introtext += "\"This is really it, isn't it?!\" he smiles, wonderstruck. ";
					break;
				case 1:
					introtext += "He stops, crouching, putting his fingers to a dark stain on the concrete floor: \"I've heard so much about this place,\" he whispers. ";
					break;
				case 2:
					introtext += "\"So it's true. All of it!\" he glances between the stock, the chains, the whips and cages. ";
					break;
				case 3:
					introtext += "\"They say you can do ANYTHING here,\" he hisses. ";
					break;
				default:
					introtext += "He is speechl(E)ss: ";
					break;
				}
				introtext += "\"" + The_Player->RealName() + "'s Dungeon.\"\n";
				if (g_Dice.percent(20)) introtext += "\nHe laughs nervously. \"I do get to come out again, right?\"\n";
				customer->m_Stats[STAT_HAPPINESS] += 10;	//Evil customer likes evil
			}
			else if (The_Player->disposition() < 0)		//You're bad, you're bad, you know it.
			{
				introtext += "to your ominous dungeons.\n";
				switch (RANDROLL)
				{
				case 0:
					introtext += "The customer looks around: \"This is it... ";
					break;
				case 1:
					introtext += "\"I've heard about this place,\" he says evenly. \"";
					break;
				case 2:
					introtext += "\"I heard the rumours but I never believed them,\" he says. \"";
					break;
				case 3:
					introtext += "He grins: \"So you'll be my toy here in ";
					break;
				default:
					introtext += "He is speechl(E)ss: \"";
					break;
				}
				introtext += The_Player->RealName() + "'s Dungeon.\"\n";
			}
			else if (The_Player->disposition() < 40)		//You're good
			{
				introtext += "to the dungeons.\n";
			}
			else									//You're nice, bless your little cotton socks.
			{
				introtext += "to an aseptic, custom-made and well-equipped Bondage room";
				switch (RANDROLL)
				{
				case 0:
					introtext += " right next to the first aid station.\n";
					break;
				case 1:
					introtext += " within earshot of the guard station. Just in case.\n";
					break;
				case 2:
					introtext += " designed for safety first.\n\"Some interesting toys here\" he says.";
					break;
				case 3:
					introtext += ".\nThe customer nods approvingly.\n";
					break;
				default:
					introtext += ". Wow you'r(E) nice.\n";
					break;
				}
				if (g_Dice.percent(20)) introtext += " \"Nice set up.\"\n";
			}
		}
		else if (SexType == SKILL_BEASTIALITY && g_Dice.percent(40))
		{
			switch (RANDROLL)
			{
			case 0:
				introtext += "to the beasts' den.\n";
				break;
			case 1:
				introtext += "to the animal pit.\n";
				break;
			case 2:
				introtext += "to the monster's lair.\n";
				break;
			case 3:
				introtext += "to a small cave in the garden.\n";
				break;
			default:
				introtext += "to the b(E)ast cave.\n";
				break;
			}
		}
		else	// nice room = nicer for customer too, right? (Good customer likes Good)
		{

			introtext += "back to her ";
			if (girl->m_AccLevel < 2)
			{
				introtext += "cramped little hovel.";
				customer->m_Stats[STAT_HAPPINESS] -= 5;
				if (SexType == SKILL_GROUP && g_Dice.percent(50))
				{
					introtext += " It was awkward getting this many customers in her tiny room.";
					customer->m_Stats[STAT_HAPPINESS] -= 5;
				}
			}
			else if (girl->m_AccLevel < 4) introtext += "modest little room.";
			else if (girl->m_AccLevel < 6) introtext += "nice, spacious room.",				customer->m_Stats[STAT_HAPPINESS] += 5;
			else if (girl->m_AccLevel < 8) introtext += "large, elegant room.",				customer->m_Stats[STAT_HAPPINESS] += 10;
			else /*                     */ introtext += "huge, extravagant suite.",			customer->m_Stats[STAT_HAPPINESS] += 20;
			introtext += "\n";
		}
	}
	else if (currentjob == JOB_WHORESTREETS)
		introtext += "to a secluded alley way.\n";
	else if (currentjob == JOB_PEEP)
		introtext = "";		// intro is handled in peep show job
	else
		introtext += "to a secluded spot.\n";

	if (z)
	{
		introtext += "She blankly stares at them as they procede to have their way with her.\n";
	}
	else
	{
		if (currentjob == JOB_PEEP){}
		else if ((girl->has_trait("Nervous") && girl->m_Enjoyment[ACTION_SEX] < 10) ||
			girl->m_Enjoyment[ACTION_SEX] < -20)
		{
			introtext += "She is clearly uncomfortable with the arrangement, and it makes the customer feel uncomfortable.\n";
			customer->m_Stats[STAT_HAPPINESS] -= 5;
		}
		else if (g_Dice.percent(10))
		{
			introtext += "She smiles slightly and makes the effort to arouse the customer.\n";
		}
		else
		{
			introtext += "\n";
		}
	}
	introtext += "\n";
	message += introtext;



	int choice = g_Dice.d100(); //Gondra: initializing a choice variable here because god this is a mess of different ways to handle this
	stringstream sexMessage; //Gondra: using this as a temporary storage so I have less problems when there shouldn't be girlname at the start.

#if defined(SPICE)
	//SIN- Adding will for willfull chars - they can refuse jobs they are bad at.
	//First, a var to store her willfullness. Max (full spirit, no obed) = 50% refusal; Min (all obedience, no spirit) = 0%
	int willfullness = ((100 + (girl->spirit() - girl->obedience())) / 2);
	// next a couple of reasons why refuse
	bool pigHeaded = girl->has_trait("Iron Will");
	bool highStatus = (girl->has_trait("Princess") || girl->has_trait("Queen") || girl->has_trait("Noble"));

	// now the implementation...
	if ((check < 40) && !z && !girl->has_trait("Mute"))  //if she's bad at this sex-type (and not a zombie or mute!), pride kicks in
	{
		//if she's pigheaded, or thinks this is beneath her - she refuses
		if (g_Dice.percent(willfullness) && (pigHeaded || highStatus))
		{
			//Initiate locally used stuff
			int newSexType = 0;			//What will she change it to?
			bool resisting = true;		//Does she even resist? Low-level non-intrusive requests will be honored
			bool forced = false;		//in case the customer overpowers her
			bool askedForHerTopSkill = false;	//in case what she's refusing is already her top skill.
			bool angry = false;			//has he actively pissed her off

			//Why is she refusing - let's put it in a string now
			string refusesbecause;
			const int REASONS = 5;
			if (pigHeaded)
			{
				switch (g_Dice%REASONS)
				{
				case 0:
					refusesbecause = "Whatever anyone says, she's noone's sex-slave.";
					break;
				case 1:
					refusesbecause = "It's her body: she will not let him do this to her.";
					break;
				case 2:
					refusesbecause = "No way. He'll have to kill her first.";
					break;
				case 3:
					refusesbecause = "She still has some sense of self-worth.";
					break;
				case 4:
					refusesbecause = "If she doesn't want to, she doesn't want to. End of.";
					break;
				default:
					refusesbecause = "She is downright pigheaded.(E)"; //(E)rror
					break;
				}
				refusesbecause += "\n";
			}
			else
			{
				switch (g_Dice%REASONS)
				{
				case 0:
					refusesbecause = "This is beneath her.";
					break;
				case 1:
					refusesbecause = "Does he think she's some common street whore?";
					break;
				case 2:
					refusesbecause = "A Lady does NOT do this.";
					break;
				case 3:
					refusesbecause = "She will leave this kind of thing for the common-folk.";
					break;
				case 4:
					refusesbecause = "She was raised for Greatness, not this.";
					break;
				default:
					refusesbecause = "She is too posh for this.(E)"; //(E)rror
					break;
				}
				refusesbecause += "\n";
			}

			//Find top skill - what 'skill' is she most comfortable with? Working from the most extreme down...
			int TopSkillID = 0, TopSkillLev = 0;

			if (TopSkillLev < girl->beastiality())	TopSkillID = SKILL_BEASTIALITY;
			if (TopSkillLev < girl->anal())			TopSkillID = SKILL_ANAL;
			if (TopSkillLev < girl->normalsex())	TopSkillID = SKILL_NORMALSEX;
			if (TopSkillLev < girl->oralsex())		TopSkillID = SKILL_ORALSEX;
			if (TopSkillLev < girl->tittysex())		TopSkillID = SKILL_TITTYSEX;
			if (TopSkillLev < girl->handjob())		TopSkillID = SKILL_HANDJOB;
			if (TopSkillLev < girl->footjob())		TopSkillID = SKILL_FOOTJOB;
			if (TopSkillLev < girl->strip())		TopSkillID = SKILL_STRIP;
			TopSkillLev = girl->get_skill(TopSkillID);

			//is the thing she's being asked for already her top skill?
			if (TopSkillID == SexType) askedForHerTopSkill = true;


			//She's getting willful, time to explain why and what is being refused
	switch (SexType)
	{
			case SKILL_BEASTIALITY:
				if (choice < 33)
				{
					sexMessage << girlName << " will not be fucked by animals for this slithery pervert's entertainment. He only wants to see her degraded.\n"
						<< refusesbecause
						<< "\"No,\" she says. \"Not doing that.\"";
				}
				else if (choice < 66)
				{
					sexMessage << "The customer brings in a unicorn-like creature, with a large horn on it's head and the hugest, weirdest animal-cock " << girlName
						<< " has ever seen. He smirks at her with a shit-eating grin.\n"
						<< refusesbecause
						<< "\"No,\" she says simply.";
				}
				else
				{
					sexMessage << "The customer want to see " << girlName << " fucked by large tentacle creatures.\n"
						<< refusesbecause
						<< "She refuses to let the beast near her.";
				}
				break;

			case SKILL_BDSM:
				if (choice < 33)
				{
					sexMessage << "Without warning, the customer grabs " << girlName << " by the throat and slams her back onto a table.\n"
						<< "\"You're mine, whore,\" he growls, climbing onto her. The last straw is when he spits in her mouth.\n"
						<< refusesbecause
						<< "She resists";
					angry = true;
					if (g_Dice.percent(girl->strength())) sexMessage << " and in her fury, overpowers him.";
					else if (g_Dice.percent(girl->combat())) sexMessage << ", and in her fury fights him off.";
					else
					{
						sexMessage << ", but he is too strong, and with his arm locked on her throat, she can't even scream. "
							<< "As he tortures and rapes her, her blood runs cold with impotent fury.";
						forced = true;
						girl->upd_stat(STAT_HAPPINESS, -5);	//sad
						girl->upd_stat(STAT_SPIRIT, 2);		//angry
						//girl->upd_stat(STAT_SANITY, -4);		//and a bit crazy
					}
				}
				else if (choice < 67)
				{
					sexMessage << "As " << girlName << " bends down easing off her panties, giving the customer a great show, he unexpectedly whips her ass/pussy. "
						<< "Hard. With an actual whip - three-headed, metal-tipped.\nShe yelps in pain, as he howls laughing.\n"
						<< refusesbecause
						<< "She cooly locks the BDSM tools away.";
				}
				else
				{
					sexMessage << "As customer starts to paw through the whips, clamps, and insertions, " << girlName << " decides she doesn't feel like being anyone's bitch right now.\n"
						<< refusesbecause
						<< "\"We'll do something else.\"\nHe looks crest-fallen.";
				}
				break;

			case SKILL_ANAL:
				if (choice < 33)
				{
					sexMessage << "Without warning, the customer slips a finger up " << girlName << "'s asshole. She squeals and spins away.\n"
						<< refusesbecause
						<< "\"THAT,\" she says. \"Is out of bounds.\"";
				}
				else if (choice < 67)
				{
					sexMessage << "The customer pats " << girlName << "'s ass, and grins with a wink: \"After THIS don't reckon you gonna be sittin' down for a while.\"\n"
						<< refusesbecause
						<< "She just shakes her head: \"Not happening.\"\nHis face drops.";
				}
				else
				{
					sexMessage << girlName << " is still removing her clothes when the customer pushes her over a desk, yanks apart her butt-cheeks, and spits on her asshole. "
						<< "She spins round like a demon and shoves him away.\n"
						<< refusesbecause
						<< "\"NOT that.\"";
					angry = true;
				}
				break;

			case SKILL_NORMALSEX:
				if (highStatus && customer->m_Class == 1 && g_Dice.percent(20))
				{
					sexMessage << "The customer, a minor noble, smirked when he saw " << girlName << ".\n\"" << girlName
						<< "! A lady or breeding!\" he says pulling away her clothes. \"Okay, let's breed.\"\n"
						<< refusesbecause
						<< "\"I was intimate with three beggars before,\" she says coldly, shaking her head. \"But that's as low-class as I'm willing to go.\"";
					angry = true;
				}
				else if (choice < 33)
				{
					sexMessage << "Without warning, the customer shoves three fingers up " << girlName << "'s pussy. She squeals and pulls away.\n"
						<< refusesbecause
						<< "\"That is NOT how you ask,\" she tells him angrily.";
				}
				else if (choice < 67)
				{
					sexMessage << "The customer nods approvingly at " << girlName << "'s pussy.\n\"Damn,\" he grins. \"I am gonna tear that apart.\"\n"
						<< refusesbecause
						<< "She shakes her head.\nHis face drops.";
				}
				else
				{
					sexMessage << "As " << girlName << " makes a show of easing off her panties, bending way down to give the customer a view, he pounces slamming his cock hard into her pussy.\n"
						<< refusesbecause
						<< "She thrashes to shake him off";
					angry = true;
					if (g_Dice.percent(girl->strength())) sexMessage << ", and in a frenzy, throws him clean across the room.";
					else if (g_Dice.percent(girl->combat())) sexMessage << ", a hammer-fist between her legs catching him right in the balls. He falls back with a whimper.";
					else
					{
						sexMessage << "But he is too strong, and pinned under all his weight, she can't even wriggle away. "
							<< "As his filthy cock slides into her, she refuses to give him any pleasure.";
						forced = true;
						girl->upd_stat(STAT_HAPPINESS, -5);	//sad
						girl->upd_stat(STAT_SPIRIT, 2);		//angry
						//girl->upd_stat(STAT_SANITY, -2);		//crazy
						check -= 40;							//deliberately underperforms
					}
				}
				break;

			case SKILL_ORALSEX:
				if (highStatus && customer->m_Class == 1 && g_Dice.percent(20))
				{
					sexMessage << "The customer, a minor noble, smirked when he saw " << girlName << ".\n\"" << girlName
						<< "!\" he smiles pulling out his cock. \"Get on your knees. Remember when you were 'too good' for me?!\"\n"
						<< refusesbecause
						<< "\"I still am,\" she says coldly.";
					angry = true;
				}
				else if (choice < 33)
				{
					sexMessage << "Without warning, the customer waves his cock in " << girlName << "'s face. She pats it away, shaking her head.\n"
						<< refusesbecause;
				}
				else if (choice < 67)
				{
					sexMessage << "The customer suddenly puts his hand in " << girlName << "'s mouth, grabbing her by the jaw and pinching her lips.\n";
					if (girl->has_trait("Dick-Sucking Lips")) sexMessage << "\"Those lips!\" he says, shoving her head down. \"Made for dick-sucking.\"\n";
					else sexMessage << "\"On your knees, whore,\" he says. \"And suck this.\"\n"
						<< refusesbecause
						<< "She shakes her head: \"Not if you wanna keep it.\"";
				}
				else
				{
					sexMessage << girlName << " is still locking the door when the customer grabs her head and starts pulling it down toward his cock.\n"
						<< refusesbecause
						<< "She grabs his balls with a sharp squeeze. He freezes.\n\"I wouldn't do that,\" she says.";
				}
				break;

			default:
				resisting = false;	//if they're asking for anything else, it is okay
				break;
			}
			if (resisting)
			{
				sexMessage << "\n";

				//if she's resisted what they asked for, and has not been forced she "chooses"...
				if (!forced)
				{
					//If she's pissed, she won't give more than an angry handjob
					if (angry)
					{
						newSexType = SKILL_HANDJOB;
						check -= 30;
					}
					else
					{
						//she will give her best skill - or if that's what they already asked for...
						if (!askedForHerTopSkill) newSexType = TopSkillID;
						else
						{
							//she will statically downgrade to something (K.I.S.S.)
							if (SexType == SKILL_BEASTIALITY)		newSexType = SKILL_NORMALSEX;
							else if (SexType == SKILL_BDSM)			newSexType = SKILL_NORMALSEX;
							else if (SexType == SKILL_ANAL)			newSexType = SKILL_NORMALSEX;
							else if (SexType == SKILL_NORMALSEX)	newSexType = SKILL_HANDJOB;
							else if (SexType == SKILL_ORALSEX)		newSexType = SKILL_HANDJOB;
						}
					}

					if (SexType != newSexType) //if these aren't the same, report the change
					{
						//report the old
						switch (SexType)
						{
						case SKILL_BEASTIALITY:
							sexMessage << "\"I won't fuck some filthy animal for you,\" " << girlName << " says";
							break;
						case SKILL_BDSM:
							sexMessage << "\"You do not get to abuse me. You do not get to hurt me.\" " << girlName << " says";
							break;
						case SKILL_ANAL:
							sexMessage << "\"There is no way That is going in my ass ";
							Day0Night1 ? sexMessage << "today,\" " : sexMessage << "tonight,\" ";
							sexMessage << girlName << " says";
							break;
						case SKILL_NORMALSEX:
							sexMessage << "\"I will not have you in my pussy,\" " << girlName << " says";
							break;
						case SKILL_ORALSEX:
							sexMessage << "\"Your cock is not going in my mouth ";
							Day0Night1 ? sexMessage << "today,\" " : sexMessage << "tonight,\" ";
							sexMessage << girlName << " says";
							break;
						default: // no way, if it doesn't match these types, it should not change.
							sexMessage << "(E)";
							break;
						}
						//report the new
						switch (newSexType)
						{
						case SKILL_BEASTIALITY:
							sexMessage << ". \"I'd sooner fuck a beast than that... Though I guess you can watch if you want.\"";
							break;
						case SKILL_ANAL:
							sexMessage << ". \"But you can have my ass, if you want.\"";
							break;
						case SKILL_NORMALSEX:
							sexMessage << ". \"But if you're man enough, I guess you can fuck me.\"";
							break;
						case SKILL_ORALSEX:
							sexMessage << ", adding with a smile. \"But if you want your dick sucked";
							if (highStatus)
							{
								if (girl->has_trait("Queen")) sexMessage << " by a Queen";
								else if (girl->has_trait("Princess")) sexMessage << " by a Princess";
								else sexMessage << " by Nobility";
							}
							else sexMessage << " by a girl who doesn't stop";
							sexMessage << "... That can still happen.\"";
							break;
						case SKILL_TITTYSEX:
							sexMessage << ". She slips off her gown exposing her chest, and sits back on the bed, rubbing her nipples and squeezing ";
							if (girl->has_trait("Flat Chest") || girl->has_trait("Petite Breasts") || girl->has_trait("Small Boobs")) sexMessage << "her small, under-developed breasts";
							else if (girl->has_trait("Busty Boobs") || girl->has_trait("Big Boobs") || girl->has_trait("Giant Juggs")) sexMessage << "her large, round breasts";
							else if (girl->has_trait("Massive Melons") || girl->has_trait("Abnormally Large Boobs") || girl->has_trait("Titanic Tits")) sexMessage << "her enormous breasts";
							else sexMessage << "her breasts";
							sexMessage << ". \"But we can oil these up and have some fun.\"";
							break;
						case SKILL_HANDJOB:
							if (angry) sexMessage << ". Without a word she grabs his cock and starts giving an angry handjob.";
							else sexMessage << ", grabbing his penis and rubbing it. \"But this is okay.\"";
							break;
						case SKILL_FOOTJOB:
							sexMessage << ". \"But have you ever tried a girl who's good with her feet?\"";
							break;
						case SKILL_STRIP:
							sexMessage << ". \"But if you promise to good from here, I'll give you a lapdance.\"";
							break;
						default: // no way, if it doesn't match these types, it should not change.
							sexMessage << "(E)";
							break;
						}
						//finally update variable for next bit
						SexType = newSexType;
						sexMessage << "\n";
					}

				}
				//Forced to do what they want, so no changes
				//let's reset choice to avoid outcomes above being always paired with ones below
				choice = g_Dice.d100();
			}
			//She didn't have a problem with what they asked for
		}
		//else she doesn't refuse - so move along, nothing to see here
	}

	//SIN - also adding a few vars for branches of dialogue below
	bool SheAintPretty = (girl->beauty() < 45);
	bool GirlGotNoClass = (girl->refinement() < 35);
	int HateLove = girl->pclove() - girl->pchate();

#endif
	switch (SexType)
	{
	case SKILL_ANAL:
	{
#if 1
		if (z)
		{
			sexMessage << girlName << " moaned lightly as her customer pounded her dead ass.";
			break;
		}
		/* Gondra: Commented out for now
		//TODO Gondra: replace this with a descripton string in front of ALL Sexmessages?
		if (girl->has_trait("Great Arse") || girl->has_trait("Deluxe Derriere")) sexMessage << "'s behind is a thing of beauty. She ";
		else if (girl->has_trait("Phat Booty") || girl->has_trait("Plump Tush")) sexMessage << "'s big round booty was up in the air. She "; //Gondra: Wide Bottom is mising here?
		else if (girl->has_trait("Tight Butt")) sexMessage << " has a tight, round firm little butt. She ";
		else if (girl->has_trait("Flat Ass")) sexMessage << "'s ass is flat as a board. She ";
		*/

		//Gondra: reworking this part with choice variable
		if (check < 20)		//Gondra: if the girl is unskilled show one of these messages
		{
			if (g_Dice.percent(33) && (girl->has_trait("Phat Booty") || girl->has_trait("Plump Tush") || girl->has_trait("Wide Bottom") || girl->has_trait("Great Arse")))	//Gondra: not sure if it is the best idea to always show Trait related messages
			{
				sexMessage << girlName << " was clearly uncomfortable as the customer pushed his cock into her jiggling booty.";
			}
			else if (g_Dice.percent(60) && girl->has_trait("Prehensile Tail") || girl->has_trait("Playful Tail"))
			{
				sexMessage << "Using her tail as a handhold the customer made the fuck a lot more traumatic for " << girlName << " than it normally would have been.";
			}
			else if (g_Dice.percent(33) && girl->has_trait("Tight Butt"))
			{
				sexMessage << "It took the customer quite a bit of effort to force himself into " << girlName << "'s tight ass, ignoring her cries when he was finally inside her, moving harshly until he finished.";
				//Gondra: add happiness and health reduction?
			}
#if defined(SPICE)	//SIN - Replace/supplement...
			else if (g_Dice.percent(33) && SheAintPretty)
			{
				sexMessage << "As he fucked her ass, the customer did it from behind, shoving " << girlName
					<< "'s face into the pillow so he could imagine it was someone prettier. Unfortunately she was awful at this.";
			}

			else if (choice < 25)
			{
				sexMessage << "When she saw the customer's cock, " << girlName << " begged him to use her pussy instead. "
					"But it was his dollar and he insisted on trying to squeeze all of that into her asshole, despite her yelps and squeals.";
			}
#endif
			else if (choice < 50)	//Gondra: if we have no Trait related message use vanilla ones. TODO Gondra: Replace/supplement these Anal Vanilla messages.
			{
				sexMessage << girlName << " found it difficult to get it in but painfully allowed the customer to fuck her in her tight ass.";
			}
#if defined(SPICE)
			//SIN - Replace/supplement...
			else if (choice < 75)
			{
				sexMessage << girlName << " didn't realize he was aiming for *that* hole until it was too late. She couldn't really enjoy it.";
			}
#endif
			else
			{
				sexMessage << girlName << " bit the pillow to muffle her cries as the customer managed to squeeze his cock into her ass.";
			}
		}
		else if (check < 40) //Gondra:  if the girl is slightly skilled
		{
			if (g_Dice.percent(75) && (girl->has_trait("Phat Booty") || girl->has_trait("Plump Tush") || girl->has_trait("Wide Bottom") || girl->has_trait("Great Arse"))) //Gondra: Trait messages
			{
				sexMessage << girlName << " felt a bit uncomfortable as the customer's erect cock slipped between her ass-cheeks, but the customer hardly noticed as her plentiful flesh wrapped around him.";
			}
#if defined(SPICE)
			//SIN - supplement...
			else if (g_Dice.percent(75) && girl->check_virginity())
			{
				sexMessage << girlName << "'s virginity was spared, as he used her ass-hole. Considering she's a 'virgin' she seems to have done THIS before.";
			}
			else if (g_Dice.percent(33) && SheAintPretty)
			{
				sexMessage << "He fucked " << girlName << "'s ass from behind, so that he wouldn't have to look at her ugly face. "
					<< "She was okay at this.";
			}
			else if (choice < 25)
			{
				sexMessage << "When " << girlName << " saw the customer's cock, her eyes widened. "
					"Getting all of that in her ass would take some work.";
			}
#endif
			else if (choice < 50)	//Gondra: Vanilla Messages
			{
				sexMessage << girlName << " had to relax somewhat but had the customer fucking her in her ass.";
			}
#if defined(SPICE)
			else if (choice < 75)
			{
				sexMessage << girlName << " enjoyed it, even though she's not that good at this yet. He did most of the work.";
			}
#endif
			else
			{
				sexMessage << girlName << " struggled to relax, but was okay with the customer gently screwing her ass.";
			}
		}
		else if (check < 60) //Gondra: the girl is reasonably skilled
		{
			if (g_Dice.percent(60) && (girl->has_trait("Phat Booty") || girl->has_trait("Plump Tush") || girl->has_trait("Wide Bottom") || girl->has_trait("Great Arse"))) //Gondra: Trait messages
			{
				sexMessage << girlName << "'s voluminous ass jiggles quite a bit as the customer goes at it.";
			}
			else if (g_Dice.percent(60) && girl->has_trait("Tight Butt"))
			{
				sexMessage << girlName << "'s very tight butt forced him to take it slow but the vice like grip seemed to do the trick either way as he came quickly.";
			}
#if defined(SPICE)
			else if (g_Dice.percent(20) && girl->has_trait("Queen"))
			{
				sexMessage << "As one of the rebels that once tried to overthrow " << girlName << ", the customer relished the chance to fuck her in the ass for some paltry gold coins. "
					<< "He left boasting to everyone about how good it had been to buttfuck a notorious Queen! (Leading to lots of sniggering and rumours that would follow him for years)\n";
				customer->m_Stats[STAT_HAPPINESS] += 10;
				girl->upd_stat(STAT_FAME, 5);
			}
			else if (g_Dice.percent(33) && SheAintPretty)
			{
				sexMessage << "He fucked " << girlName << "'s ass from behind, so that he wouldn't have to look at her face. "
					<< "She was pretty good at this.";
			}
#endif
			else if (choice < 25)	//Gondra: Vanilla Messages
			{
				sexMessage << "The customer lay on the bed groaning as " << girlName << " bounced up and down, working his cock with her ass.";
			}

			else if (choice < 50)	//Gondra: Vanilla Messages
			{
				sexMessage << girlName << " found it easier going with the customer fucking her in her ass.";
			}
			else
			{
				sexMessage << girlName << " was comfortable with the customer fucking her in her ass.";
			}
		}
		else if (check < 80) //Gondra: the girl is VERY skilled
		{
#if defined(SPICE)
			//SIN: Unnecessary testing all of these traits twice if she has none - also the "choice" was only ANDED with the "Great Arse" - so any other trait would fire whether choice passed or not
			//Rewritten to cut overhead and fix logic. Also using 'choice' more to avoid locking into ALWAYS one message for a girl with a given trait.

			if (g_Dice.percent(60) && (girl->has_trait("Phat Booty") || girl->has_trait("Plump Tush") || girl->has_trait("Wide Bottom") || girl->has_trait("Great Arse")))
			{
				/* */if (choice < 33)	sexMessage << girlName << " enjoyed showing of that she can hide the customers whole cock between her cheeks, before she lets him slip into her ass proper.";
				else if (choice < 66)	sexMessage << "Encouraged by " << girlName << " the customer plowed her ass hard, both enjoying the sound her jiggling backside made each time he drove his cock home.";
				else /*			   */	sexMessage << "The customer slammed his cock again and again into " << girlName << "'s asshole, loving the slap and the jiggle of flesh each time he drove it home.";
			}
			else if (g_Dice.percent(60) && girl->has_trait("Tight Butt"))
			{
				sexMessage << "Just as the customer wondered if he would fit into the tight ass in front of him, " << girlName << "spread her ass for him, inviting him to push it deep into her.";
			}
			else if (g_Dice.percent(20) && girl->has_trait("Queen"))
			{
				sexMessage << "As one of the rebels that once tried to overthrow " << girlName << ", the customer relished the chance to fuck her in the ass for some gold coins. "
					<< "He left telling to everyone about how amazing it felt to buttfuck a Queen! (Leading to lots of sniggering, and rumours that would follow him for years)\n";
				customer->m_Stats[STAT_HAPPINESS] += 10;
				girl->upd_stat(STAT_FAME, 5);
			}
			else if (g_Dice.percent(33) && SheAintPretty)
			{
				sexMessage << "At first the customer seemed uphappy with " << girlName << "'s looks, but as soon as she had her sphincter locked around his cock "
					<< "and started grinding her ass onto him, any complaints quickly vanished.";
			}
			else if (choice < 25)	//Gondra: Vanilla Messages
			{
				sexMessage << "The customer lay on the bed gasping with pleasure as " << girlName << " bounced up and down, riding his pole for all she was worth.";
			}
#else
			if (girl->has_trait("Phat Booty") || girl->has_trait("Plump Tush") || girl->has_trait("Wide Bottom") || girl->has_trait("Great Arse") && choice < 50) //Gondra: EXAMPLE Since I have two texts for the same Trait set I am reusing the choice variable here
			{
				sexMessage << girlName << " enjoyed showing of that she can hide the customers whole cock between her cheeks, before she lets him slip into her ass proper.";
			}
			else if (girl->has_trait("Phat Booty") || girl->has_trait("Plump Tush") || girl->has_trait("Wide Bottom") || girl->has_trait("Great Arse"))
			{
				sexMessage << "Encouraged by " << girlName << " the customer plowed her ass hard, both enjoying the sound her jiggling backside made each time he drove his cock home.";
			}
			else if (girl->has_trait("Tight Butt"))
			{
				sexMessage << "Just as the customer wondered if he would fit into the tight ass in front of him, " << girlName << "spread her ass for him, inviting him to push it deep into her.";
			}
#endif
			else if (choice < 50)	//Gondra: Vanilla Messages
			{
				sexMessage << "The customer slid it right into her ass and " << girlName << " loved every minute of it.";
			}
#if defined(SPICE)
			else if (choice < 75)	//Gondra: Vanilla Messages
			{
				sexMessage << girlName << " sat on the bed and placed her legs on his shoulders, as the customer plunged it deep in her ass.";
			}
#endif
			else
			{
				sexMessage << girlName << " had the customer's cock go in easy. She found having him fuck her ass a very pleasurable experience.";
			}
		}
		//Gondra: I thought 'check' values larger than 80 were added in that randomized fashion but there isn't one for ANAL?
		else //Gondra: the girl is EXTREMELY skilled
		{
			if (g_Dice.percent(60) && (girl->has_trait("Phat Booty") || girl->has_trait("Plump Tush") || girl->has_trait("Wide Bottom") || girl->has_trait("Great Arse"))) //Gondra: Trait messages
			{
				sexMessage << "The customer played around with the big round ass " << girlName << " held up for him, which already made her moan loudly. And then made her cum for the first of many times, just by pushing his throbbing length into her willing anus.";
			}
			else if (g_Dice.percent(50) && girl->has_trait("Tight Butt"))
			{
				sexMessage << "The customer looked surprised when " << girlName << " slipped her ass onto his cock, the tight embrace of her backside milking him several times with exquisite motions. Not a single drop of cum leaked from her even after they had finished and she accompanied him out.";
			}
#if defined(SPICE)	//SIN
			else if (g_Dice.percent(35) && (girl->has_trait("Flexible") || girl->has_trait("Agile")))
			{
				sexMessage << girlName << " did the splits on the edge of the bed, her spread pussy and ass facing the customer. After a moment's indecision, he fucked her deep in the ass until they both came.";
			}
			else if (g_Dice.percent(40) && girl->has_trait("Cum Addict"))
			{
				sexMessage << girlName << " skillfully milked the customer's cock with her ass";
				/* */if (choice < 33) sexMessage << ", quickly switching to her mouth when she sensed him ready to give up his precious cum.";
				else if (choice < 67) sexMessage << " until, without warning, he wasted his precious cum shooting it all up her ass. She managed to claw some back out, "
					<< "licking it greedily off her fingers, but most was lost.\nThe customer was awed, thinking this was all part of the show.";
				else sexMessage << " until, without warning, he threw away his precious load in her ass.\nAnnoyed, she climbed off him and squatted on the floor, squeezing, "
					<< "until - with a little toot - it squirted and oozed back out. Relieved, she nodded politely to the customer, and started lapping it off the floor.";
			}
			else if (choice < 25)	//Gondra: Vanilla Messages
			{
				sexMessage << "The customer writhed on the bed breathless with pleasure as " << girlName << " rode his pole, her anus milking orgasm after crippling orgasm out of him.";
			}
#endif
			else if (choice < 50)	//Gondra: Vanilla Messages
			{
				sexMessage << "The customer started slow but quickly began to pound hard into " << girlName << "'s ass making her moan like crazy.";
			}
			else if (choice < 75)	//Gondra: Vanilla Messages
			{
				sexMessage << girlName << " sat on the bed and put her legs around his neck, as the customer plunged his cock up her ass.";
			}
			else
			{
				sexMessage << girlName << " came hard as the customer fucked her ass.";
			}
		}
		message += sexMessage.str(); //Gondra: add our sexMessage to our message string
#endif
	}break; //End of SKILL_ANAL Case

	case SKILL_BDSM:
	{
#if 1
		if (z)
		{
			sexMessage << girlName << " resisted a little as her customer tied her up and played with her.";
			break;
		}

		//Gondra: reworking this part with choice variable
		if (check < 20)		//Gondra: if the girl is unskilled show one of these messages
		{
			if (girl->has_trait("Masochist"))	//Gondra: Trait messages
			{	//Gondra: Would this one even show up? I know Masochist gives +BDSM but not how much
				// `J` while masochist gives +50 bdsm (probably a little too high) other things could reduce it below 20
				sexMessage << "While " << girlName << " was visibly uncomfortable, she was eager to learn more about this 'interesting' act after the fact.";
			}
#if defined(SPICE)
			else if (g_Dice.percent(66) && girl->is_pregnant())
			{
				int term = (girl->m_States&(1 << STATUS_INSEMINATED) ? cfg.pregnancy.weeks_monster_p() : cfg.pregnancy.weeks_pregnant());
				if (girl->m_WeeksPreg < (term / 4)) // if she's not showing
				{
					sexMessage << girlName << " was awkward with the customer, refusing to do anything too extreme for fear of harming her unborn child.";
					customer->m_Stats[STAT_HAPPINESS] -= 10;
				}
				else if (g_Dice.percent(10))
				{
					sexMessage << girlName << " was bound and roughly used by some sicko who seemed to get off on her pregnancy.";
					customer->m_Stats[STAT_HAPPINESS] += 20;
					girl->upd_Enjoyment(ACTION_SEX, -5);
					girl->upd_stat(STAT_HAPPINESS, -5);
				}
				else
				{
					if (girl->m_WeeksPreg < ((3 * term) / 4)) // if she's showing
					{
						sexMessage << "Both " << girlName << " and her customer were reluctant to do any real bondage while she is bearing child.";
						customer->m_Stats[STAT_HAPPINESS] -= 20;
					}
					else		//if she's about to drop
					{
						sexMessage << "\"Can you do this in your condition?\"\nWith " << girlName << "'s very obvious pregnancy, the customer was reluctant to do anything at all for fear harming her unborn child.";
						customer->m_Stats[STAT_HAPPINESS] -= 30;
					}
				}
			}
			else if (g_Dice.percent(66) && girl->has_trait("Mind Fucked"))
			{
				sexMessage << "Naked, hollow-eyed and open-mouthed " << girlName << " smiled at the customer drooling ";
				const int FREAK = 5;
				switch (g_Dice%FREAK)
				{
				case 0:
					sexMessage << " and singing a Nursery Rhyme as she brutally slapped her clit.";
					break;
				case 1:
					sexMessage << " and crying as she fisted herself.";
					break;
				case 2:
					sexMessage << " into her hands, and then wiping the slobber over her face and giggling like a child.";
					break;
				case 3:
					sexMessage << " and purring as she played with the candle, its wax and its exquisite flame.";
					break;
				case 4:
					sexMessage << " and laughing as she played with the high-voltage toys and insertions.";
					break;
				default:
					sexMessage << " and doing something so awful it cannot be described (E).";
					break;
				}
				sexMessage << "\nShe lives for bondage. Unfortunately she had the customer very freaked out and unable to get aroused.\n";
			}
			else if (g_Dice.percent(33) && girl->has_trait("Mute"))
			{
				sexMessage << girlName << " was tied down for a BDSM session.\nNot realising " << girlName << " was mute, the customer took her silence as a challenge, and grew increasingly "
					<< "brutal in his efforts to get a scream out of 'this insolent bitch.'\nHe was frustrated, and she won't sit comfortably for a few weeks.";
				girl->upd_stat(STAT_HEALTH, -10);
			}
			else if (g_Dice.percent(25) && girl->has_trait("Shy"))
			{
				sexMessage << "Annoyed by " << girlName << "'s boring shyness, the customer finally jacked off in her face, zipped up, angrily flung open the door and left, "
					<< "leaving waiting customers with a clear view of her butt-naked, chained spread-eagled to a rack, as cum ran down her face. A number of them wolf-whistled.";
				girl->upd_stat(STAT_DIGNITY, -5);
				girl->upd_stat(STAT_FAME, 5);
			}
			//else if (g_Dice.percent(25) && GetStat(girl, STAT_SANITY) < 25)
			//{
			//	const int HEDIDWHAT = 5;
			//	const int SHEDIDWHAT = 5;

			//	sexMessage << "The customer was unsettled by this BDSM session. As he ";

			//	switch (g_Dice%HEDIDWHAT)
			//	{
			//	case 0:
			//		sexMessage << "thwacked her nipples red with a stick";
			//		break;
			//	case 1:
			//		sexMessage << "deliberately came in her eye";
			//		break;
			//	case 2:
			//		sexMessage << "shoved his dick in her ass";
			//		break;
			//	case 3:
			//		sexMessage << "dripped hot candle wax on her asshole";
			//		break;
			//	case 4:
			//		sexMessage << "cruelly fisted her";
			//		break;
			//	default:
			//		sexMessage << "did som(E)thing nasty";
			//		break;
			//	}

			//	sexMessage << " " << girlName << " ";

			//	switch (g_Dice%SHEDIDWHAT)
			//	{
			//	case 0:
			//		sexMessage << "started suddenly laughing";
			//		break;
			//	case 1:
			//		sexMessage << "softly began to sing";
			//		break;
			//	case 2:
			//		sexMessage << "stared up at him with an unnerving empty smile";
			//		break;
			//	case 3:
			//		sexMessage << "grinned and silently wept";
			//		break;
			//	case 4:
			//		sexMessage << "started speaking in tongues";
			//		break;
			//	default:
			//		sexMessage << "did som(E)thing weird";
			//		break;
			//	}

			//	sexMessage << ", ";

			//	if (girl->morality() > 33) //fundie
			//	{
			//		sexMessage << "urging him to cleanse her and saying something about fate, the Gods, and divine retribution.";
			//	}
			//	else if (girl->morality() > -33) //crazy
			//	{
			//		sexMessage << "calling him 'bunny' and urging him to teach her the secret ways.";
			//	}
			//	else	//evil
			//	{
			//		sexMessage << "urging him to make her stronger, and muttering about the strong consuming the unworthy.";
			//	}
			//	sexMessage << " It was just weird.\n";
			//	girl->upd_stat(STAT_FAME, 5);
			//}
			else if (choice < 25)
			{
				sexMessage << girlName << " was horrified as the customer clamped things on her, shoved things in her, and whipped, hurt and deliberately degraded her for his own sexual gratification.";
			}
#endif
			else if (choice < 50)	//Gondra: Vanilla Messages TODO Gondra: Replace/supplement these Vanilla messages.
			{
				sexMessage << girlName << " was frightened by being tied up and having pain inflicted on her.";
			}
#if defined(SPICE)
			else if (choice < 75)
			{
				sexMessage << girlName << " wept in pain and humilation as the customer poured hot candle-wax on her sensitive parts.";
			}
#endif
			else
			{
				sexMessage << "Being unfamiliar with the tools of this part of the trade, " << girlName << " had a questioning look on her face that made it hard for the customer to enjoy themselves.";
			}
#if defined(SPICE)
			//SIN
			// NOT reusing "choice" here because want this to be independent of above message
			if (g_Dice.percent(20)) //customer goes wild - unlikely and only affects lower level girls (more skilled girls can take it)
			{
				sexMessage << "The customer suddenly turned sadist and started really hurting " << girlName << ". ";
				//default harm stored in local vars to avoid repeat update calls
				int upset = 4, damage = 4, PCLove = -1, PCFear = 1;

				bool guardingGang = (g_Gangs.gangs_watching_girls().size() > 0);
				bool guardingGirl = (g_Brothels.GetNumGirlsOnJob(0, JOB_SECURITY, Day0Night1) > 0);

				if (guardingGirl) //there's a girl watching the place
				{
					sGirl * guard = (g_Brothels.GetRandomGirlOnJob(0, JOB_SECURITY, Day0Night1));
					string guardName = guard->m_Realname;

					if (The_Player->disposition() > 30)						//Player is Actively Good.
					{
						sexMessage << "Knowing you wouldn't want things to go too far, your guard, " << guardName << ", intervened, instructing the customer to take it easy on "
							<< girlName << " or get banned from your establishment.\n";
						PCFear -= 3;	//you protect her
						PCLove += 3;	//ditto
						damage -= 2;	//reduced damage
						upset -= 3;		//and upset
						customer->m_Stats[STAT_HAPPINESS] -= 5;
					}
					else if (guard->morality() > 50)	//If guard is good
					{
						sexMessage << "Your guard, " << guardName << ", intervened, angrily demanding the customer 'back off' when things were clearly getting too extreme for " << girlName << ".\n";
						PCFear -= 1;
						damage -= 2;
						upset -= 3;
						customer->m_Stats[STAT_HAPPINESS] -= 8;
					}
					else if (g_Dice.percent(50) && (girl->has_trait("Psychic") || girl->has_trait("Strong Magic"))) //If girl can compell
					{
						sexMessage << "Your guard, " << guardName << ", felt oddly compelled to help " << girlName << ", demanding the customer take it easier on " << girlName << ".\n";
						damage -= 2;
						upset -= 2;
						customer->m_Stats[STAT_HAPPINESS] -= 5;
					}
					else
					{
						sexMessage << "Your guard, " << guardName << ", heard " << girlName << "'s screams, but didn't care to do anything. It's bought and paid for.\n";
						//girl->upd_stat(STAT_SANITY, -4);
					}
				}
				else if (guardingGang)
				{
					if (The_Player->disposition() > 30)						//Player is Actively Good.
					{
						sexMessage << "Knowing you wouldn't want things to go too far, your defending gang intervened, ordering the customer to take it easy on "
							<< girlName << " or get banned from your establishment.\n";
						PCFear -= 3;	//you protect her
						PCLove += 3;	//ditto
						damage -= 2;	//reduced damage
						upset -= 3;		//and upset
						customer->m_Stats[STAT_HAPPINESS] -= 5;
					}
					else if (g_Dice.percent(50) && (girl->has_trait("Psychic") || girl->has_trait("Strong Magic"))) //If girl can compell
					{
						sexMessage << "Your defending gang felt oddly compelled to help " << girlName << ", demanding the customer take it easier on her.\n";
						damage -= 2;
						upset -= 2;
						customer->m_Stats[STAT_HAPPINESS] -= 5;
					}
					else
					{
						sexMessage << "Your guards heard " << girlName << "'s screams, but didn't care to do anything. It's bought and paid for.\n";
						//girl->upd_stat(STAT_SANITY, -4);
					}
				}
				else if (girl->has_trait("Psychic") && (girl->mana() > 60))	//If girl can sway customer
				{
					sexMessage << "Suddenly, he calmed down and stopped. " << girlName << "'s eyes glowed as the customer tenderly untied her bindings.\n";
					damage -= 2;
					upset -= 2;
					customer->m_Stats[STAT_HAPPINESS] += 5; //magical bliss
					girl->upd_stat(STAT_MANA, -50);	//BIG mana hit
				}
				else
				{
					sexMessage << "No one was around to stop him, so she ended up taking some damage.";
					//girl->upd_stat(STAT_SANITY, -4);
				}
				girl->upd_stat(STAT_PCFEAR, PCFear);
				girl->upd_stat(STAT_PCLOVE, PCLove);
				girl->upd_stat(STAT_HEALTH, -damage);
				girl->upd_stat(STAT_HAPPINESS, -upset);
		}
#endif
		}
		else if (check < 40) //Gondra:  if the girl is slightly skilled
		{
			if (girl->has_trait("Masochist")) //Gondra: Trait messages
			{
				sexMessage << girlName << " eagerly let herself be bound by the customer, visibly enjoying herself as the customer began inflicting pain on her. It wasn't that great for him though.";
			}
#if defined(SPICE)
			else if (g_Dice.percent(60) && girl->is_pregnant())
			{
				int term = (girl->m_States&(1 << STATUS_INSEMINATED) ? cfg.pregnancy.weeks_monster_p() : cfg.pregnancy.weeks_pregnant());
				if (girl->m_WeeksPreg < (term / 4)) // if she's not showing
				{
					sexMessage << girlName << " was a little awkward with the customer, refusing to do things that may harm her unborn child.";
					customer->m_Stats[STAT_HAPPINESS] -= 5;
				}
				else if (g_Dice.percent(10))
				{
					sexMessage << girlName << " was bound and roughly used by some sicko who seemed to get off on her pregnancy.";
					customer->m_Stats[STAT_HAPPINESS] += 20;
					girl->upd_Enjoyment(ACTION_SEX, -2);
					girl->upd_stat(STAT_HAPPINESS, -2);
				}
				else
				{
					if (girl->m_WeeksPreg < ((3 * term) / 4)) // if she's showing
					{
						sexMessage << "The customer was reluctant to do bondage while " << girlName << " is bearing child, but she told him some things would be okay.";
						customer->m_Stats[STAT_HAPPINESS] -= 5;
					}
					else		//if she's about to drop
					{
						sexMessage << "\"Can you do this in your condition?\"\nWith her advanced pregnancy, the customer and " << girlName << " proceeded carefully, so as not to harm her unborn child.";
						customer->m_Stats[STAT_HAPPINESS] -= 10;
					}
				}
			}
			else if (g_Dice.percent(33) && (girl->has_trait("Plump") || girl->has_trait("Fat")))
			{
				sexMessage << "The customer tied " << girlName << "to a rack, where he used a paddle to spank her fat ass, her wobbly thighs and her flabby breasts, sending fat waves rippling all over her body. "
					<< "The pain was too much for her, and her wimpering totally killed his mood.";
			}
			else if (choice < 25)
			{
				sexMessage << girlName << " was upset as the customer clamped things on her nipples and shoved things in her mouth, deliberately degrading her for his amusement.";
			}
#endif
			else if (choice < 50) //Gondra: Vanilla Messages
			{
				sexMessage << girlName << " was not enjoying being bound and hurt, but endured it.";
			}
#if defined(SPICE)
			else if (choice < 75)
			{
				sexMessage << girlName << " squealed and struggled as the customer dripped sizzling candle-wax on sensitive areas.";
			}
#endif
			else
			{
				sexMessage << girlName << " was still a bit scared as the customer began applying the bondage gear on her body, but didn't really show it.";
			}
#if defined(SPICE)
			//SIN
			// NOT reusing "choice" here because want this to be independent of above message
			if (g_Dice.percent(20)) //customer goes wild - unlikely and only affects lower level girls (more skilled girls can take it)
			{
				sexMessage << "The customer suddenly turned sadist and started really hurting " << girlName << ". ";
				//default harm stored in local vars to avoid repeat update calls
				int upset = 4, damage = 4, PCLove = -1, PCFear = 1;

				bool guardingGang = (g_Gangs.gangs_watching_girls().size() > 0);
				bool guardingGirl = (g_Brothels.GetNumGirlsOnJob(0, JOB_SECURITY, Day0Night1) > 0);

				if (guardingGirl) //there's a girl watching the place
				{
					sGirl * guard = (g_Brothels.GetRandomGirlOnJob(0, JOB_SECURITY, Day0Night1));
					string guardName = guard->m_Realname;

					if (The_Player->disposition() > 30)						//Player is Actively Good.
					{
						sexMessage << "Knowing you wouldn't want things to go too far, your guard, " << guardName << ", intervened, instructing the customer to take it easy on "
							<< girlName << " or get banned from your establishment.\n";
						PCFear -= 3;	//you protect her
						PCLove += 3;	//ditto
						damage -= 2;	//reduced damage
						upset -= 3;		//and upset
						customer->m_Stats[STAT_HAPPINESS] -= 5;
					}
					else if (guard->morality() > 50)	//If guard is good
					{
						sexMessage << "Your guard, " << guardName << ", intervened, angrily demanding the customer 'back off' when things were clearly getting too extreme for " << girlName << ".\n";
						PCFear -= 1;
						damage -= 2;
						upset -= 3;
						customer->m_Stats[STAT_HAPPINESS] -= 8;
		}
					else if (g_Dice.percent(50) && (girl->has_trait("Psychic") || girl->has_trait("Strong Magic"))) //If girl can compell
					{
						sexMessage << "Your guard, " << guardName << ", felt oddly compelled to help " << girlName << ", demanding the customer take it easier on " << girlName << ".\n";
						damage -= 2;
						upset -= 2;
						customer->m_Stats[STAT_HAPPINESS] -= 5;
					}
					else
					{
						sexMessage << "Your guard, " << guardName << ", heard " << girlName << "'s screams, but didn't care to do anything. It's bought and paid for.\n";
						//girl->upd_stat(STAT_SANITY, -4);
					}
				}
				else if (guardingGang)
				{
					if (The_Player->disposition() > 30)						//Player is Actively Good.
					{
						sexMessage << "Knowing you wouldn't want things to go too far, your defending gang intervened, ordering the customer to take it easy on "
							<< girlName << " or get banned from your establishment.\n";
						PCFear -= 3;	//you protect her
						PCLove += 3;	//ditto
						damage -= 2;	//reduced damage
						upset -= 3;		//and upset
						customer->m_Stats[STAT_HAPPINESS] -= 5;
					}
					else if (g_Dice.percent(50) && (girl->has_trait("Psychic") || girl->has_trait("Strong Magic"))) //If girl can compell
					{
						sexMessage << "Your defending gang felt oddly compelled to help " << girlName << ", demanding the customer take it easier on her.\n";
						damage -= 2;
						upset -= 2;
						customer->m_Stats[STAT_HAPPINESS] -= 5;
					}
					else
					{
						sexMessage << "Your guards heard " << girlName << "'s screams, but didn't care to do anything. It's bought and paid for.\n";
						//girl->upd_stat(STAT_SANITY, -4);
					}
				}
				else if (girl->has_trait("Psychic") && (girl->mana() > 60))	//If girl can sway customer
				{
					sexMessage << "Suddenly, he calmed down and stopped. " << girlName << "'s eyes glowed as the customer tenderly untied her bindings.\n";
					damage -= 2;
					upset -= 2;
					customer->m_Stats[STAT_HAPPINESS] += 5; //magical bliss
					girl->upd_stat(STAT_MANA, -50);	//BIG mana hit
				}
				else
				{
					sexMessage << "No one was around to stop him, so she ended up taking some damage.";
					//girl->upd_stat(STAT_SANITY, -4);
				}
				girl->upd_stat(STAT_PCFEAR, PCFear);
				girl->upd_stat(STAT_PCLOVE, PCLove);
				girl->upd_stat(STAT_HEALTH, -damage);
				girl->upd_stat(STAT_HAPPINESS, -upset);
			}
#endif
		}
		else if (check < 60) //Gondra: the girl is reasonably skilled
		{
			if (g_Dice.percent(40) && girl->has_trait("Masochist")) //Gondra: Trait messages
			{
				sexMessage << "Once bound, " << girlName << " was already beginning to show visible arousal, that only intensified as the customer started to use the various tools available on her.";
			}
#if defined(SPICE)
			else if (g_Dice.percent(50) && girl->is_pregnant())
			{
				int term = (girl->m_States&(1 << STATUS_INSEMINATED) ? cfg.pregnancy.weeks_monster_p() : cfg.pregnancy.weeks_pregnant());
				if (girl->m_WeeksPreg < (term / 4)) // if she's not showing
				{
					sexMessage << girlName << " provided an exciting bondage session, while subtly steering the customer away from things that might be harmful to her unborn child.";
				}
				else if (g_Dice.percent(10))
				{
					sexMessage << girlName << " was bound and roughly used by some sicko who got off on her pregnancy. She enjoyed it.";
					customer->m_Stats[STAT_HAPPINESS] += 20;
				}
				else
				{
					if (girl->m_WeeksPreg < ((3 * term) / 4)) // if she's showing
					{
						sexMessage << "Seeing her condition, the customer was reluctant to do bondage. " << girlName << " reassured him and showed him there was lots he could still do to her.";
					}
					else		//if she's about to drop
					{
						sexMessage << "Seeing her advanced pregnancy, the customer figured bondage was impossibe. " << girlName << " helped him see there were lots of fun things he could still do to her."
							<< " She squeezed her breasts, watching him watch the milk run down.";
					}
				}
			}
			else if (g_Dice.percent(30) && (girl->has_trait("Pierced Clit") || girl->has_trait("Pierced Nipples") || girl->has_trait("Pierced Tongue")))
			{
				sexMessage << "The customer showed real imagination involving " << girlName << "'s piercings in the BDSM action. ";
				if (girl->has_trait("Pierced Clit")) sexMessage << "Her clit piercing got extra special attention.\n";
				else if (girl->has_trait("Pierced Nipples")) sexMessage << "He 'led' her between tools using a chain on her nipple piercings.\n";
				else sexMessage << "He pulled her around using a chain on her tongue piercing.\n";
			}
			else if (g_Dice.percent(30) && (girl->has_trait("Phat Booty") || girl->has_trait("Deluxe Derriere") || girl->has_trait("Wide Bottom") || girl->has_trait("Plump Tush")))
			{
				sexMessage << girlName << " was aroused being abused by the customer. He particularly enjoyed spanking her ass and thighs, just to watch her booty ripple.";
			}
			else if (g_Dice.percent(30) && (girl->has_trait("MILF") || girl->has_trait("Whore")))
			{
				sexMessage << "The customer ties " << girlName << " upside-down and starts inserting large 'toys' in her pussy. An impressive number fit inside.\n"
					<< "She enjoys this.\n";
			}
			else if (g_Dice.percent(30) && (girl->has_trait("Smoker")))
			{
				sexMessage << "While 'playing' with her, the customer finds " << girlName << "'s cigarretes and carefully singes her with them.\n"
					<< "She actually quite enjoys this.\n";
			}
			else if (g_Dice.percent(25) /*&& GetStat(girl, STAT_SANITY) < 25*/)
			{
				const int HEDIDWHAT = 5;
				const int SHEDIDWHAT = 5;

				sexMessage << "The customer had a great time in this BDSM session. As he ";

				switch (g_Dice%HEDIDWHAT)
				{
				case 0:
					sexMessage << "thwacked her nipples red with a stick";
					break;
				case 1:
					sexMessage << "deliberately came in her eye";
					break;
				case 2:
					sexMessage << "shoved his dick in her ass";
					break;
				case 3:
					sexMessage << "dripped hot candle wax on her asshole";
					break;
				case 4:
					sexMessage << "cruelly fisted her";
					break;
				default:
					sexMessage << "did som(E)thing nasty";
					break;
				}

				sexMessage << " " << girlName << " ";

				switch (g_Dice%SHEDIDWHAT)
				{
				case 0:
					sexMessage << "started suddenly laughing";
					break;
				case 1:
					sexMessage << "softly began to sing";
					break;
				case 2:
					sexMessage << "stared up at him with an unnerving empty smile";
					break;
				case 3:
					sexMessage << "grinned and silently wept";
					break;
				case 4:
					sexMessage << "started speaking in tongues";
					break;
				default:
					sexMessage << "did som(E)thing weird";
					break;
				}

				sexMessage << ", ";

				if (girl->morality() > 33) //fundie
				{
					sexMessage << "urging him to cleanse her and saying something about fate, the Gods, and divine retribution.";
				}
				else if (girl->morality() > -33) //crazy
				{
					sexMessage << "calling him 'bunny' and urging him to teach her the secret ways.";
				}
				else	//evil
				{
					sexMessage << "urging him to make her stronger, and muttering about the strong consuming the unworthy.";
				}

				sexMessage << " This really got him in the mood.\n";
				girl->upd_stat(STAT_FAME, 5);
			}
			else if (choice < 25)
			{
				sexMessage << girlName << " was aroused as the customer deliberately hurt, used and degraded her for his sexual gratification.";
			}
#endif
			else if (choice < 50)	//Gondra: Vanilla Messages
			{
				sexMessage << girlName << " was a little turned on by being tied up and having the customer hurting her.";
			}
#if defined(SPICE)
			else if (choice < 75)
			{
				sexMessage << girlName << " squealed and groaned as the customer stimulated her sensitive areas with scalding candle-wax.";
			}
#endif
			else
			{
				sexMessage << "Being at the mercy of the customer was something " << girlName << " actually found herself enjoying a bit.";
			}
		}
		else if (check < 80) //Gondra: the girl is VERY skilled
		{
			if (g_Dice.percent(30) && (girl->has_trait("Masochist"))) //Gondra: Trait messages
			{
				sexMessage << "After telling the customer to hit her harder several times, " << girlName << " found herself gagged. Her now muffled cries seemingly adding to the enjoyment of both her and her customer.";
			}
			else if (g_Dice.percent(30) && (girl->has_trait("No Gag Reflex") || girl->has_trait("Deep Throat")))
			{
				sexMessage << girlName << " found her drooling mouth filled by the customers hard, pulsing cock, as he continued to slap her bound body, enjoying his impromptu gag service by her throat.";
			}
#if defined(SPICE)
			else if (g_Dice.percent(40) && girl->is_pregnant())
			{
				int term = (girl->m_States&(1 << STATUS_INSEMINATED) ? cfg.pregnancy.weeks_monster_p() : cfg.pregnancy.weeks_pregnant());
				if (girl->m_WeeksPreg < (term / 4)) // if she's not showing
				{
					sexMessage << girlName << " gave an amazing bondage session, steering the customer away from things that might be harmful to her unborn child, "
						<< "without him ever noticing.";
				}
				else if (g_Dice.percent(10))
				{
					sexMessage << girlName << " was bound and roughly used by some sicko who got off on her pregnancy. She loved it: "
						<< "it was great to be treated like a real whore again, and not some little china doll. She completely forgot herself and gave him an amazing time.";
					customer->m_Stats[STAT_HAPPINESS] += 20;
					girl->upd_stat(STAT_HEALTH, -4);
				}
				else
				{
					if (girl->m_WeeksPreg < ((3 * term) / 4)) // if she's showing
					{
						sexMessage << "Due to her obvious pregnancy, the customer was reluctant to do bondage.\n" << girlName << " shoved her hands into his pants and grabbed him by the balls. "
							<< "\"If you're not man enough, I can take charge?\"\nWith just the right blend of goading and encouragement, " << girlName << " pushed the customer past his inhibitions, "
							<< "and they had an amazing time.";
						girl->upd_stat(STAT_HEALTH, -1);
					}
					else		//if she's about to drop
					{
						sexMessage << "Seeing her advanced pregnancy, the customer figured bondage was impossibe. " << girlName << " helped him see there were lots of fun things he could still do to her."
							<< " She squeezed her breasts, squirting milk in his eye.\n\"Mister,\" she smiled, impudently. \"Are you going to let me get away with that?!\"";
						girl->upd_stat(STAT_HEALTH, -1);
					}
				}
			}
			else if (g_Dice.percent(30) && (girl->has_trait("Pierced Clit") || girl->has_trait("Pierced Nipples") || girl->has_trait("Pierced Tongue")))
			{
				sexMessage << girlName << " urged the customer to use her piercings while torturing her. ";
				if (girl->has_trait("Pierced Clit")) sexMessage << "Her clit piercing was definitely the most 'useful'.\n";
				else if (girl->has_trait("Pierced Nipples")) sexMessage << "Her nipple piercings were useful both for control, and for target practice.\n";
				else sexMessage << "The tongue-piercing was great for holding her mouth open while he face-fucked her.\n";
			}
			else if (g_Dice.percent(33) && (girl->has_trait("Plump") || girl->has_trait("Fat")))
			{
				sexMessage << "The customer tied " << girlName << " to a rack and spanked her fat ass, her wobbly thighs and her flabby breasts with a paddle, sending fat waves rippling all over. "
					<< "She was into the pain and humiliation, forcing the customer to degrade her further. They ended up drenched with sweat and cum.";
			}
			else if (g_Dice.percent(25) /*&& GetStat(girl, STAT_SANITY) < 25*/)
			{
				const int HEDIDWHAT = 5;
				const int SHEDIDWHAT = 5;

				sexMessage << "The customer loved this BDSM session. " << girlName << " was completely crazy, and as he ";

				switch (g_Dice%HEDIDWHAT)
				{
				case 0:
					sexMessage << "thwacked her nipples red with a stick";
					break;
				case 1:
					sexMessage << "deliberately came in her eye";
					break;
				case 2:
					sexMessage << "shoved his dick hard up her ass";
					break;
				case 3:
					sexMessage << "dripped hot candle wax on her clit";
					break;
				case 4:
					sexMessage << "cruelly fisted her";
					break;
				default:
					sexMessage << "did som(E)thing nasty";
					break;
				}

				sexMessage << " " << girlName << " ";

				switch (g_Dice%SHEDIDWHAT)
				{
				case 0:
					sexMessage << "started suddenly laughing";
					break;
				case 1:
					sexMessage << "softly began to sing";
					break;
				case 2:
					sexMessage << "stared up at him with an vacant smile";
					break;
				case 3:
					sexMessage << "grinned and silently wept";
					break;
				case 4:
					sexMessage << "started speaking in tongues";
					break;
				default:
					sexMessage << "did som(E)thing weird";
					break;
				}

				sexMessage << ", ";

				if (girl->morality() > 33) //fundie
			{
					sexMessage << "urging him to cleanse her and saying something about fate, the Gods, and divine retribution.";
			}
				else if (girl->morality() > -33) //crazy
				{
					sexMessage << "calling him 'bunny' and urging him to teach her the secret ways.";
				}
				else	//evil
				{
					sexMessage << "urging him to make her stronger, and muttering about the strong consuming the unworthy.";
				}

				sexMessage << " He didn't need to be told twice, and got so into the session.\n";
				girl->upd_stat(STAT_FAME, 10);
			}
			else if (g_Dice.percent(30) && (girl->has_trait("Flat Chest") || girl->has_trait("Petite Breasts") || girl->has_trait("Small Boobs")))
			{
				sexMessage << "The customer repeatedly spanked and slapped her 'pathetic little breasts', demanding that she grow some. "
					<< girlName << " was aroused from the pain and degradation.";
			}
			else if (g_Dice.percent(30) && girl->has_trait("Mute"))
			{
				sexMessage << "When he realises she can't scream, the customer takes the BDSM to a whole new level. Luckily she's tough and into it, "
					<< "and they both have a great time.";
			}
			else if (choice < 25)
			{
				sexMessage << girlName << " was aroused as the customer deliberately hurt, used and degraded her for his sexual gratification.";
			}
#endif
			else if (choice < 50)	//Gondra: Vanilla Messages
			{
				sexMessage << "Thoroughly bound, " << girlName << " found herself being teased endlessly by the customers cock and hands, coming hard under his expert care shortly before the end of the session.";
			}
#if defined(SPICE)
			else if (choice < 75)
			{
				sexMessage << girlName << " was aroused as the customer singed her sensitive areas with candle-wax, begging him for more.";
			}
#endif
			else
			{
				sexMessage << girlName << " was highly aroused by the pain and bondage, even more so when fucking at the same time.";
			}
		}
		else //Gondra: the girl is EXTREMELY skilled
		{
			//Gondra: BDSM has an outside function for 'check' values bigger than 80
			sexMessage << girlName << GetRandomBDSMString();
		}
		message += sexMessage.str(); //Gondra: add our sexMessage to our message string

		/* Gondra: Commented out since I would prefer indivivual messages for each proficiency level //TODO Gondra: add Sadist Texts
		//SIN: added traits
		if (girl->has_trait("Masochist"))
		{
		if (g_Dice.percent(50)) message += " She kept encouraging the customer to get more and more extreme on her.";
		else	              	message += " Despite everything, she got off on the pain and degradation.";
		}
		if (girl->has_trait("Sadist"))
		{
		if (g_Dice.percent(50)) message += " She prefers to be in charge, but the customer wouldn't have it.";
		else					message += " She took charge for awhile, which the customer enjoyed.";

		}*/
#endif
	}break; //End of SKILL_BDSM Case

	case SKILL_NORMALSEX:
	{
#if 1
		if (z)
		{
			sexMessage << girlName << " moaned lightly as her customer pounded her dead pussy.";
			break;
		}
		//Gondra: reworking this part with choice variable
		if (check < 20)		//Gondra: if the girl is unskilled show one of these messages
		{
			if (girl->has_trait("Aggressive") && g_Dice.percent(35))	//Gondra: Trait messages TODO Gondra: add positive Trait messages here?
			{
				sexMessage << girlName << " stared angrily at the customer as she tore the clothes off of her body. It made the customer feel uncomfortable.";
				customer->m_Stats[STAT_HAPPINESS] -= 5;
			}
			else if (girl->has_trait("Nervous") && g_Dice.percent(35))
			{
				sexMessage << girlName << " was clearly uncomfortable with the arrangement, and it made the customer feel uncomfortable.";
				customer->m_Stats[STAT_HAPPINESS] -= 5;
			}
			else if (!girl->has_trait("Fake Orgasm Expert") && g_Dice.percent(10))
			{
				sexMessage << girlName << "'s robotic moans along with her tearful eyes ruined the customer's boner. He didn't even manage to finish before angrily stomping out of the room.";
				customer->m_Stats[STAT_HAPPINESS] -= 15;
			}
#if defined(SPICE)
			else if (girl->has_trait("Priestess") && g_Dice.percent(55))
			{
				sexMessage << girlName << "'s mini-sermon about sin and judgement made the sex awkward. The customer was clearly uncomfortable.";
				customer->m_Stats[STAT_HAPPINESS] -= 5;
			}
			else if (g_Dice.percent(45) && girl->is_pregnant())
			{
				sexMessage << girlName << " tried to have sex, but it was awkward because of her pregnancy.";
				customer->m_Stats[STAT_HAPPINESS] -= 5;
			}
			else if ((girl->has_trait("Agile") || girl->has_trait("Flexible")) && g_Dice.percent(66))
			{
				sexMessage << "Even though " << girlName << " doesn't have much skill in bed, her suppleness meant that the customer could twist her into some great positions.";
				customer->m_Stats[STAT_HAPPINESS] += 15;
			}
			else if (g_Dice.percent(25) && girl->has_trait("Assassin"))
			{
				sexMessage << "The look " << girlName << " gave when the customer asked for a pre-sex blowie was terrifying. In fear of his life he got the deed done and escaped as fast as he could.";
				customer->m_Stats[STAT_HAPPINESS] -= 5;
			}
			else if (g_Dice.percent(25) && girl->has_trait("Alchoholic"))
			{
				sexMessage << "Somehow " << girlName << " had gotten hold of alcohol. She lay incoherent on the bed while the customer 'came' and went.";
			}
			else if (g_Dice.percent(35) && (girl->has_trait("Great Arse") || girl->has_trait("Tight Butt") || girl->has_trait("Phat Booty")
				|| girl->has_trait("Deluxe Derriere") || girl->has_trait("Wide Bottom") || girl->has_trait("Plump Tush")))
			{
				sexMessage << girlName << " is embarrassingly awkward at sex, but also has a great ass. When the customer flipped her over, gripped onto her ass and did her from behind, "
					<< "he had a better time.";
				customer->m_Stats[STAT_HAPPINESS] += 10;
			}
			else if (g_Dice.percent(35) && The_Player->disposition() > 40) //bonus if you are nice
			{
				sexMessage << girlName << " is not great at sex. However, inspired by your goodness and not wanting to let you down, she tried her best to give the customer a decent fuck.";
				customer->m_Stats[STAT_HAPPINESS] += 20;
				girl->upd_stat(STAT_HAPPINESS, 2);
				girl->upd_stat(STAT_OBEDIENCE, 2);
				girl->upd_Enjoyment(ACTION_SEX, 2);
			}
			else if (g_Dice.percent(35) && girl->refinement() > 66)
			{
				sexMessage << girlName << " didn't do much as the customer fucked her pussy, but was clearly horrified when he dumped a load of cum inside.";
			}
#endif
			else if (choice < 20)	//Gondra: Vanilla Messages TODO Gondra: Replace/supplement these Vanilla messages.
			{
				sexMessage << girlName << " didn't do much as the customer fucked her pussy.";
			}
			else if (choice < 40)
			{
				sexMessage << "The customer's inexperience combined with " << girlName << "'s inexperience lead to lots of painful grabbing of breasts, aggressive thrusts, and a quick finish. Everyone was clearly unhappy.";
				customer->m_Stats[STAT_HAPPINESS] -= 5;
			}
			else if (choice < 60)
			{
				sexMessage << "\"You get what you pay for,\" the customer grumbled as he threw a few wads of money on the jizz covered floor.";
				customer->m_Stats[STAT_HAPPINESS] -= 5;
			}
			else if (choice < 80)
			{
				sexMessage << girlName << "'s forced smile and awkward demeanor made the whole ordeal more awkward than necessary, but the deed got done.";
			}
			else
			{
				sexMessage << girlName << " just laid back and let the customer fuck her.";
			}
		}
		else if (check < 40) //Gondra:  if the girl is slightly skilled
		{

			if (g_Dice.percent(50) && girl->has_trait("Plump")) //Gondra: Trait messages
			{
				sexMessage << "The constant prodding and groping of her embarrassingly plump body made it hard for " << girlName << " to concentrate on being a good fuck.";
			}
			else if (g_Dice.percent(33) && girl->has_trait("Fast Orgasms"))
			{
				sexMessage << girlName << "'s moans grew louder and louder as the customer kept going at it with her, and even though he came before she had a chance to, it was still an enjoyable fuck for both of them.";
				customer->m_Stats[STAT_HAPPINESS] += 5;
			}
			else if (g_Dice.percent(25) && girl->has_trait("Fake Orgasm Expert"))
			{
				sexMessage << girlName << "'s sudden faked orgasm just as her customer came didn't really do it's job, but as he had already finished the customer didn't bother reprimanding her.";
			}
#if defined(SPICE)
			else if ((girl->has_trait("Agile") || girl->has_trait("Flexible")) && g_Dice.percent(66))
			{
				sexMessage << girlName << " was okay in bed, and the positions she could twist herself into impressed the customer.";
				customer->m_Stats[STAT_HAPPINESS] += 15;
			}
			else if ((girl->has_trait("Clumsy") || girl->has_trait("Dojikko")) && g_Dice.percent(66))
			{
				sexMessage << girlName << " was doing okay at sex until she accidentally sat on the customer balls, causing him quite a lot of pain.";
				customer->m_Stats[STAT_HAPPINESS] -= 20;
			}
			else if (g_Dice.percent(25) && girl->has_trait("Assassin"))
			{
				sexMessage << "The customer was excited to have sex with a dangerous femme-fatale like " << girlName << ".";
				customer->m_Stats[STAT_HAPPINESS] += 5;
			}
			else if (g_Dice.percent(25) && girl->has_trait("Alchoholic"))
			{
				sexMessage << "Somehow " << girlName << " had gotten hold of alcohol. She was too drunk to to anything for the customer and just lay back rocking and grunting while he fucked her.";
				customer->m_Stats[STAT_HAPPINESS] -= 15;
			}
			else if (g_Dice.percent(35) && (girl->has_trait("Great Arse") || girl->has_trait("Tight Butt") || girl->has_trait("Phat Booty")
				|| girl->has_trait("Deluxe Derriere") || girl->has_trait("Wide Bottom") || girl->has_trait("Plump Tush")))
			{
				sexMessage << girlName << " is okay at sex, but the customer figured out: to have a really great time you have to grip dat ass and do her from behind.";
				customer->m_Stats[STAT_HAPPINESS] += 20;
			}
			else if (g_Dice.percent(35) && (HateLove > 0) && The_Player->disposition() > 40) //bonus if you are nice
			{
				sexMessage << girlName << " is inspired by your kindness and wants you to succeed. She tried her best to give the customer great sex.";
				customer->m_Stats[STAT_HAPPINESS] += 20;
				girl->upd_stat(STAT_HAPPINESS, 2);
				girl->upd_stat(STAT_OBEDIENCE, 2);
				girl->upd_Enjoyment(ACTION_SEX, 2);
			}
			else if (g_Dice.percent(35) && HateLove > 60)	//if she likes you
			{
				sexMessage << "When " << girlName << " couldn't get in the mood for the customer, she closed her eyes and imagined it was you. She fucked him with some real passion.";
				customer->m_Stats[STAT_HAPPINESS] += 15;
			}
			else if (g_Dice.percent(35) && (GirlGotNoClass || girl->has_trait("Cum Addict")))
			{
				sexMessage << girlName << "'s sex was... efficient. However, after the customer came in her, she quickly sucked his cock clean and "
					<< "started scooping his cum out of her pussy and lapping it off her fingers, while fingering herself to orgasm right in front of him. It was a hell of an after-show.";
				customer->m_Stats[STAT_HAPPINESS] += 10;
			}
			else if (choice < 25)	//Gondra: Vanilla Messages TODO Gondra: Replace/supplement these Vanilla messages.
			{
				sexMessage << girlName << " let him fuck her, while she did her best to look like she loved it.";
			}
#endif
			else if (choice < 50)
			{
				sexMessage << girlName << " fucked the customer back while their cock was embedded in her cunt.";
			}
			else if (choice < 75)
			{
				sexMessage << girlName << " made the right noises and held the customer as he fucked her.";
			}
			else
			{
				sexMessage << girlName << " let the customer push her down and paw at her breasts, allowing a few fake moans to escape.";
			}
		}
		else if (check < 60) //Gondra: the girl is reasonably skilled
		{
			if (g_Dice.percent(35) && girl->has_trait("Slut")) //Gondra: Trait messages
			{
				sexMessage << girlName << " was on the customers cock quickly and surprised him with a few tricks while they fucked.";
			}
			else if (g_Dice.percent(50) && (girl->has_trait("Fast Orgasms") || girl->has_trait("Fake Orgasm Expert")))
			{
				sexMessage << girlName << "'s increasingly audible pleasure spurns the customer to fuck her hard, pushing her over the edge before he cums himself.";
			}
#if defined(SPICE)
			else if (g_Dice.percent(35) && SheAintPretty)
			{
				sexMessage << "The customer initially grumbled about getting \"some ugly skank\", but " << girlName << " showed him a damn good time.";
				customer->m_Stats[STAT_HAPPINESS] += 5;
			}
			else if (g_Dice.percent(35) && girl->is_pregnant())
			{
				sexMessage << "Horny from her pregnancy, " << girlName << " wanted cum inside her, and gave the customer a great time.";
				customer->m_Stats[STAT_HAPPINESS] += 5;
			}
			else if ((girl->has_trait("Agile") || girl->has_trait("Flexible")) && g_Dice.percent(66))
			{
				sexMessage << girlName << " was great at sex, athletically twisting herself into positions the customer had never imagined.";
				customer->m_Stats[STAT_HAPPINESS] += 20;
			}
			else if ((girl->has_trait("Clumsy") || girl->has_trait("Dojikko")) && g_Dice.percent(66))
			{
				sexMessage << "Despite a few clumsy bumps, " << girlName << " gave the customer some damn good sex.";
			}
			else if (g_Dice.percent(25) && girl->has_trait("Assassin"))
			{
				sexMessage << "The customer was excited to have sex with a dangerous femme-fatale. And " << girlName << " was damn good in the sack.";
				customer->m_Stats[STAT_HAPPINESS] += 10;
			}
			else if (g_Dice.percent(25) && girl->has_trait("Alchoholic"))
			{
				sexMessage << "Somehow " << girlName << " had gotten hold of alcohol. She was completely wild, fucking the customer like crazy even after he came.";
				customer->m_Stats[STAT_HAPPINESS] += 5;
			}
			else if (g_Dice.percent(35) && (HateLove > 0) && (The_Player->disposition() > 40)) //bonus if you are nice
			{
				sexMessage << girlName << " knows you are the best " << (girl->is_slave() ? "master" : "brother owner")
					<< " in town, and pulls out all the stops to give the customer amazing sex.";
				customer->m_Stats[STAT_HAPPINESS] += 20;
				girl->upd_stat(STAT_HAPPINESS, 2);
				girl->upd_stat(STAT_OBEDIENCE, 2);
				girl->upd_Enjoyment(ACTION_SEX, 2);
			}
			else if (g_Dice.percent(35) && HateLove > 60)	//if she likes you
			{
				sexMessage << girlName << " closed her eyes and imagined it was you. She fucked him dry.";
				customer->m_Stats[STAT_HAPPINESS] += 15;
			}
			else if (g_Dice.percent(35) && (GirlGotNoClass || girl->has_trait("Cum Addict")))
			{
				sexMessage << girlName << "'s sex was great. And afterward she made a show of scooping his cum out of her pussy "
					<< "and licking it off her hands. Finally she fingered herself to orgasm right in front of him.";
				customer->m_Stats[STAT_HAPPINESS] += 10;
			}
			else if (choice < 25)	//Gondra: Vanilla Messages TODO Gondra: Replace/supplement these Vanilla messages.
			{
				sexMessage << girlName << " let him fuck her, while she did her best to look like she loved it.";
			}
#endif
			else if (choice < 33)	//Gondra: Vanilla Messages TODO Gondra: Replace/supplement these Vanilla messages.
			{
				sexMessage << girlName << " pushed back against the customers pistoning hips, inspiring him to work a bit harder himself.";
			}
			else if (choice < 66)
			{
				sexMessage << girlName << " allowed the customer's hands to roam along her soft curves as she complimented his \"great\" skill.";
			}
			else
			{
				sexMessage << girlName << " managed to keep the customer going until he finished, but forgot to fake her own orgasm. Despite that, the customer left pleased with the experience.";
			}
		}
		else if (check < 80) //Gondra: the girl is very skilled
		{

			if (g_Dice.percent(35) && (girl->has_trait("Fake Orgasm Expert") || girl->has_trait("Fast Orgasms"))) //Gondra: Trait messages
			{
				sexMessage << girlName << " went at it hard with the customer, cumming shortly after he penetrated her, and then several times until she finished her performance with an especially loud orgasm as the customer came.";
			}
			else if (g_Dice.percent(35) && girl->has_trait("Slow Orgasms"))
			{
				sexMessage << "Although she is known to be hard to please, " << girlName << " managed to cum through a combination of her considerable skill and an particularly observant customer that left with a smile on his face.";
				customer->m_Stats[STAT_HAPPINESS] += 5;
			}
#if defined(SPICE)
			//SIN - more spice
			else if (g_Dice.percent(35) && (girl->has_trait("Plump") || girl->has_trait("Fat")))
			{
				if (g_Dice.percent(50))
				{
					sexMessage << "The customer is stunned that a heavy-set girl like " << girlName << " can be so good in the sack.";
					customer->m_Stats[STAT_HAPPINESS] += 5;
				}
				else
				{
					sexMessage << "The customer initially grumbled about getting \"some fat whore\", but " << girlName
						<< " really showed him how a big-girl can fuck.";
					customer->m_Stats[STAT_HAPPINESS] += 5;
				}
			}
			else if (g_Dice.percent(35) && SheAintPretty)
			{
				sexMessage << "The customer initially grumbled about getting \"some ugly skank\", but " << girlName << " really showed what she could do.";
				customer->m_Stats[STAT_HAPPINESS] += 5;
			}
			else if (g_Dice.percent(35) && girl->is_pregnant())
			{
				sexMessage << "Despite her pregnancy - or perhaps because of it - " << girlName
					<< " really fucked the customer, orgasming twice before bringing him to a powerful climax.";
				customer->m_Stats[STAT_HAPPINESS] += 5;
			}
			else if (girl->has_trait("Priestess") && g_Dice.percent(55))
			{
				sexMessage << "Sexy Priestess, " << girlName << ", lay the customer on the floor and knelt over him. She energetically praised the divine as she rode his cock, "
					<< "until she was filled with his divine blessing.\nThen she used her mouth and made him rise again.";
				customer->m_Stats[STAT_HAPPINESS] += 5;
			}
			else if (g_Dice.percent(40) && (girl->has_trait("Agile") || girl->has_trait("Flexible") || girl->agility() > 75))
			{
				if (choice < 33)
				{
					sexMessage << "The customer made full use of " << girlName << "'s agility, fucking her in a wide range of positions.";
					customer->m_Stats[STAT_HAPPINESS] += 5;
				}
				else if (choice < 67)
				{
					sexMessage << "The customer fucked " << girlName << " as she went through her stretch routine. It was hot.";
					customer->m_Stats[STAT_HAPPINESS] += 5;
				}
				else
				{
					sexMessage << "The customer was nervous and had some performance problems... Until a naked " << girlName << " lay on the bed, "
						<< "twisted both feet behind her head and smiled up at him.";
					customer->m_Stats[STAT_HAPPINESS] += 5;
				}
			}
			else if (g_Dice.percent(35) && (GirlGotNoClass || girl->has_trait("Cum Addict")))
			{
				sexMessage << girlName << "'s sex was amazing, and her well trained pussy drained him dry. Afterward she made a show of "
					<< "crouching on the floor and watching his cum dribble out. Finally she made him fuck her again while she licked his cum off the floor.";
				customer->m_Stats[STAT_HAPPINESS] += 10;
			}
			else if (g_Dice.percent(25) && girl->has_trait("Assassin"))
			{
				sexMessage << "The customer was excited to have sex with a notorious femme-fatale like " << girlName
					<< ". She gave him an incredible time.";
				customer->m_Stats[STAT_HAPPINESS] += 10;
			}
			else if (g_Dice.percent(25) && girl->has_trait("Alchoholic"))
			{
				sexMessage << "Somehow " << girlName << " was drunk. She was completely wild and uninhibited fucking and sucking the customer like crazy even after he came twice.";
				customer->m_Stats[STAT_HAPPINESS] += 15;
			}
			else if (g_Dice.percent(35) && (HateLove > 0) && (The_Player->disposition() > 40)) //bonus if you are nice
			{
				sexMessage << girlName << " knows you are the kindest " << (girl->is_slave() ? "master" : "brother owner")
					<< " in town, and feels like she owes you. She uses all her skills to make sure the customer has an incredible time.";
				customer->m_Stats[STAT_HAPPINESS] += 20;
				girl->upd_stat(STAT_HAPPINESS, 2);
				girl->upd_stat(STAT_OBEDIENCE, 2);
				girl->upd_Enjoyment(ACTION_SEX, 2);
			}
			else if (choice < 25)	//Vanilla Messages
			{
				sexMessage << girlName << " passionately rode the customer's cock until it erupted inside her.";
			}
#endif
			else if (choice < 50)	//Gondra: Vanilla Messages TODO Gondra: Replace/supplement these Vanilla messages.
			{
				sexMessage << girlName << " loved having a cock buried in her cunt and fucked back as much as she got.";
			}
			else if (choice < 75)
			{
				sexMessage << girlName << " stunned the customer with her range of positions and techniques, making him cum multiple times.";
			}
			else
			{
				sexMessage << girlName << " fucked like a wild animal, cumming several times and ending with her and the customer covered in sweat.";
			}
		}
		else //Gondra: the girl is EXTREMELY skilled
		{
			//Gondra: another case of an outside function handling check values over 80
			sexMessage << girlName << GetRandomSexString();
		}
		message += sexMessage.str(); //Gondra: add our sexMessage to our message string
#endif
	}break; //End of SKILL_NORMALSEX Case

	case SKILL_ORALSEX:
	{
#if 1
		if (z)
		{
			sexMessage << "For some reason her customer wanted his dick in her mouth. She was all too happy to oblige him.";
			if (girl->has_trait("No Teeth"))
			{
				sexMessage << "Luckily, she has no teeth so she just gummed his dick until he came.";
				customer->m_Stats[STAT_HAPPINESS] += 20;
			}
			else if (g_Dice.percent(girl->health()))
			{
				sexMessage << "Luckily, she has already been fed and did not eat him.";
			}
			else if (g_Dice.percent(girl->intelligence()))
			{
				sexMessage << "Luckily, she has some of her senses left and did not eat him.";
			}
			else if (g_Dice.percent(50))
			{
				sexMessage << "Unfortunately for him she was hungry and tried to eat what he put in her mouth.";
				customer->m_Stats[STAT_HAPPINESS] -= 50;
			}
			else
			{
				sexMessage << "Unfortunately for him she was hungry and she ate what he put in her mouth.";
				customer->m_Stats[STAT_HAPPINESS] -= 100;
				girl->health(5);
				girl->happiness(5);
			}
			break;
		}

		//Gondra: reworking this part with choice variable
		if (check < 20)		//Gondra: if the girl is unskilled show one of these messages
		{
			if (g_Dice.percent(50) && (girl->has_trait("Cum Addict")))	//Gondra: Trait messages
			{
				sexMessage << "The smell that came from the customers cock in front of her awoke " << girlName << "'s hunger for cum, which made her work his shaft with considerable greed, forgetting to be careful with her teeth, until the customer came with a pained expression, letting her swallow what she craved.";
			}
#if defined(SPICE)
			//SIN
			else if (g_Dice.percent(50) && (girl->has_trait("Princess") || girl->has_trait("Queen") || girl->has_trait("Goddess")
				|| girl->has_trait("Fallen Goddess") || girl->has_trait("Demon") || girl->has_trait("Your Daughter")))
			{
				sexMessage << "The customer is ecstatic. Sure it was awful and " << girlName << " had no clue what she was doing - but";

				bool royalty = (girl->has_trait("Princess") || girl->has_trait("Queen"));
				bool divinity = (girl->has_trait("Goddess") || girl->has_trait("Fallen Goddess"));
				bool demon = girl->has_trait("Demon");
				bool yourKid = girl->has_trait("Your Daughter");

				if (choice < 25)
				{
					sexMessage << " he just made ";
					/* */if (royalty) sexMessage << "ROYALTY";
					else if (divinity) sexMessage << "a GOD";
					else if (demon) sexMessage << "a DEMON";
					else if (yourKid) sexMessage << "your DAUGHTER";
					else sexMessage << "this girl (E)";	//Shouldn't get here, but just in case
					sexMessage << " swallow his cum!\n";
				}
				else
				{
					/* */if (choice < 50) sexMessage << "... He just had his dick sucked by ";
					else if (choice < 75) sexMessage << " he just finished face-fucking ";
					else /*            */ sexMessage << "... He just managed to Angry Dragon ";

					/* */if (royalty)	sexMessage << "Royalty!";
					else if (divinity)	sexMessage << "a Goddess!";
					else if (demon)		sexMessage << "a Demon Whore!";
					else if (yourKid)	sexMessage << "your Daughter!";
					else /*        */	sexMessage << "this girl! (E)";	//Shouldn't get here, but just in case
					sexMessage << "\n";
				}
				customer->m_Stats[STAT_HAPPINESS] += 20;
			}
			//else if (g_Dice.percent(15) && (girl->has_trait("Lolita") || girl->age() < (MINAGE + 2))) //if looks young or is near legal limit in game...
			//{
			//	sexMessage << "The 'customer' was a City Official following up on complaints that " << girlName << " looked 'too young.' Specifically, girls under-"
			//		<< MINAGE << " should not be used in this city. She explained that she is " << girl->age() << ", but with no documents, he demanded "
			//		<< "- under Statute 2218-C - that she 'prove Majority' by demonstrating 'adult-level competence' in oral sex.\nShe submitted to testing";
			//	if (g_Dice.percent(GetSkill(girl, SKILL_ORALSEX)))  //Better she is, more hope she has
			//	{
			//		sexMessage << ", and against the odds managed to make him come in her mouth.\n\"She took a while, but seemed to have some basic idea,\" he said,"
			//			<< " rating her 'Age Questionable.'\nThere is no fine, but this rating raises your suspicion.";
			//		The_Player->suspicion(10);
			//	}
			//	else
			//	{
			//		sexMessage << ", and failed badly, not even managing to get him hard.\n";
			//		if (g_Dice.percent(girl->beauty()))
			//		{
			//			sexMessage << "\"To make sure, I carried out other checks: she's young-looking, obviously; ";
			//			if (girl->has_trait("Delicate") || girl->has_trait("Fragile") || GetStat(girl, STAT_CONSTITUTION) < 40) sexMessage << "she's childishly delicate; ";
			//			if (girl->has_trait("Dependant")) sexMessage << "she's dependent, showing little sign of being able to care for herself; ";
			//			if (girl->has_trait("Tight Butt"))
			//			{
			//				sexMessage << "she has ";
			//				if (girl->has_trait("Flat Ass")) sexMessage << "a flat underdeveloped ass and ";
			//				sexMessage << "an extremely tight anus barely capable of fitting even my finger; ";
			//			}
			//			else if (girl->has_trait("Flat Ass")) sexMessage << "she has a flat underdeveloped ass; ";
			//			if (girl->has_trait("Short") || girl->has_trait("Dwarf")) sexMessage << "she's childishly short, probably not fully-grown; ";
			//			if (girl->has_trait("Virgin"))
			//			{
			//				sexMessage << "with modern surgery it's hard to be sure, but from closely examining her vagina she looks like a virgin, "
			//					<< "it's certainly very tight; ";
			//			}
			//			if (girl->has_trait("Clumsy") || girl->has_trait("Dojikko")) sexMessage << "she's clumsy; ";
			//			if (girl->has_trait("Shy") || (GetStat(girl,STAT_CHARISMA) < 40)) sexMessage << "she has the communication skills of a child; ";
			//			if (girl->has_trait("Tsundere") || girl->has_trait("Yandere")) sexMessage << "she has adolescent mood swings; ";
			//			if (girl->has_trait("Exhibitionist")) sexMessage << "she hasn't learned social norms yet, with no shame around nudity; ";
			//			if (girl->has_trait("Princess")) sexMessage << "she has childish delusions of being a 'Princess'; ";
			//			if (girl->has_trait("Nymphomaniac") || girl->has_trait("Fast Orgasms")) sexMessage << "she has sex-cravings and clitoral hyper-sensitivity - classic signs of adolescent nymphomania; ";
			//			if (girl->has_trait("Flat Chest") || girl->has_trait("Petite Breasts") || girl->has_trait("Small Boobs"))
			//			{
			//				sexMessage << "she has underdeveloped breasts, noticeably firm to the touch";
			//				if (girl->has_trait("Perky Nipples")) sexMessage << " and highly-sensitive perky little nipples";
			//				else if (girl->has_trait("Puffy Nipples")) sexMessage << " and sensitive puffy nipples";
			//				sexMessage << "; ";
			//			}
			//			sexMessage << "she... Well, the list goes on. You get the picture.\"\n \n";
			//		}
			//		sexMessage << "He finally rated her 'underage' and fined you 1,000 gold on the spot, and filed a report against you with the City Hall.\nThis will not help your reputation.";
			//		g_Gold.misc_debit(1000);
			//		The_Player->suspicion(10);
			//	}
			//}
			else if (g_Dice.percent(30) && (girl->has_trait("Shy") || girl->has_trait("Nervous")|| girl->has_trait("Lolita")))
			{
				sexMessage << girlName << " blushed furiously, with no idea how to pleasure this thing in front of her. ";
				if (choice < 50)
				{
					sexMessage << "The customer was patient, teaching her how to do it properly.";
					customer->m_Stats[STAT_HAPPINESS] += 5;
					SetSkill(girl, SKILL_ORALSEX, 2);
				}
				else
				{
					sexMessage << "The customer eventually got bored, grabbed her head and started face-fucking her. ";
					if (girl->has_trait("Strong Gag Reflex") || girl->has_trait("Gag Reflex"))
					{
						sexMessage << "She gagged, retched and threw up on his cock. The customer left disgusted.";
						customer->m_Stats[STAT_HAPPINESS] -= 30;
					}
					else
					{
						sexMessage << "She started gagging and just when she thought she was going to throw up, the customer's hot cum pumped into her mouth.";
					}
				}
			}
#endif
			else if (g_Dice.percent(40) && girl->has_trait("Dick-Sucking Lips"))
			{
				sexMessage << "Although she isn't particularly good at it, the customer enjoyed seeing " << girlName << "'s lips wrapped around his cock.";
				customer->m_Stats[STAT_HAPPINESS] += 5;
			}
#if defined(SPICE)
			else if (choice < 30)	//Vanilla
			{
				sexMessage << girlName << " gave the customer a sloppy, awkward blowjob that wasn't going anywhere. Finally he finished himself off in her face.";
			}
#endif
			else if (choice < 70)	//Gondra: Vanilla Messages TODO Gondra: Replace/supplement these Vanilla messages.
			{
				sexMessage << girlName << " awkwardly licked the customer's cock, and recoiled when he came.";
			}
			else
			{
				sexMessage << "Annoyed by her slow licks, the customer pushed his throbbing cock through " << girlName << "'s lips, roughly fucking her mouth until he finished, leaving the room while she still spit out his cum.";
				customer->m_Stats[STAT_HAPPINESS] -= 5;
			}
		}
		else if (check < 40) //Gondra:  if the girl is slightly skilled
		{
			if (g_Dice.percent(40) && girl->has_trait("Cum Addict")) //Gondra: Trait messages
			{
				sexMessage << "Knowing about the reward that awaited her, " << girlName << " sucked on the customers length with a singular drive that made the customer come quickly. She continued sucking until she had swallowed the last drop of his cum";
			}
#if defined(SPICE)
//#if FMA		//in case min age cannot be raised.
//			else if (g_Dice.percent(15) && (girl->has_trait("Lolita") || girl->age() < 20)) //if looks young or is near legal limit in game...
//#else
//			else if (g_Dice.percent(15) && (girl->has_trait("Lolita") || girl->age() < (MINAGE + 2))) //if looks young or is near legal limit in game...
//#endif
//			{
//				sexMessage << "The 'customer' was a City Official following up on complaints that " << girlName << " looked 'too young.' Specifically, girls under-"
//					<< MINAGE << " should not be used in this city. She explained that she is " << girl->age() << ", but with no documents, he demanded "
//					<< "- under Statute 2218-C - that she 'prove Majority' by demonstrating 'adult-level competence' in oral sex.\nShe submitted to testing, "
//					<< "and he carried out a full test, finally rating her skills 'insufficient' - \"I told her to swallow and she couldn't even do it,\" he explained, shaking his head.\n";
//				int evidence = 0;
//				if (g_Dice.percent(girl->beauty())) //He seems to investigate pretty girls more - coincidence, huh?!
//				{
//					sexMessage << "\"When she failed THAT,\" he explained. \"I carried out further tests to establish her maturity: firstly, she's clearly very young-looking; ";
//					if (girl->has_trait("Delicate") || girl->has_trait("Fragile") || GetStat(girl, STAT_CONSTITUTION) < 40) sexMessage << "she's childishly delicate; ", evidence++;
//					if (girl->has_trait("Dependant")) sexMessage << "she's dependent, showing little sign of being able to care for herself; ", evidence++;
//					if (girl->has_trait("Tight Butt"))
//					{
//						sexMessage << "she has ";
//						if (girl->has_trait("Flat Ass")) sexMessage << "a flat underdeveloped ass and ", evidence++;
//						sexMessage << "an extremely tight anus barely capable of fitting even my finger; ", evidence++;
//					}
//					else if (girl->has_trait("Flat Ass")) sexMessage << "she has a flat underdeveloped ass; ", evidence++;
//					if (girl->has_trait("Short") || girl->has_trait("Dwarf")) sexMessage << "she's childishly short, probably not fully-grown; ", evidence++;
//					if (girl->has_trait("Virgin"))
//					{
//						sexMessage << "with modern surgery it's hard to be sure, but from closely examining her vagina she looks like a virgin, "
//							<< "it's certainly very tight; ", evidence++;
//					}
//					if (girl->has_trait("Clumsy") || girl->has_trait("Dojikko")) sexMessage << "she's clumsy; ", evidence++;
//					if (girl->has_trait("Shy") || (girl->charisma() < 40)) sexMessage << "she has the communication skills of a child; ", evidence++;
//					if (girl->has_trait("Tsundere") || girl->has_trait("Yandere")) sexMessage << "she has adolescent mood swings; ", evidence++;
//					if (girl->has_trait("Exhibitionist")) sexMessage << "she hasn't learned social norms yet, with no shame around nudity; ", evidence++;
//					if (girl->has_trait("Princess")) sexMessage << "she has childish delusions of being a 'Princess'; ", evidence+=2;
//					if (girl->has_trait("Nymphomaniac") || girl->has_trait("Fast Orgasms")) sexMessage << "she has sex-cravings and clitoral hyper-sensitivity - classic signs of adolescent nymphomania; ", evidence++;
//					if (girl->has_trait("Flat Chest") || girl->has_trait("Petite Breasts") || girl->has_trait("Small Boobs"))
//					{
//						sexMessage << "she has underdeveloped breasts, noticeably firm to the touch", evidence++;
//						if (girl->has_trait("Perky Nipples")) sexMessage << " and highly-sensitive perky little nipples";
//						else if (girl->has_trait("Puffy Nipples")) sexMessage << " and sensitive puffy nipples";
//						sexMessage << "; ";
//					}
//					sexMessage << "she's ... Well, the list goes on. You get the picture.\"\n";
//				}
//				if (evidence > 3)
//				{
//					sexMessage << "\nOn this 'evidence' he rated her \"Likely Underage\" - fining you 500 gold and filing a report against you.\n\""
//						<< ((The_Player->Gender() == GENDER_MALE) ? "Men" : "Women")
//						<< " like you disgust me!\"\n";
//					g_Gold.misc_debit(500);
//				}
//				else sexMessage << "He finally rated her \"Age Questionable - follow up visit required\" - this significantly raises suspicion on your establishments.";
//				The_Player->suspicion(10);
//			}
			else if (g_Dice.percent(20) && (girl->has_trait("Aggressive") || girl->has_trait("Twisted") || girl->has_trait("Sadistic")))
			{
				sexMessage << "After some time there was a high-pitch squeal. A few minutes later, " << girlName << " left the room, with the customer hobbling out behind her clutching his groin."
					<< "\"They come in my eyes,\" she calmly explained, wiping down her face with a tissue. \"I punch in their balls.\n\"Fair's fair.\"";
				customer->m_Stats[STAT_HAPPINESS] -= 20;
			}
			else if (g_Dice.percent(60) && girl->has_trait("Sexy Air"))
			{
				sexMessage << girlName << " isn't the best at this, but something about the sexy way she keeps eye-contact right through "
					<< "makes the experience far more intense.";
				customer->m_Stats[STAT_HAPPINESS] += 10;
			}

			else if (choice < 50)	//Gondra: Vanilla Messages TODO Gondra: Replace/supplement these Vanilla messages.
			{
				sexMessage << "Although still a bit awkward, " << girlName << " worked the customers length with her tongue and mouth, only spitting out the customers cum after he had left.";
			}
			else
			{
				sexMessage << girlName << " mechanically pleasured her customers cock, his load shooting all over her face as she didn't pay attention.";
			}
#else
			else if (choice < 50)	//Gondra: Vanilla Messages TODO Gondra: Replace/supplement these Vanilla messages.
			{
				sexMessage << "Although still a bit awkward, " << girlName << " worked the customers length with her tongue and mouth, only spitting out the customers cum after he had left.";
		}
			else
			{
				sexMessage << girlName << " mechanically pleasured her customers cock, his load shooting all over her face as she didn't pay attention.";
			}
#endif
		}
		else if (check < 60) //Gondra: the girl is reasonably skilled
		{
			if (g_Dice.percent(35) && girl->has_trait("Cum Addict")) //Gondra: Trait messages
			{
				sexMessage << girlName << " managed to make the customer cum a second time as she continued to suck on him after she had swallowed his first load.";
			}
#if defined(SPICE)
			//SIN: more spice
			else if (g_Dice.percent(35) && (girl->has_trait("Old") || girl->age() > 45))
			{
				sexMessage << girlName << " sucked on his cock until his cum splattered across her face. She rubbed it into her skin - explaining that it's her secret ingredient for youthful looks.\n"
					<< "The customer joked that she could probably use some more.";
				customer->m_Stats[STAT_HAPPINESS] += 2;
				girl->upd_stat(STAT_HAPPINESS, -2);
			}
			/*else if (g_Dice.percent(5) && girl->has_trait("Lolita"))
			{
				sexMessage << "The customer, a City Official, claimed to be responding to 'complaints' that " << girlName << " was 'clearly under-" << MINAGE << ".' She told him she is actually "
					<< girl->age() << ", but in the absense of documentation, he demanded - under City Statute 2218-C - that she 'prove Majority' by 'demonstrating adult-level competence' in oral sex.\n"
					<< "He carried out a thorough test and finally rated her skills 'good' - \"She was very efficient,\" he nodded. \"I tested her three times and her skill certainly matches an appropriate age.\"\n"
					<< "He rated her age 'legal' - this slightly reduces suspicion around your establishments.";
				The_Player->suspicion(-5);
			}*/
			else if (g_Dice.percent(50) && girl->has_trait("Dick-Sucking Lips"))
			{
				sexMessage << "Sure there are more skillful girls out there, but having " << girlName << "'s full, soft lips wrapped around his meat blew the customer's mind.";
				customer->m_Stats[STAT_HAPPINESS] += 10;
			}
			else if (g_Dice.percent(34) && (girl->has_trait("Clumsy") || girl->has_trait("Dojikko")))
			{
				sexMessage << girlName << " gave pretty good head. Unfortunately, a clumsy accident ";
				const int ACCIDENTSHAPPEN = 5;
				switch (g_Dice%ACCIDENTSHAPPEN)
				{
				case 0:
					sexMessage << "with some candles";
					break;
				case 1:
					/* */if (girl->has_item("Cat"))			sexMessage << "involving her cat";
					else if (girl->has_item("Guard Dog"))	sexMessage << "with her Guard Dog";
					else /*****************************/	sexMessage << "with a badly timed sneeze";
					break;
				case 2:
					/* */if (girl->has_item("Studded Dildo") || girl->has_item("Dreidel Dildo") || girl->has_item("Dildo")) sexMessage << "with a misplaced dildo";
					else if (girl->has_item("Lolita Wand") || girl->has_item("Lolita Wand (Lesser)") || girl->has_item("Magical Girl Wand") || girl->has_item("Hermione's Wand")) sexMessage << "with a misplaced wand";
					else sexMessage << "with some chewing gum and a lava lamp";
					break;
				case 3:
					sexMessage << "with a plugged-in hair-curler";
					break;
				case 4:
					if (girl->has_trait("Pierced Clit") || girl->has_trait("Pierced Nipples") || girl->has_trait("Pierced Tongue") || girl->has_trait("Pierced Navel") || girl->has_trait("Pierced Nose"))
						sexMessage << "with her piercing";
					else sexMessage << "with an ornate hairpin";
					break;
				default:
					sexMessage << "of some nature (E)";
					break;
				}
				sexMessage << " left the customer in pain.";
				customer->m_Stats[STAT_HAPPINESS] -= 10;
			}
			else if (choice < 25)
			{
				sexMessage << girlName << " licked and sucked the customer's cock with some skill.";
			}
			else if (choice < 50)
			{
				sexMessage << girlName << " gave the customer some pretty good head, even giving the balls a good licking, until he came on her face.";
			}
			else if (choice < 75)
			{
				sexMessage << girlName << " sucked the customer off until he came in her mouth. She then made a great show of dribbling it all onto her chest and rubbing it into her breasts.";
			}
			else
			{
				sexMessage << girlName << " made a few more slurping noises than necessary, didn't forget to give his balls a bit of attention and swallowed the customer's cum after showing it to him. Altogether good work.";
			}
#else
			else if (choice < 33)	//Gondra: Vanilla Messages TODO Gondra: Replace/supplement these Vanilla messages.
			{
				sexMessage << girlName << " licked and sucked the customer's cock with some skill.";
			}
			else
			{
				sexMessage << girlName << " made a few more slurping noises than necessary, didn't forget to give his balls a bit of attention and swallowed the customers cum after showing it to him. Altogether good work.";
			}
#endif
		}
		else if (check < 80) //Gondra: the girl is very skilled
		{
#if defined(SPICE)
			//SIN
			if (g_Dice.percent(65) && girl->has_trait("Doctor"))
			{
				sexMessage << girlName;
				if (g_Dice.percent(50)) sexMessage << " expertly stimulated the customer with her mouth and tongue - extracting every drop of semen with medical precision.";
				else sexMessage << " gives a whole new meaning to 'bedside manner' as she kneels beside the bed sucking the customer's balls dry.";
			}
		/*	else if (g_Dice.percent(5) && girl->has_trait("Lolita"))
			{
				sexMessage << "The customer, a City Official, claimed to be responding to 'complaints' that " << girlName << " was 'clearly under-" << MINAGE << ".' She told him she is actually "
					<< girl->age() << ", but in the absense of documentation, he demanded - under City Statute 2218-C - that she 'prove Majority' by 'demonstrating adult-level competence' in oral sex.\n"
					<< "He carried out a thorough test and finally rated her skills 'Excellent' - \"A girl who can do that,\" he said. \"Is definitely old enough in my book.\"\n"
					<< "He rated her age 'Legal' and left praising her skills - this reduces suspicion around your establishments and improves her fame.";
				The_Player->suspicion(-15);
				girl->upd_stat(STAT_FAME, 5);
		}*/
			else if (g_Dice.percent(65) && girl->has_trait("Cum Addict")) //added a roll, just so that it doesn't always overpower all other options
			{
				sexMessage << girlName << " kept caressing the customers cock and balls making him cum again and again, swallowing each load until he was dry.";
			}
			else if (g_Dice.percent(50) && girl->has_trait("Lesbian"))
			{
				sexMessage << "From the expert way " << girlName << " sucks the customer's cock, you'd never think she was a lesbian. She even swallows. A true professional.";
			}
#else
			if (girl->has_trait("Cum Addict")) //Gondra: Trait messages
			{
				sexMessage << girlName << " kept caressing the customers cock and balls making him cum again and again, swallowing each load until he was dry.";
			}
#endif
			else if (g_Dice.percent(60) && (girl->has_trait("Deep Throat") || girl->has_trait("No Gag Reflex")))
			{
				sexMessage << "Surprising the customer, " << girlName << " rammed his hard cock down her own throat, occasionally looking up to his face while she worked on it with all her skill.";
			}
			else if (g_Dice.percent(60) && girl->has_trait("Nimble Tongue"))
			{
				sexMessage << "Instead of a normal blowjob, " << girlName << " showed off just how nimble her tongue is, making him blow his load after keeping him on edge for several minutes just with the tip of her tongue.";
			}
#if defined(SPICE)	//SIN - and variety
			else if (g_Dice.percent(50) && (girl->has_trait("Princess") || girl->has_trait("Queen") || girl->has_trait("Goddess")
				|| girl->has_trait("Fallen Goddess") || girl->has_trait("Demon") || girl->has_trait("Your Daughter")))
			{
				sexMessage << "The customer was overjoyed. " << girlName << " gave him amazing oral ";

				bool royalty = (girl->has_trait("Princess") || girl->has_trait("Queen"));
				bool divinity = (girl->has_trait("Goddess") || girl->has_trait("Fallen Goddess"));
				bool demon = girl->has_trait("Demon");
				bool yourKid = girl->has_trait("Your Daughter");

				if (choice < 25)
				{
					sexMessage << "and he just watched ";
					/* */if (royalty) sexMessage << "a ROYAL";
					else if (divinity) sexMessage << "a GODDESS";
					else if (demon) sexMessage << "a DEMON";
					else if (yourKid) sexMessage << "your DAUGHTER";
					else sexMessage << "this girl (E)";	//Shouldn't get here, but just in case
					sexMessage << " swallow down his cum!\n";
				}
				else
				{
					/* */if (choice < 50) sexMessage << "and he can't wait to tell his friends he had his cock blown by ";
					else if (choice < 75) sexMessage << "- he just face-fucked ";
					else /**************/ sexMessage << "- and the girl whose face he painted... ";

					/* */if (royalty) sexMessage << "Royalty!";
					else if (divinity) sexMessage << "a Goddess!";
					else if (demon) sexMessage << "a Demon!";
					else if (yourKid) sexMessage << "your Daughter!";
					else /**********/ sexMessage << "this girl! (E)";	//Shouldn't get here, but just in case
					sexMessage << "\n";
				}
				customer->m_Stats[STAT_HAPPINESS] += 20;
			}
			else if (g_Dice.percent(60) && girl->has_trait("Queen"))
			{
				sexMessage << girlName << "'s former subject, the customer was overjoyed to have his Monarch suck his dick, and felt great pride when she swallowed his cum. "
					<< "He left to tell all his friends how he had been expertly sucked off by a Queen! (Leading to lots sniggering, and rumours that would follow him for years to come)\n";
				customer->m_Stats[STAT_HAPPINESS] += 10;
				girl->upd_stat(STAT_FAME, 5);
			}
			else if (choice < 25)	//Gondra: Vanilla Messages
			{
				sexMessage << girlName << " dropped to her knees and let her mouth make love to the customer's cock.";
			}
#endif
			else if (choice < 50)
			{
				sexMessage << girlName << " loved sucking the customer's cock, and let him cum all over her.";
			}
#if defined(SPICE)	//SIN
			else if (choice < 75)
			{
				sexMessage << girlName << " licked the customer's balls and shaft so expertly, he came within seconds of her putting his cock in her mouth. "
					<< "She looked up at him, swallowed, and started again.";
			}
#endif
			else
			{
				sexMessage << girlName << " wouldn't stop licking and sucking the customer's cock until she had swallowed his entire load.";
			}
		}
		else //Gondra: the girl is EXTREMELY skilled //TODO Gondra: add extremely skilled texts.
		{
			//Gondra:
			sexMessage << girlName << GetRandomOralSexString();
		}
		message += sexMessage.str(); //Gondra: add our sexMessage to our message string
#endif
	}break; //End of SKILL_ORALSEX Case

	case SKILL_TITTYSEX:
	{
#if 1
		if (z)
		{
			//message += " stared off vancantily as the customer used her tits to get off."; /*Its not great but try to get something.. wrote when net was down so spelling isnt right CRAZY*/
			sexMessage << "(Z text not done)\n";
			//break;
		}

		//Gondra: reworking this part with choice variable
		if (check < 20)		//Gondra: if the girl is unskilled show one of these messages
		{
			if (g_Dice.percent(50) && girl->has_trait("Cum Addict"))	//Gondra: Trait messages
			{
				sexMessage << "After he was done fucking her tits, " << girlName << " scooped up his cum from her tits greedily licking off every single drop from her fingers.";
			}
			else if (g_Dice.percent(35) && (girl->has_trait("Flat Chest") || girl->has_trait("Petite Breasts") || girl->has_trait("Small Boobs")))
			{
				sexMessage << girlName << " struggled to pleasure the customer with the little bit of chest she has, until the customer jerked off onto her tiny tits telling her to rub his cum in if she wants to have actual tits someday.";
			}
			else if (g_Dice.percent(35) && (girl->has_trait("Busty Boobs") || girl->has_trait("Big Boobs") || girl->has_trait("Giant Juggs") || girl->has_trait("Massive Melons") || girl->has_trait("Abnormally Large Boobs") || girl->has_trait("Titanic Tits"))) //Gondra: Catch all for large tits for now
			{
				sexMessage << girlName << " was lying on her back occasionally yelping in pain as the customer roughly fucked her quavering tits";
			}
#if defined(SPICE)
			else if (g_Dice.percent(35) && (girl->has_trait("Furry") || girl->has_trait("Cow Girl")))
			{
				sexMessage << "Despite having more breasts than most, " << (girl->has_trait("Furry")? "beast-girl ":"cow-girl ") << girlName << " gave poor titty-sex.";
			}
			else if (g_Dice.percent(35) && (girl->has_trait("Plump") || girl->has_trait("Fat")))
			{
				sexMessage << girlName << " gave awful titty-sex and flubbered uselessly around the bed like a beached whale.";
				customer->m_Stats[STAT_HAPPINESS] -= 10;
			}
			else if (g_Dice.percent(35) && (girl->has_trait("Abundant Lactation") || girl->has_trait("Cow Tits") || girl->lactation() > 50))
			{
				sexMessage << girlName << "'s breast milk squirted around as the customer tried to fuck her tits. She wasn't good at this.";
			}
			else if (g_Dice.percent(35) && girl->has_trait("Exotic"))
			{
				sexMessage << girlName << " finds this breast fetish strange. In her land people are topless and breasts are boring, functional things for feeding babies. Why would you fuck them?!";
			}
			else if (g_Dice.percent(35) && (girl->has_trait("Exhibitionist") || girl->has_trait("Sexy Air")))
			{
				sexMessage << girlName << " had an amazing, sexy way of revealing her self which really aroused the customer. Sadly the titty-sex that followed was disappointing.";
				customer->m_Stats[STAT_HAPPINESS] += 5;
			}
			else if (g_Dice.percent(35) && girl->has_trait("Cute"))
			{
				sexMessage << girlName << " was awful at titty-sex and did nothing for the customer. But she's very cute. And there are worse things than spunking on a cute girl's chest, so the customer didn't mind so much.";
				customer->m_Stats[STAT_HAPPINESS] += 15;
			}
			else if (choice < 25)	//Vanilla Messages
			{
				sexMessage << girlName << " lay passively on the bed as the customer tried to bring himself off with her tits.";
			}
			else if (choice < 50)
			{
				sexMessage << girlName << " squeezed her breasts around amateurishly, making it difficult for the customer to get any good feeling from fucking them.";
			}
#endif
			else if (choice < 75)	//Gondra: Vanilla Messages
			{
				sexMessage << girlName << " awkwardly let the customer fuck her tits.";
			}
			else
			{
				sexMessage << girlName << " held together her breasts for the customer to fuck, sighing loudly as his cum dirtied her chest.";
			}
		}
		else if (check < 40) //Gondra:  if the girl is slightly skilled
		{
			if (g_Dice.percent(50) && girl->has_trait("Cum Addict")) //Gondra: Trait messages
			{
				sexMessage << "After letting her customer use her tits, " << girlName << " managed to catch most of his load in her mouth as he came, eagerly licking up the rest.";
			}
			else if (g_Dice.percent(35) && (girl->has_trait("Flat Chest") || girl->has_trait("Petite Breasts") || girl->has_trait("Small Boobs")))
			{
				sexMessage << girlName << " let the customer rub his cock against the nipples of her meager breasts until he came.";
			}
			else if (g_Dice.percent(35) && (girl->has_trait("Busty Boobs") || girl->has_trait("Big Boobs") || girl->has_trait("Giant Juggs") || girl->has_trait("Massive Melons") || girl->has_trait("Abnormally Large Boobs") || girl->has_trait("Titanic Tits"))) //Gondra: Catch all for large tits for now
			{
				sexMessage << "Her customers cock completely disappearing between her breasts, " << girlName << " heaved her chest up and down her customers cock, until she could feel his hot cum between her breasts.";
			}
#if defined(SPICE)
			else if (g_Dice.percent(35) && (girl->has_trait("Furry") || girl->has_trait("Cow Girl")))
			{
				sexMessage << "With more breasts than most girls, " << (girl->has_trait("Furry") ? "beast-girl " : "cow-girl ") << girlName << " gave okay titty-sex.";
				customer->m_Stats[STAT_HAPPINESS] += 5;
			}
			else if (g_Dice.percent(35) && (girl->has_trait("Plump") || girl->has_trait("Fat")))
			{
				sexMessage << girlName << " her sweat lubricated the titty-sex a little as she hefted herself around the bed.";
			}
			else if (g_Dice.percent(35) && (girl->has_trait("Abundant Lactation") || girl->has_trait("Cow Tits") || girl->lactation() > 50))
			{
				sexMessage << girlName << "'s breast milk squirted around as the customer fucked her tits.";
			}
			else if (g_Dice.percent(35) && girl->has_trait("Exotic"))
			{
				sexMessage << girlName << " finds this breast fetish strange. In her land breasts are boring, functional things. However, she is making a good effort to learn.";
			}
			else if (g_Dice.percent(35) && (girl->has_trait("Exhibitionist") || girl->has_trait("Sexy Air")))
			{
				sexMessage << girlName << " had an amazing, sexy way of revealing her self which really aroused the customer. The titty-sex that followed was okay.";
				customer->m_Stats[STAT_HAPPINESS] += 5;
			}
			else if (g_Dice.percent(35) && girl->has_trait("Cute"))
			{
				sexMessage << girlName << " was cute enough that the customer barely noticed how poor she was at titty-sex. In the end he spunked on a cute girl's breasts - that's a win in anyone's book.";
				customer->m_Stats[STAT_HAPPINESS] += 15;
			}
			else if (choice < 25)	//Vanilla Messages
			{
				sexMessage << girlName << " lay awkwardly on the bed as the customer used her tits.";
			}
			else if (choice < 50)
			{
				sexMessage << girlName << " squeezed her breasts around, helping the customer get some good feeling from fucking them.";
			}
#endif
			else if (choice < 75)	//Gondra: Vanilla Messages
			{
				sexMessage << girlName << " used her breasts on the customer's cock.";
			}
			else
			{
				sexMessage << "Holding her customers cock between her breasts, " << girlName << " unintentionally let her hot breath run over the customers tip as she massaged his shaft with her chest, earning her a sudden faceful of cum, some of it spraying into her mouth.";
			}
		}
		else if (check <60) //Gondra: the girl is reasonably skilled
		{
			if (girl->has_trait("Cum Addict")) //Gondra: Trait messages
			{
				sexMessage << "As she rubbed spit onto her tits, " << girlName << " asked her customer to give her something nice and hot to drink when he is done. Smiling, the customer fulfilled her wish with a big load of cum sprayed directly into her mouth.";
			}
			else if (girl->has_trait("Flat Chest") || girl->has_trait("Petite Breasts") || girl->has_trait("Small Boobs"))
			{
				sexMessage << "With her chest oiled up, " << girlName << " moaned lightly as she rubbed her whole upper body against her customer, letting a pleasant moan escape her lips as his hot cum splattered over her chest.";
			}
			else if (girl->has_trait("Busty Boobs") || girl->has_trait("Big Boobs") || girl->has_trait("Giant Juggs") || girl->has_trait("Massive Melons") || girl->has_trait("Abnormally Large Boobs") || girl->has_trait("Titanic Tits")) //Gondra: Catch all for large tits for now
			{
				sexMessage << "Moaning lightly as she 'accidentally' pushed the customers cock against one of her nipples, " << girlName << " begun to run him through a long, teasing routine, at the end of which he covered her large chest with a large load of his seed.";
			}
#if defined(SPICE)
			else if (g_Dice.percent(40) && girl->has_trait("Waitress"))
			{
				sexMessage << girlName << " entered dressed as a topless waitress, with her breasts pushed up on a tray. She presented them to the customer, who excitedly fucked her chest.";
				customer->m_Stats[STAT_HAPPINESS] += 5;
			}
			else if (g_Dice.percent(40) && girl->has_trait("Muscular"))
			{
				sexMessage << girlName << " gripped his cock between her powerful chest muscles, and gave him a powerful titty-fuck.";
				customer->m_Stats[STAT_HAPPINESS] += 5;
			}
			else if (g_Dice.percent(35) && (girl->has_trait("Furry") || girl->has_trait("Cow Girl")))
			{
				sexMessage << "With her extra breasts, " << (girl->has_trait("Furry") ? "beast-girl " : "cow-girl ") << girlName << " gave some memorable titty-sex.";
				customer->m_Stats[STAT_HAPPINESS] += 5;
			}
			else if (g_Dice.percent(35) && (girl->has_trait("Plump") || girl->has_trait("Fat")))
			{
				sexMessage << girlName << " used her fat to good effect in titty-sex, making her breasts grip him, and ripple very pleasantly.";
				customer->m_Stats[STAT_HAPPINESS] += 5;
			}
			else if (g_Dice.percent(35) && (girl->has_trait("Abundant Lactation") || girl->has_trait("Cow Tits") || girl->lactation() > 50))
			{
				sexMessage << girlName << "'s breast milk squirted everywhere as the customer fucked her tits, providing nice lubricaton.";
			}
			else if (g_Dice.percent(35) && girl->has_trait("Exotic"))
			{
				sexMessage << girlName << " finds this breast fetish strange. In her land breasts are boring, functional things. However, she is making a good effort to learn.";
			}
			else if (g_Dice.percent(35) && (girl->has_trait("Exhibitionist") || girl->has_trait("Sexy Air")))
			{
				sexMessage << girlName << " had an amazing, sexy way of revealing her self which really aroused the customer. The titty-sex that followed was okay.";
				customer->m_Stats[STAT_HAPPINESS] += 5;
			}
			else if (g_Dice.percent(35) && girl->has_trait("Cute"))
			{
				sexMessage << girlName << " was cute enough that the customer barely noticed how poor she was titty-sex. In the end he spunked on a cute girl's breasts - that's a win in anyone's book.";
				customer->m_Stats[STAT_HAPPINESS] += 15;
			}
			else if (choice < 25)	//Vanilla Messages
			{
				sexMessage << girlName << " clamped her breasts around his cock and jiggled them rhythmically until his cum splattered onto her chin.";
			}
			else if (choice < 50)
			{
				sexMessage << girlName << " squeezed her breasts around, enhancing the customer's feeling as he fucks them.";
			}
#endif
			else if (choice < 75)	//Gondra: Vanilla Messages
			{
				sexMessage << girlName << " smiled as her customer spurted his load over her face and chest, rubbing it over her chest as he left before she went and cleaned herself up.";
			}
			else
			{
				sexMessage << girlName << " enjoyed using her breasts on the customer's cock, letting him cum all over her.";
			}
		}
		else// if (check <80) //Gondra: the girl is very skilled
		{
			if (girl->has_trait("Cum Addict")) //Gondra: Trait messages
			{
				sexMessage << girlName << " expertly rubbed her chest against her customer, making him blow his load after only a few minutes, rubbing strength back into his length even as she still swallowed his first load, earning herself a second portion of her favorite meal before he left exhausted.";
			}
			else if (girl->has_trait("Flat Chest") || girl->has_trait("Petite Breasts") || girl->has_trait("Small Boobs"))
			{
				sexMessage << "Although the customer seemed to have originally having wanted to pick on " << girlName << ", he is left breathless as she easily makes him cum with her small bosom, that he had wanted to mock.";
			}
			else if (girl->has_trait("Busty Boobs") || girl->has_trait("Big Boobs") || girl->has_trait("Giant Juggs") || girl->has_trait("Massive Melons") || girl->has_trait("Abnormally Large Boobs") || girl->has_trait("Titanic Tits")) //Gondra: Catch all for large tits for now
			{
				sexMessage << "It didn't take long before " << girlName << " had the first load of cum coat the flesh between her breasts, but through a combination of breathless moans and expert handling of her large mammaries she managed to add a second load onto her jiggling flesh before the session ended.";
			}
			else if (choice < 50)	//Gondra: Vanilla Messages
			{
				sexMessage << girlName << " wouldn't stop using her breasts to massage the customer's cock until she had made him spill his entire load.";
			}
			else
			{
				sexMessage << girlName << " expertly used her breasts to massage the customer's cock until his entire load exploded over her.";
			}
		}
		/*else //Gondra: the girl is EXTREMELY skilled //TODO Gondra: add extremely skilled texts.
		{
			//Gondra:
			sexMessage << girlName << GetRandomSexString();
		}*/
		message += sexMessage.str(); //Gondra: add our sexMessage to our message string
#endif
	}break; //End of SKILL_TITTYSEX Case

	case SKILL_HANDJOB:
	{
		if (z)
		{
			sexMessage << "(Z text not done)\n";
			//break;
		}

		//Gondra: reworking this part with choice variable
		if (check < 20)		//Gondra: if the girl is unskilled show one of these messages
		{
			if (girl->has_trait("Cum Addict"))	//Gondra: Trait messages
			{
				sexMessage << girlName << " sat down for a few minutes to lick the cum from her hands after she had finally managed to get her customer off with her hands.";
			}
			//SIN - added spice
			else if (girl->has_trait("Farmer") || girl->has_trait("Farmers Daughter") || girl->has_trait("Country Gal"))
			{
				sexMessage << girlName << " yanks it about like she's milking a cow, leaving the customer in real pain.\n";
				customer->m_Stats[STAT_HAPPINESS] -= 10;
			}
			else if (choice < 40)	//Gondra: Vanilla Messages TODO Gondra: Replace/supplement these Vanilla messages.
			{
				sexMessage << "After enduring a few minutes of her awful handjob, the customer took hold of " << girlName << "'s hand, spit on it, and then proceeded to quickly jerk off using her hand, not paying attention to her disgusted face as he left her hand dripping with his seed.";
			}
			else
			{
				sexMessage << girlName << " awkwardly worked the customer's cock with one hand, looking a bit disgusted at the gooey seed coating her hand after he had spurted his load without warning.";
			}
		}

		else if (check < 40) //Gondra:  if the girl is slightly skilled
		{
			if (girl->has_trait("Cum Addict")) //Gondra: Trait messages
			{
				sexMessage << girlName << "'s handjob was more awkward than necessary as she almost fell because she tried to catch all his seed in her hands as her customer came.";
			}
			//SIN - added spice
			else if (girl->has_trait("Farmer") || girl->has_trait("Farmers Daughter") || girl->has_trait("Country Gal"))
			{
				sexMessage << girlName << " still seems think this is like milking cows on the farm, yanking the customer around quite unpleasantly.\n";
				customer->m_Stats[STAT_HAPPINESS] -= 5;
			}
			else if (choice < 66)	//Gondra: Vanilla Messages TODO Gondra: Replace/supplement these Vanilla messages.
			{
				sexMessage << girlName << " jerked her customer off mechanically, letting his seed dribble onto the floor.";
			}
			else
			{
				sexMessage << girlName << " used her hand on the customer's cock.";
			}
		}
		else if (check <60) //Gondra: the girl is reasonably skilled
		{
			if (girl->has_trait("Cum Addict")) //Gondra: Trait messages
			{
				sexMessage << girlName << " massaged his length and balls slowly with one hand, collecting his seed in a glass she held in her other hand as he came, greedily licking every last drop from it after she was done milking him.";
			}
			else if (choice < 66)	//Gondra: Vanilla Messages TODO Gondra: Replace/supplement these Vanilla messages.
			{
				sexMessage << "Audibly breathing and seemingly completely fixated on the cock in her hands, " << girlName << " teased her customer with her fingertips until he came hard, the first spurt of his seed hitting the floor quite a bit away.";
			}
			else
			{
				sexMessage << girlName << " enjoyed using her hand on the customer's cock to make him cum.";
			}
		}
		else// if (check <80) //Gondra: the girl is very skilled
		{
			if (girl->has_trait("Cum Addict")) //Gondra: Trait messages
			{
				sexMessage << "Massaging her customers scepter and crown jewels intensly, " << girlName << " prevented him from cumming until with a small string of silken string wrapped around the base of his shaft until he begged her to let him cum, visibly enjoying as rope after rope of his hot cum landed in her mouth.";
			}
			else if (girl->has_trait("Prehensile Tail"))
			{
				sexMessage << girlName << "'s tail wrapped tightly around her customers cock jerking him off while both her hands ran over her body giving him quite the show, which he must have enjoyed judging by the mess he made when he came.";
			}
			else if (choice < 50)	//Gondra: Vanilla Messages TODO Gondra: Replace/supplement these Vanilla messages.
			{
				sexMessage << girlName << " was moaning lightly as strings of hot cum covered her body, but didn't stop moving her hands over his cock, squeezing another exited spurt from his balls before the customer left with quivering knees.";
			}
			else
			{
				sexMessage << girlName << " loved using her hand on the customer's cock, and let him cum all over her.";
			}
		}
		/*else //Gondra: the girl is EXTREMELY skilled //TODO Gondra: add extremely skilled texts.
		{
			//Gondra:
			sexMessage << girlName << GetRandomSexString();
		}*/
		message += sexMessage.str(); //Gondra: add our sexMessage to our message string
	}break; //End of SKILL_HANDJOB Case

	case SKILL_FOOTJOB:
	{
		if (z)
		{
			//sexMessage << " laid back as the customer used her feet to get off."; /*Its not great but trying to get something.. wrote when net was down so spelling isnt right CRAZY*/
			sexMessage << "(Z text not done)\n";
			//break;
		}
#if 0
		//TODO Gondra: rework this with the standard system I used in the prior Cases
		//SIN - DONE - below
		if (check < 20)
		{
			if (g_Dice.percent(40))	sexMessage << girlName << " awkwardly worked the customer's cock with her feet,";
			else /*              */	sexMessage << girlName << " awkwardly squashed the customer's cock around with her feet,";
			if (girl->has_trait("Cum Addict")) sexMessage << girlName << " and licked up every last drop of cum when he finished.";
			else if (g_Dice.percent(40))	sexMessage << girlName << " and recoiled when he came.";
			else /*              */	sexMessage << girlName << " recoiling when he finally came.";
		}
		else if (check < 60) /*  */	sexMessage << girlName << " used her feet on the customer's cock.";
		else if (check < 80) /*  */	sexMessage << girlName << " loved using her feet on the customer's cock, and let him cum all over her.";
		else /*                  */	sexMessage << girlName << " wouldn't stop using her feet to massage the customer's cock until she had made him spill his entire load.";

		if ((g_Dice.percent(20) && girl->has_trait("Alcoholic")) ||
			(g_Dice.percent(10) && girl->has_trait("Social Drinker")))
		{
			sexMessage << "\n \n" << girlName << " had a few drinks, and ";
			switch (g_Dice % 3)
			{
			case 0:
				sexMessage << "was a little rough with her feet, causing the customer some pain.";
				customer->m_Stats[STAT_HAPPINESS] -= 5;
				break;
			case 1:
				sexMessage << "was a little more charming than usual.";
				customer->m_Stats[STAT_HAPPINESS] += 5;
				break;
			case 2:
				sexMessage << "accidentally stepped on a sensitive part of him.";
				customer->m_Stats[STAT_HAPPINESS] -= 10;
				break;
			}
#else
		if (check < 20)
		{
			//Traits
			if (g_Dice.percent(30) && girl->has_trait("Cum Addict"))
				sexMessage << girlName << " squeezed the customer's cock around with her feet and licked up every last drop of cum when he finally finished.";

			else if (g_Dice.percent(30) && girl->has_trait("Flexible"))
			{
				sexMessage << "Though " << girlName << " wasn't very skilled with her feet, her flexibility impressed the customer and improved his enjoyment a little.\n";
				customer->m_Stats[STAT_HAPPINESS] += 5;
		}

			else if ((g_Dice.percent(30) && girl->has_trait("Alcoholic")) ||
				(g_Dice.percent(10) && girl->has_trait("Social Drinker")))
			{
				sexMessage << girlName << " was supposed to give a footjob. However, she'd had a few drinks";
				switch (g_Dice % 3)
				{
				case 0:
					sexMessage << " and was rough with her feet, giving the customer no pleasure and quite a lot of pain.";
					customer->m_Stats[STAT_HAPPINESS] -= 15;
					break;
				case 1:
					sexMessage << ", so even though the footjob was awful, she was fairly charming.";
					customer->m_Stats[STAT_HAPPINESS] += 5;
					break;
				case 2:
					sexMessage << " and accidentally stepped on a sensitive part of him.";
					customer->m_Stats[STAT_HAPPINESS] -= 10;
					break;
				}
			}
			//Vanilla
			else if (choice < 33)
			{
				sexMessage << girlName << " awkwardly worked the customer's cock with her feet, not even managing to get him hard. Disappointed, he finally took matters into his own hands.\n";
			}
			else if (choice < 67)
			{
				sexMessage << girlName << " squashed the customer's cock around with her feet, and accidentally stamped a ball, causing him some pain. He left disappointed.\n";
			}
			else
			{
				sexMessage << girlName << " squeezed the customer's cock around with her feet, eventually managing to create some pleasant feelings, but recoiling when he finally came.\n";
			}
		}
		else if (check < 40)
		{
			//trait
			if (g_Dice.percent(30) && girl->has_trait("Cum Addict"))
				sexMessage << girlName << " massage the customer's cock eagerly with her feet and licked up every last drop of cum when he finally came.";

			else if (girl->has_trait("Sexy Air"))
			{
				sexMessage << girlName << " flirted expertly as her feet massaged his cock. Her movements were crude and unskilled, but her sexy air more than made up for it.";
		}

			else if ((g_Dice.percent(30) && girl->has_trait("Alcoholic")) ||
				(g_Dice.percent(10) && girl->has_trait("Social Drinker")))
			{
				sexMessage << "The customer asked " << girlName << " for a footjob. She'd had a few drinks";
				switch (g_Dice % 3)
				{
				case 0:
					sexMessage << ", however, and was far too rough with her feet, making the whole experience a lot less pleasant. He did eventually come.";
					customer->m_Stats[STAT_HAPPINESS] -= 5;
					break;
				case 1:
					sexMessage << ", so even though the footjob was average, she flirted, goaded and charmed him to a powerful orgasm.";
					customer->m_Stats[STAT_HAPPINESS] += 10;
					break;
				case 2:
					sexMessage << " and accidentally kicked him a few times, causing him to leave in pain.";
					customer->m_Stats[STAT_HAPPINESS] -= 15;
					break;
				}
			}

			else if (g_Dice.percent(30) && girl->has_trait("Lesbian"))
			{
				sexMessage << girlName << " did her best to bring the customer off with her feet. This is about as close to a cock as this dyke wants to get.";
			}

			//Vanilla
			else if (choice < 33)
			{
				sexMessage << girlName << " worked the customer's cock with her feet for a while, until his cum finally spurted onto her.\n";
			}
			else if (choice < 67)
			{
				sexMessage << girlName << " rubbed the customer's cock around with her feet, accidentally swiping a ball with her toe, but she got him there in the end.\n";
			}
			else
			{
				sexMessage << girlName << " massaged the customer's cock with her feet, managing to create some good feelings, but flinching when he finally came.\n";
			}
		}
		else if (check < 60)
		{
			//Traits
			if (g_Dice.percent(30) && girl->has_trait("Natural Pheromones"))
			{
				sexMessage << "Something about " << girlName << "'s smell had him rock hard the moment he entered the room. "
					<< "She didn't have to do so much with her feet to bring him to an explosive orgasm.\n";
			}
			else if (g_Dice.percent(40) && (girl->has_trait("Twisted") || girl->has_trait("Audacity")))
			{
				sexMessage << girlName << " skillfully rubbed the customer's cock with her feet. "
					<< "At the last possible moment, she moved his cock so that he came all over his own stomach, laughing at his disgusted expression.\n";
			}
			else if (g_Dice.percent(30) && girl->has_trait("Lesbian"))
			{
				sexMessage << girlName << " skillfully milked the man's cock with her feet. She could do this all day with a smile if it meant she didn't have to fuck the stupid things.\n";
			}
			else if (g_Dice.percent(30) && girl->has_trait("Cum Addict"))
			{
				sexMessage << girlName << " massage the customer's cock powerfully with her feet until his cum exploded in her face.\n";
			}
			else if (g_Dice.percent(30) && girl->has_trait("Sexy Air"))
			{
				sexMessage << girlName << " flirted as her feet expertly massaged the customer's cock. Her movements were good, and her sexy air heightened the experience for him.\n";
			}
			else if (g_Dice.percent(30) && girl->has_trait("Flexible"))
			{
				sexMessage << girlName << " used all her skills and her stunning flexibility to give the customer an excellent footjob.\n";
				customer->m_Stats[STAT_HAPPINESS] += 15;
			}
			//Vanilla
			else if (choice < 33)
			{
				sexMessage << girlName << " skillfully worked the customer's cock with her feet, until his cum spurted all over her.\n";
			}
			else if (choice < 67)
			{
				sexMessage << girlName << " deftly rubbed the customer's cock with her feet, making him cum all in her toes.\n";
			}
			else
			{
				sexMessage << girlName << " expertly massaged the customer's cock with her feet, making him come twice.\n";
			}
		}
		else if (check < 80)
		{
			//traits
			if (g_Dice.percent(30) && girl->has_trait("Natural Pheromones"))
			{
				sexMessage << "Something about " << girlName << "'s smell had his dick rock-hard the moment he entered the room. "
					<< "Her expert feet took him through several orgasms, and left him blissed-out and exhausted.\n";
			}
			else if (g_Dice.percent(30) && (girl->has_trait("Twisted") || girl->has_trait("Audacity")))
			{
				sexMessage << girlName << " expertly made love to the customer's cock with her feet until, at the last possible moment, "
					<< "she pointed it up so that he shot his whole load up at his own gasping cum-face. After an experience like that, "
					<< "he couldn't help laughing at her audacity, as he hocked his own cum out of his mouth.\n";
			}
			else if (g_Dice.percent(30) && girl->has_trait("Lesbian"))
			{
				sexMessage << girlName << " expertly milked the man's cock with her feet, making sure the cum mostly missed her. "
					<< "Being skillful at this means she doesn't have to fuck the filthy things.\n";
			}
			else if (g_Dice.percent(30) && girl->has_trait("Cum Addict"))
			{
				sexMessage << girlName << " massage the customer's cock powerfully with her feet until his cum exploded in her open mouth.\n";
			}
			else if (g_Dice.percent(30) && girl->has_trait("Sexy Air"))
			{
				sexMessage << girlName << " flirted as her feet expertly massaged the customer's cock. Her movements were perfect, and her sexy air heightened the experience for to something divine.\n";
			}
			else if (g_Dice.percent(30) && girl->has_trait("Flexible"))
			{
				sexMessage << girlName << " used all her foot-skills and her stunning flexibility to give the customer an unforgettable experience.\n";
				customer->m_Stats[STAT_HAPPINESS] += 25;
			}
			else if (g_Dice.percent(10) && girl->has_trait("Incest"))
			{
				sexMessage << "A child of incest, " << girlName << " has unusual webbed-toes that make her footjobs an unforgettably 'gripping' experience.\n";
			}
			//Vanilla
			else if (choice < 33)
			{
				sexMessage << girlName << " wouldn't stop using her feet to massage the customer's cock until she had made him spill his entire load.";
			}
			else if (choice < 67)
			{
				sexMessage << girlName << " exoertly milked the customer's cock with her feet, until she felt his sticky cum pumping through her toes.\n";
			}
			else
			{
				sexMessage << girlName << " loved using her feet on the customer's cock, and let him cum all over her.";
			}
		}
		/*else //Gondra: the girl is EXTREMELY skilled //TODO Gondra: add extremely skilled texts.
		{
		//Gondra:
		sexMessage << GetRandomSexString();
		}*/
#endif
		message += sexMessage.str(); //Gondra: add our sexMessage to our message string
	}break; //End of SKILL_FOOTJOB Case

	case SKILL_BEASTIALITY:
	{
#if 1
		if (z)
		{
			//sexMessage << "Seems the customer thought having a beast fuck the dead would be great fun."; /*Its not great but try to get something.. wrote when net was down so spelling isnt right CRAZY*/
			//sexMessage << "(Z text not done)\n";
			//SIN - ADDED
			sexMessage << "This customer wanted to see an undead girl fucked by a beast. ";
			if (g_Brothels.GetNumBeasts() == 0)
			{
				sexMessage << "Unfortunately there were no beasts, so a fake was used. It's not the same.\n \n";
				customer->m_Stats[STAT_HAPPINESS] -= 30;
				break;
		}
			sexMessage << "\n" << girlName;
			if (g_Dice.percent(50))
			{
				sexMessage << " was docile and unresponsive";
				if (g_Dice.percent(50))
				{
					sexMessage << " as the beast tried a number of ways to mate with her cold, motionless body.\nThe customer was entertained.\n";
					customer->m_Stats[STAT_HAPPINESS] += 30;
				}
				else
				{
					sexMessage << ". The beast instinctively recoiled, refusing to go anywhere near her.\nThe customer was disappointed.\n";
					customer->m_Stats[STAT_HAPPINESS] -= 30;
				}
			}
			else
			{
				sexMessage << " was irrational and aggressive. With an angry grunt she lurched and clawed at the amorous beast";
				if (g_Dice.percent(girl->combat()))
				{
					sexMessage << " quickly tearing the poor creature apart and feasting on its flesh.\nThe customer seemed a little shocked.\n";
					g_Brothels.add_to_beasts(-1);
					girl->happiness(+5);
					girl->health(+5);
					customer->m_Stats[STAT_HAPPINESS] -= 10;
				}
				else
				{
					sexMessage << ", but your beast easily overpowered her.\nThe customer is thrilled to watch as your beast pins and fucks this furious, snarling zombie-girl.\n";
					girl->health(-10);
					customer->m_Stats[STAT_HAPPINESS] += 50;
				}
			}
			break;

		}

		//TODO Gondra: rework this with the standard system I used in the prior Cases - although this will require a bit more work.
		if (g_Brothels.GetNumBeasts() == 0)
		{
			sexMessage << girlName << " found that there were no beasts available, so some fake ones were used. This disappointed the customer somewhat.";
			customer->m_Stats[STAT_HAPPINESS] -= 10;
		}
		else
		{
			int harmchance = -(girl->beastiality() + girl->animalhandling() - 50);  // 50% chance at 0 skill, 1% chance at 49 skill
			if (g_Dice.percent(harmchance))
			{
				sexMessage << girlName << " accidentally harmed some beasts during the act.\n";
				g_Brothels.add_to_beasts(-((g_Dice % 3) + 1));
			}
			else if (g_Dice.percent(1 +
				(girl->has_trait("Aggressive") ? 3 : 0) +
				(girl->has_trait("Assassin") ? 1 : 0) +
				(girl->has_trait("Merciless") ? 1 : 0) +
				(girl->has_trait("Sadistic") ? 2 : 0) +
				(girl->has_trait("Twisted") ? 1 : 0)))
			{
				sexMessage << girlName << " \"accidentally\" harmed a beast during the act.\n";//Made it actually use quote marks CRAZY
				g_Brothels.add_to_beasts(-1);
			}

			if (check < 20)		//Gondra: if the girl is unskilled show one of these messages
			{
				//Gondra: Trait messages
				if (girl->has_trait("Cow Girl"))
				{
					sexMessage << "Held down by the customer, " << girlName << " gritted her teeth as the beast penetrated her roughly, mooing with definite discomfort when the customer told her to do so.";
				}
				else if (choice < 40)	//Gondra: Vanilla Messages TODO Gondra: Replace/supplement these Vanilla messages.
				{
					sexMessage << "Initially the customers was amused as the beast pounced " << girlName << ", who was not able to handle the beast, but the panicked shrieks of the girl as the horny monster tried to force itself onto her killed his mood though.";
				}
				else
				{
					sexMessage << girlName << " was disgusted by the idea but still allowed the customer to watch as she was fucked by some animals.";
				}
			}

			else if (check < 40) //Gondra:  if the girl is slightly skilled
			{
				if (girl->has_trait("Canine") || girl->has_trait("Cat Girl")) //Gondra: Trait messages
				{
					sexMessage << "The beast seemed to be a bit wary of " << girlName << " but came closer as the customer made her present herself, fucking her hard as the customer watched.";
				}
				else if (choice < 50)	//Gondra: Vanilla Messages TODO Gondra: Replace/supplement these Vanilla messages.
				{
					sexMessage << girlName << " was not very enthusiastic about this, but allowed the customer to direct the beast to mount her.";
				}
				else
				{
					sexMessage << girlName << " was a little put off by the idea, but still allowed the customer to watch and help as she was fucked by animals.";
				}
			}
			else if (check <60) //Gondra: the girl is reasonably skilled
			{
				if (girl->has_trait("Prehensile Tail") || girl->has_trait("Playful Tail")) //Gondra: Trait messages
				{
					sexMessage << girlName << " playfully lifted her tail, presenting her wet cunt to the beast which eagerly filled her needy cunt as the customer stroked his length.";
				}
				else if (girl->has_trait("Fertile") || girl->has_trait("Broodmother"))
				{
					sexMessage << "The beast kept pumping its cum deep into " << girlName << "'s pussy, leaving her belly a bit distended with cum afterwards, making the customer remark that the monster seemed intent on making her carry its progeny.";
				}
				else if (choice < 50)	//Gondra: Vanilla Messages TODO Gondra: Replace/supplement these Vanilla messages.
				{
					sexMessage << "Tentacles wriggled themselves into " << girlName << "'s welcoming holes, making her cum while the customer leered at them.";
				}
				else
				{
					sexMessage << girlName << " took a large animal's cock deep inside her and enjoyed being fucked by it, her cries of pleasure being muffled by the customer's cock in her mouth.";
				}
			}
			else if (check <80) //Gondra: the girl is very skilled
			{
				if (girl->has_trait("Cum Addict")) //Gondra: Trait messages
				{
					sexMessage << girlName << " eagerly swallowed every drop of cum she could squeeze from the many cocks of the beast while it fucked her, only pausing for a moment to receive the customers load.";
				}
				else if (girl->has_trait("Cat Girl") || girl->has_trait("Canine"))
				{
					sexMessage << girlName << " acted like she was in heat as she kept fucking the beasts, cumming too often to count, fully embracing her wild side until she had exhausted the monsters.";
				}
				else if (girl->has_trait("Cum Addict"))
				{
					sexMessage << girlName << " captivated the customer with her performance of a noble elf falling into depravity. Although she could not really hide that she was well experienced, she made it up to the customer by subtly making it so that he could see the beast fucking her from the best angle as she enjoyed herself getting railed by the monster.";
				}
				else if (choice < 50)	//Gondra: Vanilla Messages TODO Gondra: Replace/supplement these Vanilla messages.
				{
					sexMessage << girlName << " made sure that the customer could see the monsters many tentacles and cocks fucking her, as she came again and again.";
				}
				else
				{
					sexMessage << girlName << " fucked some exotic beasts covered with massive cocks and tentacles, she came over and over alongside the customer.";
				}
			}
			else //Gondra: the girl is EXTREMELY skilled
			{
			//Gondra: random beast string
				sexMessage << girlName << GetRandomBeastString();
			}
		}
		message += sexMessage.str(); //Gondra: add our sexMessage to our message string
#endif
	}break; //End of SKILL_BEASTIALITY Case

	case SKILL_GROUP:
	{
#if 1
		if (z)
		{
			sexMessage << "The group of customers had fun chasing, tackling and gangbanging their zombie sex toy.\n";
			break;
		}
		//Gondra: reworking this part with choice variable
		if (check < 20)		//Gondra: the girl is unskilled
		{
#if defined(SPICE)
			//SIN - more spice
			if (g_Dice.percent(33) && (girl->has_trait("Slut") || girl->has_trait("Nymphomaniac")))	//V. common traits - added roll to stop this supressing everything else
			{
				sexMessage << "At first " << girlName << " seemed to be in her element surrounded by so many \"wonderful\" cocks, but it quickly became apparent that she does not have the experience to satisfy them all.";
			}
			else if (g_Dice.percent(60) && girl->check_virginity())
			{
				sexMessage << girlName << " has never fucked ONE person before and had no idea how to handle this kind of group. She was completely overwhelmed and had no control over what was happening "
					<< "as strangers twisted and dragged her around while endless cocks were shoved painfully inside her and splurted cum in her face.";
				girl->upd_stat(STAT_HEALTH, -5);
				girl->upd_stat(STAT_HAPPINESS, -5);
			}
			else if (g_Dice.percent(60) && (girl->has_trait("Delicate") || girl->has_trait("Lolita")))
			{
				sexMessage << "This was far too much for a delicate flower like " << girlName << ". By the end she had no control over what happened, as endless cocks rammed into her.";
				girl->upd_stat(STAT_HEALTH, -3);
				girl->upd_stat(STAT_HAPPINESS, -1);
			}
			else if (g_Dice.percent(30) && (girl->has_trait("Optimist") || girl->has_trait("Quick Learner")))
			{
				sexMessage << girlName << " was completely unable to handle this group. While it was damn uncomfortable, being fucked this many ways by this many customers, it was a powerful experience. She's quietly confident she'll do better next time.";
				SetSkill(girl, SKILL_GROUP, g_Dice % 3);
				girl->upd_stat(STAT_HAPPINESS, 1);
			}
			else if (g_Dice.percent(40) && girl->has_trait("Natural Pheromones"))
			{
				sexMessage << "Her powerful pheromones drove the group insane. When the customers finally staggered out one of your staff found "<< girlName << " unconscious over a desk with cum coating her face and hair, and dribbling from her pussy, mouth and asshole.";
			}
			else if (g_Dice.percent(30) && SheAintPretty)
			{
				sexMessage << girlName << " was completely unable to handle this group. The whole experience was awful, especially the bit where they held her to the bed and deliberately splurted stinking cum in her eyes, up her nose and all over her mouth to hide her 'ugly-bitch face.'";
			}
			else if (choice < 20)
			{
				sexMessage << girlName << " was overwhelmed by the group surrounding her, barely able to react to what was done to her.";
			}
			else if (choice < 40)
			{
				sexMessage << girlName << " was being used by the group more than her actively servicing them.";
			}
			else if (choice < 60)
			{
				sexMessage << girlName << " struggled to service everyone in the group that came to fuck her.";
			}
			else if (choice < 80)
			{
				sexMessage << girlName << " was powerless as hands pinned her down and groped her, while cocks were stuffed into every hole.";
			}
			else
			{
				sexMessage << girlName << " awkwardly tried to service the group, but was soon overwhelmed as they got bored and took the initiative.";
			}
#else
			if (girl->has_trait("Slut") || girl->has_trait("Nymphomaniac"))	//Gondra: Trait messages
			{
				sexMessage << "At first " << girlName << " seemed to be in her element surrounded by so many \"wonderful\" cocks, but it quickly became apparent that she does not have the experience to satisfy them all.";
			}
			else if (choice < 33)	//Gondra: Vanilla Messages TODO Gondra: Replace/supplement these Vanilla messages.
			{
				sexMessage << girlName << " was overwhelmed by the group surrounding her, barely able to react to what was done to her.";
			}
			else if (choice < 66)
			{
				sexMessage << girlName << " was being used by the group more than her actively servicing them.";
			}
			else
			{
				sexMessage << girlName << " struggled to service everyone in the group that came to fuck her.";
			}
#endif
		}
		else if (check < 40) //Gondra:  if the girl is slightly skilled
		{
#if defined(SPICE)
			//SIN - more spice and variety
			if (g_Dice.percent(35) && girl->has_trait("Plump"))
			{
				sexMessage << girlName << "'s jiggling body seemed to invite the cocks around her to prod her everywhere as she struggled to satisfy the demands of the group.";
			}
			else if (g_Dice.percent(35) && girl->has_trait("Dick-Sucking Lips"))
			{
				sexMessage << girlName << "'s full, soft lips looked like heaven. Her body was almost untouched as they all had a turn making love to her mouth. She was left ";
				if (choice < 33)
				{
					sexMessage << "pretty nauseous from all the cum she swallowed down.";
					girl->upd_stat(STAT_HEALTH, -2);
				}
				else if (choice < 67)
				{
					sexMessage << "with a raw throat and cum all over her face and hair.";
					girl->upd_stat(STAT_HEALTH, -1);
					girl->upd_stat(STAT_DIGNITY, -1);
				}
				else
				{
					sexMessage << "broken, slumped naked in a corner, with cum and drool running from her mouth.";
					girl->upd_stat(STAT_SPIRIT, -2);
				}
			}
			else if (g_Dice.percent(35) && (girl->has_trait("Busty Boobs") || girl->has_trait("Big Boobs") || girl->has_trait("Giant Juggs") ||
				girl->has_trait("Massive Melons") || girl->has_trait("Abnormally Large Boobs") || girl->has_trait("Titanic Tits")))
			{
				sexMessage << "When " << girlName << "'s pussy, ass and mouth can't keep the group happy, they start squeezing her breasts together and fucking them too.";
			}
			else if (g_Dice.percent(30) && SheAintPretty)
			{
				sexMessage << girlName << " was barely able to handle this group. The whole experience was pretty bad for her, especially the bit where "
					"they held her to the bed and deliberately splooged stinking cum in her eyes, nose and all over her head to hide 'that ugly-bitch face.'";
			}
			else if (g_Dice.percent(35) && (GirlGotNoClass || girl->has_trait("Open Minded")))
			{
				sexMessage << girlName << " struggled to keep everyone in the group happy, but impressed them with her lack of inhibitions. "
					"Some girls have class, but " << girlName << " was happy to do anything they wanted.";
			}
			else if (g_Dice.percent(35) && SheAintPretty)
			{
				sexMessage << girlName << " didn't impress them with her looks, but she was okay in the bedroom.";
			}
			else if (choice < 25)	//Gondra: Vanilla Messages
			{
				sexMessage << girlName << " was barely able to service everyone, but managed to entertain her customers nonetheless.";
			}
			else if (choice < 50)
			{
				sexMessage << girlName << " used every trick she knew to keep the whole group satisfied.";
			}
			else if (choice < 75)
			{
				sexMessage << girlName << " lost count after the first four or five creampies, but they all seemed satisfied.";
			}
			else
			{
				sexMessage << girlName << " struggled to keep all the group satisfied all the time, but certainly gave everyone something to remember.";
			}
#else
			if (girl->has_trait("Plump")) //Gondra: Trait messages
			{
				sexMessage << girlName << "'s jiggling body seemed to invite the cocks around her to prod her everywhere as she struggled to satisfy the demands of the group.";
			}
			else if (choice < 50)	//Gondra: Vanilla Messages
			{
				sexMessage << girlName << " barely was able to service everyone, but managed to entertain her customers nonetheless.";
			}
			else
			{
				sexMessage << girlName << " managed to keep the group of customers fucking her satisfied.";
			}
#endif
		}
		else if (check < 60) //Gondra: the girl is reasonably skilled
		{
			if (g_Dice.percent(50) && (girl->has_trait("Busty Boobs") || girl->has_trait("Big Boobs") || girl->has_trait("Giant Juggs") || girl->has_trait("Massive Melons") || girl->has_trait("Abnormally Large Boobs") || girl->has_trait("Titanic Tits"))) //Gondra: Trait messages
			{
				sexMessage << girlName << "'s large chest was the center of attention as she serviced the group, being prodded and fucked by the customers numerous dicks, leaving her chest glazed with layers of cum";
				if (girl->has_trait("Cum Addict"))
				{
					sexMessage << " which she hungrily licked off as if it was candy.";
				}
				else
				{
					sexMessage << " which took her quite a bit of time to clean up afterwards.";
				}
			}
#if defined(SPICE)
			//SIN - a little more variety
			else if (g_Dice.percent(66) && girl->has_trait("Shape Shifter"))
			{
				sexMessage << "As a shape shifter, " << girlName << " is able to fuck and suck the entire group simultaneously. It's not something they're ever likely to forget.";
				customer->m_Stats[STAT_HAPPINESS] += 30;
			}
			else if (g_Dice.percent(66) && girl->has_trait("Shy"))
			{
				sexMessage << "For a 'shy' girl, " << girlName << " seems surprising comfortable fucking and sucking an group of random men.";
			}
			else if (g_Dice.percent(50) && (girl->has_trait("Plump") || girl->has_trait("Fat")))
			{
				sexMessage << girlName << " is pretty skilled at this. Her size just means there's plenty of girl go round.";
			}
			else if (g_Dice.percent(35) && (GirlGotNoClass || girl->has_trait("Open Minded")))
			{
				sexMessage << girlName << " worked hard to keep everyone happy, and impressed them with her lack on inhibitions. "
					"Some girls won't lick a dick that just came inside her, but " << girlName << " didn't seem to care.";
			}
			else if (g_Dice.percent(35) && SheAintPretty)
			{
				sexMessage << "The gang were disappointed with " << girlName << " looks at first, but she soon made them forget about it.";
			}
			else if (choice < 25)
			{
				sexMessage << girlName << " had some sexy tricks for pleasuring more customers at once than they expected.";
			}
			else if (choice < 50)
			{
				sexMessage << girlName << " did a good job. When she got up, cum oozed out and was running down her leg for over a minute.";
			}
#endif
			else if (choice < 75)	//Gondra: Vanilla Messages
			{
				sexMessage << girlName << " managed to surprise a few of her customers as she pleasured more of them at the same time than they had thought possible.";
			}
			else
			{
				sexMessage << girlName << " serviced everyone in the group of customers that fucked her.";
			}
		}
		else if (check < 80) //Gondra: the girl is very skilled
		{
			//SIN - some reworking and a little added variety
			if (g_Dice.percent(40) && (girl->has_trait("Deep Throat") || girl->has_trait("No Gag Reflex"))) //roll added to stop these common traits suppressing everything else
			{
				sexMessage << "After seeing " << girlName << "'s throat easily handling the largest cock in the group, they all took turns cumming deep in her throat.";
				if (girl->has_trait("Cum Addict"))
				{
					sexMessage << " Leaving her happy and full.";
				}
				else
				{
					sexMessage << " Leaving her looking a tiny bit ill because of the sheer amount of cum forced down her throat in such a small amount of time."; // Gondra: chance to gain cum addict?
				}
			}
#if defined(SPICE)
			else if (g_Dice.percent(40) && girl->has_trait("Natural Pheromones"))
			{
				sexMessage << "Her powerful pheromones drove the group insane. Luckily she was skilled enough to keep up with them all. "
					<< "Many orgasms later, she lay amid her boys in a naked bundle of sweat, semen and satisfied smiles.";
			}
#endif
			else if (g_Dice.percent(50) && (girl->has_trait("Phat Booty") || girl->has_trait("Plump Tush") || girl->has_trait("Wide Bottom") || girl->has_trait("Great Arse")))
			{
				sexMessage << "While it certainly isn't the only thing the group uses, " << girlName << "'s great arse sees near constant use, always a fresh cock ready to make her backside ripple when the previous one is done filling her insides with creamy white cum.";
			}
#if defined(SPICE)
			else if (g_Dice.percent(50) && (girl->has_trait("Slut") || girl->has_trait("Nymphomaniac")))
			{
				sexMessage << girlName << " was definitely in her element surrounded by so many \"wonderful\" cocks, and she refused to stop until she had drained every one dry.";
			}
			else if (g_Dice.percent(35) && (GirlGotNoClass || girl->has_trait("Open Minded")))
			{
				sexMessage << girlName << " impressed the group with her total absense of inhibitions, licking and sucking *anything* no matter where it had been, and doing everything they could imagine.";
			}
			else if (g_Dice.percent(35) && SheAintPretty)
			{
				sexMessage << "The gang were disappointed with " << girlName << " looks at first, but she soon made them forget about it.";
			}
#endif
			else if (choice < 50)	//Gondra: Vanilla Messages
			{
				sexMessage << girlName << " was praised for her enthusiastic multitasking, which left everyone satisfied and a bit exhausted.";
			}
			else
			{
				sexMessage << girlName << " fucked and came many times with everyone in the group of customers.";
			}
		}
		else //Gondra: the girl is EXTREMELY skilled
		{
			//Gondra: Group has a randomized way of providing texts for 'check' values greater than 80
			sexMessage << girlName << GetRandomGroupString();
		}
		message += sexMessage.str(); //Gondra: add our sexMessage to our message string
#endif
	}break; //End of SKILL_GROUP Case

	case SKILL_LESBIAN:
	{
		if (z)
		{
			//SIN: Zombie lesbians? CHECK
			//sexMessage << "Seems the customer was interested in knowing if a dead girls pussy tasted any different."; /*Its not great but try to get something.. wrote when net was down so spelling isnt right CRAZY*/
			//sexMessage << "(Z text not done)\n";
			int zLezroll = g_Dice.d100();
			if (zLezroll < 45)
			{
				sexMessage << "This thrill-seeking woman wanted a zombie-girl to eat her pussy. ";
				if (girl->has_trait("No Teeth"))
				{
					sexMessage << "Frighteningly, " << girlName << " had exactly the same idea. Luckily she has no teeth, so her furious efforts to consume "
						<< "the customer's pussy only succeeded in bringing the thrilled woman to a powerful orgasm.";
					customer->m_Stats[STAT_HAPPINESS] += 60;
				}
				else if (g_Dice.percent(girl->health()))
				{
					sexMessage << "Luckily, " << girlName << " has been fed recently and showed no interest.";
				}
				else if (g_Dice.percent(girl->intelligence()))
				{
					sexMessage << "Luckily, " << girlName << " has some of sense left and did not eat the woman.";
				}
				else
				{
					sexMessage << "Unfortunately, " << girlName << " had exactly the same idea, "
						<< "and bit into the customer's cunt.\nHer shocked screams quickly brought help.\n";
					customer->m_Stats[STAT_HAPPINESS] -= 50;
				}
				break;
			}
			else if (zLezroll < 85)
			{
				const int OPTIONS = 5;
				sexMessage << "This slim, unnaturally pale female customer stared silently at your zombie-girl. She stood in the corner waiting silently until " << girlName << "'s handler left.\nThe guards heard the lock click shut";
				switch (g_Dice%OPTIONS)
				{
				case 0:
					sexMessage << " and after that, not another sound";
					break;
				case 1:
					sexMessage << ". Then the screams started. They echoed through the halls for a solid hour";
					break;
				case 2:
					sexMessage << ", and an Angelic singing begun, punctuated by strange zombie grunts";
					break;
				case 3:
					sexMessage << ". Moments later, with no warning, several of the guards passed out. Suddenly there was a wraithlike scream that came from everywhere. Then silence";
					break;
				case 4:
					sexMessage << " and not the slightest sound was heard, except for one guard who swore he could hear weeping";
					break;
				default:
					sexMessage << " and what happened next can never be spoken of (Error)";
					break;
				}
				sexMessage << ".\nAt the end of her time, the lock clicked open. Without a word, the woman walked out of the building.\n";
				if (g_Dice.percent(40))
				{
					sexMessage << "\nFearing she hadn't paid your guards gave chase";
					const int ESCAPE = 3;
					switch (g_Dice%ESCAPE)
					{
					case 0:
						sexMessage << ", but she was already gone";
						break;
					case 1:
						sexMessage << " and caught her at the door. They surrounded her and demanded she turn around. She did. Your guards woke up minutes later with little idea what had happened";
						break;
					case 2:
						sexMessage << ". They actually caught her, but for reasons none could explain, they let her go";
						break;
					default:
						sexMessage << " and what happened next can never be spoken of (Error)";
						break;
					}
					sexMessage << ".\n";
				}
				sexMessage << "\nRetrieving " << girlName << ", the handler found her rocking and making a strange noise.\nNext to her, a bundle of gold.\n \n";
				customer->m_Stats[STAT_HAPPINESS] += 50;
			}
			else
			{
				sexMessage << "This slutty young woman was involved in some kind of bet that her mindblowing oral could even make an undead girl come.\n";
				sexMessage << "She spread the zombie's legs, slipped her tongue into " << girlName << "'s cold pussy and";
				if (zLezroll < 95)
				{
					sexMessage << " almost immediately threw up at the taste. She won't try that again.\n";
					customer->m_Stats[STAT_HAPPINESS] -= 50;
				}
				else if (zLezroll < 87)
				{
					sexMessage << " retched repeatedly, complaining about the 'taste,' and gave it up for impossible after a few minutes\n";
					customer->m_Stats[STAT_HAPPINESS] -= 30;
				}
				else
				{
					sexMessage << " warmed it up with her expert licks. Despite some early gagging, the customer ate her out for a long time. " << girlName << " just sat motionless with a slightly confused expression.\n";
					customer->m_Stats[STAT_HAPPINESS] -= 10;
				}
				break;
			}
		}

		//Gondra: reworking this part with choice variable
		if (check < 20)		//Gondra: the girl is unskilled
		{
			if (girl->has_trait("Lesbian"))	//Gondra: Trait messages
			{
				sexMessage << girlName << " was a bit too enthused about getting a female customer, fumbling quite a bit between the customers legs, including an unfortunate contact between her teeth and the clit she was sucking on.";
			}
			else if (girl->has_trait("Farmers Daughter") && !girl->has_trait("Lesbian"))
			{
				sexMessage << girlName << " looked a bit perplexed when she saw that her customer was a woman. The customer needed to push " << girlName << "'s head between her legs to get her to work instead of looking around dumbfounded like a cow.";
			}
			else if (choice < 40)	//Gondra: Vanilla Messages TODO Gondra: Replace/supplement these Vanilla messages.
			{
				sexMessage << girlName << " mechanically worked the customers pussy, barely managing to satisfy her.";
			}
			else if (choice < 80)
			{
				sexMessage << girlName << " managed to make the female customer cum, but seemed distressed about where she was touched by a fellow woman.";
			}
			else
			{
				sexMessage << girlName << " licked her female customer's cunt until she came. She didn't want any herself.";
			}
		}
		else if (check < 40) //Gondra:  if the girl is slightly skilled
		{
			if (girl->has_trait("Dick-Sucking Lips")) //Gondra: Trait messages
			{
				sexMessage << "The customer enjoyed feeling " << girlName << "'s lips run over her body, especially when they were sucking on her nipples or clit, elicting an orgasm from the customer after a few directions.";
			}
			else if (choice < 20)	//Gondra: Vanilla Messages
			{
				sexMessage << girlName << " enjoyed herself a little bit as her hands and tongue made her customer cum.";
			}
			else if (choice < 40)
			{
				sexMessage << girlName << " didn't seem to mind her customers hands drifting over her body as she brought her to orgasm.";
			}
			else if (choice < 60)
			{
				sexMessage << girlName << " was a bit uncomfortable with herself being visibly aroused after servicing her customer.";
			}
			else if (choice < 80)
			{
				sexMessage << girlName << " certainly was surprised that her customer insisted on being the one licking and rubbing, but enjoyed the attention and gave back as well as she could.";
			}
			else
			{
				sexMessage << girlName << " was aroused as she made her female customer cum.";
			}
		}
		else if (check <60) //Gondra: the girl is reasonably skilled
		{
			if (girl->has_trait("Straight"))
			{
				sexMessage << "Although it doesn't do anything for her, " << girlName << " made the woman buying her service happy without a problem, ";
				if (girl->has_trait("Fake Orgasm Expert"))
				{
					sexMessage << "believably faking an orgasm as her customer returned the favor.";
					customer->m_Stats[STAT_HAPPINESS] += 5;
				}
				else
				{
					sexMessage << "declining the customer's offer to return the favor without being rude.";
				}
			}
			else if (girl->has_trait("Good Kisser")) //Gondra: Trait messages
			{
				sexMessage << girlName << " managed to elicit the first few moans from her customer just placing a handful kisses on her neck, gradually traveling down her customers body, teasing her for quite a while before finally making her cum.";
			}
			else if (choice < 33)	//Gondra: Vanilla Messages
			{
				sexMessage << girlName << " and her customer both came as they rubbed their bodies against each other.";
			}
			else if (choice < 66)
			{
				sexMessage << "Both of their faces had a satisfied look to them when " << girlName << " and her customer were done.";
			}
			else
			{
				sexMessage << girlName << " fucked and was fucked by her female customer.";;
			}
		}
		else if (check <80) //Gondra: the girl is very skilled
		{
			if (girl->has_trait("Lesbian")) //Gondra: Trait messages
			{
				sexMessage << girlName << "'s customer moaned loudly the first time before she even dropped a single piece of clothing, the first orgasm audible soon after, quickly followed by several more, before" << girlName << " is heard cumming for the first time. In the end the customer walks away with quivering knees, exhausted but practically glowing with happiness.";
			}
			else if (choice < 33)	//Gondra: Vanilla Messages
			{
				sexMessage << girlName << " seemingly had a blast with her customer as both their moans were quite loud.";
			}
			else if (choice < 66)
			{
				sexMessage << girlName << " managed to make her partner shriek loudly several times as she tickled multiple orgasms out of her customer.";
			}
			else
			{
				sexMessage << girlName << " and her female customer's cumming could be heard thoughout the building.";
			}
		}
		else //Gondra: the girl is EXTREMELY skilled
		{
			//Gondra: again with the function
			sexMessage << girlName << GetRandomLesString();
		}
		message += sexMessage.str();
	}break; //End of SKILL_LESBIAN Case


	case SKILL_STRIP:
	default:
	{
		if (z)
		{
			sexMessage << "While Zombies don't generally care about clothes, " << girlName << " did not so much \"strip\" as tear her clothes off.\n";
			break;
		}

		if (check < 20)
		{
			if (SheAintPretty && g_Dice.percent(60)) sexMessage << girlName << " was self-conscious of her looks and gave a weak performance.";
			else if (g_Dice.percent(30)) sexMessage << girlName << " shyly took her clothes off in front of the customer.";
			else /*              */	sexMessage << girlName << " stared at the floor as she stood there awkwardly taking off her clothes in front of the customer. She was embarrassed and kept covering herself with her arms and hands.";
		}
		else if (check < 40)
		{
			if (girl->is_pregnant() && g_Dice.percent(60)) sexMessage << girlName << " stripped off well, despite her pregnancy.";
			else if (g_Dice.percent(30)) sexMessage << girlName << " coyly took her clothes off in front of the customer.";
			else /*              */	sexMessage << girlName << " made occasional eye contact as she coyly took her clothes off in front of the customer, moving around a little so the customer could see better.";
		}
		else if (check < 60)
		{
			if (SheAintPretty && g_Dice.percent(60)) sexMessage << girlName << " isn't the most beautiful, but gave a good enough show that the customer didn't notice.";
			else if (g_Dice.percent(30)) sexMessage << girlName << " hotly took her clothes off in front of the customer.";
			else /*              */	sexMessage << girlName << " moved around and stripped off her clothes in front of the customer.";
		}
		else if (check < 80)
		{
			if (girl->is_pregnant() && g_Dice.percent(60)) sexMessage << girlName << " stripped off incredibly well, despite the bulge in her belly.";
			else if (GirlGotNoClass && g_Dice.percent(40)) sexMessage << girlName << " clearly enjoyed taking everything off in front of the seated customer, and masturbated right in front of his face.";
			else if (g_Dice.percent(30)) sexMessage << girlName << " proudly took her clothes off in front of the customer.";
			else /*              */	sexMessage << girlName << " sexily danced around the customer stripping off her clothes.";
		}
		else
		{
			if (SheAintPretty && g_Dice.percent(50)) sexMessage << girlName << " gave a such an energetic, joyous and sensual strip-show that the customer didn't even notice her plain face.";
			else if (GirlGotNoClass && g_Dice.percent(40))
			{
				sexMessage << girlName << " was clearly aroused stripping everything off right in front of the seated customer";
				switch (g_Dice % 4)
				{
				case 0:
					sexMessage << ", fisting herself to orgasm inches from his nose.";
					break;
				case 1:
					sexMessage << " before bending over and fingering both holes right in front of his face.";
					break;
				case 2:
					sexMessage << " and stuffing her tiny, cotton panties up her vagina, before inviting the customer to pull back them out.";
					break;
				case 3:
					sexMessage << ". She then took a few steps back, spread her legs, and pissed on her hands - afterwards licking and sucking her fingers in front of the astonished customer.";
					break;
				default:
					sexMessage << " and doing some pr(E)tty dirty stuff in front of him.";
					break;
				}

			}
			else if (g_Dice.percent(30)) sexMessage << girlName << " joyously took her clothes off in front of the customer.";
			else /*              */	sexMessage << girlName << " sensuously prowled around the customer stripping off her clothes, while caressing herself, always making sure the customer had the best possible view.";
		}
		message += sexMessage.str(); //Gondra: add our sexMessage to our message string
	}break; //End of SKILL_STRIP Case
	}	//end switch




	// WD:	customer HAPPINESS changes complete now cap the stat to 100
	customer->m_Stats[STAT_HAPPINESS] = min(100, (int)customer->m_Stats[STAT_HAPPINESS]);

	message += (SexType == SKILL_GROUP) ? "\nThe customers " : "\nThe customer ";
	/* */if (customer->m_Stats[STAT_HAPPINESS] > 99)	message += "swore it was the best ever.";
	else if (customer->m_Stats[STAT_HAPPINESS] > 80)	message += "swore they would come back.";
	else if (customer->m_Stats[STAT_HAPPINESS] > 60)	message += "enjoyed the experience.";
	else if (customer->m_Stats[STAT_HAPPINESS] > 50)	message += "has had a better experience before."; //added this CRAZY
	else if (customer->m_Stats[STAT_HAPPINESS] > 40)	message += "thought it was okay."; //added this CRAZY
	else if (customer->m_Stats[STAT_HAPPINESS] > 30)	message += "didn't enjoy it.";
	else /*                                       */	message += "thought it was crap.";

	// WD: update Fame based on Customer HAPPINESS
	girl->upd_stat(STAT_FAME, (customer->m_Stats[STAT_HAPPINESS] - 1) / 33);

	// The girls STAT_CONSTITUTION and STAT_AGILITY modify how tired she gets
	AddTiredness(girl);

	// if the girl likes sex and the sex type then increase her happiness otherwise decrease it
	if (girl->has_trait("Succubus"))
	{
		message += "\nIt seems that she lives for this sort of thing.";//succubus does live for sex lol.. Idk if this will work like i want it to CRAZY
		girl->upd_stat(STAT_HEALTH, 10);//Idk where I should put this really but succubus gain live force or whatever from sex
		girl->upd_stat(STAT_HAPPINESS, 5);
	}
	else if (girl->libido() > 5)
	{
		/* */if (check < 20)	message += "\nThough she had a tough time with it, she was horny and still managed to gain some little enjoyment.";
		else if (check < 40)	message += "\nShe considered it a learning experience and enjoyed it a bit.";
		else if (check < 60)	message += "\nShe enjoyed it a lot and wanted more.";
		else if (check < 80)	message += "\nIt was nothing new for her, but she really does appreciate such work.";//girl->upd_stat(STAT_SANITY, 1);
		else /*           */	message += "\nIt seems that she lives for this sort of thing.";//girl->upd_stat(STAT_SANITY, 2);
		girl->upd_stat(STAT_HAPPINESS, girl->libido() / 5);
	}
	else
	{
		if (check < 30)
		{
			message += "\nShe wasn't in the mood at all and didn't enjoy being used this way.";
			girl->upd_stat(STAT_HAPPINESS, -2);
			//girl->upd_stat(STAT_SANITY, -1);
		}
		else if (check < 60)
		{
			message += "\nShe wasn't really in the mood.";
			girl->upd_stat(STAT_HAPPINESS, -1);
		}
		else
		{
			message += "\nShe didn't need this right now, but was happy enough to do it for the ";
			message += (customer->m_IsWoman ? "girl." : "guy.");
		}
	}

	// special cases for certain sex types
	switch (SexType)
	{
	case SKILL_ANAL:
	{
		if (z)
		{
			message += "(Z text not done)\n";
			//break;
		}
		if (check <= 20)	// if unexperienced then will get hurt
		{
			if (g_Dice.percent(30)) message += "\nHer inexperience hurt her a little.";
			else /*              */	message += "\nHer inexperience hurt her. It's now quite painful to sit down.";
			girl->upd_stat(STAT_HAPPINESS, -3);
			girl->upd_stat(STAT_CONFIDENCE, -1);
			girl->upd_stat(STAT_SPIRIT, -3);
			girl->upd_stat(STAT_HEALTH, -3);
			//girl->upd_stat(STAT_SANITY, -3);
		}
		girl->upd_temp_stat(STAT_LIBIDO, -10, true);
		girl->upd_stat(STAT_SPIRIT, -1);
		STDchance += 30;

	 //SIN - GIFT DROP
		if (g_Dice.percent(5) && customer->m_Stats[STAT_HAPPINESS] > 75)
		{
			bool keep = false;
			message += ("\n \nAfterwards he stuffed a toy up " + girlName + "'s cum lubricated ass, telling her it was a 'gift.' ");
			if (girl->has_trait( "Nymphomaniac"))
			{
				message += "She kept it in while she dressed and tidied up. She'll definitely keep ";
				keep = true;
			}
			else if (g_Dice.percent(girl->dignity())) //higher dig = higher chance
			{
				message += "As soon as he was gone, she pulled out the stinking thing and threw it in the trash.";
			}
			else
			{
				message += "She decided to keep ";
				keep = true;
			}
			if (keep)
			{

				if (g_Dice.percent(66)) girl->add_inv(g_InvManager.GetItem("Buttplug")), message += "this buttplug.";
				else girl->add_inv(g_InvManager.GetItem("Anal Beads")), message += "these anal beads.";
			}
		}
	}break;

	case SKILL_BDSM:
	{
		if (z)
		{
			message += "(Z text not done)\n";
			//break;
		}
		if (check <= 30)	// if unexperienced then will get hurt
		{
			if (g_Dice.percent(30)) message += "\nHer inexperience hurt her a little.";
			else /*              */	message += "\nHer inexperience hurt her a little. She's not used to having pain in those places.";
			girl->upd_stat(STAT_HAPPINESS, -2);
			girl->upd_stat(STAT_SPIRIT, -3);
			girl->upd_stat(STAT_CONFIDENCE, -1);
			girl->upd_stat(STAT_HEALTH, -3);
			//girl->upd_stat(STAT_SANITY, -5);
		}
		if (!customer->m_IsWoman)
		{
			contraception = girl->calc_pregnancy(*customer, false, 0.75);
			STDchance += (contraception ? 2 : 20);
		}

		girl->upd_temp_stat(STAT_LIBIDO, -5, true);
		girl->upd_stat(STAT_SPIRIT, -1);

	 //SIN - GIFT DROP
		if (g_Dice.percent(5) && customer->m_Stats[STAT_HAPPINESS] > 75)
		{
			bool keep = false;
			message += "\n \nAs he untied " + girlName + ", he told her she could keep the collar. ";
			if (girl->has_trait( "Masochist") || girl->has_trait( "Slut"))
			{
				message += "She agreed. It's exactly what she deserves: a ";
				keep = true;
			}
			else if (g_Dice.percent(girl->dignity())) //higher dig = higher chance
			{
				message += "\nA collar?! As soon as he was gone, she ripped it off and threw it in the trash.";
			}
			else
			{
				message += "She decided to keep this ";
				keep = true;
			}
			if (keep)
			{
				if (g_Dice.percent(66)) girl->add_inv(g_InvManager.GetItem("Spiked Collar")), message += "spiked collar.";
				else girl->add_inv(g_InvManager.GetItem("Slut Collar")), message += "'slut' collar.";
			}
		}
	}break;

	case SKILL_NORMALSEX:
	{
		if (z)
		{
			message += "(Z text not done)\n";
			//break;
		}
		if (check < 15)
		{
			if (g_Dice.percent(30)) message += "\nHer inexperience hurt her a little.";
			else /*              */	message += "\nShe's inexperienced and got poked in the eye.";/*Wouldnt this work better in oral? CRAZY*/
			girl->upd_stat(STAT_HAPPINESS, -2);
			girl->upd_stat(STAT_SPIRIT, -3);
			girl->upd_stat(STAT_CONFIDENCE, -1);
			girl->upd_stat(STAT_HEALTH, -3);
			//girl->upd_stat(STAT_SANITY, -2);
		}
		// if they're both happy afterward, it's good sex which modifies the chance of pregnancy
		good = (customer->happiness() >= 60 && girl->happiness() >= 60);
		if (girl->has_trait("Sterile"))
		{
			contraception = false;							// none needed
			STDchance += 16;								// less chance than not using but more chance than using
		}
		//Trait modifications
		else if (girl->has_trait("Cum Addict") && girl->m_UseAntiPreg &&
			g_Dice.percent(girl->libido()) && !g_Dice.percent(girl->intelligence()))
		{
			message += "\n \nShe got over-excited by her desire for cum, and failed to use her anti-preg. ";
			girl->m_UseAntiPreg = false;	// turn off anti
			contraception = girl->calc_pregnancy(*customer, good);
			STDchance += (contraception ? 4 : 40);
			if (contraception) message += "Luckily she didn't get pregnant.\n";
			else message += "Sure enough, she got pregnant.";
			girl->m_UseAntiPreg = true;		// return to normal (as checked in initial if condition)
		}
		else
		{
			contraception = girl->calc_pregnancy(*customer, good);
			STDchance += (contraception ? 4 : 40);
		}
		girl->upd_temp_stat(STAT_LIBIDO, -15, true);

	 //SIN - GIFT DROP
		if (g_Dice.percent(5) && customer->m_Stats[STAT_HAPPINESS] > 75)
		{
			bool keep = false;
			message += "\n \nAfterwards he squeezed a toy into " + girlName + "'s cummy cunt, leaving it there as a 'gift.' ";
			if (girl->has_trait( "Nymphomaniac"))
			{
				message += "While he was getting dressed, she noisily tried out the new ";
				keep = true;
			}
			else if (g_Dice.percent(girl->dignity())) //higher dig = higher chance
			{
				message += "As soon as he was gone, she pulled out the filthy thing and threw it in the trash.";
			}
			else
			{
				message += "She decided to keep ";
				keep = true;
			}
			if (keep)
			{
				if (g_Dice.percent(66)) girl->add_inv(g_InvManager.GetItem("Dildo")), message += "dildo.";
				else girl->add_inv(g_InvManager.GetItem("Studded Dildo")), message += "studded dildo.";
			}
		}
	}break;

	case SKILL_ORALSEX:
	{
		if (z)
		{
			message += "(Z text not done)\n";
			//break;
		}
		if (girl->get_skill(SexType) <= 20)	// if unexperienced then will get hurt
		{
			if (girl->has_trait("Gag Reflex") || girl->has_trait("Strong Gag Reflex"))
			{
				message += "\nHer throat is raw from gagging on the customer's cock. She was nearly sick.";
				girl->upd_stat(STAT_HAPPINESS, -4);
				girl->upd_stat(STAT_SPIRIT, -3);
				girl->upd_stat(STAT_CONFIDENCE, -1);
				girl->upd_stat(STAT_HEALTH, -3);
				//girl->upd_stat(STAT_SANITY, -3);
			}
			else
			{
				if (g_Dice.percent(40)) message += "\nHer inexperience caused her some embarrassment."; // Changed... being new at oral doesn't hurt, but can be embarrasing. --PP
				else /*              */	message += "\nShe's inexperienced and got poked in the eye.";/*CRAZY*/
				girl->upd_stat(STAT_HAPPINESS, -2);
				girl->upd_stat(STAT_SPIRIT, -3);
				girl->upd_stat(STAT_CONFIDENCE, -1);
				//girl->upd_stat(STAT_SANITY, -1);
			}
		}
		STDchance += 10;
		girl->upd_temp_stat(STAT_LIBIDO, -2, true);

	 //SIN - GIFT DROP
		if (g_Dice.percent(5) && customer->m_Stats[STAT_HAPPINESS] > 75)
		{
			bool keep = false;
			message += "\n \nAfterwards he gave " + girlName + " a gift to help her give head in future. She got ";
			if (g_Dice.percent(50)) girl->add_inv(g_InvManager.GetItem("Oral Sex Candy")), message += "some delicious oral sex candies.";
			else girl->add_inv(g_InvManager.GetItem("Knee Pads")), message += "some comfortable knee pads.";
		}
	}break;

	case SKILL_TITTYSEX:
	{
		if (z)
		{
			message += "(Z text not done)\n";
			//break;
		}
		if (check <= 20)	// if unexperienced then will get hurt
		{
			message += "\nHer inexperience caused her some embarrassment.";	// Changed... being new at oral doesn't hurt, but can be embarrasing. --PP
			girl->upd_stat(STAT_HAPPINESS, -2);
			girl->upd_stat(STAT_SPIRIT, -3);
			girl->upd_stat(STAT_CONFIDENCE, -1);
			//girl->upd_stat(STAT_SANITY, -1);
		}
		STDchance += 1;
		girl->upd_temp_stat(STAT_LIBIDO, -2, true);
	}break;

	case SKILL_HANDJOB:
	{
		if (z)
		{
			message += "(Z text not done)\n";
			//break;
		}
		if (check <= 20)	// if unexperienced then will get hurt
		{
			message += "\nHer inexperience caused her some embarrassment.";	// Changed... being new at handjob doesn't hurt, but can be embarrasing. --PP
			girl->upd_stat(STAT_HAPPINESS, -2);
			girl->upd_stat(STAT_SPIRIT, -3);
			girl->upd_stat(STAT_CONFIDENCE, -1);
		}
		STDchance += 1;
		girl->upd_temp_stat(STAT_LIBIDO, -1, true);
	}break;

	case SKILL_FOOTJOB:
	{
		if (z)
		{
			message += "(Z text not done)\n";
			//break;
		}
		if (check <= 20)	// if unexperienced then will get hurt
		{
			message += "\nHer inexperience caused her some embarrassment.";	// Changed... being new at footjob doesn't hurt, but can be embarrasing. --PP
			girl->upd_stat(STAT_HAPPINESS, -2);
			girl->upd_stat(STAT_SPIRIT, -3);
			girl->upd_stat(STAT_CONFIDENCE, -1);
		}
		STDchance += 1;
		girl->upd_temp_stat(STAT_LIBIDO, -1, true);
	}break;

	case SKILL_BEASTIALITY:
	{
		if (z)
		{
			message += "(Z text not done)\n";
			//break;
		}
		if (check <= 30)	// if unexperienced then will get hurt
		{
			message += "\nHer inexperience hurt her a little.";
			girl->upd_stat(STAT_HAPPINESS, -2);
			girl->upd_stat(STAT_SPIRIT, -3);
			girl->upd_stat(STAT_CONFIDENCE, -1);
			girl->upd_stat(STAT_HEALTH, -3);
			//girl->upd_stat(STAT_SANITY, -4);
		}
		girl->upd_stat(STAT_SPIRIT, -1);	// is pretty degrading
		// if they're both happy afterward, it's good sex which modifies the chance of pregnancy
		good = (customer->happiness() >= 60 && girl->happiness() >= 60);
		// mod: added check for number of beasts owned; otherwise, fake beasts could somehow inseminate the girl
		if (g_Brothels.GetNumBeasts() > 0)
		{
			contraception = girl->calc_insemination(*GetBeast(), good);
			STDchance += (contraception ? 2 : 20);
		}
		girl->upd_temp_stat(STAT_LIBIDO, -10, true);

	//SIN - GIFT DROP
		if (g_Dice.percent(5) && customer->m_Stats[STAT_HAPPINESS] > 50)
		{
			bool keep = true;
			if (g_Dice.percent(90))
			{
				message += "\n \nAfterward the customer gave " + girlName + " a pet collar as a gift.";
				if (g_Dice.percent(girl->dignity())) //higher dig = higher chance
				{
					message += "Annoyed, she later threw out this demeaning trash.";
					keep = false;
				}
				if (keep) girl->add_inv(g_InvManager.GetItem("Pet Collar"));
			}
			else
			{
				message += "\n \nAfterward the customer gave " + girlName + " some cute Paw-Print Teddy lingerie as a gift.";
				girl->add_inv(g_InvManager.GetItem("Paw-Print Teddy"));
			}
		}
	}break;

	case SKILL_GROUP:
	{
		if (z)
		{
			message += "(Z text not done)\n";
			//break;
		}
		if (check <= 30)	// if unexperienced then will get hurt
		{
			if (g_Dice.percent(30)) message += "\nHer inexperience hurt her a little.";
			else /*              */	message += "\nShe's inexperienced and got rubbed pretty raw.";
			girl->upd_stat(STAT_HAPPINESS, -2);
			girl->upd_stat(STAT_SPIRIT, -3);
			girl->upd_stat(STAT_CONFIDENCE, -1);
			girl->upd_stat(STAT_HEALTH, -3);
			//girl->upd_stat(STAT_SANITY, -3);
		}
		// if they're both happy afterward, it's good sex which modifies the chance of pregnancy
		good = (customer->happiness() >= 60 && girl->happiness() >= 60);
		if (girl->has_trait("Sterile"))
		{
			contraception = false;							// none needed
			STDchance += ((4 + customer->m_Amount) * 4);	// less chance than not using but more chance than using
		}
		//trait variation
		else if (girl->has_trait("Cum Addict") && girl->m_UseAntiPreg &&
			g_Dice.percent(girl->libido()) && !g_Dice.percent(girl->intelligence()))
		{
			message += "\n \nShe got over-excited by her desire for cum, and failed to use her anti-preg. ";
			girl->m_UseAntiPreg = false;	// turn off anti
			contraception = girl->calc_group_pregnancy(*customer, good, 1.5);
			STDchance += ((4 + customer->m_Amount) * (contraception ? 1 : 10));
			if (contraception) message += "Luckily she didn't get pregnant.\n";
			else message += "Sure enough, she got pregnant.";
			girl->m_UseAntiPreg = true;		// return to normal (as checked in initial if condition)
		}
		else
		{
			//50% bonus to the chance of pregnancy since there's more than one partner involved
			contraception = girl->calc_group_pregnancy(*customer, good, 1.5);
			STDchance += ((4 + customer->m_Amount) * (contraception ? 1 : 10));
		}
	 //GIFT DROP
		if (g_Dice.percent(5) && customer->m_Stats[STAT_HAPPINESS] > 50)
		{
			if (g_Dice.percent(90))
			{
				message += "\n \nAfter the group had left, " + girlName + " found they had filled her decorative goblet with their cum. ";
				if (girl->has_trait( "Cum Addict"))
				{
					message += "She immediately swallowed it all down.";
				}
				else if (g_Dice.percent(girl->dignity())) //higher dig = higher chance
				{
					message += "Annoyed, she threw the goblet, and its disgusting contents, in the trash.";
				}
				else
				{
					message += "She decided to keep it.";
					girl->add_inv(g_InvManager.GetItem("Goblet of Cum"));
				}
			}
			else if (customer->m_Stats[STAT_HAPPINESS] > 90)
			{
				girl->add_inv(g_InvManager.GetItem("Ring of the Hivemind"));
				message += "\n \nThe grateful group were so exhausted they forgot to take their Ring of Hivemind. It's hers now.";
			}
			else
			{
				girl->add_inv(g_InvManager.GetItem("Herpes Cure"));
				message += "\n \nWorryingly, as she tidied up she found a Herpes Cure dropped under the bed. It's hers now.";
			}
		}
		girl->upd_temp_stat(STAT_LIBIDO, -20, true);
	}break;

	case SKILL_LESBIAN:
	{
		if (z)
		{
			message += "(Z text not done)\n";
			//break;
		}
		if (check <= 20)	// if unexperienced then will get hurt
		{
			message += "\nHer inexperience caused her some embarrassment.";	// Changed... being new at lesbian doesn't hurt, but can be embarrasing. --PP
			girl->upd_stat(STAT_HAPPINESS, -2);
			girl->upd_stat(STAT_SPIRIT, -3);
			girl->upd_stat(STAT_CONFIDENCE, -1);
			//girl->upd_stat(STAT_SANITY, -1);
		}
		STDchance += 5;
		girl->upd_temp_stat(STAT_LIBIDO, -10, true);

	 //SIN - GIFT DROP
		if (g_Dice.percent(5) && customer->m_Stats[STAT_HAPPINESS] > 75)
		{
			bool keep = true;
			if (g_Dice.percent(90))
			{
				message += "\n \nAfterwards the woman let " + girlName + " keep the double-dildo they had shared.";
				if (girl->has_trait( "Straight"))
				{
					message += "\nBeing straight, she had no desire to keep it and threw it out later.";
					keep = false;
				}
				if (keep) girl->add_inv(g_InvManager.GetItem("Dildo"));
			}
			else
			{
				message += "\n \nThe woman gave " + girlName + " an expensive, illustrated book on lesbian sex.";
				if (girl->has_trait( "Straight"))
				{
					message += "\nBeing straight, she found it kinda gross and threw it out.";
					keep = false;
				}
				if (keep) girl->add_inv(g_InvManager.GetItem("Manual of Two Roses"));
			}
		}
	}break;

	case SKILL_STRIP:
	default:
	{
		if (z)
		{
			message += "(Z text not done)\n";
			//break;
		}
		if (check <= 20)	// if inexperienced then will get hurt
		{
			message += "\nShe got tangled in her clothes and fell on her face.";
			girl->upd_stat(STAT_HAPPINESS, -2);
			girl->upd_stat(STAT_SPIRIT, -3);
			girl->upd_stat(STAT_CONFIDENCE, -1);
			girl->upd_stat(STAT_HEALTH, -3);
		}
		STDchance += 0;
		girl->upd_temp_stat(STAT_LIBIDO, 0, true);
	}break;
	}	// end switch

	// lose virginity unless it was anal sex -- or lesbian, or Oral also customer is happy no matter what. -PP
	if (girl->check_virginity())
	{
		bool virgincheck = false;
		int chappy = 0;
		if (SexType == SKILL_ORALSEX || SexType == SKILL_TITTYSEX || SexType == SKILL_HANDJOB || SexType == SKILL_FOOTJOB)
		{
			// virginity does not matter at all
			chappy = 2;	// but lets give a little anyway
		}
		else if (SexType == SKILL_STRIP)
		{
			message += " The customer was excited to have a virgin stripping for them.";
			chappy = 10;
			virgincheck = true;
		}
		else if (SexType == SKILL_BEASTIALITY)
		{
			message += " The customer was excited that she was a virgin as he pushed the beast onto her.";
			chappy = 25;
			virgincheck = true;
		}
		else if (SexType == SKILL_ANAL)
		{
			message += " The customer was overjoyed that she was a virgin, but that was not what he was into.";
			chappy = 50;
		}
		else if (SexType == SKILL_LESBIAN)
		{
			message += " The customer was happy that she had never been with a \"man\" and was still a virgin.";
			chappy = 69;
		}
		else if (SexType == SKILL_GROUP)
		{
			message += " The customers were overjoyed that she was a virgin. Well... the first one was at least.";
			chappy = 80;
			virgincheck = true;
		}
		else if (SexType == SKILL_BDSM)
		{
			message += " The customer was overjoyed that she was a virgin.";
			chappy = 90;
			if (g_Dice.percent(70)) virgincheck = true;
		}
		else if (SexType == SKILL_NORMALSEX)
		{
			message += " The customer was overjoyed that she was a virgin.";
			chappy = 100;
			virgincheck = true;
		}
		if (chappy > 0) customer->m_Stats[STAT_HAPPINESS] += chappy;
		if (girl->is_pregnant()) virgincheck = true;
		if (virgincheck) girl->lose_virginity();
	}

	//SIN - poor accomodation minus...
	if (g_Dice.percent(3) && girl->m_AccLevel < 2)
	{
		message += "\n \nOn the way out, the customer mentioned " + girlName + "'s \"awful personal hygiene\" to one of your staff.\n";
		switch (g_Dice % 5)
		{
		case 0:
			message += "\"She has cum-dreadlocks in her hair!\"";
			break;
		case 1:
			message += "\"The stink when I took her panties off...\"";
			break;
		case 2:
			message += "\"Her breath stank of cock.\"";
			break;
		case 3:
			message += "\"There were stains all over her clothes and bed.\"";
			break;
		case 4:
			message += "\"Her kiss tasted like horse cum. And do NOT ask how I know what that tastes like...\"";
			break;
		default:
			message += "\"Dirty girl. And not in a pl(E)asant way.\""; //(E)rror
			break;
		}
		message += "\nIt's not really her fault: the room/closet you have her living in doesn't even have a sink.";
		customer->m_Stats[STAT_HAPPINESS] -= 15;
	}

	// Now calculate other skill increases
	int skillgain = 4;	int exp = 5;
	if (girl->has_trait("Quick Learner"))		{ skillgain += 1; exp += 2; }
	else if (girl->has_trait("Slow Learner"))	{ skillgain -= 1; exp -= 2; }
	if (SexType == SKILL_GROUP)
	{
		girl->upd_skill(SKILL_ANAL, max(0, g_Dice % skillgain + 1));
		girl->upd_skill(SKILL_BDSM, max(0, g_Dice % skillgain - 1));
		girl->upd_skill(SKILL_NORMALSEX, max(0, g_Dice % skillgain + 1));
		girl->upd_skill(SKILL_BEASTIALITY, max(0, g_Dice % skillgain - 3));
		girl->upd_skill(SKILL_GROUP, max(2, g_Dice % skillgain + 2));
		girl->upd_skill(SKILL_LESBIAN, max(0, g_Dice % skillgain - 2));
		girl->upd_skill(SKILL_STRIP, max(0, g_Dice % skillgain - 2));
		girl->upd_skill(SKILL_ORALSEX, max(0, g_Dice % skillgain + 0));
		girl->upd_skill(SKILL_TITTYSEX, max(0, g_Dice % skillgain - 1));
		girl->upd_skill(SKILL_HANDJOB, max(0, g_Dice % skillgain + 0));
		girl->upd_skill(SKILL_FOOTJOB, max(0, g_Dice % skillgain - 1));
	}
	else	// single sex act focus gets more base gain
	{
		girl->upd_skill(SexType, g_Dice % (skillgain + 2) + 1);
	}
	girl->upd_skill(SKILL_SERVICE, max(0, g_Dice % skillgain - 1));	// everyone gets some service gain
	girl->upd_stat(STAT_EXP, max(1, (g_Dice % (exp * 3))));

	int enjoy = 1;
	if (girl->has_trait("Nymphomaniac"))
	{
		girl->upd_temp_stat(STAT_LIBIDO, 15);		// she just had sex and she wants more
		switch (SexType)
		{
		case SKILL_GROUP:			enjoy += 3; break;
		case SKILL_ANAL:			enjoy += 2; break;
		case SKILL_NORMALSEX:		enjoy += 2; break;
		case SKILL_BDSM:
		case SKILL_BEASTIALITY:
		case SKILL_LESBIAN:			enjoy += 1; break;
			// Nymphomaniac would rather have something inside her so if she can't, she does not enjoy it as much
		case SKILL_STRIP:			enjoy -= 2; break;
		case SKILL_TITTYSEX:		enjoy -= 1; break;
		case SKILL_HANDJOB:			enjoy -= 1; break;
		case SKILL_FOOTJOB:			enjoy -= 1; break;
		case SKILL_ORALSEX:
		default:
			break;
		}
	}
	if (girl->has_trait("Lesbian"))
	{
		switch (SexType)
		{
		case SKILL_LESBIAN:			enjoy += 3; break;
		case SKILL_STRIP:			enjoy += 1; break;
			// Lesbian would rather not have sex with a male
		case SKILL_NORMALSEX:		enjoy -= 3; break;
		case SKILL_TITTYSEX:
		case SKILL_ORALSEX:			enjoy -= 1; break;
		case SKILL_HANDJOB:
		case SKILL_FOOTJOB:			enjoy -= 1; break;
		case SKILL_ANAL:
		case SKILL_GROUP:			enjoy -= 2; break;
		default:
			break;
		}
	}
	if (girl->has_trait("Straight"))
	{
		switch (SexType)
		{
		case SKILL_NORMALSEX:		enjoy += 2; break;
		case SKILL_LESBIAN:			enjoy -= 1; break;
		default:
			break;
		}
	}
	if (girl->has_trait("Cum Addict"))
	{
		switch (SexType)
		{
		case SKILL_ORALSEX:			enjoy += 5; break;
		case SKILL_GROUP:			enjoy += 5; break;
		default:
			break;
		}
	}

	if (girl->happiness() > 50)			enjoy += 2;
	else if (girl->happiness() <= 5)	enjoy -= 2;

	int health = girl->health();
	/* */if (health > 95)	{ STDchance = 1; }
	else if (health > 30)	{ STDchance /= (health - 25) / 5; }
	else /*if (health < 30)*/	{ STDchance *= (35 - health) / 10; } //SIN: check not required
	if (STDchance < 0.1)	STDchance = 0.1;

	if (girl->has_trait("AIDS") && !customer->m_HasAIDS && g_Dice.percent(STDchance))
	{
		girl->m_Events.AddMessage(girlName + " gave the customer AIDS! They are not happy about this.", IMGTYPE_PROFILE, EVENT_DANGER);
		customer->m_HasAIDS = true;
		customer->m_Stats[STAT_HAPPINESS] -= 100;
		enjoy -= 3;
	}
	else if (!girl->has_trait("AIDS") && customer->m_HasAIDS && g_Dice.percent(STDchance))
	{
		girl->m_Events.AddMessage(girlName + " has caught the disease AIDS! She will likely die, but a rare cure can sometimes be found in the shop.", IMGTYPE_PROFILE, EVENT_DANGER);
		girl->add_trait("AIDS");
		girl->happiness(-50);
		enjoy -= 30;
	}
	if (girl->has_trait("Chlamydia") && !customer->m_HasChlamydia && g_Dice.percent(STDchance))
	{
		girl->m_Events.AddMessage(girlName + " gave the customer Chlamydia! They are not happy about this.", IMGTYPE_PROFILE, EVENT_DANGER);
		customer->m_HasChlamydia = true;
		customer->m_Stats[STAT_HAPPINESS] -= 40;
		enjoy -= 3;
	}
	else if (!girl->has_trait("Chlamydia") && customer->m_HasChlamydia && g_Dice.percent(STDchance))
	{
		girl->m_Events.AddMessage(girlName + " has caught the disease Chlamydia! A cure can sometimes be found in the shop.", IMGTYPE_PROFILE, EVENT_DANGER);
		girl->add_trait("Chlamydia");
		girl->happiness(-30);
		enjoy -= 30;
	}

	if (girl->has_trait("Syphilis") && !customer->m_HasSyphilis && g_Dice.percent(STDchance))
	{
		girl->m_Events.AddMessage(girlName + " gave the customer Syphilis! They are not happy about this.", IMGTYPE_PROFILE, EVENT_DANGER);
		customer->m_HasSyphilis = true;
		customer->m_Stats[STAT_HAPPINESS] -= 50;
		enjoy -= 3;
	}
	else if (!girl->has_trait("Syphilis") && customer->m_HasSyphilis && g_Dice.percent(STDchance))
	{
		girl->m_Events.AddMessage(girlName + " has caught the disease Syphilis! This can be deadly, but a cure can sometimes be found in the shop.", IMGTYPE_PROFILE, EVENT_DANGER);
		girl->add_trait("Syphilis");
		girl->happiness(-30);
		enjoy -= 30;
	}

	if (girl->has_trait("Herpes") && !customer->m_HasHerpes && g_Dice.percent(STDchance))
	{
		girl->m_Events.AddMessage(girlName + " gave the customer Herpes! They are not happy about this.", IMGTYPE_PROFILE, EVENT_DANGER);
		customer->m_HasHerpes = true;
		customer->m_Stats[STAT_HAPPINESS] -= 30;
		enjoy -= 3;
	}
	else if (!girl->has_trait("Herpes") && customer->m_HasHerpes && g_Dice.percent(STDchance))
	{
		girl->m_Events.AddMessage(girlName + " has caught the disease Herpes! A cure can sometimes be found in the shop.", IMGTYPE_PROFILE, EVENT_DANGER);
		girl->add_trait("Herpes");
		girl->happiness(-30);
		enjoy -= 30;
	}

	girl->upd_Enjoyment(ACTION_SEX, enjoy);

	if (cfg.debug.log_debug())
	{
		g_LogFile.os() << "STD Debug ::: Sex Type : " << sGirl::skill_names[SexType]
			<< " :: Contraception: " << (contraception ? "True" : "False")
			<< " :: Health: " << health
			<< " :: STD Chance: " << STDchance
			<< endl;
	}
}

string cGirls::GetRandomSexString()
{
	int roll1 = 0, roll2 = 0, roll3 = 0, random = 0;
	stringstream OStr;
	// MYR: Can't resist a little cheeky chaos
	random = g_Dice % 500;
	if (random == 345)
	{
		OStr << " (phrase 1). (phrase 2) (phrase 3).";
		return OStr.str();
	}
	OStr << " ";  // Consistency
	// Roll #1
# pragma region sex1
	roll1 = g_Dice % 8 + 1;   // Remember to update this when new strings are added
	switch (roll1)
	{
	case 1:
		random = g_Dice % 10 + 1;
		/* */if (random <= 2)	OStr << ("straddled");
		else if (random <= 4)	OStr << ("cow-girled");
		else if (random <= 6)	OStr << ("wrapped her legs around");
		else if (random <= 8)	OStr << ("contorted her legs behind her head for");
		else /*            */	OStr << ("scissored");
		OStr << (" the client, because it ");
		random = g_Dice % 10 + 1;
		/* */if (random <= 2)	OStr << ("turned him on");
		else if (random <= 4)	OStr << ("made him crazy");
		else if (random <= 6)	OStr << ("gave him a massive boner");
		else if (random <= 8)	OStr << ("was more fun than talking");
		else /*            */	OStr << ("made him turn red");
		break;
	case 2:
		OStr << ("was told to grab ");
		random = g_Dice % 10 + 1;
		/* */if (random <= 2)	OStr << ("her ankles");
		else if (random <= 4)	OStr << ("the chair");
		else if (random <= 6)	OStr << ("her knees");
		else if (random <= 8)	OStr << ("the table");
		else /*            */	OStr << ("the railing");
		OStr << (" and ");
		random = g_Dice % 10 + 1;
		/* */if (random <= 2)	OStr << ("shook her hips");
		else if (random <= 4)	OStr << ("spread her legs");
		else if (random <= 6)	OStr << ("close her eyes");
		else if (random <= 8)	OStr << ("look away");
		else /*            */	OStr << ("bend waaaaayyy over");
		break;
	case 3:
		OStr << ("had fun with his ");
		random = g_Dice % 12 + 1;
		/* */if (random <= 2)	OStr << ("foot");
		else if (random <= 4)	OStr << ("stocking");
		else if (random <= 6)	OStr << ("hair");
		else if (random <= 8)	OStr << ("lace");
		else if (random <= 10)	OStr << ("butt");
		else /*            */	OStr << ("food");
		OStr << (" fetish and gave him an extended ");
		random = g_Dice % 6 + 1;
		/* */if (random <= 2)	OStr << ("foot");
		else if (random <= 4)	OStr << ("hand");
		else /*            */	OStr << ("oral");
		OStr << (" surprise");
		break;
	case 4:
		OStr << ("dressed as ");
		random = g_Dice % 10 + 1;
		/* */if (random <= 2)	OStr << ("a school girl");
		else if (random <= 4)	OStr << ("a nurse");
		else if (random <= 6)	OStr << ("a nun");
		else if (random <= 8)	OStr << ("an adventurer");
		else /*            */	OStr << ("a dominatrix");
		OStr << (" to grease ");
		random = g_Dice % 10 + 1;
		/* */if (random <= 2)	OStr << ("the little man");
		else if (random <= 4)	OStr << ("his pole");
		else if (random <= 6)	OStr << ("his tool");
		else if (random <= 8)	OStr << ("his fingers");
		else /*            */	OStr << ("his toes");
		break;
	case 5:
		OStr << ("decided to skip ");
		random = g_Dice % 8 + 1;
		/* */if (random <= 2)	OStr << ("the bed");
		else if (random <= 4)	OStr << ("foreplay");
		else if (random <= 6)	OStr << ("niceties");
		else /*            */	OStr << ("greetings");
		OStr << (" and assumed position ");
		random = g_Dice % 9999 + 1;
		OStr << random;
		break;
	case 6:
		OStr << ("gazed in awe at ");
		random = g_Dice % 10 + 1;
		/* */if (random <= 2)	OStr << ("how well hung he was");
		else if (random <= 4)	OStr << ("the time");
		else if (random <= 6)	OStr << ("his muscles");
		else if (random <= 8)	OStr << ("his handsome face");
		else /*            */	OStr << ("his collection of sexual magic items");
		OStr << (" and ");
		random = g_Dice % 8 + 1;
		/* */if (random <= 2)	OStr << ("felt inspired");
		else if (random <= 4)	OStr << ("played hard to get");
		else if (random <= 6)	OStr << ("squealed like a little girl");
		else /*            */	OStr << ("prepared for action");
		break;
	case 7: OStr << ("bent into ");
		random = g_Dice % 8 + 1;
		/* */if (random <= 2)	OStr << ("a delightful");
		else if (random <= 4)	OStr << ("an awkward");
		else if (random <= 6)	OStr << ("a difficult");
		else /*            */	OStr << ("a crazy");
		OStr << (" position and ");
		random = g_Dice % 8 + 1;
		/* */if (random <= 2)	OStr << ("squealed");
		else if (random <= 4)	OStr << ("moaned");
		else /*            */	OStr << ("grew hot");
		OStr << (" as he ");
		random = g_Dice % 8 + 1;
		/* */if (random <= 2)	OStr << ("touched");
		else if (random <= 4)	OStr << ("caressed");
		else /*            */	OStr << ("probed");
		OStr << (" her defenseless body");
		break;
	case 8:
		OStr << ("lay on the ");
		random = g_Dice % 6 + 1;
		/* */if (random <= 2)	OStr << ("floor");
		else if (random <= 4)	OStr << ("bed");
		else /*            */	OStr << ("couch");
		OStr << (" and ");
		random = g_Dice % 6 + 1;
		/* */if (random <= 2)	OStr << ("had him take off all her clothes");
		else if (random <= 4)	OStr << ("told him exactly what turned her on");
		else /*            */	OStr << ("encouraged him to take off her bra and panties with his teeth");
		break;
	}
# pragma endregion sex1
	// Roll #2
# pragma region sex2
	OStr << ". ";
	roll2 = g_Dice % 11 + 1;
	switch (roll2)
	{
	case 1:
		OStr << ("She ");
		random = g_Dice % 8 + 1;
		/* */if (random <= 2)	OStr << ("rode him all the way to the next town");
		else if (random <= 4)	OStr << ("massaged his balls and sucked him dry");
		else if (random <= 6)	OStr << ("titty fucked and sucked the well dry");
		else /*            */	OStr << ("fucked him blind");
		OStr << (". He was a trooper though and rallied: She ");
		random = g_Dice % 8 + 1;
		/* */if (random <= 2)	OStr << ("was deeply penetrated");
		else if (random <= 4)	OStr << ("was paralyzed with stunning sensations");
		else if (random <= 6)	OStr << ("bucked like a bronko");
		else /*            */	OStr << ("shook with pleasure");
		OStr << (" and ");
		random = g_Dice % 4 + 1;
		/* */if (random <= 2)	OStr << ("came like a fire hose from");
		else /*            */	OStr << ("repeatedly shook in orgasm with");
		break;
	case 2:
		OStr << ("It took a lot of effort to stay ");
		random = g_Dice % 10 + 1;
		/* */if (random <= 3)	OStr << ("interested in");
		else if (random <= 7)	OStr << ("awake for");
		else 	OStr << ("conscious for");
		break;
	case 3:
		OStr << ("She was fucked ");
		random = g_Dice % 10 + 1;
		/* */if (random <= 2)	OStr << ("blind");
		else if (random <= 4)	OStr << ("silly twice over");
		else if (random <= 6)	OStr << ("all crazy like");
		else if (random <= 8)	OStr << ("for hours");
		else /*            */	OStr << ("for minutes");
		OStr << (" by"); break;
	case 4:
		OStr << ("She performed ");
		random = g_Dice % 10 + 1;
		/* */if (random <= 5)	OStr << ("uninspired ");
		else /*            */	OStr << ("inspired ");
		random = g_Dice % 8 + 1;
		/* */if (random <= 2)	OStr << ("missionary ");
		else if (random <= 4)	OStr << ("oral ");
		else if (random <= 6)	OStr << ("foot ");
		else /*            */	OStr << ("hand ");
		OStr << ("sex for"); break;
	case 5:
		//OStr <<"";
		random = g_Dice % 10 + 1;
		/* */if (random <= 2)	OStr << ("Semen");
		else if (random <= 4)	OStr << ("Praise");
		else if (random <= 6)	OStr << ("Flesh");
		else if (random <= 8)	OStr << ("Drool");
		else /*            */	OStr << ("Chocolate sauce");
		OStr << (" rained down on her from");
		break;
	case 6:
		OStr << ("She couldn't ");
		random = g_Dice % 10 + 1;
		/* */if (random <= 2)	OStr << ("stand");
		else if (random <= 4)	OStr << ("walk");
		else if (random <= 6)	OStr << ("swallow");
		else if (random <= 8)	OStr << ("feel her legs");
		else /*            */	OStr << ("move");
		OStr << (" after screwing");
		break;
	case 7:
		OStr << ("It took a great deal of effort to look ");
		random = g_Dice % 10 + 1;
		/* */if (random <= 2)	OStr << ("interested in");
		else if (random <= 4)	OStr << ("awake for");
		else if (random <= 6)	OStr << ("alive for");
		else if (random <= 8)	OStr << ("enthusiastic for");
		else /*            */	OStr << ("hurt for");
		break;
	case 8:
		OStr << ("She played 'clean up the ");
		random = g_Dice % 8 + 1;
		/* */if (random <= 2)	OStr << ("tools");
		else if (random <= 4)	OStr << ("customer");
		else if (random <= 6)	OStr << ("sword");
		else /*            */	OStr << ("sugar frosting");
		OStr << ("' with");
		break;
	case 9:
		OStr << ("Hopefully her ");
		random = g_Dice % 10 + 1;
		/* */if (random <= 2)	OStr << ("cervix");
		else if (random <= 4)	OStr << ("pride");
		else if (random <= 6)	OStr << ("reputation");
		else if (random <= 8)	OStr << ("ego");
		else /*            */	OStr << ("stomach");
		OStr << (" wasn't bruised by");
		break;
	case 10:
		OStr << ("She called in ");
		random = g_Dice % 3 + 2;
		OStr << random;
		OStr << (" reinforcements to tame");
		break;
	case 11:
		OStr << ("She orgasmed ");
		random = g_Dice % 100 + 30;
		OStr << random;
		OStr << (" times with"); break;
	}
# pragma endregion sex2
	// Roll #3
# pragma region sex3
	OStr << " ";	// Consistency
	roll3 = g_Dice % 20 + 1;
	switch (roll3)
	{
	case 1:
		OStr << ("the guy ");
		random = g_Dice % 10 + 1;
		/* */if (random <= 2)	OStr << ("wearing three amulets of the sex elemental.");
		else if (random <= 4)	OStr << ("wearing eight rings of the horndog.");
		else if (random <= 6)	OStr << ("wearing a band of invulnerability.");
		else if (random <= 8)	OStr << ("carrying a waffle iron.");
		else /*            */	OStr << ("carrying a body probe of irresistable sensations.");
		break;
	case 2: OStr << ("Thor, God of Thunderfucking!!!!"); break;
	case 3:
		OStr << ("the frustrated ");
		random = g_Dice % 10 + 1;
		/* */if (random <= 2)	OStr << ("astronomer.");
		else if (random <= 4)	OStr << ("physicist.");
		else if (random <= 6)	OStr << ("chemist.");
		else if (random <= 8)	OStr << ("biologist.");
		else /*            */	OStr << ("engineer.");
		break;
	case 4: OStr << ("the invisible something or other????"); break;
	case 5: OStr << ("the butler. (He always did it.)"); break;
	case 6:
		OStr << ("the ");
		random = g_Dice % 10 + 1;
		/* */if (random <= 2)	OStr << "sentient apple tree.";
		else if (random <= 4)	OStr << ("sentient sex toy.");
		else if (random <= 6)	OStr << ("pan-dimensional toothbrush.");
		else if (random <= 8)	OStr << ("magic motorcycle.");
		else /*            */	OStr << ("regular bloke.");
		break;
	case 7:
		OStr << ("the unbelievably well behaved ");
		random = g_Dice % 8 + 1;
		/* */if (random <= 2)	OStr << ("Pink Petal forum member.");
		else if (random <= 4)	OStr << ("tentacle.");
		else if (random <= 6)	OStr << ("pirate.");
		else /*            */	OStr << ("sentient bottle.");
		break;
	case 8:
		random = g_Dice % 20 + 1;
		/* */if (random <= 2)	OStr << ("Cousin");
		else if (random <= 4)	OStr << ("Brother");
		else if (random <= 6)	OStr << ("Saint");
		else if (random <= 8)	OStr << ("Lieutenant");
		else if (random <= 10)	OStr << ("Master");
		else if (random <= 12)	OStr << ("Doctor");
		else if (random <= 14)	OStr << ("Mr.");
		else if (random <= 16)	OStr << ("Smith");
		else if (random <= 18)	OStr << ("DockMaster");
		else /*            */	OStr << ("Perfect");
		OStr << (" Parkins from down the street.");
		break;
	case 9: OStr << ("the master of the hidden dick technique. (Where is it? Nobody knows.)"); break;
	case 10: OStr << ("cake. It isn't a lie!"); break;
	case 11:
		OStr << ("the really, really macho ");
		random = g_Dice % 10 + 1;
		/* */if (random <= 2)	OStr << ("Titan.");
		else if (random <= 4)	OStr << ("Storm Giant.");
		else if (random <= 6)	OStr << ("small moon.");
		else if (random <= 8)	OStr << ("kobold.");
		else /*            */	OStr << ("madness.");
		break;
	case 12:
		OStr << ("the clockwork man!");
		OStr << (" (With no sensation in his clockwork ");
		random = g_Dice % 10 + 1;
		/* */if (random <= 2)	OStr << ("tool");
		else if (random <= 4)	OStr << ("head");
		else if (random <= 6)	OStr << ("fingers");
		else if (random <= 8)	OStr << ("attachment");
		else /*            */	OStr << ("clock");
		OStr << (" and no sense to ");
		random = g_Dice % 10 + 1;
		/* */if (random <= 2)	OStr << ("stop");
		else if (random <= 4)	OStr << ("slow down");
		else if (random <= 6)	OStr << ("moderate");
		else if (random <= 8)	OStr << ("be gentle");
		else
		{
			OStr << ("stop at ");
			random = g_Dice % 50 + 30;
			OStr << random;
			OStr << (" orgasms");
		}
		OStr << (".)");
		break;
	case 13:
		// MYR: This one gives useful advice to the players.  A gift from us to them.
		OStr << ("the Brothel Master developer. ");
		random = g_Dice % 20 + 1;
		/* */if (random <= 2)	OStr << ("(Quick learner is a great talent to have.)");
		else if (random <= 4)	OStr << ("(Don't ignore the practice skills option for your girls.)");
		else if (random <= 6)	OStr << ("(Train your gangs.)");
		else if (random <= 8)	OStr << ("(Every time you restart the game, the shop inventory is reset.)");
		else if (random <= 10)	OStr << ("(Invulnerable (incorporeal) characters should be exploring the catacombs.)");
		else if (random <= 12)	OStr << ("(High dodge gear is great for characters exploring the catacombs.)");
		else if (random <= 14)	OStr << ("(For a character with a high constitution, experiment with working on both shifts.)");
		else if (random <= 16)	OStr << ("(Matrons need high service skills.)");
		else if (random <= 18)	OStr << ("(Girls see a max of 3 people for high reputations, 3 for high appearance and 3 for high skills.)");
		else /*            */	OStr << ("(Don't overlook the bribery option in the town hall and the bank.)");
		break;
	case 14: OStr << ("grandmaster piledriver the 17th."); break;
	case 15:
		OStr << ("the evolved sexual entity from ");
		random = g_Dice % 8 + 1;
		if (random <= 2)
		{
			random = g_Dice % 200000 + 100000;
			OStr << random;
			OStr << (" years in the future.");
		}
		else if (random <= 4)	OStr << ("the closet.");
		else if (random <= 6)	OStr << ("the suburbs.");
		else /*            */	OStr << ("somewhere in deep space.");
		break;
	case 16:
		OStr << ("the ");
		random = g_Dice % 8 + 1;
		/* */if (random <= 2)	OStr << ("mayor");
		else if (random <= 4)	OStr << ("bishop");
		else if (random <= 6)	OStr << ("town treasurer");
		else /*            */	OStr << ("school principle");
		OStr << (", on one of his regular health checkups.");
		break;
	case 17: OStr << ("the letter H."); break;
	case 18: OStr << ("a completely regular and unspectacular guy."); break;
	case 19:
		OStr << ("the ");
		random = g_Dice % 20 + 5;
		OStr << random;
		OStr << (" dick, ");
		random = g_Dice % 20 + 5;
		OStr << random;
		OStr << ("-armed ");
		OStr << ("(Each wearing ");
		random = g_Dice % 2 + 4;
		OStr << random;
		OStr << " ";
		random = g_Dice % 8 + 1;
		/* */if (random <= 2)	OStr << ("rings of the Schwarzenegger");
		else if (random <= 4)	OStr << ("rings of the horndog");
		else if (random <= 6)	OStr << ("rings of beauty");
		else /*            */	OStr << ("rings of potent sexual stamina");
		OStr << (") ");
		random = g_Dice % 8 + 1;
		/* */if (random <= 2)	OStr << ("neighbor");
		else if (random <= 4)	OStr << ("yugoloth");
		else if (random <= 6)	OStr << ("abberation");
		else /*            */	OStr << ("ancient one");
		OStr << (".");
		break;
	case 20:
		OStr << ("the number 69."); break;
	}
# pragma endregion sex3
	OStr << ("\n");
	return OStr.str();
}

//SIN - adding
string cGirls::GetRandomOralSexString()
{
	int roll1 = 0, roll2 = 0, roll3 = 0, random = 0;
	stringstream OStr;
	OStr << " ";  // Consistency
	// Roll #1
# pragma region oral1
	roll1 = g_Dice % 6 + 1;   // << updated??
	switch (roll1)
	{
	case 1:
		random = g_Dice % 10 + 1;
		/* */if (random <= 2)	OStr << ("knelt in front of");
		else if (random <= 4)	OStr << ("bowed deeply to");
		else if (random <= 6)	OStr << ("fingered herself in front of");
		else if (random <= 8)	OStr << ("stripped off before");
		else /*            */	OStr << ("smiled hungrily at");
		OStr << (" the client, and grabbed his ");
		random = g_Dice % 10 + 1;
		/* */if (random == 1)	OStr << ("meat");
		else if (random == 2)	OStr << ("one-eyed dragon");
		else if (random == 3)	OStr << ("cock");
		else if (random == 4)	OStr << ("trouser-snake");
		else if (random == 5)	OStr << ("love train");
		else if (random == 6)	OStr << ("bald bishop");
		else if (random == 7)	OStr << ("fuckpole");
		else if (random == 8)	OStr << ("meaty womb raider");
		else if (random == 9)	OStr << ("crankshaft");
		else /*            */	OStr << ("greasy gutbuster");
		break;
	case 2:
		OStr << ("made him sit on a chair and watch as she 'fellated' a ");
		random = g_Dice % 10 + 1;
		/* */if (random <= 2)	OStr << ("banana");
		else if (random <= 4)	OStr << ("dildo");
		else if (random <= 6)	OStr << ("bottle");
		else if (random <= 8)	OStr << ("broomstick");
		else /*            */	OStr << ("bedknob");
		OStr << (" while ");
		random = g_Dice % 10 + 1;
		/* */if (random <= 2)	OStr << ("never breaking eye-contact");
		else if (random <= 4)	OStr << ("fingering her clit");
		else if (random <= 6)	OStr << ("rubbing her nipples");
		else if (random <= 8)	OStr << ("another girl watched");
		else /*            */	OStr << ("bending waaaaayyy over");
		break;
	case 3:
		OStr << ("had some fun with his ");
		random = g_Dice % 10 + 1;
		/* */if (random <= 2)	OStr << ("foot");
		else if (random <= 4)	OStr << ("tickle");
		else if (random <= 6)	OStr << ("hair");
		else if (random <= 8)	OStr << ("butt");
		else /*            */	OStr << ("food");
		OStr << (" fetish and gave him an extended ");
		random = g_Dice % 6 + 1;
		/* */if (random <= 2)	OStr << ("ball");
		else if (random <= 4)	OStr << ("anal");
		else /*            */	OStr << ("cock");
		OStr << (" 'massage'");
		break;
	case 4:
		OStr << ("dressed as a ");
		random = g_Dice % 10 + 1;
		/* */if (random <= 2)	OStr << ("schoolgirl");
		else if (random <= 4)	OStr << ("girl-next-door");
		else if (random <= 6)	OStr << ("cheerleader");
		else if (random <= 8)	OStr << ("baby-sitter");
		else /*            */	OStr << ("priestess");
		OStr << (" and begged him to ");
		random = g_Dice % 10 + 1;
		/* */if (random <= 2)	OStr << ("let her know what cum tastes like");
		else if (random <= 4)	OStr << ("teach her how to please a man");
		else if (random <= 6)	OStr << ("let her taste his love");
		else if (random <= 8)	OStr << ("use her worthless face");
		else /*            */	OStr << ("be gentle... but not too gentle");
		break;
	case 5:
		OStr << ("lay naked across ");
		random = g_Dice % 10 + 1;
		/* */if (random <= 2)	OStr << ("the bed");
		else if (random <= 4)	OStr << ("the desk");
		else if (random <= 6)	OStr << ("some cushions");
		else if (random <= 8)	OStr << ("a cold table-top");
		else /*            */	OStr << ("a pile of boxes");
		OStr << (" with her head hanging over the edge ");
		random = g_Dice % 10 + 1;
		/* */if (random <= 2)	OStr << ("and her mouth wide open");
		else if (random <= 4)	OStr << ("and her throat ready");
		else if (random <= 6)	OStr << ("ready for a face-fuck");
		else if (random <= 8)	OStr << ("and her lips puckered");
		else /*            */	OStr << ("and a craving in her eyes");
		break;
	case 6:
		OStr << ("warmed up by ");
		random = g_Dice % 10 + 1;
		/* */if (random <= 2)	OStr << ("shoving a giant dildo down her throat");
		else if (random <= 4)	OStr << ("deepthroating a perfume bottle");
		else if (random <= 6)	OStr << ("shoving her fingers down her throat");
		else if (random <= 8)	OStr << ("gagging down her favourite butt-plug");
		else /*            */	OStr << ("deepthroatng a sacred religious symbol");
		OStr << (", as she absently ");
		random = g_Dice % 10 + 1;
		/* */if (random <= 2)	OStr << ("rubbed her clit");
		else if (random <= 4)	OStr << ("pinched her nipples");
		else if (random <= 6)	OStr << ("flicked her bean");
		else if (random <= 8)	OStr << ("fisted herself");
		else /*            */	OStr << ("fingered her anus");
		break;
	}
# pragma endregion oral1
	// Roll #2
# pragma region oral2
	OStr << ". Finally, ";
	roll2 = g_Dice % 7 + 1;  //<<< updated????
	switch (roll2)
	{
	case 1:
		OStr << ("her ");
		random = g_Dice % 8 + 1;
		/* */if (random <= 2)	OStr << ("soft-lips made love to his");
		else if (random <= 4)	OStr << ("open throat sucked down his");
		else if (random <= 6)	OStr << ("tongue milked his");
		else /*            */	OStr << ("mouth energetically fucked his");
		OStr << (" ");
		random = g_Dice % 8 + 1;
		/* */if (random <= 2)	OStr << ("rock hard cock");
		else if (random <= 4)	OStr << ("slippery man-meat");
		else if (random <= 6)	OStr << ("greasy sex-pole");
		else /*            */	OStr << ("fat flesh-flute");
		OStr << (" until ");
		random = g_Dice % 10 + 1;
		/* */if (random <= 2)	OStr << ("hot cum was forced down her throat");
		else if (random <= 4)	OStr << ("cum splattered across her face");
		else if (random <= 6)	OStr << ("she received a spunky facial");
		else if (random <= 8)	OStr << ("cum was shooting out of her nose");
		else /*            */	OStr << ("her mouth was shot full of semen");
		OStr << (" courtesy of");
		break;
	case 2:
		OStr << ("her jaw felt like it was gonna lock as ");
		random = g_Dice % 10 + 1;
		/* */if (random <= 2)	OStr << ("she greedily gulped his meat");
		else if (random <= 4)	OStr << ("she relentlessly sucked him off");
		else if (random <= 6)	OStr << ("her mouth expertly milked his manhood");
		else if (random <= 8)	OStr << ("his cock slipped into her throat");
		else /*            */	OStr << ("she sucked and licked his cock");
		OStr << (" until ");
		random = g_Dice % 10 + 1;
		/* */if (random <= 2)	OStr << ("a warm desposit was left in her mouth");
		else if (random <= 4)	OStr << ("cum was pumped directly into her stomach");
		else if (random <= 6)	OStr << ("her eyes bulged as cum filled her head");
		else if (random <= 8)	OStr << ("cum was blasted into her face");
		else /*            */	OStr << ("sperm dripped from her mouth and nose");
		OStr << (" courtesy of");
		break;
	case 3:
		OStr << ("she was face-fucked ");
		random = g_Dice % 10 + 1;
		/* */if (random <= 2)	OStr << ("blind");
		else if (random <= 4)	OStr << ("raw");
		else if (random <= 6)	OStr << ("senseless");
		else if (random <= 8)	OStr << ("for hours");
		else /*            */	OStr << ("against the wall");
		OStr << (" by");
		break;
	case 4:
		OStr << ("her throat was stuffed full of ");
		random = g_Dice % 10 + 1;
		/* */if (random <= 2)	OStr << ("hot cock");
		else if (random <= 4)	OStr << ("male anatomy");
		else if (random <= 6)	OStr << ("her favourite thing");
		else if (random <= 8)	OStr << ("quivering pork sword");
		else /*            */	OStr << ("stanky dick");
		OStr << (" until ");
		random = g_Dice % 10 + 1;
		/* */if (random <= 2)	OStr << ("her eyes bulged");
		else if (random <= 4)	OStr << ("her ears popped");
		else if (random <= 6)	OStr << (", with a shudder, cum was shot straight into her gut");
		else if (random <= 8)	OStr << ("he pulled out and splurged in her face");
		else /*            */	OStr << ("cum was left drooling from her mouth");
		OStr << (" thanks to");
		break;
	case 5:
		OStr << ("she took his cock in her mouth and expertly ");
		random = g_Dice % 10 + 1;
		/* */if (random <= 2)	OStr << ("sucked it down");
		else if (random <= 4)	OStr << ("licked from smooth head to hairy base");
		else if (random <= 6)	OStr << ("pleasured the head with her soft lips");
		else if (random <= 8)	OStr << ("deepthroated the lot");
		else /*            */	OStr << ("massaged it with her lips while licking with her tongue");
		OStr << (" while her hands ");
		random = g_Dice % 10 + 1;
		/* */if (random <= 2)	OStr << ("played with his balls");
		else if (random <= 4)	OStr << ("cradled his balls");
		else if (random <= 6)	OStr << ("gripped his butt");
		else if (random <= 8)	OStr << ("stayed behind her back");
		else /*            */	OStr << ("fingered his ass-hole");
		OStr << (" until she got a hot mouthful of cum from");
		break;
	case 6:
		OStr << ("the customer climbed over, and full-on fucked her face. She ended up ");
		random = g_Dice % 10 + 1;
		/* */if (random <= 2)	OStr << ("with a warm desposit filling her mouth");
		else if (random <= 4)	OStr << ("with her gut pumped full of cum");
		else if (random <= 6)	OStr << ("spunked up from nose to navel");
		else if (random <= 8)	OStr << ("with cum blasted in her face");
		else /*            */	OStr << ("with sperm exploding from her mouth and nose");
		OStr << (" courtesy of");
		break;
	case 7:
		OStr << ("she clamped her lips around his dick and wouldn't stop ");
		random = g_Dice % 10 + 1;
		/* */if (random <= 2)	OStr << ("sucking and stimulating it with her mouth");
		else if (random <= 4)	OStr << ("orally fucking it");
		else if (random <= 6)	OStr << ("deepthroating it");
		else if (random <= 8)	OStr << ("pumping it");
		else /*            */	OStr << ("gulping it down");
		OStr << (" until she had swallowed down ") << (g_Dice % 5 + 2) << (" loads of cum from ");
		break;
	}
# pragma endregion oral2
	// Roll #3
# pragma region oral3
	OStr << " ";	// Consistency
	roll3 = g_Dice % 18 + 1;
	switch (roll3)
	{
	case 1:
		OStr << ("the guy ");
		random = g_Dice % 10 + 1;
		/* */if (random <= 2)	OStr << ("wearing three amulets of the sex elemental.");
		else if (random <= 4)	OStr << ("wearing eight rings of the horndog.");
		else if (random <= 6)	OStr << ("wearing a band of invulnerability.");
		else if (random <= 8)	OStr << ("in the Guy mask.");
		else /*            */	OStr << ("with the funny eyes.");
		break;
	case 2: OStr << ("Poseidon, God of Salty Seamen!!!!"); break;
	case 3:
		OStr << ("the good ");
		random = g_Dice % 10 + 1;
		/* */if (random <= 2)	OStr << ("Lord Mayor.");
		else if (random <= 4)	OStr << ("samaritan.");
		else if (random <= 6)	OStr << ("Reverend.");
		else if (random <= 8)	OStr << ("shepherd.");
		else /*            */	OStr << ("husband (a blowjob isn't cheating).");
		break;
	case 4: OStr << ("the surprisingly endowed dwarven gentleman."); break;
	case 5: OStr << ("Colonel Mustard (right after he did it with the candlestick)."); break;
	case 6:
		OStr << ("the ");
		random = g_Dice % 10 + 1;
		/* */if (random <= 2)	OStr << "talking bear.";
		else if (random <= 4)	OStr << ("bearded biker dude.");
		else if (random <= 6)	OStr << ("dude who looked like a lady.");
		else if (random <= 8)	OStr << ("mischievous monkey man.");
		else /*            */	OStr << ("escaped pensioner.");
		break;
	case 7:
		OStr << ("the unbelievably well behaved ");
		random = g_Dice % 8 + 1;
		/* */if (random <= 2)	OStr << ("Pink Petal forum member.");
		else if (random <= 4)	OStr << ("Judge.");
		else if (random <= 6)	OStr << ("pirate.");
		else /*            */	OStr << ("ninja.");
		break;
	case 8:
		random = g_Dice % 20 + 1;
		OStr << ("the infamous ");
		/* */if (random <= 2)	OStr << ("Lord");
		else if (random <= 4)	OStr << ("Master");
		else if (random <= 6)	OStr << ("Saint");
		else if (random <= 8)	OStr << ("Lieutenant");
		else if (random <= 10)	OStr << ("Master");
		else if (random <= 12)	OStr << ("Doctor");
		else if (random <= 14)	OStr << ("Gardener");
		else if (random <= 16)	OStr << ("Blacksmith");
		else if (random <= 18)	OStr << ("DockMaster");
		else /*            */	OStr << ("Sherrif");
		OStr << (" Peterson.");
		break;
	case 9: OStr << ("this regular customer."); break;
	case 10: OStr << ("the pesky poltergeist."); break;
	case 11:
		OStr << ("her ");
		random = g_Dice % 10 + 1;
		/* */if (random <= 2)	OStr << ("platonic guy friend, who she'd always thought of more as a brother.");
		else if (random <= 4)	OStr << ("personal trainer, Buck.");
		else if (random <= 6)	OStr << ("excited former-classmate.");
		else if (random <= 8)	OStr << ("father's best-friend.");
		else /*            */	OStr << ("kindly uncle.");
		break;
	case 12:
		OStr << ("the clockwork man! (With no sensation in his clockwork ");
		random = g_Dice % 10 + 1;
		/* */if (random <= 2)	OStr << ("tool");
		else if (random <= 4)	OStr << ("head");
		else if (random <= 6)	OStr << ("fingers");
		else if (random <= 8)	OStr << ("attachment");
		else /*            */	OStr << ("clock");
		OStr << (" and no sense to ");
		random = g_Dice % 10 + 1;
		/* */if (random <= 2)	OStr << ("stop");
		else if (random <= 4)	OStr << ("slow down");
		else if (random <= 6)	OStr << ("moderate");
		else if (random <= 8)	OStr << ("be gentle");
		else
		{
			OStr << ("stop at ");
			random = g_Dice % 50 + 30;
			OStr << random;
			OStr << (" orgasms");
		}
		OStr << (".)");
		break;
	case 13:
		// MYR: This one gives useful advice to the players.  A gift from us to them.
		OStr << ("the Brothel Master developer. ");
		random = g_Dice % 20 + 1;
		/* */if (random <= 2)	OStr << ("(Quick learner is a great talent to have.)");
		else if (random <= 4)	OStr << ("(Don't ignore the practice skills option for your girls.)");
		else if (random <= 6)	OStr << ("(Train your gangs.)");
		else if (random <= 8)	OStr << ("(Every time you restart the game, the shop inventory is reset.)");
		else if (random <= 10)	OStr << ("(Invulnerable (incorporeal) characters should be exploring the catacombs.)");
		else if (random <= 12)	OStr << ("(High dodge gear is great for characters exploring the catacombs.)");
		else if (random <= 14)	OStr << ("(For a character with a high constitution, experiment with working on both shifts.)");
		else if (random <= 16)	OStr << ("(Matrons need high service skills.)");
		else if (random <= 18)	OStr << ("(Girls see a max of 3 people for high reputations, 3 for high appearance and 3 for high skills.)");
		else /*            */	OStr << ("(Don't overlook the bribery option in the town hall and the bank.)");
		break;
	case 14: OStr << ("a guy she never noticed at school."); break;
	case 15:
		OStr << ("a man cursed with permanent 'hardness.'");	break;
	case 16:
		OStr << ("the ");
		random = g_Dice % 8 + 1;
		/* */if (random <= 2)	OStr << ("mayor");
		else if (random <= 4)	OStr << ("bishop");
		else if (random <= 6)	OStr << ("town treasurer");
		else /*            */	OStr << ("school principle");
		OStr << (", on one of his regular health checkups.");
		break;
	case 17: OStr << ("an young guy in ");
		random = g_Dice % 10 + 1;
		/* */if (random <= 2)	OStr << ("a dress.");
		else if (random <= 4)	OStr << ("robes.");
		else if (random <= 6)	OStr << ("handcuffs.");
		else if (random <= 8)	OStr << ("a straight-jacket.");
		else /*            */	OStr << ("trouble with the mob.");
		break;
	case 18: OStr << ("the untapped virgin."); break;
	}
# pragma endregion oral3
	OStr << ("\n");
	return OStr.str();
}

string cGirls::GetRandomGroupString()
{
	int roll1 = 0, roll2 = 0, roll3 = 0, random = 0;
	stringstream OStr;
	// Part 1
# pragma region group1
	OStr << " ";
	roll1 = g_Dice % 4 + 1;   // Remember to update this when new strings are added
	switch (roll1)
	{
	case 1:
		OStr << ("counted the number of customers: ");
		random = g_Dice % 20 + 5;
		OStr << random;
		OStr << ". ";
		random = g_Dice % 14 + 1;
		/* */if (random <= 2)	OStr << ("This was going to be rough");
		else if (random <= 4)	OStr << ("Sweet");
		else if (random <= 6)	OStr << ("It could be worse");
		else if (random <= 8)	OStr << ("A smile formed on her lips. This was going to be fun");
		else if (random <= 10)	OStr << ("Boring");
		else if (random <= 12)	OStr << ("Not enough");
		else /*            */	OStr << ("'Could you get more?' she wondered");
		break;
	case 2: OStr << ("was lost in ");
		random = g_Dice % 8 + 1;
		/* */if (random <= 2)	OStr << ("a sea");
		else if (random <= 4)	OStr << ("a storm");
		else if (random <= 6)	OStr << ("an ocean");
		else /*            */	OStr << ("a jungle");
		OStr << (" of hot bodies");
		break;
	case 3:
		random = g_Dice % 8 + 1;
		/* */if (random <= 2)	OStr << ("sat");
		else if (random <= 4)	OStr << ("lay");
		else if (random <= 6)	OStr << ("stood");
		else /*            */	OStr << ("crouched");
		OStr << " ";
		random = g_Dice % 10 + 1;
		/* */if (random <= 5)	OStr << ("blindfolded and ");
		random = g_Dice % 8 + 1;
		/* */if (random <= 2)	OStr << ("handcuffed");
		else if (random <= 4)	OStr << ("tied up");
		else if (random <= 6)	OStr << ("wrists bound in rope");
		else /*            */	OStr << ("wrists in chains hanging from the ceiling");
		OStr << (" in the middle of a ");
		random = g_Dice % 8 + 1;
		/* */if (random <= 2)	OStr << ("circle");
		else if (random <= 4)	OStr << ("smouldering pile");
		else if (random <= 6)	OStr << ("phalanx");
		else /*            */	OStr << ("wall");
		OStr << (" of flesh");
		break;
	case 4:
		OStr << ("was ");
		random = g_Dice % 8 + 1;
		/* */if (random <= 2)	OStr << ("amazed by");
		else if (random <= 4)	OStr << ("disappointed by");
		else if (random <= 6)	OStr << ("overjoyed with");
		else /*            */	OStr << ("ecstatically happy with");
		OStr << (" the ");
		random = g_Dice % 8 + 1;
		/* */if (random <= 2)	OStr << ("brigade");
		else if (random <= 4)	OStr << ("army group");
		else if (random <= 6)	OStr << ("squad");
		else /*            */	OStr << ("batallion");
		OStr << (" of ");
		random = g_Dice % 8 + 1;
		/* */if (random <= 2)	OStr << ("man meat");
		else if (random <= 4)	OStr << ("cock");
		else if (random <= 6)	OStr << ("muscle");
		else /*            */	OStr << ("horny, brainless thugs");
		OStr << (" around her");
		break;
	}
# pragma endregion group1
	// Part 2
# pragma region group2
	OStr << (". ");
	roll2 = g_Dice % 8 + 1;
	switch (roll2)
	{
	case 1:
		OStr << ("She was thoroughly ");
		random = g_Dice % 8 + 1;
		/* */if (random <= 2)	OStr << ("banged");
		else if (random <= 4)	OStr << ("fucked");
		else if (random <= 6)	OStr << ("disappointed");
		else /*            */	OStr << ("penetrated");
		OStr << (" by");
		break;
	case 2:
		OStr << ("They handled her like ");
		random = g_Dice % 10 + 1;
		/* */if (random <= 5)	OStr << ("an expensive");
		else /*            */	OStr << ("a cheap");
		OStr << " ";
		random = g_Dice % 8 + 1;
		/* */if (random <= 2)	OStr << ("hooker");
		else if (random <= 4)	OStr << ("street worker");
		else if (random <= 6)	OStr << ("violin");
		else /*            */	OStr << ("wine");
		OStr << (" for");
		break;
	case 3:
		OStr << ("Her ");
		random = g_Dice % 10 + 1;
		/* */if (random <= 5)	OStr << ("holes were");
		else /*            */	OStr << ("love canal was");
		OStr << (" plugged by");
		break;
	case 4:
		OStr << ("She ");
		random = g_Dice % 6 + 1;
		/* */if (random <= 2)	OStr << ("was bukkaked by");
		else if (random <= 4)	OStr << ("was given pearl necklaces by");
		else /*            */	OStr << ("received a thorough face/hair job from");
		break;
	case 5:
		OStr << ("They demanded simultaneous hand, foot and mouth ");
		random = g_Dice % 6 + 1;
		/* */if (random <= 2)	OStr << ("jobs");
		else if (random <= 4)	OStr << ("action");
		else /*            */	OStr << ("combat");
		OStr << (" for");
		break;
	case 6:
		OStr << ("There was a positive side: 'So much ");
		random = g_Dice % 8 + 1;
		/* */if (random <= 2)	OStr << ("meat");
		else if (random <= 4)	OStr << ("cock");
		else if (random <= 6)	OStr << ("testosterone");
		else /*            */	OStr << ("to do");
		OStr << (", so little time' she said to");
		break;
	case 7:
		OStr << ("They made sure she had a nutritious meal of ");
		random = g_Dice % 8 + 1;
		/* */if (random <= 2)	OStr << ("man meat");
		else if (random <= 4)	OStr << ("cock");
		else if (random <= 6)	OStr << ("penis");
		else /*            */	OStr << ("meat rods");
		OStr << (" and drinks of delicious ");
		random = g_Dice % 8 + 1;
		/* */if (random <= 2)	OStr << ("semen");
		else if (random <= 4)	OStr << ("man mucus");
		else if (random <= 6)	OStr << ("man-love");
		else /*            */	OStr << ("man-cream");
		OStr << (" from");
		break;
	case 8:
		OStr << ("She was ");
		random = g_Dice % 8 + 1;
		/* */if (random <= 2)	OStr << ("fucked");
		else if (random <= 4)	OStr << ("banged");
		else if (random <= 6)	OStr << ("humped");
		else /*            */	OStr << ("sucked");
		OStr << (" silly ");
		/* */if (random <= 2)	OStr << ("twice over");
		else if (random <= 4)	OStr << ("three times over");
		else /*            */	OStr << ("so many times");
		OStr << (" by");
		break;
	}
# pragma endregion group2
	// Part 3
# pragma region group3
	OStr << " ";
	roll3 = g_Dice % 11 + 1;
	switch (roll3)
	{
	case 1:
		OStr << ("every member of the Crossgate ");
		random = g_Dice % 8 + 1;
		/* */if (random <= 2)	OStr << ("roads crew.");
		else if (random <= 4)	OStr << ("administrative staff.");
		else if (random <= 6)	OStr << ("interleague volleyball team.");
		else /*            */	OStr << ("short persons defense league.");
		; break;
	case 2:
		OStr << ("all the ");
		random = g_Dice % 8 + 1;
		/* */if (random <= 2)	OStr << ("lieutenants");
		else if (random <= 4)	OStr << ("sergeants");
		else if (random <= 6)	OStr << ("captains");
		else /*            */	OStr << ("junior officers");
		OStr << (" in the Mundigan ");
		random = g_Dice % 8 + 1;
		/* */if (random <= 2)	OStr << ("army.");
		else if (random <= 4)	OStr << ("navy.");
		else if (random <= 6)	OStr << ("elite forces.");
		else /*            */	OStr << ("foreign legion.");
		break;
	case 3:
		OStr << ("the visiting ");
		random = g_Dice % 8 + 1;
		/* */if (random <= 2)	OStr << ("half-giants. (Ouch!)");
		else if (random <= 4)	OStr << ("storm giants.");
		else if (random <= 6)	OStr << ("titans.");
		else /*            */	OStr << ("ogres.");
		break;
	case 4:
		OStr << ("the ");
		random = g_Dice % 8 + 1;
		/* */if (random <= 2)	OStr << ("Hentai Research");
		else if (random <= 4)	OStr << ("Women's Rights");
		else if (random <= 6)	OStr << ("Prostitution Studies");
		else /*            */	OStr << ("Celibacy");
		OStr << (" club of the University of Cunning Linguists.");
		break;
	case 5:
		OStr << ("the squad of ");
		random = g_Dice % 10 + 1;
		/* */if (random <= 5)	OStr << ("hard-to-find ninjas.");
		else /*            */	OStr << ("racous pirates.");
		break;
	case 6: OStr << ("a group of people from some place called the 'Pink Petal forums'."); break;
	case 7:
		OStr << ("the seemingly endless ");
		random = g_Dice % 8 + 1;
		/* */if (random <= 2)	OStr << ("army");
		else if (random <= 4)	OStr << ("horde");
		else if (random <= 6)	OStr << ("number");
		else /*            */	OStr << ("group");
		OStr << (" of really");
		random = g_Dice % 10 + 1;
		/* */if (random <= 5)	OStr << (", really ");
		else /*            */	OStr << " ";
		random = g_Dice % 8 + 1;
		/* */if (random <= 2)	OStr << ("fired up");
		else if (random <= 4)	OStr << ("horny");
		else if (random <= 6)	OStr << ("randy");
		else /*            */	OStr << ("backed up");
		OStr << " ";
		random = g_Dice % 8 + 1;
		/* */if (random <= 2)	OStr << ("gnomes.");
		else if (random <= 4)	OStr << ("halflings.");
		else if (random <= 6)	OStr << ("kobolds.");
		else /*            */	OStr << ("office workers.");
		break;
	case 8:
		OStr << ("CSI ");
		random = g_Dice % 8 + 1;
		/* */if (random <= 2)	OStr << ("New York");
		else if (random <= 4)	OStr << ("Miami");
		else if (random <= 6)	OStr << ("Mundigan");
		else /*            */	OStr << ("Tokyo");
		OStr << (" branch.");
		break;
	case 9:
		random = g_Dice % 8 + 1;
		/* */if (random <= 2)	OStr << ("frosh");
		else if (random <= 4)	OStr << ("seniors");
		else if (random <= 6)	OStr << ("young adults");
		else /*            */	OStr << ("women");
		OStr << (" on a ");
		random = g_Dice % 10 + 1;
		/* */if (random <= 5)	OStr << ("serious ");
		random = g_Dice % 8 + 1;
		/* */if (random <= 2)	OStr << ("bender");
		else if (random <= 4)	OStr << ("road trip");
		else if (random <= 6)	OStr << ("medical study");
		else /*            */	OStr << ("lark");
		OStr << (".");
		break;
	case 10:
		OStr << ("all the ");
		random = g_Dice % 8 + 1;
		/* */if (random <= 2)	OStr << ("second stringers");
		else if (random <= 4)	OStr << ("has-beens");
		else if (random <= 6)	OStr << ("never-weres");
		else /*            */	OStr << ("victims");
		OStr << (" from the ");
		random = g_Dice % 20 + 1991;
		OStr << random;
		OStr << (" H anime season.");
		break;
	case 11:
		OStr << ("Grandpa Parkins and his extended family of ");
		random = g_Dice % 200 + 100;
		OStr << random;
		OStr << (".");
		random = g_Dice % 10 + 1;
		if (random <= 5)
		{
			OStr << (" (And ");
			random = g_Dice % 100 + 50;
			OStr << random;
			OStr << (" guests.)");
		}
		random = g_Dice % 10 + 1;
		if (random <= 5)
		{
			OStr << (" (And ");
			random = g_Dice % 100 + 50;
			OStr << random;
			OStr << (" more from the extended extended family.)");
		}
		break;
	}
# pragma endregion group3
	OStr << ("\n");
	return OStr.str();
}
string cGirls::GetRandomBDSMString()
{
	int roll2 = 0, roll3 = 0, random = 0;
	stringstream OStr;
	OStr << (" was ");
	// Part 1:
# pragma region bdsm1
	// MYR: Was on a roll so I completely redid the first part
	random = g_Dice % 12 + 1;
	/* */if (random <= 2)	OStr << ("dressed as a dominatrix");
	else if (random <= 4)	OStr << ("stripped naked");
	else if (random <= 6)	OStr << ("dressed as a (strictly legal age) school girl");
	else if (random <= 8)	OStr << ("dressed as a nurse");
	else if (random <= 10)	OStr << ("put in heels");
	else /*            */	OStr << ("covered in oil");
	random = g_Dice % 4 + 1;
	if (random == 3)
	{
		random = g_Dice % 4 + 1;
		if (random <= 2)	OStr << (", rendered helpless by drugs");
		else 	OStr << (", restrained by magic");
	}
	random = g_Dice % 4 + 1;
	/* */if (random == 2)	OStr << (", blindfolded");
	random = g_Dice % 4 + 1;
	/* */if (random == 2)	OStr << (", gagged");
	OStr << (", and ");
	random = g_Dice % 12 + 1;
	/* */if (random <= 2)	OStr << ("chained");
	else if (random <= 4)	OStr << ("lashed");
	else if (random <= 6)	OStr << ("tied");
	else if (random <= 8)	OStr << ("bound");
	else if (random <= 10)	OStr << ("cuffed");
	else /*            */	OStr << ("leashed");
	random = g_Dice % 4 + 1;
	/* */if (random == 3)	OStr << (", arms behind her back");
	random = g_Dice % 4 + 1;
	/* */if (random == 2)	OStr << (", fettered");
	random = g_Dice % 4 + 1;
	/* */if (random == 2)	OStr << (", spread eagle");
	random = g_Dice % 4 + 1;
	/* */if (random == 2)	OStr << (", upside down");
	OStr << " ";
	random = g_Dice % 16 + 1;
	/* */if (random <= 2)	OStr << ("to a bed");
	else if (random <= 4)	OStr << ("to a post");
	else if (random <= 6)	OStr << ("to a wall");
	else if (random <= 8)	OStr << ("to vertical stocks");
	else if (random <= 10)	OStr << ("to a table");
	else if (random <= 12)	OStr << ("on a wooden horse");
	else if (random <= 14)	OStr << ("in stocks");
	else /*            */	OStr << ("at the dog house");
# pragma endregion bdsm1
	// Part 2
# pragma region bdsm2
	OStr << (". ");
	roll2 = g_Dice % 8 + 1;
	switch (roll2)
	{
	case 1:
		OStr << ("She was fucked ");
		random = g_Dice % 12 + 1;
		/* */if (random <= 2)	OStr << ("with a rake");
		else if (random <= 4)	OStr << ("with a giant dildo");
		else if (random <= 6)	OStr << ("and flogged");
		else if (random <= 8)	OStr << ("and lashed");
		else if (random <= 10)	OStr << ("tenderly");
		else /*            */	OStr << ("like a dog");
		OStr << (" by");
		break;
	case 2:
		OStr << ("Explanations were necessary before she was ");
		random = g_Dice % 14 + 1;
		/* */if (random <= 2)	OStr << ("screwed");
		else if (random <= 4)	OStr << ("penetrated");
		else if (random <= 6)	OStr << ("abused");
		else if (random <= 8)	OStr << ("whipped");
		else if (random <= 10)	OStr << ("yelled at");
		else if (random <= 12)	OStr << ("banged repeatedly");
		else /*            */	OStr << ("smacked around");
		OStr << (" by");
		break;
	case 3:
		OStr << ("Her holes were filled ");
		random = g_Dice % 16 + 1;
		/* */if (random <= 2)	OStr << ("with wiggly things");
		else if (random <= 4)	OStr << ("with vibrating things");
		else if (random <= 6)	OStr << ("with sex toys");
		else if (random <= 8)	OStr << ("by things with uncomfortable edges");
		else if (random <= 10)	OStr << ("with marbles");
		else if (random <= 12)	OStr << ("with foreign objects");
		else if (random <= 14)	OStr << ("with hopes and dreams");
		else /*            */	OStr << ("with semen");
		OStr << (" by");
		break;
	case 4:
		OStr << ("A massive aphrodisiac was administered before she was ");
		random = g_Dice % 10 + 1;
		/* */if (random <= 2)	OStr << ("teased");
		else if (random <= 4)	OStr << ("fucked");
		else if (random <= 6)	OStr << ("left alone");
		else if (random <= 8)	OStr << ("repeatedly brought to the edge of orgasm, but not over");
		else /*            */	OStr << ("mercilessly tickled by a feather wielded");
		OStr << (" by");
		break;
	case 5:
		OStr << ("Entertainment was demanded before she was ");
		random = g_Dice % 12 + 1;
		/* */if (random <= 2)	OStr << ("humped");
		else if (random <= 4)	OStr << ("rough-housed");
		else if (random <= 6)	OStr << ("pinched over and over");
		else if (random <= 8)	OStr << ("probed by instruments");
		else if (random <= 10)	OStr << ("fondled roughly");
		else /*            */	OStr << ("sent away");
		OStr << (" by");
		break;
	case 6:
		OStr << ("She was pierced repeatedly by ");
		random = g_Dice % 6 + 1;
		/* */if (random <= 2)	OStr << ("needles");
		else if (random <= 4)	OStr << ("magic missiles");
		else /*            */	OStr << ("evil thoughts");
		OStr << (" from");
		break;
	case 7:
		//OStr <<"She had ";
		random = g_Dice % 10 + 1;
		/* */if (random <= 2)	OStr << ("Weights");
		else if (random <= 4)	OStr << ("Christmas ornaments");
		else if (random <= 6)	OStr << ("Lewd signs");
		else if (random <= 6)	OStr << ("Trinkets");
		else /*            */	OStr << ("Abstract symbols");
		OStr << (" were hung from her unmentionables by");
		break;
	case 8:
		OStr << ("She was ordered to ");
		random = g_Dice % 8 + 1;
		/* */if (random <= 2)	OStr << ("flail herself");
		else if (random <= 4)	OStr << ("perform fellatio");
		else if (random <= 6)	OStr << ("masturbate");
		else /*            */	OStr << ("beg for it");
		OStr << (" by");
		break;
	}
# pragma endregion bdsm2
	// Part 3
# pragma region bdsm3
	OStr << " ";
	roll3 = g_Dice % 18 + 1;
	switch (roll3)
	{
	case 1: OStr << ("Iron Man."); break;
	case 2:
		OStr << ("the ");
		random = g_Dice % 8 + 1;
		/* */if (random <= 2)	OStr << ("wizard");
		else if (random <= 4)	OStr << ("sorceress");
		else if (random <= 6)	OStr << ("archmage");
		else /*            */	OStr << ("warlock");
		OStr << ("'s ");
		random = g_Dice % 8;
		/* */if (random <= 2)	OStr << ("golem.");
		else if (random <= 4)	OStr << ("familiar.");
		else if (random <= 6)	OStr << ("homoculous.");
		else /*            */	OStr << ("summoned monster.");
		break;
	case 3:
		OStr << ("the amazingly hung ");
		random = g_Dice % 8;
		if (random <= 2)	OStr << ("goblin.");
		else if (random <= 4)	OStr << ("civic worker.");
		else if (random <= 6)	OStr << ("geletanious cube.");
		else /*            */	OStr << ("sentient shirt.");   // MYR: I love this one.
		break;
	case 4: OStr << ("the pirate dressed as a ninja. (Cool things are cool.)"); break;
	case 5: OStr << ("Hannibal Lecter."); break;
	case 6:
		OStr << ("the stoned ");
		random = g_Dice % 10 + 1;
		if (random <= 2)	OStr << ("dark elf.");
		else if (random <= 4)	OStr << ("gargoyle.");
		else if (random <= 6)	OStr << ("earth elemental.");
		else if (random <= 8)	OStr << ("astral deva.");
		else /*            */	OStr << ("college kid.");
		break;
	case 7:
		OStr << ("your hyperactive ");
		random = g_Dice % 10 + 1;
		if (random <= 2)	OStr << ("cousin.");
		else if (random <= 4)	OStr << ("grandmother.");
		else if (random <= 6)	OStr << ("grandfather.");
		else if (random <= 8)	OStr << ("brother.");
		else /*            */	OStr << ("sister.");
		break;
	case 8: OStr << ("someone who looks exactly like you!"); break;
	case 9:
		OStr << ("the horny ");
		random = g_Dice % 10 + 1;
		if (random <= 2)	OStr << ("genie.");
		else if (random <= 4)	OStr << ("fishmonger.");
		else if (random <= 6)	OStr << ("chauffeur.");
		else if (random <= 8)	OStr << ("Autobot.");
		else /*            */	OStr << ("thought.");
		break;
	case 10:
		OStr << ("the rampaging ");
		random = g_Dice % 10 + 1;
		if (random <= 2)	OStr << ("english professor.");
		else if (random <= 4)	OStr << ("peace activist.");
		else if (random <= 6)	OStr << ("color red.");
		else if (random <= 8)	OStr << ("special forces agent.");
		else /*            */	OStr << ("chef.");
		break;
	case 11:
		random = g_Dice % 10 + 1;
		if (random <= 2)	OStr << ("disloyal thugs");
		else if (random <= 4)	OStr << ("girls");
		else if (random <= 6)	OStr << ("dissatisfied customers");
		else if (random <= 8)	OStr << ("workers");
		else /*            */	OStr << ("malicious agents");
		OStr << (" from a competing brothel."); break;
	case 12: OStr << ("a cruel ");
		random = g_Dice % 8 + 1;
		if (random <= 2)	OStr << ("Cyberman.");
		else if (random <= 4)	OStr << ("Dalek.");
		else if (random <= 6)	OStr << ("Newtype.");
		else /*            */	OStr << ("Gundam.");
		break;
	case 13: OStr << ("Sexbot Mk-");
		random = g_Dice % 200 + 50;
		OStr << random;
		OStr << (".");
		break;
	case 14:
		OStr << ("underage kids ");
		random = g_Dice % 10 + 1;
		if (random <= 2)	OStr << ("(Who claim to be of age.)");
		else if (random <= 4)	OStr << ("(Who snuck in.)");
		else if (random <= 6)	OStr << ("(Who are somehow related to the Brothel Master, so its ok.)");
		else if (random <= 8)	OStr << ("(They paid, so who cares?)");
		else /*            */	OStr << ("(We must corrupt them while they're still young.)");
		break;
	case 15: OStr << ("Grandpa Parkins from down the street."); break;
	case 16:
		OStr << ("the ... thing living ");
		random = g_Dice % 10 + 1;
		if (random <= 2)	OStr << ("in the underwear drawer");
		else if (random <= 4)	OStr << ("in the sex-toy box");
		else if (random <= 6)	OStr << ("under the bed");
		else if (random <= 8)	OStr << ("in her shadow");
		else /*            */	OStr << ("in her psyche");
		OStr << (".");
		break;
	case 17: OStr << ("the senior member of the cult of ");
		random = g_Dice % 10 + 1;
		if (random <= 2)	OStr << ("tentacles.");
		else if (random <= 4)	OStr << ("unending pain.");
		else if (random <= 6)	OStr << ("joy and happiness.");
		else if (random <= 8)	OStr << ("Whore Master developers.");
		else /*            */	OStr << ("eunuchs.");
		break;
	case 18:
		OStr << ("this wierdo who appeared out of this blue box called a ");
		random = g_Dice % 10 + 1;
		if (random <= 5)	OStr << ("TARDIS.");
		else /*            */	OStr << ("TURDIS"); // How many people will say I made a spelling mistake?
		random = g_Dice % 10 + 1;
		if (random <= 5)
		{
			OStr << " ";
			random = g_Dice % 10 + 1;
			/* */if (random <= 2)	OStr << ("His female companion was in on the action too.");
			else if (random <= 4)	OStr << ("His mechanical dog was involved as well.");
			else if (random <= 6)	OStr << ("His female companion and mechanical dog did lewd things to each other while they watched.");
		}
		break;
	}
# pragma endregion bdsm3
	OStr << "\n";
	return OStr.str();
}
string cGirls::GetRandomBeastString()
{
	int roll1 = 0, roll2 = 0, roll3 = 0, random = 0;
	stringstream OStr;
	bool NeedAnd = false;
	OStr << (" was ");
# pragma region beast1
	roll1 = g_Dice % 7 + 1;   // Remember to update this when new strings are added
	switch (roll1)
	{
	case 1: OStr << ("filled with ");
		random = g_Dice % 12 + 1;
		/* */if (random <= 2)	OStr << ("vibrating");
		else if (random <= 4)	OStr << ("wiggling");
		else if (random <= 6)	OStr << ("living");
		else if (random <= 8)	OStr << ("energetic");
		else if (random <= 10)	OStr << ("big");
		else 	OStr << ("pokey");
		OStr << (" things that ");
		random = g_Dice % 8 + 1;
		/* */if (random <= 2)	OStr << ("tickled");
		else if (random <= 4)	OStr << ("pleasured");
		else if (random <= 6)	OStr << ("massaged");
		else /*            */	OStr << ("scraped");
		OStr << (" her insides");
		break;
	case 2:
		OStr << ("forced against ");
		random = g_Dice % 8 + 1;
		/* */if (random <= 2)	OStr << ("a wall");
		else if (random <= 4)	OStr << ("a window");
		else if (random <= 6)	OStr << ("another client");
		else /*            */	OStr << ("another girl");
		OStr << (" and told to ");
		random = g_Dice % 8 + 1;
		/* */if (random <= 2)	OStr << ("spread her legs");
		else if (random <= 4)	OStr << ("give up hope");
		else if (random <= 6)	OStr << ("hold on tight");
		else /*            */	OStr << ("smile through it");
		break;
	case 3:
		OStr << ("worried by the ");
		random = g_Dice % 8 + 1;
		/* */if (random <= 2)	OStr << ("huge size");
		else if (random <= 4)	OStr << ("skill");
		else if (random <= 6)	OStr << ("reputation");
		else /*            */	OStr << ("aggressiveness");
		OStr << (" of the client");
		break;
	case 4:
		OStr << ("stripped down to her ");
		random = g_Dice % 10 + 1;
		/* */if (random <= 2)	OStr << ("g-string");
		else if (random <= 4)	OStr << ("panties");
		else if (random <= 6)	OStr << ("bra and panties");
		else if (random <= 8)	OStr << ("teddy");
		else /*            */	OStr << ("skin");
		OStr << (" and covered in ");
		random = g_Dice % 10 + 1;
		/* */if (random <= 2)	OStr << ("compliments");
		else if (random <= 4)	OStr << ("abuse");
		else if (random <= 6)	OStr << ("peanut butter");
		else if (random <= 8)	OStr << ("honey");
		else /*            */	OStr << ("motor oil");
		break;
	case 5:
		OStr << ("chained up in the ");
		random = g_Dice % 10 + 1;
		/* */if (random <= 2)	OStr << ("dungeon");
		else if (random <= 4)	OStr << ("den");
		else if (random <= 6)	OStr << ("kitchen");
		else if (random <= 8)	OStr << ("most public of places");
		else /*            */	OStr << ("backyard");
		OStr << (" and her ");
		random = g_Dice % 6 + 1;
		/* */if (random <= 2)	OStr << ("arms");
		else if (random <= 4)	OStr << ("legs");
		else /*            */	OStr << ("arms and legs");
		OStr << (" were lashed to posts");
		break;
	case 6:
		random = g_Dice % 10 + 1;
		/* */if (random <= 2)	OStr << ("tied up");
		else if (random <= 4)	OStr << ("wrapped up");
		else if (random <= 6)	OStr << ("trapped");
		else if (random <= 8)	OStr << ("bound");
		else /*            */	OStr << ("covered");
		OStr << (" in ");
		random = g_Dice % 10 + 1;
		/* */if (random <= 2)	OStr << ("flesh");
		else if (random <= 4)	OStr << ("tentacles");
		else if (random <= 6)	OStr << ("cellophane");
		else if (random <= 8)	OStr << ("tape");
		else /*            */	OStr << ("false promises");
		OStr << (" and ");
		random = g_Dice % 10 + 1;
		/* */if (random <= 2)	OStr << ("groped");
		else if (random <= 4)	OStr << ("tweaked");
		else if (random <= 6)	OStr << ("licked");
		else if (random <= 8)	OStr << ("spanked");
		else /*            */	OStr << ("left alone");
		OStr << (" for hours");
		break;
	case 7:
		OStr << ("pushed to the limits of ");
		random = g_Dice % 10 + 1;
		/* */if (random <= 2)	OStr << ("flexibility");
		else if (random <= 4)	OStr << ("endurance");
		else if (random <= 6)	OStr << ("patience");
		else if (random <= 8)	OStr << ("consciousness");
		else /*            */	OStr << ("sanity");
		OStr << (" and ");
		random = g_Dice % 6 + 1;
		/* */if (random <= 2)	OStr << ("cried out");
		else if (random <= 4)	OStr << ("swooned");
		else /*            */	OStr << ("spasmed");
		break;
	}
# pragma endregion beast1
	// Part 2
# pragma region beast2
	OStr << ". ";
	roll2 = g_Dice % 9 + 1;
	switch (roll2)
	{
	case 1:
		OStr << ("She ");
		random = g_Dice % 10 + 1;
		/* */if (random <= 2)	OStr << ("smoothly");
		else if (random <= 4)	OStr << ("roughly");
		else if (random <= 6)	OStr << ("lustily");
		else if (random <= 8)	OStr << ("repeatedly");
		else /*            */	OStr << ("orgasmically");
		OStr << " ";
		random = g_Dice % 10 + 1;
		/* */if (random <= 2)	OStr << ("fucked");
		else if (random <= 4)	OStr << ("railed");
		else if (random <= 6)	OStr << ("banged");
		else if (random <= 8)	OStr << ("screwed");
		else /*            */	OStr << ("pleasured");
		break;
	case 2:
		OStr << ("She was ");
		random = g_Dice % 6 + 1;
		/* */if (random <= 2)	OStr << ("teased");
		else if (random <= 4)	OStr << ("taunted");
		else /*            */	OStr << ("roughed up");
		OStr << (" and ");
		random = g_Dice % 10 + 1;
		/* */if (random <= 2)	OStr << ("orally");
		else if (random <= 4)	OStr << "";         // MYR: This isn't a bug.  'physically violated' is redundant, so this just prints 'violated'
		else if (random <= 6)	OStr << ("mentally");
		else if (random <= 8)	OStr << ("repeatedly");
		else /*            */	OStr << ("haughtily");
		OStr << (" violated by");
		break;
	case 3:
		OStr << ("She was drenched in ");
		random = g_Dice % 10 + 1;
		/* */if (random <= 2)	OStr << ("cum");
		else if (random <= 4)	OStr << ("sweat");
		else if (random <= 6) 	OStr << ("broken hopes and dreams");
		else if (random <= 8)	OStr << ("Koolaid");
		else /*            */	OStr << ("sticky secretions");
		OStr << (" by");
		break;
	case 4:
		OStr << ("She ");
		random = g_Dice % 10 + 1;
		/* */if (random <= 2)	OStr << ("moaned");
		else if (random <= 4)	OStr << ("winced");
		else if (random <= 6) 	OStr << ("swooned");
		else if (random <= 8) 	OStr << ("orgasmed");
		else /*            */	OStr << ("begged for more");
		OStr << (" as her stomach repeatedly poked out from ");
		random = g_Dice % 8 + 1;
		/* */if (random <= 2)	OStr << ("thrusts");
		else if (random <= 4)	OStr << ("strokes");
		else if (random <= 6) 	OStr << ("fistings");
		else /*            */	OStr << ("a powerful fucking");
		OStr << (" by");
		break;
	case 5:
		OStr << ("She used her ");
		if (g_Dice % 10 + 1 <= 5)	{ NeedAnd = true;	OStr << ("hands, "); }
		if (g_Dice % 10 + 1 <= 5)	{ NeedAnd = true;	OStr << ("feet, "); }
		if (g_Dice % 10 + 1 <= 5)	{ NeedAnd = true;	OStr << ("mouth, "); }
		if (NeedAnd == true)		{ NeedAnd = false;	OStr << ("and "); }
		if (g_Dice % 10 + 1 <= 5)	OStr << ("pussy");
		else /*            */	OStr << ("holes");
		OStr << (" to ");
		random = g_Dice % 6 + 1;
		/* */if (random <= 2)	OStr << ("please");
		else if (random <= 4)	OStr << ("pleasure");
		else /*            */	OStr << ("tame");
		break;
	case 6:
		OStr << ("She shook with ");
		random = g_Dice % 10 + 1;
		/* */if (random <= 2)	OStr << ("orgasmic joy");
		else if (random <= 4)	OStr << ("searing pain");
		else if (random <= 6)	OStr << ("frustration");
		else if (random <= 8)	OStr << ("agony");
		else /*            */	OStr << ("frustrated boredom");
		OStr << (" when fondled by");
		break;
	case 7:
		OStr << ("It felt like she was ");
		random = g_Dice % 10 + 1;
		/* */if (random <= 2)	OStr << ("ripping apart");
		else if (random <= 4)	OStr << ("exploding");
		else if (random <= 6)	OStr << ("imploding");
		else if (random <= 8)	OStr << ("nothing");
		else /*            */	OStr << ("absent");
		OStr << (" when handled by");
		break;
	case 8:
		OStr << ("She passed out from ");
		random = g_Dice % 10 + 1;
		/* */if (random <= 2)	OStr << ("pleasure");
		else if (random <= 4)	OStr << ("pain");
		else if (random <= 6)	OStr << ("boredom");
		else if (random <= 8)	OStr << ("rough sex");
		else /*            */	OStr << ("inactivity");
		OStr << (" from");
		break;
	case 9:
		OStr << ("She screamed as ");
		random = g_Dice % 10 + 1;
		/* */if (random <= 2)	OStr << ("wrenching pain");
		else if (random <= 4)	OStr << ("powerful orgasms");
		else if (random <= 6)	OStr << ("incredible sensations");
		else if (random <= 8)	OStr << ("freight trains");
		else /*            */	OStr << ("lots and lots of nothing");
		OStr << (" thundered through her from");
		break;
	}
# pragma endregion beast2
	// Part 3
# pragma region beast3
	OStr << " ";
	roll3 = g_Dice % 12 + 1;
	switch (roll3)
	{
	case 1:
		OStr << ("the ravenous ");
		random = g_Dice % 10 + 1;
		/* */if (random <= 2)	OStr << ("tentacles.");
		else if (random <= 4)	OStr << (", sex-starved essences of lust.");
		else if (random <= 6)	OStr << ("Balhannoth. (Monster Manual 4, pg. 15.)");
		else if (random <= 8)	OStr << ("priest.");
		else /*            */	OStr << ("Yugoloth.");
		break;
	case 2:
		random = g_Dice % 10 + 1;
		/* */if (random <= 5)	OStr << ("an evil");
		else /*            */	OStr << ("a misunderstood");
		OStr << " ";
		random = g_Dice % 10 + 1;
		/* */if (random <= 2)	OStr << ("tengu.");
		else if (random <= 4)	OStr << ("Whore Master developer.");
		else if (random <= 6)	OStr << ("school girl.");
		else if (random <= 8)	OStr << ("garden hose.");
		else /*            */	OStr << ("thought.");
		break;
	case 3:
		OStr << ("a major ");
		random = g_Dice % 12 + 1;
		/* */if (random <= 2)	OStr << ("demon");
		else if (random <= 4)	OStr << ("devil");
		else if (random <= 6)	OStr << ("oni");
		else if (random <= 8)	OStr << ("fire elemental");
		else if (random <= 10)	OStr << ("god");
		else /*            */	OStr << ("Mr. Coffee");
		OStr << (" from the outer planes.");
		break;
	case 4:
		OStr << ("the angel.");
		random = g_Dice % 10 + 1;
		if (random <= 5)
		{
			OStr << (" ('");
			random = g_Dice % 8 + 1;
			/* */if (random <= 2)	OStr << ("You're very pretty");
			else if (random <= 4)	OStr << ("I was never here");
			else if (random <= 6)	OStr << ("I had a great time");
			else /*            */	OStr << ("I didn't know my body could do that");
			OStr << ("' he said.)");
		}
		break;
	case 5:
		OStr << ("the ");
		random = g_Dice % 12 + 1;
		/* */if (random <= 2)	OStr << ("demon");
		else if (random <= 4)	OStr << ("major devil");
		else if (random <= 6)	OStr << ("oni");
		else if (random <= 8)	OStr << ("earth elemental");
		else if (random <= 10)	OStr << ("raging hormome beast");
		else /*            */	OStr << ("Happy Fun Ball");
		OStr << (" with an urge to exercise his ");
		random = g_Dice % 30 + 20;
		OStr << random;
		OStr << (" cocks and ");
		random = g_Dice % 30 + 20;
		OStr << random;
		OStr << (" claws.");
		break;
	case 6: OStr << ("the swamp thing with (wait for it) swamp tentacles!"); break;
	case 7: OStr << ("the pirnja gestalt. (The revolution is coming.)"); break;
	case 8:
		OStr << ("the color ");
		random = g_Dice % 12 + 1;
		/* */if (random <= 2)	OStr << ("purple");
		else if (random <= 4)	OStr << ("seven");  // MYR: Not a mistake. I meant to write 'seven'.
		else if (random <= 6)	OStr << ("mauve");
		else if (random <= 8)	OStr << ("silver");
		else if (random <= 10)	OStr << ("ochre");
		else /*            */	OStr << ("pale yellow");
		OStr << (".");
		break;
	case 9:
		random = g_Dice % 10 + 5;
		OStr << random;
		OStr << (" werewolves wearing ");
		random = g_Dice % 10 + 1;
		/* */if (random <= 2)	OStr << ("true");
		else if (random <= 4)	OStr << ("minor artifact");
		else if (random <= 6)	OStr << ("greater artifact");
		else if (random <= 10)	OStr << ("godly");
		else /*            */	OStr << ("near omnipitent");
		OStr << (" rings of the ");
		random = g_Dice % 10 + 1;
		/* */if (random <= 2)	OStr << ("eternal");
		else if (random <= 4)	OStr << ("body breaking");
		else if (random <= 6)	OStr << ("vorporal");
		else if (random <= 10)	OStr << ("transcendent");
		else /*            */	OStr << ("incorporeal");
		OStr << (" hard-on.");
		break;
	case 10:
		random = g_Dice % 10 + 5;
		OStr << random;
		OStr << (" Elder Gods.");
		random = g_Dice % 10 + 1;
		if (random <= 4)
		{
			OStr << (" (She thought ");
			random = g_Dice % 12 + 1;
			/* */if (random <= 2)	OStr << ("Cthulhu");
			else if (random <= 4)	OStr << ("Hastur");
			else if (random <= 6)	OStr << ("an Old One");
			else if (random <= 8)	OStr << ("Shub-Niggurath");
			else if (random <= 10)	OStr << ("Nyarlathotep");
			else /*            */	OStr << ("Yog-Sothoth");
			OStr << (" was amongst them, but blacked out after a minute or so.)");
		}
		break;
	case 11:
		OStr << ("the level ");
		random = g_Dice % 20 + 25;
		OStr << random;
		OStr << (" epic paragon ");
		random = g_Dice % 12 + 1;
		/* */if (random <= 2)	OStr << ("troll");
		else if (random <= 4)	OStr << ("beholder");
		else if (random <= 6)	OStr << ("displacer beast");
		else if (random <= 8)	OStr << ("ettin");
		else if (random <= 10)	OStr << ("gargoyle");
		else /*            */	OStr << ("fire extinguisher");
		OStr << (" with ");
		random = g_Dice % 20 + 20;
		OStr << random;
		OStr << (" strength and ");
		random = g_Dice % 20 + 20;
		OStr << random;
		OStr << (" constitution.");
		break;
	case 12:
		OStr << ("the phalanx of ");
		random = g_Dice % 10 + 1;
		if (random <= 2)	OStr << ("horny orcs.");
		else if (random <= 4)	OStr << ("goblins.");
		else if (random <= 6)	OStr << ("sentient marbles.");
		else if (random <= 8)	OStr << ("living garden gnomes.");
		else /*            */	OStr << ("bugbears.");
		break;
	}
# pragma endregion beast3
	OStr << ("\n");
	return OStr.str();
}
string cGirls::GetRandomLesString()
{
	int roll1 = 0, roll2 = 0, roll3 = 0, random = 0, plus = 0;
	stringstream OStr;
	OStr << " ";
	// Part1
# pragma region les1
	roll1 = g_Dice % 6 + 1;   // Remember to update this when new strings are added
	switch (roll1)
	{
	case 1:
		random = g_Dice % 10 + 1;
		/* */if (random <= 2)	OStr << ("aggressively");
		else if (random <= 4)	OStr << ("tenderly");
		else if (random <= 6)	OStr << ("slowly");
		else if (random <= 8) 	OStr << ("authoratively");
		else /*            */	OStr << ("violently");
		OStr << " ";
		random = g_Dice % 10 + 1;
		/* */if (random <= 2)	OStr << ("straddled");
		else if (random <= 4)	OStr << ("scissored");
		else if (random <= 6)	OStr << ("symmetrically docked with");
		else if (random <= 8) 	OStr << ("cowgirled");
		else /*            */	OStr << ("69ed");
		OStr << (" the woman");
		break;
	case 2:
		random = g_Dice % 10 + 1;
		if (random <= 5)	OStr << ("shaved her");
		else /*          */	OStr << ("was shaved");
		OStr << (" with a +");
		plus = g_Dice % 7 + 4;
		OStr << plus;
		OStr << " ";
		random = g_Dice % 14 + 1;
		/* */if (random <= 2)	OStr << ("vorporal broadsword");
		else if (random <= 4)
		{
			OStr << ("dagger, +");
			plus = plus + g_Dice % 5 + 2;
			OStr << plus;
			OStr << (" vs pubic hair");
		}
		else if (random <= 6)	OStr << ("flaming sickle");
		else if (random <= 8) 	OStr << ("lightning burst bo-staff");
		else if (random <= 10)	OStr << ("human bane greatsword");
		else if (random <= 12)	OStr << ("acid burst warhammer");
		else /*            */	OStr << ("feral halfling");
		break;
	case 3:
		OStr << ("had a ");
		random = g_Dice % 14 + 1;
		/* */if (random <= 2)	OStr << ("pleasant");
		else if (random <= 4)	OStr << ("long");
		else if (random <= 6)	OStr << ("heartfelt");
		else if (random <= 8)	OStr << ("deeply personal");
		else if (random <= 10)	OStr << ("emotional");
		else if (random <= 12)	OStr << ("angry");
		else /*            */	OStr << ("violent");
		OStr << (" conversation with her lady-client about ");
		random = g_Dice % 16 + 1;
		/* */if (random <= 2)	OStr << ("sadism");
		else if (random <= 4)	OStr << ("particle physics");
		else if (random <= 6)	OStr << ("domination");
		else if (random <= 8) 	OStr << ("submission");
		else if (random <= 10)	OStr << ("brewing poisons");
		else if (random <= 12) 	OStr << ("flower arranging");
		else if (random <= 14)	OStr << ("the Brothel Master");
		else /*            */	OStr << ("assassination techniques");
		break;
	case 4:
		OStr << ("massaged the woman with ");
		// MYR: Ok, I know I'm being super-silly
		random = g_Dice % 20 + 1;
		/* */if (random <= 2)	OStr << ("bath oil");
		else if (random <= 4)	OStr << ("aloe vera");
		else if (random <= 6)	OStr << ("the tears of Chuck Norris's many victims");
		else if (random <= 8)	OStr << ("the blood of innocent angels");
		else if (random <= 10)	OStr << ("Unicorn blood");
		else if (random <= 12)	OStr << ("Unicorn's tears");
		else if (random <= 14)	OStr << ("a strong aphrodisiac");
		else if (random <= 16)	OStr << ("oil of greater breast growth");
		else if (random <= 18)	OStr << ("potent oil of massive breast growth");
		else /*            */	OStr << ("oil of camel-toe growth");
		break;
	case 5:
		random = g_Dice % 10 + 1;
		/* */if (random <= 5)	OStr << ("put a ball gag and blindfolded on");
		else /*            */	OStr << ("put a sensory deprivation hood on");
		OStr << (", was ");
		random = g_Dice % 8 + 1;
		/* */if (random <= 2)	OStr << ("lashed");
		else if (random <= 4)	OStr << ("cuffed");
		else if (random <= 6)	OStr << ("tied");
		else /*            */	OStr << ("chained");
		OStr << (" to a ");
		random = g_Dice % 8 + 1;
		/* */if (random <= 2)	OStr << ("bed");
		else if (random <= 4)	OStr << ("bench");
		else if (random <= 6)	OStr << ("table");
		else /*            */	OStr << ("post");
		OStr << (" and ");
		random = g_Dice % 8 + 1;
		/* */if (random <= 2)	OStr << ("symmetrically docked");
		else if (random <= 4)	OStr << ("69ed");
		else if (random <= 6)	OStr << ("straddled");
		else /*            */	OStr << ("scissored");
		break;
	case 6:
		// MYR: This is like a friggin movie! The epic story of the whore and her customer.
		OStr << ("looked at the woman across from her. ");
		random = g_Dice % 10 + 1;
		/* */if (random <= 2)	OStr << ("Position");
		else if (random <= 4)	OStr << ("Toy");
		else if (random <= 6)	OStr << ("Oil");
		else if (random <= 8)	OStr << ("Bed sheet color");
		else /*            */	OStr << ("Price");
		OStr << (" was to be ");
		random = g_Dice % 8 + 1;
		/* */if (random <= 2)	OStr << ("a trial");
		else if (random <= 4)	OStr << ("decided");
		else if (random <= 6)	OStr << ("resolved");
		else /*            */	OStr << ("dictated");
		OStr << (" by combat. Both had changed into ");
		random = g_Dice % 10 + 1;
		/* */if (random <= 2)	OStr << ("string bikinis");
		else if (random <= 4)	OStr << ("lingerie");
		else if (random <= 6)	OStr << ("body stockings");
		else if (random <= 8)	OStr << ("their old school uniforms");
		else /*            */	OStr << ("dominatrix outfits");
		OStr << (" and wielded ");
		random = g_Dice % 8 + 1;
		/* */if (random <= 2)	OStr << ("whips");
		else if (random <= 4)	OStr << ("staves");
		else if (random <= 6)	OStr << ("boxing gloves");
		else /*            */	OStr << ("cat-o-nine tails");
		OStr << (" of ");
		random = g_Dice % 10 + 1;
		/* */if (random <= 2)	OStr << ("explosive orgasms");
		else if (random <= 4)	OStr << ("clothes shredding");
		else if (random <= 6)	OStr << ("humiliation");
		else if (random <= 8) 	OStr << ("subjugation");
		else /*            */	OStr << ("brutal stunning");
		OStr << (". ");
		random = g_Dice % 10 + 1;
		if (random <= 5)
		{
			OStr << ("They stared at each other across the ");
			random = g_Dice % 8 + 1;
			/* */if (random <= 2)	OStr << ("mud");
			else if (random <= 4)	OStr << ("jello");
			else if (random <= 6)	OStr << ("whip cream");
			else /*            */	OStr << ("clothes-eating slime");
			OStr << (" pit.");
		}
		OStr << (" A bell sounded! They charged and ");
		random = g_Dice % 6 + 1;
		/* */if (random <= 2)	OStr << ("dueled");
		else if (random <= 4)	OStr << ("fought it out");
		else if (random <= 6)	OStr << ("battled");
		OStr << ("!\n");
		random = g_Dice % 10 + 1;
		if (random <= 6)	OStr << ("The customer won");
		else /*          */	OStr << ("The customer was vanquished");
		break;
	}
# pragma endregion les1
	OStr << (". ");
	// Part 2
# pragma region les2
	roll2 = g_Dice % 8 + 1;
	switch (roll2)
	{
	case 1:
		OStr << ("She was ");
		random = g_Dice % 10 + 1;
		/* */if (random <= 2)	OStr << ("tormented");
		else if (random <= 4)	OStr << ("teased");
		else if (random <= 6)	OStr << ("massaged");
		else if (random <= 8) 	OStr << ("frustrated");
		else /*            */	OStr << ("satisfied");
		OStr << (" with ");
		random = g_Dice % 10 + 1;
		/* */if (random <= 2)	OStr << ("great care");
		else if (random <= 4)	OStr << ("deva feathers");
		else if (random <= 6)	OStr << ("drug-soaked sex toys");
		else if (random <= 8) 	OStr << ("extreme skill");
		else /*            */	OStr << ("wild abandon");
		OStr << (" by");
		break;
	case 2:
		// Case 1 reversed and reworded
		OStr << ("She used ");
		random = g_Dice % 10 + 1;
		/* */if (random <= 2)	OStr << ("phoenix down");
		else if (random <= 4)	OStr << ("deva feathers");
		else if (random <= 6)	OStr << ("drug-soaked sex toys");
		else if (random <= 8) 	OStr << ("restraints");
		else /*            */	OStr << ("her wiles");
		OStr << (" to ");
		random = g_Dice % 10 + 1;
		/* */if (random <= 2)	OStr << ("torment");
		else if (random <= 4)	OStr << ("tease");
		else if (random <= 6)	OStr << ("massage");
		else if (random <= 8) 	OStr << ("frustrate");
		else /*            */	OStr << ("satisfy");
		break;
	case 3:
		OStr << ("She ");
		random = g_Dice % 10 + 1;
		/* */if (random <= 2)	OStr << ("fingered");
		else if (random <= 4)	OStr << ("teased");
		else if (random <= 6)	OStr << ("caressed");
		else if (random <= 8) 	OStr << ("fondled");
		else /*            */	OStr << ("pinched");
		OStr << (" the client's ");
		random = g_Dice % 6 + 1;
		/* */if (random <= 2)	OStr << ("clit");
		else if (random <= 4)	OStr << ("clitorus");
		else /*            */	OStr << ("love bud");
		OStr << (" and expertly elicited orgasm after orgasm from");
		break;
	case 4:
		OStr << ("Her ");
		random = g_Dice % 6 + 1;
		/* */if (random <= 2)	OStr << ("clit");
		else if (random <= 4)	OStr << ("clitorus");
		else /*            */	OStr << ("love bud");
		OStr << (" was ");
		random = g_Dice % 10 + 1;
		/* */if (random <= 2)	OStr << ("fingered");
		else if (random <= 4)	OStr << ("teased");
		else if (random <= 6)	OStr << ("caressed");
		else if (random <= 8) 	OStr << ("fondled");
		else /*            */	OStr << ("pinched");
		OStr << (" and she orgasmed repeatedly under the expert touch of");
		break;
	case 5:
		OStr << ("She ");
		random = g_Dice % 8 + 1;
		/* */if (random <= 2)	OStr << ("clam wrestled");
		else if (random <= 4)	OStr << ("rubbed");
		else if (random <= 6)	OStr << ("attacked");
		else /*            */	OStr << ("hammered");
		OStr << (" the client's ");
		random = g_Dice % 8 + 1;
		/* */if (random <= 2)	OStr << ("clit");
		else if (random <= 4)	OStr << ("clitorus");
		else if (random <= 6)	OStr << ("love bud");
		else /*            */	OStr << ("vagina");
		OStr << (" causing waves of orgasms to thunder through");
		break;
	case 6:
		OStr << ("She ");
		random = g_Dice % 6 + 1;
		/* */if (random <= 2)	OStr << ("single mindedly");
		else if (random <= 4)	OStr << ("repeatedly");
		else /*            */	OStr << ("roughly");
		OStr << " ";
		random = g_Dice % 2 + 1;
		/* */if (random <= 2)	OStr << ("rubbed");
		else if (random <= 4)	OStr << ("fondled");
		else if (random <= 6)	OStr << ("prodded");
		else if (random <= 8)	OStr << ("attacked");
		else if (random <= 10)	OStr << ("tongued");
		else /*            */	OStr << ("licked");
		OStr << (" the client's g-spot. Wave after wave of ");
		random = g_Dice % 8 + 1;
		/* */if (random <= 2)	OStr << ("orgasms");
		else if (random <= 4)	OStr << ("pleasure");
		else if (random <= 6)	OStr << ("powerful sensations");
		else /*            */	OStr << ("indescribable joy");
		OStr << " ";
		random = g_Dice % 8 + 1;
		/* */if (random <= 2)	OStr << ("rushed");
		else if (random <= 4)	OStr << ("thundered");
		else if (random <= 6)	OStr << ("cracked");
		else /*            */	OStr << ("pounded");
		OStr << (" through");
		break;
	case 7:
		OStr << ("Wave after wave of ");
		random = g_Dice % 8 + 1;
		/* */if (random <= 2)	OStr << ("orgasms");
		else if (random <= 4)	OStr << ("back-stretching joy");
		else if (random <= 6)	OStr << ("madness");
		else /*            */	OStr << ("incredible feeling");
		OStr << " ";
		random = g_Dice % 8 + 1;
		/* */if (random <= 2)	OStr << ("throbbed");
		else if (random <= 4)	OStr << ("shook");
		else if (random <= 6)	OStr << ("arced");
		else /*            */	OStr << ("stabbed");
		OStr << (" through her as she was ");
		random = g_Dice % 6 + 1;
		/* */if (random <= 2)	OStr << ("single mindedly");
		else if (random <= 4)	OStr << ("repeatedly");
		else /*            */	OStr << ("roughly");
		OStr << " ";
		random = g_Dice % 12 + 1;
		/* */if (random <= 2)	OStr << ("rubbed");
		else if (random <= 4)	OStr << ("fondled");
		else if (random <= 6)	OStr << ("prodded");
		else if (random <= 8)	OStr << ("attacked");
		else if (random <= 10)	OStr << ("tongued");
		else /*            */	OStr << ("licked");
		OStr << (" by");
		break;
	case 8:
		// MYR: I just remembered about \n
		OStr << ("Work stopped ");
		random = g_Dice % 14 + 1;
		/* */if (random <= 2)	OStr << ("in the brothel");
		else if (random <= 4)	OStr << ("on the street");
		else if (random <= 6)	OStr << ("all over the block");
		else if (random <= 8)	OStr << ("in the town");
		else if (random <= 10)	OStr << ("within the country");
		else  if (random <= 12)	OStr << ("over the whole planet");
		else  if (random <= 12)	OStr << ("within the solar system");
		else /*            */	OStr << ("all over the galactic sector");
		OStr << (". Everything was drowned out by:\n \n");
		OStr << ("Ahhhhh!\n \n");
		random = g_Dice % 10 + 1;
		if (random <= 5)
		{
			random = g_Dice % 6 + 1;
			/* */if (random <= 2)	OStr << ("For the love... of aaaaahhhhh mercy.  No nnnnnnnnh more!\n \n");
			else if (random <= 4)	OStr << ("oooooOOOOOO YES! ahhhhhhHHHH!\n \n");
			else /*            */	OStr << ("nnnnnhhh nnnnnhhhh NNNHHHHHH!!!!\n \n");
		}
		OStr << ("Annnnnhhhhhaaa!\n \n");
		OStr << ("AHHHHHHHH! I'm going to ");
		random = g_Dice % 12 + 1;
		/* */if (random <= 2)	OStr << ("CCCUUUUUUMMMMMMMM!!!!!");
		else if (random <= 4)	OStr << ("EEEXXXXXPLLLOOODDDDEEEE!!!");
		else if (random <= 6)	OStr << ("DIEEEEEE!");
		else if (random <= 8)	OStr << ("AHHHHHHHHHHH!!!!");
		else if (random <= 10)	OStr << "BRRRREEEEAAAAKKKKKK!!!!";
		else /*            */	OStr << "WAAAAHHHHHOOOOOOO!!!";
		OStr << ("\nfrom ");
		break;
	}
# pragma endregion les2
	OStr << " ";
	// Part 3
# pragma region les3
	// For case 2
	int BrothelNo = -1, NumGirlsInBroth = -1;
	sGirl * TempGPtr = 0;
	roll3 = g_Dice % 6 + 1;
	switch (roll3)
	{
	case 1:
		OStr << ("the ");
		random = g_Dice % 10 + 1;
		/* */if (random <= 2)	OStr << "prostitute";
		else if (random <= 4)	OStr << "street worker";
		else if (random <= 6)	OStr << "hooker";
		else if (random <= 8)	OStr << "street walker";
		else /*            */	OStr << "working girl";
		OStr << (" from a ");
		random = g_Dice % 10 + 1;
		/* */if (random <= 2)	OStr << "friendly";
		else if (random <= 4)	OStr << "rival";
		else if (random <= 6)	OStr << "hostile";
		else 	OStr << "feeder";
		OStr << " brothel.";
		break;
	case 2:
		BrothelNo = g_Dice%g_Brothels.GetNumBrothels();
		NumGirlsInBroth = g_Brothels.GetNumGirls(BrothelNo);
		random = g_Dice%NumGirlsInBroth;
		TempGPtr = g_Brothels.GetGirl(BrothelNo, random);
		/* */if (TempGPtr == 0)	OStr << "a girl";
		else /*            */	OStr << TempGPtr->m_Realname;
		OStr << " from ";
		OStr << g_Brothels.GetName(BrothelNo);
		OStr << " brothel.";
		BrothelNo = -1;        // MYR: Paranoia
		NumGirlsInBroth = -1;
		TempGPtr = 0;
		break;
	case 3:
		OStr << "the ";
		random = g_Dice % 12 + 1;
		/* */if (random <= 2)	OStr << "sexy";
		else if (random <= 4)	OStr << "rock hard";
		else if (random <= 6)	OStr << "hot";
		else if (random <= 8)	OStr << "androgynous";
		else if (random <= 10)	OStr << "spirited";
		else /*            */	OStr << "exuberant";
		OStr << " MILF.";
		break;
	case 4:
		OStr << "the ";
		random = g_Dice % 10 + 1;
		/* */if (random <= 5)	OStr << "senior";
		else /*            */	OStr << "junior";
		OStr << " ";
		random = g_Dice % 12 + 1;
		/* */if (random <= 2)	OStr << "Sorceress";
		else if (random <= 4)	OStr << "Warrioress";
		else if (random <= 6)	OStr << "Priestess";
		else if (random <= 8)	OStr << "Huntress";
		else if (random <= 10)	OStr << "Amazon";
		else /*            */	OStr << "Druidess";
		OStr << (" of the ");
		random = g_Dice % 10 + 1;
		/* */if (random <= 2)	OStr << "Hidden";
		else if (random <= 4)	OStr << "Silent";
		else if (random <= 6)	OStr << "Masters";
		else if (random <= 8)	OStr << "Scarlet";
		else /*            */	OStr << "Resolute";
		OStr << " ";
		random = g_Dice % 10 + 1;
		/* */if (random <= 2)	OStr << "Hand";
		else if (random <= 4)	OStr << "Dagger";
		else if (random <= 6)	OStr << "Will";
		else if (random <= 8)	OStr << "League";
		else /*            */	OStr << "Hearts";
		OStr << ".";
		break;
	case 5:
		OStr << "the ";
		random = g_Dice % 6 + 1;
		/* */if (random <= 2)	OStr << "high-ranking";
		else if (random <= 4)	OStr << "mid-tier";
		else /*            */	OStr << "low-ranking";
		OStr << " ";
		random = g_Dice % 14 + 1;
		/* */if (random <= 2)	OStr << "elf";
		else if (random <= 4)	OStr << "woman";     // MYR: Human assumed
		else if (random <= 6)	OStr << "dryad";
		else if (random <= 8)	OStr << "succubus";
		else if (random <= 10)	OStr << "nymph";
		else if (random <= 12)	OStr << "eyrine";
		else /*            */	OStr << "cat girl";
		OStr << " from the ";
		random = g_Dice % 10 + 1;
		/* */if (random <= 2)	OStr << "Nymphomania";
		else if (random <= 4)	OStr << "Satyriasis";
		else if (random <= 6)	OStr << "Women Who Love Sex";
		else if (random <= 8)	OStr << "Real Women Don't Marry";
		else /*            */	OStr << "Monster Sex is Best";
		OStr << " ";
		random = g_Dice % 10 + 1;
		/* */if (random <= 2)	OStr << "support group";
		else if (random <= 4)	OStr << "league";
		else if (random <= 6)	OStr << "club";
		else if (random <= 8)	OStr << "faction";
		else /*            */	OStr << "guild";
		OStr << ".";
		break;
	case 6:
		OStr << "the ";
		random = g_Dice % 10 + 1;
		/* */if (random <= 2)	OStr << "disguised";
		else if (random <= 4)	OStr << "hot";
		else if (random <= 6)	OStr << "sexy";
		else if (random <= 8)	OStr << "curvacious";
		else /*            */	OStr << "sultry";
		OStr << " ";
		// MYR: Covering the big fetishes/stereotpes
		random = g_Dice % 12 + 1;
		/* */if (random <= 2)	OStr << "idol singer";
		else if (random <= 4)	OStr << "princess";
		else if (random <= 6)	OStr << "school girl";
		else if (random <= 8)	OStr << "nurse";
		else if (random <= 10)	OStr << "maid";
		else /*            */	OStr << "waitress";
		OStr << ".";
		break;
	}
# pragma endregion les3
	OStr << "\n";
	return OStr.str();
}
// MYR: Burned out before anal. Someone else feeling creative?
string cGirls::GetRandomAnalString()
{
	int roll1 = 0, roll2 = 0, roll3 = 0;
	stringstream OStr;
	OStr << " ";
	// Part 1
#pragma region anal1
	roll1 = g_Dice % 10 + 1;   // Remember to update this when new strings are added
	switch (roll1)
	{
	case 1: OStr << ""; break;
	case 2: OStr << ""; break;
	case 3: OStr << ""; break;
	case 4: OStr << ""; break;
	case 5: OStr << ""; break;
	case 6: OStr << ""; break;
	case 7: OStr << ""; break;
	case 8: OStr << ""; break;
	case 9: OStr << ""; break;
	case 10: OStr << ""; break;
	}
#pragma endregion anal1
	OStr << ". ";
	// Part 2
#pragma region anal2
	roll2 = g_Dice % 10 + 1;
	switch (roll2)
	{
	case 1: OStr << ""; break;
	case 2: OStr << ""; break;
	case 3: OStr << ""; break;
	case 4: OStr << ""; break;
	case 5: OStr << ""; break;
	case 6: OStr << ""; break;
	case 7: OStr << ""; break;
	case 8: OStr << ""; break;
	case 9: OStr << ""; break;
	case 10: OStr << ""; break;
	}
#pragma endregion anal2
	OStr << " ";
	// Part 3
#pragma endregion anal3
	roll3 = g_Dice % 10 + 1;
	switch (roll3)
	{
	case 1: OStr << ""; break;
	case 2: OStr << ""; break;
	case 3: OStr << ""; break;
	case 4: OStr << ""; break;
	case 5: OStr << ""; break;
	case 6: OStr << ""; break;
	case 7: OStr << ""; break;
	case 8: OStr << ""; break;
	case 9: OStr << ""; break;
	case 10: OStr << ""; break;
	}
#pragma endregion anal3
	return OStr.str();
}

// ----- Combat

Uint8 cGirls::girl_fights_girl(sGirl* a, sGirl* b)
{
	// NB: For girls exploring the catacombs, a is the character, b the monster
	// Return value of 1 means a (the girl) won
	//   "      "    " 2  "   b (the monster) won
	//   "      "    " 0  "   it was a draw

	CLog l;

	if (a == 0 || b == 0)
	{
		l.ss() << "\ngirl_fights_girl: ";
		if (a == 0 && b == 0)	l.ss() << "No one";
		else if (a == 0)		l.ss() << "Only " << b->m_Realname;
		else if (b == 0)		l.ss() << "Only " << a->m_Realname;
		l.ss() << " showed up for the fight so no one wins?\n";
		return 0;
	}
	// MYR: Sanity checks on incorporeal. It is actually possible (but very rare)
	//      for both girls to be incorporeal.
	if (a->has_trait("Incorporeal") && b->has_trait("Incorporeal"))
	{
		l.ss() << "\ngirl_fights_girl: Both " << a->m_Realname << " and " << b->m_Realname
			<< " are incorporeal, so the fight is a draw.\n";
		return 0;
	}
	else if (a->has_trait("Incorporeal"))
	{
		l.ss() << "\ngirl_fights_girl: " << a->m_Realname << " is incorporeal, so she wins.\n";
		return 1;
	}
	else if (b->has_trait("Incorporeal"))
	{
		l.ss() << "\ngirl_fights_girl: " << b->m_Realname << " is incorporeal, so she wins.\n";
		return 2;
	}

	int a_dodge = 0;
	int b_dodge = 0;
	u_int a_attack = SKILL_COMBAT;	// determined later, defaults to combat
	u_int b_attack = SKILL_COMBAT;




	// first determine what skills they will fight with
	// girl a
	if (a->combat() >= a->magic())
		a_attack = SKILL_COMBAT;
	else
		a_attack = SKILL_MAGIC;

	// girl b
	if (b->combat() >= b->magic())
		b_attack = SKILL_COMBAT;
	else
		b_attack = SKILL_MAGIC;

	// determine dodge ability
	// girl a
	if ((a->agility() - a->tiredness()) < 0)
		a_dodge = 0;
	else
		a_dodge = a->agility() - a->tiredness();

	// girl b
	if ((b->agility() - b->tiredness()) < 0)
		b_dodge = 0;
	else
		b_dodge = b->agility() - b->tiredness();

	l.ss() << "Girl vs. Girl: " << a->m_Realname << " fights " << b->m_Realname << "\n";
	l.ss() << "\t" << a->m_Realname << ": Health " << a->health() << ", Dodge " << a_dodge << ", Mana " << a->mana() << "\n";
	l.ss() << "\t" << b->m_Realname << ": Health " << b->health() << ", Dodge " << b_dodge << ", Mana " << b->mana() << "\n";
	l.ssend();

	sGirl* Attacker = b;
	sGirl* Defender = a;
	unsigned int attack = 0;
	int dodge = a_dodge;
	int attack_count = 0;
	int winner = 0; // 1 for a, 2 for b
	while (1)
	{
		if (a->health() <= 20)
		{
			a->upd_Enjoyment(ACTION_COMBAT, -1);
			winner = 2;
			break;
		}
		else if (b->health() <= 20)
		{
			b->upd_Enjoyment(ACTION_COMBAT, -1);
			winner = 1;
			break;
		}

		if (attack_count > 1000)  // if the fight's not over after 1000 blows, call it a tie
		{
			l.ss() << "The fight has gone on for too long, over 1000 (attempted) blows either way. Calling it a draw.";
			l.ssend();

			return 0;
		}
		attack_count++;

		if (Attacker == a)
		{
			Attacker = b;
			attack = b_attack;
			Defender = a;
			b_dodge = dodge;
			dodge = a_dodge;
		}
		else if (Attacker == b)
		{
			Attacker = a;
			attack = a_attack;
			Defender = b;
			a_dodge = dodge;
			dodge = b_dodge;
		}

		// Girl attacks
		l.ss() << "\t\t" << Attacker->m_Realname << " attacks: ";

		if (attack == SKILL_MAGIC)
		{
			if (Attacker->mana() < 7)
				l.ss() << "Insufficient mana: using combat";
			else
			{
				Attacker->mana(-7);
				l.ss() << "Casts a spell (mana now " << Attacker->mana() << ")";
			}
		}
		else
			l.ss() << "Using physical attack";

		l.ssend();

		int girl_attack_chance = Attacker->get_skill(attack);
		int die_roll = g_Dice.d100();

		l.ss() << "\t\t" << "Attack chance: " << girl_attack_chance << " Die roll: " << die_roll;
		l.ssend();

		if (die_roll > girl_attack_chance)
			l.ss() << "\t\t\tMiss!";
		else
		{
			int damage = g_Girls.GetCombatDamage(Attacker, attack);
			l.ss() << "\t\t\t" << "Hit! base damage is " << damage << ". ";

			die_roll = g_Dice.d100();

			// Defender attempts Dodge
			l.ss() << "\t\t\t\t" << Defender->m_Realname << " tries to dodge: needs " << dodge << ", gets " << die_roll << ": ";

			if (die_roll <= dodge)
			{
				l.ss() << "Success!";
				l.ssend();
			}
			else
			{
				l.ss() << "Failure! ";
				l.ssend();

				//int con_mod = Defender->m_Stats[STAT_CONSTITUTION] / 10;
				int con_mod = Defender->constitution() / 10;
				int ActualDmg = damage - con_mod;
				if (ActualDmg <= 0)
					l.ss() << "\t\t\t\t" << Defender->m_Realname << " shrugs it off.";
				else
				{
					Defender->health(-ActualDmg);
					l.ss() << "\t\t\t\t" << Defender->m_Realname << " takes " << damage << " damage, less " << con_mod << " for CON\n";
					l.ss() << "\t\t\t\t\tNew health value = " << Defender->health();
				}
			} // Didn't dodge
		}     // Hit
		l.ssend();

		// update girls dodge ability
		if ((dodge - 1) < 0)
			dodge = 0;
		else
			dodge--;

		// Check if girl 2 (the Defender) has been defeated
		// Highest damage is 27 pts.  Checking for health between 20 and 30 is a bug as it can be stepped over.
		if (Defender->health() <= 40 && Defender->health() >= 10)
		{
			Defender->upd_Enjoyment(ACTION_COMBAT, -1);
			break;
		}  // if defeated
	}  // while (1)

	// Girls exploring catacombs: Girl is "a" - and thus wins
	if (Attacker == a)
	{
		l.ss() << a->m_Realname << " WINS!";
		l.ssend();

		a->upd_Enjoyment(ACTION_COMBAT, +1);

		return 1;
	}
	if (Attacker == b)  // Catacombs: Monster wins
	{
		l.ss() << b->m_Realname << " WINS!";
		l.ssend();

		b->upd_Enjoyment(ACTION_COMBAT, +1);

		return 2;
	}

	l.ss() << "ERROR: cGirls::girl_fights_girl - Shouldn't reach the function bottom.";
	l.ssend();

	return 0;
}

/*
* passing the girl_wins boolean here to avoid confusion over
* what returning TRUE actually means
*/

// MYR: How should incorporeal factor in to this?

void sGirl::fight_own_gang(bool &girl_wins)
{
	girl_wins = false;
	vector<sGang*> v = g_Gangs.gangs_on_mission(MISS_GUARDING);
	/*
	*	we'll take goons from the top gang in the list
	*/
	sGang *gang = v[0];
	/*
	*	4 + 1 for each gang on guard duty
	*	that way there's a benefit to multiple gangs guarding
	*/
	int max_goons = 4 + v.size();
	/*
	*	to the maximum of the number in the gang
	*/
	if (max_goons > gang->m_Num) {
		max_goons = gang->m_Num;
	}
	/*
	*	now - sum the girl and gang stats
	*	we're not going to average the gangs.
	*	yes this gives them an unfair advantage
	*	that's the point of having 5:1 odds :)
	*/
	int girl_stats = combat() + magic() + intelligence();
	/*
	*	Now the gangs. I'm not factoring the girl's health
	*	because there's something dramatically satisfying
	*	about her breeaking out of the dungeon after being
	*	tortured near unto death, and then still beating the
	*	thugs up. You'd buy into it in a Hollywood blockbuster...
	*
	*	Annnnyway....
	*/
	int goon_stats = 0;
	for (int i = 0; i < max_goons; i++) {
		goon_stats += gang->combat() +
			gang->magic() +
			gang->intelligence()
			;
	}
	/*
	*	the girl's base chance of winning is determined by the stat ratio
	*/
	double odds = 1.0 * girl_stats / (goon_stats + girl_stats);
	/*
	*	let's add some trait based bonuses
	*	I'm not going to do any that are already reflected in stat values
	*	(so no "Psychic" bonus, no "Tough" either)
	*	we can streamline this with the trait overhaul
	*/
	if (has_trait("Clumsy"))		odds -= 0.05;
	if (has_trait("Broken Will"))	odds -= 0.10;
	if (has_trait("Meek"))		odds -= 0.05;
	if (has_trait("Dependant"))	odds -= 0.10;
	if (has_trait("Fearless"))	odds += 0.10;
	if (has_trait("Fleet of Foot"))	odds += 0.10;
	/*
	*	get it back into the 0 <= N <= 1 range
	*/
	if (odds < 0) odds = 0;
	if (odds > 1) odds = 1;
	/*
	*	roll the dice! If it passes then the girl wins
	*/
	if (g_Dice.percent(int(odds * 100))) {
		win_vs_own_gang(v, max_goons, girl_wins);
	}
	else {
		lose_vs_own_gang(v, max_goons, girl_stats, goon_stats, girl_wins);
	}
}

void sGirl::win_vs_own_gang(vector<sGang*> &v, int max_goons, bool &girl_wins)
{
	sGang *gang = v[0];
	girl_wins = true;
	/*
	*	Give her some damage from the combat. She won, so don't kill her.
	*	should really modify this for traits. And agility.
	*/
	int damage = g_Dice.random(10) * max_goons;
	health(-damage);
	if (health() == 0) health(1);
	/*
	*	now - how many goons die today?
	*	I'm assuming the girl's priority is to escape
	*	rather than "clearing the level" so let's have a base of
	*	1-5
	*/
	int casualties = g_Dice.in_range(1, 6);
	/*
	*	some more trait mods. Assasin adds an extra dice since
	*	she's been trained to deliver the coup-de-grace on a helpless
	*	opponent.
	*/
	if (has_trait("Assassin")) {
		casualties += g_Dice.in_range(1, 6);
	}
	if (has_trait("Adventurer")) {	// some level clearing instincts
		casualties += 2;
	}
	if (has_trait("Merciless")) casualties++;
	if (has_trait("Yandere")) casualties++;
	if (has_trait("Tsundere")) casualties++;
	if (has_trait("Meek")) casualties--;
	if (has_trait("Dependant")) casualties -= 2;  //misspelled fixed by crazy
	/*
	*	fleet of foot means she gets out by running away more than fighting
	*	so fewer fatalities
	*/
	if (has_trait("Fleet of Foot")) casualties -= 2;
	/*
	*	OK, apply the casualties and make sure it doesn't go negative
	*/
	gang->m_Num -= casualties;
	if (gang->m_Num < 0) {
		gang->m_Num = 0;

	}
}

void sGirl::lose_vs_own_gang(vector<sGang*> &v, int max_goons, int girl_stats, int gang_stats, bool &girl_wins)
{
	sGang *gang = v[0];
	girl_wins = false;
	/*
	*	She's going to get hurt some. Moderating this, we have the fact that
	*	the goons don't really want to damage her (lest the boss get all shouty)
	*	However, depending on how good she is, they may not have a choice
	*
	*	also, I don't want to kill her if she was a full health. I hate it when
	*	that happens. You can send a girl to the dungeons and she's dead before
	*	you've even had a chance to twirl your moustaches at her.
	*	So we need to know how much health she had in the first place
	*/
	int start_health = health();
	int damage = g_Dice.random(12) * max_goons;
	/*
	*	and if the gangs had the advantage, reduce the
	*	damage actually taken accordingly
	*/
	if (gang_stats > girl_stats) {
		damage *= girl_stats;
		damage /= gang_stats;
	}
	/*
	*	lastly, make sure this isn't going to kill her, if her health was
	*	> 90 in the first place
	*/
	if (start_health >= 90 && damage >= start_health) {
		damage = start_health - 1;
	}
	health(-damage);
	/*
	*	need a bit more detail here, really...
	*/
	int casualties = g_Dice.in_range(1, 6);
	casualties += 3;
	if (casualties < 0) casualties = 0;
	gang->m_Num -= casualties;
}

bool cGirls::GirlInjured(sGirl* girl, unsigned int unModifier)
{  // modifier: 5 = 5% chance, 10 = 10% chance
	/*
	*	WD	Injury was only possible if girl is pregnant or
	*		hasn't got the required traits.
	*
	*		Now check for injury first
	*		Use usigned int so can't pass negative chance
	*/
	//	bool injured = false;
	string message;
	int nMod = static_cast<int>(unModifier);

	// Sanity check, Can't get injured
	if (girl->has_trait("Incorporeal")) return false;
	if (girl->has_trait("Fragile")) nMod += nMod;
	if (girl->has_trait("Tough"))	nMod /= 2;

	// Did the girl get injured
	if (!g_Dice.percent(nMod))
		return false;
	/*
	*	INJURY PROCESSING
	*	Only injured girls continue past here
	*/

	// getting hurt badly could lead to scars
	if (
		g_Dice.percent(nMod * 2) &&
		!girl->has_trait("Small Scars") &&
		!girl->has_trait("Cool Scars") &&
		!girl->has_trait("Horrific Scars")
		)
	{
		//injured = true;
		int chance = g_Dice % 6;
		if (chance == 0)
		{
			girl->add_trait("Horrific Scars", false);
			if (g_Dice.percent(50))
			{
				message = "She was horribly injured, and now is now covered with Horrific Scars.";
			}
			else
			{
				message = "She was badly injured, and now has to deal with Horrific Scars.";
			}
		}
		else if (chance <= 2)
		{
			girl->add_trait("Small Scars", false);
			message = "She was injured and now has a couple of Small Scars.";
		}
		else
		{
			girl->add_trait("Cool Scars", false);
			message = "She was injured and scarred. As scars go however, at least they are pretty Cool Scars.";
		}
		girl->m_Events.AddMessage(message, IMGTYPE_PROFILE, EVENT_WARNING);
	}

	// in rare cases, she might even lose an eye
	if (
		g_Dice.percent((nMod / 2)) &&
		!girl->has_trait("One Eye") &&
		!girl->has_trait("Eye Patch")
		)
	{
		//injured = true;
		int chance = g_Dice % 3;
		if (chance == 0)
		{
			girl->add_trait("One Eye", false);
			message = "Oh, no! She was badly injured, and now only has One Eye!";
		}
		else
		{
			girl->add_trait("Eye Patch", false);
			message = "She was injured and lost an eye, but at least she has a cool Eye Patch to wear.";
		}
		girl->m_Events.AddMessage(message, IMGTYPE_PROFILE, EVENT_WARNING);
	}

	// or become fragile
	if (
		g_Dice.percent((nMod / 2))
		&& !girl->has_trait("Fragile")
		)
	{
		//injured = true;
		girl->add_trait("Fragile", false);
		message = "Her body has become rather Fragile due to the extent of her injuries.";
		girl->m_Events.AddMessage(message, IMGTYPE_PROFILE, EVENT_WARNING);
	}

	// and if pregnant, she might lose the baby; I'll assume inseminations can't be aborted so easily
	if (girl->carrying_human() && g_Dice.percent((nMod * 2)))
	{  // unintended abortion time
		//injured = true;
		girl->m_ChildrenCount[CHILD08_MISCARRIAGES]++;
		girl->clear_pregnancy();
		girl->happiness(-20);
		girl->spirit(-5);
		message = "Her unborn child has been lost due to the injuries she sustained, leaving her quite distraught.";
		girl->m_Events.AddMessage(message, IMGTYPE_PROFILE, EVENT_WARNING);
	}
	if (girl->carrying_monster() && g_Dice.percent((nMod)))
	{  // unintended abortion time
		//injured = true;
		girl->m_ChildrenCount[CHILD08_MISCARRIAGES]++;
		girl->clear_pregnancy();
		girl->happiness(-10);
		girl->spirit(-5);
		message = "The creature growing inside her has been lost due to the injuries she sustained, leaving her distraught.";
		girl->m_Events.AddMessage(message, IMGTYPE_PROFILE, EVENT_WARNING);
	}

	// Lose between 5 - 14 hp
	girl->health(-5 - g_Dice % 10);

	return true;
}

// MYR: Centralized the damage calculation and added in traits that should apply
//      This fn and TakeCombatDamage(..) are in the attack/defense/agility style of combat
//      and will probably be made redundanty by the new style

int cGirls::GetCombatDamage(sGirl *girl, int CombatType)
{
	int damage = max(1, girl->strength() / 10);

	// Some traits help for both kinds of combat
	// There are a number of them so I set them at one point each
	// This also has the effect that some traits actually do something in the
	// game now

	if (girl->has_trait("Psychic"))					damage++;
	if (girl->has_trait("Adventurer"))				damage++;
	if (girl->has_trait("Aggressive"))				damage++;
	if (girl->has_trait("Fearless"))				damage++;
	if (girl->has_trait("Yandere"))					damage++;
	if (girl->has_trait("Merciless"))				damage++;
	if (girl->has_trait("Sadistic"))				damage++;
	if (girl->has_trait("Twisted"))					damage++;
	if (unsigned(CombatType) == SKILL_MAGIC)
	{
		int mdamage = 2 + (girl->magic() / 5);

		// Depending on how you see magic, charisma can influence how it flows
		// (Think Dungeons and Dragons sorcerer)
		if (girl->has_trait("Charismatic"))			mdamage += 1;
		if (girl->has_trait("Muggle"))				mdamage -= 4;
		if (girl->has_trait("Weak Magic"))			mdamage -= 2;
		if (girl->has_trait("Strong Magic"))		mdamage += 2;
		if (girl->has_trait("Powerful Magic"))		mdamage += 4;

		// Same idea as charismatic.
		// Note that I love using brainwashing oil, so this hurts me more than
		// it hurts you
		if (girl->has_trait("Iron Will"))			mdamage += 2;
		if (girl->has_trait("Broken Will"))			mdamage -= 2;
		if (girl->has_trait("Strong Magic"))		mdamage += 2;
		// Can Mind Fucked people even work magic?
		if (girl->has_trait("Mind Fucked"))			mdamage -= 5;

		if (mdamage < 0) mdamage = 0;
		damage += mdamage;
	}
	else   // SKILL_COMBAT case
	{
		damage += girl->combat() / 10;
		if (girl->has_trait("Manly"))				damage += 2;
		if (girl->has_trait("Strong"))				damage += 2;
		if (girl->has_trait("Muscular"))			damage += 2;
		if (girl->has_trait("Brawler"))				damage += 1;
	}
	return damage;
}

// MYR: Separated out taking combat damage from taking damage from other sources
//     Combat damage can be lowered a bit by certain traits, where other kinds of
//     damage cannot
//
// Returns the new health value

int cGirls::TakeCombatDamage(sGirl* girl, int amt)
{

	if (girl->has_trait("Incorporeal"))
	{
		girl->m_Stats[STAT_HEALTH] = 100;	// WD: Sanity - Incorporeal health should allways be at 100%
		return 100;                         // MYR: Sanity is good. Moved to the top
	}
	if (amt == 0) return girl->m_Stats[STAT_HEALTH];

	// This function works with negative numbers, but we'll be flexible and take positive numbers as well
	int value = (amt > 0) ? amt * -1 : amt;

	// High con allows you to shrug off damage
	value = value + (girl->constitution() / 20);

	if (girl->has_trait("Fragile"))			value -= 3;		// Takes more damage
	if (girl->has_trait("Tough"))			value += 2;		// Takes less damage
	if (girl->has_trait("Adventurer"))		value += 1;
	if (girl->has_trait("Fleet of Foot"))	value += 1;
	if (girl->has_trait("Optimist"))			value += 1;
	if (girl->has_trait("Pessimist"))		value -= 1;
	if (girl->has_trait("Manly"))			value += 1;
	if (girl->has_trait("Maschoist"))		value += 2;
	if (girl->has_trait("Construct"))		value /= 10;	// `J` constructs take 10% damage
	//  Can't heal when damage is dealed and always at least 1 damage inflicted
	if (value >= 0) value = -1;

	girl->upd_stat(STAT_HEALTH, value, false);

	return girl->m_Stats[STAT_HEALTH];
}

// ----- Update

void cGirls::SetEnjoyment(sGirl* girl, int whatSheEnjoys, int amount)										// `J` added
{
	girl->m_Enjoyment[whatSheEnjoys] = amount;
	if (girl->m_Enjoyment[whatSheEnjoys] > 100) 		girl->m_Enjoyment[whatSheEnjoys] = 100;
	else if (girl->m_Enjoyment[whatSheEnjoys] < -100) 	girl->m_Enjoyment[whatSheEnjoys] = -100;
}
void cGirls::SetEnjoymentTR(sGirl* girl, int whatSheEnjoys, int amount)									// `J` added for traits
{
	girl->m_EnjoymentTR[whatSheEnjoys] = amount;
	if (girl->m_EnjoymentTR[whatSheEnjoys] > 100) 			girl->m_EnjoymentTR[whatSheEnjoys] = 100;
	else if (girl->m_EnjoymentTR[whatSheEnjoys] < -100) 	girl->m_EnjoymentTR[whatSheEnjoys] = -100;
}

void cGirls::UpdateEnjoymentTR(sGirl* girl, int whatSheEnjoys, int amount)
{
	girl->m_EnjoymentTR[whatSheEnjoys] += amount;
}
void cGirls::UpdateEnjoymentMod(sGirl* girl, int whatSheEnjoys, int amount)
{
	girl->m_EnjoymentMods[whatSheEnjoys] += amount;
}

// Normalise to zero by 30%
void cGirls::updateTempEnjoyment(sGirl* girl)
{
	// Sanity check. Abort on dead girl
	if (girl->is_dead()) return;

	for (u_int i = 0; i < NUM_ACTIONTYPES; i++)
	{
		if (girl->m_EnjoymentTemps[i] != 0)
		{											// normalize towards 0 by 30% each week
			int newEnjoy = (int)(float(girl->m_EnjoymentTemps[i]) * 0.7);
			if (newEnjoy != girl->m_EnjoymentTemps[i])	girl->m_EnjoymentTemps[i] = newEnjoy;
			else
			{										// if 30% did nothing, go with 1 instead
				/* */if (girl->m_EnjoymentTemps[i] > 0)	girl->m_EnjoymentTemps[i]--;
				else if (girl->m_EnjoymentTemps[i] < 0)	girl->m_EnjoymentTemps[i]++;
			}
		}
	}
}

void cGirls::SetTraining(sGirl* girl, int whatSheTrains, int amount)										// `CRAZY` added
{
	girl->m_Training[whatSheTrains] = amount;
	if (girl->m_Training[whatSheTrains] > 100) 		girl->m_Training[whatSheTrains] = 100;
	else if (girl->m_Training[whatSheTrains] < 0) 	girl->m_Training[whatSheTrains] = 0;
}
void cGirls::SetTrainingTR(sGirl* girl, int whatSheTrains, int amount)									// `CRAZY` added for traits
{
	girl->m_TrainingTR[whatSheTrains] = amount;
	if (girl->m_TrainingTR[whatSheTrains] > 100) 			girl->m_TrainingTR[whatSheTrains] = 100;
	else if (girl->m_TrainingTR[whatSheTrains] < 0) 	girl->m_TrainingTR[whatSheTrains] = 0;
}

void cGirls::UpdateTrainingTR(sGirl* girl, int whatSheTrains, int amount)
{
	girl->m_TrainingTR[whatSheTrains] += amount;
}
void cGirls::UpdateTrainingMod(sGirl* girl, int whatSheTrains, int amount)
{
	girl->m_TrainingMods[whatSheTrains] += amount;
}

// Normalise to zero by 30%
void cGirls::updateTempTraining(sGirl* girl)
{
	// Sanity check. Abort on dead girl
	if (girl->is_dead()) return;

	for (u_int i = 0; i < NUM_TRAININGTYPES; i++)
	{
		if (girl->m_TrainingTemps[i] != 0)
		{											// normalize towards 0 by 30% each week
			int newEnjoy = (int)(float(girl->m_TrainingTemps[i]) * 0.7);
			if (newEnjoy != girl->m_TrainingTemps[i])	girl->m_TrainingTemps[i] = newEnjoy;
			else
			{										// if 30% did nothing, go with 1 instead
				/* */if (girl->m_TrainingTemps[i] > 0)	girl->m_TrainingTemps[i]--;
				else if (girl->m_TrainingTemps[i] < 0)	girl->m_TrainingTemps[i]++;
			}
		}
	}
}



// Increment birthday counter and update Girl's age if needed
void cGirls::updateGirlAge(sGirl* girl, bool inc_inService)
{
	// Sanity check. Abort on dead girl
	if (girl->is_dead()) return;
	if (inc_inService)
	{
		girl->m_WeeksPast++;
		girl->m_BDay++;
	}
	if (girl->m_BDay >= 52)					// Today is girl's birthday
	{
		girl->m_BDay = 0;
		girl->age(1);
		if (girl->age() > 20 && girl->has_trait("Lolita")) girl->remove_trait("Lolita");
		if (girl->age() >= 50)
		{
			girl->beauty(-(g_Dice % 3 + 1));
		}
	}
}

// Update health and other things for STDs
void cGirls::updateSTD(sGirl* girl)
{
	// Sanity check. Abort on dead girl
	if (girl->is_dead()) return;

	bool matron = girl_has_matron(girl, SHIFT_DAY);

	int Dhea = 0, Dhap = 0, Dtir = 0, Dint = 0, Dcha = 0;
	if (girl->has_trait("AIDS"))
	{
		string cureitem = "AIDS Cure";
		int brothelhasscure = g_Brothels.HasItem(cureitem);
		if (brothelhasscure != -1 && matron)
		{
			stringstream cure;
			cure << girl->m_Realname << " was given an " << cureitem << " from the brothel's stock to cure the disease.";
			g_Brothels.AutomaticFoodItemUse(girl, brothelhasscure, cure.str());
		}
		else
		{
			Dhea += g_Dice % 15 + 5;
			Dhap += g_Dice % 5 + 5;
			Dtir += g_Dice % 2 + 1;
		}
	}
	if (girl->has_trait("Herpes"))
	{
		string cureitem = "Herpes Cure";
		int brothelhasscure = g_Brothels.HasItem(cureitem);
		if (brothelhasscure != -1 && matron)
		{
			stringstream cure;
			cure << girl->m_Realname << " was given a " << cureitem << " from the brothel's stock to cure the disease.";
			g_Brothels.AutomaticFoodItemUse(girl, brothelhasscure, cure.str());
		}
		else
		{
			Dhea += max(0, g_Dice % 4 - 2);
			Dhap += max(1, g_Dice % 5 - 2);
			Dcha += max(0, g_Dice % 4 - 1);
		}
	}
	if (girl->has_trait("Chlamydia"))
	{
		string cureitem = "Chlamydia Cure";
		int brothelhasscure = g_Brothels.HasItem(cureitem);
		if (brothelhasscure != -1 && matron)
		{
			stringstream cure;
			cure << girl->m_Realname << " was given a " << cureitem << " from the brothel's stock to cure the disease.";
			g_Brothels.AutomaticFoodItemUse(girl, brothelhasscure, cure.str());
		}
		else
		{
			Dhea += g_Dice % 3 + 1;
			Dhap += g_Dice % 3 + 1;
			Dtir += g_Dice % 2 + 1;
		}
	}
	if (girl->has_trait("Syphilis"))
	{
		string cureitem = "Syphilis Cure";
		int brothelhasscure = g_Brothels.HasItem(cureitem);
		if (brothelhasscure != -1 && matron)
		{
			stringstream cure;
			cure << girl->m_Realname << " was given a " << cureitem << " from the brothel's stock to cure the disease.";
			g_Brothels.AutomaticFoodItemUse(girl, brothelhasscure, cure.str());
		}
		else
		{
			Dhea += g_Dice % 10 + 5;
			Dhap += g_Dice % 5 + 5;
			Dint += max(0, g_Dice % 4 - 2);
		}
	}

	girl->health(-Dhea);
	girl->happiness(-Dhap);
	girl->tiredness(Dtir);
	girl->charisma(-Dcha);
	girl->intelligence(-Dint);



	if (girl->is_dead())
	{
		string msg = girl->m_Realname + " has died from STDs.";
		girl->m_Events.AddMessage(msg, IMGTYPE_DEATH, EVENT_DANGER);
		g_MessageQue.AddToQue(msg, COLOR_RED);
	}
}

// Stat update code that is to be run every turn
void cGirls::updateGirlTurnStats(sGirl* girl)
{
	if (girl->is_dead()) return;		// Sanity check. Abort on dead girl

	// TIREDNESS Really tired girls get unhappy fast
	int bonus = girl->tiredness() - 90;
	int b = 0;
	if (bonus > 0)							// bonus is 1-10
	{
		girl->obedience(-1);				// Base loss for being tired
		girl->pclove(-1);
		b = bonus / 3 + 1;
		girl->happiness(-b);				// 1-4
		if (girl->health() - bonus < 10)	// Don't kill the girl from tiredness
		{
			b = bonus / 2 + 1;				// halve the damage
			girl->health(-b);				// Girl will hate player more if badly hurt from being tired
			girl->pclove(-1);
			girl->pchate(1);
		}
		else girl->health(-bonus);			// Really tired girls lose more health
	}

	// HEALTH hurt girls get tired fast
	bonus = 40 - girl->health();
	if (bonus > 0)							// bonus is 1-40
	{
		girl->pchate(1);					// Base loss for being hurt
		girl->pclove(-1);
		girl->happiness(-1);

		bonus = bonus / 8 + 1;				// bonus vs health values 1: 33-39, 2: 25-32, 3: 17-24, 4: 09-16 5: 01-08
		girl->tiredness(bonus);

		/* `J` She gets more injured for being injured???  commenting out
		bonus = bonus / 2 + 1;				// bonus vs health values 1: 33-39, 2: 17-32, 3: 01-16
		if (girl->health() - bonus < 1)		// Don't kill the girl from low health
		{
			girl->health(1);				// Girl will hate player more for very low health
			girl->pclove(-1);
			girl->pchate(1);
		}
		else
		{
			girl->health(-bonus);
		}
		*/

		/*		These messages duplicate warning messages in the matron code
		*
		*		msg = "DANGER " + girlName + " health is low!";
		*		girl->m_Events.AddMessage(msg, IMGTYPE_PROFILE, EVENT_DANGER);
		*/
	}

	// LOVE love is updated only if happiness is >= 100 or < 50
	if (girl->happiness() >= 100)
	{
		girl->pclove(2);					// Happy girls love player more
		girl->obedience(g_Dice%2);			// `J` added
	}
	if (!girl->is_slave() && girl->happiness() < 50)
	{
		girl->pclove(-2);					// Unhappy FREE girls love player less
		girl->obedience(-(g_Dice % 2));		// `J` added
	}
}

// ----- Stream operators

/*
* While I'm here, let's defined the stream operator for the girls,
* random and otherwise.
*
* This looks big and complex, but it's just printing out the random
* girl data from the load. It's so I can say cout << *rgirl << endl;
* and get a something sensible printed
*/

ostream& operator<<(ostream &os, sRandomGirl &g)
{
	os << g.m_Name << endl;
	os << g.m_Desc << endl;
	os << ("Human? ") << (g.m_Human ? "Yes" : "No") << endl;
	os << ("Catacomb Dweller? ") << (g.m_Catacomb ? "Yes" : "No") << endl;
	os << ("Arena Girl? ") << (g.m_Arena ? "Yes" : "No") << endl;
	os << ("Your Daughter? ") << (g.m_YourDaughter ? "Yes": "No") << endl;
	os << ("Is Daughter? ") << (g.m_IsDaughter ? "Yes" : "No") << endl;
	os << ("Money: Min = ") << g.m_MinMoney << ". Max = " << g.m_MaxMoney << endl;
	/*
	*	loop through stats
	*	setw sets a field width for the next operation,
	*	left forces left alignment. Makes the columns line up.
	*/
	for (unsigned int i = 0; i < sGirl::max_stats; i++)
	{
		os << setw(14) << left << sGirl::stat_names[i] << ": Min = " << (g.m_MinStats[i]) << endl;
		os << setw(14) << "" << ": Max = " << (g.m_MaxStats[i]) << endl;
	}
	/*
	*	loop through skills
	*/
	for (unsigned int i = 0; i < sGirl::max_skills; i++)
	{
		os << setw(14) << left << sGirl::skill_names[i] << ": Min = " << int(g.m_MinSkills[i]) << endl;
		os << setw(14) << "" << ": Max = " << int(g.m_MaxSkills[i]) << endl;
	}
	/*
	*	loop through traits
	*/
	for (int i = 0; i < g.m_NumTraits; i++)
	{
		string name = g.m_Traits[i]->name();
		int percent = int(g.m_TraitChance[i]);
		os << "Trait: " << setw(14) << left << name << ": " << percent << "%" << endl;
	}
	/*
	*	important to return the stream, so the next
	*	thing in the << chain has something on which to operate
	*/
	return os;
}

// * another stream operator, this time for sGirl

ostream& operator<<(ostream& os, sGirl &g)
{
	os << g.m_Realname << endl;
	os << g.m_Desc << endl;
	os << endl;

	for (int i = 0; i < NUM_STATS; i++)
	{
		os.width(20);
		os.flags(ios::left);
		os << g.stat_names[i] << "\t: " << int(g.m_Stats[i]) << endl;
	}
	os << endl;

	for (u_int i = 0; i < NUM_SKILLS; i++)
	{
		os.width(20);
		os.flags(ios::left);
		os << g.skill_names[i] << "\t: " << int(g.m_Skills[i]) << endl;
	}
	os << endl;

	os << endl;
	return os;
}

// ----- Pregnancy, insemination & children

// if contraception is TRUE, then she can't get pregnant which makes sense
static bool has_contraception(sGirl *girl)
{
	// `J` rearranged to speed up checks
	if (girl->has_trait("Skeleton") || girl->has_trait("Zombie"))	return true;	// Skeletons and Zombies can't get pregnant
	if (girl->has_trait("Sterile"))		return true;	// If she's Sterile, she can't get pregnant
	if (girl->is_pregnant())			return true;	// If she's pregnant, she shouldn't get pregnant
	if (girl->m_PregCooldown > 0) 		return true;	// If she's in her cooldown period after giving birth
	if (girl->m_DayJob == JOB_INDUNGEON || girl->m_NightJob == JOB_INDUNGEON)	// `J`
	{
		girl->m_InStudio = girl->m_InCentre = girl->m_InClinic = girl->m_InHouse = girl->m_InArena = girl->m_InFarm = false;
		girl->where_is_she = 0;
	}
	return UseAntiPreg(girl);
}

bool sGirl::calc_pregnancy(int chance, cPlayer *player)
{
	return calc_pregnancy(chance, STATUS_PREGNANT_BY_PLAYER, player->m_Stats, player->m_Skills);
}
bool sGirl::calc_pregnancy(int chance, sCustomer *cust)
{
	return calc_pregnancy(chance, STATUS_PREGNANT, cust->m_Stats, cust->m_Skills);
}

sChild::sChild(bool is_players, Gender gender, int MultiBirth)
{
	// set all the basics
	m_Unborn = 1;
	m_Age = 0;
	m_IsPlayers = is_players;
	if (gender == None && MultiBirth != 0)
		m_Sex = (g_Dice.is_girl() ? Girl : Boy);
	else m_Sex = gender;
	m_MultiBirth = 1;
	m_GirlsBorn = (m_Sex == Girl ? 1 : 0);
	m_Next = m_Prev = 0;
	for (u_int i = 0; i < NUM_SKILLS; i++)		// Added m_Skills here to zero out any that are not specified -- PP
		m_Skills[i] = 0;
	for (int i = 0; i < NUM_STATS; i++)			// Added m_Stats here to zero out any that are not specified -- PP
		m_Stats[i] = 0;
	m_Stats[STAT_HEALTH] = 100;
	m_Stats[STAT_HAPPINESS] = 100;

	if (MultiBirth == 0) return;	// 0 means we are creating a new child in order to load one so we can skip the rest

	// so now the first baby is ready, check if there are more
	m_MultiBirth = MultiBirth;

	int trycount = 1;
	double multichance = cfg.pregnancy.multi_birth_chance();
	while (g_Dice.percent(multichance) && m_MultiBirth < 5)
	{
		m_MultiBirth++;
		if (g_Dice.is_girl()) m_GirlsBorn++;	// check if the new one is a girl
		trycount++;
		multichance /= trycount;
	}
}

void cChildList::add_child(sChild * child)
{
	m_NumChildren++;
	if (m_LastChild)
	{
		m_LastChild->m_Next = child;
		child->m_Prev = m_LastChild;
		m_LastChild = child;
	}
	else
	{
		m_LastChild = child;
		m_FirstChild = child;
	}
}

sChild * cChildList::remove_child(sChild * child, sGirl * girl)
{
	m_NumChildren--;
	sChild* temp = child->m_Next;
	if (child->m_Next)	child->m_Next->m_Prev = child->m_Prev;
	if (child->m_Prev)	child->m_Prev->m_Next = child->m_Next;
	if (child == girl->m_Children.m_FirstChild)	girl->m_Children.m_FirstChild = child->m_Next;
	if (child == girl->m_Children.m_LastChild)	girl->m_Children.m_LastChild = child->m_Prev;
	child->m_Next = 0;
	delete child;
	return temp;
}

int sGirl::preg_chance(int base_pc, bool good, double factor)
{
	g_LogFile.ss() << "sGirl::preg_chance: " << "base %chance = " << base_pc << ", " << "good flag = " << good << ", " << "factor = " << factor;
	g_LogFile.ssend();
	double chance = base_pc;
	// factor is used to pass situational modifiers. For instance BDSM has a 25% reduction in chance
	chance *= factor;
	// if the sex was good, the chance is modded, again from the config file
	if (good) chance *= cfg.pregnancy.good_sex_factor();
	return int(chance);
}

bool sGirl::calc_pregnancy(cPlayer *player, bool good, double factor)
{
	double chance = preg_chance(cfg.pregnancy.player_chance(), good, factor);
	//	now do the calculation
	return calc_pregnancy(int(chance), STATUS_PREGNANT_BY_PLAYER, player->m_Stats, player->m_Skills);
}
bool sGirl::calc_group_pregnancy(cPlayer *player, bool good, double factor)
{
	double chance = preg_chance(cfg.pregnancy.player_chance(), good, factor);
	// player has 25% chance to be father (4 men in the group)
	int father = STATUS_PREGNANT;
	if (g_Dice.percent(25)) father = STATUS_PREGNANT_BY_PLAYER;
	// now do the calculation
	return calc_pregnancy(int(chance), father, player->m_Stats, player->m_Skills);
}
bool sGirl::calc_group_pregnancy(const sCustomer& cust, bool good, double factor)
{
	double chance = preg_chance(cfg.pregnancy.customer_chance(), good, factor);
	chance += cust.m_Amount;
	// now do the calculation
	return calc_pregnancy(int(chance), STATUS_PREGNANT, cust.m_Stats, cust.m_Skills);
}
bool sGirl::calc_pregnancy(const sCustomer& cust, bool good, double factor)
{
	double chance = preg_chance(cfg.pregnancy.customer_chance(), good, factor);
	return calc_pregnancy(int(chance), STATUS_PREGNANT, cust.m_Stats, cust.m_Skills);
}
bool sGirl::calc_insemination(const sCustomer& cust, bool good, double factor)
{
	double chance = preg_chance(cfg.pregnancy.monster_chance(), good, factor);
	return calc_pregnancy(int(chance), STATUS_INSEMINATED, cust.m_Stats, cust.m_Skills);
}
bool sGirl::calc_insemination(cPlayer *player, bool good, double factor)
{
	double chance = preg_chance(cfg.pregnancy.monster_chance(), good, factor);
	return calc_pregnancy(int(chance), STATUS_INSEMINATED, player->m_Stats, player->m_Skills);
}


bool sGirl::carrying_monster()			{ return(m_States & (1 << STATUS_INSEMINATED)) != 0; }
bool sGirl::carrying_human()			{ return carrying_players_child() || carrying_customer_child(); }
bool sGirl::carrying_players_child()	{ return(m_States & (1 << STATUS_PREGNANT_BY_PLAYER)) != 0; }
bool sGirl::carrying_customer_child()	{ return(m_States & (1 << STATUS_PREGNANT)) != 0; }
bool sGirl::is_pregnant()				{ return(m_States & (1 << STATUS_PREGNANT) || m_States & (1 << STATUS_PREGNANT_BY_PLAYER) || m_States & (1 << STATUS_INSEMINATED)); }
bool sGirl::is_mother()					{ return(m_States&(1 << STATUS_HAS_DAUGHTER) || m_States&(1 << STATUS_HAS_SON)); }
bool sGirl::is_poisoned()				{ return(m_States&(1 << STATUS_POISONED) || m_States&(1 << STATUS_BADLY_POISONED)); }
bool sGirl::has_weapon()		// `J` added .06.02.22
{
	if (this->m_NumInventory <= 0) return false;	// she has no items
	for (int i = 0; i < MAXNUM_GIRL_INVENTORY; i++)	// check all her items
	{
		if (this->m_Inventory[i] != 0 && this->m_EquipedItems[i] == 1 &&
			(this->m_Inventory[i]->m_Type == INVWEAPON || this->m_Inventory[i]->m_Type == INVSMWEAPON))
			return true;							// and return true if she has a weapon equiped
	}
	return false;
}
void sGirl::clear_dating()				{ m_States &= ~(1 << STATUS_DATING_PERV); m_States &= ~(1 << STATUS_DATING_MEAN); m_States &= ~(1 << STATUS_DATING_NICE); }
void sGirl::clear_pregnancy()
{
	m_States &= ~(1 << STATUS_PREGNANT);
	m_States &= ~(1 << STATUS_PREGNANT_BY_PLAYER);
	m_States &= ~(1 << STATUS_INSEMINATED);
	m_WeeksPreg = 0;
	sChild* leftover = (this)->m_Children.m_FirstChild;
	while (leftover)
	{
		leftover = (this)->next_child(leftover, (leftover->m_Unborn > 0));
	}

}


void cGirls::CreatePregnancy(sGirl* girl, int numchildren, int type, const int stats[NUM_STATS], const int skills[NUM_SKILLS])
{
	girl->m_States |= (1 << type);	// set the pregnant status

	if (girl->has_trait( "Broodmother"))
	{
		if (g_Dice.percent(cfg.pregnancy.multi_birth_chance())) numchildren++;
		if (g_Dice.percent(cfg.pregnancy.multi_birth_chance())) numchildren++;
	}
	sChild* child = new sChild(unsigned(type) == STATUS_PREGNANT_BY_PLAYER, sChild::None, numchildren);

	// `J` average the mother's and father's stats and skills
	for (int i = 0; i < NUM_STATS; i++)		child->m_Stats[i] = (stats[i] + girl->m_Stats[i]) / 2;
	for (u_int i = 0; i < NUM_SKILLS; i++)	child->m_Skills[i] = (skills[i] + girl->m_Skills[i]) / 2;
	child->m_Stats[STAT_HEALTH] = 100;
	child->m_Stats[STAT_HAPPINESS] = 100;

	// if there is somehow leftover pregnancy data, clear it
	girl->m_WeeksPreg = 0;
	sChild* leftover = girl->m_Children.m_FirstChild;
	while (leftover)
	{
		leftover = girl->next_child(leftover, (leftover->m_Unborn > 0));
	}

	girl->m_Children.add_child(child);
}

int cGirls::calc_abnormal_pc(sGirl *mom, sGirl *sprog, bool is_players)
{
	if (is_players == false) 	// the non-pc-daughter case is simpler
	{
		if (mom->has_trait("Your Daughter")) return 0;		// if the mom is your daughter then any customer is a safe dad - genetically speaking, anyway
		if (g_Dice.percent(98)) return 0;					// so what are the odds that this customer fathered both mom and sprog. Let's say 2%
		sprog->add_trait("Incest", false);					// that's enough to give the sprog the incest trait
		if (mom->has_trait("Incest") == false) return 0;	// but there's only a risk of abnormality if mom is herself incestuous
		return 5;											// If we get past all that lot, there's a 5% chance of abnormality
	}
	sprog->add_trait("Your Daughter", false);				// OK. The sprog is the player's get
	if (mom->has_trait("Your Daughter") == false) return 0;	// if mom isn't the player's then there is no problem
	sprog->add_trait("Incest", false);						// she IS, so we add the incest trait
	if (mom->has_trait("Incest")) return 10;				// if mom is also incestuous, that adds 5% to the odds
	return 5;
}

bool sGirl::add_trait(string name, int temptime, bool removeitem, bool remember)
{
	/*
*	WD: Added logic for remembered trait
*
*		removeitem = true Will add to Remember
*		trait list if the trait is already active
*		Used with items / efects may be removed
*		later eg items - rings
*
*		inrememberlist = true only add trait if
*		exists in the remember list and remove
*		from the list. Use mainly with unequiping
*		items and Trait overiding in ApplyTraits()
*
*		Returns true if trait made active or added
*		trait remember list.
*
*		This should fix items with duplicate
*		traits and loss of original trait if
*		overwritten by a trait from an item
*		that is later removed
*
*/

	if (has_trait(name))
	{
		if (removeitem)								//	WD: Overwriting existing trait with removable item / effect
			g_Girls.AddRememberedTrait(this, name);			//	WD:	Save trait for when item is removed
		return true;
	}

	if (remember)								// WD: Add trait only if it is in the Remember List
	{
		if (g_Girls.HasRememberedTrait(this, name)) g_Girls.RemoveRememberedTrait(this, name);
		else return false;							//	WD:	No trait to add
	}

	for (int i = 0; i < MAXNUM_TRAITS; i++)				// add the trait
	{
		if (m_Traits[i] == 0)
		{
			if (temptime>0) m_TempTrait[i] = temptime;
			m_NumTraits++;
			TraitSpec *addthistrait = g_Traits.GetTrait(name);
			m_Traits[i] = addthistrait;

			g_Girls.MutuallyExclusiveTrait(this, 1, m_Traits[i], removeitem);
			g_Girls.ApplyTraits(this, addthistrait);

			return true;
		}
	}
	return false;
}
bool sGirl::remove_trait(string name,  bool addrememberlist, bool force, bool keepinrememberlist)
{
	/*
*	WD: Added logic for remembered trait
*
*		addrememberlist = true = will move the trait from active to the remember list
*		addrememberlist = false = will remove the trait from active but not add it to the remember list
*
*		force = false = will ignore this check
*		force = true = will remove the trait entirely
*
*		keepinrememberlist = false = will ignore this check
*		keepinrememberlist = true will add it to the remember list even if it is completely removed
*		`J` added - this is for when an item tries to add a trait that the girl can not possible get because she has a trait that precludes it
*			ie. adding "Perky Nipples" to a girl who has "No Nipples", if "No Nipples" gets removed the "Perky Nipples" will show themselves
*
*		Returns true if there was a active trait or remember list entry removed
*
*		This should fix items with duplicate traits and loss of original trait if
*		overwritten by a trait from an item that is later removed
*/

	bool hasRemTrait = g_Girls.HasRememberedTrait(this, name);

	if (!has_trait(name))							// WD:	no active trait to remove
	{
		if (hasRemTrait && !keepinrememberlist)
		{	// WD:	try remembered trait // `J` only if we want to remove it
			g_Girls.RemoveRememberedTrait(this, name);
			return true;
			// `J` explain - she had the trait removed before and it is getting removed again so remove it for good
		}
		if (!hasRemTrait && keepinrememberlist)
		{	// `J` if she does not have it at all but we want her to remember trying to get it
			g_Girls.AddRememberedTrait(this, name);
		}
		return false;	// otherwise just return false
	}
	// `J` - so she has the trait active at this point...

	if (!force && hasRemTrait)	//	WD:	has remembered trait so don't touch active trait unless we are forcing removal of active trait
	{
		g_Girls.RemoveRememberedTrait(this, name);
		return true;
		// `J` explain - she had the trait in both active and remembered so instead of removing active and replacing with remembered, just remove remembered
	}

	//	WD:	save trait to remember list before removing
	if (addrememberlist || keepinrememberlist) g_Girls.AddRememberedTrait(this, name);

	//	WD: Remove trait
	TraitSpec* trait = g_Traits.GetTrait(name);
	for (int i = 0; i < MAXNUM_TRAITS; i++)			// remove the traits
	{
		if (m_Traits[i] && m_Traits[i] == trait)
		{
			m_NumTraits--;

			g_Girls.MutuallyExclusiveTrait(this, 0, m_Traits[i]);
			g_Girls.ApplyTraits(this);

			if (m_TempTrait[i] > 0) m_TempTrait[i] = 0;
			m_Traits[i] = 0;
			return true;
		}
	}
	return false;
}

bool sGirl::has_trait(string trait)
{
	for (int i = 0; i < MAXNUM_TRAITS; i++)
	{
		if (m_Traits[i])
		{
			if (trait.compare(m_Traits[i]->name()) == 0) return true;
		}
	}
	return false;
}

int sGirl::breast_size()	// `J` Breast size number, normal is 4, 1 is flat, max is 10
{
	/* */if (this->has_trait("Flat Chest"))				return 1;	// No bra needed
	else if (this->has_trait("Petite Breasts"))			return 2;	// A cup
	else if (this->has_trait("Small Boobs"))			return 3;	// B cup
	else if (this->has_trait("Busty Boobs"))			return 5;	// D cup
	else if (this->has_trait("Big Boobs"))				return 6;	// E-F cup
	else if (this->has_trait("Giant Juggs"))			return 7;	// G-H cup
	else if (this->has_trait("Massive Melons"))			return 8;	// I-J cup
	else if (this->has_trait("Abnormally Large Boobs"))	return 9;	// K-M cup
	else if (this->has_trait("Titanic Tits"))			return 10;	// N+ cup
	return 4;														// C cup
}
bool sGirl::is_dead(bool sendmessage)
{
	if (this->health() <= 0)
	{
		if (sendmessage)
		{
			stringstream ss; ss << this->m_Realname << " is dead. She isn't going to work anymore and her body will be removed by the end of the week.";
			g_MessageQue.AddToQue(ss.str(), 1);
		}
		return true;
	}
	return false;

}
bool sGirl::is_addict(bool onlyhard)	// `J` added bool onlyhard to allow only hard drugs to be checked for
{
	if (onlyhard)
	{
		return	has_trait("Shroud Addict") ||
			has_trait("Fairy Dust Addict") ||
			has_trait("Viras Blood Addict");
	}
	return	has_trait("Shroud Addict") ||
		has_trait("Fairy Dust Addict") ||
		has_trait("Smoker") ||
		has_trait("Alcoholic") ||
		has_trait("Cum Addict") ||
		has_trait("Viras Blood Addict");
}
bool sGirl::has_disease()
{
	return	has_trait("AIDS") ||
		has_trait("Herpes") ||
		has_trait("Chlamydia") ||
		has_trait("Syphilis");
}
bool sGirl::is_fighter(bool canbehelped)
{
	if (canbehelped)
	{
		return	has_trait("Aggressive") ||
			has_trait("Yandere") ||
			has_trait("Tsundere");
	}
	return	has_trait("Aggressive") ||
		has_trait("Assassin") ||
		has_trait("Yandere") ||
		has_trait("Brawler") ||
		has_trait("Tsundere");
}

sChild* sGirl::next_child(sChild* child, bool remove)
{
	if (!remove)
	{
		return child->m_Next;
	}
	return m_Children.remove_child(child, this);
}

int sGirl::preg_type(int image_type)
{
	int new_type = image_type + PREG_OFFSET;
	/*
	*		if the new image type is >=  NUM_IMGTYPES
	*		then it was one of the types that doesn't have
	*		an equivalent pregnant form
	*/
	if (new_type >= NUM_IMGTYPES)
	{
		return image_type;
	}
	return new_type;
}

bool sGirl::is_resting()
{
	return ((m_DayJob == JOB_FILMFREETIME	&& m_NightJob == JOB_FILMFREETIME) ||
		(m_DayJob == JOB_ARENAREST		&& m_NightJob == JOB_ARENAREST) ||
		(m_DayJob == JOB_CENTREREST		&& m_NightJob == JOB_CENTREREST) ||
		(m_DayJob == JOB_CLINICREST		&& m_NightJob == JOB_CLINICREST) ||
		(m_DayJob == JOB_HOUSEREST		&& m_NightJob == JOB_HOUSEREST) ||
		(m_DayJob == JOB_FARMREST		&& m_NightJob == JOB_FARMREST) ||
		(m_DayJob == JOB_RESTING		&& m_NightJob == JOB_RESTING));
}
bool sGirl::is_havingsex()
{
	return (
		m_DayJob == JOB_TRAINING || m_NightJob == JOB_TRAINING ||
		m_DayJob == JOB_ESCORT || m_NightJob == JOB_ESCORT ||
		m_DayJob == JOB_WHOREGAMBHALL || m_NightJob == JOB_WHOREGAMBHALL ||
		m_DayJob == JOB_BARWHORE || m_NightJob == JOB_BARWHORE ||
		m_DayJob == JOB_WHOREBROTHEL || m_NightJob == JOB_WHOREBROTHEL ||
		m_DayJob == JOB_WHORESTREETS || m_NightJob == JOB_WHORESTREETS ||
		m_DayJob == JOB_FILMANAL || m_NightJob == JOB_FILMANAL ||
		m_DayJob == JOB_FILMFOOTJOB || m_NightJob == JOB_FILMFOOTJOB ||
		m_DayJob == JOB_FILMHANDJOB || m_NightJob == JOB_FILMHANDJOB ||
		m_DayJob == JOB_FILMLESBIAN || m_NightJob == JOB_FILMLESBIAN ||
		m_DayJob == JOB_FILMORAL || m_NightJob == JOB_FILMORAL ||
		m_DayJob == JOB_FILMSEX || m_NightJob == JOB_FILMSEX ||
		m_DayJob == JOB_FILMTITTY || m_NightJob == JOB_FILMTITTY ||
		m_DayJob == JOB_FILMBEAST || m_NightJob == JOB_FILMBEAST ||
		m_DayJob == JOB_FILMBONDAGE || m_NightJob == JOB_FILMBONDAGE ||
		m_DayJob == JOB_FILMBUKKAKE || m_NightJob == JOB_FILMBUKKAKE ||
		m_DayJob == JOB_FILMFACEFUCK || m_NightJob == JOB_FILMFACEFUCK ||
		m_DayJob == JOB_FILMGROUP || m_NightJob == JOB_FILMGROUP ||
		m_DayJob == JOB_FILMPUBLICBDSM || m_NightJob == JOB_FILMPUBLICBDSM ||
		m_DayJob == JOB_FILMRANDOM || m_NightJob == JOB_FILMRANDOM ||
		m_DayJob == JOB_PERSONALBEDWARMER || m_NightJob == JOB_PERSONALBEDWARMER ||
		m_DayJob == JOB_PERSONALTRAINING || m_NightJob == JOB_PERSONALTRAINING ||
		m_DayJob == JOB_FAKEORGASM || m_NightJob == JOB_FAKEORGASM ||
		m_DayJob == JOB_SO_STRAIGHT || m_NightJob == JOB_SO_STRAIGHT ||
		m_DayJob == JOB_SO_BISEXUAL || m_NightJob == JOB_SO_BISEXUAL ||
		m_DayJob == JOB_SO_LESBIAN || m_NightJob == JOB_SO_LESBIAN ||
		m_DayJob == JOB_HOUSEPET || m_NightJob == JOB_HOUSEPET
		);
}
bool sGirl::was_resting()
{
	return ((m_PrevDayJob == JOB_FILMFREETIME	&& m_PrevNightJob == JOB_FILMFREETIME) ||
		(m_PrevDayJob == JOB_ARENAREST		&& m_PrevNightJob == JOB_ARENAREST) ||
		(m_PrevDayJob == JOB_CENTREREST		&& m_PrevNightJob == JOB_CENTREREST) ||
		(m_PrevDayJob == JOB_CLINICREST		&& m_PrevNightJob == JOB_CLINICREST) ||
		(m_PrevDayJob == JOB_HOUSEREST		&& m_PrevNightJob == JOB_HOUSEREST) ||
		(m_PrevDayJob == JOB_FARMREST		&& m_PrevNightJob == JOB_FARMREST) ||
		(m_PrevDayJob == JOB_RESTING		&& m_PrevNightJob == JOB_RESTING));
}


// Crazy found some old code to allow Canonical_Daughters
sGirl *cGirls::find_girl_by_name(string name, int *index_pt)
{
	int count = 0;
	sGirl* current = m_Parent;

	if (index_pt)
	{
		*index_pt = -1;
	}

	while (current)
	{
		if (current->m_Name == name)
		{
			if (index_pt)
			{
				*index_pt = count;
			}
			return current;
		}
		count++;
		current = current->m_Next;
	}
	return 0;
}

sRandomGirl *cGirls::find_random_girl_by_name(string name, int *index_pt)
{
	int count = 0;
	sRandomGirl* current = m_RandomGirls;
	if (index_pt) *index_pt = -1;

	while (current)
	{
		if (current->m_Name == name)
		{
			if (index_pt) *index_pt = count;
			return current;
		}
		count++;
		current = current->m_Next;
	}
	return 0;
}

// returns 0 if not found, 1 if girl found, 2 if rgirl found.
int cGirls::test_child_name(string name)
{
	sGirl* current = find_girl_by_name(name);
	// did we get a girl?
	if (current)	return 1;		// yes, we did!

	//	OK, we need to search for a random girl
	sRandomGirl* rgirl = find_random_girl_by_name(name);
	if (rgirl)		return 2;
	return 0;
}

sGirl *cGirls::make_girl_child(sGirl* mom, bool playerisdad)
{
    sGirl* sprog = nullptr;
	bool slave = mom->is_slave();
	bool non_human = mom->is_human();

	// if there are no entries in the duaghter_names list we just create a random girl
	int n_names = mom->m_Canonical_Daughters.size();
	if (n_names == 0)
	{
		return sprog = CreateRandomGirl(17, false, slave, false, non_human, false, false, playerisdad, true, "");
	}
	/*
	*	OK, we get to work for our living in which case.
	*
	*	we'll need a retry loop
	*/
	for (;;) {
		/*
		*		get a random index into the daughter list
		*		(some people might prefer them to be born in order
		*		we'll see ... )
		*/
		int index = g_Dice.random(n_names);
		/*
		*		get the name at that index
		*/
		string name = mom->m_Canonical_Daughters[index];
		/*
		*		see if we can find or create a girl based on that name
		*/
		int found = test_child_name(name);

		if (found > 0)		// success! break out of the retry loop
		{
			if (found == 1)			// found a girl
			{
				sprog = find_girl_by_name(name);
			}
			else if (found == 2)	// found a rgirl
			{
				sprog = CreateRandomGirl(17, false, slave, false, non_human, false, false, playerisdad, true, name);
			}
			if (sprog)
			{
				return sprog;
			}
		}
		/*
		*		OK - that name is not usable - remove it
		*/
		mom->m_Canonical_Daughters.erase(mom->m_Canonical_Daughters.begin() + index);
		/*
		*		check the size of the list - if empty, break out of the loop
		*/
		n_names = mom->m_Canonical_Daughters.size();
		if (n_names == 0) {
			break;
		}
	}
	/*
	*	If the above palaver resulted in a sprog, return it
	*	otherwise, back to the random girls
	*/
	return  g_Girls.CreateRandomGirl(17, false, slave, false, non_human, false, false, playerisdad);
}

// returns false if the child is not grown up, returns true when the child grows up
bool cGirls::child_is_grown(sGirl* mom, sChild *child, string& summary, bool PlayerControlled)
{
	if (child->m_MultiBirth < 1) child->m_MultiBirth = 1; // `J` fix old code
	if (child->m_MultiBirth > 5) child->m_MultiBirth = 5; // `J` fix old code

	// bump the age - if it's still not grown, go home
	child->m_Age++;		if (child->m_Age < cfg.pregnancy.weeks_till_grown())	return false;

	//cTariff tariff;
	stringstream ss;

#if 1
	if (child->m_MultiBirth == 1)	// `J` only 1 child so use the old code
	{
		// we need a coming of age ceremony
		if (child->is_boy())
		{
			summary += "A son grew of age. ";
			mom->m_States |= (1 << STATUS_HAS_SON);
			ss << "Her son has grown of age";
			sGang* gang = g_Gangs.GetGangNotFull(1, false);
			if (gang&&gang->m_Num < 15)
			{
				gang->m_Num++;
				ss << " and was sent to join your gang " << gang->m_Name << ".\n";
			}
			else if (PlayerControlled)	// get the going rate for a male slave and sell the poor sod
			{
				int gold = tariff.male_slave_sales();
				g_Gold.slave_sales(gold);
				ss << " and has been sold into slavery.\n";
				ss << "You make " << gold << " gold selling the boy.\n";
			}
			else	// or send him on his way
			{
				int roll = g_Dice % 4;
				ss << " and ";
				if (roll == 0)		ss << "moved away";
				else if (roll == 1)	ss << "joined the army";
				else 				ss << "got his own place in town";
				ss << ".\n";
			}
			mom->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_GOODNEWS);
			return true;
		}

		bool playerfather = child->m_IsPlayers;		// is 1 if father is player
		summary += "A daughter grew of age. ";
		mom->m_States |= (1 << STATUS_HAS_DAUGHTER);
		bool slave = mom->is_slave();
		bool MomIsMonster = mom->is_monster();
		// create a new girl for the barn
		sGirl* sprog = 0;
		if (mom->m_Canonical_Daughters.size() > 0)
		{
			sprog = make_girl_child(mom, playerfather);
		}
		if (!sprog && playerfather && GetNumYourDaughterGirls() > 0)				// this should check all your daughter girls that apply
		{
			sprog = GetUniqueYourDaughterGirl(MomIsMonster);						// first try to get the same human/nonhuman as mother
			if (!sprog && MomIsMonster) sprog = GetUniqueYourDaughterGirl(true);	// next, if mom is nonhuman, try to get a human daughter
		}
		if (!sprog)
		{
			sprog = g_Girls.CreateRandomGirl(17, false, slave, false, MomIsMonster, false, false, playerfather);
		}
		// check for incest, get the odds on abnormality
		int abnormal_pc = calc_abnormal_pc(mom, sprog, child->m_IsPlayers);
		if (g_Dice.percent(abnormal_pc))
		{
			if (g_Dice.percent(50)) sprog->add_trait("Malformed");
			else 					sprog->add_trait("Retarded");
		}
		// loop throught the mom's traits, inheriting where appropriate
		for (int i = 0; i < mom->m_NumTraits && sprog->m_NumTraits < 30; i++)
		{
			if (mom->m_Traits[i])
			{
				if (mom->m_Traits[i]->name() == "Queen")
				{
					/* */if (g_Dice.percent(60))	sprog->add_trait("Princess");
					else if (g_Dice.percent(60))	sprog->add_trait("Noble");
				}
				else if (mom->m_Traits[i]->name() == "Princess" || mom->m_Traits[i]->name() == "Noble")
				{
					if (g_Dice.percent(40))			sprog->add_trait("Noble");
				}
				else if (mom->m_Traits[i]->inherit_chance() != -1)	// `J` new method for xml traits
				{
					if (g_Dice.percent(mom->m_Traits[i]->inherit_chance()))
					{
						sprog->add_trait(mom->m_Traits[i]->name());
					}
				}
				else	// old method
				{
					string tname = mom->m_Traits[i]->name();
					if (g_Girls.InheritTrait(mom->m_Traits[i]) && tname != "")
						sprog->add_trait(mom->m_Traits[i]->name());
				}
			}
		}
		if (playerfather)
		{
			sprog->add_trait("Your Daughter");
		}

		g_Girls.MutuallyExclusiveTraits(sprog, 1);	// make sure all the trait effects are applied
		g_Girls.ApplyTraits(sprog);
		RemoveAllRememberedTraits(sprog);	// WD: remove any rembered traits created from trait incompatibilities

		// inherit stats
		for (int i = 0; i < NUM_STATS; i++)
		{
			int min = 0, max = 100;
			if (mom->m_Stats[i] < child->m_Stats[i]) { min = mom->m_Stats[i]; max = child->m_Stats[i]; }
			else 									 { max = mom->m_Stats[i]; min = child->m_Stats[i]; }
			sprog->m_Stats[i] = (g_Dice % (max - min)) + min;
		}

		// set age to 18, fix health
		sprog->m_Stats[STAT_AGE] = 18;	// `J` Legal Note: 18 is the Legal Age of Majority for the USA where I live
		sprog->m_Stats[STAT_HEALTH] = 100;
		sprog->m_Stats[STAT_HAPPINESS] = 100;
		sprog->m_Stats[STAT_TIREDNESS] = 0;
		sprog->m_Stats[STAT_LEVEL] = 0;
		sprog->m_Stats[STAT_EXP] = 0;


		// `J` set her birthday
		int m0 = g_Month;
		int d0 = g_Day;
		d0 -= g_Dice % 7;
		if (d0 < 1) { d0 += 30; m0--; }
		if (m0 < 1) { m0 = 12; }
		sprog->BirthMonth = m0;
		sprog->BirthDay = d0;



		// inherit skills
		for (u_int i = 0; i < NUM_SKILLS; i++)
		{
			int s = 0;
			if (mom->m_Skills[i] < child->m_Skills[i])	s = child->m_Skills[i];
			else										s = mom->m_Skills[i];
			sprog->m_Skills[i] = g_Dice%max(s, 20);
		}


		// new code to add first and last name to girls

		// at this point the sprog should have temporary firstname, surname, and realname
		string prevsurname = sprog->m_Surname;		// save the temporary surname incase it is needed later
		string biography = "";
		if (playerfather)
		{
			if (The_Player->Surname().size() > 0)
			{
				sprog->m_Surname = The_Player->Surname();
			}
			else	// probably shouldn't happen because there is a default surname
			{
				sprog->m_Surname = "Playerchild";	// generic name
			}
			biography = "Daughter of " + mom->m_Realname + " and the Brothel owner, Mr. " + The_Player->RealName();
		}
		else
		{
			if (mom->m_Surname.length() > 0)	// mom has a surname already
			{
				sprog->m_Surname = mom->m_Surname;
			}
			else
			{
				sprog->m_Surname = prevsurname;
			}
			biography = "Daughter of " + mom->m_Realname + " and an anonymous brothel client";
		}
		g_Girls.CreateRealName(sprog);
		sprog->m_Desc = sprog->m_Desc + "\n \n" + biography + ".";

		// make sure slave daughters have house perc. set to 100, otherwise 60
		sprog->m_Stats[STAT_HOUSE] = (slave) ? cfg.initial.slave_house_perc() : cfg.initial.girls_house_perc();

		int sendwhere = -1;
		string sendwherestring = "Dungeon.";

#if 0	// not going to use this until safeguards are in place
		if (playerfather && g_House.GetFreeRooms(0) > 0)	// `J` send your daughters to your house if there is room.
		{
			sendwhere = BUILDING_HOUSE;
			sendwherestring = "House.";
		}
		if (sendwhere < 0 &&
			(PlayerControlled || sprog->is_slave) &&		// `J` if the girl is controlled or a slave and
			g_Brothels.GetNumBrothelsWithVacancies > 0)		// there are any brothels with space, send her to one
		{
			sendwhere = 1;
			sendwherestring = "Brothel";
		}
#endif

		stringstream Sss;
		Sss << sprog->m_Realname << ", the " << biography << ", has grown of age and was sent to your " << sendwherestring;
		sprog->m_Events.AddMessage(Sss.str(), IMGTYPE_PROFILE, EVENT_GOODNEWS);

		ss << "Her daughter " << sprog->m_Realname << " has grown of age and was sent to your " << sendwherestring;
		if (sendwhere == BUILDING_HOUSE)	// house
		{
			g_House.AddGirl(0, sprog);
		}
		else if (sendwhere == 1)	// brothel
		{
			int sendto = 0;
			if (mom->where_is_she > 0 && g_Brothels.GetFreeRooms(mom->where_is_she) > 0)
				sendto = mom->where_is_she;
			g_Brothels.AddGirl(sendto, sprog);
		}
		else	// dungeon
		{
			g_Brothels.GetDungeon()->AddGirl(sprog, DUNGEON_KID);
		}


		mom->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_GOODNEWS);
		return true;
	}
#else
if (0){}
#endif

	else		// `J` new code
	{
		// prepare for the checks
		int numchildren = child->m_MultiBirth;
		int aregirls[5] = { -1, -1, -1, -1, -1 };
		for (int i = 0; i < child->m_MultiBirth; i++)	aregirls[i] = 1;
		for (int i = 0; i < child->m_GirlsBorn; i++)	aregirls[i] = 0;

		for (int i = 0; i < numchildren; i++)
		{
			ss.str("");
			if (aregirls[i] == 1)	// boys first
			{
				summary += "A son has grown of age. ";
				mom->m_States |= (1 << STATUS_HAS_SON);
				if (PlayerControlled)	// get the going rate for a male slave and sell the poor sod
				{
					int gold = tariff.male_slave_sales();
					g_Gold.slave_sales(gold);
					ss << "Her son has grown of age and has been sold into slavery.\n";
					ss << "You make " << gold << " gold selling the boy.\n";
				}
				else	// or send him on his way
				{
					int roll = g_Dice % 4;
					ss << "Her son has grown of age and ";
					if (roll == 0)		ss << "moved away";
					else if (roll == 1)	ss << "joined the army";
					else 				ss << "got his own place in town";
					ss << ".\n";
				}
				mom->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_GOODNEWS);
			}
			else if (aregirls[i] == 0)	// girls next
			{
				bool playerfather = child->m_IsPlayers;		// is 1 if father is player
				summary += "A daughter has grown of age. ";
				mom->m_States |= (1 << STATUS_HAS_DAUGHTER);
				bool slave = mom->is_slave();
				bool MomIsMonster = mom->is_monster();
				// create a new girl for the barn
				sGirl* sprog = 0;
				if (mom->m_Canonical_Daughters.size() > 0)
				{
					sprog = make_girl_child(mom, playerfather);
				}
				if (!sprog && playerfather && GetNumYourDaughterGirls() > 0)				// this should check all your daughter girls that apply
				{
					sprog = GetUniqueYourDaughterGirl(MomIsMonster);						// first try to get the same human/nonhuman as mother
					if (!sprog && MomIsMonster) sprog = GetUniqueYourDaughterGirl(true);	// next, if mom is nonhuman, try to get a human daughter
				}
				if (!sprog)
				{
					sprog = g_Girls.CreateRandomGirl(17, false, slave, false, MomIsMonster, false, false, playerfather);
				}
				// check for incest, get the odds on abnormality
				int abnormal_pc = calc_abnormal_pc(mom, sprog, child->m_IsPlayers);
				if (g_Dice.percent(abnormal_pc))
				{
					if (g_Dice.percent(50)) sprog->add_trait("Malformed");
					else 					sprog->add_trait("Retarded");
				}
				// loop throught the mom's traits, inheriting where appropriate
				for (int i = 0; i < mom->m_NumTraits && sprog->m_NumTraits < 30; i++)
				{
					if (mom->m_Traits[i])
					{
						if (mom->m_Traits[i]->name() == "Queen")
						{
							/* */if (g_Dice.percent(60))	sprog->add_trait("Princess");
							else if (g_Dice.percent(60))	sprog->add_trait("Noble");
						}
						else if (mom->m_Traits[i]->name() == "Princess" || mom->m_Traits[i]->name() == "Noble")
						{
							if (g_Dice.percent(40))			sprog->add_trait("Noble");
						}
						else if (mom->m_Traits[i]->inherit_chance() != -1)	// `J` new method for xml traits
						{
							if (g_Dice.percent(mom->m_Traits[i]->inherit_chance()))
							{
								sprog->add_trait(mom->m_Traits[i]->name());
							}
						}
						else
						{
							string tname = mom->m_Traits[i]->name();
							if (g_Girls.InheritTrait(mom->m_Traits[i]) && tname != "")
								sprog->add_trait(mom->m_Traits[i]->name());
						}
					}
				}
				if (playerfather)
				{
					sprog->add_trait("Your Daughter");
				}

				g_Girls.MutuallyExclusiveTraits(sprog, 1);	// make sure all the trait effects are applied
				g_Girls.ApplyTraits(sprog);
				RemoveAllRememberedTraits(sprog);	// WD: remove any rembered traits created from trait incompatibilities

				// inherit stats
				for (int i = 0; i < NUM_STATS; i++)
				{
					int min = 0, max = 100;
					if (mom->m_Stats[i] < child->m_Stats[i]) { min = mom->m_Stats[i]; max = child->m_Stats[i]; }
					else 									 { max = mom->m_Stats[i]; min = child->m_Stats[i]; }
					sprog->m_Stats[i] = (g_Dice % (max - min)) + min;
				}

				// set age to 18, fix health
				sprog->m_Stats[STAT_AGE] = 18;	// `J` Legal Note: 18 is the Legal Age of Majority for the USA where I live
				sprog->m_Stats[STAT_HEALTH] = 100;
				sprog->m_Stats[STAT_HAPPINESS] = 100;
				sprog->m_Stats[STAT_TIREDNESS] = 0;
				sprog->m_Stats[STAT_LEVEL] = 0;
				sprog->m_Stats[STAT_EXP] = 0;

				// `J` set her birthday
				int m0 = g_Month;
				int d0 = g_Day;
				d0 -= g_Dice % 7;
				if (d0 < 1) { d0 += 30; m0--; }
				if (m0 < 1) { m0 = 12; }
				sprog->BirthMonth = m0;
				sprog->BirthDay = d0;

				// inherit skills
				for (u_int i = 0; i < NUM_SKILLS; i++)
				{
					int s = 0;
					if (mom->m_Skills[i] < child->m_Skills[i])	s = child->m_Skills[i];
					else										s = mom->m_Skills[i];
					sprog->m_Skills[i] = g_Dice%min(s, 20);
				}


				// new code to add first and last name to girls

				// at this point the sprog should have temporary firstname, surname, and realname
				string prevsurname = sprog->m_Surname;		// save the temporary surname incase it is needed later
				string biography = "";
				if (playerfather)
				{
					if (The_Player->Surname().size() > 0)
					{
						sprog->m_Surname = The_Player->Surname();
					}
					else	// probably shouldn't happen because there is a default surname
					{
						sprog->m_Surname = "Playerchild";	// generic name
					}
					biography = "Daughter of " + mom->m_Realname + " and the Brothel owner, Mr. " + The_Player->RealName();
				}
				else
				{
					if (mom->m_Surname.length() > 0)	// mom has a surname already
					{
						sprog->m_Surname = mom->m_Surname;
					}
					else
					{
						sprog->m_Surname = prevsurname;
					}
					biography = "Daughter of " + mom->m_Realname + " and an anonymous brothel client";
				}
				g_Girls.CreateRealName(sprog);
				sprog->m_Desc = sprog->m_Desc + "\n \n" + biography + ".";

				// make sure slave daughters have house perc. set to 100, otherwise 60
				sprog->m_Stats[STAT_HOUSE] = (slave) ? cfg.initial.slave_house_perc() : cfg.initial.girls_house_perc();

				int sendwhere = -1;
				string sendwherestring = "Dungeon.";

#if 0	// not going to use this until safeguards are in place
				if (playerfather && g_House.GetFreeRooms(0) > 0)	// `J` send your daughters to your house if there is room.
				{
					sendwhere = BUILDING_HOUSE;
					sendwherestring = "House.";
				}
				if (sendwhere < 0 &&
					(PlayerControlled || sprog->is_slave) &&		// `J` if the girl is controlled or a slave and
					g_Brothels.GetNumBrothelsWithVacancies > 0)		// there are any brothels with space, send her to one
				{
					sendwhere = 1;
					sendwherestring = "Brothel";
				}
#endif

				stringstream Sss;
				Sss << sprog->m_Realname << ", the " << biography << ", has grown of age and was sent to your " << sendwherestring;
				sprog->m_Events.AddMessage(Sss.str(), IMGTYPE_PROFILE, EVENT_GOODNEWS);

				ss << "Her daughter " << sprog->m_Realname << " has grown of age and was sent to your " << sendwherestring;
				if (sendwhere == BUILDING_HOUSE)	// house
				{
					g_House.AddGirl(0, sprog);
				}
				else if (sendwhere == 1)	// brothel
				{
					int sendto = 0;
					if (mom->where_is_she > 0 && g_Brothels.GetFreeRooms(mom->where_is_she) > 0)
						sendto = mom->where_is_she;
					g_Brothels.AddGirl(sendto, sprog);
				}
				else	// dungeon
				{
					g_Brothels.GetDungeon()->AddGirl(sprog, DUNGEON_KID);
				}
				mom->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_GOODNEWS);
			}
		}
	}
	return true;
}

void cGirls::UncontrolledPregnancies()
{
	sGirl* current = m_Parent;
	string summary;
	while (current)
	{
		HandleChildren(current, summary, false);
		current = current->m_Next;
	}
}

void cGirls::HandleChildren(sGirl* girl, string& summary, bool PlayerControlled)
{
	girl->m_JustGaveBirth = false;
	/*
	*	start by advancing pregnancy cooldown time
	*/
	if (girl->m_PregCooldown > 0) girl->m_PregCooldown--;
	/*
	*	now: if the girl has no children we have nothing to do logically this can precede the cooldown bump
	*	since if she's on cooldown she must have given birth but I guess this way offers better bugproofing
	*/
	if (girl->m_Children.m_FirstChild == 0) return;
	/*
	*	loop through the girl's children, and divide them into those growing up and those still to be born
	*/

	bool remove_flag;
	sChild* child = girl->m_Children.m_FirstChild;
	while (child)
	{
		// if the child is yet unborn see if it is due
		if (child->m_Unborn)
		{
			// some births (monsters) we do not track to adulthood these need removing from the list
			remove_flag = child_is_due(girl, child, summary, PlayerControlled);
		}
		else
		{
			// the child has been born already if it comes of age we remove it from the list
			remove_flag = child_is_grown(girl, child, summary, PlayerControlled);
		}
		child = girl->next_child(child, remove_flag);
	}
}

// Returns false if we do not want to remove the child. Returns true and we get rid of the child.
bool cGirls::child_is_due(sGirl* girl, sChild *child, string& summary, bool PlayerControlled)
{
	if (child->m_MultiBirth < 1) child->m_MultiBirth = 1; // `J` fix old code
	if (child->m_MultiBirth > 5 && girl->carrying_human())
		child->m_MultiBirth = 5; // `J` fix old code - maximum 5 human children - no max for beasts
	/*
	*	clock on the count and see if she's due
	*	if not, return false (meaning "do not remove this child yet)
	*/
	girl->m_WeeksPreg++;
	if (girl->m_WeeksPreg < (girl->m_States&(1 << STATUS_INSEMINATED) ? cfg.pregnancy.weeks_monster_p() : cfg.pregnancy.weeks_pregnant()))
		return false;
	/*
	*	OK, it's time to give birth
	*	start with some basic bookkeeping.
	*/
	//cTariff tariff;
	stringstream ss;

	girl->m_WeeksPreg = 0;
	child->m_Unborn = 0;
	girl->m_PregCooldown = cfg.pregnancy.cool_down();

	//ADB low health is risky for pregnancy!
	//80 health will add 2 to percent chance of sterility and death, 10 health will add 9 percent!
	int healthFactor = (100 - girl->health()) / 10;
	/*
	*	the human-baby case is marginally easier than the
	*	tentacle-beast-monstrosity one, so we do that first
	*/
	if (girl->carrying_human())
	{
		/*
		*		first things first - clear the pregnancy bit
		*		this is a human birth, so add the MILF trait
		*/
		girl->clear_pregnancy();
		girl->m_JustGaveBirth = true;
		girl->add_trait("MILF");

		girl->tiredness(100);
		girl->happiness(10 + g_Dice % 91);
		girl->health(-(child->m_MultiBirth + g_Dice % 10 + healthFactor));

		// `J` If/when the baby gets moved somewhere else in the code, then the maother can die from giving birth
		// For now don't kill her, it causes too many problems with the baby.
		if (girl->health() < 1) SetStat(girl, STAT_HEALTH, 1);

		if (child->m_MultiBirth == 1)	// only 1 baby so use the old code
		{
			if (g_Dice.percent(cfg.pregnancy.miscarriage_chance()))	// the baby dies
			{
				// format a message
				girl->m_ChildrenCount[CHILD08_MISCARRIAGES]++;
				ss << "She has given birth to " << child->boy_girl_str() << " but it did not survive the birth.\n \nYou grant her the week off to grieve.";
				//check for sterility
				if (g_Dice.percent(5 + healthFactor))
				{
					// `J` updated old code to use new traits from new code
					ss << "It was a difficult birth and ";
					if (girl->has_trait( "Broodmother"))
					{
						ss << "her womb has been damaged.\n";
						girl->health(-(1 + g_Dice % 2));
						if (girl->health() < 1) g_Girls.SetStat(girl, STAT_HEALTH, 1);	// don't kill her now, it causes all the babies to go away.
					}
					else if (girl->has_trait( "Fertile"))	// loose fertile
					{
						ss << "her womb has been damaged reducing her fertility.\n";
						AdjustTraitGroupFertility(girl, -1);
					}
					else	// add sterile
					{
						ss << "she has lost the ability to have children.\n";
						AdjustTraitGroupFertility(girl, -1);
					}
				}
				if (PlayerControlled) girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_DANGER);
				return true;
			}
			else	// the baby lives
			{
				girl->m_ChildrenCount[CHILD00_TOTAL_BIRTHS]++;
				if (child->is_girl())
				{
					girl->m_ChildrenCount[CHILD02_ALL_GIRLS]++;
					if (child->m_IsPlayers) girl->m_ChildrenCount[CHILD06_YOUR_GIRLS]++;
					else					girl->m_ChildrenCount[CHILD04_CUSTOMER_GIRLS]++;
				}
				else
				{
					girl->m_ChildrenCount[CHILD03_ALL_BOYS]++;
					if (child->m_IsPlayers) girl->m_ChildrenCount[CHILD07_YOUR_BOYS]++;
					else					girl->m_ChildrenCount[CHILD05_CUSTOMER_BOYS]++;
				}
				// format a message
				ss << "She has given birth to " << child->boy_girl_str() << ".\n \nYou grant her the week off for maternity leave.";
				//check for sterility
				if (g_Dice.percent(healthFactor))
				{
					// `J` updated old code to use new traits from new code
					ss << "It was a difficult birth and ";
					if (girl->has_trait( "Broodmother"))
					{
						ss << "her womb has been damaged.\n";
						girl->health(-(1 + g_Dice % 2));
						if (girl->health() < 1) g_Girls.SetStat(girl, STAT_HEALTH, 1);	// don't kill her now, it causes all the babies to go away.
					}
					else if (girl->has_trait( "Fertile"))	// loose fertile
					{
						ss << "her womb has been damaged reducing her fertility.\n";
						AdjustTraitGroupFertility(girl, -1);
					}
					else	// add sterile
					{
						ss << "she has lost the ability to have children.\n";
						AdjustTraitGroupFertility(girl, -1);
					}
				}
				// queue the message and return false because we need to see this one grow up
				if (PlayerControlled) girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_DANGER);
				return false;
			}
		}
		else	// multiple births
		{
			int unbornbabies = child->m_MultiBirth;
			healthFactor += child->m_MultiBirth;
			int t = 0;	// total
			int m = 0;	// miscarriages
			int g = 0;	// girls born live
			int b = 0;	// boys born live
			int s = 0;	// sterility count
			int c = 0;	// current baby, 0=girl, 1=boy
			while (unbornbabies > 0)
			{
				c = (unbornbabies > child->m_GirlsBorn ? 1 : 0);
				t++;
				if (g_Dice.percent(cfg.pregnancy.miscarriage_chance()			// the baby dies
					+ child->m_MultiBirth))										// more likely for multiple births
				{
					m++; girl->m_ChildrenCount[CHILD08_MISCARRIAGES]++;			// add to miscarriage count
					child->m_MultiBirth--; if (c == 0) child->m_GirlsBorn--;	// and remove the baby from the counts
					if (g_Dice.percent(5 + healthFactor)) s++;					// check for sterility
				}
				else	// the baby lives
				{
					girl->m_ChildrenCount[CHILD00_TOTAL_BIRTHS]++;
					if (c == 0)
					{
						g++;
						girl->m_ChildrenCount[CHILD02_ALL_GIRLS]++;
						if (child->m_IsPlayers) girl->m_ChildrenCount[CHILD06_YOUR_GIRLS]++;
						else					girl->m_ChildrenCount[CHILD04_CUSTOMER_GIRLS]++;
					}
					else
					{
						b++;
						girl->m_ChildrenCount[CHILD03_ALL_BOYS]++;
						if (child->m_IsPlayers) girl->m_ChildrenCount[CHILD07_YOUR_BOYS]++;
						else					girl->m_ChildrenCount[CHILD05_CUSTOMER_BOYS]++;
					}
					if (g_Dice.percent(healthFactor)) s++;						//check for sterility
				}
				unbornbabies--; // count down and check the next one
			}

			ss << girl->m_Realname << " has given birth";
			if (g + b > 0)		ss << " to ";
			if (g > 0)			ss << g << " girl" << (g > 1 ? "s" : "");
			if (g > 0 && b > 0)	ss << " and ";
			if (b > 0)			ss << b << " boy" << (b > 1 ? "s" : "");
			if (m > 0)
			{
				ss << ".\nShe lost ";
				if (m < t) ss << m << " of her " << t << " babies";
				else ss << "all of her babies, she is very distraught";
			}

			ss << ".\n \nYou grant her the week off ";
			if (g + b > 0)			ss << "for maternity leave";
			if (g + b > 0 && m > 0)	ss << " and ";
			if (m > 0)				ss << "to mourn her lost child" << (m > 1 ? "ren" : "");
			ss << ".\n \n";

			if (s > 0)
			{
				ss << "It was a difficult birth and ";
				if (girl->has_trait( "Broodmother"))
				{
					if (s > 1)	// loose broodmother only if 2 or more miscarriages
					{
						ss << "her womb has been damaged reducing her fertility.\n";
						AdjustTraitGroupFertility(girl, (s == 5 ? -2 : -1));
					}
					else		// otherwise take more damage
					{
						ss << "her womb has been damaged.\n";
						girl->health(-(s + g_Dice % (s * 2)));
						if (girl->health() < 1) SetStat(girl, STAT_HEALTH, 1);	// don't kill her now, it causes all the babies to go away.
					}
				}
				else if (girl->has_trait("Fertile"))	// loose fertile
				{
					ss << "her womb has been damaged " << (s > 3 ? "leaving her sterile" : "reducing her fertility") << ".\n";
					AdjustTraitGroupFertility(girl, (s > 3 ? -2 : -1));
				}
				else	// add sterile
				{
					ss << "she has lost the ability to have children.\n";
					AdjustTraitGroupFertility(girl, -1);
				}
			}
			else if (!girl->has_trait("Broodmother") && t > 1 && g_Dice.percent(t * 5))
			{
				ss << "She has given birth to so many children, her womb has gotten used to carrying babies.\n";
				AdjustTraitGroupFertility(girl, 1);
			}

			if (PlayerControlled) girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_DANGER);

			if (child->m_MultiBirth > 0) return false;	// there are some babies that survived so we need to keep them
			else return true;							// or they all died so we can remove this pregnancy
		}
	}
	/*
	*	It's monster time!
	*
	*	How much does one of these suckers bring on the open market
	*	anyway?
	*
	*	might as well record the transaction and clear the preggo bit
	*	while we're at it
	*/


	if (PlayerControlled)
	{
		int number = child->m_MultiBirth;
		ss << "The ";
		if (number > 1)	{ ss << number << " creatures within her have"; }
		else { number = 1;	ss << "creature within her has"; }
		ss << " matured and emerged from her womb.\n";

		int died = 0; int add = 0; int sell = 0; long gold = 0;
		for (int i = 0; i < number; i++)
		{
			if (g_Dice.percent(cfg.pregnancy.miscarriage_monster() + number - 1))	died++;		// some may die
			else if (g_Dice.percent(child->m_Stats[STAT_BEAUTY]))					add++;		// keep the good looking ones
			else if (g_Dice.percent(child->m_Stats[STAT_CONSTITUTION]))				add++;		// and the realy healthy ones
			else sell++;																		// sell the rest
		}
		girl->m_ChildrenCount[CHILD01_ALL_BEASTS] += add + sell;
		girl->m_ChildrenCount[CHILD08_MISCARRIAGES] += died;
		if (died > 0)
		{
			healthFactor += died;
			if (died > 2) healthFactor += died;	// the more that died the worse off she will be
		}
		if (add > 0)
		{
			g_Brothels.add_to_beasts(add);
		}
		if (sell > 0)
		{
			gold = sell * tariff.creature_sales();
			g_Gold.creature_sales(gold);
		}

		if (died > 0)
		{
			ss << "\nUnfortunately ";
			if (add + sell < 1)
			{
				/* */if (died == 1)	ss << "it did not survive.";
				else if (died == 2)	ss << "neither of them survived.";
				else /*          */	ss << "none of them survived.";
			}
			else
			{
				if (died == 1) ss << "one";
				else ss << died;
				ss << " of them did not survive.";
			}
		}

		if (add > 0)
		{
			ss << "\n";
			if (died + sell < 1)	// all added
			{
				if (add == 1) ss << "It was";
				else ss << "They were " << (add == 2 ? "both" : "all");
				ss << " added to your stable of beasts.";
			}
			else
			{
				if (add == 1) ss << "One of them was";
				else ss << add << " of them were";
				ss << " good enough to be added to your stable of beasts.";
			}
		}

		if (sell > 0)
		{
			ss << "\n";
			if (died + add < 1)	// all sold
			{
				if (sell == 1) ss << "It was";
				else ss << "They were " << (sell ==2 ? "both" : "all");
				ss << " sold for " << tariff.creature_sales() << "gold" << (sell > 1 ? " each" : "") << ".";
			}
			else
			{
				if (sell == 1) ss << "One of them was";
				else ss << sell << " of them were";
				ss << " sold for " << tariff.creature_sales() << " gold" << (sell > 1 ? " each" : "") << ".";
			}
			if (sell > 1) ss << "\nYou made " << int(gold) << " gold for selling " << (sell == 2 ? "both" : "all") << " of them.";
		}

		ss << "\n \nYou grant her the week off for her body to recover.";
	}
	girl->clear_pregnancy();
	/*
	*	check for death
	*/
	if (g_Dice.percent(healthFactor))
	{
		//summary += "And died from it. ";
		ss << "\nSadly, the girl did not survive the experience.";
		girl->m_Stats[STAT_HEALTH] = 0;
	}
	/*
	*	and sterility
	*	slight mod: 1% death, 5% sterility for monster births
	*	as opposed to other way around. Seems better this way.
	*/
	else if (g_Dice.percent(5 + healthFactor))
	{
		// `J` updated old code to use new traits from new code
		ss << "It was a difficult birth and ";
		if (girl->has_trait( "Broodmother"))
		{
			ss << "her womb has been damaged.\n";
			girl->health(-(1 + g_Dice % 2));
			if (girl->health() < 1) g_Girls.SetStat(girl, STAT_HEALTH, 1);	// don't kill her now, it causes all the babies to go away.
		}
		else if (girl->has_trait( "Fertile"))	// loose fertile
		{
			ss << "her womb has been damaged reducing her fertility.\n";
			AdjustTraitGroupFertility(girl, -1);
		}
		else	// add sterile
		{
			ss << "she has lost the ability to have children.\n";
			AdjustTraitGroupFertility(girl, -1);
		}
	}
	/*
	*	queue the message and return TRUE
	*	because we're not interested in watching
	*	little tentacles grow to adulthood
	*/
	if (PlayerControlled) girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_DANGER);
	girl->m_JustGaveBirth = true;
	return true;
}

// `J` this has been replased by trait->m_InheritChance - this is still used if the trait does not have it
bool cGirls::InheritTrait(TraitSpec* trait)
{
	// `J` When adding new traits, search for "J-Add-New-Traits"  :  found in >> InheritTrait

	string name = trait->name();
	if (trait)
	{
		if (name == "Nymphomaniac" ||
			name == "Chaste" ||
			name == "Malformed" ||
			name == "Retarded" ||
			name == "Sterile")
		{
			if (g_Dice.percent(10)) return true;
		}
		if (name == "Fragile" ||
			name == "Fleet of Foot" ||
			name == "Clumsy" ||
			name == "Strong" ||
			name == "Psychic" ||
			name == "Strong Magic")
		{
			if (g_Dice.percent(30)) return true;
		}
		if (name == "Tough" ||
			name == "Fast Orgasms" ||
			name == "Slow Orgasms" ||
			name == "Quick Learner" ||
			name == "Slow Learner" ||
			name == "Chlamydia" ||
			name == "Syphilis" ||
			name == "Herpes" ||

			name == "Shroud Addict" ||
			name == "Fairy Dust Addict" ||
			name == "Viras Blood Addict")
		{
			if (g_Dice.percent(50)) return true;
		}
		if (name == "Perky Nipples" ||
			name == "Puffy Nipples" ||
			name == "Long Legs" ||
			//zzzzzz boobs

			name == "Big Boobs" ||
			name == "Busty Boobs" ||
			name == "Giant Juggs" ||
			name == "Abnormally Large Boobs" ||
			name == "Massive Melons" ||
			name == "Titanic Tits" ||
			name == "Small Boobs" ||
			name == "Petite Breasts" ||
			name == "Flat Chest" ||
			name == "Great Arse" ||
			name == "Great Figure" ||
			name == "AIDS" ||
			name == "Cute" ||
			name == "Strange Eyes" ||
			name == "Different Colored Eyes")
		{
			if (g_Dice.percent(70)) return true;
		}
		if (name == "Demon" ||
			name == "Cat Girl" ||
			name == "Not Human")
			return true;

		// WD: traits that can't be inherited or are a special case
		if (name == "Construct" ||
			name == "Half-Construct" ||
			name == "Cool Scars" ||
			name == "Small Scars" ||
			name == "Horrific Scars" ||
			name == "MILF" ||
			name == "Your Daughter" ||
			name == "Your Wife" ||
			name == "Incest" ||
			name == "One Eye" ||
			name == "Eye Patch" ||
			name == "Assassin" ||
			name == "Adventurer" ||
			name == "Mind Fucked" ||
			name == "Broken Will")
			return false;

		// WD: Any unlisted traits here
		if (g_Dice.percent(10))
			return true;
	}
	return false;
}

void sGirl::OutputGirlRow(string* Data, const vector<string>& columnNames)
{
	for (unsigned int x = 0; x < columnNames.size(); ++x)
	{
		//for each column, write out the statistic that goes in it
		OutputGirlDetailString(Data[x], columnNames[x]);
	}
}

void sGirl::OutputGirlDetailString(string& Data, const string& detailName)
{
	//given a statistic name, set a string to a value that represents that statistic
	static stringstream ss;
	ss.str("");

	bool interrupted = false;	// `J` added
	if ((this) && (this)->m_YesterDayJob != (this)->m_DayJob &&
		(g_Clinic.is_Surgery_Job((this)->m_YesterDayJob) || (this)->m_YesterDayJob == JOB_REHAB) &&
		(((this)->m_WorkingDay > 0) || (this)->m_PrevWorkingDay > 0))
		interrupted = true;

	/* */if (detailName == "Name")				{ ss << m_Realname; }
	else if (detailName == "Health")			{ if (get_stat(STAT_HEALTH) <= 0) ss << "DEAD"; else ss << get_stat(STAT_HEALTH) << "%"; }
	else if (detailName == "Age")				{ if (get_stat(STAT_AGE) == 100) ss << "???"; else ss << get_stat(STAT_AGE); }
	else if (detailName == "Libido")			{ ss << libido(); }
	else if (detailName == "Rebel")				{ ss << rebel(); }
	else if (detailName == "Looks")				{ ss << ((get_stat(STAT_BEAUTY) + get_stat(STAT_CHARISMA)) / 2) << "%"; }
	else if (detailName == "Tiredness")			{ ss << get_stat(STAT_TIREDNESS) << "%"; }
	else if (detailName == "Happiness")			{ ss << get_stat(STAT_HAPPINESS) << "%"; }
	else if (detailName == "Virgin")			{ ss << (this->check_virginity() ? "Yes" : "No"); }
	else if (detailName == "Weeks_Due")
	{
		if (is_pregnant())
		{
			int to_go = ((this)->m_States&(1 << STATUS_INSEMINATED) ? cfg.pregnancy.weeks_monster_p() : cfg.pregnancy.weeks_pregnant()) - (this)->m_WeeksPreg;
			ss << to_go;
		}
		else
		{
			ss << "---";
		}
	}
	else if (detailName == "PregCooldown")		{ ss << m_PregCooldown; }
	else if (detailName == "Accommodation")
	{
		ss << g_Girls.Accommodation(m_AccLevel);
	}
	else if (detailName == "Gold")
	{
		if (g_Gangs.GetGangOnMission(MISS_SPYGIRLS))
		{
			ss << m_Money;
		}
		else
		{
			ss << "???";
		}
	}
	else if (detailName == "Pay")				{ ss << m_Pay; }

	// 'J' Added for .06.03.01
	else if (detailName == "DayJobShort" || detailName == "NightJobShort")
	{
		ss << g_Brothels.m_JobManager.JobQkNm[(detailName == "DayJobShort" ? m_DayJob : m_NightJob)];
	}

	// 'J' Girl Table job text
	else if (detailName == "DayJob" || detailName == "NightJob")
	{
		bool DN_Day = detailName == "NightJob";
		int DN_Job = (DN_Day ? m_NightJob : m_DayJob);

		// `J` When modifying Jobs, search for "J-Change-Jobs"  :  found in >>
		if (DN_Job >= NUM_JOBS)
		{
			ss << "None";
		}
		else if (DN_Job == JOB_FAKEORGASM || DN_Job == JOB_SO_STRAIGHT || DN_Job == JOB_SO_BISEXUAL || DN_Job == JOB_SO_LESBIAN)
		{
			ss << g_House.m_JobManager.JobName[DN_Job] << " (" << m_WorkingDay << "%)";
		}
		else if (DN_Job == JOB_CUREDISEASES)
		{
			if (g_Clinic.GetNumGirlsOnJob(0, JOB_DOCTOR, DN_Day) > 0)
			{
				ss << g_Brothels.m_JobManager.JobName[DN_Job] << " (" << m_WorkingDay << "%)";
			}
			else
			{
				ss << g_House.m_JobManager.JobName[DN_Job] << " (" << m_WorkingDay << "%) **";
			}
		}
		else if (DN_Job == JOB_REHAB || DN_Job == JOB_ANGER || DN_Job == JOB_EXTHERAPY || DN_Job == JOB_THERAPY)
		{
			if (g_Centre.GetNumGirlsOnJob(0, JOB_COUNSELOR, DN_Day) > 0)
			{
				ss << g_Brothels.m_JobManager.JobName[DN_Job] << " (" << 3 - m_WorkingDay << ")";
			}
			else
			{
				ss << g_Brothels.m_JobManager.JobName[DN_Job] << " (?)***";
			}
		}
		else if (DN_Job == JOB_GETHEALING)
		{
			if (g_Clinic.GetNumGirlsOnJob(0, JOB_DOCTOR, DN_Day) > 0)
			{
				ss << g_Brothels.m_JobManager.JobName[DN_Job];
			}
			else
			{
				ss << g_Brothels.m_JobManager.JobName[DN_Job] << " ***";
			}
		}
		else if (DN_Job == JOB_GETREPAIRS)
		{
			if (g_Clinic.GetNumGirlsOnJob(0, JOB_MECHANIC, DN_Day) > 0 &&
				(this->has_trait("Construct") || this->has_trait("Half-Construct")))
			{
				ss << g_Brothels.m_JobManager.JobName[DN_Job];
			}
			else if (this->has_trait("Construct"))
			{
				ss << g_Brothels.m_JobManager.JobName[DN_Job] << " ****";
			}
			else
			{
				ss << g_Brothels.m_JobManager.JobName[DN_Job] << " !!";
			}
		}
		else if (DN_Job == JOB_GETABORT)
		{
			int wdays = (2 - (this)->m_WorkingDay);
			if (g_Clinic.GetNumGirlsOnJob(0, JOB_NURSE, DN_Day) > 0)
			{
				wdays = 1;
			}
			if (g_Clinic.GetNumGirlsOnJob(0, JOB_DOCTOR, DN_Day) > 0)
			{
				ss << g_Brothels.m_JobManager.JobName[DN_Job] << " (" << wdays << ")*";
			}
			else
			{
				ss << g_Brothels.m_JobManager.JobName[DN_Job] << " (?)***";
			}
		}
		else if (g_Clinic.is_Surgery_Job(DN_Job))
		{
			int wdays = (5 - (this)->m_WorkingDay);
			if (g_Clinic.GetNumGirlsOnJob(0, JOB_NURSE, DN_Day) > 0)
			{
				if (wdays >= 3)		{ wdays = 3; }
				else if (wdays > 1)	{ wdays = 2; }
				else				{ wdays = 1; }
			}
			if (g_Clinic.GetNumGirlsOnJob(0, JOB_DOCTOR, DN_Day) > 0)
			{
				ss << g_Brothels.m_JobManager.JobName[DN_Job] << " (" << wdays << ")*";
			}
			else
			{
				ss << g_Brothels.m_JobManager.JobName[DN_Job] << " (?)***";
			}
		}
		else if (g_Studios.is_Actress_Job(DN_Job) && g_Studios.CrewNeeded())
		{
			ss << g_Brothels.m_JobManager.JobName[DN_Job] << " **";
		}
		else if (is_resting() && !was_resting() && m_PrevDayJob != 255 && m_PrevNightJob != 255)
		{
			ss << g_Brothels.m_JobManager.JobName[DN_Job];
			ss << " (" << g_Brothels.m_JobManager.JobQkNm[(DN_Day == 0 ? m_PrevDayJob : m_PrevNightJob)] << ")";
		}
		else
		{
			ss << g_Brothels.m_JobManager.JobName[DN_Job];
		}
		if (interrupted)
		{
			ss << " **";
		}
	}

	else if (detailName.find("STAT_") != string::npos)
	{
		string stat = detailName;
		stat.replace(0, 5, "");
		int code = sGirl::lookup_stat_code(stat);
		if (code != -1)
		{
			ss << get_stat(code);
		}
		else
		{
			ss << "Error";
		}
	}
	else if (detailName.find("SKILL_") != string::npos)
	{
		string skill = detailName;
		skill.replace(0, 6, "");
		int code = sGirl::lookup_skill_code(skill);
		if (code != -1)
		{
			ss << get_skill(code);
		}
		else
		{
			ss << "Error";
		}
	}
	else if (detailName.find("TRAIT_") != string::npos)
	{
		string trait = detailName;
		trait.replace(0, 6, "");
		if (this->has_trait(trait))
		{
			ss << "Yes";
		}
		else
		{
			ss << "No";
		}
	}
	else if (detailName.find("STATUS_") != string::npos)
	{
		string status = detailName;
		status.replace(0, 7, "");
		int code = lookup_status_code(status);
		if (code != -1)
		{
			ss << (m_States&(1 << code) ? "Yes" : "No");
		}
		else
		{
			ss << "Error";
		}
	}
	else if (detailName == "is_pregnant")
	{
		if (this->check_virginity()) ss << "Vg.";
		else if (is_pregnant())
		{
			int to_go = ((this)->m_States&(1 << STATUS_INSEMINATED) ? cfg.pregnancy.weeks_monster_p() : cfg.pregnancy.weeks_pregnant()) - (this)->m_WeeksPreg;
			if (carrying_players_child())	ss << "Yours";
			else if (carrying_monster())	ss << "Beast";
			else /*                      */	ss << "Yes";
			if (has_trait("Sterile") || has_trait("Zombie") || has_trait("Skeleton"))
				ss << "?" << to_go << "?";	// how?
			else
				ss << "(" << to_go << ")";
		}
		else if (m_PregCooldown > 0)
		{
			ss << "No";
			if (has_trait("Sterile") || has_trait("Zombie") || has_trait("Skeleton"))
				ss << "!" << m_PregCooldown << "!";
			else
				ss << "(" << m_PregCooldown << ")";
		}
		else if (has_trait("Zombie") || has_trait("Skeleton")) ss << "Ud.";
		else if (has_trait("Sterile"))	    ss << "St.";
		else if (has_trait("Fertile"))      ss << "No+";
		else if (has_trait("Broodmother"))  ss << "No++";
		else                                ss << "No";
	}
	else if (detailName == "is_slave")			{ ss << (is_slave() ? "Yes" : "No"); }
	else if (detailName == "carrying_human")	{ ss << (carrying_human() ? "Yes" : "No"); }
	else if (detailName == "is_addict")			{ ss << (is_addict() ? "Yes" : "No"); }
	else if (detailName == "has_disease")		{ ss << (has_disease() ? "Yes" : "No"); }
	else if (detailName == "is_mother")			{ ss << (is_mother() ? "Yes" : "No"); }
	else if (detailName == "is_poisoned")		{ ss << (is_poisoned() ? "Yes" : "No"); }
	else if (detailName == "Value")
	{
		g_Girls.CalculateAskPrice(this, 0);
		ss << (int)tariff.slave_price(this, false);
	}
	else if (detailName == "SO")
	{
		/* */if (has_trait("Lesbian"))	ss << "L";
		else if (has_trait("Straight"))	ss << "S";
		else if (has_trait("Bisexual"))	ss << "B";
		else/*                       */	ss << "-";
	}
	else if (detailName == "SexAverage")
	{
		ss << (int)g_Girls.GetAverageOfSexSkills(this);
	}
	else if (detailName == "NonSexAverage")
	{
		ss << (int)g_Girls.GetAverageOfNSxSkills(this);
	}
	else if (detailName == "SkillAverage")
	{
		ss << (int)g_Girls.GetAverageOfAllSkills(this);
	}
	else /*                            */		{ ss << "Not found"; }
	Data = ss.str();
}

int sGirl::rebel()
{
	// return g_Girls.GetRebelValue(this, this->m_DayJob == JOB_MATRON); // `J` old version
	if (this->m_DayJob == JOB_INDUNGEON)	// `J` Dungeon "Matron" can be a Torturer from any brothel
		return g_Girls.GetRebelValue(this, (
		g_Brothels.GetNumGirlsOnJob(0, JOB_TORTURER, 0) > 0 ||
		g_Brothels.GetNumGirlsOnJob(1, JOB_TORTURER, 0) > 0 ||
		g_Brothels.GetNumGirlsOnJob(2, JOB_TORTURER, 0) > 0 ||
		g_Brothels.GetNumGirlsOnJob(3, JOB_TORTURER, 0) > 0 ||
		g_Brothels.GetNumGirlsOnJob(4, JOB_TORTURER, 0) > 0 ||
		g_Brothels.GetNumGirlsOnJob(5, JOB_TORTURER, 0) > 0 ||
		g_Brothels.GetNumGirlsOnJob(6, JOB_TORTURER, 0) > 0));
	else if (this->m_InStudio)	return g_Girls.GetRebelValue(this, g_Studios.GetNumGirlsOnJob(0, JOB_DIRECTOR, 1) > 0);
	else if (this->m_InArena)	return g_Girls.GetRebelValue(this, g_Arena.GetNumGirlsOnJob(0, JOB_DOCTORE, 0) > 0);
	else if (this->m_InCentre)	return g_Girls.GetRebelValue(this, g_Centre.GetNumGirlsOnJob(0, JOB_CENTREMANAGER, 0) > 0);
	else if (this->m_InClinic)	return g_Girls.GetRebelValue(this, g_Clinic.GetNumGirlsOnJob(0, JOB_CHAIRMAN, 0) > 0);
	else if (this->m_InHouse)	return g_Girls.GetRebelValue(this, g_House.GetNumGirlsOnJob(0, JOB_HEADGIRL, 0) > 0);
	else if (this->m_InFarm)	return g_Girls.GetRebelValue(this, g_Farm.GetNumGirlsOnJob(0, JOB_FARMMANGER, 0) > 0);
	else						return g_Girls.GetRebelValue(this, g_Brothels.GetNumGirlsOnJob(this->where_is_she, JOB_MATRON, 0) > 0);
}

// `J` .06.01.17 condensed and added log_extradetails
string sGirl::JobRating(double value, string type, string name)
{
	stringstream jr;
	jr << JobRatingLetter(value) << "  " << type << "  " << name;
	if (cfg.debug.log_extradetails()) jr << "   ( " << (int)value << " )";
	jr << "\n";
	return jr.str();
}

string sGirl::JobRatingLetter(double value)
{
	/* */if (value < -500)		return "X    ";	// Can not do this job
	else if (value == 0)		return "0    ";	// Bad input
	else if (value >= 350)		return "   I  ";	// Incomparable
	else if (value >= 245)		return "  S  ";	// Superior
	else if (value >= 185)		return " A   ";	// Amazing
	else if (value >= 145)		return "B    "; // Better
	else if (value >= 100)		return "C    ";	// Can do it
	else if (value >= 70)		return "D    ";	// Don't bother
	else						return "E    "; // Expect Failure
}

bool sGirl::FixFreeTimeJobs()
{
	bool fixedD = false;
	bool fixedN = false;
	if (this->m_DayJob == JOB_FILMFREETIME || this->m_DayJob == JOB_ARENAREST || this->m_DayJob == JOB_CENTREREST || this->m_DayJob == JOB_CLINICREST ||
		this->m_DayJob == JOB_HOUSEREST || this->m_DayJob == JOB_FARMREST || this->m_DayJob == JOB_RESTING || this->m_DayJob == 255)
	{
		fixedD = true;
		/* */if (this->m_InStudio	&&	this->m_DayJob != JOB_FILMFREETIME)	this->m_DayJob = JOB_FILMFREETIME;
		else if (this->m_InArena	&&	this->m_DayJob != JOB_ARENAREST)	this->m_DayJob = JOB_ARENAREST;
		else if (this->m_InCentre	&&	this->m_DayJob != JOB_CENTREREST)	this->m_DayJob = JOB_CENTREREST;
		else if (this->m_InClinic	&&	this->m_DayJob != JOB_CLINICREST)	this->m_DayJob = JOB_CLINICREST;
		else if (this->m_InHouse	&&	this->m_DayJob != JOB_HOUSEREST)	this->m_DayJob = JOB_HOUSEREST;
		else if (this->m_InFarm		&&	this->m_DayJob != JOB_FARMREST)		this->m_DayJob = JOB_FARMREST;
		else if (!this->m_InStudio && !this->m_InArena && !this->m_InCentre && !this->m_InClinic && !this->m_InHouse
			&& !this->m_InFarm && this->m_DayJob != JOB_RESTING)				this->m_DayJob = JOB_RESTING;
		else fixedD = false;
	}
	if (this->m_NightJob == JOB_FILMFREETIME || this->m_NightJob == JOB_ARENAREST || this->m_NightJob == JOB_CENTREREST || this->m_NightJob == JOB_CLINICREST ||
		this->m_NightJob == JOB_HOUSEREST || this->m_NightJob == JOB_FARMREST || this->m_NightJob == JOB_RESTING || this->m_NightJob == 255)
	{
		fixedN = true;
		/* */if (this->m_InStudio	&&	this->m_NightJob != JOB_FILMFREETIME)	this->m_NightJob = JOB_FILMFREETIME;
		else if (this->m_InArena	&&	this->m_NightJob != JOB_ARENAREST)		this->m_NightJob = JOB_ARENAREST;
		else if (this->m_InCentre	&&	this->m_NightJob != JOB_CENTREREST)		this->m_NightJob = JOB_CENTREREST;
		else if (this->m_InClinic	&&	this->m_NightJob != JOB_CLINICREST)		this->m_NightJob = JOB_CLINICREST;
		else if (this->m_InHouse	&&	this->m_NightJob != JOB_HOUSEREST)		this->m_NightJob = JOB_HOUSEREST;
		else if (this->m_InFarm		&&	this->m_NightJob != JOB_FARMREST)		this->m_NightJob = JOB_FARMREST;
		else if (!this->m_InStudio && !this->m_InArena && !this->m_InCentre && !this->m_InClinic && !this->m_InHouse
			&& !this->m_InFarm && this->m_NightJob != JOB_RESTING)				this->m_NightJob = JOB_RESTING;
		else fixedN = false;
	}

	if (fixedD || fixedN) return true;
	return false;
}

string cGirls::GetHoroscopeName(int month, int day)
{
	// There are 2 types of of horoscopes, 1: Tropical (Western) and 2: Sideral (Hindu)
	int ZodiacType = cfg.initial.horoscopetype();

	if (ZodiacType == 2)	//	Sideral
	{
		switch (month)
		{
		case 1:		if (day <= 14) return "Sagittarius";	else return "Capricorn";
		case 2:		if (day <= 14) return "Capricorn";		else return "Aquarius";
		case 3:		if (day <= 14) return "Aquarius";		else return "Pisces";
		case 4:		if (day <= 14) return "Pisces";			else return "Aries";
		case 5:		if (day <= 15) return "Aries";			else return "Taurus";
		case 6:		if (day <= 15) return "Taurus";			else return "Gemini";
		case 7:		if (day <= 15) return "Gemini";			else return "Cancer";
		case 8:		if (day <= 15) return "Cancer";			else return "Leo";
		case 9:		if (day <= 15) return "Leo";			else return "Virgo";
		case 10:	if (day <= 15) return "Virgo";			else return "Libra";
		case 11:	if (day <= 15) return "Libra";			else return "Scorpio";
		case 12:	if (day <= 15) return "Scorpio";		else return "Sagittarius";
		default:	return "";	break;
		}
	}
	else	//	Tropical
	{
		switch (month)
		{
		case 1:		if (day <= 19) return "Capricorn";		else return "Aquarius";
		case 2:		if (day <= 18) return "Aquarius";		else return "Pisces";
		case 3:		if (day <= 20) return "Pisces";			else return "Aries";
		case 4:		if (day <= 19) return "Aries";			else return "Taurus";
		case 5:		if (day <= 20) return "Taurus";			else return "Gemini";
		case 6:		if (day <= 20) return "Gemini";			else return "Cancer";
		case 7:		if (day <= 22) return "Cancer";			else return "Leo";
		case 8:		if (day <= 22) return "Leo";			else return "Virgo";
		case 9:		if (day <= 22) return "Virgo";			else return "Libra";
		case 10:	if (day <= 22) return "Libra";			else return "Scorpio";
		case 11:	if (day <= 21) return "Scorpio";		else return "Sagittarius";
		case 12:	if (day <= 21) return "Sagittarius";	else return "Capricorn";
		default:	return "";	break;
		}
	}

	return "";
}

bool cGirls::girl_has_matron(sGirl* girl, int shift)
{
	/* */if (girl->m_InArena)		{ if (g_Arena.GetNumGirlsOnJob(0, JOB_DOCTORE, shift) > 0)						return true; }
	else if (girl->m_InStudio)		{ if (g_Studios.GetNumGirlsOnJob(0, JOB_DIRECTOR, shift) > 0)					return true; }
	else if (girl->m_InClinic)		{ if (g_Clinic.GetNumGirlsOnJob(0, JOB_CHAIRMAN, shift) > 0)					return true; }
	else if (girl->m_InCentre)		{ if (g_Centre.GetNumGirlsOnJob(0, JOB_CENTREMANAGER, shift) > 0)				return true; }
	else if (girl->m_InHouse)		{ if (g_House.GetNumGirlsOnJob(0, JOB_HEADGIRL, shift) > 0)						return true; }
	else if (girl->m_InFarm)		{ if (g_Farm.GetNumGirlsOnJob(0, JOB_FARMMANGER, shift) > 0)					return true; }
	else /*                   */	{ if (g_Brothels.GetNumGirlsOnJob(girl->where_is_she, JOB_MATRON, shift) > 0)	return true; }
	return false;
}

string cGirls::Accommodation(int acc)
{
	/* */if (acc == 0)	return "Bare Bones";
	else if (acc == 1)	return "Very Poor";
	else if (acc == 2)	return "Poor";
	else if (acc == 3)	return "Adequate";
	else if (acc == 4)	return "Comfortable";
	else if (acc == 5)	return "Nice";
	else if (acc == 6)	return "Good";
	else if (acc == 7)	return "Great";
	else if (acc == 8)	return "Wonderful";
	else if (acc == 9)	return "High Class";
	else /*         */	return "Error";
}

string cGirls::AccommodationDetails(sGirl* girl, int acc)
{
	if (girl->m_States&(1 << STATUS_SLAVE))
	{
		/* */if (acc == 0)	return "Slave";
		else if (acc == 1)	return "Slave 2";
		else if (acc == 2)	return "Slave 3";
		else if (acc == 3)	return "Slave 4";
		else if (acc == 4)	return "Slave 5";
		else if (acc == 5)	return "Slave 6";
		else if (acc == 6)	return "Slave 7";
		else if (acc == 7)	return "Slave 8";
		else if (acc == 8)	return "Slave 9";
		else if (acc == 9)	return "Slave 10";
		else /*         */	return "Error";
	}
	else
	{
		/* */if (acc == 0)	return "Free";
		else if (acc == 1)	return "2";
		else if (acc == 2)	return "3";
		else if (acc == 3)	return "4";
		else if (acc == 4)	return "5";
		else if (acc == 5)	return "6";
		else if (acc == 6)	return "7";
		else if (acc == 7)	return "8";
		else if (acc == 8)	return "9";
		else if (acc == 9)	return "10";
		else /*         */	return "Error";
	}
}

// The accommodation level the girl expects/demands
int cGirls::PreferredAccom(sGirl* girl)
{
	double preferredaccom = (girl->is_slave() ? 1.0 : 2.5);
	preferredaccom += girl->level() * (girl->is_slave() ? 0.1 : 0.3);
	if (girl->is_pregnant()) preferredaccom += 1.5;

	if (girl->has_trait("Your Wife"))			preferredaccom += 2.0;	// You married her
	else if (girl->has_trait("Your Daughter"))	preferredaccom += 1.0;	// She is your kid
	if (girl->has_trait("Queen"))				preferredaccom += 3.0;	// Royalty is accustomed to higher quality stuff.
	else if (girl->has_trait("Princess"))		preferredaccom += 2.0;	// Royalty is accustomed to higher quality stuff.
	else if (girl->has_trait("Noble"))			preferredaccom += 1.5;	//
	else if (girl->has_trait("Former Official"))preferredaccom += 1.0;	//
	else if (girl->has_trait("Elegant"))		preferredaccom += 0.5;	//
	if (girl->has_trait("Heroine"))				preferredaccom += 0.5;	//
	if (girl->has_trait("Bimbo"))				preferredaccom += 2.0;	// she needs a place to keep all her stuff
	if (girl->has_trait("Broodmother"))			preferredaccom += 2.5;	// she needs somewhere to raise her kids
	if (girl->has_trait("Actress"))				preferredaccom += 2.0;	//
	else if (girl->has_trait("Porn Star"))		preferredaccom += 1.0;	//
	if (girl->has_trait("Idol"))				preferredaccom += 2.0;	//
	if (girl->has_trait("Iron Will"))			preferredaccom += 1.0;	//
	if (girl->has_trait("Nerd"))				preferredaccom += 0.5;	// she probably spends her free time in her room

	if (girl->has_trait("Tomboy"))				preferredaccom -= 0.5;	//
	if (girl->has_trait("Open Minded"))			preferredaccom -= 0.5;	// 'I can sleep anywhere'
	if (girl->has_trait("Dependant"))			preferredaccom -= 1.0;	// she will take what you give her
	if (girl->has_trait("Adventurer"))			preferredaccom -= 1.0;	// she likes sleeping under the stars
	if (girl->has_trait("Farmers Daughter"))	preferredaccom -= 1.5;	//
	else if (girl->has_trait("Country Gal"))	preferredaccom -= 1.5;	//
	if (girl->has_trait("Maid"))				preferredaccom -= 1.0;	//
	if (girl->has_trait("Optimist"))			preferredaccom -= 0.8;	// 'I can make due with what I have'
	else if (girl->has_trait("Pessimist"))		preferredaccom -= 1.0;	// 'whatever'
	if (girl->has_trait("Whore"))				preferredaccom -= 1.5;	//
	if (girl->has_trait("Broken Will"))			preferredaccom -= 1.5;	//
	if (girl->has_trait("Homeless"))			preferredaccom -= 2.0;	// used to live outdoors
	if (girl->has_trait("Masochist"))			preferredaccom -= 2.0;	// 'I deserve to sleep on rocks'
	if (girl->has_trait("Retarded"))			preferredaccom -= 3.0;	//
	if (girl->has_trait("Zombie"))				preferredaccom -= 3.0;	//
	if (girl->has_trait("Skeleton"))			preferredaccom -= 4.0;	//
	if (girl->has_trait("Mind Fucked"))			preferredaccom -= 5.0;	//

	if (girl->m_NumInventory > 0)	// only bother checking items if the girl has at least 1
	{
		if (girl->has_item_j("Chrono Bed") != -1)						preferredaccom -= 2.0;	// She gets a great night sleep so she is happier when she wakes up
		else if (girl->has_item_j("Rejuvenation Bed") != -1)			preferredaccom -= 1.0;	// She gets a good night sleep so she is happier when she wakes up
		if (girl->has_item_j("150 Piece Drum Kit") != -1)				preferredaccom += 0.5;	// Though she may annoy her neighbors and it takes a lot of space, it it fun
		if (girl->has_item_j("Android, Assistance") != -1)			preferredaccom -= 0.5;	// This little guy cleans up for her
		if (girl->has_item_j("Anger Management Tapes") != -1)			preferredaccom -= 0.1;	// When she listens to these it takes her mind off other things
		if (girl->has_item_j("Appreciation Trophy") != -1)			preferredaccom -= 0.1;	// Something nice to look at
		if (girl->has_item_j("Art Easel") != -1)						preferredaccom -= 1.0;	// She can make her room nicer by herself.
		if (girl->has_item_j("Black Cat") != -1)						preferredaccom -= 0.3;	// Small and soft, it mostly cares for itself
		if (girl->has_item_j("Cat") != -1)							preferredaccom -= 0.3;	// Small and soft, it mostly cares for itself
		if (girl->has_item_j("Claptrap") != -1)						preferredaccom -= 0.1;	// An annoying little guy but he does help a little
		if (girl->has_item_j("Computer") != -1)						preferredaccom -= 1.5;	// Something to do but it takes up a little room
		if (girl->has_item_j("Death Bear") != -1)						preferredaccom += 2.0;	// Having a large bear living with her she needs a little more room.
		if (girl->has_item_j("Deathtrap") != -1)						preferredaccom += 1.0;	// Having a large robot guarding her her she needs a little more room.
		if (girl->has_item_j("Free Weights") != -1)					preferredaccom += 0.2;	// She may like the workout but it takes up a lot of room
		if (girl->has_item_j("Guard Dog") != -1)						preferredaccom += 0.2;	// Though she loves having a pet, a large dog takes up some room
		if (girl->has_item_j("Happy Orb") != -1)						preferredaccom -= 0.5;	// She has happy dreams
		if (girl->has_item_j("Relaxation Orb") != -1)					preferredaccom -= 0.5;	// She can relax anywhere
		if (girl->has_item_j("Library Card") != -1)					preferredaccom -= 0.5;	// She has somewhere else to go and she can bring books back, they keep her mind off other things
		if (girl->has_item_j("Lovers Orb") != -1)						preferredaccom -= 0.5;	// She really enjoys her dreams
		if (girl->has_item_j("Nightmare Orb") != -1)					preferredaccom += 0.2;	// She does not sleep well
		if (girl->has_item_j("Pet Spider") != -1)						preferredaccom -= 0.1;	// A little spider, she may be afraid of it but it takes her mind off her room
		if (girl->has_item_j("Room Decorations") != -1)				preferredaccom -= 0.5;	// They make her like her room more.
		if (girl->has_item_j("Safe by Marcus") != -1)					preferredaccom -= 0.3;	// Somewhere to keep her stuff where ske knows no one can get to it.
		if (girl->has_item_j("Smarty Pants") != -1)					preferredaccom -= 0.2;	// A little stuffed animal to hug and squeeze
		if (girl->has_item_j("Stick Hockey Game") != -1)				preferredaccom += 0.3;	// While fun, it takes a lot of room to not break things
		if (girl->has_item_j("Stripper Pole") != -1)					preferredaccom += 0.1;	// She may like the workout but it takes up a lot of room
		if (girl->has_item_j("Television Set") != -1)					preferredaccom -= 2.0;	// When she stares at this, she doesn't notice anything else
		if (girl->has_item_j("The Realm of Darthon") != -1)			preferredaccom -= 0.1;	// She and her friends can have fun together but they need some space to play it
		if (girl->has_item_j("Weekly Social Therapy Session") != -1)	preferredaccom -= 0.1;	// She has somewhere to go and get her troubles off her chest.
	}

	if (preferredaccom <= 0.0) return 0;
	if (preferredaccom >= 9.0) return 9;
	return (int)preferredaccom;
}

// `J` the girl will check the customer for diseases before continuing.
bool cGirls::detect_disease_in_customer(sBrothel * brothel, sGirl* girl, sCustomer* Cust, double mod)
{
	string girlName = girl->m_Realname;
	stringstream ss;
	if (g_Dice.percent(0.1))	// 0.001 chance of false positive
	{
		ss << girlName << " thought she detected that her customer had a disease and refused to allow them to touch her just to be safe.";
		g_MessageQue.AddToQue(ss.str(), COLOR_RED);
		girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_WARNING);
		return true;
	}
	// if the customer is clean, then it will return false
	if (!Cust->m_HasAIDS && !Cust->m_HasChlamydia && !Cust->m_HasHerpes && !Cust->m_HasSyphilis) return false;
	// 10% chance to miss it
	if (g_Dice.percent(10))	return false;

	double detectdisease = 1.0;												// base 1% chance
	detectdisease += mod;													// add mod
	detectdisease += girl->medicine() / 2.0;								// +50 medicine
	detectdisease += girl->intelligence() / 5.0;							// +20 intelligence
	detectdisease += girl->magic() / 5.0;									// +20 magic
	detectdisease -= girl->libido() / 2.0;									// -50 libido

	if (girl->has_disease())						detectdisease += 20;	// has it so know what to look for
	if (girl->is_addict(true))						detectdisease -= 20;	// if your high your not paying any mind to things
	if (girl->has_trait("Alcoholic"))				detectdisease -= 20;	// if your drunk your not paying any mind to things
	if (girl->has_trait("Bimbo"))					detectdisease -= 20;	//
	if (girl->has_trait("Blind"))					detectdisease -= 20;	// can't see it
	if (girl->has_trait("Broken Will"))				detectdisease -= 90;	//
	if (girl->has_trait("Canine"))					detectdisease += 20;	// I can smell it in you
	if (girl->has_trait("Cat Girl"))				detectdisease += 10;	//
	if (girl->has_trait("Chaste"))					detectdisease -= 10;	// not shure what to look for
	if (girl->has_trait("Clumsy"))					detectdisease -= 10;	//
	if (girl->has_trait("Construct"))				detectdisease += 5;		//
	if (girl->has_trait("Country Gal"))				detectdisease += 5;		//
	if (girl->has_trait("Cum Addict"))				detectdisease -= 50;	//
	if (girl->has_trait("Demon"))					detectdisease += 10;	//
	if (girl->has_trait("Dependant"))				detectdisease -= 50;	//
	if (girl->has_trait("Doctor"))					detectdisease += 50;	// knows what to look for
	if (girl->has_trait("Dominatrix"))				detectdisease += 10;	// force an truth out
	if (girl->has_trait("Dryad"))					detectdisease += 5;		//
	if (girl->has_trait("Elf"))						detectdisease += 5;		//
	if (girl->has_trait("Emprisoned Customer"))		detectdisease += 10;	// suspicious of everyone
	if (girl->has_trait("Fallen Goddess"))			detectdisease += 10;	//
	if (girl->has_trait("Farmers Daughter"))		detectdisease += 5;		//
	if (girl->has_trait("Fearless"))				detectdisease += 5;		//
	if (girl->has_trait("Former Official"))			detectdisease += 10;	//
	if (girl->has_trait("Goddess"))					detectdisease += 20;	//
	if (girl->has_trait("Idiot Savant"))			detectdisease += 5;		//
	if (girl->has_trait("Iron Will"))				detectdisease += 10;	// I'm not letting you touch me until you answer the question
	if (girl->has_trait("Kidnapped"))				detectdisease += 10;	//
	if (girl->has_trait("Lesbian"))					detectdisease += 10;	// all men are diseased
	if (girl->has_trait("Maid"))					detectdisease += 5;		// regular cleaning check
	if (girl->has_trait("Masochist"))				detectdisease -= 10;	// hurt me bad
	if (girl->has_trait("Meek"))					detectdisease -= 50;	//
	if (girl->has_trait("Merciless"))				detectdisease += 5;		//
	if (girl->has_trait("Mind Fucked"))				detectdisease -= 200;	//
	if (girl->has_trait("Mute"))					detectdisease -= 10;	// can't say no
	if (girl->has_trait("Nerd"))					detectdisease += 5;		//
	if (girl->has_trait("Nervous"))					detectdisease -= 10;	//
	if (girl->has_trait("Noble"))					detectdisease += 5;		// commoners are dirty
	if (girl->has_trait("Nymphomaniac"))			detectdisease -= 50;	// too horny to check
	if (girl->has_trait("Old"))						detectdisease -= 10;	//
	if (girl->has_trait("Open Minded"))				detectdisease += 1;		//
	if (girl->has_trait("Optimist"))				detectdisease -= 10;	// I'll never get sick
	if (girl->has_trait("Pessimist"))				detectdisease -= 10;	// I will get sick
	if (girl->has_trait("Pierced Clit"))			detectdisease -= 5;		//
	if (girl->has_trait("Pierced Navel"))			detectdisease -= 5;		//
	if (girl->has_trait("Pierced Nipples"))			detectdisease -= 5;		//
	if (girl->has_trait("Pierced Nose"))			detectdisease -= 5;		//
	if (girl->has_trait("Pierced Tongue"))			detectdisease -= 5;		//
	if (girl->has_trait("Porn Star"))				detectdisease += 10;	// manditory tests for porn stars
	if (girl->has_trait("Powerful Magic"))			detectdisease += 10;	//
	if (girl->has_trait("Priestess"))				detectdisease += 10;	//
	if (girl->has_trait("Princess"))				detectdisease += 10;	// commoners are dirty
	if (girl->has_trait("Psychic"))					detectdisease += 20;	//
	if (girl->has_trait("Queen"))					detectdisease += 20;	// commoners are dirty
	if (girl->has_trait("Quick Learner"))			detectdisease += 5;		//
	if (girl->has_trait("Reptilian"))				detectdisease += 5;		//
	if (girl->has_trait("Retarded"))				detectdisease -= 80;	//
	if (girl->has_trait("Sadistic"))				detectdisease -= 5;		//
	if (girl->has_trait("Sharp-Eyed"))				detectdisease += 10;	//
	if (girl->has_trait("Slut"))					detectdisease -= 20;	//
	if (girl->has_trait("Strong Magic"))			detectdisease += 5;		//
	if (girl->has_trait("Succubus"))				detectdisease += 10;	// I can smell your disease
	if (girl->has_trait("Teacher"))					detectdisease += 5;		// knows what to look for
	if (girl->has_trait("Tsundere"))				detectdisease += 5;		//
	if (girl->has_trait("Twisted"))					detectdisease -= 10;	//
	if (girl->has_trait("Vampire"))					detectdisease += 20;	// I can smell it in your blood
	if (girl->check_virginity())					detectdisease -= 20;	// not sure what to look for
	if (girl->has_trait("Whore"))					detectdisease += 20;	// I've seen it all
	if (girl->has_trait("Yandere"))					detectdisease += 5;		//
	if (girl->has_trait("Your Daughter"))			detectdisease += 30;	// you taught her what to look out for
	if (girl->has_trait("Your Wife"))				detectdisease += 10;	// she knows what to look out for

	// these need better texts
	if (Cust->m_HasAIDS && g_Dice.percent(min(90.0, detectdisease*0.5)))	// harder to detect
	{
		ss << girlName << " detected that her customer has AIDS and refused to allow them to touch her.";
		g_MessageQue.AddToQue(ss.str(), COLOR_RED);
		girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_WARNING);
		brothel->m_RejectCustomersDisease++;
		return true;
	}
	if (Cust->m_HasSyphilis && g_Dice.percent(detectdisease*0.8))	// harder to detect
	{
		ss << girlName << " detected that her customer has Syphilis and refused to allow them to touch her.";
		g_MessageQue.AddToQue(ss.str(), COLOR_RED);
		girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_WARNING);
		brothel->m_RejectCustomersDisease++;
		return true;
	}
	if (Cust->m_HasChlamydia && g_Dice.percent(detectdisease))
	{
		ss << girlName << " detected that her customer has Chlamydia and refused to allow them to touch her.";
		g_MessageQue.AddToQue(ss.str(), COLOR_RED);
		girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_WARNING);
		brothel->m_RejectCustomersDisease++;
		return true;
	}
	if (Cust->m_HasHerpes && g_Dice.percent(detectdisease))
	{
		ss << girlName << " detected that her customer has Herpes and refused to allow them to touch her.";
		g_MessageQue.AddToQue(ss.str(), COLOR_RED);
		girl->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_WARNING);
		brothel->m_RejectCustomersDisease++;
		return true;
	}
	return false;
}

string cGirls::catacombs_look_for(int girls, int items, int beast)
{
	stringstream ss;
	ss << "You tell them to ";


	// over 99%
	if (girls >= 99 || items >= 99 || beast >= 99)
	{
		ss << "only bring back ";
		if (girls >= 99)	ss << "girls";
		if (items >= 99)	ss << "items";
		if (beast >= 99)	ss << "beasts";
	}

	// over 80%
	else if (girls >= 80 || items >= 80 || beast >= 80)
	{
		ss << "mostly try to only bring back ";
		if (girls >= 80)
		{
			ss << "girls but bring back ";
			/* */if (items < 1) ss << "any beasts";
			else if (beast < 1) ss << "any items";
			else ss << "anything else";
		}
		else if (items >= 80)
		{
			ss << "items but bring back ";
			/* */if (girls < 1) ss << "any beasts";
			else if (beast < 1) ss << "any girls";
			else ss << "anything else";
		}
		else if (beast >= 80)
		{
			ss << "beasts but bring back ";
			/* */if (items < 1) ss << "any girls";
			else if (girls < 1) ss << "any items";
			else ss << "anything else";
		}
		ss << " they find as well";
	}

	// over 50%
	else if (girls >= 50 || items >= 50 || beast >= 50)
	{
		ss << "bring back any ";
		if (girls >= 50)
		{
			ss << "girls and ";
			/* */if (beast > 40) ss << "beasts";
			else if (items > 40) ss << "items";
			else ss << "anything else they find";
		}
		else if (items >= 50)
		{
			ss << "items and ";
			double remainder = 100 - items;
			/* */if (beast > 40)	ss << "beasts";
			else if (girls > 40)	ss << "girls";
			else ss << "anything else they find";
		}
		else if (beast >= 50)
		{
			ss << "beasts and ";
			double remainder = 100 - beast;
			/* */if (girls > 40) ss << "girls";
			else if (items > 40) ss << "items";
			else ss << "anything else they find";
		}
	}
	else ss << "bring back anything they can find";

	//else  ss << "have fun. (error code::  CLF01|" << girls << "|" << items << "|" << beast << "  ::  Please report it to pinkpetal.org so it can be fixed)";
	ss << ".\n";
	return ss.str();
}

// `J` for use with beast jobs where the girl has sex with a beast
sCustomer* cGirls::GetBeast()
{
	sCustomer* beast = new sCustomer;
	beast->m_Amount = 1;
	beast->m_IsWoman = 0;
	// get their stats generated
	for (int j = 0; j < NUM_STATS; j++)		beast->m_Stats[j] = g_Dice % 100;
	for (int j = 0; j < NUM_SKILLS; j++)	beast->m_Skills[j] = g_Dice % 10;

	beast->m_Skills[SKILL_BEASTIALITY] = 40 + g_Dice % 61;
	beast->m_Skills[SKILL_COMBAT] = 40 + g_Dice % 61;
	beast->m_Skills[SKILL_FARMING] = g_Dice % 20;
	beast->m_Skills[SKILL_ANIMALHANDLING] = 40 + g_Dice % 61;
	beast->m_Stats[STAT_INTELLIGENCE] = g_Dice % 30;
	beast->m_Stats[STAT_LEVEL] = 0;
	beast->m_Stats[STAT_AGE] = g_Dice % 20;
	beast->m_Stats[STAT_STRENGTH] = 20+g_Dice % 81;

	beast->m_SexPref = beast->m_SexPrefB = SKILL_BEASTIALITY;

	beast->m_HasAIDS = g_Dice.percent(0.5);
	beast->m_HasChlamydia = g_Dice.percent(1);
	beast->m_HasSyphilis = g_Dice.percent(1.5);
	beast->m_HasHerpes = g_Dice.percent(2.5);
	beast->m_Money = 0;
	beast->m_Next = 0;

	return beast;
}


int sGirl::get_skill(int skill_id)
{
	int value = (m_Skills[skill_id]) + m_SkillTemps[skill_id] + m_SkillMods[skill_id] + m_SkillTr[skill_id];
	if (value > 100)	value = 100;
	if (has_trait("Vampire"))
	{
		if (g_Brothels.m_Processing_Shift == 0)			value -= 10;
		else if (g_Brothels.m_Processing_Shift == 1)	value += 10;
	}
	if (value < 0)			value = 0;
	else if (value > 100)	value = 100;
	return value;
}

int sGirl::upd_stat(int stat_id, int amount, bool usetraits) {
	u_int stat = stat_id;
	int value = 0, min = 0, max = 100;
	switch (stat)
	{
		// `J` first do special cases that do not follow standard stat 0 to 100 rule
		case STAT_AGE:
			if (m_Stats[STAT_AGE] != 100 && amount != 0)	// WD: Dont change imortal age = 100
			{
				value = m_Stats[stat] + amount;
				if (value > 80)			value = 80;
				else if (value < 18)	value = 18;	// `J` Legal Note: 18 is the Legal Age of Majority for the USA where I live
				m_Stats[stat] = value;
			}
			return get_stat(stat_id);
			break;
			// now for the stats with modifiers
		case STAT_HEALTH:
		case STAT_TIREDNESS:
			if (has_trait("Incorporeal"))
			{
				m_Stats[STAT_HEALTH] = 100;	// WD: Sanity - Incorporeal health should allways be at 100%
				m_Stats[STAT_TIREDNESS] = 0;	// WD: Sanity - Incorporeal Tiredness should allways be at 0%
				return get_stat(stat_id);
			}
			if (stat == STAT_TIREDNESS && (has_trait("Skeleton") || has_trait("Zombie")))
			{
				m_Stats[STAT_TIREDNESS] = 0;	// `J` Sanity - Zombie Tiredness should allways be at 0%
				return get_stat(stat_id);
			}
			if (!usetraits) break;

			if (has_trait("Fragile"))	// 20% health and 10% tired penalties
			{
				if (stat == STAT_HEALTH)
				{
					if (amount > 0)			// gain health - a little less than normal
					{	// reduce the intended amount healed by 20% or at least 1 except keeping at least 1 point healed
						int mod = amount / 5;
						if (mod < 1) mod = 1;
						amount -= mod;
						if (amount < 1) amount = 1;
					}
					else if (amount < 0)	// lose health - a little more than normal
					{	// increase the amound of damage taken by 20% or at least 1 extra point lost
						int mod = -amount / 5;
						if (mod < 1) mod = 1;
						amount -= mod;
					}
					else // (amount == 0)	// no change intended but possibly injure her anyway
					{	// based on her constitution
						if (!g_Dice.percent(constitution()))
							amount -= 1;
					}
				}
				else	// STAT_TIREDNESS
				{
					if (amount > 0)			// gain tiredness
					{	// increase the amound of tiredness gained by 10% (+0 ok)
						int mod = amount / 10;
						amount += mod;
					}
					else if (amount < 0)	// lose tiredness
					{	// reduce the intended rest gained by 10% (-0 ok)
						int mod = -amount / 10;
						amount += mod;
					}
					else // (amount == 0)	// no change intended but possibly tire her anyway
					{	// based on her constitution but only 50% of the time
						if (!g_Dice.percent(constitution()) && g_Dice.percent(50))
							amount += 1;
					}
				}
			}
			if (has_trait("Delicate"))	// 20% tired and 10% health penalties
			{
				if (stat == STAT_HEALTH)
				{
					if (amount > 0)			// gain health - a little less than normal
					{	// reduce the intended amount healed by 10% (-0 ok) keeping at least 1 point healed
						int mod = amount / 10;
						amount -= mod;
						if (amount < 1) amount = 1;
					}
					else if (amount < 0)	// lose health - a little more than normal
					{	// increase the amound of damage taken by 10% (+0 ok)
						int mod = -amount / 10;
						amount -= mod;
					}
					else // (amount == 0)	// no change intended but possibly injure her anyway
					{	// based on her constitution but only 50% of the time
						if (!g_Dice.percent(constitution()) && g_Dice.percent(50))
							amount -= 1;
					}
				}
				else	// STAT_TIREDNESS
				{
					if (amount > 0)			// gain tiredness
					{	// increase the amound of tiredness gained by 20% and at least 1
						int mod = amount / 5;
						if (mod < 1) mod = 1;
						amount += mod;
					}
					else if (amount < 0)	// lose tiredness
					{	// decrease the amound of tiredness recovered by 20% and at least 1 but recovering at least 1
						int mod = -amount / 5;
						if (mod < 1) mod = 1;
						amount += mod;
						if (amount > -1) amount = -1;
					}
					else // (amount == 0)	// no change intended but possibly tire her anyway
					{	// based on her constitution
						if (!g_Dice.percent(constitution()))
							amount += 1;
					}
				}
			}
			if (has_trait("Tough"))	// 20% health and 10% tired bonuses
			{
				if (stat == STAT_HEALTH)
				{
					if (amount > 0)			// gain health
					{	// increase the amount of health gained by 20% and at least 1 point
						int mod = amount / 5;
						if (mod < 1) mod = 1;
						amount += mod;
					}
					else if (amount < 0)	// lose health
					{	// reduce the amount of health lost by 20% and at least 1 but lose at least 1
						int mod = -amount / 5;
						if (mod < 1) mod = 1;
						amount += mod;
						if (amount > -1) amount = -1;
					}
					else // (amount == 0)	// no change intended but possibly heal her anyway
					{	// based on her constitution
						if (g_Dice.percent(constitution()))
							amount += 1;
					}
				}
				else	// STAT_TIREDNESS
				{
					if (amount > 0)			// gain tiredness
					{	// decrease the amount of tiredness gained by 10% (-0 ok) but gain at least 1
						int mod = amount / 10;
						amount -= mod;
						if (amount < 1) amount = 1;
					}
					else if (amount < 0)	// lose tiredness
					{	// increase the amount of tiredness lost by 10% (+0 ok)
						int mod = -amount / 10;
						amount -= mod;
					}
					else // (amount == 0)	// no change intended but possibly rest her anyway
					{	// based on her constitution but only 50% of the time
						if (g_Dice.percent(constitution()) && g_Dice.percent(50))
							amount -= 1;
					}
				}
			}
			/*		`J` need another trait that will cover this
            if (has_trait("????????????"))		// 20% tired and 10% health bonuses
            {
            if (stat == STAT_HEALTH)
            {
            if (amount > 0)			// gain health
            {

            }
            else if (amount < 0)	// lose health
            {

            }
            else // (amount == 0)	// no change
            {

            }
            amount -= 2;
            }
            else	// STAT_TIREDNESS
            {
            if (amount > 0)			// gain tiredness
            {

            }
            else if (amount < 0)	// lose tiredness
            {

            }
            else // (amount == 0)	// no change
            {

            }
            amount += 2;
            }
            }
            //*/
			if (has_trait("Construct") && ((stat == STAT_HEALTH && amount < 0) || (stat == STAT_TIREDNESS && amount > 0)))
				amount = (int)ceil((float)amount * 0.1); // constructs take 10% damage
			break;

		case STAT_HAPPINESS:
			if (has_trait("Pessimist") && g_Dice % 5 == 1 && usetraits)	amount -= 1; // `J` added
			if (has_trait("Optimist") && g_Dice % 5 == 1 && usetraits)		amount += 1; // `J` added
			break;

		case STAT_LEVEL:
			max = 255;
			break;
		case STAT_EXP:
			max = 32000;
			break;
		case STAT_PCFEAR:
		case STAT_PCLOVE:
		case STAT_PCHATE:
		case STAT_MORALITY:
		case STAT_REFINEMENT:
		case STAT_DIGNITY:
		case STAT_LACTATION:
			min = -100;
			break;
			// and the rest
		default:
			break;
	}
	if (amount != 0)
	{
		value = m_Stats[stat] + amount;
		if (value > max)		value = max;
		else if (value < min)	value = min;
		m_Stats[stat] = value;
	}
	return get_stat(stat_id);
}

int sGirl::upd_skill(int skill_id, int amount, bool usetraits) {
	if (amount >= 0)
	{
		m_Skills[skill_id] = min(100, amount + m_Skills[skill_id]);
	}
	else
	{
		m_Skills[skill_id] = max(0, amount + m_Skills[skill_id]);
	}
	return get_skill(skill_id);
}

bool sGirl::check_virginity() {
	if (has_trait("Virgin") && m_Virgin == 1) // `J` if already correct settings then return true
	{
		return true;
	}
	else if (has_trait("Virgin"))	// `J` if not set correctly, set it correctly and return true
	{
		m_Virgin = 1;
		return true;
	}
	else if (m_Virgin == 1)	// `J` if not set correctly, set it correctly and return true
	{
		add_trait("Virgin");
		return true;
	}
	else if (m_Virgin == 0) // `J` if already correct settings then return false
	{
		return false;
	}
	else if (m_Stats[STAT_AGE] < 18)	// `J` If she just turned 18 she should not legally have had sex yet
	{
		m_Stats[STAT_AGE] = 18;	// `J` Legal Note: 18 is the Legal Age of Majority for the USA where I live
		m_Virgin = 1;
		add_trait("Virgin");
		remove_trait("MILF");
		return true;
	}
	else	// `J` average all sex skills plus age
	{
		int totalsex = m_Stats[STAT_AGE];
		int div = 1;
		for (u_int i = 0; i < NUM_SKILLS; i++)
		{
			// `J` removed nonsex from virginity check
			if (i != SKILL_SERVICE && i != SKILL_MAGIC && i != SKILL_COMBAT && i != SKILL_MEDICINE && i != SKILL_PERFORMANCE && i != SKILL_COOKING &&
				i != SKILL_CRAFTING && i != SKILL_HERBALISM && i != SKILL_FARMING && i != SKILL_BREWING && i != SKILL_ANIMALHANDLING)
			{
				totalsex += m_Skills[i];
				div++;	// `J` added to allow new skills
			}
		}
		int avg = totalsex / div;	// `J` fixed to allow new skills
		if (avg < 20)
		{
			m_Virgin = 1;
			add_trait("Virgin");
			return true;
		}
		else
		{
			m_Virgin = 0;
			return false;
		}
	}
	if (m_Virgin == 1) add_trait("Virgin");
	else { m_Virgin = 0; remove_trait("Virgin"); }
	return (m_Virgin == 1);
}

bool sGirl::regain_virginity() {
	/*  Very similar to (and uses) AddTrait(). Added since trait "Virgin" created 04/14/2013.
    *	This includes capability for items, magic or other processes
    *	to have a "remove but remember" effect, like a "Belt of False Defloration"
    *	that provides a magical substitute vagina, preserving the original while worn.
    *	Well, why not?		DustyDan
    */

	bool traitOpSuccess = false;
	m_Virgin = 1;
	//	Let's avoid re-inventing the wheel
	traitOpSuccess = add_trait("Virgin", false, false, false);
	return traitOpSuccess;
}

// returns false if she becomes pregnant or true if she does not
bool sGirl::calc_pregnancy(int chance, int type, const int stats[NUM_STATS], const int skills[NUM_SKILLS]) {
    /*
	*	If there's a condition that would stop her getting preggers
	*	then we get to go home early
	*
	*	return TRUE to indicate that pregnancy is FALSE
	*	(actually, supposed to mean that contraception is true,
	*	but it also applies for things like being pregnant,
	*	or just blowing the dice roll. That gets confusing too.
	*/
    if (has_contraception(this)) return true;

    string text = "She has";
    /*
    *	for reasons I do not understand, but nevertheless think
    *	are kind of cool, virgins have a +10 to their pregnancy
    *	chance
    */
    if (check_virginity() && chance > 0) chance += 10;
    /*
    *	the other effective form of contraception, of course,
    *	is failing the dice roll. Let's check the chance of
    *	her NOT getting preggers here
    */
    if (has_trait( "Broodmother") && chance > 0)	chance += 60;//this should work CRAZY
    else if (has_trait( "Fertile") && chance > 0)	chance += 30;//this should work CRAZY
    if (g_Dice.percent(100 - chance)) return true;
    /*
    *	narrative depends on what it was that Did The Deed
    *	specifically, was it human or not?
    */
    switch (type)
    {
        case STATUS_INSEMINATED:
            text += " been inseminated.";
            break;
        case STATUS_PREGNANT_BY_PLAYER:
            text += " become pregnant with your child.";
            break;
        case STATUS_PREGNANT:
        default:
            type = STATUS_PREGNANT;		// `J` rearranged and added default to make sure there are no complications
            text += " become pregnant.";
            break;
    }

    m_Events.AddMessage(text, IMGTYPE_PREGNANT, EVENT_DANGER);
    g_Girls.CreatePregnancy(this, 1, type, stats, skills);
    return false;
}
