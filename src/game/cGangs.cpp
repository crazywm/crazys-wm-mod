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
#include <sstream>
#include <algorithm>

#include "cGangs.h"
#include "buildings/IBuilding.h"
#include "character/cCustomers.h"
#include "cGirls.h"
#include "cTariff.h"
#include "XmlMisc.h"
#include "cObjectiveManager.hpp"
#include "cInventory.h"
#include "IGame.h"
#include "sStorage.h"
#include "gang_missions.h"
#include "cRival.h"

#include "utils/DirPath.h"
#include "utils/streaming_random_selection.hpp"
#include "utils/algorithms.hpp"
#include "utils/string.hpp"
#include "CLog.h"
#include "xml/util.h"
#include "buildings/cBuildingManager.h"

extern cRng g_Dice;

namespace settings {
    extern const char* GANG_MAX_MEMBERS;
}

tinyxml2::XMLElement& sGang::SaveGangXML(tinyxml2::XMLElement& elRoot)
{
    auto& elGang = PushNewElement(elRoot, "Gang");
    elGang.SetAttribute("Num", m_Num);
    SaveSkillsXML(elGang, m_Skills);
    SaveStatsXML(elGang, m_Stats);
    elGang.SetAttribute("Name", m_Name.c_str());
    elGang.SetAttribute("Nets", m_Nets);
    elGang.SetAttribute("WeaponLevel", m_WpnLevel);

    elGang.SetAttribute("MissionID", m_MissionID);
    elGang.SetAttribute("LastMissID", m_LastMissID);
    elGang.SetAttribute("Combat", m_Combat);
    elGang.SetAttribute("AutoRecruit", m_AutoRecruit);
    return elGang;
}

bool sGang::LoadGangXML(const tinyxml2::XMLElement& element)
{
    if (element.Attribute("Name")) m_Name = element.Attribute("Name");
    element.QueryIntAttribute("Num", &m_Num);
    LoadSkillsXML(element.FirstChildElement("Skills"), m_Skills);
    LoadStatsXML(element.FirstChildElement("Stats"), m_Stats);
    if (m_Skills[SKILL_MAGIC] <= 0 || m_Skills[SKILL_COMBAT] <= 0 || m_Stats[STAT_INTELLIGENCE] <= 0 || m_Stats[STAT_AGILITY] <= 0 ||
        m_Stats[STAT_CONSTITUTION] <= 0 || m_Stats[STAT_CHARISMA] <= 0 || m_Stats[STAT_STRENGTH] <= 0 || m_Skills[SKILL_SERVICE] <= 0)
    {
        int total =
                std::max(0, m_Skills[SKILL_MAGIC]) +
                std::max(0, m_Skills[SKILL_COMBAT]) +
                std::max(0, m_Stats[STAT_INTELLIGENCE]) +
                std::max(0, m_Stats[STAT_AGILITY]) +
                std::max(0, m_Stats[STAT_CONSTITUTION]) +
                std::max(0, m_Stats[STAT_CHARISMA]) +
                std::max(0, m_Stats[STAT_STRENGTH]);
        int low = total / 8;
        int high = total / 6;
        if (m_Skills[SKILL_MAGIC] <= 0)                m_Skills[SKILL_MAGIC] = g_Dice.bell(low, high);
        if (m_Skills[SKILL_COMBAT] <= 0)            m_Skills[SKILL_COMBAT] = g_Dice.bell(low, high);
        if (m_Stats[STAT_INTELLIGENCE] <= 0)        m_Stats[STAT_INTELLIGENCE] = g_Dice.bell(low, high);
        if (m_Stats[STAT_AGILITY] <= 0)                m_Stats[STAT_AGILITY] = g_Dice.bell(low, high);
        if (m_Stats[STAT_CONSTITUTION] <= 0)        m_Stats[STAT_CONSTITUTION] = g_Dice.bell(low, high);
        if (m_Stats[STAT_CHARISMA] <= 0)            m_Stats[STAT_CHARISMA] = g_Dice.bell(low, high);
        if (m_Stats[STAT_STRENGTH] <= 0)            m_Stats[STAT_STRENGTH] = g_Dice.bell(low, high);
        if (m_Skills[SKILL_SERVICE] <= 0)            m_Skills[SKILL_SERVICE] = g_Dice.bell(low / 2, high);    // `J` added for .06.02.41
    }

    //these may not have been saved
    //if not, the query just does not set the value
    //so the default is used, assuming the gang was properly init
    element.QueryAttribute("MissionID", &m_MissionID);
    element.QueryIntAttribute("LastMissID", &m_LastMissID);
    element.QueryAttribute("Combat", &m_Combat);
    element.QueryAttribute("AutoRecruit", &m_AutoRecruit);
    element.QueryIntAttribute("Nets", &m_Nets);
    element.QueryIntAttribute("WeaponLevel", &m_WpnLevel);

    return true;
}

void sGang::AdjustGangSkill(int Skill, int amount)    // `J` added for .06.02.41
{
    this->m_Skills[Skill] += amount;
    if (this->m_Skills[Skill] < 0)        this->m_Skills[Skill] = 0;
    if (this->m_Skills[Skill] > 100)    this->m_Skills[Skill] = 100;
}
void sGang::AdjustGangStat(int Stat, int amount)    // `J` added for .06.02.41
{
    this->m_Stats[Stat] += amount;
    if (this->m_Stats[Stat] < 0)        this->m_Stats[Stat] = 0;
    if (this->m_Stats[Stat] > 100)        this->m_Stats[Stat] = 100;
}

sGang::sGang(std::string name) :
    m_Name(std::move(name))
{
    m_MissionID = MISS_GUARDING;
    m_AutoRecruit = false;
    for (int & m_Skill : m_Skills)    m_Skill = 0;
    for (int & m_Stat : m_Stats)    m_Stat = 0;
    m_Stats[STAT_HEALTH] = 100;
    m_Stats[STAT_HAPPINESS] = 100;

}

void sGang::BoostCombatSkills(int amount)
{
    std::vector<int*> possible_skills;
    possible_skills.push_back(&m_Skills[SKILL_COMBAT]);
    possible_skills.push_back(&m_Skills[SKILL_MAGIC]);
    possible_skills.push_back(&m_Stats[STAT_AGILITY]);
    possible_skills.push_back(&m_Stats[STAT_CONSTITUTION]);
    BoostRandomSkill(possible_skills, amount, 1);
}

int sGang::members() const
{
    return m_Num;
}

void sGang::BoostRandomSkill(const std::vector<int *>& possible_skills, int count, int boost_count)
{
/*
    *    Which of the passed skills/stats will be raised this time?
    *    Hopefully they'll tend to focus a bit more on what they're already good at...
    *    that way, they will have strengths instead of becoming entirely homogenized
    *
    *    ex. 60 combat, 50 magic, and 40 intelligence: squared, that comes to 3600, 2500 and 1600...
    *        so: ~46.75% chance combat, ~32.46% chance magic, ~20.78% chance intelligence
    */
    for (int j = 0; j < count; j++)  // we'll pick and boost a skill/stat "count" number of times
    {
        int *affect_skill = nullptr;
        int total_chance = 0;
        std::vector<int> chance;

        for (int i = 0; i < (int)possible_skills.size(); i++)
        {  // figure chances for each skill/stat; more likely to choose those they're better at
            chance.push_back((int)pow((float)*possible_skills.at(i), 2));
            total_chance += chance[i];
        }
        int choice = g_Dice.random(total_chance);

        total_chance = 0;
        for (int i = 0; i < (int)chance.size(); i++)
        {
            if (choice < (chance[i] + total_chance))
            {
                affect_skill = possible_skills.at(i);
                break;
            }
            total_chance += chance[i];
        }
        /*
        *    OK, we've picked a skill/stat. Now to boost it however many times were specified
        */
        BoostSkill(affect_skill, boost_count);
    }
}

void sGang::BoostSkill(int* affect_skill, int count)
{
/*
    *    OK, we've been passed a skill/stat. Now to raise it an amount depending on how high the
    *    skill/stat already is. The formula is fairly simple.
    *    Where x = current skill level, and y = median boost amount:
    *    y = (70/x)^2
    *    If y > 5, y = 5.
    *    Then, we get a random number ranging from (y/2) to (y*1.5) for the actual boost
    *    amount.
    *    Of course, we can't stick a floating point number into a char/int, so instead we
    *    use the remaining decimal value as a percentage chance for 1 more point. For
    *    example, 3.57 would be 3 points guaranteed, with 57% chance to instead get 4 points.
    *
    *    ex. 1: 50 points in skill. (70/50)^2 = 1.96. Possible point range: 0.98 to 2.94
    *    ex. 2: 30 points in skill. (70/30)^2 = 5.44. Possible point range: 2.72 to 8.16
    *    ex. 3: 75 points in skill. (70/75)^2 = 0.87. Possible point range: 0.44 to 1.31
    */
    for (int j = 0; j < count; j++)  // we'll boost the skill/stat "count" number of times
    {
        if (*affect_skill < 1) *affect_skill = 1;

        double boost_amount = pow(70 / (double)*affect_skill, 2);
        if (boost_amount > 5) boost_amount = 5;

        boost_amount = (double)g_Dice.in_range(int((boost_amount / 2) * 100), int((boost_amount*1.5) * 100)) / 100;
        char one_more = g_Dice.percent(int((boost_amount - (int)boost_amount) * 100)) ? 1 : 0;
        char final_boost = (char)boost_amount + one_more;

        *affect_skill += final_boost;

        if (*affect_skill > 100) *affect_skill = 100;
    }
}

void sGang::BoostStat(STATS stat, int count)
{
    BoostSkill(&m_Stats[stat], count);
}

void sGang::BoostSkill(SKILLS skill, int count)
{
    BoostSkill(&m_Skills[skill], count);
}

bool sGang::use_net(sGirl& target)
{
    if(m_Nets <= 0)
        return false;

    bool captured = false;
    int damagechance = 40;
    if (g_Dice.percent(combat()))    // hit her with the net
    {
        // girls escape change: Rise with agility
        double escape_chance = 25 + target.agility();
        //  but decrease with tiredness
        int    stamina       = 20 + target.constitution();
        if(target.tiredness() > stamina)
            escape_chance -= (target.tiredness() - stamina) / 2.0;
        // and with wounds
        if(target.health() < 80)
            escape_chance -= (80 - target.health()) / 2.0;

        escape_chance = std::min(90.0, std::max(10.0, escape_chance));

        if (!g_Dice.percent(escape_chance))    // she can't avoid or get out of the net
        {
            captured = true;
        }
        else
        {
            damagechance = 60;
        }
        target.tiredness(escape_chance * (0.5 - target.constitution() / 300.0));
    }

    damage_net(damagechance);

    return captured;
}

bool sGang::damage_net(double chance)
{
    if (g_Dice.percent(chance)) { m_Nets--; return true; }
    return false;
}

int sGang::give_nets(int n)
{
    int max_amount = std::min(n, members() / 2 - m_Nets);
    m_Nets += max_amount;
    return max_amount;
}

int sGang::give_potions(int n)
{
    int max_amount = std::min(n, members() - m_Potions);
    m_Potions += max_amount;
    return max_amount;
}

void sGang::use_potion()
{
    if(m_Potions > 0) {
        m_Potions--;
        // TODO currently, health only exists temporarily for combat!
    }
}

void sGang::AddMessage(std::string message, EventType event_type) {
    m_Events.AddMessage(std::move(message), IMGTYPE_PROFILE, event_type);
}

int sGang::max_members() {
    return g_Game->settings().get_integer(settings::GANG_MAX_MEMBERS);
}
