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
	"COMBAT",
	"SEX",
	"WORKESCORT",
	"WORKCLEANING",
	"WORKMATRON",
	"WORKBAR",
	"WORKHALL",
	"WORKSHOW",
	"WORKSECURITY",
	"WORKADVERTISING",
	"WORKTORTURER",
	"WORKCARING",
	"WORKDOCTOR",
	"WORKMOVIE",
	"WORKCUSTSERV",
	"WORKCENTRE",
	"WORKCLUB",
	"WORKHAREM",
	"WORKRECRUIT",
	"WORKNURSE",
	"WORKMECHANIC",
	"WORKCOUNSELOR",
	"WORKMUSIC",
	"WORKSTRIP",
	"WORKMILK",
	"WORKMASSUSSE",
	"WORKFARM",
	"WORKINTERN",
	"WORKREHAB",
	"MAKEPOTIONS",
	"MAKEITEMS",
	"COOKING",
	"GENERAL"
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
	for(int i=0; i<NUM_STATS; i++)
	{
		TiXmlElement* pStat = new TiXmlElement(XMLifyString(sGirl::stat_names[i]));
		pStats->LinkEndChild(pStat);
		pStat->SetAttribute("Value", stats[i]);
		if (statMods && statMods[i])
		{
			pStat->SetAttribute("Mod", statMods[i]);
		}
		if (tempStats && tempStats[i])
		{
			pStat->SetAttribute("Temp", tempStats[i]);
		}
	}
	return pStats;
}

bool LoadStatsXML(TiXmlHandle hStats, int stats[], int statMods[], int tempStats[])
{
	TiXmlElement* pStats = hStats.ToElement();
	if (pStats == 0)
	{
		return false;
	}
	for(int i=0; i<NUM_STATS; i++)
	{
		TiXmlElement* pStat = pStats->FirstChildElement(XMLifyString(sGirl::stat_names[i]));
		if (pStat)
		{
			int tempInt = 0;
			pStat->QueryIntAttribute("Value", &tempInt); stats[i] = tempInt; tempInt = 0;
			if (statMods)
			{
				pStat->QueryIntAttribute("Mod", &(statMods[i]));
			}
			if (tempStats)
			{
				pStat->QueryIntAttribute("Temp", &(tempStats[i]));
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
		if (skillMods && skillMods[i])
		{
			pSkill->SetAttribute("Mod", skillMods[i]);
		}
		if (tempSkills && tempSkills[i])
		{
			pSkill->SetAttribute("Temp", tempSkills[i]);
		}
	}
	return pSkills;
}

bool LoadSkillsXML(TiXmlHandle hSkills, int skills[], int skillMods[], int tempSkills[])
{
	TiXmlElement* pSkills = hSkills.ToElement();
	if (pSkills == 0)
	{
		return false;
	}
	for(int i=0; i<NUM_SKILLS; i++)
	{
		TiXmlElement* pSkill = pSkills->FirstChildElement(XMLifyString(sGirl::skill_names[i]));
		if (pSkill)
		{
			int tempInt = 0;
			pSkill->QueryIntAttribute("Value", &tempInt); skills[i] = tempInt; tempInt = 0;
			if (skillMods)
			{
				pSkill->QueryIntAttribute("Mod", &(skillMods[i]));
			}
			if (tempSkills)
			{
				pSkill->QueryIntAttribute("Temp", &(tempSkills[i]));
			}
		}
	}
	return true;
}

TiXmlElement* SaveJobsXML(TiXmlElement* pRoot, int buildingQualities[])
{
	TiXmlElement* pJobs = new TiXmlElement("Jobs");
	pRoot->LinkEndChild(pJobs);
	for(int i=0; i<NUMJOBTYPES; i++)
	{
		TiXmlElement* pJob = new TiXmlElement(XMLifyString(g_Brothels.m_JobManager.JobName[i]));
		pJobs->LinkEndChild(pJob);
		pJob->SetAttribute("Qual", buildingQualities[i]);
	}
	return pJobs;
}

TiXmlElement* SaveTraitsXML(TiXmlElement* pRoot, std::string TagName, const int numTraits, sTrait* traits[], unsigned char tempTraits[])
{
	TiXmlElement* pTraits = new TiXmlElement(TagName);
	pRoot->LinkEndChild(pTraits);
	for(int i=0; i<numTraits; i++)	// save the trait names
	{
		if(traits[i])
		{
			TiXmlElement* pTrait = new TiXmlElement(XMLifyString(traits[i]->m_Name));  // Trait name
			pTraits->LinkEndChild(pTrait);
			if (tempTraits)
			{
				pTrait->SetAttribute("Temp", tempTraits[i]);  // Is temporary
			}
		}
	}
	return pTraits;
}

bool LoadTraitsXML(TiXmlHandle hTraits, unsigned char& numTraits, sTrait* traits[], unsigned char tempTraits[])
{
	numTraits = 0;
	TiXmlElement* pTraits = hTraits.ToElement();
	if (pTraits == 0)
	{
		return false;
	}

	//this loop does not need UnXMLifyString, which is a bit of a hack currently
	//however, it's coupled more tightly to traits, and seems to do more processing
	sTrait* pTrait = g_Traits.GetTraitNum(0);
	while(pTrait)
	{
		TiXmlElement* pTraitElement = pTraits->FirstChildElement(XMLifyString(pTrait->m_Name));
		if (pTraitElement)
		{
			int tempInt = 0;
			traits[numTraits] = pTrait;
			if (tempTraits)
			{
				pTraitElement->QueryIntAttribute("Temp", &tempInt); tempTraits[numTraits] = tempInt; tempInt = 0;
			}
			++numTraits;
		}
		pTrait = pTrait->m_Next;
	}

#if 0
	//old loop, not sure which way is better
	//also, this loop method has not been tested
	for(TiXmlElement* pTrait = pTraits->FirstChildElement();
		pTrait != 0;
		pTrait = pTrait->NextSiblingElement())
	{
		std::string traitName = pTrait->ValueStr();
		if (traitName.empty() == false)
		{
			int tempInt = 0;
			traits[numTraits] = g_Traits.GetTrait(UnXMLifyString(traitName));
			if (tempTraits)
			{
				pTrait->QueryIntAttribute("Temp", &tempInt); tempTraits[numTraits] = tempInt; tempInt = 0;
			}
			++numTraits;
		}
	}
#endif
	return true;
}

TiXmlElement* SaveActionsXML(TiXmlElement* pRoot, int enjoyments[])
{
	TiXmlElement* pActions = new TiXmlElement("Actions");
	pRoot->LinkEndChild(pActions);
	for(int i=0; i<NUM_ACTIONTYPES; i++)
	{
		TiXmlElement* pAction = new TiXmlElement(XMLifyString(actionTypeNames[i]));
		pActions->LinkEndChild(pAction);
		pAction->SetAttribute("Enjoys", enjoyments[i]);
	}
	return pActions;
}

bool LoadActionsXML(TiXmlHandle hActions, int enjoyments[])
{
	TiXmlElement* pActions = hActions.ToElement();
	if (pActions == 0)
	{
		return false;
	}
	for (int x = 0; x < NUM_ACTIONTYPES; ++x)
	{
		TiXmlElement* pAction = pActions->FirstChildElement(XMLifyString(actionTypeNames[x]));
		if (pAction)
		{
			pAction->QueryIntAttribute("Enjoys", &(enjoyments[x]));
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
			if (isEquipped)
			{
				pItem->SetAttribute("isEquipped", isEquipped[index]);
			}
			if (quantities)
			{
				pItem->SetAttribute("quantity", quantities[index]);
			}
		}
	}
	return pItems;
}

bool LoadInventoryXML(TiXmlHandle hInventory, sInventoryItem* items[], int& numItems, unsigned char isEquipped[], int quantities[])
{
	numItems = 0;
	TiXmlElement* pInventory = hInventory.ToElement();
	if (pInventory == 0)
	{
		return false;
	}

	TiXmlElement* pItems = pInventory->FirstChildElement("Items");
	if (pItems)
	{
		for(TiXmlElement* pItem = pItems->FirstChildElement("Item");
			pItem != 0;
			pItem = pItem->NextSiblingElement("Item"))
		{
			if (pItem->Attribute("Name"))
			{
				sInventoryItem* tempItem = g_InvManager.GetItem(pItem->Attribute("Name"));
				if (tempItem)
				{
					int tempInt = 0;
					items[numItems] = tempItem;
					if (isEquipped)
					{
						isEquipped[numItems] = 0;
						// Equipped or not (0 = no)
						pItem->QueryIntAttribute("isEquipped", &tempInt); isEquipped[numItems] = tempInt; tempInt = 0;
					}
					if (quantities)
					{
						quantities[numItems] = 1;
						pItem->QueryIntAttribute("quantity", &tempInt); quantities[numItems] = tempInt; tempInt = 0;
					}
					++numItems;
				}
			}
		}
	}
	return true;
}

