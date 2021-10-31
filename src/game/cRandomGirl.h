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

#include <memory>
#include <string>
#include <vector>
#include <array>
#include <functional>
#include "utils/sPercent.h"
#include "utils/DirPath.h"
#include "Constants.h"

namespace tinyxml2
{
    class XMLElement;
}
class sInventoryItem;
struct sGirl;

// structure to hold randomly generated girl information
struct sRandomGirl
{
    sRandomGirl();
    ~sRandomGirl();

    std::string Name;
    std::string Desc;

    bool Human;                           // 1 means they are human otherwise they are not
    bool Catacomb;                        // 1 means they are a monster found in catacombs, 0 means wanderer
    bool Arena;                           // 1 means they are fighter found in arena
    bool YourDaughter;                    // `J` 1 means they are your daughter
    bool IsDaughter;                      // 1 means they are a set daughter

    std::array<int, NUM_STATS> MinStats;   // min and max stats they may start with
    std::array<int, NUM_STATS> MaxStats;

    std::array<int, NUM_SKILLS> MinSkills; // min and max skills they may start with
    std::array<int, NUM_SKILLS> MaxSkills;

    std::vector<std::string> TraitNames;
    std::vector<int> TraitChance;

    // `J` added starting items for random girls
    struct sItemRecord {
        const sInventoryItem* Item;
        sPercent Chance;
    };
    std::vector<sItemRecord> Inventory;


    int MinMoney;    // min and max money they can start with
    int MaxMoney;
    DirPath ImageDirectory;

    struct sTriggerData {
        std::string Event;
        std::string Script;
        std::string Function;
    };
    std::vector<sTriggerData> Triggers;
};


class cRandomGirls {
public:
    void LoadRandomGirlXML(const std::string& filename, const std::string& base_path,
                           const std::function<void(const std::string&)>& error_handler);
    sRandomGirl* find_random_girl_by_name(const std::string& name);
    sRandomGirl GetRandomGirlSpec(bool Human0Monster1, bool arena, bool daughter, const std::string& findbyname);
    std::shared_ptr<sGirl> CreateRandomGirl(int age, bool slave, bool undead, bool Human0Monster1, bool kidnapped, bool arena,
                                            bool your_daughter, bool is_daughter, const std::string& find_by_name);
private:
    void load_random_girl_imp(const std::string& filename, const std::string& base_path,
                           const std::function<void(const std::string&)>& error_handler);

    std::vector<sRandomGirl> m_RandomGirls;

    int m_NumHumanRandomGirls = 0;
    int m_NumNonHumanRandomGirls = 0;

    int m_NumRandomYourDaughterGirls = 0;
    int m_NumHumanRandomYourDaughterGirls = 0;
    int m_NumNonHumanRandomYourDaughterGirls = 0;
};


#endif //__CRANDOMGIRL_H
