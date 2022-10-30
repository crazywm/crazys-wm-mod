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

    std::string Name;
    std::string Desc;

    std::array<int, NUM_SPAWN> SpawnWeights;

    std::array<int, NUM_STATS> MinStats;   // min and max stats they may start with
    std::array<int, NUM_STATS> MaxStats;

    std::array<int, NUM_SKILLS> MinSkills; // min and max skills they may start with
    std::array<int, NUM_SKILLS> MaxSkills;

    struct sTraitSpawnInfo {
        std::string Name;
        float SpawnChance;          //!< Overall chance that this trait will be spawned
        float InherentChance;       //!< Chance that this trait will be spawned as an inherent trait, if it is spawned.
        float DormantChance;        //!< Chance that this trait will be spawned as a dormant trait, if it is inherent.
    };

    std::vector<sTraitSpawnInfo> Traits;

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
    void load_from_file(const std::string& filename, const std::string& base_path,
                        const std::function<void(const std::string&)>& error_handler);

    std::shared_ptr<sGirl> spawn(SpawnReason reason, int age);
    std::shared_ptr<sGirl> spawn(SpawnReason reason, int age, const std::string& name);
private:
    void load_random_girl_imp(const std::string& filename, const std::string& base_path,
                           const std::function<void(const std::string&)>& error_handler);

    std::shared_ptr<sGirl> create_from_template(const sRandomGirl& template_, SpawnReason reason, int age) const;

    const sRandomGirl* get_spec(const std::string& name);
    const sRandomGirl* get_spec(SpawnReason reason, int age = -1);

    std::vector<sRandomGirl> m_RandomGirls;
};


#endif //__CRANDOMGIRL_H
