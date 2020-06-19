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

#ifndef CRAZYS_WM_MOD_COBJECTIVEMANAGER_HPP
#define CRAZYS_WM_MOD_COBJECTIVEMANAGER_HPP

#include <string>
#include <memory>

namespace tinyxml2
{
    class XMLElement;
}

// holds an objective and its data
struct sObjective
{
    unsigned m_Objective;    // the objective type
    int m_Reward;        // the reward type
    int m_Target;        // the x variable for the objective
    int m_SoFar;        // how much of the x variable has been achieved
    int m_Limit;        // the number of weeks must be done by
    int m_Difficulty;    // a number representing how hard it is
    std::string m_Text;        // save the text for pass objective report.
    std::string m_FailText;    // save the text for fail objective report.
    std::string m_PassText;    // save the text for fail objective report.

};

class cObjectiveManager {
public:
    void UpdateObjective();                // updates an objective and checks for compleation
    sObjective* GetObjective();            // returns the objective
    void CreateNewObjective();            // Creates a new objective
    void PassObjective();                // Gives a reward

    void LoadFromXML(const tinyxml2::XMLElement& root);
    void SaveToXML(tinyxml2::XMLElement& root) const;
private:
    std::unique_ptr<sObjective> m_Objective;
};


#endif //CRAZYS_WM_MOD_COBJECTIVEMANAGER_HPP
