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

#ifndef __CBROTHEL_H
#define __CBROTHEL_H

#include <string>
#include <vector>
#include <algorithm>

#include "cRival.h"
#include "cGold.h"
#include "cDungeon.h"                // WD:    cBrothel clean up move class to own file
#include "character/cPlayer.h"       // WD:    cBrothel clean up move class to own file
#include "IBuilding.hpp"

// ---------------------------------------------------------------------------------------------------------------------

// defines a single brothel
struct sBrothel : public IBuilding
{
    explicit sBrothel();        // constructor
    virtual ~sBrothel();        // destructor

    unsigned char    m_Bar;                        // level of bar: 0 => none
    unsigned char    m_GamblingHall;                // as above

    // For keeping track of any shows currently being produced here
    int                m_ShowTime;            // when reaches 0 then the show is ready
    int                m_ShowQuality;        // Determined by the average fame and skill of the girls in the show
    unsigned char    m_HasGambStaff;        // gambling hall or
    unsigned char    m_HasBarStaff;        // Bar staff. Not as good as girls but consistent

    void save_additional_xml(tinyxml2::XMLElement& root) const override;
    virtual bool LoadXML(tinyxml2::XMLElement * pBrothel);
    // currently unused for brothel
    void auto_assign_job(sGirl* target, std::stringstream& message, bool is_night) override {};

    void Update() override;
    void UpdateGirls(bool is_night) override;

    bool runaway_check(sGirl * girl);
};

struct BrothelCreationData {
    int    rooms;
    int maxrooms;
    std::string background;
    BuildingType type;
};


class cBuildingManager
{
public:
    IBuilding& AddBuilding(std::unique_ptr<IBuilding> building);
    IBuilding& AddBuilding(const BrothelCreationData& data);

    void LoadXML(const tinyxml2::XMLElement& root);
    tinyxml2::XMLElement * SaveXML(tinyxml2::XMLElement& root) const;

    /// Gets the total number of buildings
    int num_buildings() const;
    int num_buildings(BuildingType type) const;

    /// Gets the index of the given building
    std::size_t find(const IBuilding * target) const;

    /// Gets a building of given type
    IBuilding* building_with_type(BuildingType type, int index = 0);
    IBuilding* random_building_with_type(BuildingType type);

    /// Gets the total number of girls in all buildings.
    int total_girls() const;

    /// Get total number of girls fulfilling a predicate
    template<class F>
    int total_girls_with(F&& predicate) const {
        int count = 0;
        for(const auto& b : m_Buildings) {
            count += b->num_girls_with(std::forward<F>(predicate));
        }
        return count;
    }

    template<class F>
    bool has_any_girl_with(F&& predicate) const {
        for(const auto& b : m_Buildings) {
            if(b->num_girls_with(std::forward<F>(predicate)) > 0) {
                return true;
            }
        }
        return false;
    }

    template<class F>
    std::vector<sGirl*> all_girls_with(F&& predicate) const {
        std::vector<sGirl*> target;
        for(const auto& b : m_Buildings) {
            auto result = b->find_all_girls(predicate);
            target.insert( target.end(), result.begin(), result.end() );
        }
        return std::move(target);
    }

    /// Gets buildings by linear index
    const IBuilding& get_building(int pos) const;
    IBuilding& get_building(int pos);

    // building info functions
    bool has_building(BuildingType type) const;

    /// name checks
    bool NameExists(string name) const;
    bool SurnameExists(string name) const;

    const std::vector<std::unique_ptr<IBuilding>>& buildings() const {
        return m_Buildings;
    }

private:
    std::unique_ptr<IBuilding> create_building(std::string type) const;

    std::vector<std::unique_ptr<IBuilding>> m_Buildings;
};

// global lookup functions
int get_total_player_girls();
int get_total_player_monster_girls();

sGirl* random_girl_on_job(const cBuildingManager& mgr, JOBS job, bool at_night);
sGirl* random_girl_on_job(const IBuilding& building, JOBS job, bool at_night);

#endif  /* __CBROTHEL_H */
