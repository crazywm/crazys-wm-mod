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

#ifndef __CGIRL_H
#define __CGIRL_H

#include <map>
#include <iostream>
#include <iomanip>
#include <string>
#include <fstream>

#include "Constants.h"
#include "cTraits.h"
#include "cInventory.h"
#include "cCustomers.h"
#include "cEvents.h"
#include "CSurface.h"
#include "cTriggers.h"
//#include "cNameList.h"
#include "cAnimatedSurface.h"
#include "cFont.h"
#include "cImageItem.h"

extern cRng g_Dice;

using namespace std;

// Prototypes
class	cIndexedList;
struct	sInventoryItem;
struct	sBrothel;
class	TiXmlElement;
class   cPlayer;
struct  sCustomer;
struct  sGang;

// structure to hold randomly generated girl information
typedef struct sRandomGirl
{
	sRandomGirl();
	~sRandomGirl();

	string m_Name;
	string m_Desc;

	bool m_newRandom;
	bool* m_newRandomTable;

	bool m_Human;							// 1 means they are human otherwise they are not
	bool m_Catacomb;						// 1 means they are a monster found in catacombs, 0 means wanderer
	bool m_Arena;							// 1 means they are fighter found in arena
	bool m_YourDaughter;					// `J` 1 means they are your daughter
	bool m_IsDaughter;						// 1 means they are a set daughter

	int m_MinStats[NUM_STATS];			    // min and max stats they may start with
	int m_MaxStats[NUM_STATS];

	int m_MinSkills[NUM_SKILLS];		    // min and max skills they may start with
	int m_MaxSkills[NUM_SKILLS];

	int m_NumTraits;						// number of traits they are assigned
	int m_NumTraitNames;					// number of traits they are assigned
	TraitSpec* m_Traits[MAXNUM_TRAITS];		// List of traits they may start with
	int m_TraitChance[MAXNUM_TRAITS];		// the percentage change for each trait
	int m_TraitChanceB[200];
	string m_TraitNames[200];				// `J` fix for more than MAXNUM_TRAITS in .rgirlsx files

	// `J` added starting items for random girls
	int m_NumItems;
	int m_NumItemNames;
	sInventoryItem* m_Inventory[MAXNUM_INVENTORY];
	int m_ItemChance[MAXNUM_GIRL_INVENTORY];
	int m_ItemChanceB[200];
	string m_ItemNames[200];


	int m_MinMoney;	// min and max money they can start with
	int m_MaxMoney;

	sRandomGirl* m_Next;
	/*
	*	MOD: DocClox Sun Nov 15 06:11:43 GMT 2009
	*	stream operator for debugging
	*	plus a shitload of XML loader funcs
	*/
	friend ostream& operator<<(ostream &os, sRandomGirl &g);
	/*
	*	one func to load the girl node,
	*	and then one each for each embedded node
	*
	*	Not so much difficult as tedious.
	*/
	void load_from_xml(TiXmlElement*);	// uses sRandomGirl::load_from_xml
	void process_trait_xml(TiXmlElement*);
	void process_item_xml(TiXmlElement*);
	void process_stat_xml(TiXmlElement*);
	void process_skill_xml(TiXmlElement*);
	void process_cash_xml(TiXmlElement*);
	/*
	*	END MOD
	*/
	static sGirl* lookup;  // used to look up stat and skill IDs
}sRandomGirl;

typedef struct sChild
{
	int m_MultiBirth;
	string multibirth_str()
	{
		if (m_MultiBirth == 2) return "Twins";
		if (m_MultiBirth == 3) return "Triplets";
		if (m_MultiBirth == 4) return "Quads";
		if (m_MultiBirth == 5) return "Quints";
		// `J` anything else is single
		m_MultiBirth = 1;
		return "Single";
	};
	enum Gender {
		None = -1,
		Girl = 0,
		Boy = 1
	};
	Gender m_Sex;
	int m_GirlsBorn;			// if multiple births, how many are girls
	string boy_girl_str()
	{
		if (m_MultiBirth == 2)	return "twins";
		if (m_MultiBirth == 3)	return "triplets";
		if (m_MultiBirth > 3)	return "a litter";
		if (m_Sex == Boy)		return "a baby boy";
		if (m_Sex == Girl)		return "a baby girl";
		return "a baby";
	}
	bool is_boy()	{ return m_Sex == Boy; }
	bool is_girl()	{ return m_Sex == Girl; }

	int m_Age;	// grows up at 60 weeks
	bool m_IsPlayers;	// 1 when players
	unsigned char m_Unborn;	// 1 when child is unborn (for when stats are inherited from customers)

	// skills and stats from the father
	int m_Stats[NUM_STATS];
	int m_Skills[NUM_SKILLS];

	sChild* m_Next;
	sChild* m_Prev;

	sChild(bool is_players = false, Gender gender = None, int MultiBirth = 1);
	~sChild(){ m_Prev = 0; if (m_Next)delete m_Next; m_Next = 0; }

	TiXmlElement* SaveChildXML(TiXmlElement* pRoot);
	bool LoadChildXML(TiXmlHandle hChild);

}sChild;
/*
a class to handle all the child related code to prevent errors.
*/
class cChildList
{
public:

	sChild* m_FirstChild;
	sChild* m_LastChild;
	int m_NumChildren;
	cChildList(){ m_FirstChild = 0; m_LastChild = 0; m_NumChildren = 0; }
	~cChildList(){ if (m_FirstChild) delete m_FirstChild; }
	void add_child(sChild*);
	sChild* remove_child(sChild*, sGirl*);
	//void handle_childs();
	//void save_data(ofstream);
	//void write_data(ofstream);
	//sChild* GenerateBornChild();// need to figure out what the player/customer base class is and if needed create one
	//sChild* GenerateUnbornChild();

};

// Represents a single girl
struct sGirl
{
	sGirl();
	~sGirl();

	int m_newRandomFixed;

	string m_Name;								// The girls name
	string m_Realname;							// this is the name displayed in text
	/*	`J` adding first and surnames for future use.
	*	m_Realname will be used for girl tracking until first and surnames are fully integrated
	*	a girl id number system may be added in the future to allow for absolute tracking
	*/
	string m_FirstName;							// this is the girl's first name
	string m_MiddleName;						// this is the girl's middle name
	string m_Surname;							// this is the girl's surname
	string m_MotherName;						//	`J` added mother and father names
	string m_FatherName;						//	`J` added mother and father names
	/*
	*	MOD: changed from char* -- easier to change from lua -- doc
	*/
	string m_Desc;								// Short story about the girl

	unsigned char m_NumTraits;					// current number of traits they have
	TraitSpec* m_Traits[MAXNUM_TRAITS];			// List of traits they have
	int m_TempTrait[MAXNUM_TRAITS];	// a temp trait if not 0. Trait removed when == 0. traits last for 20 weeks.

	unsigned char m_NumRememTraits;				// number of traits that are apart of the girls starting traits
	TraitSpec* m_RememTraits[MAXNUM_TRAITS * 2];		// List of traits they have inbuilt

	unsigned int m_DayJob;						// id for what job the girl is currently doing
	unsigned int m_NightJob;					// id for what job the girl is currently doing
	unsigned int m_PrevDayJob;					// id for what job the girl was doing
	unsigned int m_PrevNightJob;				// id for what job the girl was doing
	unsigned int m_YesterDayJob;				// id for what job the girl did yesterday
	unsigned int m_YesterNightJob;				// id for what job the girl did yesternight

	//ADB needs to be int because player might have more than 256
	int m_NumInventory;							// current amount of inventory they have
	sInventoryItem* m_Inventory[MAXNUM_GIRL_INVENTORY];		// List of inventory items they have (40 max)
	unsigned char m_EquipedItems[MAXNUM_GIRL_INVENTORY];	// value of > 0 means equipped (wearing) the item

	long m_States;								// Holds the states the girl has
	long m_BaseStates;							// `J` Holds base states the girl has for use with equipable items

	// Abstract stats (not shown as numbers but as a raiting)
	int m_Stats[NUM_STATS];
	int m_StatTr[NUM_STATS];					// Trait modifiers to stats
	int m_StatMods[NUM_STATS];					// perminant modifiers to stats
	int m_StatTemps[NUM_STATS];					// these go down (or up) by 30% each week until they reach 0

	int m_Enjoyment[NUM_ACTIONTYPES];			// these values determine how much a girl likes an action
	int m_EnjoymentTR[NUM_ACTIONTYPES];			// `J` added for traits to affect enjoyment
	int m_EnjoymentMods[NUM_ACTIONTYPES];		// `J` added perminant modifiers to stats
	int m_EnjoymentTemps[NUM_ACTIONTYPES];		// `J` added these go down (or up) by 30% each week until they reach 0
	// (-100 is hate, +100 is loves)
	int m_Virgin;								// is she a virgin, 0=false, 1=true, -1=not checked

	bool m_UseAntiPreg;							// if true she will use anit preg measures

	unsigned char m_Withdrawals;				// if she is addicted to something this counts how many weeks she has been off

	int m_Money;

	int m_AccLevel;					// how good her Accommodation is, 0 is slave like and non-slaves will really hate it

	int m_Skills[NUM_SKILLS];
	int m_SkillTr[NUM_SKILLS];
	int m_SkillMods[NUM_SKILLS];
	int m_SkillTemps[NUM_SKILLS];				// these go down (or up) by 1 each week until they reach 0

	int m_Training[NUM_TRAININGTYPES];			// these values determine how far a girl is into her training CRAZY
	int m_TrainingTR[NUM_TRAININGTYPES];		// 
	int m_TrainingMods[NUM_TRAININGTYPES];		// 
	int m_TrainingTemps[NUM_TRAININGTYPES];		// 
	// (starts at 0, 100 if fully trained)

	int m_RunAway;					// if 0 then off, if 1 then girl is removed from list,
	// otherwise will count down each week
	unsigned char m_Spotted;					// if 1 then she has been seen stealing but not punished yet

	unsigned long m_WeeksPast;					// number of weeks in your service
	unsigned int m_BDay;						// number of weeks in your service since last aging

	int BirthMonth;
	int BirthDay;


	unsigned long m_NumCusts;					// number of customers this girl has slept with
	unsigned long m_NumCusts_old;				// number of customers this girl has slept with before this week

	bool m_Tort;								// if true then have already tortured today
	bool m_JustGaveBirth;						// did she give birth this current week?

	int m_Pay;									// used to keep track of pay this turn
	int m_Tips;									// used to keep track of tips this turn

	long m_FetishTypes;							// the types of fetishes this girl has

	char m_Flags[NUM_GIRLFLAGS];				// flags used by scripts

	cEvents m_Events;							// Each girl keeps track of all her events that happened to her in the last turn


	cTriggerList m_Triggers;					// triggers for the girl

	unsigned char m_DaysUnhappy;				// used to track how many days they are really unhappy for

	sGirl* m_Next;
	sGirl* m_Prev;

	int m_WeeksPreg;							// number of weeks pregnant or inseminated
	int m_PregCooldown;							// number of weeks until can get pregnant again
	cChildList m_Children;
	int m_ChildrenCount[CHILD_COUNT_TYPES];

	vector<string> m_Canonical_Daughters;

	bool m_InStudio;
	bool m_InArena;
	bool m_InCentre;
	bool m_InClinic;
	bool m_InFarm;
	bool m_InHouse;
	int where_is_she;
	int m_PrevWorkingDay;						// `J` save the last count of the number of working days
	int m_WorkingDay;							// count the number of working day
	int m_SpecialJobGoal;						// `J` Special Jobs like surgeries will have a specific goal
	bool m_Refused_To_Work_Day;					// `J` to track better if she refused to work her assigned job
	bool m_Refused_To_Work_Night;				// `J` to track better if she refused to work her assigned job


	void dump(ostream &os);

	/*
	*	MOD: docclox. attach the skill and stat names to the
	*	class that uses them. Plus an XML load method and
	*	an ostream << operator to pretty print the struct for
	*	debug purposes.
	*
	*	Sun Nov 15 05:58:55 GMT 2009
	*/
	static const char	*stat_names[];
	static const char	*skill_names[];
	static const char	*status_names[];
	static const char	*enjoy_names[];
	static const char	*enjoy_jobs[];
	static const char	*training_names[];
	static const char	*training_jobs[];
	static const char	*children_type_names[];	// `J` added
	/*
	*	again, might as well make them part of the struct that uses them
	*/
	static const unsigned int	max_stats;
	static const unsigned int	max_skills;
	static const unsigned int	max_statuses;
	static const unsigned int	max_enjoy;
	static const unsigned int	max_jobs;
	static const unsigned int	max_training;
	/*
	*	we need to be able to go the other way, too:
	*	from string to number. The maps map stat/skill names
	*	onto index numbers. The setup flag is so we can initialise
	* 	the maps the first time an sGirl is constructed
	*/
	static bool		m_maps_setup;
	static map<string, unsigned int>	stat_lookup;
	static map<string, unsigned int>	skill_lookup;
	static map<string, unsigned int>	status_lookup;
	static map<string, unsigned int>	enjoy_lookup;
	static map<string, unsigned int>	jobs_lookup;
	static map<string, unsigned int>	training_lookup;
	static void		setup_maps();

	static int lookup_stat_code(string s);
	static int lookup_skill_code(string s);
	static int lookup_status_code(string s);
	static int lookup_enjoy_code(string s);
	static int lookup_jobs_code(string s);
	static int lookup_training_code(string s);
	string lookup_where_she_is();
	/*
	*	Strictly speaking, methods don't belong in structs.
	*	I've always thought that more of a guideline than a hard and fast rule
	*/
	void load_from_xml(TiXmlElement* el);	// uses sGirl::load_from_xml
	TiXmlElement* SaveGirlXML(TiXmlElement* pRoot);
	bool LoadGirlXML(TiXmlHandle hGirl);

	/*
	*	stream operator - used for debug
	*/
	friend ostream& operator<<(ostream& os, sGirl &g);
	/*
	*	it's a bit daft that we have to go through the global g_Girls
	*	every time we want a stat.
	*
	*	I mean the sGirl type is the one we're primarily concerned with.
	*	that ought to be the base for the query.
	*
	*	Of course, I could just index into the stat array,
	*	but I'm not sure what else the cGirls method does.
	*	So this is safer, if a bit inefficient.
	*/
	int get_stat(int stat_id)
	{
        // returns the total of stat + statmod + tempstat + stattr

        if (stat_id < 0) return 0;
        u_int stat = stat_id;
        int value = 0, min = 0, max = 100;
        /* */if (stat == STAT_AGE) min = 18;	// `J` Legal Note: 18 is the Legal Age of Majority for the USA where I live
        else if (stat == STAT_EXP) max = 32000;
        else if (stat == STAT_LEVEL) max = 255;
        else if (stat == STAT_HEALTH	&& has_trait( "Incorporeal"))	return 100;
        else if (stat == STAT_TIREDNESS &&
                 (has_trait( "Incorporeal") ||
                  has_trait( "Skeleton") ||
                  has_trait( "Zombie")))
            return 0;
        else if (stat == STAT_PCLOVE || stat == STAT_PCFEAR || stat == STAT_PCHATE || stat == STAT_MORALITY ||
                 stat == STAT_REFINEMENT || stat == STAT_DIGNITY || stat == STAT_LACTATION) min = -100;
        // Generic calculation
        value = m_Stats[stat] + m_StatMods[stat] + m_StatTemps[stat] + m_StatTr[stat];

        if (value < min) value = min;
        else if (value > max) value = max;
        return value;
	}

	int upd_temp_stat(int stat_id, int amount, bool usetraits=false)
	{
        if (usetraits)
        {
            if (stat_id == STAT_LIBIDO)
            {
                if (has_trait("Nymphomaniac"))	{ amount = int((double)amount * (amount > 0 ? 1.5 : 0.5));	if (amount == 0)	amount = 1; }
                else if (has_trait("Chaste"))		{ amount = int((double)amount * (amount > 0 ? 0.5 : 1.5));	if (amount == 0)	amount = -1; }
            }
        }
        // TODO Does it really make sense to silently change some stats non temporarily in upd_temp_stat?
        if (stat_id == STAT_HEALTH || stat_id == STAT_HAPPINESS || stat_id == STAT_TIREDNESS || stat_id == STAT_EXP ||
                stat_id == STAT_LEVEL || stat_id == STAT_HOUSE || stat_id == STAT_ASKPRICE)
        {
            upd_stat(stat_id, amount);
            return get_stat(stat_id);
        }
        m_StatTemps[stat_id] += amount;

		return get_stat(stat_id);
	}
	int upd_stat(int stat_id, int amount, bool usetraits = true);

	int upd_temp_Enjoyment(int stat_id, int amount)
	{
		m_EnjoymentTemps[stat_id] += amount;
		return get_enjoyment(stat_id);
	}
	int upd_Enjoyment(int stat_id, int amount, bool usetraits = true)
	{
		m_Enjoyment[amount] += amount;
		/* */if (m_Enjoyment[amount] > 100) 	m_Enjoyment[amount] = 100;
		else if (m_Enjoyment[amount] < -100) 	m_Enjoyment[amount] = -100;

		return get_enjoyment(stat_id);
	}

	int upd_temp_Training(int stat_id, int amount)
	{
		m_TrainingTemps[stat_id] += amount;
		return get_training(stat_id);
	}
	int upd_Training(int stat_id, int amount, bool usetraits = true)
	{
		m_Training[stat_id] += amount;
		/* */if (m_Training[stat_id] > 100) 	m_Training[stat_id] = 100;
		else if (m_Training[stat_id] < 0) 		m_Training[stat_id] = 0;
		return get_training(stat_id);
	}


	/*
	*	Now then:
	*/
	// `J` When modifying Stats or Skills, search for "J-Change-Stats-Skills"  :  found in >> cGirls.h
	int charisma()									{ return get_stat(STAT_CHARISMA); }
	int charisma(int n, bool usetraits = true)		{ return upd_stat(STAT_CHARISMA, n, usetraits); }
	int happiness()									{ return get_stat(STAT_HAPPINESS); }
	int happiness(int n, bool usetraits = true)		{ return upd_stat(STAT_HAPPINESS, n, usetraits); }
	int libido()									{ return get_stat(STAT_LIBIDO); }
	int libido(int n, bool usetraits = true)		{ return upd_stat(STAT_LIBIDO, n, usetraits); }
	int constitution()								{ return get_stat(STAT_CONSTITUTION); }
	int constitution(int n, bool usetraits = true)	{ return upd_stat(STAT_CONSTITUTION, n, usetraits); }
	int intelligence()								{ return get_stat(STAT_INTELLIGENCE); }
	int intelligence(int n, bool usetraits = true)	{ return upd_stat(STAT_INTELLIGENCE, n, usetraits); }
	int confidence()								{ return get_stat(STAT_CONFIDENCE); }
	int confidence(int n, bool usetraits = true)	{ return upd_stat(STAT_CONFIDENCE, n, usetraits); }
	int mana()										{ return get_stat(STAT_MANA); }
	int mana(int n, bool usetraits = true)			{ return upd_stat(STAT_MANA, n, usetraits); }
	int agility()									{ return get_stat(STAT_AGILITY); }
	int agility(int n, bool usetraits = true)		{ return upd_stat(STAT_AGILITY, n, usetraits); }
	int strength()									{ return get_stat(STAT_STRENGTH); }
	int strength(int n, bool usetraits = true)		{ return upd_stat(STAT_STRENGTH, n, usetraits); }
	int fame()										{ return get_stat(STAT_FAME); }
	int fame(int n, bool usetraits = true)			{ return upd_stat(STAT_FAME, n, usetraits); }
	int level()										{ return get_stat(STAT_LEVEL); }
	int level(int n, bool usetraits = true)			{ return upd_stat(STAT_LEVEL, n, usetraits); }
	int askprice()									{ return get_stat(STAT_ASKPRICE); }
	int askprice(int n, bool usetraits = true)		{ return upd_stat(STAT_ASKPRICE, n, usetraits); }
	int house()										{ return get_stat(STAT_HOUSE); }				/* It's NOT lupus! */
	int house(int n, bool usetraits = true)			{ return upd_stat(STAT_HOUSE, n, usetraits); }
	int exp()										{ return get_stat(STAT_EXP); }
	int exp(int n, bool usetraits = true)			{ return upd_stat(STAT_EXP, n, usetraits); }
	int age()										{ return get_stat(STAT_AGE); }
	int age(int n, bool usetraits = true)			{ return upd_stat(STAT_AGE, n, usetraits); }
	int obedience()									{ return get_stat(STAT_OBEDIENCE); }
	int obedience(int n, bool usetraits = true)		{ return upd_stat(STAT_OBEDIENCE, n, usetraits); }
	int spirit()									{ return get_stat(STAT_SPIRIT); }
	int spirit(int n, bool usetraits = true)		{ return upd_stat(STAT_SPIRIT, n, usetraits); }
	int beauty()									{ return get_stat(STAT_BEAUTY); }
	int beauty(int n, bool usetraits = true)		{ return upd_stat(STAT_BEAUTY, n, usetraits); }
	int tiredness()									{ return get_stat(STAT_TIREDNESS); }
	int tiredness(int n, bool usetraits = true)		{ return upd_stat(STAT_TIREDNESS, n, usetraits); }
	int health()									{ return get_stat(STAT_HEALTH); }
	int health(int n, bool usetraits = true)		{ return upd_stat(STAT_HEALTH, n, usetraits); }
	int pcfear()									{ return get_stat(STAT_PCFEAR); }
	int pcfear(int n, bool usetraits = true)		{ return upd_stat(STAT_PCFEAR, n, usetraits); }
	int pclove()									{ return get_stat(STAT_PCLOVE); }
	int pclove(int n, bool usetraits = true)		{ return upd_stat(STAT_PCLOVE, n, usetraits); }
	int pchate()									{ return get_stat(STAT_PCHATE); }
	int pchate(int n, bool usetraits = true)		{ return upd_stat(STAT_PCHATE, n, usetraits); }
	int morality()									{ return get_stat(STAT_MORALITY); }
	int morality(int n, bool usetraits = true)		{ return upd_stat(STAT_MORALITY, n, usetraits); }
	int refinement()								{ return get_stat(STAT_REFINEMENT); }
	int refinement(int n, bool usetraits = true)	{ return upd_stat(STAT_REFINEMENT, n, usetraits); }
	int dignity()									{ return get_stat(STAT_DIGNITY); }
	int dignity(int n, bool usetraits = true)		{ return upd_stat(STAT_DIGNITY, n, usetraits); }
	int lactation()									{ return get_stat(STAT_LACTATION); }
	int lactation(int n, bool usetraits = true)		{ return upd_stat(STAT_LACTATION, n, usetraits); }
	int npclove()									{ return get_stat(STAT_NPCLOVE); }
	int npclove(int n, bool usetraits = true)		{ return upd_stat(STAT_NPCLOVE, n, usetraits); }
	int sanity()									{ return get_stat(STAT_SANITY); }
	int sanity(int n, bool usetraits = true)		{ return upd_stat(STAT_SANITY, n, usetraits); }


	int rebel();
	string JobRating(double value, string type = "", string name = "");
	string JobRatingLetter(double value);
	bool FixFreeTimeJobs();
	/*
	*	notice that if we do tweak get_stat to reference the stats array
	*	direct, the above still work.
	*
	*	similarly...
	*/
	int get_skill(int skill_id);

	int upd_temp_skill(int skill_id, int amount)
	{
        m_SkillTemps[skill_id] += amount;
		return get_skill(skill_id);
	}

	int upd_skill(int skill_id, int amount, bool usetraits = true);

	int	anal()										{ return get_skill(SKILL_ANAL); }
	int	anal(int n, bool usetraits = true)			{ return upd_skill(SKILL_ANAL, n, usetraits); }
	int	bdsm()										{ return get_skill(SKILL_BDSM); }
	int	bdsm(int n, bool usetraits = true)			{ return upd_skill(SKILL_BDSM, n, usetraits); }
	int	beastiality()								{ return get_skill(SKILL_BEASTIALITY); }
	int	beastiality(int n, bool usetraits = true)	{ return upd_skill(SKILL_BEASTIALITY, n, usetraits); }
	int	combat()									{ return get_skill(SKILL_COMBAT); }
	int	combat(int n, bool usetraits = true)		{ return upd_skill(SKILL_COMBAT, n, usetraits); }
	int	group()										{ return get_skill(SKILL_GROUP); }
	int	group(int n, bool usetraits = true)			{ return upd_skill(SKILL_GROUP, n, usetraits); }
	int	lesbian()									{ return get_skill(SKILL_LESBIAN); }
	int	lesbian(int n, bool usetraits = true)		{ return upd_skill(SKILL_LESBIAN, n, usetraits); }
	int	magic()										{ return get_skill(SKILL_MAGIC); }
	int	magic(int n, bool usetraits = true)			{ return upd_skill(SKILL_MAGIC, n, usetraits); }
	int	normalsex()									{ return get_skill(SKILL_NORMALSEX); }
	int	normalsex(int n, bool usetraits = true)		{ return upd_skill(SKILL_NORMALSEX, n, usetraits); }
	int oralsex()									{ return get_skill(SKILL_ORALSEX); }
	int oralsex(int n, bool usetraits = true)		{ return upd_skill(SKILL_ORALSEX, n, usetraits); }
	int tittysex()									{ return get_skill(SKILL_TITTYSEX); }
	int tittysex(int n, bool usetraits = true)		{ return upd_skill(SKILL_TITTYSEX, n, usetraits); }
	int handjob()									{ return get_skill(SKILL_HANDJOB); }
	int handjob(int n, bool usetraits = true)		{ return upd_skill(SKILL_HANDJOB, n, usetraits); }
	int footjob()									{ return get_skill(SKILL_FOOTJOB); }
	int footjob(int n, bool usetraits = true)			{ return upd_skill(SKILL_FOOTJOB, n, usetraits); }
	int	service()									{ return get_skill(SKILL_SERVICE); }
	int	service(int n, bool usetraits = true)			{ return upd_skill(SKILL_SERVICE, n, usetraits); }
	int	strip()										{ return get_skill(SKILL_STRIP); }
	int	strip(int n, bool usetraits = true)			{ return upd_skill(SKILL_STRIP, n, usetraits); }
	int	medicine()									{ return get_skill(SKILL_MEDICINE); }
	int	medicine(int n, bool usetraits = true)			{ return upd_skill(SKILL_MEDICINE, n, usetraits); }
	int	performance()								{ return get_skill(SKILL_PERFORMANCE); }
	int	performance(int n, bool usetraits = true)		{ return upd_skill(SKILL_PERFORMANCE, n, usetraits); }
	int	crafting()									{ return get_skill(SKILL_CRAFTING); }
	int	crafting(int n, bool usetraits = true)			{ return upd_skill(SKILL_CRAFTING, n, usetraits); }
	int	herbalism()									{ return get_skill(SKILL_HERBALISM); }
	int	herbalism(int n, bool usetraits = true)		{ return upd_skill(SKILL_HERBALISM, n, usetraits); }
	int	farming()									{ return get_skill(SKILL_FARMING); }
	int	farming(int n, bool usetraits = true)			{ return upd_skill(SKILL_FARMING, n, usetraits); }
	int	brewing()									{ return get_skill(SKILL_BREWING); }
	int	brewing(int n, bool usetraits = true)			{ return upd_skill(SKILL_BREWING, n, usetraits); }
	int	animalhandling()							{ return get_skill(SKILL_ANIMALHANDLING); }
	int	animalhandling(int n, bool usetraits = true)	{ return upd_skill(SKILL_ANIMALHANDLING, n, usetraits); }
	int	cooking()									{ return get_skill(SKILL_COOKING); }
	int	cooking(int n, bool usetraits = true)			{ return upd_skill(SKILL_COOKING, n, usetraits); }

	int get_enjoyment(int actiontype)
	{
		if (actiontype < 0) return 0;
		// Generic calculation
		int value = m_Enjoyment[actiontype] + m_EnjoymentTR[actiontype] +
					m_EnjoymentMods[actiontype] + m_EnjoymentTemps[actiontype];

		if (value < -100) value = -100;
		else if (value > 100) value = 100;
		return value;
	}
	int get_training(int actiontype)
	{
        if (actiontype < 0) return 0;
        // Generic calculation
        int value = m_Training[actiontype] + m_TrainingTR[actiontype] +
                    m_TrainingMods[actiontype] + m_TrainingTemps[actiontype];

        if (value < 0) value = 0;
        else if (value > 100) value = 100;
        return value;
	}

	/*
	*	convenience func. Also easier to read like this
	*/
	bool carrying_monster();
	bool carrying_human();
	bool carrying_players_child();
	bool carrying_customer_child();
	bool is_pregnant();
	bool is_mother();
	bool is_poisoned();
	bool has_weapon();
	void clear_pregnancy();
	void clear_dating();

	int preg_chance(int base_pc, bool good = false, double factor = 1.0);
	bool calc_pregnancy(int chance, int type, const int stats[NUM_STATS], const int skills[NUM_SKILLS]);

	bool calc_pregnancy(int, cPlayer*);
	bool calc_pregnancy(cPlayer* player, bool good = false, double factor = 1.0);
	bool calc_insemination(cPlayer* player, bool good = false, double factor = 1.0);
	bool calc_group_pregnancy(cPlayer* player, bool good = false, double factor = 1.0);

	bool calc_pregnancy(int, sCustomer*);
	bool calc_pregnancy(const sCustomer& cust, bool good = false, double factor = 1.0);
	bool calc_insemination(const sCustomer& cust, bool good = false, double factor = 1.0);
	bool calc_group_pregnancy(const sCustomer& cust, bool good = false, double factor = 1.0);
	/*
	*	let's overload that...
	*	should be able to do the same using sCustomer as well...
	*/
	bool add_trait(string trait, int temptime = 0, bool removeitem = false, bool remember = false);
	bool remove_trait(string trait,  bool addrememberlist = false, bool force = false, bool keepinrememberlist = false);
	bool has_trait(string trait);
	bool check_virginity();
	bool lose_virginity();
	bool regain_virginity();
	int breast_size();
	bool is_dead(bool sendmessage = false);		// `J` replaces a few DeadGirl checks
	bool is_addict(bool onlyhard = false);	// `J` added bool onlyhard to allow only hard drugs to be checked for
	bool has_disease();
	bool is_fighter(bool canbehelped = false);
	sChild* next_child(sChild* child, bool remove = false);
	int preg_type(int image_type);
	sGirl* run_away();

	bool is_slave()			{ return (m_States & (1 << STATUS_SLAVE)) != 0; }
	bool is_free()			{ return !is_slave(); }
	void set_slave()		{ m_States |= (1 << STATUS_SLAVE); }
	bool is_monster()		{ return (m_States & (1 << STATUS_CATACOMBS)) != 0; }
	bool is_human()			{ return !is_monster(); }
	bool is_arena()			{ return (m_States & (1 << STATUS_ARENA)) != 0; }
	bool is_yourdaughter()	{ return (m_States & (1 << STATUS_YOURDAUGHTER)) != 0; }
	bool is_isdaughter()	{ return (m_States & (1 << STATUS_ISDAUGHTER)) != 0; }
	bool is_warrior()		{ return !is_arena(); }
	bool is_resting();
	bool is_havingsex();
	bool was_resting();

	void fight_own_gang(bool &girl_wins);
	void win_vs_own_gang(vector<sGang*> &v, int max_goons, bool &girl_wins);
	void lose_vs_own_gang(vector<sGang*> &v, int max_goons, int girl_stats, int gang_stats, bool &girl_wins);

	void OutputGirlRow(string* Data, const vector<string>& columnNames);
	void OutputGirlDetailString(string& Data, const string& detailName);

	// END MOD

	// more useful functions
	int has_item(const std::string& item);
	int has_item_j(const std::string& item);
	int add_inv(sInventoryItem* item);

	bool disobey_check(int action, sBrothel* brothel = nullptr);
};

class GirlPredicate {
public:
	virtual bool test(sGirl*) { return true; }
};

// Keeps track of all the available (not used by player) girls in the game.
class cGirls
{
public:
	cGirls();
	~cGirls();

	void Free();

	/*
	*	load the templated girls
	*	(if loading a save game doesn't load from the global template,
	*	loads from the save games' template)
	*
	*	LoadGirlsDecider is a wrapper function that decides to load XML or Legacy formats.
	//  `J` Legacy support has been removed
	*	LoadGirlsXML loads the XML files
	*/
	void LoadGirlsDecider(string filename);
	void LoadGirlsXML(string filename);
	/*
	*	SaveGirls doesn't seem to be the inverse of LoadGirls
	*	but rather writes girl data to the save file
	*/
	TiXmlElement* SaveGirlsXML(TiXmlElement* pRoot);	// Saves the girls to a file
	bool LoadGirlsXML(TiXmlHandle hGirls);

	void AddGirl(sGirl* girl);		// adds a girl to the list
	void RemoveGirl(sGirl* girl, bool deleteGirl = false);	// Removes a girl from the list (only used with editor where all girls are available)

	sGirl* GetGirl(int girl);	// gets the girl by count

	void GirlFucks(sGirl* girl, bool Day0Night1, sCustomer* customer, bool group, string& message, u_int& SexType);	// does the logic for fucking
	// MYR: Millions of ways to say, [girl] does [act] to [customer]
	static string GetRandomGroupString();
	static string GetRandomSexString();
	static string GetRandomOralSexString();
	static string GetRandomLesString();
	static string GetRandomBDSMString();
	static string GetRandomBeastString();
	static string GetRandomAnalString();

	// MYR: More functions for attack/defense/agility-style combat.
	bool GirlInjured(sGirl* girl, unsigned int modifier);
	int GetCombatDamage(sGirl* girl, int CombatType);
	int TakeCombatDamage(sGirl* girl, int amt);

	void LevelUp(sGirl* girl);	// advances a girls level
	void LevelUpStats(sGirl* girl); // Functionalized stat increase for LevelUp

	void EndDayGirls(sBrothel* brothel, sGirl* girl);

    void SetStat(sGirl* girl, int stat, int amount);
	// updates a stat temporarily
	void UpdateStatMod(sGirl* girl, int stat, int amount);							// updates a statmod usually from items
	static void UpdateStatTr(sGirl* girl, int stat, int amount);							// updates a statTr from traits

	void SetSkill(sGirl* girl, int skill, int amount);
    // updates a skill temporarily
	void UpdateSkillMod(sGirl* girl, int skill, int amount);	// updates a skillmods usually from items
	static void UpdateSkillTr(sGirl* girl, int skill, int amount);		// updates a skillTr from traits

	// `J` added
	void SetEnjoyment(sGirl* girl, int a_Enjoy, int amount);									// `J` added
	static void SetEnjoymentTR(sGirl* girl, int a_Enjoy, int amount);									// `J` added for traits
	// updates what she enjoys
	static void UpdateEnjoymentTR(sGirl* girl, int whatSheEnjoys, int amount);							// `J` added for traits
	void UpdateEnjoymentMod(sGirl* girl, int whatSheEnjoys, int amount);							// `J` added for traits
    // `J` added for traits

    // `CRAZY` added
	void SetTraining(sGirl* girl, int a_Training, int amount);									// `CRAZY` added
	static void SetTrainingTR(sGirl* girl, int a_Training, int amount);									// `CRAZY` added for traits
	// updates what she enjoys
	static void UpdateTrainingTR(sGirl* girl, int whatSheTrains, int amount);							// `CRAZY` added for traits
	void UpdateTrainingMod(sGirl* girl, int whatSheTrains, int amount);							// `CRAZY` added for traits
	// `CRAZY` added for traits


	double GetAverageOfAllSkills(sGirl* girl);	// `J` added
	double GetAverageOfSexSkills(sGirl* girl);	// `J` added
	double GetAverageOfNSxSkills(sGirl* girl);	// `J` added

	static bool HasRememberedTrait(sGirl* girl, string trait);
	static int HasTempTrait(sGirl* girl, string trait);
	static bool RestoreRememberedTrait(sGirl* girl, string trait);


	static void ApplyTraits(sGirl* girl, TraitSpec* trait = 0);	// applys the stat bonuses for traits to a girl
	static void MutuallyExclusiveTraits(sGirl* girl, bool apply);
	static void MutuallyExclusiveTrait(sGirl* girl, bool apply, TraitSpec* trait, bool rememberflag = false);

	static bool PossiblyGainNewTrait(sGirl* girl, string Trait, int Threshold, int ActionType, string Message, bool Day0Night1, int eventtype = EVENT_GOODNEWS);
	static bool PossiblyLoseExistingTrait(sGirl* girl, string Trait, int Threshold, int ActionType, string Message, bool Day0Night1);

	// `J` When adding new traits, search for "J-Add-New-Traits"  :  found in >> cGirls.h > AdjustTraitGroup

	// `J` adding these to allow single step adjustment of linked traits
	static string AdjustTraitGroupGagReflex(sGirl* girl, int steps, bool showmessage = false, bool Day0Night1 = false);
	static string AdjustTraitGroupBreastSize(sGirl* girl, int steps, bool showmessage = false, bool Day0Night1 = false);
	static string AdjustTraitGroupFertility(sGirl* girl, int steps, bool showmessage = false, bool Day0Night1 = false);

	int GetNumSlaveGirls();
	int GetNumCatacombGirls();
	int GetNumArenaGirls();
	int GetNumYourDaughterGirls();
	int GetNumIsDaughterGirls();
	int GetSlaveGirl(int from);
	int GetRebelValue(sGirl* girl, bool matron);
	int CheckEquipment(sGirl* girl);	// Check what combat equipment the girl has equipped
	void EquipCombat(sGirl* girl);		// girl makes sure best armor and weapons are equipped, ready for combat
	void UnequipCombat(sGirl* girl);	// girl unequips armor and weapons, ready for brothel work or other non-aggressive jobs
	bool RemoveInvByNumber(sGirl* girl, int Pos);

	Uint8 girl_fights_girl(sGirl* a, sGirl* b);

	bool InheritTrait(TraitSpec* trait);

	void AddRandomGirl(sRandomGirl* girl);
	/*
	*	mod - docclox
	*	same deal here: LoadRandomGirl is a wrapper
	*	The "-XML" version is the new one that loads from XML files
	*/
	void LoadRandomGirl(string filename);
	void LoadRandomGirlXML(string filename);
	// end mod

	sGirl* CreateRandomGirl(int age, bool addToGGirls, bool slave = false, bool undead = false, bool Human0Monster1 = false, bool childnaped = false, bool arena = false, bool daughter = false, bool isdaughter = false, string findbyname = "");

	sGirl* GetRandomGirl(bool slave = false, bool catacomb = false, bool arena = false, bool daughter = false, bool isdaughter = false);
	sGirl* GetUniqueYourDaughterGirl(int Human0Monster1 = -1);	// -1 either, 0 human, 1 monster

	bool NameExists(string name);
	bool SurnameExists(string surname);
	string CreateRealName(string first, string middle = "", string last = "");	// if you have first, middle and last
	bool CreateRealName(sGirl* girl);
	void DivideName(sGirl* girl);
	bool BuildName(sGirl* girl);

	string GetHoroscopeName(int month, int day);


	bool CheckInvSpace(sGirl* girl) { if (girl->m_NumInventory == 40)return false; return true; }
	int AddInv(sGirl* girl, sInventoryItem* item);
	bool EquipItem(sGirl* girl, int num, bool force);
	bool CanEquip(sGirl* girl, int num, bool force);
	int GetWorseItem(sGirl* girl, int type, int cost);
	int GetNumItemType(sGirl* girl, int Type, bool splitsubtype = false);
	void SellInvItem(sGirl* girl, int num);
	void UseItems(sGirl* girl);

    static void RemoveRememberedTrait(sGirl* girl, string name);
	static void RemoveAllRememberedTraits(sGirl* girl);					// WD: Cleanup remembered traits on new girl creation
	int GetNumItemEquiped(sGirl* girl, int Type);
	bool IsItemEquipable(sGirl* girl, int num);
	bool IsInvFull(sGirl* girl);

	int GetSkillWorth(sGirl* girl);

	string GetDetailsString(sGirl* girl, bool purchace = false);
	string GetMoreDetailsString(sGirl* girl, bool purchace = false);
	string GetThirdDetailsString(sGirl* girl);
	string GetGirlMood(sGirl* girl);
	string GetSimpleDetails(sGirl* girl, int fontsize = 8);

    void AddRememberedTrait(sGirl* girl, string name);

	void CalculateAskPrice(sGirl* girl, bool vari);

	void AddTiredness(sGirl* girl);

	void SetAntiPreg(sGirl* girl, bool useAntiPreg) { girl->m_UseAntiPreg = useAntiPreg; }

	void CalculateGirlType(sGirl* girl);	// updates a girls fetish type based on her traits and stats
	bool CheckGirlType(sGirl* girl, int type);	// Checks if a girl has this fetish type

	void do_abnormality(sGirl* sprog, int chance);
	void HandleChild(sGirl* girl, sChild* child, string& summary);
	void HandleChild_CheckIncest(sGirl* mum, sGirl* sprog, sChild* child, string& summary);
	bool child_is_grown(sGirl* girl, sChild* child, string& summary, bool PlayerControlled = true);
	bool child_is_due(sGirl* girl, sChild* child, string& summary, bool PlayerControlled = true);
	void HandleChildren(sGirl* girl, string& summary, bool PlayerControlled = true);	// ages children and handles pregnancy
	// checks if a girl gets pregnant
	void CreatePregnancy(sGirl* girl, int numchildren, int type, const int stats[NUM_STATS], const int skills[NUM_SKILLS]);	// create the actual pregnancy

	void UncontrolledPregnancies();	// ages children and handles pregnancy for all girls not controlled by player

	// mod - docclox - func to return random girl N in the chain
	// returns null if n out of range
	sRandomGirl* random_girl_at(int n);
	/*
	*	while I'm on, a few funcs to factor out some common code in DrawImages
	*/
	int draw_with_default(sGirl* girl, int x, int y, int width, int height, int ImgType, bool random, int img);
	int calc_abnormal_pc(sGirl* mom, sGirl* sprog, bool is_players);

	vector<sGirl* >  get_girls(GirlPredicate* pred);

	// end mod

	void updateTemp(sGirl* girl);		// `J` group all the temp updates into one area

	// WD:	Consolidate common code in BrothelUpdate and DungeonUpdate to fn's
	void updateGirlAge(sGirl* girl, bool inc_inService = false);
	void updateTempStats(sGirl* girl);
	void updateTempSkills(sGirl* girl);
	void updateTempTraits(sGirl* girl);
	void updateTempEnjoyment(sGirl* girl);
	void updateTempTraining(sGirl* girl);
	void updateTempTraits(sGirl* girl, string trait, int amount);
	void updateSTD(sGirl* girl);
	void updateHappyTraits(sGirl* girl);
	void updateGirlTurnStats(sGirl* girl);

	bool girl_has_matron(sGirl* girl, int shift = 0);
	bool detect_disease_in_customer(sBrothel* brothel, sGirl* girl, sCustomer* cust, double mod = 0.0);

	string Accommodation(int acc);
	string AccommodationDetails(sGirl* girl, int acc);
	int PreferredAccom(sGirl* girl);
	string catacombs_look_for(int girls, int items, int beast);

	sCustomer* GetBeast();


private:
	unsigned int m_NumGirls;	// number of girls in the class
	sGirl* m_Parent;	// first in the list of girls who are dead, gone or in use
	sGirl* m_Last;	// last in the list of girls who are dead, gone or in use

	int m_NumRandomGirls;
	int m_NumHumanRandomGirls;
	int m_NumNonHumanRandomGirls;

	int m_NumRandomYourDaughterGirls;
	int m_NumHumanRandomYourDaughterGirls;
	int m_NumNonHumanRandomYourDaughterGirls;

	sRandomGirl* m_RandomGirls;
	sRandomGirl* m_LastRandomGirls;

	int test_child_name(string name);

	sGirl* make_girl_child(sGirl* mom, bool playerisdad = false);
	sGirl* find_girl_by_name(string name, int* index_pt = 0);
	sRandomGirl* find_random_girl_by_name(string name, int* index_pt = 0);

};

#endif  /* __CGIRL_H */
