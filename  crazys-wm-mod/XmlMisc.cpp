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
#include "Constants.h"
#include "XmlMisc.h"
#include "cGirls.h"
#include "cBrothel.h"
extern cBrothelManager g_Brothels;
extern CLog g_LogFile;
extern cTraits g_Traits;
extern cInventory g_InvManager;

const char* actionTypeNames[] =
{
	// `J` When modifying Action types, search for "J-Change-Action-Types"  :  found in > XmlMisc.cpp
	"COMBAT", "SEX", "WORKESCORT", "WORKCLEANING", "WORKMATRON", "WORKBAR", "WORKHALL", "WORKSHOW", "WORKSECURITY",
	"WORKADVERTISING", "WORKTORTURER", "WORKCARING", "WORKDOCTOR", "WORKMOVIE", "WORKCUSTSERV", "WORKCENTRE", "WORKCLUB",
	"WORKHAREM", "WORKRECRUIT", "WORKNURSE", "WORKMECHANIC", "WORKCOUNSELOR", "WORKMUSIC", "WORKSTRIP", "WORKMILK",
	"WORKMASSEUSE", "WORKFARM", "WORKTRAINING", "WORKREHAB", "MAKEPOTIONS", "MAKEITEMS", "COOKING", "GETTHERAPY",
	"WORKHOUSEPET", "GENERAL"
};
const char* trainingTypeNames[] =
{
	// When modifying Training types, search for "Change-Traning-Types"  :  found in > XmlMisc.cpp
	"PUPPY", "PONY", "GENERAL"
};

/*
It's possible to save everything like so:
<Traits>
<Trait Name="Big Boobs" Temp="0"/>
</Traits>
But this is slightly less readable in a huge list, imo, than
<Traits>
<Big_Boobs Temp="0"/>
</Traits>

With that in mind, XML tags names and attribute names cannot have special characters
so <Big Boobs Temp="0"/> does NOT work, it needs to be XMLified

Also note that we CAN have Name="Big Boobs", no need to XMLify value strings
*/
std::string XMLifyString(const char* XMLName)
{
	std::string newName(XMLName);
	//find characters that we can't put into XML names
	//and change them to '_' or whatever works in a name
	for (size_t position = newName.find_first_of(" <>\'\"");
		position != newName.npos;
		position = newName.find_first_of(" <>\'\"", position))
	{
		newName.replace(position, 1, 1, '_');
	}
	return newName;
}

std::string XMLifyString(const std::string& XMLName)
{
	return XMLifyString(XMLName.c_str());
}

/*
the inverse of XMLifyString
it's not an exact inverse, and both functions should be cleaned up
*/
std::string UnXMLifyString(const char* XMLName)
{
	std::string newName(XMLName);
	//find characters that we can't put into XML names
	//and change them to '_' or whatever works in a name
	for (size_t position = newName.find_first_of("_");
		position != newName.npos;
		position = newName.find_first_of("_", position))
	{
		newName.replace(position, 1, 1, ' ');
	}
	return newName;
}

std::string UnXMLifyString(const std::string& XMLName)
{
	return UnXMLifyString(XMLName.c_str());
}





/*
suppose you have an unsigned char called var with value 255, and you save it like this:
SetAttribute("test", var);
This is what you get:    test="255"

suppose you load it like this:
QueryValueAttribute<unsigned char>("test", &var);
when this is called, you'll only load the first character, which is a 2!
the value of var is now 2, though it should have been 255

therefore, you must do the following
int tempInt = 0;
QueryIntAttribute("test", &tempInt);
var = tempInt;
tempInt = 0;

other types that appear to work:
bool
unsigned short
u_int
unsigned int
long
unsigned long
*/


TiXmlElement* SaveStatsXML(TiXmlElement* pRoot, int stats[], int statMods[], int tempStats[])
{
	TiXmlElement* pStats = new TiXmlElement("Stats");
	pRoot->LinkEndChild(pStats);
	for (int i = 0; i < NUM_STATS; i++)
	{
		TiXmlElement* pStat = new TiXmlElement(XMLifyString(sGirl::stat_names[i]));
		pStats->LinkEndChild(pStat);
		pStat->SetAttribute("Value", stats[i]);
		if (statMods && statMods[i])	pStat->SetAttribute("Mod", statMods[i]);
		if (tempStats && tempStats[i])	pStat->SetAttribute("Temp", tempStats[i]);
	}
	return pStats;
}

bool LoadStatsXML(TiXmlHandle hStats, int stats[], int statMods[], int tempStats[])
{
	TiXmlElement* pStats = hStats.ToElement();
	if (pStats == 0) return false;

	for (int i = 0; i < NUM_STATS; i++)
	{
		TiXmlElement* pStat = pStats->FirstChildElement(XMLifyString(sGirl::stat_names[i]));
		if (pStat)
		{
			int tempInt = 0;
			if (pStat->Attribute("Value"))	pStat->QueryIntAttribute("Value", &tempInt);

			int min = 0, max = 100;
			switch (i)
			{
			case STAT_AGE:
				if (tempInt > 99)		tempInt = 100;
				else if (tempInt > 80)		tempInt = 80;
				else if (tempInt < 18)		tempInt = 18;	// `J` Legal Note: 18 is the Legal Age of Majority for the USA where I live 
				break;
			case STAT_EXP:		max = 32000;	break;
			case STAT_LEVEL:	max = 255;		break;
			case STAT_PCFEAR:		case STAT_PCHATE:	case STAT_PCLOVE:	case STAT_MORALITY:
			case STAT_REFINEMENT:	case STAT_DIGNITY:	case STAT_LACTATION:
				min = -100;		break;
			default:	break;
			}
			if (tempInt < -1000)	tempInt = -1;	// test for null stats and set them to 0
			if (tempInt > max) tempInt = max;
			else if (tempInt < min) tempInt = min;
			stats[i] = tempInt;

			if (statMods)
			{
				tempInt = 0;
				if (pStat->Attribute("Mod"))	pStat->QueryIntAttribute("Mod", &tempInt);
				statMods[i] = tempInt;
			}
			if (tempStats)
			{
				tempInt = 0;
				if (pStat->Attribute("Temp"))	pStat->QueryIntAttribute("Temp", &tempInt);
				tempStats[i] = tempInt;
			}
		}
	}
	return true;
}

TiXmlElement* SaveSkillsXML(TiXmlElement* pRoot, int skills[], int skillMods[], int tempSkills[])
{
	TiXmlElement* pSkills = new TiXmlElement("Skills");
	pRoot->LinkEndChild(pSkills);
	for (int i = 0; i < NUM_SKILLS; i++)
	{
		TiXmlElement* pSkill = new TiXmlElement(XMLifyString(sGirl::skill_names[i]));
		pSkills->LinkEndChild(pSkill);
		pSkill->SetAttribute("Value", skills[i]);
		if (skillMods && skillMods[i])		pSkill->SetAttribute("Mod", skillMods[i]);
		if (tempSkills && tempSkills[i])	pSkill->SetAttribute("Temp", tempSkills[i]);
	}
	return pSkills;
}

bool LoadSkillsXML(TiXmlHandle hSkills, int skills[], int skillMods[], int tempSkills[])
{
	TiXmlElement* pSkills = hSkills.ToElement();
	if (pSkills == 0) return false;

	for (int i = 0; i < NUM_SKILLS; i++)
	{
		TiXmlElement* pSkill = pSkills->FirstChildElement(XMLifyString(sGirl::skill_names[i]));
		if (pSkill)
		{
			int tempInt = 0;
			if (pSkill->Attribute("Value"))	pSkill->QueryIntAttribute("Value", &tempInt);
			if (tempInt < 0) tempInt = 0; if (tempInt > 100) tempInt = 100;					// normalize base skill
			skills[i] = tempInt;

			if (skillMods)
			{
				tempInt = 0;
				if (pSkill->Attribute("Mod"))	pSkill->QueryIntAttribute("Mod", &tempInt);
				skillMods[i] = tempInt;
			}
			if (tempSkills)
			{
				tempInt = 0;
				if (pSkill->Attribute("Temp"))	pSkill->QueryIntAttribute("Temp", &tempInt);
				tempSkills[i] = tempInt;
			}
		}
		else		// if not found set it all to 0
		{ 
			if (skills)			skills[i] = 0;
			if (skillMods)		skillMods[i] = 0;
			if (tempSkills)		tempSkills[i] = 0; 
		}
	}
	return true;
}

TiXmlElement* SaveJobsXML(TiXmlElement* pRoot, int buildingQualities[])
{
	TiXmlElement* pJobs = new TiXmlElement("Jobs");
	pRoot->LinkEndChild(pJobs);
	for (int i = 0; i < NUMJOBTYPES; i++)
	{
		TiXmlElement* pJob = new TiXmlElement(XMLifyString(g_Brothels.m_JobManager.JobName[i]));
		pJobs->LinkEndChild(pJob);
		pJob->SetAttribute("Qual", buildingQualities[i]);
	}
	return pJobs;
}

TiXmlElement* SaveTraitsXML(TiXmlElement* pRoot, std::string TagName, const int numTraits, TraitSpec* traits[], int tempTraits[])
{
	TiXmlElement* pTraits = new TiXmlElement(TagName);
	pRoot->LinkEndChild(pTraits);
	for (int i = 0; i < numTraits; i++)	// save the trait names
	{
		if (traits[i])
		{
			TiXmlElement* pTrait = new TiXmlElement(XMLifyString(traits[i]->name()));  // Trait name
			pTraits->LinkEndChild(pTrait);
			if (tempTraits)	pTrait->SetAttribute("Temp", tempTraits[i]);  // Is temporary
		}
	}
	return pTraits;
}

bool LoadTraitsXML(TiXmlHandle hTraits, unsigned char& numTraits, TraitSpec* traits[], int tempTraits[])
{
	numTraits = 0;
	TiXmlElement* pTraits = hTraits.ToElement();
	if (pTraits == 0) return false;

	//this loop does not need UnXMLifyString, which is a bit of a hack currently
	//however, it's coupled more tightly to traits, and seems to do more processing
	for(const auto& pTrait : g_Traits.all_traits())
	{
		TiXmlElement* pTraitElement = pTraits->FirstChildElement(XMLifyString(pTrait->name()));
		if (pTraitElement)
		{
			int tempInt = 0;
			traits[numTraits] = pTrait.get();
			if (tempTraits)
			{
				pTraitElement->QueryIntAttribute("Temp", &tempInt); tempTraits[numTraits] = tempInt; tempInt = 0;
			}
			++numTraits;
		}
	}
	return true;
}

TiXmlElement* SaveActionsXML(TiXmlElement* pRoot, int enjoyments[], int enjoymentsMods[], int enjoymentsTemps[])
{
	TiXmlElement* pActions = new TiXmlElement("Actions");
	pRoot->LinkEndChild(pActions);
	for (int i = 0; i < NUM_ACTIONTYPES; i++)
	{
		TiXmlElement* pAction = new TiXmlElement(XMLifyString(actionTypeNames[i]));
		pActions->LinkEndChild(pAction);
		pAction->SetAttribute("Enjoys", enjoyments[i]);
		if (enjoymentsMods && enjoymentsMods[i])	pAction->SetAttribute("Mod", enjoymentsMods[i]);
		if (enjoymentsTemps && enjoymentsTemps[i])	pAction->SetAttribute("Temp", enjoymentsTemps[i]);
	}
	return pActions;
}

bool LoadActionsXML(TiXmlHandle hActions, int enjoyments[], int enjoymentsMods[], int enjoymentsTemps[])
{
	TiXmlElement* pActions = hActions.ToElement();
	if (pActions == 0) return false;

	for (int x = 0; x < NUM_ACTIONTYPES; ++x)
	{
		TiXmlElement* pAction = pActions->FirstChildElement(XMLifyString(actionTypeNames[x]));
		
		// `J` a fix for the old WORKINTERN changed to WORKTRAINING
		if (x == ACTION_WORKTRAINING && !pAction) pAction = pActions->FirstChildElement(XMLifyString("WORKINTERN"));

		if (pAction)
		{
			int tempInt = 0;
			if (pAction->Attribute("Enjoys"))	pAction->QueryIntAttribute("Enjoys", &tempInt);
			if (tempInt < -100)	tempInt = -100; if (tempInt > 100)	tempInt = 100;
			enjoyments[x] = tempInt; 
			
			tempInt = 0;
			if (pAction->Attribute("Mod"))	pAction->QueryIntAttribute("Mod", &tempInt);
			enjoymentsMods[x] = tempInt;

			tempInt = 0;
			if (pAction->Attribute("Temp"))	pAction->QueryIntAttribute("Temp", &tempInt);
			enjoymentsTemps[x] = tempInt;
		}
	}
	return true;
}

TiXmlElement* SaveTrainingXML(TiXmlElement* pRoot, int training[], int trainingMods[], int trainingTemps[])
{
	TiXmlElement* pTrainings = new TiXmlElement("Training");
	pRoot->LinkEndChild(pTrainings);
	for (int i = 0; i < NUM_TRAININGTYPES; i++)
	{
		TiXmlElement* pTraining = new TiXmlElement(XMLifyString(trainingTypeNames[i]));
		pTrainings->LinkEndChild(pTraining);
		pTraining->SetAttribute("Train", training[i]);
		if (trainingMods && trainingMods[i])	pTraining->SetAttribute("Mod", trainingMods[i]);
		if (trainingTemps && trainingTemps[i])	pTraining->SetAttribute("Temp", trainingTemps[i]);
	}
	return pTrainings;
}

bool LoadTrainingXML(TiXmlHandle hTrainings, int training[], int trainingMods[], int trainingTemps[])
{
	TiXmlElement* pTrainings = hTrainings.ToElement();
	if (pTrainings == 0)
	{
		for (int x = 0; x < NUM_TRAININGTYPES; ++x)	// `J` added to set missing trainings to 0
		{
			training[x] = 0;
			trainingMods[x] = 0;
			trainingTemps[x] = 0;
		}
		return false;
	}

	for (int x = 0; x < NUM_TRAININGTYPES; ++x)
	{
		TiXmlElement* pTraining = pTrainings->FirstChildElement(XMLifyString(trainingTypeNames[x]));
		
		if (pTraining)
		{
			int tempInt = 0;
			if (pTraining->Attribute("Train"))	pTraining->QueryIntAttribute("Train", &tempInt);
			if (tempInt < 0)	tempInt = 0; if (tempInt > 100)	tempInt = 100;
			training[x] = tempInt; 
			
			tempInt = 0;
			if (pTraining->Attribute("Mod"))	pTraining->QueryIntAttribute("Mod", &tempInt);
			trainingMods[x] = tempInt;

			tempInt = 0;
			if (pTraining->Attribute("Temp"))	pTraining->QueryIntAttribute("Temp", &tempInt);
			trainingTemps[x] = tempInt;
		}
		else
		{
			training[x] = 0;
			trainingMods[x] = 0;
			trainingTemps[x] = 0;
		}
	}
	return true;
}

TiXmlElement* SaveInventoryXML(TiXmlElement* pRoot, sInventoryItem* items[], const int numItems, unsigned char isEquipped[], int quantities[])
{
	TiXmlElement* pItems = new TiXmlElement("Items");
	pRoot->LinkEndChild(pItems);
	for (int index = 0; index < numItems; ++index)
	{
		if (items[index])
		{
			TiXmlElement* pItem = new TiXmlElement("Item");
			pItems->LinkEndChild(pItem);
			pItem->SetAttribute("Name", items[index]->m_Name);
			if (isEquipped)		pItem->SetAttribute("isEquipped", isEquipped[index]);
			if (quantities)		pItem->SetAttribute("quantity", quantities[index]);
		}
	}
	return pItems;
}

bool LoadInventoryXML(TiXmlHandle hInventory, sInventoryItem* items[], int& numItems, unsigned char isEquipped[], int quantities[])
{
	numItems = 0;
	TiXmlElement* pInventory = hInventory.ToElement();
	if (pInventory == 0)	return false;

	TiXmlElement* pItems = pInventory->FirstChildElement("Items");
	if (pItems)
	{
		for (TiXmlElement* pItem = pItems->FirstChildElement("Item"); pItem != 0; pItem = pItem->NextSiblingElement("Item"))
		{
			if (pItem->Attribute("Name"))
			{
				sInventoryItem* tempItem = g_InvManager.GetItem(pItem->Attribute("Name"));
				if (tempItem)
				{
					items[numItems] = tempItem;

					if (isEquipped)
					{
						int tempInt = 0;
						if (pItem->Attribute("isEquipped"))	pItem->QueryIntAttribute("isEquipped", &tempInt);
						isEquipped[numItems] = tempInt;
					}

					if (quantities)
					{
						int tempInt = 1;
						if (pItem->Attribute("quantity"))	pItem->QueryIntAttribute("quantity", &tempInt);
						quantities[numItems] = tempInt;
					}
					++numItems;
				}
			}
		}
	}
	return true;
}

