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
#include <list>
#include <set>
#include <iostream>
#include <string>

#include "Constants.h"
#include "cEvents.h"
#include "cRandomGirl.h"

extern cRng g_Dice;

using namespace std;

// Prototypes
class   IBuilding;
struct	sInventoryItem;
struct	sBrothel;
class	TiXmlElement;
class   cPlayer;
struct  sCustomer;
struct  sGang;
class   sChild;
class   TiXmlHandle;
class   sGirl;
class   TraitSpec;

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

	static void GirlFucks(sGirl* girl, bool Day0Night1, sCustomer* customer, bool group, string& message, u_int& SexType);	// does the logic for fucking
	// MYR: Millions of ways to say, [girl] does [act] to [customer]
	static string GetRandomGroupString();
	static string GetRandomSexString();
	static string GetRandomOralSexString();
	static string GetRandomLesString();
	static string GetRandomBDSMString();
	static string GetRandomBeastString();
	static string GetRandomAnalString();

	// MYR: More functions for attack/defense/agility-style combat.
	static bool GirlInjured(sGirl* girl, unsigned int modifier);
	static int GetCombatDamage(sGirl* girl, int CombatType);
	static int TakeCombatDamage(sGirl* girl, int amt);

	static void LevelUp(sGirl* girl);	// advances a girls level
	static void LevelUpStats(sGirl* girl); // Functionalized stat increase for LevelUp

	static void EndDayGirls(IBuilding& brothel, sGirl * girl);

    // updates a stat temporarily
	static void UpdateStatMod(sGirl* girl, int stat, int amount);							// updates a statmod usually from items

	static void SetSkill(sGirl* girl, int skill, int amount);
    // updates a skill temporarily
	static void UpdateSkillMod(sGirl* girl, int skill, int amount);	// updates a skillmods usually from items

    // updates what she enjoys
	static void UpdateEnjoymentMod(sGirl* girl, int whatSheEnjoys, int amount);							// `J` added for traits
    // `J` added for traits


	static double GetAverageOfAllSkills(sGirl* girl);	// `J` added
	static double GetAverageOfSexSkills(sGirl* girl);	// `J` added
	static double GetAverageOfNSxSkills(sGirl* girl);	// `J` added

	static bool HasRememberedTrait(sGirl* girl, string trait);
    static bool RestoreRememberedTrait(sGirl* girl, string trait);


	static void ApplyTraits(sGirl* girl, TraitSpec* trait = nullptr);	// applys the stat bonuses for traits to a girl
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
	static int GetRebelValue(sGirl * girl, bool matron, JOBS job=NUM_JOBS);
	static void EquipCombat(sGirl* girl);		// girl makes sure best armor and weapons are equipped, ready for combat
	static void UnequipCombat(sGirl* girl);	// girl unequips armor and weapons, ready for brothel work or other non-aggressive jobs

    static std::uint8_t girl_fights_girl(sGirl* a, sGirl* b);

	static bool InheritTrait(TraitSpec* trait);

    void LoadRandomGirl(string filename);
	sGirl* CreateRandomGirl(int age, bool addToGGirls, bool slave = false, bool undead = false, bool Human0Monster1 = false, bool childnaped = false, bool arena = false, bool daughter = false, bool isdaughter = false, string findbyname = "");

	sGirl* GetRandomGirl(bool slave = false, bool catacomb = false, bool arena = false, bool daughter = false, bool isdaughter = false);
	sGirl* GetUniqueYourDaughterGirl(int Human0Monster1 = -1);	// -1 either, 0 human, 1 monster

    static string CreateRealName(string first, string middle = "", string last = "");	// if you have first, middle and last
	static bool CreateRealName(sGirl* girl);
	static void DivideName(sGirl* girl);
	static bool BuildName(sGirl* girl);

	static string GetHoroscopeName(int month, int day);


	static bool CheckInvSpace(sGirl* girl);

    static int GetWorseItem(sGirl* girl, int type, int cost);
	static int GetNumItemType(sGirl* girl, int Type, bool splitsubtype = false);
	static void SellInvItem(sGirl* girl, int num);
	static void UseItems(sGirl* girl);

    static void RemoveRememberedTrait(sGirl* girl, string name);
	static void RemoveAllRememberedTraits(sGirl* girl);					// WD: Cleanup remembered traits on new girl creation
    static bool IsItemEquipable(sGirl* girl, int num);
	static bool IsInvFull(sGirl* girl);

	static int GetSkillWorth(sGirl* girl);

	static string GetDetailsString(sGirl* girl, bool purchace = false);
	static string GetMoreDetailsString(sGirl* girl, bool purchace = false);
	static string GetThirdDetailsString(sGirl* girl);
	static string GetGirlMood(const sGirl * girl);
	static string GetSimpleDetails(sGirl* girl, int fontsize = 8);

    static void AddRememberedTrait(sGirl* girl, string name);

	static void CalculateAskPrice(sGirl* girl, bool vari);

    void SetAntiPreg(sGirl* girl, bool useAntiPreg);

	static void CalculateGirlType(sGirl* girl);	// updates a girls fetish type based on her traits and stats
	static bool CheckGirlType(sGirl* girl, Fetishs type);	// Checks if a girl has this fetish type

    bool child_is_grown(sGirl* girl, sChild* child, string& summary, bool PlayerControlled = true);
	static bool child_is_due(sGirl* girl, sChild* child, string& summary, bool PlayerControlled = true);
	void HandleChildren(sGirl* girl, string& summary, bool PlayerControlled = true);	// ages children and handles pregnancy
	// checks if a girl gets pregnant
	static void CreatePregnancy(sGirl* girl, int numchildren, int type, const int stats[NUM_STATS], const int skills[NUM_SKILLS]);	// create the actual pregnancy

	void UncontrolledPregnancies();	// ages children and handles pregnancy for all girls not controlled by player

    static int calc_abnormal_pc(sGirl* mom, sGirl* sprog, bool is_players);

	vector<sGirl* >  get_girls(GirlPredicate* pred);

	// end mod

	static void updateTemp(sGirl* girl);		// `J` group all the temp updates into one area

	// WD:	Consolidate common code in BrothelUpdate and DungeonUpdate to fn's
	static void updateGirlAge(sGirl* girl, bool inc_inService = false);
	static void updateTempStats(sGirl* girl);
	static void updateTempSkills(sGirl* girl);
	static void updateTempTraits(sGirl* girl);
	static void updateTempEnjoyment(sGirl* girl);
    static void updateTempTraits(sGirl* girl, string trait, int amount);
	static void updateSTD(sGirl* girl);
	static void updateHappyTraits(sGirl* girl);
	static void updateGirlTurnStats(sGirl* girl);

	static bool girl_has_matron(sGirl* girl, int shift = 0);
	static bool detect_disease_in_customer(IBuilding * brothel, sGirl * girl, sCustomer * Cust, double mod = 0.0);

	static string Accommodation(int acc);
	static int PreferredAccom(sGirl* girl);
	static string catacombs_look_for(int girls, int items, int beast);

	static sCustomer* GetBeast();


private:
	std::list<sGirl*> m_Girls;	// list of girls who are dead, gone or in use

	int test_child_name(string name);

	sGirl* make_girl_child(sGirl* mom, bool playerisdad = false);
	sGirl* find_girl_by_name(string name, int* index_pt = nullptr);

    static void handle_son(sGirl * mom, string& summary, bool PlayerControlled);
    void handle_daughter(sGirl * mom, const sChild * child, string& summary);

    cRandomGirls m_RandomGirls;
};

#endif  /* __CGIRL_H */
