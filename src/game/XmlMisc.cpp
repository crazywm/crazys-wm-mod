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
#include "buildings/cBrothel.h"
#include "IGame.h"
#include "cInventory.h"
#include "CLog.h"
#include "xml/util.h"

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


tinyxml2::XMLElement& SaveStatsXML(tinyxml2::XMLElement& elRoot, int * stats, int * statMods, int * tempStats)
{
    auto& stats_el = PushNewElement(elRoot, "Stats");
    for (int i = 0; i < NUM_STATS; i++)
    {
        auto& stat = PushNewElement(stats_el, get_stat_name((STATS)i));
        stat.SetAttribute("Value", stats[i]);
        if (statMods && statMods[i])    stat.SetAttribute("Mod", statMods[i]);
        if (tempStats && tempStats[i])    stat.SetAttribute("Temp", tempStats[i]);
    }
    return stats_el;
}

bool LoadStatsXML(const tinyxml2::XMLElement* pStats, int * stats, int * statMods, int * tempStats)
{
    if (pStats == nullptr) return false;

    for (int i = 0; i < NUM_STATS; i++)
    {
        auto* pStat = pStats->FirstChildElement(get_stat_name((STATS)i));
        if (pStat)
        {
            int tempInt = pStat->IntAttribute("Value", 0);

            int min = 0, max = 100;
            switch (i)
            {
            case STAT_AGE:
                if (tempInt > 99)        tempInt = 100;
                else if (tempInt > 80)        tempInt = 80;
                else if (tempInt < 18)        tempInt = 18;    // `J` Legal Note: 18 is the Legal Age of Majority for the USA where I live
                break;
            case STAT_EXP:        max = 32000;    break;
            case STAT_LEVEL:    max = 255;        break;
            case STAT_PCFEAR:        case STAT_PCHATE:    case STAT_PCLOVE:    case STAT_MORALITY:
            case STAT_REFINEMENT:    case STAT_DIGNITY:    case STAT_LACTATION:
                min = -100;        break;
            default:    break;
            }
            if (tempInt < -1000)    tempInt = -1;    // test for null stats and set them to 0
            if (tempInt > max) tempInt = max;
            else if (tempInt < min) tempInt = min;
            stats[i] = tempInt;

            if (statMods)
            {
                statMods[i] = pStat->IntAttribute("Mod", 0);
            }
            if (tempStats)
            {
                tempStats[i] = pStat->IntAttribute("Temp", 0);;
            }
        }
    }
    return true;
}

tinyxml2::XMLElement& SaveSkillsXML(tinyxml2::XMLElement& elRoot, int * skills, int * skillMods, int * tempSkills)
{
    auto& skills_el = PushNewElement(elRoot, "Skills");
    for (int i = 0; i < NUM_SKILLS; i++)
    {
        auto& skill_el = PushNewElement(skills_el, get_skill_name((SKILLS)i));
        skill_el.SetAttribute("Value", skills[i]);
        if (skillMods && skillMods[i])        skill_el.SetAttribute("Mod", skillMods[i]);
        if (tempSkills && tempSkills[i])    skill_el.SetAttribute("Temp", tempSkills[i]);
    }
    return skills_el;
}

bool LoadSkillsXML(const tinyxml2::XMLElement* pSkills, int * skills, int * skillMods, int * tempSkills)
{
    if (pSkills == nullptr) return false;

    for (int i = 0; i < NUM_SKILLS; i++)
    {
        auto* pSkill = pSkills->FirstChildElement(XMLifyString(get_skill_name((SKILLS)i)).c_str());
        if (pSkill)
        {
            int tempInt = pSkill->IntAttribute("Value", 0);
            if (tempInt < 0) tempInt = 0;
            if (tempInt > 100) tempInt = 100;                    // normalize base skill
            skills[i] = tempInt;

            if (skillMods)
            {
                skillMods[i] = pSkill->IntAttribute("Mod", 0);
            }
            if (tempSkills)
            {
                tempSkills[i] = pSkill->IntAttribute("Temp", 0);
            }
        }
        else        // if not found set it all to 0
        { 
            if (skills)            skills[i] = 0;
            if (skillMods)        skillMods[i] = 0;
            if (tempSkills)        tempSkills[i] = 0; 
        }
    }
    return true;
}

tinyxml2::XMLElement& SaveActionsXML(tinyxml2::XMLElement& elRoot, int * enjoyments, int * enjoymentsMods, int * enjoymentsTemps)
{
    auto& actions_el = PushNewElement(elRoot, "Actions");
    for (int i = 0; i < NUM_ACTIONTYPES; i++)
    {
        auto& action_el = PushNewElement(actions_el, get_action_name((Action_Types)i));
        action_el.SetAttribute("Enjoys", enjoyments[i]);
        if (enjoymentsMods && enjoymentsMods[i])    action_el.SetAttribute("Mod", enjoymentsMods[i]);
        if (enjoymentsTemps && enjoymentsTemps[i])    action_el.SetAttribute("Temp", enjoymentsTemps[i]);
    }
    return actions_el;
}

bool LoadActionsXML(const tinyxml2::XMLElement* pActions, int * enjoyments, int * enjoymentsMods, int * enjoymentsTemps)
{
    if (pActions == nullptr) return false;

    for (int x = 0; x < NUM_ACTIONTYPES; ++x)
    {
        auto* pAction = pActions->FirstChildElement(get_action_name((Action_Types)x));
        
        // `J` a fix for the old WORKINTERN changed to WORKTRAINING
        if (x == ACTION_WORKTRAINING && !pAction) pAction = pActions->FirstChildElement("WORKINTERN");

        if (pAction)
        {
            int tempInt = pAction->IntAttribute("Enjoys", 0);
            if (tempInt < -100)    tempInt = -100;
            if (tempInt > 100)    tempInt = 100;
            enjoyments[x] = tempInt;
            enjoymentsMods[x] = pAction->IntAttribute("Mod", 0);
            enjoymentsTemps[x] = pAction->IntAttribute("Temp", 0);
        }
    }
    return true;
}

tinyxml2::XMLElement& SaveTrainingXML(tinyxml2::XMLElement& pRoot, int * training, int * trainingMods, int * trainingTemps)
{
    auto& elTrainings = PushNewElement(pRoot, "Training");
    for (int i = 0; i < NUM_TRAININGTYPES; i++)
    {
        auto& elTraining = PushNewElement(elTrainings, trainingTypeNames[i]);
        elTraining.SetAttribute("Train", training[i]);
        if (trainingMods && trainingMods[i])    elTraining.SetAttribute("Mod", trainingMods[i]);
        if (trainingTemps && trainingTemps[i])    elTraining.SetAttribute("Temp", trainingTemps[i]);
    }
    return elTrainings;
}

bool LoadTrainingXML(const tinyxml2::XMLElement* pTrainings, int * training, int * trainingMods, int * trainingTemps)
{
    if (pTrainings == nullptr)
    {
        for (int x = 0; x < NUM_TRAININGTYPES; ++x)    // `J` added to set missing trainings to 0
        {
            training[x] = 0;
            trainingMods[x] = 0;
            trainingTemps[x] = 0;
        }
        return false;
    }

    for (int x = 0; x < NUM_TRAININGTYPES; ++x)
    {
        auto* pTraining = pTrainings->FirstChildElement(trainingTypeNames[x]);
        
        if (pTraining)
        {
            int tempInt = pTraining->IntAttribute("Train", 0);
            if (tempInt < 0)    tempInt = 0;
            if (tempInt > 100)    tempInt = 100;
            training[x] = tempInt; 

            trainingMods[x] = pTraining->IntAttribute("Mod", 0);
            trainingTemps[x] = pTraining->IntAttribute("Temp", 0);
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

