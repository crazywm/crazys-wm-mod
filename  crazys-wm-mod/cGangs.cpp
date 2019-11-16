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
#include <sstream>
#include <algorithm>

#include "cGangs.h"
#include "src/buildings/cBrothel.h"
#include "cCustomers.h"
#include "cGirls.h"
#include "cTariff.h"
#include "XmlMisc.h"
#include "cObjectiveManager.hpp"
#include "cInventory.h"
#include "src/Game.hpp"
#include "src/sStorage.hpp"
#include "gang_missions.hpp"

#include "DirPath.h"
#include "utils/streaming_random_selection.hpp"
#include "utils/algorithms.hpp"
#include "CLog.h"

extern cRng g_Dice;

cGangManager::cGangManager()
{
	m_BusinessesExtort = 0;
	m_SwordLevel = 0;
	m_Control_Gangs = false;
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

bool cGangManager::LoadGangsXML(TiXmlHandle hGangManager)
{
	TiXmlElement* pGangManager = hGangManager.ToElement();
	if (pGangManager == nullptr) return false;

	// load goons and goon missions
	TiXmlElement* pGangs = pGangManager->FirstChildElement("Gangs");
	if (pGangs)
	{
		for (TiXmlElement* pGang = pGangs->FirstChildElement("Gang"); pGang != nullptr; pGang = pGang->NextSiblingElement("Gang"))
		{
			auto gang = std::make_unique<sGang>();
			if (gang->LoadGangXML(TiXmlHandle(pGang))) {
			    AddGang(std::move(gang));
			}
		}
	}
	// load hireable goons
	TiXmlElement* pHireables = pGangManager->FirstChildElement("Hireables");
	if (pHireables)
	{
		for (TiXmlElement* pGang = pHireables->FirstChildElement("Gang"); pGang != nullptr; pGang = pGang->NextSiblingElement("Gang"))
		{
			auto gang = std::make_unique<sGang>();
			if (gang->LoadGangXML(TiXmlHandle(pGang))) {
			    m_HireableGangs.push_back(std::move(gang));
			}
		}
	}

	pGangManager->QueryIntAttribute("BusinessesExtort", &m_BusinessesExtort);
	pGangManager->QueryIntAttribute("SwordLevel", &m_SwordLevel);
	pGangManager->QueryIntAttribute("NumHealingPotions", &m_NumHealingPotions);
	pGangManager->QueryIntAttribute("NumNets", &m_NumNets);
	pGangManager->QueryIntAttribute("KeepHealStocked", &m_KeepHealStocked);
	pGangManager->QueryIntAttribute("KeepNetsStocked", &m_KeepNetsStocked);

	// `J` added for .06.01.10
	pGangManager->QueryValueAttribute<bool>("ControlCatacombs", &m_Control_Gangs);
	pGangManager->QueryIntAttribute("Gang_Gets_Girls", &m_Gang_Gets_Girls);
	pGangManager->QueryIntAttribute("Gang_Gets_Items", &m_Gang_Gets_Items);
	pGangManager->QueryIntAttribute("Gang_Gets_Beast", &m_Gang_Gets_Beast);
	if ((m_Gang_Gets_Girls == 0 && m_Gang_Gets_Items == 0 && m_Gang_Gets_Beast == 0) ||
		m_Gang_Gets_Girls + m_Gang_Gets_Items + m_Gang_Gets_Beast != 100)
	{
		m_Control_Gangs = cfg.catacombs.control_gangs();
		m_Gang_Gets_Items = (int)cfg.catacombs.gang_gets_items();
		m_Gang_Gets_Beast = (int)cfg.catacombs.gang_gets_beast();
		m_Gang_Gets_Girls = 100 - m_Gang_Gets_Items - m_Gang_Gets_Beast;
	}

	return true;
}

TiXmlElement* cGangManager::SaveGangsXML(TiXmlElement* pRoot)
{
	TiXmlElement* pGangManager = new TiXmlElement("Gang_Manager");
	pRoot->LinkEndChild(pGangManager);

	TiXmlElement* pGangs = new TiXmlElement("Gangs");
	pGangManager->LinkEndChild(pGangs);
	for(auto& gang : m_PlayersGangs)
	{
		TiXmlElement* pGang = gang->SaveGangXML(pGangs);
	}
	TiXmlElement* pHireables = new TiXmlElement("Hireables");
	pGangManager->LinkEndChild(pHireables);
    for(auto& gang : m_HireableGangs)
	{
		gang->SaveGangXML(pHireables);
	}

	pGangManager->SetAttribute("BusinessesExtort", m_BusinessesExtort);
	pGangManager->SetAttribute("SwordLevel", m_SwordLevel);
	pGangManager->SetAttribute("NumHealingPotions", m_NumHealingPotions);
	pGangManager->SetAttribute("NumNets", m_NumNets);
	pGangManager->SetAttribute("KeepHealStocked", m_KeepHealStocked);
	pGangManager->SetAttribute("KeepNetsStocked", m_KeepNetsStocked);

	// `J` added for .06.01.10
	if (m_Gang_Gets_Girls == 0 && m_Gang_Gets_Items == 0 && m_Gang_Gets_Beast == 0)
	{
		m_Control_Gangs = cfg.catacombs.control_gangs();
		m_Gang_Gets_Items = (int)cfg.catacombs.gang_gets_items();
		m_Gang_Gets_Beast = (int)cfg.catacombs.gang_gets_beast();
		m_Gang_Gets_Girls = 100 - m_Gang_Gets_Items - m_Gang_Gets_Beast;

	}
	pGangManager->SetAttribute("ControlCatacombs", m_Control_Gangs);
	pGangManager->SetAttribute("Gang_Gets_Girls", m_Gang_Gets_Girls);
	pGangManager->SetAttribute("Gang_Gets_Items", m_Gang_Gets_Items);
	pGangManager->SetAttribute("Gang_Gets_Beast", m_Gang_Gets_Beast);

	return pGangManager;
}

TiXmlElement* sGang::SaveGangXML(TiXmlElement* pRoot)
{
	TiXmlElement* pGang = new TiXmlElement("Gang");
	pRoot->LinkEndChild(pGang);
	pGang->SetAttribute("Num", m_Num);
	SaveSkillsXML(pGang, m_Skills);
	SaveStatsXML(pGang, m_Stats);
	pGang->SetAttribute("Name", m_Name);
	pGang->SetAttribute("Nets", m_Nets);

    pGang->SetAttribute("MissionID", m_MissionID);
    pGang->SetAttribute("LastMissID", m_LastMissID);
    pGang->SetAttribute("Combat", m_Combat);
    pGang->SetAttribute("AutoRecruit", m_AutoRecruit);
	return pGang;
}

bool sGang::LoadGangXML(TiXmlHandle hGang)
{
	TiXmlElement* pGang = hGang.ToElement();
	if (pGang == nullptr) return false;
	if (pGang->Attribute("Name")) m_Name = pGang->Attribute("Name");
	pGang->QueryIntAttribute("Num", &m_Num);
	LoadSkillsXML(hGang.FirstChild("Skills"), m_Skills);
	LoadStatsXML(hGang.FirstChild("Stats"), m_Stats);
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
	pGang->QueryValueAttribute<u_int>("MissionID", &m_MissionID);
	pGang->QueryIntAttribute("LastMissID", &m_LastMissID);
	pGang->QueryValueAttribute<bool>("Combat", &m_Combat);
	pGang->QueryValueAttribute<bool>("AutoRecruit", &m_AutoRecruit);
	pGang->QueryIntAttribute("Nets", &m_Nets);

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
    if (m_HireableGangs.size() < cfg.gangs.max_recruit_list())
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

	int max_members = cfg.gangs.init_member_max();
	int min_members = cfg.gangs.init_member_min();
	newGang->m_Num = min_members + g_Dice % (max_members + 1 - min_members);
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

	return newGang;
}

sGang* cGangManager::GetGang(int gangID)
{
    if(gangID >= m_PlayersGangs.size())
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

/*	GangBrawl - returns true if gang1 wins and false if gang2 wins
*	If the Player's gang is in the fight, make sure it is the first gang
*	If two Rivals are fighting set rivalVrival to true
*/
bool cGangManager::GangBrawl(sGang* gang1, sGang* gang2, bool rivalVrival)
{
	if (!gang1 || gang1->members() < 1) return false;	// gang1 does not exist
	if (!gang2 || gang2->members() < 1) return true;	// gang2 does not exist

	// Player's gang or first gang if rivalVrival = true
	gang1->m_Combat = true;
	u_int g1attack = SKILL_COMBAT;
	int initalNumber1 = gang1->members();
	int g1dodge = gang1->m_Stats[STAT_AGILITY];
	int g1SwordLevel = (rivalVrival ? min(5, (g_Dice % (gang1->m_Stats[SKILL_COMBAT] / 20) + 1)) : m_SwordLevel);

	gang2->m_Combat = true;
	u_int g2attack = SKILL_COMBAT;
	int initalNumber2 = gang2->members();
	int g2dodge = gang2->m_Stats[STAT_AGILITY];
	int g2SwordLevel = min(5, (g_Dice % (gang2->m_Stats[SKILL_COMBAT] / 20) + 1));

	int tmp = std::max(gang1->members(), gang2->members());	// get the largest gang's number

	for (int i = 0; i < tmp; i++)						// for each gang member in the largest gang
	{
		int g1Health = 100;		int g1Mana = 100;
		int g2Health = 100;		int g2Mana = 100;
		g1attack = SKILL_MAGIC;	g2attack = SKILL_MAGIC;

		while (g1Health > 0 && g2Health > 0)
		{
			// set what they attack with
			g1attack = (g1Mana <= 0 ? SKILL_COMBAT : SKILL_MAGIC);
			g2attack = (g2Mana <= 0 ? SKILL_COMBAT : SKILL_MAGIC);

			// gang1 attacks
			if (g1attack == SKILL_MAGIC) g1Mana -= 7;	// spend the mana before attacking
			if (g_Dice.percent(gang1->m_Skills[g1attack]))
			{
				int damage = (g1SwordLevel + 1) * max(1, gang1->strength() / 10);
				if (g1attack == SKILL_MAGIC)
				{
					damage += gang1->m_Skills[SKILL_MAGIC] / 10 + 3;
				}

				// gang 2 attempts Dodge
				if (!g_Dice.percent(g2dodge))
				{
					damage = max(1, (damage - (gang2->m_Stats[STAT_CONSTITUTION] / 15)));
					g2Health -= damage;
				}
			}

			// gang2 use healing potions
			if (gang2->num_potions() > 0 && g2Health <= 40)
			{
			    g2Health += 30;
			    gang2->use_potion();
			}

			// gang2 Attacks
			if (g2attack == SKILL_MAGIC) g2Mana -= 7;	// spend the mana before attacking
			if (g_Dice.percent(gang2->m_Skills[g2attack]))
			{
				int damage = (g2SwordLevel + 1) * max(1, gang2->strength() / 10);
				if (g2attack == SKILL_MAGIC)
				{
					damage += gang2->m_Skills[SKILL_MAGIC] / 10 + 3;
				}

				if (!g_Dice.percent(g1dodge))
				{
					damage = max(1, (damage - (gang1->m_Stats[STAT_CONSTITUTION] / 15)));
					g1Health -= damage;
				}
			}

			// gang1 use healing potions
			if (gang1->num_potions() > 0 && g1Health <= 40)
			{
				gang1->use_potion();
				g1Health += 30;
			}

			g1dodge = max(0, g1dodge - 1);		// degrade gang1 dodge ability
			g2dodge = max(0, g2dodge - 1);		// degrade gang2 dodge ability
		}

		if (g2Health <= 0) gang2->m_Num--;
		if (gang2->m_Num == 0)
		{
			gang1->BoostCombatSkills(3);	// win by KO, boost 3 skills
			return true;
		}

		if (g1Health <= 0) gang1->m_Num--;
		if (gang1->m_Num == 0)
		{
            gang2->BoostCombatSkills(3);	// win by KO, boost 3 skills
			return false;
		}

		if ((initalNumber2 / 2) > gang2->m_Num)	// if the gang2 has lost half its number there is a 40% chance they will run away
		{
			if (g_Dice.percent(40))
			{
                gang1->BoostCombatSkills(2);	// win by runaway, boost 2 skills
				return true;	// the men run away
			}
		}

		if ((initalNumber1 / 2) > gang1->m_Num)	// if the gang has lost half its number there is a 40% chance they will run away
		{
			if (g_Dice.percent(40))
			{
                gang2->BoostCombatSkills(2);	// win by runaway, boost 2 skills
				return false;	// the men run away
			}
		}
	}

	return false;
}

bool cGangManager::GangCombat(sGirl* girl, sGang* gang)
{
	CLog l;

	// MYR: Sanity check: Incorporeal is an auto-win.
	if (girl->has_trait("Incorporeal"))
	{
		girl->m_Stats[STAT_HEALTH] = 100;
		l.ss() << "\nGirl vs. Goons: " << girl->m_Realname << " is incorporeal, so she wins.\n";
		gang->m_Num = (int)gang->m_Num / 2;
		while (gang->m_Num > 0)   // Do the casualty calculation
		{
			if (g_Dice.percent(40))	gang->m_Num--;
			else break;
		}
		l.ss() << "  " << gang->members() << " goons escaped with their lives.\n";
		return true;
	}

	if (gang == nullptr || gang->m_Num == 0) return true;

	int dodge = 0;
	u_int attack = SKILL_COMBAT;	// determined later, defaults to combat
	u_int gattack = SKILL_COMBAT;

	int initalNumber = gang->m_Num;

	attack = (girl->combat() >= girl->magic()) ? SKILL_COMBAT : SKILL_MAGIC;	// first determine what she will fight with
	gattack = (gang->combat() >= gang->magic()) ? SKILL_COMBAT : SKILL_MAGIC;		// determine how gang will fight

	dodge = max(0, (girl->agility()) - girl->tiredness());

	int num_goons = gang->m_Num;
	gang->m_Combat = true;

	/*
	*	don't let a gang use up more than their
	*	fair share in any one combat
	*
	*	limit is recalcualted each time on the number
	*	of potions remaining, restock is at end-of-turn
	*
	*	this means that gangs in combats later in the turn
	*	have fewer potions available.
	*/

	l.ss() << "Girl vs. Goons: " << girl->m_Realname << " fights " << num_goons << " opponents!";
	l.ss() << girl->m_Realname << ": Health " << girl->health() << ", Dodge " << dodge << ", Mana " << girl->mana();
	l.ssend();

	for (int i = 0; i<num_goons; i++)
	{
		int gHealth = 100;
		int gDodge = gang->m_Stats[STAT_AGILITY];
		int gMana = 100;

		l.ss() << "	Goon #" << i << ": Health 100, Dodge " << gDodge << ", Mana ";
		l.ssend();

		while (girl->health() >= 20 && gHealth > 0)
		{
			// Girl attacks
			l.ss() << "\t\t" << girl->m_Realname << " attacks the goon.";
			l.ssend();

			if (attack == SKILL_MAGIC)
			{
				if (girl->mana() < 7)
				{
					l.ss() << "\t\t" << girl->m_Realname << " insufficient mana: using combat";
					l.ssend();
				}
				else
				{
					girl->mana(-7);
					l.ss() << "\t\t" << girl->m_Realname << " casts a spell; mana now " << girl->mana();
					l.ssend();
				}
			}
			else
			{
				l.ss() << "\t\t" << girl->m_Realname << " using physical attack";
				l.ssend();
			}

			int girl_attack_chance = girl->get_skill(attack);
			int die_roll = g_Dice.d100();

			l.ss() << "\t\t" << " attack chance = " << girl_attack_chance;
			l.ssend();

			l.ss() << "\t\t" << " die roll = " << die_roll;
			l.ssend();

			if (die_roll > girl_attack_chance)
			{
				l.ss() << "\t\t\t" << " attack fails";
			}
			else
			{
				int damage = cGirls::GetCombatDamage(girl, attack);
				l.ss() << "\t\t\t" << " attack hits! base damage is" << damage << "."; l.ssend();

				/*
				*				she may improve a little
				*				(checked every round of combat? seems excessive)
				*/
				int gain = g_Dice % 2;
				if (gain)
				{
					l.ss() << "\t\t" << girl->m_Realname << " gains +" << gain << " to attack skill";
					l.ssend();
					girl->upd_skill(attack, gain);
				}

				die_roll = g_Dice.d100();

				// Goon attempts Dodge
				l.ss() << "\t\t" << "Goon tries to dodge: needs " << gDodge << ", gets " << die_roll << ": ";
				l.ssend();

				if (die_roll <= gDodge)
				{
					l.ss() << "\t\t" << "success!";
				}
				else
				{
					int con_mod = gang->m_Stats[STAT_CONSTITUTION] / 10;
					gHealth -= con_mod;
					l.ss() << "\t\t" << "failure!\n"
						<< "\t\t" << "Goon takes " << damage << " damage, less " << con_mod << " for CON\n"
						<< "\t\t" << "New health value = " << con_mod;
				}
			}
			l.ssend();

			// goons use healing potions
			if (gang->num_potions() > 0 && gHealth <= 40)
			{
			    gang->use_potion();
				gHealth += 30;
				l.ss() << "Goon drinks healing potion: new health value = " << gHealth << ". Gang has " << gang->num_potions() << " remaining.";
				l.ssend();
			}

			// Goon Attacks

			die_roll = g_Dice.d100();
			int goon_attack_chance = gang->m_Skills[gattack];
			l.ss() << "\tGoon Attack: ";
			l.ssend();

			l.ss() << "\t\t" << "chance = " << goon_attack_chance << ", die roll = " << die_roll << ": ";
			if (die_roll > goon_attack_chance)
			{
				l.ss() << " attack fails!";
				l.ssend();
			}
			else
			{
				l.ss() << " attack succeeds!";
				l.ssend();

				int damage = (m_SwordLevel + 1) * max(1, gang->strength() / 10);
				if (gattack == SKILL_MAGIC)
				{
					if (gMana <= 0)
						gattack = SKILL_COMBAT;
					else
					{
						damage += 10;
						gMana -= 7;
					}
				}

				// girl attempts Dodge
				if (!g_Dice.percent(dodge))
				{
					damage = max(1, (damage - (girl->constitution() / 15)));
					girl->health(-damage);
				}
			}

			dodge = max(0, (dodge - 1));	// degrade girls dodge ability
			gDodge = max(0, (gDodge - 1));	// degrade goons dodge ability

			if (girl->health() < 30 && girl->health() > 20)
			{
				if (g_Dice.percent(girl->agility()))
				{
                    gang->BoostCombatSkills(2);
					girl->upd_Enjoyment(ACTION_COMBAT, -1);
					return false;
				}
			}
		}

		if (girl->health() <= 20)
		{
            gang->BoostCombatSkills(2);
			girl->upd_Enjoyment(ACTION_COMBAT, -1);
			return false;
		}
		else
			gang->m_Num--;

		if ((initalNumber / 2) > gang->m_Num)	// if the gang has lost half its number there is a 40% chance they will run away
		{
			if (g_Dice.percent(40))
			{
				girl->upd_Enjoyment(ACTION_COMBAT, +1);
				return true;	// the men run away
			}
		}
		if (gang->m_Num == 0)
		{
			girl->upd_Enjoyment(ACTION_COMBAT, +1);
			return true;
		}
	}

	l.ss() << "No more opponents: " << girl->m_Realname << " WINS!";
	l.ssend();

	girl->upd_Enjoyment(ACTION_COMBAT, +1);

	return true;
}

// MYR: This is similar to GangCombat, but instead of one of the players gangs
//      fighting the girl, some random gang attacks her.  This random gang
//      doesn't have healing potions and the weapon levels of a player gang.
//      ATM only the new security code uses it.
//      This will also be needed to be updated to the new way of doing combat.
// true means the girl won

bool cGangManager::GirlVsEnemyGang(sGirl* girl, sGang* enemy_gang)
{
	CLog l;

	// MYR: Sanity check: Incorporeal is an auto-win.
	if (girl->has_trait("Incorporeal"))
	{
		girl->m_Stats[STAT_HEALTH] = 100;
		l.ss() << "\nGirl vs. Goons: " << girl->m_Realname << " is incorporeal, so she wins.\n";
		enemy_gang->m_Num = (int)enemy_gang->m_Num / 2;
		while (enemy_gang->m_Num > 0)   // Do the casualty calculation
		{
			if (g_Dice.percent(40)) enemy_gang->m_Num--;
			else break;
		}
		l.ss() << "  " << enemy_gang->m_Num << " goons escaped with their lives.\n";
		return true;
	}

	int dodge = girl->agility();  // MYR: Was 0
	int mana = girl->mana();      // MYR: Like agility, mana is now per battle

	u_int attack = SKILL_COMBAT;	// determined later, defaults to combat
	u_int goon_attack = SKILL_COMBAT;

	if (enemy_gang == nullptr)
		return true;
	else if (enemy_gang->m_Num == 0)
		return true;

	// first determine what she will fight with
	if (girl->combat() > girl->magic())
		attack = SKILL_COMBAT;
	else
		attack = SKILL_MAGIC;

	// determine how gang will fight
	if (enemy_gang->m_Skills[SKILL_COMBAT] > enemy_gang->m_Skills[SKILL_MAGIC])
		goon_attack = SKILL_COMBAT;
	else
		goon_attack = SKILL_MAGIC;

	int initial_num = enemy_gang->m_Num;

	enemy_gang->m_Combat = true;


	l.ss() << "\nGirl vs. Goons: " << girl->m_Realname << " fights " << initial_num << " opponents!";
	l.ssend();
	l.ss() << girl->m_Realname << ": Health " << girl->health() << ", Dodge " << girl->agility()
		<< ", Mana " << girl->mana();
	l.ssend();

	for (int i = 0; i< initial_num; i++)
	{
		l.ss() << "Goon #" << i << ": Health: " << (int)enemy_gang->m_Stats[STAT_HEALTH] << " Mana: "
			<< (int)enemy_gang->m_Stats[STAT_MANA] << " Dodge: " << (int)enemy_gang->m_Stats[STAT_AGILITY]
			<< " Attack: " << (int)enemy_gang->m_Skills[goon_attack] << " Constitution: " << (int)enemy_gang->m_Stats[STAT_CONSTITUTION];
		l.ssend();

		int gHealth = enemy_gang->m_Stats[STAT_HEALTH];
		int gDodge = enemy_gang->m_Stats[STAT_AGILITY];
		int gMana = enemy_gang->m_Stats[STAT_MANA];

		while (girl->health() >= 20 && gHealth > 0)
		{
			// Girl attacks
			//l.ss()	<< "\t" << girl->m_Realname << " attacks the goon.";
			//l.ssend();

			if (attack == SKILL_MAGIC) {

				if (mana < 5) {
					attack = SKILL_COMBAT;
					//l.ss() << "\t\t" << girl->m_Realname << " insufficient mana: using combat";
					//l.ssend();
				}
				else {

					mana = mana - 5;
					//l.ss() << "\t\t" << girl->m_Realname << " casts a spell; mana now " << mana;
					//l.ssend();
				}
			}
			else {
				//l.ss() << "\t\t" << girl->m_Realname << " using physical attack";
				//l.ssend();
			}

			int girl_attack_chance = girl->get_skill(attack);

			int die_roll = g_Dice.d100();

			//l.ss() << "\t\t" << " attack chance: " << girl_attack_chance << "\t\t" << " die roll:" << die_roll;
			//l.ssend();

			if (die_roll > girl_attack_chance) {
				//l.ss() << "\t\t\t" << " attack misses";
			}
			else {
				int damage = cGirls::GetCombatDamage(girl, attack);

				die_roll = g_Dice.d100();

				// Goon attempts Dodge
				//l.ss() << "\t\t" << "Goon tries to dodge: needs " << gDodge << ", gets " << die_roll << ": ";
				//l.ssend();

				// Dodge maxes out at 95%
				if (die_roll <= gDodge && die_roll <= 95) {
					//l.ss() << "\t\t" << "success!";
				}
				else {
					int con_mod = enemy_gang->m_Stats[STAT_CONSTITUTION] / 20;
					damage -= con_mod;
					if (damage <= 0)  // MYR: Minimum 1 damage on hit
						damage = 1;
					gHealth -= damage;
					l.ss() << "\t\tGoon takes " << damage << ". New health value: " << gHealth;
					l.ssend();
				}
			}


			if (gHealth <= 0) // Goon may have been killed by damage above
				continue;

			// Goon Attacks

			die_roll = g_Dice.d100();
			//l.ss()	<< "\tGoon Attack: ";
			//l.ssend();


			//l.ss() << "\t\t" << "chance:" << (int) enemy_gang->m_Skills[goon_attack] << ", die roll:" << die_roll << ": ";

			if (die_roll > enemy_gang->m_Skills[goon_attack]) {
				//l.ss() << " attack fails!";
				//l.ssend();
			}
			else {
				//l.ss() << " attack succeeds!";
				//l.ssend();

				// MYR: Goon damage calculation is different from girl's.  Do we care?
				int damage = 5 + enemy_gang->m_Skills[goon_attack] / 10;

				if (goon_attack == SKILL_MAGIC)
				{
					if (gMana < 10)
						goon_attack = SKILL_COMBAT;
					else
					{
						damage += 8;
						gMana -= 10;
					}
				}

				// girl attempts Dodge
				die_roll = g_Dice.d100();

				//l.ss() << "\t\t" << girl->m_Realname << " tries to dodge: needs " << dodge << ", gets " 
				//	<< die_roll << ": ";
				//l.ssend();

				// MYR: Girl dodge maxes out at 90 (Gang dodge at 95).  It's a bit of a hack
				if (die_roll <= dodge && die_roll <= 90) {
					//l.ss() << "\t\t" << "success!";
					//l.ssend();
				}
				else
				{
					cGirls::TakeCombatDamage(girl, -damage); // MYR: Note change

					l.ss() << "\t" << girl->m_Realname << " takes " << damage << ". New health value: " << girl->health();
					if (girl->has_trait("Incorporeal")) l.ss() << " (Girl is Incorporeal)";
					l.ssend();
				}
			}

			// update girls dodge ability
			if ((dodge - 1) < 0)
				dodge = 0;
			else
				dodge--;

			// update goons dodge ability
			if ((gDodge - 1) < 0)
				gDodge = 0;
			else
				gDodge--;
		}  // While loop

		if (girl->health() <= 20)
		{
			l.ss() << "The gang overwhelmed and defeated " << girl->m_Realname << ". She lost the battle.";
			l.ssend();
			girl->upd_Enjoyment(ACTION_COMBAT, -5);
			return false;
		}
		else
			enemy_gang->m_Num--;  // Gang casualty

		// if the gang has lost half its number there is a chance they will run away
		// This is checked for every member killed over 50%
		if ((initial_num / 2) > enemy_gang->m_Num)
		{
			if (g_Dice.percent(50)) // MYR: Adjusting this has a big effect
			{
				l.ss() << "The gang ran away after losing too many members. " << girl->m_Realname << " WINS!";
				l.ssend();
				girl->upd_Enjoyment(ACTION_COMBAT, +5);
				return true;	// the men run away
			}
		}
		// Gang fought to the death
		if (enemy_gang->m_Num == 0)
		{
			l.ss() << "The gang fought to bitter end. They are all dead. " << girl->m_Realname << " WINS!";
			l.ssend();
			girl->upd_Enjoyment(ACTION_COMBAT, +5);
			return true;
		}
	}

	l.ss() << "No more opponents: " << girl->m_Realname << " WINS!";
	l.ssend();

	girl->upd_Enjoyment(ACTION_COMBAT, +5);

	return true;
}

// ----- Mission related

// Missions done here - Updated for .06.01.09
void cGangManager::UpdateGangs()
{
	if (cfg.debug.log_debug()) { g_LogFile.ss() << "Debug cGangManager::UpdateGangs() || Start"; g_LogFile.ssend(); }
	stringstream ss;

	// maintain recruitable gangs list, potentially pruning some old ones
    int remove_chance = cfg.gangs.chance_remove_unwanted();
    erase_if(m_HireableGangs,  [remove_chance](auto& gang) { return g_Dice.percent(remove_chance); });

	if (cfg.debug.log_debug()) { g_LogFile.ss() << "Debug cGangManager::UpdateGangs() || 1"; g_LogFile.ssend(); }
	// maybe add some new gangs to the recruitable list
	int add_min = cfg.gangs.add_new_weekly_min();
	int add_max = cfg.gangs.add_new_weekly_max();
	int add_recruits = g_Dice.bell(add_min, add_max);
	for (int i = 0; i < add_recruits; i++)
	{
		if (m_HireableGangs.size() >= cfg.gangs.max_recruit_list()) break;
		cerr << "Adding new recruitable gang." << endl;
		AddNewGang(false);
	}
	if (cfg.debug.log_debug()) { g_LogFile.ss() << "Debug cGangManager::UpdateGangs() || 2"; g_LogFile.ssend(); }

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
            if (cfg.debug.log_debug()) {
                g_LogFile.ss() << "Debug cGangManager::UpdateGangs() || bad mission " << currentGang->m_MissionID;
                g_LogFile.ssend();
            }
            stringstream sse;
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

	if (cfg.debug.log_debug()) { g_LogFile.ss() << "Debug cGangManager::UpdateGangs() || 3"; g_LogFile.ssend(); }


	g_Game->rivals().Update(m_BusinessesExtort);	// Update the rivals

	RestockNetsAndPots();
	if (cfg.debug.log_debug()) { g_LogFile.ss() << "Debug cGangManager::UpdateGangs() || end"; g_LogFile.ssend(); }

}

// `J` restock at the start and end of the gang shift - Added for .06.01.09
void cGangManager::RestockNetsAndPots()
{
	g_LogFile.ss() << "Time to restock heal potions and nets\n"
		<< "Heal Flag    = " << bool(m_KeepHealStocked > 0) << "\n"
		<< "Heal Target  = " << m_KeepHealStocked << "\n"
		<< "Heal Current = " << m_KeepHealStocked << "\n"
		<< "Nets Flag    = " << bool(m_KeepNetsStocked > 0) << "\n"
		<< "Nets Target  = " << m_KeepNetsStocked << "\n"
		<< "Nets Current = " << m_KeepNetsStocked;
	g_LogFile.ssend();
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
	g_LogFile.ss() << "cGangManager::chance_to_catch: " << gvec.size() << " gangs spying"; g_LogFile.ssend();	// bit of debug chatter
	for (auto& gang: gvec)		// whizz down the vector adding probability as we go
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
	if (cfg.debug.log_debug()) { g_LogFile.ss() << "Debug cGangManager::GangStartOfShift() || Start"; g_LogFile.ssend(); }
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
	if (cfg.debug.log_debug()) { g_LogFile.ss() << "Debug cGangManager::GangStartOfShift() || end"; g_LogFile.ssend(); }
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
    amount = std::min(amount, 200 - m_NumNets);

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
