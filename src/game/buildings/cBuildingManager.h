/*
 * Copyright 2009, 2010, The Pink Petal Development Team.
 * The Pink Petal Devloment Team are defined as the game's coders
 * who meet on http://pinkpetal.org
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

#ifndef WM_CBUILDINGMANAGER_H
#define WM_CBUILDINGMANAGER_H

#include <string>
#include "IBuilding.hpp"

struct BrothelCreationData {
    int    rooms;
    int maxrooms;
    std::string background;
    BuildingType type;
};


class cBuildingManager
{
    using girl_pred_fn = std::function<bool(const sGirl&)>;
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
    int total_girls_with(const girl_pred_fn& predicate) const;

    bool has_any_girl_with(const girl_pred_fn& predicate) const;

    template<class F>
    std::vector<const sGirl*> all_girls_with(F&& predicate) const {
        std::vector<const sGirl*> target;
        std::function<void(const sGirl&)> visit = [&](const sGirl& girl) {
            if (predicate(girl)) {
                target.push_back(&girl);
            }
        };
        for(const auto& b : m_Buildings) {
            b->girls().visit(visit);
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
    bool SurnameExists(const string& name) const;

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
sGirl* random_girl_on_job(IBuilding& building, JOBS job, bool at_night);

#endif //WM_CBUILDINGMANAGER_H
