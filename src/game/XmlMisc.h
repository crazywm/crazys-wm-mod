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
#include <tinyxml2.h>
#include <string>

class TraitSpec;
class sInventoryItem;

std::string XMLifyString(const char* XMLName);

tinyxml2::XMLElement&
SaveStatsXML(tinyxml2::XMLElement& elRoot, int * stats, int * statMods = nullptr, int * tempStats = nullptr);
tinyxml2::XMLElement&
SaveSkillsXML(tinyxml2::XMLElement& elRoot, int * skills, int * skillMods = nullptr, int * tempSkills = nullptr);
tinyxml2::XMLElement& SaveActionsXML(tinyxml2::XMLElement& elRoot, int * enjoyments, int * enjoymentsMods = nullptr, int * enjoymentsTemps = nullptr);
tinyxml2::XMLElement& SaveTrainingXML(tinyxml2::XMLElement& pRoot, int * training, int * trainingMods = nullptr, int * trainingTemps = nullptr);

bool LoadStatsXML(const tinyxml2::XMLElement * pStats, int * stats, int * statMods = nullptr, int * tempStats = nullptr);
bool LoadSkillsXML(const tinyxml2::XMLElement * pSkills, int * skills, int * skillMods = nullptr, int * tempSkills = nullptr);
bool LoadActionsXML(const tinyxml2::XMLElement* pActions, int * enjoyments, int * enjoymentsMods, int * enjoymentsTemps);
bool LoadTrainingXML(const tinyxml2::XMLElement* pTrainings, int * training, int * trainingMods, int * trainingTemps);

