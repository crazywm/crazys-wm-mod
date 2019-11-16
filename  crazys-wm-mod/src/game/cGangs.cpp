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
#include "buildings/cBrothel.h"
#include "character/cCustomers.h"
#include "cGirls.h"
#include "cTariff.h"
#include "XmlMisc.h"
#include "cObjectiveManager.hpp"
#include "cInventory.h"
#include "Game.hpp"
#include "sStorage.hpp"
#include "gang_missions.hpp"

#include "DirPath.h"
#include "utils/streaming_random_selection.hpp"
#include "utils/algorithms.hpp"
#include "CLog.h"
#include "xml/util.h"
#include "sConfig.h"

extern cRng g_Dice;
extern cConfig cfg;

namespace settings {
    extern const char* GANG_MAX_RECRUIT_LIST;
    extern const char* GANG_MAX_START_MEMBERS;
    extern const char* GANG_MIN_START_MEMBERS;
    extern const char* GANG_REMOVE_CHANCE;
    extern const char* GANG_MIN_WEEKLY_NEW;
    extern const char* GANG_MAX_WEEKLY_NEW;
}

cGangManager::cGangManager()
{
	m_BusinessesExtort = 0;
	m_Gang_Gets_Girls = m_Gang_Gets_Items = m_Gang_Gets_Beast = 0;

    // load gang names
    ifstream in;
    // WD: Typecast to resolve ambiguous call in VS 2010
    DirPath dp = DirPath() << "Resources" << "Data" << "HiredGangNames.txt";
    in.open(dp.c_str());
    in.seekg(0);
    int num_gang_names;
    in >> num_gang_names;	// ignore the first line
    /// TODO(fix) automatically determine number of lines
    for (int i = 0; i <= num_gang_names; i++)
    {
        std::string name;
        in >> name;
        m_GangNames.push_back(std::move(name));
    }

    m_Missions.resize(MISS_COUNT);
    m_Missions[MISS_GUARDING] = std::make_unique<cMissionGuarding>();
    m_Missions[MISS_SABOTAGE] = std::make_unique<cMissionSabotage>();
    m_Missions[MISS_SPYGIRLS] = std::make_unique<cMissionSpyGirls>();
    m_Missions[MISS_CAPTUREGIRL] = std::make_unique<cMissionRecapture>();
    m_Missions[MISS_EXTORTION] = std::make_unique<cMissionExtortion>();
    m_Missions[MISS_PETYTHEFT] = std::make_unique<cMissionPettyTheft>();
    m_Missions[MISS_GRANDTHEFT] = std::make_unique<cMissionGrandTheft>();
    m_Missions[MISS_KIDNAPP] = std::make_unique<cMissionKidnap>();
    m_Missions[MISS_CATACOMBS] = std::make_unique<cMissionCatacombs>();
    m_Missions[MISS_TRAINING] = std::make_unique<cMissionTraining>();
    m_Missions[MISS_RECRUIT] = std::make_unique<cMissionRecruiting>();
    m_Missions[MISS_SERVICE] = std::make_unique<cMissionService>();
    m_Missions[MISS_DUNGEON] = nullptr;
}

cGangManager::~cGangManager() = default;

bool cGangManager::LoadGangsXML(tinyxml2::XMLElement* pGangManager)
{
	if (pGangManager == nullptr) return false;

	// load goons and goon missions
	auto* pGangs = pGangManager->FirstChildElement("Gangs");
	if (pGangs)
	{
		for (auto* pGang = pGangs->FirstChildElement("Gang"); pGang != nullptr; pGang = pGang->NextSiblingElement("Gang"))
		{
			auto gang = std::make_unique<sGang>();
			if (gang->LoadGangXML(pGang)) {
			    AddGang(std::move(gang));
			}
		}
	}
	// load hireable goons
	auto* pHireables = pGangManager->FirstChildElement("Hireables");
	if (pHireables)
	{
		for (auto* pGang = pHireables->FirstChildElement("Gang"); pGang != nullptr; pGang = pGang->NextSiblingElement("Gang"))
		{
			auto gang = std::make_unique<sGang>();
			if (gang->LoadGangXML(pGang)) {
			    m_HireableGangs.push_back(std::move(gang));
			}
		}
	}

	pGangManager->QueryIntAttribute("BusinessesExtort", &m_BusinessesExtort);
	pGangManager->QueryIntAttribute("NumHealingPotions", &m_NumHealingPotions);
	pGangManager->QueryIntAttribute("NumNets", &m_NumNets);
	pGangManager->QueryIntAttribute("KeepHealStocked", &m_KeepHealStocked);
	pGangManager->QueryIntAttribute("KeepNetsStocked", &m_KeepNetsStocked);

	// `J` added for .06.01.10
	pGangManager->QueryIntAttribute("Gang_Gets_Girls", &m_Gang_Gets_Girls);
	pGangManager->QueryIntAttribute("Gang_Gets_Items", &m_Gang_Gets_Items);
	pGangManager->QueryIntAttribute("Gang_Gets_Beast", &m_Gang_Gets_Beast);
	if ((m_Gang_Gets_Girls == 0 && m_Gang_Gets_Items == 0 && m_Gang_Gets_Beast == 0) ||
		m_Gang_Gets_Girls + m_Gang_Gets_Items + m_Gang_Gets_Beast != 100)
	{
		m_Gang_Gets_Items = (int)cfg.catacombs.gang_gets_items();
		m_Gang_Gets_Beast = (int)cfg.catacombs.gang_gets_beast();
		m_Gang_Gets_Girls = 100 - m_Gang_Gets_Items - m_Gang_Gets_Beast;
	}

	return true;
}

tinyxml2::XMLElement& cGangManager::SaveGangsXML(tinyxml2::XMLElement& elRoot)
{
    auto& elGangManager = PushNewElement(elRoot, "Gang_Manager");

	auto& elGangs = PushNewElement(elGangManager, "Gangs");
	for(auto& gang : m_PlayersGangs)
	{
		gang->SaveGangXML(elGangs);
	}
	auto& elHireables =  PushNewElement(elGangManager, "Hireables");
    for(auto& gang : m_HireableGangs)
	{
		gang->SaveGangXML(elHireables);
	}

	elGangManager.SetAttribute("BusinessesExtort", m_BusinessesExtort);
	elGangManager.SetAttribute("NumHealingPotions", m_NumHealingPotions);
	elGangManager.SetAttribute("NumNets", m_NumNets);
	elGangManager.SetAttribute("KeepHealStocked", m_KeepHealStocked);
	elGangManager.SetAttribute("KeepNetsStocked", m_KeepNetsStocked);

	// `J` added for .06.01.10
	if (m_Gang_Gets_Girls == 0 && m_Gang_Gets_Items == 0 && m_Gang_Gets_Beast == 0)
	{
		m_Gang_Gets_Items = (int)cfg.catacombs.gang_gets_items();
		m_Gang_Gets_Beast = (int)cfg.catacombs.gang_gets_beast();
		m_Gang_Gets_Girls = 100 - m_Gang_Gets_Items - m_Gang_Gets_Beast;

	}
	elGangManager.SetAttribute("Gang_Gets_Girls", m_Gang_Gets_Girls);
	elGangManager.SetAttribute("Gang_Gets_Items", m_Gang_Gets_Items);
	elGangManager.SetAttribute("Gang_Gets_Beast", m_Gang_Gets_Beast);
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

bool sGang::LoadGangXML(tinyxml2::XMLElement* pGang)
{
	if (pGang == nullptr) return false;
	if (pGang->Attribute("Name")) m_Name = pGang->Attribute("Name");
	pGang->QueryIntAttribute("Num", &m_Num);
	LoadSkillsXML(pGang->FirstChildElement("Skills"), m_Skills);
	LoadStatsXML(pGang->FirstChildElement("Stats"), m_Stats);
	if (m_Skills[SKILL_MAGIC] <= 0 || m_Skills[SKILL_COMBAT] <= 0 || m_Stats[STAT_INTELLIGENCE] <= 0 || m_Stats[STAT_AGILITY] <= 0 ||
		m_Stats[STAT_CONSTITUTION] <= 0 || m_Stats[STAT_CHARISMA] <= 0 || m_Stats[STAT_STRENGTH] <= 0 || m_Skills[SKILL_SERVICE] <= 0)
	{
		int total =
			max(0, m_Skills[SKILL_MAGIC]) +
			max(0, m_Skills[SKILL_COMBAT]) +
			max(0, m_Stats[STAT_INTELLIGENCE]) +
			max(0, m_Stats[STAT_AGILITY]) +
			max(0, m_Stats[STAT_CONSTITUTION]) +
			max(0, m_Stats[STAT_CHARISMA]) +
			max(0, m_Stats[STAT_STRENGTH]);
		int low = total / 8;
		int high = total / 6;
		if (m_Skills[SKILL_MAGIC] <= 0)				m_Skills[SKILL_MAGIC] = g_Dice.bell(low, high);
		if (m_Skills[SKILL_COMBAT] <= 0)			m_Skills[SKILL_COMBAT] = g_Dice.bell(low, high);
		if (m_Stats[STAT_INTELLIGENCE] <= 0)		m_Stats[STAT_INTELLIGENCE] = g_Dice.bell(low, high);
		if (m_Stats[STAT_AGILITY] <= 0)				m_Stats[STAT_AGILITY] = g_Dice.bell(low, high);
		if (m_Stats[STAT_CONSTITUTION] <= 0)		m_Stats[STAT_CONSTITUTION] = g_Dice.bell(low, high);
		if (m_Stats[STAT_CHARISMA] <= 0)			m_Stats[STAT_CHARISMA] = g_Dice.bell(low, high);
		if (m_Stats[STAT_STRENGTH] <= 0)			m_Stats[STAT_STRENGTH] = g_Dice.bell(low, high);
		if (m_Skills[SKILL_SERVICE] <= 0)			m_Skills[SKILL_SERVICE] = g_Dice.bell(low / 2, high);	// `J` added for .06.02.41
	}

	//these may not have been saved
	//if not, the query just does not set the value
	//so the default is used, assuming the gang was properly init
	pGang->QueryAttribute("MissionID", &m_MissionID);
	pGang->QueryIntAttribute("LastMissID", &m_LastMissID);
	pGang->QueryAttribute("Combat", &m_Combat);
	pGang->QueryAttribute("AutoRecruit", &m_AutoRecruit);
	pGang->QueryIntAttribute("Nets", &m_Nets);
    pGang->QueryIntAttribute("WeaponLevel", &m_WpnLevel);

	return true;
}

// ----- Hire fire

void cGangManager::HireGang(int gangID)
{
	auto gang = std::move(m_HireableGangs.at(gangID));
	m_HireableGangs.erase(m_HireableGangs.begin() + gangID);
    gang->m_Combat = gang->m_AutoRecruit = false;
    gang->m_LastMissID = -1;
    if (gang->m_Num <= 5) gang->m_MissionID = MISS_RECRUIT;
    AddGang(std::move(gang));
}

void cGangManager::FireGang(int gangID)
{
    auto gang = std::move(m_PlayersGangs.at(gangID));
    m_PlayersGangs.erase(m_PlayersGangs.begin() + gangID);
    if (m_HireableGangs.size() < g_Game->settings().get_integer(settings::GANG_MAX_RECRUIT_LIST))
    {
        gang->m_Combat = gang->m_AutoRecruit = false;
        gang->m_LastMissID = -1;
        m_HireableGangs.push_back(std::move(gang));
    }
}

// ----- Add remove

void cGangManager::AddNewGang(bool boosted)
{
    int name_index = -1;
    while (true)
    {
        name_index = g_Dice%m_GangNames.size();
        const std::string& name = m_GangNames[name_index];
        if(std::any_of(begin(m_PlayersGangs), end(m_PlayersGangs),
                       [&](auto& gang) { return gang->name() == name; })) {
            continue;
        }

        if(std::any_of(begin(m_HireableGangs), end(m_HireableGangs),
                       [&](auto& gang) { return gang->name() == name; })) {
            continue;
        }
        break;
    }

	auto newGang = std::make_unique<sGang>(m_GangNames[name_index]);

	int max_members = g_Game->settings().get_integer(settings::GANG_MAX_START_MEMBERS);
	int min_members = g_Game->settings().get_integer(settings::GANG_MIN_START_MEMBERS);
	newGang->m_Num = g_Dice.in_range(min_members, max_members);
	if (boosted) newGang->m_Num = min(15, newGang->m_Num + 5);

	int new_val;
	for (int & m_Skill : newGang->m_Skills)
	{
		new_val = (g_Dice % 30) + 21;
		if (g_Dice % 5 == 1)	new_val += 1 + g_Dice % 10;
		if (boosted)			new_val += 10 + g_Dice % 11;
		m_Skill = new_val;
	}
	for (int & m_Stat : newGang->m_Stats)
	{
		new_val = (g_Dice % 30) + 21;
		if (g_Dice % 5 == 1)	new_val += g_Dice % 10;
		if (boosted)			new_val += 10 + g_Dice % 11;
		m_Stat = new_val;
	}
	newGang->m_Stats[STAT_HEALTH] = 100;
	newGang->m_Stats[STAT_HAPPINESS] = 100;

    g_LogFile.info("gangs", "Added new recruitable gang ", newGang->name());
    m_HireableGangs.push_back(std::move(newGang));

}

void cGangManager::AddGang(std::unique_ptr<sGang> newGang)
{
    m_PlayersGangs.push_back(std::move(newGang));
}

void cGangManager::RemoveGang(sGang* gang)
{
    auto iter = std::find_if(m_PlayersGangs.begin(), m_PlayersGangs.end(), [gang](auto& e) { return e.get() == gang; });
    if(iter != m_PlayersGangs.end()) {
        m_PlayersGangs.erase(iter);
    }
}

// ----- Get

int cGangManager::GetNumGangs()
{
	return m_PlayersGangs.size();
}

int cGangManager::GetMaxNumGangs()
{
	m_MaxNumGangs = 7 + g_Game->buildings().num_buildings(BuildingType::BROTHEL);
	return m_MaxNumGangs;
}

int cGangManager::GetNumHireableGangs()
{
	return m_HireableGangs.size();
}

sGang cGangManager::GetTempGang()
{
	sGang newGang;
	newGang.m_Num = g_Dice % 6 + 10;
	for (int & m_Skill : newGang.m_Skills)	m_Skill = (g_Dice % 30) + 21;
	for (int & m_Stat : newGang.m_Stats)	m_Stat = (g_Dice % 30) + 21;
	newGang.m_Stats[STAT_HEALTH] = 100;
	newGang.m_Stats[STAT_HAPPINESS] = 100;
	newGang.set_weapon_level( g_Dice.in_range(1, 3) );
	return newGang;
}

// `J` added temp gang mod - base strength + mod
sGang cGangManager::GetTempGang(int mod)
{
	sGang newGang;
	newGang.m_Num = min(15, g_Dice.bell(6, 18));
	for (int & m_Skill : newGang.m_Skills)
	{
		m_Skill = (g_Dice % 40) + 21 + (g_Dice % mod);
		if (m_Skill < 1)	m_Skill = 1;
		if (m_Skill > 100)	m_Skill = 100;
	}
	for (int & m_Stat : newGang.m_Stats)
	{
		m_Stat = (g_Dice % 40) + 21 + (g_Dice % mod);
		if (m_Stat < 1)	m_Stat = 1;
		if (m_Stat > 100)	m_Stat = 100;
	}
	newGang.m_Stats[STAT_HEALTH] = 100;
	newGang.m_Stats[STAT_HAPPINESS] = 100;
    newGang.set_weapon_level( g_Dice.in_range(1, 3) );

	return newGang;
}

sGang* cGangManager::GetGang(int gangID)
{
    if(gangID >= m_PlayersGangs.size() || gangID < 0)
        return nullptr;
    else
        return m_PlayersGangs.at(gangID).get();
}

sGang* cGangManager::GetTempWeakGang()
{
	// MYR: Weak gangs attack girls when they work
	sGang* newGang = new sGang();
	newGang->m_Num = 15;
	for (int & m_Skill : newGang->m_Skills)	m_Skill = g_Dice % 30 + 51;
	for (int & m_Stat : newGang->m_Stats)	m_Stat = g_Dice % 30 + 51;
	newGang->m_Stats[STAT_HEALTH] = 100;
    newGang->set_weapon_level( g_Dice.in_range(0, 1) );
	return newGang;
}

void sGang::AdjustGangSkill(int Skill, int amount)	// `J` added for .06.02.41
{
	this->m_Skills[Skill] += amount;
	if (this->m_Skills[Skill] < 0)		this->m_Skills[Skill] = 0;
	if (this->m_Skills[Skill] > 100)	this->m_Skills[Skill] = 100;
}
void sGang::AdjustGangStat(int Stat, int amount)	// `J` added for .06.02.41
{
	this->m_Stats[Stat] += amount;
	if (this->m_Stats[Stat] < 0)		this->m_Stats[Stat] = 0;
	if (this->m_Stats[Stat] > 100)		this->m_Stats[Stat] = 100;
}

sGang::sGang(std::string name) :
    m_Name(std::move(name))
{
    m_MissionID = MISS_GUARDING;
    m_AutoRecruit = false;
    for (int & m_Skill : m_Skills)	m_Skill = 0;
    for (int & m_Stat : m_Stats)	m_Stat = 0;
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
	*	Which of the passed skills/stats will be raised this time?
	*	Hopefully they'll tend to focus a bit more on what they're already good at...
	*	that way, they will have strengths instead of becoming entirely homogenized
	*
	*	ex. 60 combat, 50 magic, and 40 intelligence: squared, that comes to 3600, 2500 and 1600...
	*		so: ~46.75% chance combat, ~32.46% chance magic, ~20.78% chance intelligence
	*/
    for (int j = 0; j < count; j++)  // we'll pick and boost a skill/stat "count" number of times
    {
        int *affect_skill = nullptr;
        int total_chance = 0;
        vector<int> chance;

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
        *	OK, we've picked a skill/stat. Now to boost it however many times were specified
        */
        BoostSkill(affect_skill, boost_count);
    }
}

void sGang::BoostSkill(int* affect_skill, int count)
{
/*
	*	OK, we've been passed a skill/stat. Now to raise it an amount depending on how high the
	*	skill/stat already is. The formula is fairly simple.
	*	Where x = current skill level, and y = median boost amount:
	*	y = (70/x)^2
	*	If y > 5, y = 5.
	*	Then, we get a random number ranging from (y/2) to (y*1.5) for the actual boost
	*	amount.
	*	Of course, we can't stick a floating point number into a char/int, so instead we
	*	use the remaining decimal value as a percentage chance for 1 more point. For
	*	example, 3.57 would be 3 points guaranteed, with 57% chance to instead get 4 points.
	*
	*	ex. 1: 50 points in skill. (70/50)^2 = 1.96. Possible point range: 0.98 to 2.94
	*	ex. 2: 30 points in skill. (70/30)^2 = 5.44. Possible point range: 2.72 to 8.16
	*	ex. 3: 75 points in skill. (70/75)^2 = 0.87. Possible point range: 0.44 to 1.31
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
    if (g_Dice.percent(combat()))	// hit her with the net
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

        if (!g_Dice.percent(escape_chance))	// she can't avoid or get out of the net
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

// ----- Combat

// MYR: This is similar to GangCombat, but instead of one of the players gangs
//      fighting the girl, some random gang attacks her.  This random gang
//      doesn't have healing potions and the weapon levels of a player gang.
//      ATM only the new security code uses it.
//      This will also be needed to be updated to the new way of doing combat.
// true means the girl won

// ----- Mission related

// Missions done here - Updated for .06.01.09
void cGangManager::UpdateGangs()
{
	stringstream ss;

	// maintain recruitable gangs list, potentially pruning some old ones
    auto remove_chance = g_Game->settings().get_percent(settings::GANG_REMOVE_CHANCE);
    erase_if(m_HireableGangs,  [remove_chance](auto& gang) { return g_Dice.percent(remove_chance); });

	// maybe add some new gangs to the recruitable list
	int add_min = g_Game->settings().get_integer(settings::GANG_MIN_WEEKLY_NEW);
	int add_max = g_Game->settings().get_integer(settings::GANG_MAX_WEEKLY_NEW);
	int add_recruits = g_Dice.bell(add_min, add_max);
	for (int i = 0; i < add_recruits; i++)
	{
		if (m_HireableGangs.size() >= g_Game->settings().get_integer(settings::GANG_MAX_RECRUIT_LIST)) break;
		AddNewGang(false);
	}

	// now, deal with player controlled gangs on missions
	for(auto& currentGang : m_PlayersGangs) {
        switch (currentGang->m_MissionID) {
        case MISS_GUARDING: // these are handled in GangStartOfShift()
        case MISS_SPYGIRLS:
            break;
        case MISS_CAPTUREGIRL:
            if (g_Game->GetNumRunaways() > 0) {
                m_Missions[MISS_CAPTUREGIRL]->run(*currentGang);
                break;
            }
            else
                currentGang->m_Events.AddMessage(
                        "This gang was sent to look for runaways but there are none so they went looking for any girl to kidnap instead.",
                        IMGTYPE_PROFILE, EVENT_GANG);
        case MISS_SABOTAGE:
        case MISS_EXTORTION:
        case MISS_PETYTHEFT:
        case MISS_GRANDTHEFT:
        case MISS_KIDNAPP:
        case MISS_CATACOMBS:
        case MISS_TRAINING:
        case MISS_RECRUIT:
        case MISS_SERVICE:
            m_Missions[currentGang->m_MissionID]->run(*currentGang);
            break;
        default: {
            stringstream sse;
            g_LogFile.log(ELogLevel::ERROR, "no mission set or mission not found : ", currentGang->m_MissionID);
            sse << "Error: no mission set or mission not found : " << currentGang->m_MissionID;
            currentGang->m_Events.AddMessage(sse.str(), IMGTYPE_PROFILE, EVENT_GANG);
        }
            break;
        }
    }

	// remove lost gangs
    erase_if(m_PlayersGangs,  [this](auto& gang) { return this->losegang(*gang); });

	// recruitment
	for(auto& gang : m_PlayersGangs) {
        if (!gang->m_Combat && gang->m_Num < 15) gang->m_Num++;
        check_gang_recruit(*gang);
    }
	g_Game->rivals().Update(m_BusinessesExtort);	// Update the rivals

	RestockNetsAndPots();

	// healing
	for(auto& gang : m_PlayersGangs) {
	    if(gang->m_MedicalCost > 0) {
	        gang->m_Events.AddMessage("Some of your goons have been hurt in the line of duty. You pay "
	                + std::to_string(gang->m_MedicalCost) + " gold so a doctor keeps them fit for work.",
	                0, EVENT_GANG);
	        g_Game->gold().goon_wages( gang->m_MedicalCost );
	        gang->m_MedicalCost = 0;
	    }
	}
}

// `J` restock at the start and end of the gang shift - Added for .06.01.09
void cGangManager::RestockNetsAndPots()
{
	g_LogFile.log(ELogLevel::DEBUG,
	        "Time to restock heal potions and nets\n"
		, "Heal Flag    = ", bool(m_KeepHealStocked > 0), "\n"
		, "Heal Target  = ", m_KeepHealStocked, "\n"
		, "Heal Current = ", m_KeepHealStocked, "\n"
		, "Nets Flag    = ", bool(m_KeepNetsStocked > 0), "\n"
		, "Nets Target  = ", m_KeepNetsStocked, "\n"
		, "Nets Current = ", m_KeepNetsStocked);
	if (m_KeepHealStocked > 0 && m_KeepHealStocked > m_NumHealingPotions)
	{
		int diff = m_KeepHealStocked - m_NumHealingPotions;
		m_NumHealingPotions = m_KeepHealStocked;
		g_Game->gold().consumable_cost(g_Game->tariff().healing_price(diff));
	}
	if (m_KeepNetsStocked > 0 && m_KeepNetsStocked > m_NumNets)
	{
		int diff = m_KeepNetsStocked - m_NumNets;
		m_NumNets = m_KeepNetsStocked;
		g_Game->gold().consumable_cost(g_Game->tariff().nets_price(diff));
	}
}

sGang* cGangManager::GetGangOnMission(u_int missID)
{
    auto found = std::find_if(begin(m_PlayersGangs), end(m_PlayersGangs),
            [missID](auto& gang) {
        return gang->m_Num > 0 && gang->m_MissionID == missID;
    });
	if(found == m_PlayersGangs.end()) {
	    return nullptr;
	}
	return found->get();
}

sGang* cGangManager::GetRandomGangOnMission(u_int missID)
{
    RandomSelector<sGang> selector;
    for(auto& gang : m_PlayersGangs) {
        if(gang->m_MissionID == missID && gang->m_Num > 0) {
            selector.process(gang.get());
        }
    }
    return selector.selection();
}

// `J` - Added for .06.02.18
sGang* cGangManager::GetGangNotFull(int roomfor, bool recruiting)
{
	if (recruiting)
	{
		int missions[5] = { MISS_RECRUIT, MISS_TRAINING, MISS_SPYGIRLS, MISS_GUARDING, MISS_SERVICE };
		for(auto& gang: m_PlayersGangs) {
			if (gang->m_Num + roomfor <= 15)
			{
				for (auto mission : missions) if (gang->m_MissionID == mission)	return gang.get();
			}
		}
	}
	else
	{
        for(auto& gang: m_PlayersGangs)
		{
			if (gang->m_Num < 15) return gang.get();
		}
	}
	return nullptr;
}

// `J` - Added for .06.01.09
sGang* cGangManager::GetGangRecruitingNotFull(int roomfor)
{
    // first try to get a gang that can hold all that are being sent to them
    sGang* rec = GetGangNotFull(roomfor, true);
    if(rec) return rec;
	// if none are found then get a gang that has room for at least 1
	return GetGangNotFull(1, true);
}

// Get a vector with all the gangs doing MISS_FOO
vector<sGang*> cGangManager::gangs_on_mission(u_int mission_id)
{
	vector<sGang*> v; // loop through the gangs
	for(auto& gang : m_PlayersGangs)
	{
		// if they're not doing the job we are looking for, disregard them
		if (gang->m_MissionID != mission_id) continue;
		v.push_back(gang.get());
	}
	return v;
}

// `J` - Added for .06.01.09
vector<sGang*> cGangManager::gangs_watching_girls()
{
	vector<sGang*> v; // loop through the gangs
    for(auto& gang : m_PlayersGangs)
    {
		// if they're not doing the job we are looking for, disregard them
		if (gang->m_MissionID == MISS_GUARDING || gang->m_MissionID == MISS_SPYGIRLS) v.push_back(gang.get());
	}
	return v;
}

/*
*	two objectives here:
*	A: multiple squads spying on the girls improves the chance of catching thieves
*	B: The intelligence of the girl and the goons affects the result
*/
int cGangManager::chance_to_catch(sGirl* girl)
{
	int pc = 0;
	vector<sGang*> gvec = gangs_on_mission(MISS_SPYGIRLS);	// get a vector containing all the spying gangs
	for (const auto& gang: gvec)		// whizz down the vector adding probability as we go
	{
		/*
		*		now then: the basic chance is 5 * number of goons
		*		but I want to modify that for the intelligence
		*		of the girl, and that of the squad
		*/
		float mod = 1.f + float(gang->intelligence() - girl->intelligence()) / 100.f;
		/*
		*		that should give us a multiplier that can
		*		at one extreme, double the chances of the sqaud
		*		catching her, and at the other, reduce it to zero
		*/
		pc += int(5 * gang->m_Num * mod);

		gang->BoostStat(STAT_INTELLIGENCE, 1);
	}
	if (pc > 100) pc = 100;
	return pc;
}

// `J` - Added for .06.01.09
bool cGangManager::losegang(sGang& gang)
{
	if (gang.m_Num <= 0)
	{
		stringstream ss;
		int mission = gang.m_MissionID;
		ss << gang.name() << " was lost while ";
		switch (mission)
		{
		case MISS_GUARDING:		ss << "guarding.";							break;
		case MISS_SABOTAGE:		ss << "attacking your rivals.";				break;
		case MISS_SPYGIRLS:		ss << "spying on your girls?";				break;
		case MISS_CAPTUREGIRL:	ss << "trying to recapture a runaway.";		break;
		case MISS_EXTORTION:	ss << "trying to extort new businesses.";	break;
		case MISS_PETYTHEFT:	ss << "performing petty crimes.";			break;
		case MISS_GRANDTHEFT:	ss << "performing major crimes.";			break;
		case MISS_KIDNAPP:		ss << "trying to kidnap girls.";			break;
		case MISS_CATACOMBS:	ss << "exploring the catacombs.";			break;
		case MISS_TRAINING:		ss << "training?";							break;
		case MISS_RECRUIT:		ss << "recruiting?";						break;
		case MISS_SERVICE:		ss << "helping the community.";				break;
		default:				ss << "on a mission.";						break;
		}
		g_Game->push_message(ss.str(), COLOR_RED);
		return true;
	}
	return false;
}

// `J` - Added for .06.01.09
void cGangManager::check_gang_recruit(sGang& gang)
{
	stringstream ss;
	if (gang.m_MissionID == MISS_SERVICE || gang.m_MissionID == MISS_TRAINING){}	// `J` service and training can have as few as 1 member doing it.
	else if (gang.m_Num <= 5 && gang.m_MissionID != MISS_RECRUIT)
	{
		ss << "Gang   " << gang.name() << "   were set to recruit due to low numbers";
		gang.m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_WARNING);
		gang.m_AutoRecruit = true;
		gang.m_LastMissID = gang.m_MissionID;
		gang.m_MissionID = MISS_RECRUIT;
	}
	else if (gang.m_MissionID == MISS_RECRUIT && gang.m_Num >= 15)
	{
		if (gang.m_AutoRecruit)
		{
			ss << "Gang   " << gang.name() << "   were placed back on their previous mission now that their numbers are back to normal.";
			gang.m_MissionID = gang.m_LastMissID;
			gang.m_AutoRecruit = false;
		}
		else
		{
			ss << "Gang   " << gang.name() << "   were placed on guard duty from recruitment as their numbers are full.";
			gang.m_MissionID = MISS_GUARDING;
		}
		gang.m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_WARNING);
	}
}

// `J` - Added for .06.01.09
void cGangManager::GangStartOfShift()
{
	stringstream ss;

	RestockNetsAndPots();

	// update goons for the start of the turn
	int cost = 0;
    // remove lost gangs
    erase_if(m_PlayersGangs,  [&,this](auto& gang) {
        if (gang->m_Num <= 0)	// clear dead
        {
            ss << "All of the men in gang " << gang->name() << " have died.";
            g_Game->push_message(ss.str(), COLOR_RED);
            return true;
        }
        return false;
    });

    if (m_PlayersGangs.empty()) return;	// no gangs

	for(auto& gang: m_PlayersGangs)
    {
        gang->m_Combat = false;
        gang->m_Events.Clear();
		cost += g_Game->tariff().goon_mission_cost(gang->m_MissionID);	// sum up the cost of all the goon missions

		check_gang_recruit(*gang);

		if (gang->m_MissionID == MISS_SPYGIRLS)	gang->m_Events.AddMessage("Gang   " + gang->name() + "   is spying on your girls.", IMGTYPE_PROFILE, EVENT_GANG);
		if (gang->m_MissionID == MISS_GUARDING)	gang->m_Events.AddMessage("Gang   " + gang->name() + "   is guarding.", IMGTYPE_PROFILE, EVENT_GANG);
	}
	g_Game->gold().goon_wages(cost);

    // give out nets
    while(m_NumNets > 0) {
        int last_nets = m_NumNets;
        for(auto& gang: m_PlayersGangs) {
            // if only a few nets are left, only give out to gangs who need them for their job.
            if(m_NumNets < 10 && !m_Missions[gang->m_MissionID]->requires_nets())
                continue;
            m_NumNets -= gang->give_nets(1);
        }

        // if no gang needs any more nets, we can stop
        if(m_NumNets == last_nets)
            break;
    }

    // give out potions
    while(m_NumNets > 0) {
        int last_potions = m_NumHealingPotions;
        for(auto& gang: m_PlayersGangs) {
            // if only a few nets are left, only give out to gangs who need them for their job.
            if(m_NumHealingPotions < 10 && !m_Missions[gang->m_MissionID]->requires_potions())
                continue;
            m_NumHealingPotions -= gang->give_potions(1);
        }

        // if no gang needs any more nets, we can stop
        if(m_NumHealingPotions == last_potions)
            break;
    }
}

int cGangManager::BuyNets(int amount, bool autobuy)
{
    amount = std::min(amount, 60 - m_NumNets);

    int cost = g_Game->tariff().nets_price(amount);
    if (g_Game->gold().item_cost(cost))
    {
        m_NumNets += amount;
    }

    if(autobuy) {
        KeepNetStocked(m_NumNets);
    }

    return amount;
}

int cGangManager::BuyHealingPotions(int amount, bool autobuy)
{
    amount = std::min(amount, 200 - m_NumHealingPotions);

    int cost = g_Game->tariff().healing_price(amount);
    if (g_Game->gold().item_cost(cost))
    {
        m_NumHealingPotions += amount;
    }

    if(autobuy) {
        KeepHealStocked(m_NumHealingPotions);
    }

    return amount;
}
