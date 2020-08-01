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
#include <tinyxml2.h>
#include <functional>
#include "character/ICharacter.h"

#include "Constants.h"
#include "cEvents.h"
#include "cRandomGirl.h"
#include "cRng.h"
class cGirlPool;

extern cRng g_Dice;

using namespace std;

// Prototypes
class   IBuilding;
struct  sInventoryItem;
struct  sBrothel;
class   cPlayer;
struct  sCustomer;
struct  sGang;
class   sChild;
class   sGirl;
class   TraitSpec;
class   CGraphics;

// Keeps track of all the available (not used by player) girls in the game.
class cGirls
{
public:
    cGirls();
    ~cGirls();

    void LoadGirlsXML(string filename);
    /*
    *    SaveGirls doesn't seem to be the inverse of LoadGirls
    *    but rather writes girl data to the save file
    */
    tinyxml2::XMLElement& SaveGirlsXML(tinyxml2::XMLElement& elRoot);    // Saves the girls to a file
    bool LoadGirlsXML(const tinyxml2::XMLElement* pGirls);

    void AddGirl(std::shared_ptr<sGirl> girl);               // adds a girl to the list
    std::shared_ptr<sGirl> TakeGirl(const sGirl* girl);      // Removes a girl from the global pool, returns the shared ptr to be put somewhere else
    /// releases a girl back to the rooster: if the girl is unique, it will become available again, if it was procedual it will be
    /// deleted
    void GiveGirl(shared_ptr<sGirl> girl);

    sGirl* GetGirl(int girl);    // gets the girl by count

    static void GirlFucks(sGirl* girl, bool Day0Night1, sCustomer* customer, bool group, string& message, SKILLS &SexType);    // does the logic for fucking

    // MYR: More functions for attack/defense/agility-style combat.
    static bool GirlInjured(sGirl& girl, unsigned int unModifier, std::function<void(std::string)> handler = {});

    static void LevelUp(sGirl& girl);    // advances a girls level
    static void LevelUpStats(sGirl& girl); // Functionalized stat increase for LevelUp

    static void EndDayGirls(IBuilding& brothel, sGirl& girl);

    // updates what she enjoys
    static void UpdateEnjoymentMod(sGirl& girl, int whatSheEnjoys, int amount);                            // `J` added for traits
    // `J` added for traits


    static double GetAverageOfAllSkills(const sGirl& girl);    // `J` added
    static double GetAverageOfSexSkills(const sGirl& girl);    // `J` added
    static double GetAverageOfNSxSkills(const sGirl& girl);    // `J` added

    static bool PossiblyGainNewTrait(sGirl& girl, string Trait, int Threshold, int ActionType, string Message, bool Day0Night1, EventType eventtype = EVENT_GOODNEWS);
    static bool PossiblyLoseExistingTrait(sGirl& girl, string Trait, int Threshold, int ActionType, string Message, bool Day0Night1);

    // `J` adding these to allow single step adjustment of linked traits
    static string AdjustTraitGroupGagReflex(sGirl& girl, int steps, bool showmessage = false);
    static string AdjustTraitGroupBreastSize(sGirl& girl, int adjustment, bool showmessage = false);
    static string AdjustTraitGroupFertility(sGirl& girl, int steps, bool showmessage = false);

    int GetNumYourDaughterGirls();

    static int GetRebelValue(const sGirl& girl, bool matron, JOBS job=NUM_JOBS);
    static void EquipCombat(sGirl& girl);        // girl makes sure best armor and weapons are equipped, ready for combat
    static void UnequipCombat(sGirl& girl);    // girl unequips armor and weapons, ready for brothel work or other non-aggressive jobs

    void LoadRandomGirl(string filename);
    std::shared_ptr<sGirl>
    CreateRandomGirl(int age, bool slave = false, bool undead = false, bool Human0Monster1 = false,
                     bool childnaped = false, bool arena = false, bool daughter = false, bool isdaughter = false,
                     string findbyname = "");

    std::shared_ptr<sGirl> GetRandomGirl(bool slave = false, bool catacomb = false, bool arena = false, bool daughter = false, bool isdaughter = false);
    std::shared_ptr<sGirl> GetUniqueYourDaughterGirl(int Human0Monster1 = -1);    // -1 either, 0 human, 1 monster
    std::shared_ptr<sGirl> GetDaughterByName(const std::string& name, bool slave, bool non_human, bool player_dad);
    std::shared_ptr<sGirl> CreateDaughter(sGirl& mother, bool player_dad);

    static string GetHoroscopeName(int month, int day);

    static const sInventoryItem* GetWorseItem(const sGirl& girl, int type, int cost);
    static int GetNumItemType(const sGirl& girl, int Type, bool splitsubtype = false);
    static void SellInvItem(sGirl& girl, const sInventoryItem* item);
    static void UseItems(sGirl& girl);

    static int GetSkillWorth(const sGirl& girl);
    static string GetDetailsString(sGirl& girl, bool purchase = false);
    static string GetMoreDetailsString(const sGirl& girl, bool purchase = false);
    static string GetThirdDetailsString(const sGirl& girl);
    static string GetGirlMood(const sGirl& girl);
    static string GetSimpleDetails(const sGirl& girl);

    static void UpdateAskPrice(sGirl& girl, bool vari);

    static void CalculateGirlType(sGirl& girl);    // updates a girls fetish type based on her traits and stats
    static bool CheckGirlType(const sGirl& girl, Fetishs type);    // Checks if a girl has this fetish type

    void UncontrolledPregnancies();    // ages children and handles pregnancy for all girls not controlled by player

    // end mod

    static void updateTemp(sGirl& girl);        // `J` group all the temp updates into one area

    // WD:    Consolidate common code in BrothelUpdate and DungeonUpdate to fn's
    static void updateGirlAge(sGirl& girl, bool inc_inService = false);
    static void updateTempEnjoyment(sGirl& girl);
    static void updateSTD(sGirl& girl);
    static void updateHappyTraits(sGirl& girl);
    static void updateGirlTurnStats(sGirl& girl);

    static bool girl_has_matron(const sGirl& girl, int shift = 0);
    static bool detect_disease_in_customer(IBuilding * brothel, sGirl& girl, sCustomer * Cust, double mod = 0.0);

    static string Accommodation(int acc);
    static int PreferredAccom(const sGirl& girl);
    static string catacombs_look_for(int girls, int items, int beast);

    static sCustomer GetBeast();

    static void TakeGold(sGirl& girl);


private:
    std::unique_ptr<cGirlPool> m_Girls;    // list of girls who are dead, gone or in use

    sGirl* find_girl_by_name(const string& name);

    cRandomGirls m_RandomGirls;
};

#endif  /* __CGIRL_H */
