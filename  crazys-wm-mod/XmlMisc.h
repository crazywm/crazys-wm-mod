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
#include "tinyxml.h"
#include <string>

class TraitSpec;
class sInventoryItem;

std::string XMLifyString(const std::string& XMLName);
std::string XMLifyString(const char* XMLName);

TiXmlElement* SaveStatsXML(TiXmlElement* pRoot, int stats[], int statMods[] = nullptr, int tempStats[] = nullptr);
TiXmlElement* SaveSkillsXML(TiXmlElement* pRoot, int skills[], int skillMods[] = nullptr, int tempSkills[] = nullptr);
TiXmlElement* SaveTraitsXML(TiXmlElement* pRoot, std::string TagName, const int numTraits, TraitSpec* traits[], int tempTraits[]);
TiXmlElement* SaveActionsXML(TiXmlElement* pRoot, int enjoyments[], int enjoymentsMods[] = nullptr, int enjoymentsTemps[] = nullptr);
TiXmlElement* SaveInventoryXML(TiXmlElement* pRoot, sInventoryItem* items[], const int numItems, unsigned char isEquipped[] = nullptr, int quantities[] = nullptr);
TiXmlElement* SaveTrainingXML(TiXmlElement* pRoot, int training[], int trainingMods[] = nullptr, int trainingTemps[] = nullptr);

bool LoadStatsXML(TiXmlHandle hStats, int stats[], int statMods[] = nullptr, int tempStats[] = nullptr);
bool LoadSkillsXML(TiXmlHandle hSkills, int skills[], int skillMods[] = nullptr, int tempSkills[] = nullptr);
bool LoadActionsXML(TiXmlHandle hActions, int enjoyments[], int enjoymentsMods[], int enjoymentsTemps[]);
bool LoadInventoryXML(TiXmlHandle hInventory, sInventoryItem* items[], int& numItems, unsigned char isEquipped[] = nullptr, int quantities[] = nullptr);
bool LoadTrainingXML(TiXmlHandle hTrainings, int training[], int trainingMods[], int trainingTemps[]);

