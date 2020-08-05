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

#ifndef __CRANDOMGIRL_H
#define __CRANDOMGIRL_H

namespace tinyxml2
{
    class XMLElement;
}
class sInventoryItem;
class TraitSpec;

#include <string>
#include <vector>
#include <array>
#include <functional>
#include "utils/sPercent.h"
#include "utils/DirPath.h"
#include "Constants.h"

// structure to hold randomly generated girl information
struct sRandomGirl
{
    sRandomGirl();
    ~sRandomGirl();

    std::string m_Name;
    std::string m_Desc;

    bool m_Human;                           // 1 means they are human otherwise they are not
    bool m_Catacomb;                        // 1 means they are a monster found in catacombs, 0 means wanderer
    bool m_Arena;                           // 1 means they are fighter found in arena
    bool m_YourDaughter;                    // `J` 1 means they are your daughter
    bool m_IsDaughter;                      // 1 means they are a set daughter

    std::array<int, NUM_STATS> m_MinStats;   // min and max stats they may start with
    std::array<int, NUM_STATS> m_MaxStats;

    std::array<int, NUM_SKILLS> m_MinSkills; // min and max skills they may start with
    std::array<int, NUM_SKILLS> m_MaxSkills;

    std::vector<std::string> m_TraitNames;
    std::vector<int> m_TraitChance;

    // `J` added starting items for random girls
    struct sItemRecord {
        const sInventoryItem* Item;
        sPercent Chance;
    };
    std::vector<sItemRecord> m_Inventory;


    int m_MinMoney;    // min and max money they can start with
    int m_MaxMoney;
    DirPath m_ImageDirectory;

    /*
    *    one func to load the girl node,
    *    and then one each for each embedded node
    *
    *    Not so much difficult as tedious.
    */
    void load_from_xml(tinyxml2::XMLElement*);    // uses sRandomGirl::load_from_xml
    void process_trait_xml(tinyxml2::XMLElement*);
    void process_item_xml(tinyxml2::XMLElement*);
    void process_stat_xml(tinyxml2::XMLElement*);
    void process_skill_xml(tinyxml2::XMLElement*);
    void process_cash_xml(tinyxml2::XMLElement*);
};


class cRandomGirls {
public:
    void LoadRandomGirlXML(const std::string& filename, const std::string& base_path,
                           const std::function<void(const std::string&)>& error_handler);
    sRandomGirl* find_random_girl_by_name(const std::string& name);
    sRandomGirl CreateRandomGirl(bool Human0Monster1, bool arena, bool daughter, const std::string& findbyname);
private:
    std::vector<sRandomGirl> m_RandomGirls;

    int m_NumHumanRandomGirls = 0;
    int m_NumNonHumanRandomGirls = 0;

    int m_NumRandomYourDaughterGirls = 0;
    int m_NumHumanRandomYourDaughterGirls = 0;
    int m_NumNonHumanRandomYourDaughterGirls = 0;
};


#endif //__CRANDOMGIRL_H
