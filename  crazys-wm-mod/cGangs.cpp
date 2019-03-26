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
#include "cBrothel.h"
#include "cJobManager.h"
#include "cCustomers.h"
#include "cGirls.h"
#include "cMessageBox.h"
#include "GameFlags.h"
#include "CGraphics.h"
#include "sConfig.h"
#include "cTariff.h"
#include "XmlMisc.h"
#include "libintl.h"

#ifdef LINUX
#include "linux.h"
#endif
#include "DirPath.h"

extern cMessageQue g_MessageQue;
extern cCustomers g_Customers;
extern cGirls g_Girls;
extern cInventory g_InvManager;
extern cBrothelManager g_Brothels;
extern unsigned long g_Year;
extern unsigned long g_Month;
extern unsigned long g_Day;
extern CGraphics g_Graphics;
extern cRng g_Dice;
extern cGold g_Gold;
extern cPlayer* The_Player;

static cRivalManager* m_Rivals = g_Brothels.GetRivalManager();
static cDungeon* m_Dungeon = g_Brothels.GetDungeon();


cGangManager::cGangManager()
{
	m_NumGangNames = 0;
	ifstream in;
	// WD: Typecast to resolve ambiguous call in VS 2010
	DirPath dp = DirPath() << "Resources" << "Data" << "HiredGangNames.txt";
	in.open(dp.c_str());
	in >> m_NumGangNames;
	in.close();
	m_NumGangs = 0;
	m_GangStart = m_GangEnd = 0;
	m_NumHireableGangs = 0;
	m_HireableGangStart = m_HireableGangEnd = 0;
	m_BusinessesExtort = 0;
	m_NumHealingPotions = m_NumNets = m_SwordLevel = 0;
	m_KeepHealStocked = m_KeepNetsStocked = 0;
	m_Control_Gangs = false;
	m_Gang_Gets_Girls = m_Gang_Gets_Items = m_Gang_Gets_Beast = 0;

}

void cGangManager::Free()
{
	if (m_GangStart) delete m_GangStart;
	m_NumGangs = 0;
	m_GangStart = m_GangEnd = 0;
	if (m_HireableGangStart) delete m_HireableGangStart;
	m_NumHireableGangs = 0;
	m_HireableGangStart = m_HireableGangEnd = 0;
	m_BusinessesExtort = 0;
	m_NumHealingPotions = m_SwordLevel = m_NumNets = 0;
	m_KeepHealStocked = m_KeepNetsStocked = 0;
	m_Control_Gangs = false;
	m_Gang_Gets_Girls = m_Gang_Gets_Items = m_Gang_Gets_Beast = 0;
}

bool cGangManager::LoadGangsXML(TiXmlHandle hGangManager)
{
	Free();							//everything should be init even if we failed to load an XML element
	TiXmlElement* pGangManager = hGangManager.ToElement();
	if (pGangManager == 0) return false;

	m_NumGangs = 0;					// load goons and goon missions
	TiXmlElement* pGangs = pGangManager->FirstChildElement("Gangs");
	if (pGangs)
	{
		for (TiXmlElement* pGang = pGangs->FirstChildElement("Gang"); pGang != 0; pGang = pGang->NextSiblingElement("Gang"))
		{
			sGang* gang = new sGang();
			bool success = gang->LoadGangXML(TiXmlHandle(pGang));
			if (success) { AddGang(gang); }
			else { delete gang; continue; }
		}
	}
	m_NumHireableGangs = 0;			// load hireable goons
	TiXmlElement* pHireables = pGangManager->FirstChildElement("Hireables");
	if (pHireables)
	{
		for (TiXmlElement* pGang = pHireables->FirstChildElement("Gang"); pGang != 0; pGang = pGang->NextSiblingElement("Gang"))
		{
			sGang* hgang = new sGang();
			bool success = hgang->LoadGangXML(TiXmlHandle(pGang));
			if (success) { AddHireableGang(hgang); }
			else { delete hgang; continue; }
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
	sGang* gang = m_GangStart;
	while (gang)
	{
		TiXmlElement* pGang = gang->SaveGangXML(pGangs);
		pGang->SetAttribute("MissionID", gang->m_MissionID);
		pGang->SetAttribute("LastMissID", gang->m_LastMissID);
		pGang->SetAttribute("Combat", gang->m_Combat);
		pGang->SetAttribute("AutoRecruit", gang->m_AutoRecruit);
		gang = gang->m_Next;
	}
	TiXmlElement* pHireables = new TiXmlElement("Hireables");
	pGangManager->LinkEndChild(pHireables);
	sGang* hgang = m_HireableGangStart;
	while (hgang)
	{
		hgang->SaveGangXML(pHireables);
		hgang = hgang->m_Next;
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
	return pGang;
}

bool sGang::LoadGangXML(TiXmlHandle hGang)
{
	TiXmlElement* pGang = hGang.ToElement();
	if (pGang == 0) return false;
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

	return true;
}

// ----- Hire fire

void cGangManager::HireGang(int gangID)
{
	sGang* currentGang = m_HireableGangStart;
	int count = 0;
	while (currentGang)
	{
		if (count == gangID) break;
		count++;
		currentGang = currentGang->m_Next;
	}
	if (currentGang)
	{
		sGang* copyGang = new sGang();
		*copyGang = *currentGang;
		copyGang->m_Next = copyGang->m_Prev = 0;
		copyGang->m_Combat = copyGang->m_AutoRecruit = false;
		copyGang->m_LastMissID = -1;
		if (copyGang->m_Num <= 5) copyGang->m_MissionID = MISS_RECRUIT;
		AddGang(copyGang);
		RemoveHireableGang(gangID);
	}
}

void cGangManager::FireGang(int gangID)
{
	sGang* currentGang = m_GangStart;
	int count = 0;
	while (currentGang)
	{
		if (count == gangID)	break;
		count++;
		currentGang = currentGang->m_Next;
	}
	if (currentGang)
	{
		if (m_NumHireableGangs < cfg.gangs.max_recruit_list())
		{
			sGang* copyGang = new sGang();
			*copyGang = *currentGang;
			copyGang->m_Next = copyGang->m_Prev = 0;
			copyGang->m_Combat = copyGang->m_AutoRecruit = false;
			copyGang->m_LastMissID = -1;
			AddHireableGang(copyGang);
		}
		RemoveGang(gangID);
	}
}

// ----- Add remove

void cGangManager::AddNewGang(bool boosted)
{
	m_NumHireableGangs++;
	sGang* newGang = new sGang();

	int max_members = cfg.gangs.init_member_max();
	int min_members = cfg.gangs.init_member_min();
	newGang->m_Num = min_members + g_Dice % (max_members + 1 - min_members);
	if (boosted) newGang->m_Num = min(15, newGang->m_Num + 5);

	int new_val;
	for (u_int i = 0; i<NUM_SKILLS; i++)
	{
		new_val = (g_Dice % 30) + 21;
		if (g_Dice % 5 == 1)	new_val += 1 + g_Dice % 10;
		if (boosted)			new_val += 10 + g_Dice % 11;
		newGang->m_Skills[i] = new_val;
	}
	for (int i = 0; i<NUM_STATS; i++)
	{
		new_val = (g_Dice % 30) + 21;
		if (g_Dice % 5 == 1)	new_val += g_Dice % 10;
		if (boosted)			new_val += 10 + g_Dice % 11;
		newGang->m_Stats[i] = new_val;
	}
	newGang->m_Stats[STAT_HEALTH] = 100;
	newGang->m_Stats[STAT_HAPPINESS] = 100;

	char buffer[256];
	bool done = false;
	ifstream in;
	// WD: Typecast to resolve ambiguous call in VS 2010
	DirPath dp = DirPath() << "Resources" << "Data" << "HiredGangNames.txt";
	in.open(dp.c_str());
	//in.open(DirPath() <<	"Resources" << "Data" << "HiredGangNames.txt");
	while (!done)
	{
		in.seekg(0);
		int name = g_Dice%m_NumGangNames;
		in >> m_NumGangNames;	// ignore the first line
		for (int i = 0; i <= name; i++)
		{
			if (in.peek() == '\n') in.ignore(1, '\n');
			in.getline(buffer, sizeof(buffer), '\n');
		}
		done = true;
		sGang* curr = m_GangStart;
		while (curr)
		{
			if (curr->m_Name == buffer)
			{
				done = false;
				break;
			}
			curr = curr->m_Next;
		}
		curr = m_HireableGangStart;
		while (curr)
		{
			if (curr->m_Name == buffer)
			{
				done = false;
				break;
			}
			curr = curr->m_Next;
		}
	}
	newGang->m_Name = buffer;
	in.close();

	if (m_HireableGangStart)
	{
		m_HireableGangEnd->m_Next = newGang;
		newGang->m_Prev = m_HireableGangEnd;
		m_HireableGangEnd = newGang;
	}
	else
		m_HireableGangStart = m_HireableGangEnd = newGang;
}

void cGangManager::AddHireableGang(sGang* newGang)
{
	m_NumHireableGangs++;
	if (m_HireableGangStart)
	{
		m_HireableGangEnd->m_Next = newGang;
		newGang->m_Prev = m_HireableGangEnd;
		m_HireableGangEnd = newGang;
	}
	else
		m_HireableGangStart = m_HireableGangEnd = newGang;
}

void cGangManager::AddGang(sGang* newGang)
{
	m_NumGangs++;
	if (m_GangStart)
	{
		m_GangEnd->m_Next = newGang;
		newGang->m_Prev = m_GangEnd;
		m_GangEnd = newGang;
	}
	else
		m_GangStart = m_GangEnd = newGang;
}

void cGangManager::RemoveHireableGang(int gangID)
{
	// find the gang by id
	sGang* currentGang = m_HireableGangStart;
	int count = 0;
	while (currentGang)
	{
		if (count == gangID) break;
		count++;
		currentGang = currentGang->m_Next;
	}

	RemoveHireableGang(currentGang);
}

void cGangManager::RemoveHireableGang(sGang* gang)
{
	if (gang)
	{
		if (gang->m_Prev == 0)
		{
			m_HireableGangStart = gang->m_Next;
			if (m_HireableGangStart)
				m_HireableGangStart->m_Prev = 0;
			else
				m_HireableGangEnd = 0;
			gang->m_Next = 0;
			delete gang;
			gang = 0;
		}
		else if (gang->m_Next == 0)
		{
			m_HireableGangEnd = gang->m_Prev;
			m_HireableGangEnd->m_Next = 0;
			delete gang;
			gang = 0;
		}
		else
		{
			gang->m_Next->m_Prev = gang->m_Prev;
			gang->m_Prev->m_Next = gang->m_Next;
			gang->m_Next = 0;
			delete gang;
			gang = 0;
		}
		m_NumHireableGangs--;
	}
}

void cGangManager::RemoveGang(sGang* gang)
{
	if (gang)
	{
		if (gang->m_Prev == 0)
		{
			m_GangStart = gang->m_Next;
			if (m_GangStart)
				m_GangStart->m_Prev = 0;
			else
				m_GangEnd = 0;
			gang->m_Next = 0;
			delete gang;
			gang = 0;
		}
		else if (gang->m_Next == 0)
		{
			m_GangEnd = gang->m_Prev;
			m_GangEnd->m_Next = 0;
			delete gang;
			gang = 0;
		}
		else
		{
			gang->m_Next->m_Prev = gang->m_Prev;
			gang->m_Prev->m_Next = gang->m_Next;
			gang->m_Next = 0;
			delete gang;
			gang = 0;
		}
		m_NumGangs--;
	}
}

void cGangManager::RemoveGang(int gangID)
{
	// find the gang by id
	sGang* currentGang = m_GangStart;
	int count = 0;
	while (currentGang)
	{
		if (count == gangID) break;
		count++;
		currentGang = currentGang->m_Next;
	}

	// Pass the acquired gang to the other RemoveGang function
	RemoveGang(currentGang);
}

// ----- Get

int cGangManager::GetNumGangs()
{
	return m_NumGangs;
}

int cGangManager::GetMaxNumGangs()
{
	m_MaxNumGangs = 7 + g_Brothels.GetNumBrothels();
	return m_MaxNumGangs;
}

int cGangManager::GetNumHireableGangs()
{
	return m_NumHireableGangs;
}

sGang* cGangManager::GetTempGang()
{
	sGang* newGang = new sGang();
	newGang->m_Num = g_Dice % 6 + 10;
	for (int i = 0; i<NUM_SKILLS; i++)	newGang->m_Skills[i] = (g_Dice % 30) + 21;
	for (int i = 0; i<NUM_STATS; i++)	newGang->m_Stats[i] = (g_Dice % 30) + 21;
	newGang->m_Stats[STAT_HEALTH] = 100;
	newGang->m_Stats[STAT_HAPPINESS] = 100;
	return newGang;
}
// `J` added temp gang mod - base strength + mod
sGang* cGangManager::GetTempGang(int mod)
{
	sGang* newGang = new sGang();
	newGang->m_Num = min(15, g_Dice.bell(6, 18));
	for (int i = 0; i < NUM_SKILLS; i++)
	{
		newGang->m_Skills[i] = (g_Dice % 40) + 21 + (g_Dice % mod);
		if (newGang->m_Skills[i] < 1)	newGang->m_Skills[i] = 1;
		if (newGang->m_Skills[i] > 100)	newGang->m_Skills[i] = 100;
	}
	for (int i = 0; i < NUM_STATS; i++)
	{
		newGang->m_Stats[i] = (g_Dice % 40) + 21 + (g_Dice % mod);
		if (newGang->m_Stats[i] < 1)	newGang->m_Stats[i] = 1;
		if (newGang->m_Stats[i] > 100)	newGang->m_Stats[i] = 100;
	}
	newGang->m_Stats[STAT_HEALTH] = 100;
	newGang->m_Stats[STAT_HAPPINESS] = 100;

	return newGang;
}

sGang* cGangManager::GetGang(int gangID)
{
	// first find the gang
	sGang* currentGang = m_GangStart;
	int count = 0;
	while (currentGang)
	{
		if (count == gangID)	break;
		count++;
		/*
		*		no next gang surely means the end of the list
		*		so therefore "gang not found"
		*
		*		or has the way it works changed?
		*
		*		Anyway, all there was where was a local variable
		*		x which was set to 1 and then ignored
		*
		*		-- doc
		*/
		if (currentGang->m_Next == 0)
		{//uh oh, bad pointer.... crap
			return 0;
		}
		currentGang = currentGang->m_Next;
	}
	return currentGang;
}

sGang* cGangManager::GetHireableGang(int gangID)
{
	// first find the gang
	sGang* currentGang = m_HireableGangStart;
	int count = 0;
	while (currentGang)
	{
		if (count == gangID)	break;
		count++;
		if (currentGang->m_Next == 0)
		{//uh oh, bad pointer.... crap
			//int x = 1;
			return 0;	// gang not found? -- doc
		}
		currentGang = currentGang->m_Next;
	}

	return currentGang;
}

sGang* cGangManager::GetTempWeakGang()
{
	// MYR: Weak gangs attack girls when they work
	sGang* newGang = new sGang();
	newGang->m_Num = 15;
	for (u_int i = 0; i<NUM_SKILLS; i++)	newGang->m_Skills[i] = g_Dice % 30 + 51;
	for (int i = 0; i<NUM_STATS; i++)		newGang->m_Stats[i] = g_Dice % 30 + 51;
	newGang->m_Stats[STAT_HEALTH] = 100;
	return newGang;
}

sGang *cGangManager::random_gang(vector<sGang*>& v)
{
	vector<sGang*> list;
	/*
	*	we're going to make a table that has one
	*	row for each gang member, and each row will
	*	contain a pointer to that member's gang
	*/
	for (u_int i = 0; i < v.size(); i++)
	{
		sGang *gang = v[i];
		for (int j = 0; j < gang->m_Num; j++)
		{
			list.push_back(gang);
		}
	}
	/*
	*	make sure the list isn't empty
	*/
	if (list.size() == 0) 	return 0;
	/*
	*	now we randomly select from the list.
	*
	*	this means the chance of a gang getting chosen
	*	is proportional to the number of gang members
	*/
	return list[g_Dice.random(list.size())];
}

// ----- Gang boosts

void cGangManager::BoostGangCombatSkills(sGang* gang, int count)
{  // simple function to increase a gang's combat skills a bit
	vector<int*> possible_skills;
	possible_skills.push_back(&gang->m_Skills[SKILL_COMBAT]);
	possible_skills.push_back(&gang->m_Skills[SKILL_MAGIC]);
	possible_skills.push_back(&gang->m_Stats[STAT_AGILITY]);
	possible_skills.push_back(&gang->m_Stats[STAT_CONSTITUTION]);
	BoostGangRandomSkill(&possible_skills, count, 1);
	possible_skills.clear();
}

void cGangManager::BoostGangRandomSkill(vector<int*>* possible_skills, int count, int boost_count)
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
		int *affect_skill = 0;
		int total_chance = 0;
		vector<int> chance;

		for (int i = 0; i < (int)possible_skills->size(); i++)
		{  // figure chances for each skill/stat; more likely to choose those they're better at
			chance.push_back((int)pow((float)*possible_skills->at(i), 2));
			total_chance += chance[i];
		}
		int choice = g_Dice.random(total_chance);

		total_chance = 0;
		for (int i = 0; i < (int)chance.size(); i++)
		{
			if (choice < (chance[i] + total_chance))
			{
				affect_skill = possible_skills->at(i);
				break;
			}
			total_chance += chance[i];
		}
		/*
		*	OK, we've picked a skill/stat. Now to boost it however many times were specified
		*/
		BoostGangSkill(affect_skill, boost_count);
	}
}

void cGangManager::BoostGangSkill(int* affect_skill, int count)
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

void sGang::AdjustGangSkill(sGang* gang, int Skill, int amount)	// `J` added for .06.02.41
{
	gang->m_Skills[Skill] += amount;
	if (gang->m_Skills[Skill] < 0)		gang->m_Skills[Skill] = 0;
	if (gang->m_Skills[Skill] > 100)	gang->m_Skills[Skill] = 100;
}
void sGang::AdjustGangStat(sGang* gang, int Stat, int amount)	// `J` added for .06.02.41
{
	gang->m_Stats[Stat] += amount;
	if (gang->m_Stats[Stat] < 0)		gang->m_Stats[Stat] = 0;
	if (gang->m_Stats[Stat] > 100)	gang->m_Stats[Stat] = 100;
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

// ----- Combat

/*	GangBrawl - returns true if gang1 wins and false if gang2 wins
*	If the Player's gang is in the fight, make sure it is the first gang
*	If two Rivals are fighting set rivalVrival to true
*/
bool cGangManager::GangBrawl(sGang* gang1, sGang* gang2, bool rivalVrival)
{
	if (!gang1 || gang1->m_Num < 1) return false;	// gang1 does not exist
	if (!gang2 || gang2->m_Num < 1) return true;	// gang2 does not exist

	cTariff tariff;
	// Player's gang or first gang if rivalVrival = true
	gang1->m_Combat = true;
	u_int g1attack = SKILL_COMBAT;
	int initalNumber1 = gang1->m_Num;
	int g1dodge = gang1->m_Stats[STAT_AGILITY];
	if (rivalVrival) gang1->m_Heal_Limit = 10;
	int g1SwordLevel = (rivalVrival ? min(5, (g_Dice % (gang1->m_Stats[SKILL_COMBAT] / 20) + 1)) : m_SwordLevel);

	gang2->m_Combat = true;
	u_int g2attack = SKILL_COMBAT;
	int initalNumber2 = gang2->m_Num;
	int g2dodge = gang2->m_Stats[STAT_AGILITY];
	gang2->m_Heal_Limit = 10;
	int g2SwordLevel = min(5, (g_Dice % (gang2->m_Stats[SKILL_COMBAT] / 20) + 1));

	int tmp = (gang1->m_Num > gang2->m_Num) ? gang1->m_Num : gang2->m_Num;	// get the largest gang's number

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
			if (gang2->heal_limit() > 0 && g2Health <= 40)
			{
				gang2->heal_limit(-1);
				g2Health += 30;
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
			if (gang1->heal_limit() > 0 && g1Health <= 40)
			{
				gang1->heal_limit(-1);
				if (!rivalVrival) m_NumHealingPotions--;
				g1Health += 30;
			}

			g1dodge = max(0, g1dodge - 1);		// degrade gang1 dodge ability
			g2dodge = max(0, g2dodge - 1);		// degrade gang2 dodge ability
		}

		if (g2Health <= 0) gang2->m_Num--;
		if (gang2->m_Num == 0)
		{
			BoostGangCombatSkills(gang1, 3);	// win by KO, boost 3 skills
			return true;
		}

		if (g1Health <= 0) gang1->m_Num--;
		if (gang1->m_Num == 0)
		{
			BoostGangCombatSkills(gang2, 3);	// win by KO, boost 3 skills
			return false;
		}

		if ((initalNumber2 / 2) > gang2->m_Num)	// if the gang2 has lost half its number there is a 40% chance they will run away
		{
			if (g_Dice.percent(40))
			{
				BoostGangCombatSkills(gang1, 2);	// win by runaway, boost 2 skills
				return true;	// the men run away
			}
		}

		if ((initalNumber1 / 2) > gang1->m_Num)	// if the gang has lost half its number there is a 40% chance they will run away
		{
			if (g_Dice.percent(40))
			{
				BoostGangCombatSkills(gang2, 2);	// win by runaway, boost 2 skills
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
		l.ss() << "  " << gang->m_Num << " goons escaped with their lives.\n";
		return true;
	}

	if (gang == 0 || gang->m_Num == 0) return true;

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
				int damage = g_Girls.GetCombatDamage(girl, attack);
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
			if (gang->heal_limit() > 0 && gHealth <= 40)
			{
				gang->heal_limit(-1);
				m_NumHealingPotions--;
				gHealth += 30;
				l.ss() << "Goon drinks healing potion: new health value = " << gHealth << ". Gang has " << gang->heal_limit() << " remaining.";
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
					BoostGangCombatSkills(gang, 2);
					girl->upd_Enjoyment(ACTION_COMBAT, -1);
					return false;
				}
			}
		}

		if (girl->health() <= 20)
		{
			BoostGangCombatSkills(gang, 2);
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

	if (enemy_gang == 0)
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
				int damage = g_Girls.GetCombatDamage(girl, attack);

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
					g_Girls.TakeCombatDamage(girl, -damage); // MYR: Note change

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

// `J` replaced with passing out of pots/nets in GangStartOfShift() for .06.01.09
int cGangManager::healing_limit()
{
	if (m_NumGangs < 1 || m_NumHealingPotions < 1) return 0;
	int limit;
	// take the number of potions and divide by the the number of gangs
	limit = m_NumHealingPotions / m_NumGangs;
	/*
	*	if that rounds to less than zero, and there are still
	*	potions available, make sure they get at least one to use
	*/
	if (limit < 1 && m_NumHealingPotions) limit = 1;
	return limit;
}

// `J` - Added for .06.01.09
int cGangManager::net_limit()
{
	if (m_NumGangs < 1 || m_NumNets < 1) return 0;
	int limit;
	// take the number of nets and divide by the the number of gangs
	limit = m_NumNets / m_NumGangs;
	/*
	*	if that rounds to less than zero, and there are still
	*	nets available, make sure they get at least one to use
	*/
	if (limit < 1 && m_NumNets) limit = 1;
	return limit;
}

// ----- Mission related

// Missions done here - Updated for .06.01.09
void cGangManager::UpdateGangs()
{
	if (cfg.debug.log_debug()) { g_LogFile.ss() << "Debug cGangManager::UpdateGangs() || Start"; g_LogFile.ssend(); }
	cTariff tariff;
	stringstream ss;

	// maintain recruitable gangs list, potentially pruning some old ones
	sGang* currentGang = m_HireableGangStart;
	int remove_chance = cfg.gangs.chance_remove_unwanted();
	while (currentGang)
	{
		if (g_Dice.percent(remove_chance))
		{
			cerr << "Culling recruitable gang: " << currentGang->m_Name << endl;
			sGang* temp = currentGang->m_Next;
			RemoveHireableGang(currentGang);
			currentGang = temp;
			continue;
		}
		currentGang = currentGang->m_Next;
	}
	if (cfg.debug.log_debug()) { g_LogFile.ss() << "Debug cGangManager::UpdateGangs() || 1"; g_LogFile.ssend(); }
	// maybe add some new gangs to the recruitable list
	int add_min = cfg.gangs.add_new_weekly_min();
	int add_max = cfg.gangs.add_new_weekly_max();
	int add_recruits = g_Dice.bell(add_min, add_max);
	for (int i = 0; i < add_recruits; i++)
	{
		if (m_NumHireableGangs >= cfg.gangs.max_recruit_list()) break;
		cerr << "Adding new recruitable gang." << endl;
		AddNewGang(false);
	}
	if (cfg.debug.log_debug()) { g_LogFile.ss() << "Debug cGangManager::UpdateGangs() || 2"; g_LogFile.ssend(); }

	// now, deal with player controlled gangs on missions
	currentGang = m_GangStart;
	while (currentGang)
	{
		switch (currentGang->m_MissionID)
		{
		case MISS_GUARDING: // these are handled in GangStartOfShift()
		case MISS_SPYGIRLS: break;
		case MISS_CAPTUREGIRL:
			if (g_Brothels.m_NumRunaways > 0) { recapture_mission(currentGang); break; }
			else currentGang->m_Events.AddMessage("This gang was sent to look for runaways but there are none so they went looking for any girl to kidnap instead.", IMGTYPE_PROFILE, EVENT_GANG);
		case MISS_KIDNAPP:		kidnapp_mission(currentGang);		break;
		case MISS_SABOTAGE:		sabotage_mission(currentGang);		break;
		case MISS_EXTORTION:	extortion_mission(currentGang);		break;
		case MISS_PETYTHEFT:	petytheft_mission(currentGang);		break;
		case MISS_GRANDTHEFT:	grandtheft_mission(currentGang);	break;
		case MISS_CATACOMBS:	catacombs_mission(currentGang);		break;
		case MISS_TRAINING:		gangtraining(currentGang);			break;
		case MISS_RECRUIT:		gangrecruiting(currentGang);		break;
		case MISS_SERVICE:		service_mission(currentGang);		break;
		default:
		{
			if (cfg.debug.log_debug()) { g_LogFile.ss() << "Debug cGangManager::UpdateGangs() || bad mission " << currentGang->m_MissionID; g_LogFile.ssend(); }
			stringstream sse;
			sse << "Error: no mission set or mission not found : " << currentGang->m_MissionID;
			currentGang->m_Events.AddMessage(sse.str(), IMGTYPE_PROFILE, EVENT_GANG);
			if (currentGang->m_Next) currentGang = currentGang->m_Next;
			else break;
		}break;
		}
		if (currentGang->m_Num <= 0)
		{
			sGang* tempgang = currentGang->m_Next;
			if (losegang(currentGang))
			{
				currentGang = tempgang;
				continue;		// if they all died, move on.
			}
		}
		if (currentGang->m_Combat == false && currentGang->m_Num < 15) currentGang->m_Num++;
		check_gang_recruit(currentGang);

		currentGang = currentGang->m_Next;
	}

	if (cfg.debug.log_debug()) { g_LogFile.ss() << "Debug cGangManager::UpdateGangs() || 3"; g_LogFile.ssend(); }


	m_Rivals->Update(m_BusinessesExtort);	// Update the rivals

	RestockNetsAndPots();
	if (cfg.debug.log_debug()) { g_LogFile.ss() << "Debug cGangManager::UpdateGangs() || end"; g_LogFile.ssend(); }

}

// `J` restock at the start and end of the gang shift - Added for .06.01.09
void cGangManager::RestockNetsAndPots()
{
	cTariff tariff;
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
		g_Gold.consumable_cost(tariff.healing_price(diff));
	}
	if (m_KeepNetsStocked > 0 && m_KeepNetsStocked > m_NumNets)
	{
		int diff = m_KeepNetsStocked - m_NumNets;
		m_NumNets = m_KeepNetsStocked;
		g_Gold.consumable_cost(tariff.nets_price(diff));
	}
}

void cGangManager::SendGang(int gangID, int missID)
{
	// first find the gang
	sGang* currentGang = m_GangStart;
	int count = 0;
	while (currentGang)
	{
		if (count == gangID) break;
		count++;
		currentGang = currentGang->m_Next;
	}
	if (currentGang) currentGang->m_MissionID = missID;
}

sGang* cGangManager::GetGangOnMission(u_int missID)
{
	// first find the gang with some men
	sGang* currentGang = m_GangStart;
	int count = 0;
	while (currentGang)
	{
		if (currentGang->m_Num < 0 || currentGang->m_Num>15) return 0;	// `J` bug fix?
		if (currentGang->m_MissionID == missID && currentGang->m_Num > 0) break;
		count++;
		currentGang = currentGang->m_Next;
	}
	return currentGang;
}

sGang* cGangManager::GetRandomGangOnMission(u_int missID)
{
	// first find the gang with some men
	sGang* currentGang = m_GangStart;
	vector<sGang*> gangs;
	int count = 0;
	while (currentGang)
	{
		if (currentGang->m_MissionID == missID && currentGang->m_Num > 0)
		{
			gangs.push_back(currentGang);
			count++;
		}
		currentGang = currentGang->m_Next;
	}
	if (count == 0)	return 0;
	return gangs[g_Dice%count];
}

// `J` - Added for .06.02.18
sGang* cGangManager::GetGangNotFull(int roomfor, bool recruiting)
{
	sGang* currentGang = m_GangStart;
	if (recruiting)
	{
		int mission[5] = { MISS_RECRUIT, MISS_TRAINING, MISS_SPYGIRLS, MISS_GUARDING, MISS_SERVICE };
		while (currentGang)
		{
			if (currentGang->m_Num + roomfor <= 15)
			{
				for (int i = 0; i < 5; i++) if (currentGang->m_MissionID == mission[i])	return currentGang;
			}
			currentGang = currentGang->m_Next;
		}
	}
	else
	{
		while (currentGang)
		{
			if (currentGang->m_Num < 15) return currentGang;
			currentGang = currentGang->m_Next;
		}
	}
	return 0;
}

// `J` - Added for .06.01.09
sGang* cGangManager::GetGangRecruitingNotFull(int roomfor)
{
	sGang* currentGang = m_GangStart;
	// first try to get a gang that can hold all that are being sent to them
	int mission[5] = { MISS_RECRUIT, MISS_TRAINING, MISS_SPYGIRLS, MISS_GUARDING, MISS_SERVICE };
	while (currentGang)
	{
		if (currentGang->m_Num + roomfor <= 15)
		{
			for (int i = 0; i < 5; i++) if (currentGang->m_MissionID == mission[i])	return currentGang;
		}
		currentGang = currentGang->m_Next;
	}
	// if none are found then get a gang that has room for at least 1
	currentGang = m_GangStart;
	while (currentGang)
	{
		if (currentGang->m_Num < 15)
		{
			for (int i = 0; i < 5; i++) if (currentGang->m_MissionID == mission[i])	return currentGang;
		}
		currentGang = currentGang->m_Next;
	}

	return 0;
}

// Get a vector with all the gangs doing MISS_FOO
vector<sGang*> cGangManager::gangs_on_mission(u_int mission_id)
{
	vector<sGang*> v; // loop through the gangs
	for (sGang *gang = m_GangStart; gang; gang = gang->m_Next)
	{
		// if they're not doing the job we are looking for, disregard them
		if (gang->m_MissionID != mission_id) continue;
		v.push_back(gang);
	}
	return v;
}

// `J` - Added for .06.01.09
vector<sGang*> cGangManager::gangs_watching_girls()
{
	vector<sGang*> v; // loop through the gangs
	for (sGang *gang = m_GangStart; gang; gang = gang->m_Next)
	{
		// if they're not doing the job we are looking for, disregard them
		if (gang->m_MissionID == MISS_GUARDING || gang->m_MissionID == MISS_SPYGIRLS) v.push_back(gang);
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
	sGang* gang = m_GangStart;
	vector<sGang*> gvec = gangs_on_mission(MISS_SPYGIRLS);	// get a vector containing all the spying gangs
	g_LogFile.ss() << "cGangManager::chance_to_catch: " << gvec.size() << " gangs spying"; g_LogFile.ssend();	// bit of debug chatter
	for (u_int i = 0; i < gvec.size(); i++)		// whizz down the vector adding probability as we go
	{
		gang = gvec[i];
		/*
		*		now then: the basic chance is 5 * number of goons
		*		but I want to modify that for the intelligence
		*		of the girl, and that of the squad
		*/
		float mod = float(100 + gang->intelligence());
		mod -= girl->intelligence();
		mod /= 100.0;
		/*
		*		that should give us a multiplier that can
		*		at one extreme, double the chances of the sqaud
		*		catching her, and at the other, reduce it to zero
		*/
		pc += int(5 * gang->m_Num * mod);
	}
	if (pc > 100) pc = 100;
	BoostGangSkill(&gang->m_Stats[STAT_INTELLIGENCE], 1);
	return pc;
}

// `J` returns true if they succeded, false if they failed - updated for .06.01.09
bool cGangManager::sabotage_mission(sGang* gang)
{
	if (cfg.debug.log_debug()) { g_LogFile.ss() << "Debug bool cGangManager::sabotage_mission(sGang* gang) || Start"; g_LogFile.ssend(); }
	stringstream ss;
	ss << "Gang   " << gang->m_Name << "   is attacking rivals.\n \n";
	/*
	*	See if they can find any enemy assets to attack
	*
	*	I'd like to add a little more intelligence to this.
	*	Modifiers based on gang intelligence, for instance
	*	Allow a "scout" activity for gangs that improves the
	*	chances of a raid. That sort of thing.
	*/
	if (!g_Dice.percent(min(90, gang->intelligence())))
	{
		gang->m_Events.AddMessage("They failed to find any enemy assets to hit.", IMGTYPE_PROFILE, EVENT_GANG);
		return false;
	}
	/*
	*	if yes then do damage to a random rival
	*
	*	Something else to consider: rival choice should be
	*	weighted by number of territories controlled
	*	(or - if we go with the ward idea - by territories
	*	controlled in the ward in question
	*
	*	of course, if there is no rival, it's academic
	*/
	cRival* rival = m_Rivals->GetRandomRivalToSabotage();
	sGang* rival_gang = 0;
	if (!rival)
	{
		gang->m_Events.AddMessage("Scouted the city in vain, seeking would-be challengers to your dominance.", IMGTYPE_PROFILE, EVENT_GANG);
		return false;
	}

	if (rival->m_NumGangs > 0)
	{
		rival_gang = GetTempGang(rival->m_Power);
		ss << "Your men run into a gang from " << rival->m_Name << " and a brawl breaks out.\n";
		if (GangBrawl(gang, rival_gang) == false)
		{
			delete rival_gang;
			rival_gang = 0;
			if (gang->m_Num > 0)
			{
				ss << "Your men lost. The ";
				if (gang->m_Num == 1) "lone survivor fights his";
				else ss << gang->m_Num << " survivors fight their";
				ss << " way back to friendly territory.";
				gang->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_DANGER);
			}
			else
			{
				ss << "Your gang " << gang->m_Name << " fails to report back from their sabotage mission.\nLater you learn that they were wiped out to the last man.";
				g_MessageQue.AddToQue(ss.str(), COLOR_RED);
			}
			return false;
		}
		else ss << "Your men win." << endl;
		if (rival_gang->m_Num <= 0)			// clean up the rival gang
		{
			rival->m_NumGangs--;
			ss << "The enemy gang is destroyed. " << rival->m_Name << " has ";
			if (rival->m_NumGangs == 0)			ss << "no more gangs left!\n";
			else if (rival->m_NumGangs <= 3)	ss << "a few gangs left.\n";
			else								ss << "a lot of gangs left.\n";
		}
		delete rival_gang;
		rival_gang = 0;
	}
	else ss << "\nYour men encounter no resistance when you go after " << rival->m_Name << ".";

	// if we had an objective to attack a rival we just achieved it
	if (g_Brothels.GetObjective() && g_Brothels.GetObjective()->m_Objective == OBJECTIVE_LAUNCHSUCCESSFULATTACK)
		g_Brothels.PassObjective();

	// If the rival has some businesses under his control he's going to lose some of them
	if (rival->m_BusinessesExtort > 0)
	{
		// mod: brighter goons do better damage they need 100% to be better than before however
		int spread = gang->intelligence() / 4;
		int num = 1 + g_Dice.random(spread);	// get the number of businesses lost
		if (rival->m_BusinessesExtort < num)  // Can't destroy more businesses than they have
			num = rival->m_BusinessesExtort;
		rival->m_BusinessesExtort -= num;

		ss << "\nYour men destroy " << num << " of their businesses. " << rival->m_Name << " have ";
		if (rival->m_BusinessesExtort == 0)			ss << "no more businesses left!\n";
		else if (rival->m_BusinessesExtort <= 10)	ss << "a few businesses left.\n";
		else										ss << "a lot of businesses left.\n";
	}
	else ss << rival->m_Name << " have no businesses to attack.\n";

	if (rival->m_Gold > 0)
	{
		// mod: brighter goons are better thieves
		// `J` changed it // they need 100% to be better than before however	
		// `J` now based on rival's gold
		// `J` bookmark - your gang sabotage mission gold taken
		int gold = g_Dice.random(
			(int)(((double)gang->intelligence() / 2000.0) * (double)rival->m_Gold))		// 0-5% of rival's gold
			+ g_Dice.random((gang->intelligence() / 5) * gang->m_Num);					// plus (int/5)*num
		if (gold > rival->m_Gold) gold = rival->m_Gold;
		rival->m_Gold -= gold;

		// some of the money taken 'dissappears' before the gang reports it.
		if (g_Dice.percent(20) && gold > 1000) gold -= g_Dice % 1000;

		ss << "\nYour men steal " << gold << " gold from them. ";
		if (rival->m_Gold == 0)				ss << "Mu-hahahaha!  They're penniless now!\n";
		else if (rival->m_Gold <= 10000)	ss << rival->m_Name << " is looking pretty poor.\n";
		else			ss << "It looks like " << rival->m_Name << " still has a lot of gold.\n";

		/*
		`J` zzzzzz - need to add more and better limiters
		Suggestions from Whitetooth:
		I'm guessing those factors are based on there skills which make sense. For Example:
		Men - Overall number of people able to carry gold after sabotage.
		Combat - total amount of gold each man can hold.
		Magic - Amount of extra gold the gang can carry with magic not relying on combat or men. Magic could be bonus gold that can't be dropped, bribed, or stolen on the way back.
		Intel - Could be a overall factor to check if the gang knows what is valuable and what isn't.
		Agility - Could be a check for clumsiness of the gang; they could drop valuables on the way back.
		Tough - Checks if there tough enough to intimidate any guards or protect the money they have.
		Charisma - Factors how much gold they have to bribe to guards if they get caught and can't intimidate them.
		The order of checks could be -> Intel -> Magic -> Men - > Combat -> Agility -> Tough -> Charisma
		*/

		// `J` bookmark - limit gold taken by gang sabotage
		bool limit = false;
		if (gold > 15000)
		{
			limit = true;
			int burnedbonds = (gold / 10000);
			int bbcost = burnedbonds * 10000;
			gold -= bbcost;
			ss << "\nAs your men are fleeing, one of them has to jump through a wall of fire. When he does, he drops ";
			if (burnedbonds == 1)		ss << "a";
			else if (burnedbonds > 4)	ss << "a stack of ";
			else						ss << burnedbonds;
			ss << " Gold Bearer Bond" << (burnedbonds > 1 ? "s" : "") << " worth 10k gold each. " << bbcost << " gold just went up in smoke.\n";
		}

		if (gold > 5000 && g_Dice.percent(50))
		{
			limit = true;
			int spill = (g_Dice % 4500) + 500;
			gold -= spill;
			ss << "\nAs they are being chased through the streets by " << rival->m_Name << "'s people, one of your gang members cuts open a sack of gold spilling its contents in the street. "
				<< "As the throngs of civilians stream in to collect the coins, they block the pursuers and allow you men to get away safely.\n";
		}

		if (gold > 5000)
		{
			limit = true;
			int bribeperc = ((g_Dice % 15) * 5) + 10;
			int bribe = (int)(gold * ((double)bribeperc / 100.0));
			gold -= bribe;
			ss << "\nAs your gang leave your rival's territory on the way back to your brothel, they come upon a band of local police that are hunting them. Their boss demands "
				<< bribeperc << "% of what your gang is carrying in order to let them go.  They pay them " << bribe << " gold and continue on home.\n";
		}

		if (limit)
		{
			ss << "\n" << gang->m_Name << " returns with " << gold << " gold.\n";
		}
		g_Gold.plunder(gold);
	}
	else ss << "The losers have no gold to take.\n";

	if (rival->m_NumInventory > 0 && g_Dice.percent(min(75, gang->intelligence())))
	{
		cRivalManager r;
		int num = r.GetRandomRivalItemNum(rival);
		sInventoryItem* item = r.GetRivalItem(rival, num);
		if (item)
		{
			ss << "\nYour men steal an item from them, one " << item->m_Name << ".";
			r.RemoveRivalInvByNumber(rival, num);
			g_Brothels.AddItemToInventory(item);
		}
	}

	if (rival->m_NumBrothels > 0 && g_Dice.percent(gang->intelligence() / min(3, 11 - rival->m_NumBrothels)))
	{
		rival->m_NumBrothels--;
		rival->m_Power--;
		ss << "\nYour men burn down one of " << rival->m_Name << "'s Brothels. " << rival->m_Name;
		if (rival->m_NumBrothels == 0)				ss << " has no Brothels left.\n";
		else if (rival->m_NumBrothels <= 3)			ss << " is in control of very few Brothels.\n";
		else										ss << " has many Brothels left.\n";
	}
	if (rival->m_NumGamblingHalls > 0 && g_Dice.percent(gang->intelligence() / min(1, 9 - rival->m_NumGamblingHalls)))
	{
		rival->m_NumGamblingHalls--;
		ss << "\nYour men burn down one of " << rival->m_Name << "'s Gambling Halls. " << rival->m_Name;
		if (rival->m_NumGamblingHalls == 0)			ss << " has no Gambling Halls left.\n";
		else if (rival->m_NumGamblingHalls <= 3)	ss << " is in control of very few Gambling Halls .\n";
		else										ss << " has many Gambling Halls left.\n";
	}
	if (rival->m_NumBars > 0 && g_Dice.percent(gang->intelligence() / min(1, 7 - rival->m_NumBars)))
	{
		rival->m_NumBars--;
		ss << "\nYour men burn down one of " << rival->m_Name << "'s Bars. " << rival->m_Name;
		if (rival->m_NumBars == 0)					ss << " has no Bars left.\n";
		else if (rival->m_NumBars <= 3)				ss << " is in control of very few Bars.\n";
		else										ss << " has many Bars left.\n";
	}

	BoostGangSkill(&gang->m_Stats[STAT_INTELLIGENCE], 2);
	gang->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_GANG);

	// See if the rival is eliminated:  If 4 or more are zero or less, the rival is eliminated
	int VictoryPoints = 0;
	if (rival->m_Gold <= 0)					VictoryPoints++;
	if (rival->m_NumGangs <= 0)				VictoryPoints++;
	if (rival->m_BusinessesExtort <= 0)		VictoryPoints++;
	if (rival->m_NumBrothels <= 0)			VictoryPoints++;
	if (rival->m_NumGamblingHalls <= 0)		VictoryPoints++;
	if (rival->m_NumBars <= 0)				VictoryPoints++;

	if (VictoryPoints >= 4)
	{
		stringstream ssVic;
		ssVic << "You have dealt " << rival->m_Name << " a fatal blow.  Their criminal organization crumbles to nothing before you.";
		m_Rivals->RemoveRival(rival);
		gang->m_Events.AddMessage(ssVic.str(), IMGTYPE_PROFILE, EVENT_GOODNEWS);
	}
	return true;
}

// `J` returns true if they succeded, false if they failed - updated for .06.01.09
bool cGangManager::recapture_mission(sGang* gang)
{
	if (cfg.debug.log_debug()) { g_LogFile.ss() << "Debug bool cGangManager::recapture_mission(sGang* gang) || Start"; g_LogFile.ssend(); }
	stringstream ss;
	ss << "Gang   " << gang->m_Name << "   is looking for escaped girls.\n \n";


	// check if any girls have run away, if no runnaway then the gang continues on as normal
	sGirl* runnaway = g_Brothels.GetFirstRunaway();
	if (runnaway == 0)	// `J` this should have been replaced by a check in the gang mission list
	{
		ss << "There are none of your girls who have run away, so they have noone to look for.";
		gang->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_GANG);
		return false;
	}

	stringstream RGmsg;
	string girlName = runnaway->m_Realname;
	bool captured = false;
	int damagednets = 0;
	int girlimagetype = IMGTYPE_PROFILE;
	int gangeventtype = EVENT_GANG;

	ss << "Your goons find " << girlName << " and ";

	if (!g_Brothels.FightsBack(runnaway))
	{
		ss << "she comes quietly without putting up a fight.";
		RGmsg << girlName << " was recaptured by " << gang->m_Name << ". She gave up without a fight.";
		captured = true;
	}
	if (!captured && gang->net_limit() > 0)	// try to capture using net
	{
		ss << "they try to catch her in their net" << (gang->net_limit() > 1 ? "s" : "") << ".\n";
		int tries = 0;
		while (gang->net_limit() > 0 && !captured)
		{
			int damagechance = 40;
			if (g_Dice.percent(gang->combat()))	// hit her with the net
			{
				if (!g_Dice.percent(double(runnaway->agility() - tries) / 2.0))	// she can't avoid or get out of the net
				{
					captured = true;
				}
				else damagechance = 60;
			}

			if (g_Dice.percent(damagechance)) { damagednets++; gang->net_limit(-1); m_NumNets--; }
			tries++;
		}
		if (captured)
		{
			if (damagednets > 0)
			{
				ss << girlName << " managed to damage " << damagednets << " of their nets before they finally caught her in ";
				/* */if (gang->net_limit() == 0) ss << "the tattered remains of their last net.\n";
				else if (gang->net_limit() == 1) ss << "their last net.\n";
				else ss << "their nets.\n";
			}
			ss << "She struggles against the net your men use, but it is pointless.  She is in your dungeon now.\n";
			girlimagetype = IMGTYPE_DEATH;
			RGmsg << girlName << " was captured in a net and dragged back to the dungeon by " << gang->m_Name << ".";
			BoostGangSkill(&gang->m_Stats[STAT_INTELLIGENCE], 2);
		}
		else
		{
			ss << girlName << " managed to damage all of their nets so they have to do things the hard way.\n";
		}
	}
	if (!captured)
	{
		if (g_Brothels.FightsBack(runnaway))	// kidnap her
		{
			if (damagednets == 0) ss << "attempt to recapture her.\n";
			if (!GangCombat(runnaway, gang))
			{
				girlimagetype = IMGTYPE_DEATH;
				ss << "She fights back but your men succeed in capturing her.\n";
				RGmsg << girlName << " fought with " << gang->m_Name << " but lost. She was dragged back to the dungeon.";
				BoostGangSkill(&gang->m_Skills[SKILL_COMBAT], 1);
				captured = true;
			}
			else
			{
				ss << "The girl fights back and defeats your men before escaping into the streets.";
				gangeventtype = EVENT_DANGER;
			}
		}
		else if (damagednets == 0)
		{
			ss << "recapture her successfully without a fuss.  She is in your dungeon now.";
			RGmsg << girlName << " was surrounded by " << gang->m_Name << " and gave up without a fight.";
			captured = true;
		}
		else
		{
			ss << "After dodging all of their nets, she gives up when they pull out their weapons and prepare to kill her.";
			RGmsg << girlName << " was surrounded by " << gang->m_Name << " and gave up without anymore of a fight.";
			captured = true;
		}
	}

	gang->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, gangeventtype);
	if (captured)
	{
		runnaway->m_Events.AddMessage(RGmsg.str(), girlimagetype, EVENT_GANG);
		runnaway->m_RunAway = 0;
		g_Brothels.RemoveGirlFromRunaways(runnaway);
		m_Dungeon->AddGirl(runnaway, DUNGEON_GIRLRUNAWAY);
		return true;
	}
	return false;
}

// `J` returns true if they succeded, false if they failed - updated for .06.01.09
bool cGangManager::extortion_mission(sGang* gang)
{
	if (cfg.debug.log_debug()) { g_LogFile.ss() << "Debug bool cGangManager::extortion_mission(sGang* gang) || Start"; g_LogFile.ssend(); }
	stringstream ss;
	The_Player->disposition(-1);	The_Player->customerfear(1);	The_Player->suspicion(1);
	ss << "Gang   " << gang->m_Name << "   is capturing territory.\n \n";

	// Case 1:  Neutral businesses still around
	int numB = m_Rivals->GetNumBusinesses();
	int uncontrolled = TOWN_NUMBUSINESSES - m_BusinessesExtort - numB;
	int n = 0;
	int trycount = 1;
	if (uncontrolled > 0)
	{
		trycount += g_Dice % 5;		// 1-5
		while (uncontrolled > 0 && trycount > 0)
		{
			trycount--;
			if (g_Dice.percent(gang->charisma() / 2))				// convince
			{
				uncontrolled--; n++;
				The_Player->customerfear(-1);
			}
			else if (g_Dice.percent(gang->intelligence() / 2))		// outwit
			{
				uncontrolled--; n++;
				The_Player->disposition(-1);
			}
			else if (g_Dice.percent(gang->combat() / 2))			// threaten
			{
				uncontrolled--; n++;
				The_Player->disposition(-1);
				The_Player->customerfear(2);
			}
		}

		if (n == 0) ss << "They fail to gain any more neutral territories.";
		else
		{
			ss << " You gain control of " << n << " more neutral territor" << (n > 1 ? "ies." : "y.");
			m_BusinessesExtort += n;
			g_Gold.extortion(n * 20);
		}
		ss << "\nThere ";
		if (uncontrolled <= 0)	ss << "are no more";
		if (uncontrolled == 1)	ss << "is one";
		else ss << "are " << uncontrolled;
		ss << " uncontrolled businesses left.";

	}
	else	// Case 2: Steal bussinesses away from rival if no neutral businesses left
	{
		cRival* rival = m_Rivals->GetRandomRivalWithBusinesses();
		if (rival && rival->m_BusinessesExtort > 0)
		{
			ss << "They storm into your rival " << rival->m_Name << "'s territory.\n";
			bool defended = false;
			if (rival->m_NumGangs > 0)
			{
				sGang* rival_gang = GetTempGang(rival->m_Power);
				defended = true;
				ss << "Your men run into one of their gangs and a brawl breaks out.\n";

				if (GangBrawl(gang, rival_gang))
				{
					trycount += g_Dice % 3;

					ss << "They ";
					if (rival_gang->m_Num <= 0)
					{
						ss << "destroy";
						rival->m_NumGangs--;
					}
					else ss << "defeat";
					ss << " the defenders and ";
				}
				else
				{
					ss << "Your gang has been defeated and fail to take control of any new territory.";
					gang->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_GANG);
					return false;
				}
				delete rival_gang;
				rival_gang = 0;
			}
			else // Rival has no gangs
			{
				ss << "They faced no opposition as they ";
				trycount += g_Dice % 5;
			}

			while (trycount > 0 && rival->m_BusinessesExtort > 0)
			{
				trycount--;
				rival->m_BusinessesExtort--;
				m_BusinessesExtort++;
				n++;
			}

			if (n > 0)
			{
				ss << "took over ";
				if (n == 1) ss << "one"; else ss << n;
				ss << " of " << rival->m_Name << "'s territor" << (n > 1 ? "ies" : "y");
			}
			else ss << "left. (Error: no territories gained but should have been)";
		}
		else
		{
			ss << "You fail to take control of any of new territories.";
		}
	}

	gang->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_GANG);

	if (g_Brothels.GetObjective() && g_Brothels.GetObjective()->m_Objective == OBJECTIVE_EXTORTXNEWBUSINESS)
	{
		g_Brothels.GetObjective()->m_SoFar += n;
	}

	return true;
}

// `J` returns true if they succeded, false if they failed - updated for .06.01.09
bool cGangManager::petytheft_mission(sGang* gang)
{
	if (cfg.debug.log_debug()) { g_LogFile.ss() << "Debug bool cGangManager::petytheft_mission(sGang* gang) || Start"; g_LogFile.ssend(); }
	stringstream ss;
	ss << "Gang   " << gang->m_Name << "   is performing petty theft.\n \n";
	The_Player->disposition(-1);
	The_Player->customerfear(1);
	The_Player->suspicion(1);

	int startnum = gang->m_Num;
	int numlost = 0;

	// `J` chance of running into a rival gang updated for .06.02.41
	int gangs = m_Rivals->GetNumRivalGangs();
	int chance = 5 + max(20, gangs * 2);				// 5% base +2% per gang, 25% max

	if (g_Dice.percent(chance))
	{
		cRival* rival = m_Rivals->GetRandomRivalWithGangs();
		ss << "Your men run into ";
		if (rival && rival->m_NumGangs > 0)	ss << "a gang from " << rival->m_Name;
		else/*                           */	ss << "group of thugs from the streets";
		ss << " and a brawl breaks out.\n";

		sGang* rival_gang = GetTempGang();
		if (GangBrawl(gang, rival_gang)) ss << "Your men win.";
		else
		{
			ss << "Your men lose the fight.";
			gang->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_GANG);
			return false;
		}
		ss << "\n \n";
		if (rival && rival->m_NumGangs > 0 && rival_gang->m_Num <= 0) rival->m_NumGangs--;
		delete rival_gang;
		rival_gang = 0;

		numlost += startnum - gang->m_Num;
	}
	else if (g_Dice.percent(1))		// `J` added for .06.02.41
	{
		sGirl* girl = g_Girls.GetRandomGirl();
		if (girl->has_trait("Incorporeal"))	girl = g_Girls.GetRandomGirl();		// try not to get an incorporeal girl but only 1 check
		if (girl)
		{
			string girlName = girl->m_Realname;
			stringstream NGmsg;
			int girlimagetype = IMGTYPE_PROFILE;
			int eventtype = EVENT_GANG;
			int gangeventtype = EVENT_GANG;
			int dungeonreason = DUNGEON_GIRLKIDNAPPED;
			int damagednets = 0;
			
			// `J` make sure she is ready for a fight
			if (girl->combat() < 50)		girl->combat(10 + g_Dice % 30);
			if (girl->magic() < 50)			girl->magic(10 + g_Dice % 20);
			if (girl->constitution() < 50)	girl->constitution(10 + g_Dice % 20);
			if (girl->agility() < 50)		girl->agility(10 + g_Dice % 20);
			if (girl->confidence() < 50)	girl->agility(10 + g_Dice % 40);
			girl->health(100);
			girl->tiredness(-100);

			ss << "Your men are confronted by a masked vigilante.\n";
			if (!GangCombat(girl, gang))
			{
				numlost += startnum - gang->m_Num;
				long gold = girl->m_Money > 0 ? girl->m_Money : g_Dice % 100 + 1;	// take all her money or 1-100 if she has none
				girl->m_Money = 0;
				g_Gold.petty_theft(gold);

				ss << "She fights ";
				if (numlost > startnum / 2)	ss << "well but your men still manage to capture her";
				else if (numlost == 0)/* */	ss << "your men but loses quickly";
				else/*                   */	ss << "your men but they take her down with only " << (numlost == 1 ? "one casualty" : "a few casualties");
				ss << ".\nThey unmask " << girlName << ", take all her gold (" << gold << ") from her and drag her to the dungeon.\n \n";
				girlimagetype = IMGTYPE_DEATH;
				dungeonreason = DUNGEON_GIRLKIDNAPPED;
				girl->m_Stats[STAT_OBEDIENCE] = 0;
				girl->add_trait("Kidnapped", 5 + g_Dice % 11);

				NGmsg << girl->m_Realname << " tried to stop " << gang->m_Name << " from comitting petty theft but lost. She was dragged back to the dungeon.";
				BoostGangSkill(&gang->m_Skills[SKILL_COMBAT], 1);

				if (g_Brothels.GetObjective() && g_Brothels.GetObjective()->m_Objective == OBJECTIVE_STEALXAMOUNTOFGOLD)
				{
					g_Brothels.GetObjective()->m_SoFar += gold;
				}
				if (g_Brothels.GetObjective() && g_Brothels.GetObjective()->m_Objective == OBJECTIVE_KIDNAPXGIRLS)
				{
					g_Brothels.GetObjective()->m_SoFar++;	// `J` You are technically kidnapping her
				}
				return true;
			}
			else
			{
				ss << "She defeats your men and disappears back into the shadows.";
				gang->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_GANG);
				return false;
			}
		}
	}

	int difficulty = max(0, g_Dice.bell(1, 6) - 2);	// 0-4
	string who = "people";
	int fightbackchance = 0;
	int numberoftargets = 2 + g_Dice % 9;
	int targetfight = numberoftargets;
	long gold = 0;
	long goldbase = 1;

	if (difficulty <= 0)	{ who = "kids";					fightbackchance = 50;	goldbase += 20;		difficulty = 0; }
	if (difficulty == 1)	{ who = "little old ladies";	fightbackchance = 40;	goldbase += 40; }
	if (difficulty == 2)	{ who = "noble men and women";	fightbackchance = 30;	goldbase += 60; }
	if (difficulty == 3)	{ who = "small stalls";			fightbackchance = 50;	goldbase += 80; }
	if (difficulty >= 4)	{ who = "traders";				fightbackchance = 70;	goldbase += 100;	difficulty = 4; }

	for (int i = 0; i < numberoftargets; i++)	gold += g_Dice % goldbase;

	if (g_Dice.percent(fightbackchance))	// determine losses if they fight back
	{
		while (gang->m_Num > 0 && targetfight > 0)	// fight until someone wins
		{
			if (g_Dice.percent(gang->combat()))		targetfight--;		// you win so lower their numbers
			else if (g_Dice.percent(g_Dice % 11 + (difficulty * 10)))		// or they win 
			{
				if (gang->heal_limit() > 0) { gang->heal_limit(-1); m_NumHealingPotions--; }	// but you heal
				else { gang->m_Num--; numlost++; }							// otherwise lower your numbers
			}
		}
	}

	if (gang->m_Num <= 0) return false;	// they all died so return and the message will be taken care of in the losegang function

	ss << "Your gang robs " << numberoftargets << " " << who << " and get " << gold << " gold from them.";
	if (numlost > 0) { ss << "\n \n" << gang->m_Name << " lost "; if (numlost == 1) ss << "one man."; else ss << numlost << " men."; }

	gang->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_GANG);

	g_Gold.petty_theft(gold);

	if (g_Brothels.GetObjective() && g_Brothels.GetObjective()->m_Objective == OBJECTIVE_STEALXAMOUNTOFGOLD)
	{
		g_Brothels.GetObjective()->m_SoFar += gold;
	}
	return true;
}

// `J` returns true if they succeded, false if they failed - updated for .06.01.09
bool cGangManager::grandtheft_mission(sGang* gang)
{
	if (cfg.debug.log_debug()) { g_LogFile.ss() << "Debug bool cGangManager::grandtheft_mission(sGang* gang) || Start"; g_LogFile.ssend(); }
	stringstream ss;
	The_Player->disposition(-3);	The_Player->customerfear(3);	The_Player->suspicion(3);
	bool fightrival = false;		cRival* rival = 0;				sGang *defenders = 0;
	string place = "place";			int defencechance = 0;			long gold = 1;
	int difficulty = max(0, g_Dice.bell(0, 6) - 2);	// 0-4

	if (difficulty <= 0)	{ place = "small shop";		defencechance = 10;		gold += 10 + g_Dice % 290; difficulty = 0; }
	if (difficulty == 1)	{ place = "smithy";			defencechance = 30;		gold += 50 + g_Dice % 550; }
	if (difficulty == 2)	{ place = "jeweler";		defencechance = 50;		gold += 200 + g_Dice % 800; }
	if (difficulty == 3)	{ place = "trade caravan";	defencechance = 70;		gold += 500 + g_Dice % 1500; }
	if (difficulty >= 4)	{ place = "bank";			defencechance = 90;		gold += 1000 + g_Dice % 4000; difficulty = 4; }

	ss << "Gang   " << gang->m_Name << "   goes out to rob a " << place << ".\n \n";

	// `J` chance of running into a rival gang updated for .06.02.41
	int gangs = m_Rivals->GetNumRivalGangs();
	int chance = 10 + max(30, gangs * 2);				// 10% base +2% per gang, 40% max
	ss << "The " << place << " ";
	if (g_Dice.percent(chance))
	{
		rival = m_Rivals->GetRandomRivalWithGangs();
		if (rival && rival->m_NumGangs > 0)
		{
			fightrival = true;
			ss << "is guarded by a gang from " << rival->m_Name;
			defenders = GetTempGang(rival->m_Power);
		}
	}
	if (defenders == 0 && g_Dice.percent(defencechance))
	{
		defenders = GetTempGang(difficulty * 3);
		ss << "has its own guards";
	}
	if (defenders == 0)	ss << "is unguarded";

	if (defenders)
	{
		ss << ". They ";
		int gstart = gang->m_Num;
		if (!GangBrawl(gang, defenders))
		{
			ss << "put up quite a fight and send your men running.";
			gang->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_DANGER);
			return false;
		}
		float dif = (float)gstart / (float)gang->m_Num;
		if (gstart == gang->m_Num)	ss << "turn tail and run as soon as they see your men";
		else if (dif > 0.9)/*   */	ss << "put up little resistance as your men walk right over them";
		else if (dif > 0.7)/*   */	ss << "put up a fight but your men win";
		else if (dif > 0.5)/*   */	ss << "fight well but your men defeat them";
		else if (dif > 0.3)/*   */	ss << "fight well but your men still manage to vanquish their foe";
		else/*                  */	ss << "fight valiantly but your men still manage to vanquish their foe";
	}
	ss << ".\n \n";

	if (fightrival && defenders->m_Num <= 0) rival->m_NumGangs--;
	delete defenders; defenders = 0;

	// rewards
	ss << "They get away with " << gold << " gold from the " << place << ".\n";

	// `J` zzzzzz - need to add items


	The_Player->suspicion(gold / 1000);

	g_Gold.grand_theft(gold);
	gang->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_GANG);

	if (g_Brothels.GetObjective() && g_Brothels.GetObjective()->m_Objective == OBJECTIVE_STEALXAMOUNTOFGOLD)
	{
		g_Brothels.GetObjective()->m_SoFar += gold;
	}
	return true;
}

// `J` returns true if they succeded, false if they failed - updated for .06.01.09
bool cGangManager::kidnapp_mission(sGang* gang)
{
	if (cfg.debug.log_debug()) { g_LogFile.ss() << "Debug bool cGangManager::kidnapp_mission(sGang* gang) || Start"; g_LogFile.ssend(); }
	stringstream ss;
	ss << "Gang   " << gang->m_Name << "   is kidnapping girls.\n \n";
	bool captured = false;

	if (g_Dice.percent(min(75, gang->intelligence())))	// chance to find a girl to kidnap
	{
		sGirl* girl = g_Girls.GetRandomGirl();
		if (girl)
		{
			int v[2] = { -1, -1 };
			if (girl->m_Triggers.CheckForScript(TRIGGER_KIDNAPPED, true, v))
			{
				return true; // not sure if they got the girl from the script but assume they do.
			}

			string girlName = girl->m_Realname;
			stringstream NGmsg;
			int girlimagetype = IMGTYPE_PROFILE;
			int eventtype = EVENT_GANG;
			int gangeventtype = EVENT_GANG;
			int dungeonreason = DUNGEON_GIRLKIDNAPPED;
			int damagednets = 0;


			/* MYR: For some reason I can't figure out, a number of girl's house percentages
			are at zero or set to zero when they are sent to the dungeon. I'm not sure
			how to fix it, so I'm explicitly setting the percentage to 60 here */
			girl->m_Stats[STAT_HOUSE] = 60;

			ss << "Your men find a girl, " << girlName << ", and ";
			if (g_Dice.percent(min(75, gang->charisma())))	// convince her
			{
				ss << "convince her that she should work for you.\n";
				NGmsg << girlName << " was talked into working for you by " << gang->m_Name << ".";
				dungeonreason = DUNGEON_NEWGIRL;
				BoostGangSkill(&gang->m_Stats[STAT_CHARISMA], 3);
				captured = true;
				if (g_Brothels.GetObjective() && g_Brothels.GetObjective()->m_Objective == OBJECTIVE_KIDNAPXGIRLS)
				{
					g_Brothels.GetObjective()->m_SoFar++;						// `J` Added to make Charisma Kidnapping count
					if (g_Dice.percent(g_Brothels.GetObjective()->m_Target * 10))	// but possibly reduce the reward to gold only
						g_Brothels.GetObjective()->m_Reward = REWARD_GOLD;
				}
			}
			if (!captured && gang->net_limit() > 0)	// try to capture using net
			{
				ss << "try to catch her in their net" << (gang->net_limit() > 1 ? "s" : "") << ".\n";
				int tries = 0;
				while (gang->net_limit() > 0 && !captured)
				{
					int damagechance = 40;
					if (g_Dice.percent(gang->combat()))	// hit her with the net
					{
						if (!g_Dice.percent(double(girl->agility() - tries) / 2.0))	// she can't avoid or get out of the net
						{
							captured = true;
						}
						else damagechance = 60;
					}

					if (g_Dice.percent(damagechance)) { damagednets++; gang->net_limit(-1); m_NumNets--; }
					tries++;
				}
				if (captured)
				{
					if (damagednets > 0)
					{
						ss << girlName << " managed to damage " << damagednets << " of their nets before they finally caught her in ";
						/* */if (gang->net_limit() == 0) ss << "the tattered remains of their last net.\n";
						else if (gang->net_limit() == 1) ss << "their last net.\n";
						else ss << "their nets.\n";
					}
					ss << "She struggles against the net your men use, but it is pointless.  She is in your dungeon now.\n";
					girlimagetype = IMGTYPE_DEATH;
					dungeonreason = DUNGEON_GIRLKIDNAPPED;
					girl->m_Stats[STAT_OBEDIENCE] = 0;
					girl->add_trait("Kidnapped", 5 + g_Dice % 11);
					NGmsg << girlName << " was captured in a net and dragged back to the dungeon by " << gang->m_Name << ".";
					BoostGangSkill(&gang->m_Stats[STAT_INTELLIGENCE], 2);
				}
				else
				{
					ss << girlName << " managed to damage all of their nets so they have to do things the hard way.\n";
				}
			}
			if (!captured)
			{
				if (g_Brothels.FightsBack(girl))	// kidnap her
				{
					if (damagednets == 0) ss << "attempt to kidnap her.\n";
					if (!GangCombat(girl, gang))
					{
						girlimagetype = IMGTYPE_DEATH;
						dungeonreason = DUNGEON_GIRLKIDNAPPED;
						girl->m_Stats[STAT_OBEDIENCE] = 0;
						girl->add_trait("Kidnapped", 10 + g_Dice % 11);
						ss << "She fights back but your men succeed in kidnapping her.\n";
						NGmsg << girl->m_Realname << " fought with " << gang->m_Name << " but lost. She was dragged back to the dungeon.";
						BoostGangSkill(&gang->m_Skills[SKILL_COMBAT], 1);
						captured = true;
					}
					else
					{
						ss << "The girl fights back and defeats your men before escaping into the streets.";
						gangeventtype = EVENT_DANGER;
					}
				}
				else if (damagednets == 0)
				{
					dungeonreason = DUNGEON_GIRLKIDNAPPED;
					girl->add_trait("Kidnapped", 3 + g_Dice % 8);
					ss << "kidnap her successfully without a fuss.  She is in your dungeon now.";
					NGmsg << girl->m_Realname << " was surrounded by " << gang->m_Name << " and gave up without a fight.";
					captured = true;
				}
				else
				{
					dungeonreason = DUNGEON_GIRLKIDNAPPED;
					girl->add_trait("Kidnapped", 5 + g_Dice % 8);
					ss << "After dodging all of their nets, she gives up when they pull out their weapons and prepare to kill her.";
					NGmsg << girl->m_Realname << " was surrounded by " << gang->m_Name << " and gave up without anymore of a fight.";
					captured = true;
				}
			}

			if (captured)
			{
				girl->m_Events.AddMessage(NGmsg.str(), girlimagetype, eventtype);
				m_Dungeon->AddGirl(girl, dungeonreason);
				BoostGangSkill(&gang->m_Stats[STAT_INTELLIGENCE], 1);
			}
			gang->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, gangeventtype);
		}
		else
		{
			ss << "They failed to find any girls to kidnap.";
			gang->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_GANG);
		}
	}
	else
	{
		ss << "They failed to find any girls to kidnap.";
		gang->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_GANG);
	}
	return captured;
}

// `J` returns true if they succeded, false if they failed - updated for .06.01.09
bool cGangManager::catacombs_mission(sGang* gang)
{
	if (cfg.debug.log_debug()) { g_LogFile.ss() << "Debug bool cGangManager::catacombs_mission(sGang* gang) || Start"; g_LogFile.ssend(); }
	stringstream ss;
	gang->m_Combat = true;
	int num = gang->m_Num;
	ss << "Gang   " << gang->m_Name << "   is exploring the catacombs.\n \n";

	if (!m_Control_Gangs)	// use old code
	{
		ss << "Your tell them to get whatever they can find.\n";

		// determine losses
		gang->m_Combat = true;
		for (int i = 0; i < num; i++)
		{
			if (g_Dice.percent(gang->combat())) continue;
			if (gang->heal_limit() == 0) { gang->m_Num--; continue; }
			if (g_Dice.percent(5))		// `J` 5% chance they will not get the healing potion in time.
				gang->m_Num--;			// needed to have atleast some chance or else they are totally invincable.
			gang->heal_limit(-1);
			m_NumHealingPotions--;
		}

		if (gang->m_Num <= 0) return false;
		else
		{
			if (num == gang->m_Num)	ss << "All " << gang->m_Num << " of them return.\n \n";
			else ss << gang->m_Num << " of the " << num << " who went out return.\n \n";

			// determine loot
			int gold = gang->m_Num;
			gold += g_Dice % (gang->m_Num * 100);
			ss << "They bring back with them:   " << gold << " gold";
			g_Gold.catacomb_loot(gold);

			int items = 0;
			while (g_Dice.percent((gang->intelligence() / 2) + 30) && items <= (gang->m_Num / 3))	// item chance
			{
				bool quit = false; bool add = false;
				sInventoryItem* temp = g_InvManager.GetRandomCatacombItem();
				if (temp)
				{
					ss << ",\n";
					int curI = g_Brothels.HasItem(temp->m_Name, -1);
					bool loop = true;
					while (loop)
					{
						if (curI != -1)
						{
							if (g_Brothels.m_NumItem[curI] >= 999)
								curI = g_Brothels.HasItem(temp->m_Name, curI + 1);
							else loop = false;
						}
						else loop = false;
					}

					if (g_Brothels.m_NumInventory < MAXNUM_INVENTORY || curI != -1)
					{
						if (curI != -1)
						{
							ss << "a " << temp->m_Name;
							g_Brothels.m_NumItem[curI]++;
							items++;
						}
						else
						{
							for (int j = 0; j < MAXNUM_INVENTORY; j++)
							{
								if (g_Brothels.m_Inventory[j] == 0)
								{
									ss << "a " << temp->m_Name;
									g_Brothels.m_Inventory[j] = temp;
									items++;
									g_Brothels.m_EquipedItems[j] = 0;
									g_Brothels.m_NumInventory++;
									g_Brothels.m_NumItem[j]++;
									break;
								}
							}
						}
					}
					else
					{
						quit = true;
						ss << "Your inventory is full\n";
					}
				}

				if (quit) break;
			}
			ss << ".";

			int girl = 0;
			// determine if get a catacomb girl (is "monster" if trait not human)
			if (g_Dice.percent((gang->intelligence() / 4) + 25))
			{
				sGirl* ugirl = 0;
				bool unique = false;
				if (g_Dice.percent(50))	unique = true;	// chance of getting unique girl
				if (unique)
				{
					ugirl = g_Girls.GetRandomGirl(false, true);
					if (ugirl == 0) unique = false;
				}

				if (g_Brothels.GetObjective() && g_Brothels.GetObjective()->m_Objective == OBJECTIVE_CAPTUREXCATACOMBGIRLS)
				{
					g_Brothels.GetObjective()->m_SoFar++;
				}

				if (unique)
				{
					girl++;
					ss << "\n \nYour men also captured a girl named ";
					ss << ugirl->m_Realname;
					ugirl->m_States &= ~(1 << STATUS_CATACOMBS);
					stringstream NGmsg;
					ugirl->add_trait("Kidnapped", 2 + g_Dice % 10);
					NGmsg << ugirl->m_Realname << " was captured in the catacombs by " << gang->m_Name << ".";
					ugirl->m_Events.AddMessage(NGmsg.str(), IMGTYPE_PROFILE, EVENT_GANG);
					m_Dungeon->AddGirl(ugirl, DUNGEON_GIRLCAPTURED);
				}
				else
				{
					ugirl = g_Girls.CreateRandomGirl(0, false, false, false, true);
					if (ugirl != 0)  // make sure a girl was returned
					{
						girl++;
						ss << "\n \nYour men also captured a girl.";
						stringstream NGmsg;
						ugirl->add_trait("Kidnapped", 2 + g_Dice % 10);
						NGmsg << ugirl->m_Realname << " was captured in the catacombs by " << gang->m_Name << ".";
						ugirl->m_Events.AddMessage(NGmsg.str(), IMGTYPE_PROFILE, EVENT_GANG);
						m_Dungeon->AddGirl(ugirl, DUNGEON_GIRLCAPTURED);
					}
				}
			}
			// `J` determine if they bring back any beasts
			int beasts = max(0, (g_Dice % 5) - 2);
			if (girl == 0 && gang->m_Num > 13) beasts++;
			if (beasts > 0 && g_Dice.percent(gang->m_Num * 5))
			{
				ss << "\n \nYour men also bring back " << beasts << " beasts.";
				g_Brothels.add_to_beasts(beasts);
			}
		}
	}
	else	// use new code
	{
		int totalgirls = 0; int totalitems = 0; int totalbeast = 0;
		int bringbacknum = 0;
		int gold = 0;

		// do the intro text
		ss << g_Girls.catacombs_look_for(m_Gang_Gets_Girls, m_Gang_Gets_Items, m_Gang_Gets_Beast);

		// do the bring back loop
		while (gang->m_Num >= 1 && bringbacknum < gang->m_Num * max(1, gang->strength() / 20))
		{
			double choice = (g_Dice % 10001) / 100.0;
			gold += g_Dice % (gang->m_Num * 20);

			if (choice < m_Gang_Gets_Girls)					// get girl = 10 point
			{
				bool gotgirl = false;
				sGirl* tempgirl = g_Girls.CreateRandomGirl(18, false, false, false, true);		// `J` Legal Note: 18 is the Legal Age of Majority for the USA where I live 
				if (gang->net_limit() > 0)	// try to capture using net
				{
					int tries = 0;
					while (gang->net_limit() > 0 && !gotgirl)	// much harder to net a girl in the catacombs
					{
						int damagechance = 40;	// higher damage net chance in the catacombs
						if (g_Dice.percent(gang->combat()))	// hit her with the net
						{
							if (!g_Dice.percent(double(tempgirl->agility() - tries)))	// she can't avoid or get out of the net
							{
								gotgirl = true;
							}
							else damagechance = 80;
						}

						if (g_Dice.percent(damagechance)) { gang->net_limit(-1); m_NumNets--; }
						tries++;
					}
				}
				if (!gotgirl)	// fight the girl if not netted
				{
					if (!GangCombat(tempgirl, gang)) gotgirl = true;
				}
				delete tempgirl; tempgirl = 0;
				if (gotgirl)
				{
					bringbacknum += 10;
					totalgirls++;
				}
				else bringbacknum += 5;
			}
			else if (choice < m_Gang_Gets_Girls + m_Gang_Gets_Items)	// get item = 4 points
			{
				bool gotitem = false;
				if (g_Dice.percent(33))	// item is guarded
				{
					sGirl* tempgirl = g_Girls.CreateRandomGirl(18, false, false, false, true);	// `J` Legal Note: 18 is the Legal Age of Majority for the USA where I live 
					if (!GangCombat(tempgirl, gang)) gotitem = true;
					if (g_Dice.percent(20))		{ totalitems++; bringbacknum += 2; }
					else if (g_Dice.percent(50))	gold += 1 + g_Dice % 200;
				}
				else gotitem = true;
				if (gotitem)
				{
					bringbacknum += 4;
					totalitems++;
				}
				else bringbacknum += 2;
			}
			else									// get beast = 2 point
			{
				bool gotbeast = false;
				if (gang->net_limit() > 0)	// try to capture using net
				{
					while (gang->net_limit() > 0 && !gotbeast)
					{
						int damagechance = 50;	// higher damage net chance in the catacombs
						if (g_Dice.percent(gang->combat()))	// hit it with the net
						{
							if (!g_Dice.percent(60)) gotbeast = true;
							else damagechance = 80;
						}
						if (g_Dice.percent(damagechance)) { gang->net_limit(-1); m_NumNets--; }
					}
				}
				if (!gotbeast)	// fight it
				{
					// the last few members will runaway or allow the beast to run away so that the can still bring back what they have
					while (gang->m_Num > 1 + g_Dice % 3 && !gotbeast)
					{
						if (g_Dice.percent(min(90, gang->combat())))
						{
							gotbeast = true;
							continue;
						}
						if (gang->heal_limit() == 0)
						{
							gang->m_Num--;
							continue;
						}
						// `J` 5% chance they will not get the healing potion in time.
						// needed to have atleast some chance or else they are totally invincable.
						if (g_Dice.percent(5)) gang->m_Num--;
						gang->heal_limit(-1);
						m_NumHealingPotions--;
					}
				}
				if (gotbeast)
				{
					int numbeasts = 1 + g_Dice % 3;
					bringbacknum += numbeasts * 2;
					totalbeast += numbeasts;
				}
				else bringbacknum++;
			}
		}

		// determine loot
		if (gang->m_Num < 1) return false;	// they all died
		else
		{
			if (num == gang->m_Num)	ss << "All " << gang->m_Num << " of them return.\n \n";
			else ss << gang->m_Num << " of the " << num << " who went out return.\n \n";

			if (gold > 0)
			{
				ss << "They bring back with them:   " << gold << " gold\n \n";
				g_Gold.catacomb_loot(gold);
			}

			// get catacomb girls (is "monster" if trait not human)
			if (totalgirls > 0)
			{
				ss << "Your men captured " << totalgirls << " girl" << (totalgirls > 1 ? "s" : "") << ":\n";
				for (int i = 0; i < totalgirls; i++)
				{
					sGirl* ugirl = 0;
					bool unique = g_Dice.percent(cfg.catacombs.unique_catacombs());	// chance of getting unique girl
					if (unique)
					{
						ugirl = g_Girls.GetRandomGirl(false, true);
						if (ugirl == 0) unique = false;
					}
					if (unique)
					{
						ss << "   " << ugirl->m_Realname << "   (u)\n";
						ugirl->m_States &= ~(1 << STATUS_CATACOMBS);
						stringstream NGmsg;
						ugirl->add_trait("Kidnapped", 2 + g_Dice % 10);
						NGmsg << ugirl->m_Realname << " was captured in the catacombs by " << gang->m_Name << ".";
						ugirl->m_Events.AddMessage(NGmsg.str(), IMGTYPE_PROFILE, EVENT_GANG);
						m_Dungeon->AddGirl(ugirl, DUNGEON_GIRLCAPTURED);
					}
					else
					{
						ugirl = g_Girls.CreateRandomGirl(0, false, false, false, true);
						if (ugirl != 0)  // make sure a girl was returned
						{
							ss << "   " << ugirl->m_Realname << "\n";
							stringstream NGmsg;
							ugirl->add_trait("Kidnapped", 2 + g_Dice % 10);
							NGmsg << ugirl->m_Realname << " was captured in the catacombs by " << gang->m_Name << ".";
							ugirl->m_Events.AddMessage(NGmsg.str(), IMGTYPE_PROFILE, EVENT_GANG);
							m_Dungeon->AddGirl(ugirl, DUNGEON_GIRLCAPTURED);
						}
					}
					if (ugirl && g_Brothels.GetObjective() && g_Brothels.GetObjective()->m_Objective == OBJECTIVE_CAPTUREXCATACOMBGIRLS)
					{
						g_Brothels.GetObjective()->m_SoFar++;
					}
				}
			}
			ss << "\n";

			// get items
			if (totalitems > 0)
			{
				ss << "Your men bring back " << totalitems << " item" << (totalitems > 1 ? "s" : "") << ":\n";
				for (int i = 0; i < totalitems; i++)
				{
					bool quit = false; bool add = false;
					sInventoryItem* temp = g_InvManager.GetRandomCatacombItem();
					if (temp)
					{
						ss << "   " << temp->m_Name << "\n";
						int curI = g_Brothels.HasItem(temp->m_Name, -1);
						bool loop = true;
						while (loop)
						{
							if (curI != -1)
							{
								if (g_Brothels.m_NumItem[curI] >= 999)
									curI = g_Brothels.HasItem(temp->m_Name, curI + 1);
								else loop = false;
							}
							else loop = false;
						}

						if (g_Brothels.m_NumInventory < MAXNUM_INVENTORY || curI != -1)
						{
							if (curI != -1)
							{
								g_Brothels.m_NumItem[curI]++;
							}
							else
							{
								for (int j = 0; j < MAXNUM_INVENTORY; j++)
								{
									if (g_Brothels.m_Inventory[j] == 0)
									{
										g_Brothels.m_Inventory[j] = temp;
										g_Brothels.m_EquipedItems[j] = 0;
										g_Brothels.m_NumInventory++;
										g_Brothels.m_NumItem[j]++;
										break;
									}
								}
							}
						}
						else
						{
							quit = true;
							ss << "Your inventory is full\n";
						}
					}
					if (quit) break;
				}
			}
			ss << "\n";

			// bring back any beasts
			if (totalbeast > 0)
			{
				ss << "Your men " << (totalgirls + totalitems > 0 ? "also " : "") << "bring back " << totalbeast << " beasts.";
				g_Brothels.add_to_beasts(totalbeast);
			}
		}
	}

	gang->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_GANG);
	return true;
}

// `J` added for .06.02.41
bool cGangManager::service_mission(sGang* gang)
{
	if (cfg.debug.log_debug()) { g_LogFile.ss() << "Debug bool cGangManager::service_mission(sGang* gang) || Start"; g_LogFile.ssend(); }
	stringstream ss;
	ss << "Gang   " << gang->m_Name << "   spend the week helping out the community.";

	int susp = g_Dice.bell(0, 2), fear = g_Dice.bell(0, 2), disp = g_Dice.bell(0, 3), serv = g_Dice.bell(0, 3);
	int cha = 0, intl = 0, agil = 0, mag = 0, gold = 0, sec = 0, dirt = 0, beasts = 0;
	int percent = max(1, min(gang->m_Num * 5, gang->service()));
	
	for (int i = 0; i < gang->m_Num / 2; i++)
	{
		if (g_Dice.percent(percent))
		{
			switch (g_Dice % 9)
			{
			case 0:		susp++;		break;
			case 1:		fear++;		break;
			case 2:		disp++;		break;
			case 3:		cha++;		break;
			case 4:		intl++;		break;
			case 5:		agil++;		break;
			case 6:		mag++;		break;
			case 7:		gold += g_Dice % 10 + 1;	break;
			default:	serv++;		break;
			}
		}
	}

	if (gang->m_Num < 15 && g_Dice.percent(min(25 - gang->m_Num, gang->charisma())))
	{
		int addnum = max(1, g_Dice.bell(-2, 4));
		if (addnum + gang->m_Num > 15)	addnum = 15 - gang->m_Num;
		ss << "\n \n";
		/* */if (addnum <= 1)	{ addnum = 1;	ss << "A local boy"; }
		else if (addnum == 2)	{ ss << "Two locals"; }
		else /*            */	{ ss << "Some locals"; }
		ss << " decided to join your gang to help out their community.";
		gang->m_Num += addnum;
	}

	if (g_Dice.percent(max(10, min(gang->m_Num * 6, gang->intelligence()))))
	{
		sBrothel* brothel = g_Brothels.GetRandomBrothel();
		sec = max(5 + g_Dice % 26, gang->intelligence() / 4);
		dirt = max(5 + g_Dice % 26, gang->service() / 4);
		brothel->m_SecurityLevel += sec;
		brothel->m_Filthiness -= dirt;
		ss << "\n \nThey cleaned up around " << brothel->m_Name << "; fixing lights, removing debris and making sure the area is secure.";
	}
	if (g_Dice.percent(max(10, min(gang->m_Num * 6, gang->intelligence()))))
	{
		beasts += (max(1, g_Dice.bell(-4, 4)));
		ss << "\n \nThey rounded up ";
		if (beasts <= 1)		{ beasts = 1;	ss << "a"; }
		else if (beasts == 2)	{ ss << "two"; }
		else/*             */	{ ss << "some"; }
		ss << " stray beast" << (beasts > 1 ? "s" : "") << " and brought " << (beasts > 1 ? "them" : "it") << " to the brothel" << (g_Brothels.m_NumBrothels > 1 ? "s" : "") << ".";

		if (g_Dice.percent(beasts * 5))
		{
			string itemfound = "";
			switch (g_Dice % 4)
			{
			case 0:		itemfound = "Black Cat";		break;
			case 1:		itemfound = "Guard Dog";		break;
			default:	itemfound = "Cat";				break;
			}

			sInventoryItem* item = g_InvManager.GetItem(itemfound);
			if (item)
			{
				sBrothel* brothel = g_Brothels.GetRandomBrothel();
				sGirl* girl = g_Brothels.GetRandomGirl(brothel->m_id);
				if (girl->add_inv(item) != -1)						// see if a girl can take it
				{
					stringstream gss;
					gss << "While " << gang->m_Name << " was bringing in the ";
					if (beasts == 1)
					{
						gss << item->m_Name << " they cought, " << girl->m_Realname << " stopped them and begged them to let her keep it.";
					}
					else
					{
						gss << "beasts they captured, " << girl->m_Realname << " picked out a ";
						if (item->m_Name == "Guard Dog")	gss << "dog from the pack";
						else/*                        */	gss << item->m_Name;
						gss << " and claimed it for herself.";
					}
					girl->m_Events.AddMessage(gss.str(), IMGTYPE_PROFILE, EVENT_GANG);
					beasts--;
					ss << "\n" << gss.str() << "\n";
				}
				else if (g_Brothels.AddItemToInventory(item))				// otherwise put it in inventory
				{
					if (beasts == 1)	ss << "\nYou take the " << item->m_Name << " that they captured and put it in a cage in your office.\n";
					else/*        */	ss << "\nOne of the beasts they catch is a " << item->m_Name << " that looks healthy enough to give to a girl. You have it put in a cage and taken to your office.\n";
					beasts--;
				}
			}
		}
	}

	
	ss << "\n";
	if (sec > 0)	{ ss << "\nSecurity + " << sec; }
	if (dirt > 0)	{ ss << "\nFilthiness - " << dirt; }
	if (beasts > 0)	{ ss << "\nBeasts +" << beasts; }
	if (susp > 0)	{ The_Player->suspicion(-susp);						ss << "\nSuspicion -" << susp; }
	if (fear > 0)	{ The_Player->customerfear(-fear);					ss << "\nCustomer Fear -" << fear; }
	if (disp > 0)	{ The_Player->disposition(disp);					ss << "\nDisposition +" << disp; }
	if (serv > 0)	{ gang->AdjustGangSkill(SKILL_SERVICE, serv);		ss << "\nService +" << serv; }
	if (cha > 0)	{ gang->AdjustGangStat(STAT_CHARISMA, cha);			ss << "\nCharisma +" << cha; }
	if (intl > 0)	{ gang->AdjustGangStat(STAT_INTELLIGENCE, intl);	ss << "\nIntelligence +" << intl; }
	if (agil > 0)	{ gang->AdjustGangStat(STAT_AGILITY, agil);			ss << "\nAgility +" << agil; }
	if (mag > 0)	{ gang->AdjustGangSkill(SKILL_MAGIC, mag);			ss << "\nMagic +" << mag; }
	if (gold > 0)	{ g_Gold.misc_credit(gold);	ss << "\nThey recieved " << gold << " gold in tips from grateful people."; }

	gang->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_GANG);
	return true;
}

// `J` - updated for .06.01.09
bool cGangManager::gangtraining(sGang* gang)
{
	stringstream ss;
	ss << "Gang   " << gang->m_Name << "   spend the week training and improving their skills.\n \n";

	int old_combat = gang->combat();
	int old_magic = gang->magic();
	int old_intel = gang->intelligence();
	int old_agil = gang->agility();
	int old_const = gang->constitution();
	int old_char = gang->charisma();
	int old_str = gang->strength();
	int old_serv = gang->service();

	vector<int*> possible_skills;
	possible_skills.push_back(&gang->m_Skills[SKILL_COMBAT]);
	possible_skills.push_back(&gang->m_Skills[SKILL_MAGIC]);
	possible_skills.push_back(&gang->m_Stats[STAT_INTELLIGENCE]);
	possible_skills.push_back(&gang->m_Stats[STAT_AGILITY]);
	possible_skills.push_back(&gang->m_Stats[STAT_CONSTITUTION]);
	possible_skills.push_back(&gang->m_Stats[STAT_CHARISMA]);
	possible_skills.push_back(&gang->m_Stats[STAT_STRENGTH]);
	possible_skills.push_back(&gang->m_Skills[SKILL_SERVICE]);

	int count = (g_Dice % 3) + 2;  // get 2-4 potential skill/stats to boost
	for (int i = 0; i < count; i++)
	{
		int boost_count = (g_Dice % 3) + 1;  // boost each 1-3 times
		BoostGangRandomSkill(&possible_skills, 1, boost_count);
	}
	possible_skills.clear();

	if (gang->m_Skills[SKILL_COMBAT] > old_combat)		ss << "\n+" << (gang->m_Skills[SKILL_COMBAT] - old_combat) << " Combat";
	if (gang->m_Skills[SKILL_MAGIC] > old_magic)		ss << "\n+" << (gang->m_Skills[SKILL_MAGIC] - old_magic) << " Magic";
	if (gang->m_Stats[STAT_INTELLIGENCE] > old_intel)	ss << "\n+" << (gang->m_Stats[STAT_INTELLIGENCE] - old_intel) << " Intelligence";
	if (gang->m_Stats[STAT_AGILITY] > old_agil)			ss << "\n+" << (gang->m_Stats[STAT_AGILITY] - old_agil) << " Agility";
	if (gang->m_Stats[STAT_CONSTITUTION] > old_const)	ss << "\n+" << (gang->m_Stats[STAT_CONSTITUTION] - old_const) << " Toughness";
	if (gang->m_Stats[STAT_CHARISMA] > old_char)		ss << "\n+" << (gang->m_Stats[STAT_CHARISMA] - old_char) << " Charisma";
	if (gang->m_Stats[STAT_STRENGTH] > old_str)			ss << "\n+" << (gang->m_Stats[STAT_STRENGTH] - old_str) << " Strength";
	if (gang->m_Skills[SKILL_SERVICE] > old_serv)		ss << "\n+" << (gang->m_Skills[SKILL_SERVICE] - old_serv) << " Service";

	gang->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_GANG);
	gang->m_Combat = false;
	return false;
}

// `J` - updated for .06.01.09
bool cGangManager::gangrecruiting(sGang* gang)
{
	stringstream ss;
	ss << "Gang   " << gang->m_Name << "   is recruiting.\n \n";
	int recruit = 0;
	int start = g_Dice.bell(1, 6);		// 1-6 people are available for recruitment
	int available = start;
	int add = max(0, g_Dice.bell(0, 4) - 1);		// possibly get 1-3 without having to ask
	start += add;
	int disp = The_Player->disposition();
	while (available > 0)
	{
		int chance = gang->charisma();
		if (g_Dice.percent(gang->magic() / 4))			chance += gang->magic() / 10;
		if (g_Dice.percent(gang->combat() / 4))			chance += gang->combat() / 10;
		if (g_Dice.percent(gang->intelligence() / 4))	chance += gang->intelligence() / 10;
		if (g_Dice.percent(gang->agility() / 4))		chance += gang->agility() / 10;
		if (g_Dice.percent(gang->constitution() / 4))	chance += gang->constitution() / 10;
		if (g_Dice.percent(gang->strength() / 4))		chance += gang->strength() / 10;

		// less chance of them wanting to work for really evil or really good player
		if (disp < -50)				chance += (disp + 50) / 2;	// -25 for -100 disp
		if (disp > -20 && disp < 0)	chance += (22 + disp) / 2;	// +1 for -19  to +10 for -2
		if (disp == 0)				chance += 10;				// +10 for -2,-1,0,1,2
		if (disp < 20 && disp > 0)	chance += (22 - disp) / 2;	// +1 for 19   to +10 for 2
		if (disp > 50)				chance -= (disp - 50) / 3;	// -16 for > 98 disp

		if (chance > 90) chance = 90; if (chance < 20) chance = 20;	// 20-90% chance
		if (g_Dice.percent(chance)) add++;
		available--;
	}

	while (add > recruit && gang->m_Num < 15)
	{
		recruit++;
		gang->m_Num++;
	}

	if (start < 1) ss << "They were unable to find anyone to recruit.";
	else
	{
		ss << "They found "; if (start == 1) ss << "one person"; else ss << start << " people"; ss << " to try to recruit";

		if (start == 1) { if (add == start) ss << " and they got him"; else ss << " but he didn't want"; ss << " to join."; }
		else if (add <= 0)		ss << " but were unable to get any to join.";
		else if (add == start)	ss << " and managed to get all of them to join.";
		else if (add == 1)		ss << " but were only able to convince one of them to join.";
		else					ss << " and were able to convince " << add << " of them to join.";

		if (gang->m_Num >= 15 && add == recruit) ss << "\nThey got as many as they needed to fill their ranks.";
		else if (gang->m_Num >= 15 && add > recruit)
		{
			gang->m_Num = 15;
			ss << "\nThey only had room for ";
			if (recruit == 1) ss << "one"; else ss << recruit;
			ss << " more in their gang so they ";
			int passnum = add - recruit;
			sGang* passto = GetGangRecruitingNotFull(passnum);
			if (passto)
			{
				ss << "sent the rest to join " << passto->m_Name << ".";
				stringstream pss;
				pss << gang->m_Name << " sent " << passnum << " recruit" << (passnum > 1 ? "s" : "") << " that they had no room for to " << passto->m_Name;
				int passnumgotthere = 0;
				for (int i = 0; i < passnum; i++)
				{
					if (passto->m_MissionID == MISS_RECRUIT)	if (g_Dice.percent(75)) passnumgotthere++;
					if (passto->m_MissionID == MISS_TRAINING)	if (g_Dice.percent(50)) passnumgotthere++;
					if (passto->m_MissionID == MISS_SPYGIRLS)	if (g_Dice.percent(95)) passnumgotthere++;
					if (passto->m_MissionID == MISS_GUARDING)	if (g_Dice.percent(30)) passnumgotthere++;
					if (passto->m_MissionID == MISS_SERVICE)	if (g_Dice.percent(90)) passnumgotthere++;
				}
				if (passnumgotthere > 0)
				{
					if (passnumgotthere == passnum) pss << ".\nThey " << (passnum > 1 ? "all " : "") << "arrived ";
					else pss << ".\nOnly " << passnumgotthere << " arrived ";
					if (passto->m_Num + passnumgotthere <= 15)
						pss << "and got accepted into the gang.";
					else
					{
						passnumgotthere = 15 - passto->m_Num;
						pss << "but " << passto->m_Name << " could only take " << passnumgotthere << " of them.";
					}
					passto->m_Num += passnumgotthere;
				}
				else pss << " but none showed up.";
				passto->m_Events.AddMessage(pss.str(), IMGTYPE_PROFILE, EVENT_GANG);
			}
			else ss << "had to turn away the rest.";
		}
	}
	gang->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_GANG);
	gang->m_Combat = true;	// though not actually combat, this prevents the automatic +1 member at the end of the week
	return false;
}

// `J` - Added for .06.01.09
bool cGangManager::losegang(sGang* gang)
{
	if (gang->m_Num <= 0)
	{
		stringstream ss;
		int mission = gang->m_MissionID;
		ss << gang->m_Name << " was lost while ";
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
		g_MessageQue.AddToQue(ss.str(), COLOR_RED);
		sGang* temp = gang->m_Next;
		RemoveGang(gang);
		gang = temp;
		return true;
	}
	return false;
}

// `J` - Added for .06.01.09
void cGangManager::check_gang_recruit(sGang* gang)
{
	stringstream ss;
	if (gang->m_MissionID == MISS_SERVICE || gang->m_MissionID == MISS_TRAINING){}	// `J` service and training can have as few as 1 member doing it.
	else if (gang->m_Num <= 5 && gang->m_MissionID != MISS_RECRUIT)
	{
		ss << "Gang   " << gang->m_Name << "   were set to recruit due to low numbers";
		gang->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_WARNING);
		gang->m_AutoRecruit = true;
		gang->m_LastMissID = gang->m_MissionID;
		gang->m_MissionID = MISS_RECRUIT;
	}
	else if (gang->m_MissionID == MISS_RECRUIT && gang->m_Num >= 15)
	{
		if (gang->m_AutoRecruit)
		{
			ss << "Gang   " << gang->m_Name << "   were placed back on their previous mission now that their numbers are back to normal.";
			gang->m_MissionID = gang->m_LastMissID;
			gang->m_AutoRecruit = false;
		}
		else
		{
			ss << "Gang   " << gang->m_Name << "   were placed on guard duty from recruitment as their numbers are full.";
			gang->m_MissionID = MISS_GUARDING;
		}
		gang->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_WARNING);
	}
}

// `J` - Added for .06.01.09
void cGangManager::GangStartOfShift()
{
	if (cfg.debug.log_debug()) { g_LogFile.ss() << "Debug cGangManager::GangStartOfShift() || Start"; g_LogFile.ssend(); }
	cTariff tariff;
	stringstream ss;

	RestockNetsAndPots();

	if (m_NumGangs < 1) return;	// no gangs

	int totalpots = m_NumHealingPotions;
	int totalnets = m_NumNets;
	int gangsneedingnets = 0, gangsneedingpots = 0;
	int givepots = 0, givenets = 0;
	int potspassedout = 0, netspassedout = 0;


	// update goons for the start of the turn
	int cost = 0;
	sGang* currentGang = m_GangStart;
	while (currentGang)
	{
		if (currentGang->m_Num <= 0)	// clear dead
		{
			ss << "All of the men in gang " << currentGang->m_Name << " have died.";
			g_MessageQue.AddToQue(ss.str(), COLOR_RED);
//			sGang* temp = currentGang->m_Next;
			RemoveGang(currentGang);
	//		currentGang = temp;
			continue;
		}
		currentGang->m_Combat = false;
		currentGang->m_Events.Clear();
		cost += tariff.goon_mission_cost(currentGang->m_MissionID);	// sum up the cost of all the goon missions
		currentGang->m_Net_Limit = 0;
		currentGang->m_Heal_Limit = 0;

		check_gang_recruit(currentGang);

		if (currentGang->m_MissionID == MISS_SPYGIRLS)	currentGang->m_Events.AddMessage("Gang   " + currentGang->m_Name + "   is spying on your girls.", IMGTYPE_PROFILE, EVENT_GANG);
		if (currentGang->m_MissionID == MISS_GUARDING)	currentGang->m_Events.AddMessage("Gang   " + currentGang->m_Name + "   is guarding.", IMGTYPE_PROFILE, EVENT_GANG);

		currentGang = currentGang->m_Next;
	}
	g_Gold.goon_wages(cost);

	if (m_NumGangs < 1) return;	// all gangs died?
	if (totalpots == 0 && totalnets == 0) return;	// no potions or nets

	// check numbers needed
	currentGang = m_GangStart;
	while (currentGang)
	{
		switch (currentGang->m_MissionID)
		{
		case MISS_SPYGIRLS: case MISS_GUARDING: case MISS_SABOTAGE: case MISS_EXTORTION: case MISS_PETYTHEFT: case MISS_GRANDTHEFT:
			gangsneedingpots++; break;
		case MISS_CAPTUREGIRL: case MISS_KIDNAPP: case MISS_CATACOMBS:
			gangsneedingpots++;  gangsneedingnets++; break;
		default:	break;
		}
		currentGang = currentGang->m_Next;
	}

	if (gangsneedingnets == 0 && gangsneedingpots == 0) return;	// no gang needs any so don't pass them out

	if (totalpots > 0 && gangsneedingpots > 0)	givepots = 1 + (totalpots / gangsneedingpots);
	if (totalnets > 0 && gangsneedingnets > 0)	givenets = 1 + (totalnets / gangsneedingnets);

	currentGang = m_GangStart;
	while (currentGang)
	{
		if (potspassedout + givepots > totalpots) givepots = totalpots - potspassedout;
		if (netspassedout + givenets > totalnets) givenets = totalnets - netspassedout;

		switch (currentGang->m_MissionID)
		{
		case MISS_GUARDING: case MISS_SPYGIRLS:	case MISS_SABOTAGE: case MISS_EXTORTION: case MISS_PETYTHEFT: case MISS_GRANDTHEFT:
			currentGang->set_heal_limit(givepots);	potspassedout += givepots;
			break;
		case MISS_CAPTUREGIRL: case MISS_KIDNAPP: case MISS_CATACOMBS:
			currentGang->set_heal_limit(givepots);	potspassedout += givepots;
			currentGang->set_net_limit(givenets);	netspassedout += givenets;
			break;
		default:
			break;
		}
		currentGang = currentGang->m_Next;
	}
	if (cfg.debug.log_debug()) { g_LogFile.ss() << "Debug cGangManager::GangStartOfShift() || end"; g_LogFile.ssend(); }
}