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
#include "cTraits.h"
#include "cInventory.h"

std::string XMLifyString(const std::string& XMLName);
std::string XMLifyString(const char* XMLName);
std::string UnXMLifyString(const std::string& XMLName);
std::string UnXMLifyString(const char* XMLName);

TiXmlElement* SaveStatsXML(TiXmlElement* pRoot, int stats[], int statMods[] = 0, int tempStats[] = 0);
TiXmlElement* SaveSkillsXML(TiXmlElement* pRoot, int skills[], int skillMods[] = 0, int tempSkills[] = 0);
TiXmlElement* SaveJobsXML(TiXmlElement* pRoot, int buildingQualities[]);
TiXmlElement* SaveTraitsXML(TiXmlElement* pRoot, std::string TagName, const int numTraits, TraitSpec* traits[], int tempTraits[]);
TiXmlElement* SaveActionsXML(TiXmlElement* pRoot, int enjoyments[], int enjoymentsMods[] = 0, int enjoymentsTemps[] = 0);
TiXmlElement* SaveInventoryXML(TiXmlElement* pRoot, sInventoryItem* items[], const int numItems, unsigned char isEquipped[] = 0, int quantities[] = 0);
TiXmlElement* SaveTrainingXML(TiXmlElement* pRoot, int training[], int trainingMods[] = 0, int trainingTemps[] = 0);

bool LoadStatsXML(TiXmlHandle hStats, int stats[], int statMods[] = 0, int tempStats[] = 0);
bool LoadSkillsXML(TiXmlHandle hSkills, int skills[], int skillMods[] = 0, int tempSkills[] = 0);
bool LoadJobsXML(TiXmlHandle hJobs, int buildingQualities[]);
bool LoadTraitsXML(TiXmlHandle hTraits, unsigned char& numTraits, TraitSpec* traits[], int tempTraits[] = 0);
bool LoadActionsXML(TiXmlHandle hActions, int enjoyments[], int enjoymentsMods[], int enjoymentsTemps[]);
bool LoadInventoryXML(TiXmlHandle hInventory, sInventoryItem* items[], int& numItems, unsigned char isEquipped[] = 0, int quantities[] = 0);
bool LoadTrainingXML(TiXmlHandle hTrainings, int training[], int trainingMods[], int trainingTemps[]);

