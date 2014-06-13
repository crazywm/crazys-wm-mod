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

extern char buffer[1000];

static cPlayer* m_Player = g_Brothels.GetPlayer();
static cRivalManager* m_Rivals = g_Brothels.GetRivalManager();
static cDungeon* m_Dungeon = g_Brothels.GetDungeon();


cGangManager::cGangManager()
{
	m_NumGangNames = 0;
	ifstream in;
	// WD: Typecast to resolve ambiguous call in VS 2010
	DirPath dp = DirPath() <<	"Resources" << "Data" << "HiredGangNames.txt";
	in.open(dp.c_str());
	//in.open(DirPath() << "Resources" << "Data" << "HiredGangNames.txt");
	in>>m_NumGangNames;
	in.close();

	m_NumGangs = 0;
	m_GangStart = m_GangEnd = 0;

	m_NumHireableGangs = 0;
	m_HireableGangStart = m_HireableGangEnd = 0;

	m_BusinessesExtort = 0;

	m_NumHealingPotions = m_NumNets = m_SwordLevel = 0;
	m_KeepHealStocked = m_KeepNetsStocked = false;
}

void cGangManager::Free()
{

//	if(m_GoonMissions)
//		delete m_GoonMissions;
//	m_GoonMissions = 0;
//	m_LastGoonMission = 0;

	if(m_GangStart)
		delete m_GangStart;
	m_NumGangs = 0;
	m_GangStart = m_GangEnd = 0;

	if(m_HireableGangStart)
		delete m_HireableGangStart;
	m_NumHireableGangs = 0;
	m_HireableGangStart = m_HireableGangEnd = 0;

	m_BusinessesExtort = 0;

	m_NumHealingPotions = m_SwordLevel = m_NumNets = 0;
	m_KeepHealStocked = m_KeepNetsStocked = false;
}

void cGangManager::LoadGangsLegacy(ifstream& ifs)
{
	Free();
	int temp;
	string message = "";

	// load goons and goon missions
	if (ifs.peek()=='\n') ifs.ignore(1,'\n');
	int numGangsToLoad = 0;
	ifs>>numGangsToLoad;
	m_NumGangs = 0;
	for(int p=0; p<numGangsToLoad; p++)
	{
		if (ifs.peek()=='\n') ifs.ignore(1,'\n');
		sGang* gang = new sGang();
		ifs>>gang->m_Num>>gang->m_MissionID>>gang->m_LastMissID;

		if (ifs.peek()=='\n') ifs.ignore(1,'\n');
		// load their skills
		for(u_int i=0; i<NUM_SKILLS; i++)
		{
			temp = 0;
			ifs>>temp;
			gang->m_Skills[i] = temp;
		}

		// load their stats
		if(ifs.peek()=='\n') ifs.ignore(1,'\n');
		for(int i=0; i<NUM_STATS; i++)
		{
			temp = 0;
			ifs>>temp;
			gang->m_Stats[i] = temp;
		}

		// load their name
		if(ifs.peek()=='\n') ifs.ignore(1,'\n');
		ifs.getline(buffer, sizeof(buffer), '\n');
		gang->m_Name = buffer;

		// load the combat bool
		if (ifs.peek()=='\n') ifs.ignore(1,'\n');
		ifs>>temp;
		if(temp == 1)
			gang->m_Combat = true;
		else
			gang->m_Combat = false;

		// load the auto recruit bool
		if (ifs.peek()=='\n') ifs.ignore(1,'\n');
		ifs>>temp;
		if(temp == 1)
			gang->m_AutoRecruit = true;
		else
			gang->m_AutoRecruit = false;

		AddGang(gang);
	}

	// load hireable goons
	if (ifs.peek()=='\n') ifs.ignore(1,'\n');
	ifs>>numGangsToLoad;
	m_NumHireableGangs = 0;
	for(int p=0; p<numGangsToLoad; p++)
	{
		if (ifs.peek()=='\n') ifs.ignore(1,'\n');
		sGang* hgang = new sGang();
		ifs>>hgang->m_Num;

		if (ifs.peek()=='\n') ifs.ignore(1,'\n');
		// load their skills
		for(u_int i=0; i<NUM_SKILLS; i++)
		{
			temp = 0;
			ifs>>temp;
			hgang->m_Skills[i] = temp;
		}

		// load their stats
		if(ifs.peek()=='\n') ifs.ignore(1,'\n');
		for(int i=0; i<NUM_STATS; i++)
		{
			temp = 0;
			ifs>>temp;
			hgang->m_Stats[i] = temp;
		}

		// load their name
		if(ifs.peek()=='\n') ifs.ignore(1,'\n');
		ifs.getline(buffer, sizeof(buffer), '\n');
		hgang->m_Name = buffer;

		AddHireableGang(hgang);
	}

	// load businesses extorted
	if (ifs.peek()=='\n') ifs.ignore(1,'\n');
	ifs>>m_BusinessesExtort;

	// load weapon level, healing potions, nets
	if(ifs.peek()=='\n') ifs.ignore(1,'\n');
	ifs>>m_SwordLevel>>m_NumHealingPotions>>m_NumNets;

	// load healing potions restock
	if (ifs.peek()=='\n') ifs.ignore(1,'\n');
	ifs>>m_KeepHealStocked;

	// load nets restock
	if (ifs.peek()=='\n') ifs.ignore(1,'\n');
	ifs>>m_KeepNetsStocked;
}

bool cGangManager::LoadGangsXML(TiXmlHandle hGangManager)
{
	Free();//everything should be init even if we failed to load an XML element
	TiXmlElement* pGangManager = hGangManager.ToElement();
	if (pGangManager == 0)
	{
		return false;
	}

	// load goons and goon missions
	m_NumGangs = 0;
	TiXmlElement* pGangs = pGangManager->FirstChildElement("Gangs");
	if (pGangs)
	{
		for(TiXmlElement* pGang = pGangs->FirstChildElement("Gang");
			pGang != 0;
			pGang = pGang->NextSiblingElement("Gang"))
		{
			sGang* gang = new sGang();
			bool success = gang->LoadGangXML(TiXmlHandle(pGang));
			if (success == true)
			{
				AddGang(gang);
			}
			else
			{
				delete gang;
				continue;
			}
		}
	}

	// load hireable goons
	m_NumHireableGangs = 0;
	TiXmlElement* pHireables = pGangManager->FirstChildElement("Hireables");
	if (pHireables)
	{
		for(TiXmlElement* pGang = pHireables->FirstChildElement("Gang");
			pGang != 0;
			pGang = pGang->NextSiblingElement("Gang"))
		{
			sGang* hgang = new sGang();
			bool success = hgang->LoadGangXML(TiXmlHandle(pGang));
			if (success == true)
			{
				AddHireableGang(hgang);
			}
			else
			{
				delete hgang;
				continue;
			}
		}
	}

	// load businesses extorted
	pGangManager->QueryIntAttribute("BusinessesExtort", &m_BusinessesExtort);

	// load weapon level, healing potions, nets
	pGangManager->QueryIntAttribute("SwordLevel", &m_SwordLevel);
	pGangManager->QueryIntAttribute("NumHealingPotions", &m_NumHealingPotions);
	pGangManager->QueryIntAttribute("NumNets", &m_NumNets);

	// load healing potions restock
	pGangManager->QueryIntAttribute("KeepHealStocked", &m_KeepHealStocked);

	// load nets restock
	pGangManager->QueryIntAttribute("KeepNetsStocked", &m_KeepNetsStocked);
	return true;
}

TiXmlElement* cGangManager::SaveGangsXML(TiXmlElement* pRoot)
{
	TiXmlElement* pGangManager = new TiXmlElement("Gang_Manager");
	pRoot->LinkEndChild(pGangManager);

	// save gangs
	TiXmlElement* pGangs = new TiXmlElement("Gangs");
	pGangManager->LinkEndChild(pGangs);
	sGang* gang = m_GangStart;
	while(gang)
	{
		TiXmlElement* pGang = gang->SaveGangXML(pGangs);

		pGang->SetAttribute("MissionID", gang->m_MissionID);
		pGang->SetAttribute("LastMissID", gang->m_LastMissID);

		// save the combat boolean
		pGang->SetAttribute(gettext("Combat"), gang->m_Combat);

		// save the auto recruit boolean
		pGang->SetAttribute("AutoRecruit", gang->m_AutoRecruit);

		gang = gang->m_Next;
	}

	// save hireable gangs
	TiXmlElement* pHireables = new TiXmlElement("Hireables");
	pGangManager->LinkEndChild(pHireables);
	sGang* hgang = m_HireableGangStart;
	while(hgang)
	{
		hgang->SaveGangXML(pHireables);
		hgang = hgang->m_Next;
	}

	// save businesses extorted
	pGangManager->SetAttribute("BusinessesExtort", m_BusinessesExtort);

	// save weapon level, healing potions, nets
	pGangManager->SetAttribute("SwordLevel", m_SwordLevel);
	pGangManager->SetAttribute("NumHealingPotions", m_NumHealingPotions);
	pGangManager->SetAttribute("NumNets", m_NumNets);

	// save heal restock
	pGangManager->SetAttribute("KeepHealStocked", m_KeepHealStocked);

	// save net restock
	pGangManager->SetAttribute("KeepNetsStocked", m_KeepNetsStocked);
	return pGangManager;
}

TiXmlElement* sGang::SaveGangXML(TiXmlElement* pRoot)
{
	TiXmlElement* pGang = new TiXmlElement("Gang");
	pRoot->LinkEndChild(pGang);

	pGang->SetAttribute("Num", m_Num);

	// save their skills
	SaveSkillsXML(pGang, m_Skills);

	// save their stats
	SaveStatsXML(pGang, m_Stats);

	// save their name
	pGang->SetAttribute("Name", m_Name);
	return pGang;
}

bool sGang::LoadGangXML(TiXmlHandle hGang)
{
	TiXmlElement* pGang = hGang.ToElement();
	if (pGang == 0)
	{
		return false;
	}

	if (pGang->Attribute("Name"))
	{
		m_Name = pGang->Attribute("Name");
	}
	// load their skills
	LoadSkillsXML(hGang.FirstChild("Skills"), m_Skills);

	// load their stats
	LoadStatsXML(hGang.FirstChild("Stats"), m_Stats);

	pGang->QueryIntAttribute("Num", &m_Num);

	//these may not have been saved
	//if not, the query just does not set the value
	//so the default is used, assuming the gang was properly init
	{
		pGang->QueryValueAttribute<u_int>("MissionID", &m_MissionID);
		pGang->QueryIntAttribute("LastMissID", &m_LastMissID);

		// load the combat boolean
		pGang->QueryValueAttribute<bool>(gettext("Combat"), &m_Combat);

		// load the auto recruit boolean
		pGang->QueryValueAttribute<bool>("AutoRecruit", &m_AutoRecruit);
	}
	return true;
}

// ----- Hire fire

void cGangManager::HireGang(int gangID)
{
	sGang* currentGang = m_HireableGangStart;
	int count = 0;
	while(currentGang)
	{
		if(count == gangID)
			break;
		count++;
		currentGang = currentGang->m_Next;
	}
	if(currentGang)
	{
		sGang* copyGang = new sGang();
		*copyGang = *currentGang;
		copyGang->m_Next = copyGang->m_Prev = 0;
		copyGang->m_Combat = copyGang->m_AutoRecruit = false;
		copyGang->m_LastMissID = -1;
		if(copyGang->m_Num <= 5)
			copyGang->m_MissionID = MISS_RECRUIT;

		AddGang(copyGang);

		RemoveHireableGang(gangID);
	}
}

void cGangManager::FireGang(int gangID)
{
	sGang* currentGang = m_GangStart;
	int count = 0;
	while(currentGang)
	{
		if(count == gangID)
			break;
		count++;
		currentGang = currentGang->m_Next;
	}
	if(currentGang)
	{
		cConfig cfg;
		if(m_NumHireableGangs < cfg.gangs.max_recruit_list())
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

	cConfig cfg;
	int max_members = cfg.gangs.init_member_max();
	int min_members = cfg.gangs.init_member_min();
	newGang->m_Num = min_members + g_Dice%(max_members + 1 - min_members);
	if(boosted)
		newGang->m_Num += 5;
	if(newGang->m_Num > 15)
		newGang->m_Num = 15;

	int new_val;
	for(u_int i=0; i<NUM_SKILLS; i++)
	{
		new_val = (1+(g_Dice%30))+20;
		if(g_Dice%5 == 1)
			new_val += 1+g_Dice%10;
		if(boosted)
			new_val += 10+g_Dice%11;
		newGang->m_Skills[i] = new_val;
	}
	for(int i=0; i<NUM_STATS; i++)
	{
		new_val = (1+(g_Dice%30))+20;
		if(g_Dice%5 == 1)
			new_val += g_Dice%10;
		if(boosted)
			new_val += 10+g_Dice%10;
		newGang->m_Stats[i] = new_val;
	}

	char buffer[256];
	bool done = false;
	ifstream in;
	// WD: Typecast to resolve ambiguous call in VS 2010
	DirPath dp = DirPath() <<	"Resources" << "Data" << "HiredGangNames.txt";
	in.open(dp.c_str());
	//in.open(DirPath() <<	"Resources" << "Data" << "HiredGangNames.txt");
	while(!done)
	{
		in.seekg(0);
		int name = g_Dice%m_NumGangNames;
		in>>m_NumGangNames;	// ignore the first line
		for(int i=0; i<=name; i++)
		{
			if (in.peek()=='\n') in.ignore(1,'\n');
			in.getline(buffer, sizeof(buffer), '\n');
		}
		done = true;
		sGang* curr = m_GangStart;
		while(curr)
		{
			if(curr->m_Name == buffer)
			{
				done = false;
				break;
			}
			curr = curr->m_Next;
		}
		curr = m_HireableGangStart;
		while(curr)
		{
			if(curr->m_Name == buffer)
			{
				done = false;
				break;
			}
			curr = curr->m_Next;
		}
	}
	newGang->m_Name = buffer;
	in.close();

	if(m_HireableGangStart)
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
	if(m_HireableGangStart)
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
	if(m_GangStart)
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
	while(currentGang)
	{
		if(count == gangID)
			break;
		count++;
		currentGang = currentGang->m_Next;
	}

	RemoveHireableGang(currentGang);
}

void cGangManager::RemoveHireableGang(sGang* gang)
{
	if(gang)
	{
		if(gang->m_Prev == 0)
		{
			m_HireableGangStart = gang->m_Next;
			if(m_HireableGangStart)
				m_HireableGangStart->m_Prev = 0;
			else
				m_HireableGangEnd = 0;
			gang->m_Next = 0;
			delete gang;
			gang = 0;
		}
		else if(gang->m_Next == 0)
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
	if(gang)
	{
		if(gang->m_Prev == 0)
		{
			m_GangStart = gang->m_Next;
			if(m_GangStart)
				m_GangStart->m_Prev = 0;
			else
				m_GangEnd = 0;
			gang->m_Next = 0;
			delete gang;
			gang = 0;
		}
		else if(gang->m_Next == 0)
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
	while(currentGang)
	{
		if(count == gangID)
			break;
		count++;
		currentGang = currentGang->m_Next;
	}

	// Pass the acquired gang to the other RemoveGang function
	RemoveGang(currentGang);
}

// ----- Get

int cGangManager::GetNumGangs()
{
	return (int)m_NumGangs;
}

int cGangManager::GetNumHireableGangs()
{
	return (int)m_NumHireableGangs;
}

sGang* cGangManager::GetTempGang()
{
	sGang* newGang = new sGang();
	newGang->m_Num = 15;
	for(u_int i=0; i<NUM_SKILLS; i++)
		newGang->m_Skills[i] = (1+(g_Dice%30))+20;
	for(int i=0; i<NUM_STATS; i++)
		newGang->m_Stats[i] = (1+(g_Dice%30))+20;
	return newGang;
}

sGang* cGangManager::GetGang(int gangID)
{
	// first find the gang
	sGang* currentGang = m_GangStart;
	int count = 0;
	while(currentGang)
	{
		if(count == gangID)
			break;
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
		if(currentGang->m_Next == 0)
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
	while(currentGang)
	{
		if(count == gangID)
			break;
		count++;
		if(currentGang->m_Next == 0)
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
	for(u_int i=0; i<NUM_SKILLS; i++)
		newGang->m_Skills[i] = g_Dice%30 + 51; 
	for(int i=0; i<NUM_STATS; i++)
		newGang->m_Stats[i] = g_Dice%30 + 51; 

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
	for(u_int i = 0; i < v.size(); i++) {
		sGang *gang = v[i];
		for(int j = 0; j < gang->m_Num; j++) {
			list.push_back(gang);
		}
	}
/*
 *	make sure the list isn't empty
 */
	if(list.size() == 0) {
		return 0;
	}
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
	for(int j = 0; j < count; j++)  // we'll pick and boost a skill/stat "count" number of times
	{
		int *affect_skill = 0;
		int total_chance = 0;
		vector<int> chance;

		for(int i = 0; i < (int)possible_skills->size(); i++)
		{  // figure chances for each skill/stat; more likely to choose those they're better at
			chance.push_back((int)pow((float)*possible_skills->at(i), 2));
			total_chance += chance[i];
		}
		int choice = g_Dice.random(total_chance);

		total_chance = 0;
		for(int i = 0; i < (int)chance.size(); i++)
		{
			if(choice < (chance[i] + total_chance))
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
	for(int j = 0; j < count; j++)  // we'll boost the skill/stat "count" number of times
	{
		if(*affect_skill < 1)
			*affect_skill = 1;

		double boost_amount = pow(70 / (double)*affect_skill, 2);
		if(boost_amount > 5)
			boost_amount = 5;

		boost_amount = (double)g_Dice.in_range(int((boost_amount/2)*100), int((boost_amount*1.5)*100)) / 100;
		char one_more = g_Dice.percent(int((boost_amount - (int)boost_amount) * 100)) ? 1 : 0;
		char final_boost = (char)boost_amount + one_more;

		*affect_skill += final_boost;

		if(*affect_skill > 100)
			*affect_skill = 100;
	}
}

// ----- Combat

bool cGangManager::GangBrawl(sGang* gang1, sGang* gang2)
{
	cTariff tariff;

	u_int g2attack = SKILL_COMBAT;	// determined later, defaults to combat
	u_int g1attack = SKILL_COMBAT;
	int initalNumber1 = 0;
	int initalNumber2 = 0;
	int g1dodge = 0;
	int g2dodge = 0;
	int g2HealPot = 10;

	if(gang1 == 0 || gang2 == 0)
		return true;
	else if(gang1->m_Num == 0)
		return false;

	initalNumber1 = gang1->m_Num;
	initalNumber2 = gang1->m_Num;

	gang1->m_Combat = true;

	g1dodge = gang1->m_Stats[STAT_AGILITY];
	g2dodge = gang2->m_Stats[STAT_AGILITY];

	int tmp = 0;
	if(gang1->m_Num > gang2->m_Num)
		tmp = gang1->m_Num;
	else
		tmp = gang2->m_Num;

	gang1->m_Combat = true;

	int heal_lim = healing_limit();
	for(int i=0; i<tmp; i++)
	{
		int g1Health = 100;		
		int g1Mana = 100;

		int g2Health = 100;		
		int g2Mana = 100;

		g1attack = SKILL_MAGIC;
		g2attack = SKILL_MAGIC;

		while(g1Health > 0 && g2Health > 0)
		{
			// gang1 attacks
			if((g_Dice%100)+1 <= gang1->m_Skills[g1attack])
			{
				int damage = (m_SwordLevel+1)*5;
				if(g1attack == SKILL_MAGIC)
				{
					if(g1Mana <= 0)
						g1attack = SKILL_COMBAT;
					else
					{
						damage += 10;
						g1Mana -= 7;
					}
				}

				// gang 2 attempts Dodge
				if((g_Dice%100)+1 >= g2dodge)
				{
					damage = (damage-(gang2->m_Stats[STAT_CONSTITUTION]/15));
					g2Health -= damage;
				}
			}

			// gang2 use healing potions
			if(g2HealPot > 0 && g2Health <= 40)
			{
				g2HealPot--;
				g2Health += 30;
			}

			// gang2 Attacks
			if((g_Dice%100)+1 <= gang2->m_Skills[g2attack])
			{
				int damage = (m_SwordLevel+1)*5;
				if(g2attack == SKILL_MAGIC)
				{
					if(g2Mana <= 0)
						g2attack = SKILL_COMBAT;
					else
					{
						damage += 10;
						g2Mana -= 7;
					}
				}

				if((g_Dice%100)+1 >= g1dodge)
				{
					damage = (damage-(gang1->m_Stats[STAT_CONSTITUTION]/15));
					g1Health -= damage;
				}
			}

			// gang1 use healing potions
			if(heal_lim > 0 && g1Health <= 40)
			{
				heal_lim --;
				m_NumHealingPotions--;
				g1Health += 30;
			}

			// update gang1 dodge ability
			if((g1dodge - 1) < 0)
				g1dodge = 0;
			else
				g1dodge--;

			// update gang2 dodge ability
			if((g2dodge - 1) < 0)
				g2dodge = 0;
			else
				g2dodge--;
		}

		if(g2Health <= 0)
			gang2->m_Num--;
		if(gang2->m_Num == 0)
		{
			BoostGangCombatSkills(gang1, 2);
			return true;
		}

		if(g1Health <= 0)
			gang1->m_Num--;
		if(gang1->m_Num == 0)
		{
			BoostGangCombatSkills(gang2, 2);
			return false;
		}

		if((initalNumber2/2) > gang2->m_Num)	// if the gang2 has lost half its number there is a 40% chance they will run away
		{
			if((1+(g_Dice%100)) < 40)
			{
				BoostGangCombatSkills(gang1, 2);
				return true;	// the men run away
			}
		}

		if((initalNumber1/2) > gang1->m_Num)	// if the gang has lost half its number there is a 40% chance they will run away
		{
			if((1+(g_Dice%100)) < 40)
			{
				BoostGangCombatSkills(gang2, 2);
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
	if(girl->has_trait("Incorporeal") || girl->has_trait("Incorporial"))
	{
		girl->m_Stats[STAT_HEALTH] = 100;
		l.ss()	<< gettext("\nGirl vs. Goons: ") << girl->m_Realname << gettext(" is incorporeal, so she wins.\n");
		gang->m_Num = (int) gang->m_Num / 2;
		while (gang->m_Num > 0)   // Do the casualty calculation
		{
			if (g_Dice%100 <= 40)
				gang->m_Num--;
			else
				break;
		}
		l.ss()	<< gettext("  ") << gang->m_Num << gettext(" goons escaped with their lives.\n");
		return true;
	}

	int dodge = 0;
	u_int attack = SKILL_COMBAT;	// determined later, defaults to combat
	u_int gattack = SKILL_COMBAT;

	if(gang == 0)
		return true;
	else if(gang->m_Num == 0)
		return true;

	int initalNumber = gang->m_Num;

	// first determine what she will fight with
	if(g_Girls.GetSkill(girl, SKILL_COMBAT) >= g_Girls.GetSkill(girl, SKILL_MAGIC))
		attack = SKILL_COMBAT;
	else
		attack = SKILL_MAGIC;

	// determine how gang will fight
	if(gang->m_Skills[SKILL_COMBAT] >= gang->m_Skills[SKILL_MAGIC])
		gattack = SKILL_COMBAT;
	else
		gattack = SKILL_MAGIC;

	if((g_Girls.GetStat(girl, STAT_AGILITY) - g_Girls.GetStat(girl, STAT_TIREDNESS)) < 0)
		dodge = 0;
	else
		dodge = (g_Girls.GetStat(girl, STAT_AGILITY) - g_Girls.GetStat(girl, STAT_TIREDNESS));

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
	int heal_lim = healing_limit();

	l.ss()	<< gettext("Girl vs. Goons: ")
		<< girl->m_Realname
		<< gettext(" fights ")
		<< num_goons 
		<< gettext(" opponents!")
	;
	l.ss()	<< girl->m_Realname 
		<< gettext(": Health ") << girl->health()
		<< gettext(", Dodge ") << dodge
		<< gettext(", Mana ") << girl->mana()
	;
	l.ssend();

	for(int i=0; i<num_goons; i++)
	{
		int gHealth = 100;
		int gDodge = gang->m_Stats[STAT_AGILITY];
		int gMana = 100;

		l.ss()	<< gettext("	Goon #") << i
			<< gettext(": Health 100, Dodge ") << gDodge
			<< gettext(", Mana ");
		l.ssend();

		while(g_Girls.GetStat(girl, STAT_HEALTH) >= 20 && gHealth > 0)
		{

			// Girl attacks
			l.ss()	<< gettext("\t\t")
				<< girl->m_Realname
				<< gettext(" attacks the goon.")
			;
			l.ssend();

			if(attack == SKILL_MAGIC) {

				if(girl->mana() < 7) {
					l.ss()	<< gettext("\t\t")
						<< girl->m_Realname
						<< gettext(" insufficient mana: using combat")
					;
					l.ssend();
				}
				else {

					girl->mana(-7);
					l.ss()	<< gettext("\t\t")
						<< girl->m_Realname
						<< gettext(" casts a spell; mana now ")
						<< girl->mana()
					;
					l.ssend();
				}
			}
			else {
				l.ss()	<< gettext("\t\t")
					<< girl->m_Realname
					<< gettext(" using physical attack")
				;
				l.ssend();
			}

			int girl_attack_chance = g_Girls.GetSkill(girl, attack);
			int die_roll = g_Dice.d100();

			l.ss()	<< gettext("\t\t")
				<< gettext(" attack chance = ")
				<< girl_attack_chance
			;
			l.ssend();

			l.ss()	<< gettext("\t\t")
				<< gettext(" die roll = ")
				<< die_roll
			;
			l.ssend();


			if(die_roll > girl_attack_chance) {
				l.ss()	<< gettext("\t\t\t")
					<< gettext(" attack fails")
				;
			}
			else {
				int damage = 5;

				l.ss()	<< gettext("\t\t\t")
					<< gettext(" attack hits! base damage is")
					<< damage
					<< gettext(".")
				;
				l.ssend();

/*
 *				she has hit now calculate how much damage will be done
 *
 *				magic works better than conventional violence.
 *				Link this amount of mana remaining?
 */
				int bonus = 0;
				if(attack == SKILL_MAGIC) {
					bonus = girl->magic()/5 + 2;
				}
				else {
					bonus = girl-> combat()/10;
				}

				l.ss()	<< gettext("\t\t")
					<< bonus 
					<< gettext(" points damage bonus.")
				;
				l.ssend();
				damage += bonus;

/*
 *				she may improve a little
 *				(checked every round of combat? seems excessive)
 */
				int gain = g_Dice % 2;
				if(gain) {
					l.ss()	<< gettext("\t\t")
						<<  girl->m_Realname
						<< gettext(" gains +")
						<< gain 
						<< gettext(" to attack skill")
					;
					l.ssend();
					g_Girls.UpdateSkill(girl, attack, gain);
				}

				die_roll = g_Dice.d100();

				// Goon attempts Dodge
				l.ss()	<< gettext("\t\t")
					<< gettext("Goon tries to dodge: needs ")
					<< gDodge
					<< gettext(", gets ")
					<< die_roll
					<< gettext(": ")
				;
				l.ssend();

				if(die_roll <= gDodge) {
					l.ss()	<< gettext("\t\t")
						<< gettext("success!");
				}
				else {
					int con_mod = gang->m_Stats[STAT_CONSTITUTION] / 10;
					gHealth -= con_mod;
					l.ss()	<< gettext("\t\t")
						<< gettext("failure!\n")
						<< gettext("\t\t")
						<< gettext("Goon takes ")
						<< damage 
						<< gettext(" damage, less ")
						<< con_mod
						<< gettext(" for CON\n")
						<< gettext("\t\t")
						<< gettext("New health value = ")
						<< con_mod
					;
				}
			}
			l.ssend();

			// goons use healing potions
			if(heal_lim > 0 && gHealth <= 40) {
				m_NumHealingPotions--;
				heal_lim --;
				gHealth += 30;
				l.ss()	<< gettext("Goon drinks healing potion: ")
					<< gettext("new health value = ") << gHealth
					<< gettext(". Gang has ")
					<< heal_lim
					<< gettext(" remaining.")
					;
				l.ssend();
			}

			// Goon Attacks

			die_roll = g_Dice.d100();
			int goon_attack_chance = gang->m_Skills[gattack];
			l.ss()	<< gettext("\tGoon Attack: ");
			l.ssend();


			l.ss()	<< gettext("\t\t")
				<< gettext("chance = ")
				<< goon_attack_chance
				<< gettext(", die roll = ")
				<< die_roll
				<< gettext(": ")
			;
			if(die_roll > goon_attack_chance) {
				l.ss() 	<< gettext(" attack fails!");
				l.ssend();
			}
			else {
				l.ss() 	<< gettext(" attack succeeds!");
				l.ssend();

				int damage = (m_SwordLevel+1)*5;
				if(gattack == SKILL_MAGIC)
				{
					if(gMana <= 0)
						gattack = SKILL_COMBAT;
					else
					{
						damage += 10;
						gMana -= 7;
					}
				}

				// girl attempts Dodge
				if((g_Dice%100)+1 >= dodge)
				{
					damage = (damage-(g_Girls.GetStat(girl, STAT_CONSTITUTION)/10));
					g_Girls.UpdateStat(girl,STAT_HEALTH,-damage);
				}
			}
			
			// update girls dodge ability
			if((dodge - 1) < 0)
				dodge = 0;
			else
				dodge--;

			// update goons dodge ability
			if((gDodge - 1) < 0)
				gDodge = 0;
			else
				gDodge--;

			if(g_Girls.GetStat(girl, STAT_HEALTH) < 30 && g_Girls.GetStat(girl, STAT_HEALTH) > 20)
			{
				if(g_Dice%100 < g_Girls.GetStat(girl, STAT_AGILITY))
				{
					BoostGangCombatSkills(gang, 2);
					g_Girls.UpdateEnjoyment(girl, ACTION_COMBAT, -1, true);
					return false;
				}
			}
		}

		if(g_Girls.GetStat(girl, STAT_HEALTH) <= 20)
		{
			BoostGangCombatSkills(gang, 2);
			g_Girls.UpdateEnjoyment(girl, ACTION_COMBAT, -1, true);
			return false;
		}
		else
			gang->m_Num--;

		if((initalNumber/2) > gang->m_Num)	// if the gang has lost half its number there is a 40% chance they will run away
		{
			if((1+(g_Dice%100)) <= 40)
			{
				g_Girls.UpdateEnjoyment(girl, ACTION_COMBAT, +1, true);
				return true;	// the men run away
			}
		}
		if(gang->m_Num == 0)
		{
			g_Girls.UpdateEnjoyment(girl, ACTION_COMBAT, +1, true);
			return true;
		}
	}

	l.ss()	<< gettext("No more opponents: ")
		<< girl->m_Realname
		<< gettext(" WINS!")
	;
	l.ssend();

	g_Girls.UpdateEnjoyment(girl, ACTION_COMBAT, +1, true);

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
	if(girl->has_trait("Incorporeal") || girl->has_trait("Incorporial"))
	{
		girl->m_Stats[STAT_HEALTH] = 100;
		l.ss()	<< gettext("\nGirl vs. Goons: ") << girl->m_Realname << gettext(" is incorporeal, so she wins.\n");
		enemy_gang->m_Num = (int) enemy_gang->m_Num / 2;
		while (enemy_gang->m_Num > 0)   // Do the casualty calculation
		{
			if (g_Dice%100 <= 40)
				enemy_gang->m_Num--;
			else
				break;
		}
		l.ss()	<< gettext("  ") << enemy_gang->m_Num << gettext(" goons escaped with their lives.\n");
		return true;
	}

	int dodge = g_Girls.GetStat(girl, STAT_AGILITY);  // MYR: Was 0
	int mana = g_Girls.GetStat(girl, STAT_MANA);      // MYR: Like agility, mana is now per battle

	u_int attack = SKILL_COMBAT;	// determined later, defaults to combat
	u_int goon_attack = SKILL_COMBAT;

	if(enemy_gang == 0)
		return true;
	else if(enemy_gang->m_Num == 0)
		return true;

	// first determine what she will fight with
	if(g_Girls.GetSkill(girl, SKILL_COMBAT) > g_Girls.GetSkill(girl, SKILL_MAGIC))
		attack = SKILL_COMBAT;
	else
		attack = SKILL_MAGIC;

	// determine how gang will fight
	if(enemy_gang->m_Skills[SKILL_COMBAT] > enemy_gang->m_Skills[SKILL_MAGIC])
		goon_attack = SKILL_COMBAT;
	else
		goon_attack = SKILL_MAGIC;

	int initial_num = enemy_gang->m_Num;
	
	enemy_gang->m_Combat = true;


	l.ss()	<< gettext("\nGirl vs. Goons: ") << girl->m_Realname << gettext(" fights ") << initial_num << gettext(" opponents!");
	l.ssend();
	l.ss()	<< girl->m_Realname << gettext(": Health ") << girl->health() << gettext(", Dodge ") << g_Girls.GetStat(girl, STAT_AGILITY) 
	<< gettext(", Mana ") << girl->mana();
	l.ssend();

	for(int i=0; i< initial_num; i++)
	{
		l.ss() << gettext("Goon #") << i << gettext(": Health: ") << (int) enemy_gang->m_Stats[STAT_HEALTH] << gettext(" Mana: ")
			<< (int) enemy_gang->m_Stats[STAT_MANA] << gettext(" Dodge: ") << (int) enemy_gang->m_Stats[STAT_AGILITY] 
			<< gettext(" Attack: ") << (int) enemy_gang->m_Skills[goon_attack] << gettext(" Constitution: ") << (int) enemy_gang->m_Stats[STAT_CONSTITUTION];
		l.ssend();

		int gHealth = enemy_gang->m_Stats[STAT_HEALTH];
		int gDodge = enemy_gang->m_Stats[STAT_AGILITY];
		int gMana = enemy_gang->m_Stats[STAT_MANA];

		while(g_Girls.GetStat(girl, STAT_HEALTH) >= 20 && gHealth > 0)
		{
			// Girl attacks
			//l.ss()	<< "\t" << girl->m_Realname << " attacks the goon.";
			//l.ssend();

			if(attack == SKILL_MAGIC) {

				if(mana < 5) {
					attack = SKILL_COMBAT;
					//l.ss() << "\t\t" << girl->m_Realname << " insufficient mana: using combat";
					//l.ssend();
				}
				else {

					mana = mana -5;
					//l.ss() << "\t\t" << girl->m_Realname << " casts a spell; mana now " << mana;
					//l.ssend();
				}
			}
			else {
				//l.ss() << "\t\t" << girl->m_Realname << " using physical attack";
				//l.ssend();
			}

			int girl_attack_chance = g_Girls.GetSkill(girl, attack);

			int die_roll = g_Dice.d100();

			//l.ss() << "\t\t" << " attack chance: " << girl_attack_chance << "\t\t" << " die roll:" << die_roll;
			//l.ssend();

			if(die_roll > girl_attack_chance) {
				//l.ss() << "\t\t\t" << " attack misses";
			}
			else {
				int damage = g_Girls.GetCombatDamage(girl, attack);

				die_roll = g_Dice.d100();

				// Goon attempts Dodge
				//l.ss() << "\t\t" << "Goon tries to dodge: needs " << gDodge << ", gets " << die_roll << ": ";
				//l.ssend();

				// Dodge maxes out at 95%
				if(die_roll <= gDodge && die_roll <= 95) {
					//l.ss() << "\t\t" << "success!";
				}
				else {
					int con_mod = enemy_gang->m_Stats[STAT_CONSTITUTION] / 20;
					damage -= con_mod;
					if (damage <= 0)  // MYR: Minimum 1 damage on hit
					  damage = 1;
                    gHealth -= damage;
					l.ss() << gettext("\t\tGoon takes ") << damage << gettext(". New health value: ") << gHealth;
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
			
			if(die_roll > enemy_gang->m_Skills[goon_attack]) {
				//l.ss() << " attack fails!";
				//l.ssend();
			}
			else {
				//l.ss() << " attack succeeds!";
				//l.ssend();

				// MYR: Goon damage calculation is different from girl's.  Do we care?
				int damage = 5 + enemy_gang->m_Skills[goon_attack] / 10;
				
				if(goon_attack == SKILL_MAGIC)
				{
					if(gMana < 10)
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
				if(die_roll <= dodge && die_roll <= 90) {
					//l.ss() << "\t\t" << "success!";
					//l.ssend();
				}
				else
				{
				  g_Girls.TakeCombatDamage(girl, -damage); // MYR: Note change

				  l.ss() << gettext("\t") << girl->m_Realname << gettext(" takes ") << damage << gettext(". New health value: ") << girl->health();
				  if (girl->has_trait("Incorporeal") || girl->has_trait("Incorporial"))
					  l.ss() << gettext(" (Girl is Incorporeal)");
				  l.ssend();
				}
			}
			
			// update girls dodge ability
			if((dodge - 1) < 0)
				dodge = 0;
			else
				dodge--;

			// update goons dodge ability
			if((gDodge - 1) < 0)
				gDodge = 0;
			else
				gDodge--;
		}  // While loop

		if(g_Girls.GetStat(girl, STAT_HEALTH) <= 20)
		{
			l.ss() << gettext("The gang overwhelmed and defeated ") << girl->m_Realname << gettext(". She lost the battle.") ;
			l.ssend();
			g_Girls.UpdateEnjoyment(girl, ACTION_COMBAT, -5, true);
			return false;
		}
		else
			enemy_gang->m_Num--;  // Gang casualty

		// if the gang has lost half its number there is a chance they will run away
		// This is checked for every member killed over 50%
		if((initial_num/2) > enemy_gang->m_Num)	
		{
			if((1+(g_Dice%100)) <= 50) // MYR: Adjusting this has a big effect
			{
				l.ss() << gettext("The gang ran away after losing too many members. ") << girl->m_Realname << gettext(" WINS!") ;
				l.ssend();
				g_Girls.UpdateEnjoyment(girl, ACTION_COMBAT, +5, true);
				return true;	// the men run away
			}
		}
		// Gang fought to the death
		if(enemy_gang->m_Num == 0)
		{
			l.ss() << gettext("The gang fought to bitter end. They are all dead. ") << girl->m_Realname << gettext(" WINS!") ;
			l.ssend();
			g_Girls.UpdateEnjoyment(girl, ACTION_COMBAT, +5, true);
			return true;
		}
	}

	l.ss()	<< gettext("No more opponents: ") << girl->m_Realname << gettext(" WINS!");
	l.ssend();

	g_Girls.UpdateEnjoyment(girl, ACTION_COMBAT, +5, true);

	return true;
}

int cGangManager::healing_limit()
{
	int limit;
/*
 *	take the number of potions and divide by the
 *	the number of gangs
 */
	limit = m_NumHealingPotions / m_NumGangs;
/*
 *	if that rounds to less than zero, and there are still 
 *	potions available, make sure they get at least one to use
 */
	if(limit == 0 && m_NumHealingPotions) {
		limit = 1;
	}
	return limit;
}

// ----- Mission related

// Missions done here

void cGangManager::UpdateGangs()
{
	cConfig cfg;
	sGang* gang;
	cTariff tariff;
	stringstream ss;

	// update goons combat status
	int cost = 0;
	for(gang = m_GangStart; gang; gang = gang->m_Next) {
		gang->m_Combat = false;
		gang->m_Events.Clear();
/*
 *		sum up the cost of all the goon missions
 */
		cost += tariff.goon_mission_cost(gang->m_MissionID);
	}
	g_Gold.goon_wages(cost);

	// maintain recruitable gangs list, potentially pruning some old ones
	sGang* currentGang = m_HireableGangStart;
	int remove_chance = cfg.gangs.chance_remove_unwanted();
	while(currentGang)
	{
		if(1+g_Dice%100 <= remove_chance)
		{
			cerr << "Culling recruitable gang: " << currentGang->m_Name << endl;
			sGang* temp = currentGang->m_Next;
			RemoveHireableGang(currentGang);
			currentGang = temp;
			continue;
		}
		currentGang = currentGang->m_Next;
	}
	// maybe add some new gangs to the recruitable list
	int add_min = cfg.gangs.add_new_weekly_min();
	int add_max = cfg.gangs.add_new_weekly_max();
	int add_recruits = add_min + g_Dice%(add_max + 1 - add_min);
	for(int i=0; i<add_recruits; i++)
	{
		if(m_NumHireableGangs >= cfg.gangs.max_recruit_list())
			break;
		cerr << "Adding new recruitable gang." << endl;
		AddNewGang(false);
	}

	// now, deal with player controlled gangs on missions
	currentGang = m_GangStart;
	while(currentGang)
	{
		if(currentGang->m_Num <= 0)
		{
			string message = gettext("All of the men in gang ");
			message += currentGang->m_Name;
			message += gettext(" have died.");
			g_MessageQue.AddToQue(message, 1);
			sGang* temp = currentGang->m_Next;
			RemoveGang(currentGang);
			currentGang = temp;
			continue;
		}

		switch(currentGang->m_MissionID) {
		case MISS_GUARDING:	// doesn't need updating
			currentGang->m_Events.AddMessage(gettext("This gang is guarding.") ,IMGTYPE_PROFILE, EVENT_GANG);
			break;
		case MISS_SPYGIRLS:
/*
 *			Doesn't need updating only recalled by player
 */
			currentGang->m_Events.AddMessage(gettext("This gang is spying."), IMGTYPE_PROFILE, EVENT_GANG);
			break;

		case MISS_SABOTAGE:
			sabotage_mission(currentGang);
			if(currentGang->m_Num == 0) {
				sGang* temp = currentGang->m_Next;
				RemoveGang(currentGang);
				currentGang = temp;
				continue;
			}
			break;

		case MISS_CAPTUREGIRL:
			if(recapture_mission(currentGang) == true) {
				currentGang = currentGang->m_Next;
				continue;
			}
			if(currentGang->m_Num == 0) {
				sGang* temp = currentGang->m_Next;
				RemoveGang(currentGang);
				currentGang = temp;
				continue;
			}
			break;

		case MISS_EXTORTION:
			{
				string message = gettext("This gang is capturing territory.");
				currentGang->m_Events.AddMessage(message, IMGTYPE_PROFILE, EVENT_GANG);

				message = gettext("Gang: ");
				message += currentGang->m_Name;
				message += ", ";
				bool ok = false;

				// WD: fix to force limits of -100 to +100
				m_Player->disposition(-1);
				m_Player->customerfear(1);
				m_Player->suspicion(1);

				// Case 1:  Neutral businesses still around
				int numB = m_Rivals->GetNumBusinesses();
				if((m_BusinessesExtort + numB) < TOWN_NUMBUSINESSES)
				{
					ok = true;
					int n = g_Dice%3;

					if((m_BusinessesExtort + numB)+n > TOWN_NUMBUSINESSES)
						n = TOWN_NUMBUSINESSES-(m_BusinessesExtort + numB);

					if(n == 0)
						message += gettext("You don't gain any more neutral territory.");
					else
					{
						message += gettext(" You gain control of ");
						_itoa(n, buffer, 10);
						message += buffer;
						message += gettext(" more neutral territory.");
						m_BusinessesExtort += n;
						g_Gold.extortion(n*20);
						if(g_Brothels.GetObjective())
						{
							if(g_Brothels.GetObjective()->m_Objective == OBJECTIVE_EXTORTXNEWBUSINESS)
								g_Brothels.GetObjective()->m_SoFar += n;
						}
					}
					ok = true;
				}
				else
				{
					// Case 2: Steal bussinesses away from rival if no neutral businesses left
					cRival* rival = m_Rivals->GetRandomRival();
					if(rival)
					{
						if(rival->m_BusinessesExtort > 0)
						{
							if(rival->m_NumGangs > 0)
							{
								sGang* rival_gang = GetTempGang();
								ok = true;
								message += gettext("Your men run into one of your rival's gangs from ");
							    message += rival->m_Name;
								message += gettext(" and a brawl breaks out.\n"); 

								if(GangBrawl(currentGang, rival_gang))
								{
									if(rival_gang->m_Num <= 0)
										rival->m_NumGangs--;

									message += gettext("\nYou defeat the gang and take control of one of their territories.");
									rival->m_BusinessesExtort--;
									m_BusinessesExtort++;
									if(g_Brothels.GetObjective())
									{
										if(g_Brothels.GetObjective()->m_Objective == OBJECTIVE_EXTORTXNEWBUSINESS)
											g_Brothels.GetObjective()->m_SoFar++;
									}
								}
								else
								{
									message += gettext("\nYou fail to take control of any of their territory.");
								}
								delete rival_gang;
								rival_gang = 0;
							}
							else // Rival has no gangs
							{
								ok = true;
								rival->m_BusinessesExtort--;
								m_BusinessesExtort++;
								if(g_Brothels.GetObjective())
								{
									if(g_Brothels.GetObjective()->m_Objective == OBJECTIVE_EXTORTXNEWBUSINESS)
										g_Brothels.GetObjective()->m_SoFar++;
								}
								message += gettext("\nYou gain control of a rival's territory that was left undefended");
							}
						}
					}
				}

				if(currentGang->m_Num <= 0)
				{
					message += gettext("\nThe gang on this mission has been killed.");
					g_MessageQue.AddToQue(message, 2);
					sGang* temp = currentGang->m_Next;
					RemoveGang(currentGang);
					currentGang = temp;
					continue;
				}

				if(ok)
					currentGang->m_Events.AddMessage(message, IMGTYPE_PROFILE, EVENT_GANG);
			}break;

		case MISS_PETYTHEFT:
			{
				string message = gettext("This gang is performing petty theft.");
				currentGang->m_Events.AddMessage(message, IMGTYPE_PROFILE, EVENT_GANG);

				message = gettext("Gang: ");
				message += currentGang->m_Name;
				message += gettext(", ");
				bool lost = false;

				//m_Player->m_Disposition--;
				//m_Player->m_CustomerFear++;
				//m_Player->m_Suspicion++;
				m_Player->disposition(-1);
				m_Player->customerfear(1);
				m_Player->suspicion(1);

				// 30% chance to take losses
				int ev = (g_Dice%100)+1;
				if(ev > 60 && ev < 90)
				{
					// determine losses
					int heal_lim = healing_limit();
					int num = currentGang->m_Num;
					for(int i=0; i<num; i++)
					{
						if(g_Dice%100 > currentGang->combat())
							continue;

						if(heal_lim == 0) {
							currentGang->m_Num--;
							continue;
						}
						heal_lim --;
						m_NumHealingPotions--;
					}
					int number = num-currentGang->m_Num;
					message += gettext(". You lose ");
					if(currentGang->m_Num <= 0)
					{
						message += gettext("all your");
						currentGang->m_Num = 0;
					}
					else
					{
						_itoa(number, buffer, 10);
						message += buffer;
						currentGang->m_Num -= number;
					}
					message += gettext(" men.\n");
				}
			    // 10% chance of running into a rival gang
				else if(ev >= 90)
				{
					cRival* rival = m_Rivals->GetRandomRival();
					if(rival)
					{
						if(rival->m_NumGangs > 0)
						{
							message += gettext("Your men run into a gang from ");
							message += rival->m_Name; 
							message +=  gettext(" and a brawl breaks out.\n"); 
							sGang* rival_gang = GetTempGang();
							if(GangBrawl(currentGang, rival_gang))
								message += gettext("Your men win.");
							else
							{
								message += gettext("Your men lose the fight.");
								if(rival_gang->m_Num <= 0)
									rival->m_NumGangs--;
								lost = true;
							}
							delete rival_gang;
							rival_gang = 0;
						}
					}
				}

				if(currentGang->m_Num <= 0)
				{
					message += gettext("The gang was killed.");
					g_MessageQue.AddToQue(message, 2);
					sGang* temp = currentGang->m_Next;
					RemoveGang(currentGang);
					currentGang = temp;
					continue;
				}
				else if(!lost)
				{
					message += gettext(". Your rob some ");
					int tmp = g_Dice%5;
					long gold = 0;
					if(tmp == 0)
					{
						message += gettext("little old ladies, and get ");
						gold = (g_Dice%130)+1;
					}
					else if(tmp == 1)
					{
						message += gettext("kids, and get ");
						gold = (g_Dice%160)+1;
					}
					else if(tmp == 2)
					{
						message += gettext("noble men and women, and get ");
						gold = (g_Dice%500)+1;
					}
					else if(tmp == 3)
					{
						message += gettext("small stalls, and get ");
						gold = (g_Dice%600)+1;
					}
					else if(tmp == 4)
					{
						message += gettext("traders, and get ");
						gold = (g_Dice%800)+1;
					}
					_ltoa(gold, buffer, 10);
					message += buffer;
					message += gettext(" gold from them.");
					g_Gold.petty_theft(gold);
					currentGang->m_Events.AddMessage(message, IMGTYPE_PROFILE, EVENT_GANG);

					if(g_Brothels.GetObjective())
					{
						if(g_Brothels.GetObjective()->m_Objective == OBJECTIVE_STEALXAMOUNTOFGOLD)
							g_Brothels.GetObjective()->m_SoFar += gold;
					}
				}
			}
			break;

		case MISS_GRANDTHEFT:
			{
				string message = gettext("This gang is performing a grand theft.");
				currentGang->m_Events.AddMessage(message, IMGTYPE_PROFILE, EVENT_GANG);

				message = gettext("Gang: ");
				message += currentGang->m_Name;
				message += ", ";
				bool lost = false;

				//m_Player->m_Disposition--;
				//m_Player->m_CustomerFear++;
				//m_Player->m_Suspicion++;
				m_Player->disposition(-1);
				m_Player->customerfear(1);
				m_Player->suspicion(1);

				int ev = (g_Dice%100)+1;
				if(ev > 40 && ev < 90)
				{
					// determine losses
					int heal_lim = healing_limit();
					int num = currentGang->m_Num;
					for(int i=0; i<num; i++)
					{
						if(g_Dice%100 > currentGang->combat())
							continue;
						if(heal_lim == 0) {
							currentGang->m_Num--;
							continue;
						}
						heal_lim --;
						m_NumHealingPotions--;
					}
					int number = num-currentGang->m_Num;
					message += gettext(". You lose ");
					if(currentGang->m_Num <= 0)
					{
						message += gettext("all your");
						currentGang->m_Num = 0;
					}
					else
					{
						_itoa(number, buffer, 10);
						message += buffer;
						currentGang->m_Num -= number;
					}
					message += " men.\n";
				}
				else if(ev >= 90)	// ran into a rivals goons
				{
					cRival* rival = m_Rivals->GetRandomRival();
					if(rival)
					{
						if(rival->m_NumGangs > 0)
						{
							message += gettext("Your men run into a gang from "); 
							message +=  rival->m_Name;
							message += gettext(" and a brawl breaks out.\n"); 
							sGang* rival_gang = GetTempGang();
							if(GangBrawl(currentGang, rival_gang))
								message += gettext("Your men win.");
							else
							{
								message += gettext("Your men lose the fight.");
								if(rival_gang->m_Num <= 0)
									rival->m_NumGangs--;
								lost = true;
							}
							delete rival_gang;
							rival_gang = 0;
						}
					}
				}

				if(currentGang->m_Num <= 0)
				{
					message += gettext("\nAll your men on this mission have died.");
					g_MessageQue.AddToQue(message, 2);
					sGang* temp = currentGang->m_Next;
					RemoveGang(currentGang);
					currentGang = temp;
					continue;
				}
				else if(!lost)
				{
					message += gettext(". Your men rob ");
					int tmp = g_Dice%5;
					long gold = 0;
					if(tmp == 0)
					{
						message += gettext("small shop, and get ");
						gold = (g_Dice%400)+1;
					}
					else if(tmp == 1)
					{
						message += gettext("a smithy, and get ");
						gold = (g_Dice%600)+1;
					}
					else if(tmp == 2)
					{
						message += gettext("a jeweler, and get ");
						gold = (g_Dice%800)+1;
					}
					else if(tmp == 3)
					{
						message += gettext("a trade caravan, and get ");
						gold = (g_Dice%1000)+1;
					}
					else if(tmp == 4)
					{
						message += gettext("a bank, and get ");
						gold = (g_Dice%2000)+1;
					}
					_ltoa(gold, buffer, 10);
					message += buffer;
					message += gettext(" gold.");
					g_Gold.grand_theft(gold);
					currentGang->m_Events.AddMessage(message, IMGTYPE_PROFILE, EVENT_GANG);

					if(g_Brothels.GetObjective())
					{
						if(g_Brothels.GetObjective()->m_Objective == OBJECTIVE_STEALXAMOUNTOFGOLD)
							g_Brothels.GetObjective()->m_SoFar += gold;
					}
				}
			}break;

		case MISS_KIDNAPP:
			{
				string message = gettext("This gang is kidnapping girls.");
				currentGang->m_Events.AddMessage(message, IMGTYPE_PROFILE, EVENT_GANG);

				message = gettext("Gang: ");
				message += currentGang->m_Name;
				message += ", ";

				if((g_Dice%101) < 35)
				{
					sGirl* girl = g_Girls.GetRandomGirl();
					if(girl)
					{
						int v[2] = {-1,-1};
						if(girl->m_Triggers.CheckForScript(TRIGGER_KIDNAPPED, true, v))
						{
							currentGang = currentGang->m_Next;
							continue;
						}

						/* MYR: For some reason I can't figure out, a number of girl's house percentages
						        are at zero or set to zero when they are sent to the dungeon. I'm not sure
								how to fix it, so I'm explicitly setting the percentage to 60 here */
						girl->m_Stats[STAT_HOUSE] = 60;

						message += gettext("Your men find a girl, "); 
						message += girl->m_Name;
						message +=  gettext("  and ");
						if(g_Dice%100 <= currentGang->m_Stats[STAT_CHARISMA])	// convince her
						{
							message += gettext("convince her that she should work for you.");
							m_Dungeon->AddGirl(girl, DUNGEON_GIRLKIDNAPPED);
						}
						else if(m_NumNets > 0 || m_KeepNetsStocked)	// capture using net
						{
							girl->m_Stats[STAT_OBEDIENCE] = 0;
							message += gettext("She struggles against the net your men use, but it is pointless.  She is in your dungeon now.");
							m_Dungeon->AddGirl(girl, DUNGEON_GIRLKIDNAPPED);
							if(m_NumNets > 0)
								m_NumNets--;
							else
								g_Gold.consumable_cost(5);
						}
						else if(g_Brothels.FightsBack(girl))	// kidnap her
						{
							message += gettext("attempt to kidnap her.");
							if(!GangCombat(girl, currentGang))
							{
								girl->m_Stats[STAT_OBEDIENCE] = 0;
								message += gettext("\nShe fights back but your men succeed in kidnapping her.");
								m_Dungeon->AddGirl(girl, DUNGEON_GIRLKIDNAPPED);
							}
							else
							{
								if(currentGang->m_Num <= 0)
								{
									message += gettext("\nThe girl fights back and kills your men before escaping into the crowd.");
									g_MessageQue.AddToQue(message, 2);
									sGang* temp = currentGang->m_Next;
									RemoveGang(currentGang);
									currentGang = temp;
									continue;
								}
								else
									message += gettext("\nThe girl fights back and defeats your men before escaping into the crowd.");
							}
						}
						else
						{
							message += gettext("kidnap her successfully without a fuss.  She is in your dungeon now.");
							m_Dungeon->AddGirl(girl, DUNGEON_GIRLKIDNAPPED);
						}
						BoostGangSkill(&currentGang->m_Stats[STAT_CHARISMA], 3);
						currentGang->m_Events.AddMessage(message, IMGTYPE_PROFILE, EVENT_GANG);
					}
					else
					{
						message += gettext("failed to find any girls to kidnap.");
						currentGang->m_Events.AddMessage(message,IMGTYPE_PROFILE, EVENT_GANG);
					}
				}
				else
				{
					message += gettext("failed to find any girls to kidnap.");
					currentGang->m_Events.AddMessage(message, IMGTYPE_PROFILE, EVENT_GANG);
				}
			}break;

		case MISS_TRAINING:
			{
				string message = gettext("This gang is training.");
				currentGang->m_Events.AddMessage(message, IMGTYPE_PROFILE, EVENT_GANG);

				message = gettext("Gang: ");
				message += currentGang->m_Name;
				message += gettext(", ");
				message += gettext(" Your men spend the week training and improving their skills.");

#if 1			// New gang training code
				int old_combat = currentGang->m_Skills[SKILL_COMBAT];
				int old_magic = currentGang->m_Skills[SKILL_MAGIC];
				int old_intel = currentGang->m_Stats[STAT_INTELLIGENCE];
				int old_agil = currentGang->m_Stats[STAT_AGILITY];
				int old_const = currentGang->m_Stats[STAT_CONSTITUTION];
				int old_char = currentGang->m_Stats[STAT_CHARISMA];

				vector<int*> possible_skills;
				possible_skills.push_back(&currentGang->m_Skills[SKILL_COMBAT]);
				possible_skills.push_back(&currentGang->m_Skills[SKILL_MAGIC]);
				possible_skills.push_back(&currentGang->m_Stats[STAT_INTELLIGENCE]);
				possible_skills.push_back(&currentGang->m_Stats[STAT_AGILITY]);
				possible_skills.push_back(&currentGang->m_Stats[STAT_CONSTITUTION]);
				possible_skills.push_back(&currentGang->m_Stats[STAT_CHARISMA]);

				int count = (g_Dice%3)+2;  // get 2-4 potential skill/stats to boost
				for(int i=0; i<count; i++)
				{
					int boost_count = (g_Dice%3)+1;  // boost each 1-3 times
					BoostGangRandomSkill(&possible_skills, 1, boost_count);
				}
				possible_skills.clear();

				stringstream ss;
				ss.str("");
				if(currentGang->m_Skills[SKILL_COMBAT] > old_combat)
					ss << gettext("\n+") << (currentGang->m_Skills[SKILL_COMBAT] - old_combat) << gettext(" Combat");
				if(currentGang->m_Skills[SKILL_MAGIC] > old_magic)
					ss << gettext("\n+") << (currentGang->m_Skills[SKILL_MAGIC] - old_magic) << gettext(" Magic");
				if(currentGang->m_Stats[STAT_INTELLIGENCE] > old_intel)
					ss << gettext("\n+") << (currentGang->m_Stats[STAT_INTELLIGENCE] - old_intel) << gettext(" Intelligence");
				if(currentGang->m_Stats[STAT_AGILITY] > old_agil)
					ss << gettext("\n+") << (currentGang->m_Stats[STAT_AGILITY] - old_agil) << gettext(" Agility");
				if(currentGang->m_Stats[STAT_CONSTITUTION] > old_const)
					ss << gettext("\n+") << (currentGang->m_Stats[STAT_CONSTITUTION] - old_const) << gettext(" Toughness");
				if(currentGang->m_Stats[STAT_CHARISMA] > old_char)
					ss << gettext("\n+") << (currentGang->m_Stats[STAT_CHARISMA] - old_char) << gettext(" Charisma");

				message += ss.str();

#else			// Original Code
				for(u_int i=0; i<NUM_SKILLS; i++)
				{
					currentGang->m_Skills[i] += (g_Dice%2)+2;
					if(currentGang->m_Skills[i] > 100)
						currentGang->m_Skills[i] = 100;
				}

				for(int i=0; i<NUM_STATS; i++)
				{
					currentGang->m_Stats[i] += (g_Dice%2)+2;
					if(currentGang->m_Stats[i] > 100)
						currentGang->m_Stats[i] = 100;
				}
#endif

				currentGang->m_Events.AddMessage(message, IMGTYPE_PROFILE, EVENT_GANG);
				currentGang->m_Combat = false;
			}break;

		case MISS_RECRUIT:
			{
				string message = gettext("This gang is recruiting.");
				currentGang->m_Events.AddMessage(message, IMGTYPE_PROFILE, EVENT_GANG);

				if(currentGang->m_Num<15)
				{
					currentGang->m_Num+=3; // MYR
					if(currentGang->m_Num > 15)
						currentGang->m_Num = 15;
				}
				currentGang->m_Combat = false;
			}break;

		case MISS_CATACOMBS:
			{
				string message = gettext("This gang is exploring the catacombs.");
				currentGang->m_Events.AddMessage(message, IMGTYPE_PROFILE, EVENT_GANG);

				message = gettext("Gang: ");
				message += currentGang->m_Name;
				message += gettext(", Your men venture into the catacombs to explore. And ");

				// determine losses
				int heal_lim = healing_limit();
				int num = currentGang->m_Num;
				for(int i=0; i<num; i++)
				{
					int D100Roll;    
					D100Roll = g_Dice.d100();
					if (D100Roll <= currentGang->combat()) {
					//if(g_Dice.percent(40)) {
						continue;
					}
					if(heal_lim == 0) {
						currentGang->m_Num--;
						continue;
					}
					heal_lim --;
					m_NumHealingPotions--;
				}
				_itoa(currentGang->m_Num, buffer, 10);
				message += buffer;
				message += gettext(" return.");
				if(num > currentGang->m_Num)
					currentGang->m_Combat = true;

				if(currentGang->m_Num > 0)
				{
					// determine loot
					message += gettext(" They return carrying ");
					long gold = (g_Dice%700)+300;
					_ltoa(gold, buffer, 10);
					g_Gold.catacomb_loot(gold);
					message += buffer;
					message += gettext(" gold");

					while((g_Dice%100)+1 < 60)
					{
						bool quit = false;
						bool add = false;
						//mod
						//hopefully fix a crash
						sInventoryItem* temp;
						do
						{
						temp = g_InvManager.GetRandomItem();
						}while(!temp);
						//end mod
						while(temp->m_Rarity < sInventoryItem::Shop25)
							temp = g_InvManager.GetRandomItem();
						switch(temp->m_Rarity)
						{
						case sInventoryItem::Shop25:
							add = true;
							break;
						case sInventoryItem::Shop05:
							if((g_Dice%100)+1 <= 15)  // 15% or 5%?
								add = true;
							break;
						case sInventoryItem::Catacomb15:
							if((g_Dice%100)+1 <= 15)
								add = true;
							break;
						case sInventoryItem::Catacomb05:
							if((g_Dice%100)+1 <= 5)
								add = true;
							break;
						case sInventoryItem::Catacomb01:
							if((g_Dice%100)+1 <= 1)
								add = true;
							break;
/*
 *						adding these cases to shut the compiler up
 */
						case sInventoryItem::Common:
						case sInventoryItem::Shop50:
							g_LogFile.ss()
								<< "Warning: unexpected rarity "
								<< temp->m_Rarity
								<< " in cGangManager::UpdateGangs"
							;
							g_LogFile.ssend();
							break;
/*
 *						I don't know if these are supposed to not happen
 *						(therefor reducing the chances of a catacomb item)
 *						or if they aren't accounted for by the while loop above
 *
 *						assuming the latter for now. silently ignore
 */
						case sInventoryItem::ScriptOnly:
						case sInventoryItem::ScriptOrReward:
							break;
						default:
							g_LogFile.ss()
								<< "Warning: cGangManager::UpdateGangs: "
								<< "unexpected enum in switch"
							;
							g_LogFile.ssend();
							break;
						}

						if(add)
						{
							message += ", ";
							int curI = g_Brothels.HasItem(temp->m_Name, -1);
							bool loop = true;
							while(loop)
							{
								if(curI != -1)
								{
									if(g_Brothels.m_NumItem[curI] >= 999)
										curI = g_Brothels.HasItem(temp->m_Name, curI+1);
									else
										loop = false;
								}
								else
									loop = false;
							}

							if(g_Brothels.m_NumInventory < MAXNUM_INVENTORY || curI != -1)
							{
								if(curI != -1)
								{
									message += gettext("a ");
									message += temp->m_Name;
									g_Brothels.m_NumItem[curI]++;
								}
								else
								{
									for(int j=0; j<MAXNUM_INVENTORY; j++)
									{
										if(g_Brothels.m_Inventory[j] == 0)
										{
											message += gettext("a ");
											message += temp->m_Name;
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
								message += gettext(" Your inventory is full\n");
							}
						}

						if(quit)
							break;
					}
					message += gettext(".");

					// determine if get a catacomb girl (is "monster" if trait not human)
					if((g_Dice%100)+1 < 40)
					{
						sGirl* ugirl = 0;
						bool unique = false;
						if((g_Dice%100)+1 < 50)	// chance of getting unique girl
							unique = true;

						if(unique)
						{
							ugirl = g_Girls.GetRandomGirl(false, true);
							if(ugirl == 0)
								unique = false;
						}

						if(g_Brothels.GetObjective())
						{
							if(g_Brothels.GetObjective()->m_Objective == OBJECTIVE_CAPTUREXCATACOMBGIRLS)
								g_Brothels.GetObjective()->m_SoFar++;
						}

						if(unique)
						{
							message += gettext(" Your men also captured a girl named ");
							message += ugirl->m_Realname;
							ugirl->m_States &= ~(1<<STATUS_CATACOMBS);
							m_Dungeon->AddGirl(ugirl, DUNGEON_GIRLCAPTURED);
						}
						else
						{
							ugirl = g_Girls.CreateRandomGirl(0, false, false, true, true);
							if(ugirl != 0)  // make sure a girl was returned
							{
								message += gettext(" Your men also captured a girl.");
								m_Dungeon->AddGirl(ugirl, DUNGEON_GIRLCAPTURED);
							}
						}
					}
				}
				else
				{
					message += gettext(" The men were wiped out.");
					g_MessageQue.AddToQue(message, 0);
					sGang* temp = currentGang->m_Next;
					RemoveGang(currentGang);
					currentGang = temp;
					continue;
				}

				currentGang->m_Events.AddMessage(message, IMGTYPE_PROFILE, EVENT_GANG);
			}break;
		}

		if(currentGang->m_Combat == false)
		{
			if(currentGang->m_Num < 15)
				currentGang->m_Num++;
		}

		if(currentGang->m_Num <= 5 && currentGang->m_MissionID != MISS_RECRUIT)
		{
			string message = gettext("Gang: ");
			message += currentGang->m_Name;
			message += gettext(", were also put on recruit mission due to low numbers");
			currentGang->m_Events.AddMessage(message, IMGTYPE_PROFILE, EVENT_WARNING);
			currentGang->m_AutoRecruit = true;
			currentGang->m_LastMissID = currentGang->m_MissionID;
			currentGang->m_MissionID = MISS_RECRUIT;
		}
		else if(currentGang->m_MissionID == MISS_RECRUIT && currentGang->m_Num == 15)
		{
			if(currentGang->m_AutoRecruit)
			{
				string message = gettext("Gang: ");
				message += currentGang->m_Name;
				message += gettext(", were placed back on their previous mission now that their numbers are back to normal.");
				currentGang->m_Events.AddMessage(message, IMGTYPE_PROFILE, EVENT_WARNING);
				currentGang->m_MissionID = currentGang->m_LastMissID;
				currentGang->m_AutoRecruit = false;
			}
			else
			{
				string message = gettext("Gang: ");
				message += currentGang->m_Name;
				message += gettext(", were placed on guard duty from recruitment as their numbers are full.");
				currentGang->m_Events.AddMessage(message, IMGTYPE_PROFILE, EVENT_WARNING);
				currentGang->m_MissionID = MISS_GUARDING;
			}
		}

		currentGang = currentGang->m_Next;
	}

	// Update the rivals
	m_Rivals->Update(m_BusinessesExtort);

/*
 *	restock on healing potions
 */

	g_LogFile.ss()
		<< "Time to restock heal potions\n"
		<< "Flag = " << bool(m_KeepHealStocked !=0) << "\n"
		<< "Target = " << int(m_KeepHealStocked) << "\n"
		<< "Current = " << m_KeepHealStocked;
	g_LogFile.ssend();

 	if(m_KeepHealStocked && int(m_KeepHealStocked) > m_NumHealingPotions) {
		int diff = m_KeepHealStocked - m_NumHealingPotions;
		m_NumHealingPotions = m_KeepHealStocked;
		g_Gold.consumable_cost(
			tariff.healing_price(diff)
		);
	}
}

void cGangManager::SendGang(int gangID, int missID)
{
	// first find the gang
	sGang* currentGang = m_GangStart;
	int count = 0;
	while(currentGang)
	{
		if(count == gangID)
			break;
		count++;
		currentGang = currentGang->m_Next;
	}

	if(currentGang)
		currentGang->m_MissionID = missID;
}

sGang* cGangManager::GetGangOnMission(u_int missID)
{
	// first find the gang with some men
	sGang* currentGang = m_GangStart;
	int count = 0;
	while(currentGang)
	{
		if(currentGang->m_MissionID == missID && currentGang->m_Num > 0)
			break;
		count++;
		currentGang = currentGang->m_Next;
	}
	return currentGang;
}

/*
 * Get a vector with all the gangs doing MISS_FOO
 */
vector<sGang*> cGangManager::gangs_on_mission(u_int mission_id)
{
	vector<sGang*> v;
/*
 *	loop through the gangs
 */
	for(sGang *gang = m_GangStart; gang; gang = gang->m_Next) {
/*
 *		if they're not spying, disregard them
 */
		if(gang->m_MissionID != mission_id) {
			continue;
		}
		v.push_back(gang);
	}
	return v;
}

/*
 * two objectives here:
 *
 * A: multiple squads spying on the girls improves
 *    the chance of catching thieves
 * B: The intelligence of the girl and the goons
 *    affects the result
 */
int cGangManager::chance_to_catch(sGirl* girl)
{
	int pc = 0;
	sGang* gang = m_GangStart;
/*
 *	get a vector containing all the spying gangs
 */
	vector<sGang*> gvec = gangs_on_mission(MISS_SPYGIRLS);
/*
 *	bit of debug chatter
 */
	g_LogFile.ss()
		<< "cGangManager::chance_to_catch: "
		<< gvec.size()
		<< " gangs spying"
	;
	g_LogFile.ssend();
/*
 *	whizz down the vector adding probability as we go
 */
	for(u_int i = 0; i < gvec.size(); i++) {
		gang = gvec[i];
/*
 *		now then: the basic chance is 5 * number of goons
 *		but I want to modify that for the intelligence
 *		of the girl, and that of the squad
 */
 		float mod = float(100 + gang->intelligence()) ;
		mod -= girl->intelligence();
		mod /= 100.0;
/*
 *		that should give us a multiplier that can 
 *		at one extreme, double the chances of the sqaud
 *		catching her, and at the other, reduce it to zero
 */
 		pc += int(5 * gang->m_Num * mod);
	}
	if(pc > 100) pc = 100;
	BoostGangSkill(&gang->m_Stats[STAT_INTELLIGENCE], 1);
	return pc;
}

void cGangManager::sabotage_mission(sGang* gang)
{
	stringstream ss;
	sGang* rival_gang = 0;
	gang->m_Events.AddMessage(gettext("This gang is attacking rivals."), IMGTYPE_PROFILE, EVENT_GANG);

	ss << gettext("Gang: ") << gang->m_Name << ", ";
/*
 *	See if they can find any enemy assets to attack
 *
 *	I'd like to add a little more intelligence to this.
 *	Modifiers based on gang intelligence, for instance
 *	Allow a "scout" activity for gangs that improves the
 *	chances of a raid. That sort of thing.
 */
	if(g_Dice.percent(30)) {
		gang->m_Events.AddMessage(gettext("Failed to attack any enemy assets."), IMGTYPE_PROFILE, EVENT_GANG);
		return;
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
	cRival* rival = m_Rivals->GetRandomRival();
	if(!rival) {
		gang->m_Events.AddMessage(gettext("Scouted the city in vain, seeking would-be challengers to your dominance."), IMGTYPE_PROFILE, EVENT_GANG);
		return;
	}

	if(rival->m_NumGangs > 0)
	{

/*
 *		I'd like to see a bit more intelligence here, too.
 *		bigger rivals should have tougher gangs, and better armed,
 *		as well.
 */

		rival_gang = GetTempGang();
		
		ss << gettext("Your men run into a gang from ") << rival->m_Name << gettext(" and a brawl breaks out.\n"); 

		if(GangBrawl(gang, rival_gang) == false) {
			if(gang->m_Num > 0) {
				delete rival_gang;
				rival_gang = 0;
				ss << gettext("Your men lost. The ") << gang->m_Num << gettext(" survivor(s) fight their way back to friendly territory.");
				gang->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_DANGER);
				return;
			}

			ss << gettext("Your gang fails to report back. Later you learn that they were wiped out to the last man.");
			g_MessageQue.AddToQue(ss.str(), 0);
			delete rival_gang;
			rival_gang = 0;
			return;
		} // if GangBrawl = false
		else // GangBrawl = true
		ss << gettext("Your men win.") << endl;
/*
 *	clean up the rival gang
 */
		if(rival_gang->m_Num <= 0)
		{
			rival->m_NumGangs--;
			ss << gettext("The enemy gang is destroyed. ");
			if (rival->m_NumGangs == 0)
				ss << rival->m_Name << gettext(" has no more gangs left!\n");
			else if (rival->m_BusinessesExtort <= 3)
				ss << rival->m_Name << gettext(" has a few gangs left.\n");
			else
				ss << rival->m_Name << gettext(" has a lot of gangs left.\n");
		}
		delete rival_gang;
		rival_gang = 0;
	}
	else // rival->m_NumGangs = 0
		ss << gettext("\nYour men encounter no resistance when you go after ") << rival->m_Name << ".";

/*
 *	if we had an objective to attack a rival
 *	we just achieved it
 */
	sObjective *obj = g_Brothels.GetObjective();
	if(obj && obj->m_Objective == OBJECTIVE_LAUNCHSUCCESSFULATTACK)
		g_Brothels.PassObjective();

/*
 *	If the rival has some businesses under his control
 *	he's going to lose some of them
 */
	if(rival->m_BusinessesExtort > 0)
	{
		// mod: brighter goons do better damage
		// they need 100% to be better than before however
		int spread = gang->intelligence() / 4;
/*
 *		get the number of businesses lost
 */
		int num = 1 + g_Dice.random(spread);
		if (rival->m_BusinessesExtort < num)  // Can't destroy more businesses than they have
			num = rival->m_BusinessesExtort;
		rival->m_BusinessesExtort -= num;
		//if(rival->m_BusinessesExtort < 0) {
		//	rival->m_BusinessesExtort = 0;
		//}

		ss << "\nYour men destroy " << num << gettext(" of their businesses. ");
		if (rival->m_BusinessesExtort == 0)
			ss << rival->m_Name << gettext(" have no more businesses left!\n");
		else if (rival->m_BusinessesExtort <= 10)
			ss << rival->m_Name << gettext(" have a few businesses left.\n");
		else
			ss << rival->m_Name << gettext(" have a lot of businesses left.\n");
	}
	else  // No businesses
		ss << rival->m_Name << gettext(" have no businesses to attack.\n");

	if(rival->m_Gold > 0)
	{
		// mod: brighter goons are better thieves
		// they need 100% to be better than before however
		int factor = 2 + gang->intelligence() / 4;

		long gold = g_Dice.random(factor * 400)+44;
		if (gold > rival->m_Gold)
			gold = rival->m_Gold;
			rival->m_Gold -= gold;

		ss << gettext("\nYour men steal ") << gold << gettext(" gold from them. ");
		if (rival->m_Gold == 0)
			ss << gettext("Mu-hahahaha!  They're penniless now!\n");
		else if (rival->m_Gold <= 20000)
			ss << rival->m_Name << gettext(" is looking pretty poor.\n");
		else
			ss << gettext("It looks like ") << rival->m_Name << gettext(" still has a lot of gold.\n");
		g_Gold.plunder(gold);
	}
	else // No gold
	  ss << gettext("The losers have no gold to take.\n");

	if(rival->m_NumBrothels > 0 && ((g_Dice%100) < (gang->intelligence() / 5)))
	{
		rival->m_NumBrothels--;
		ss << gettext("\nYour men burn down one of ") << rival->m_Name << gettext("'s brothels. ");
	}

	if (rival->m_NumBrothels == 0)
		ss << rival->m_Name << gettext(" has no brothels left.\n");
	else if (rival->m_NumBrothels <= 3)
		ss << rival->m_Name << gettext(" is in control of very few brothels.\n");
	else
		ss << rival->m_Name << gettext(" has many brothels left.\n");

	BoostGangSkill(&gang->m_Stats[STAT_INTELLIGENCE], 1);
	gang->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_GANG);

	// See if the rival is eliminated:  If 2 or more of {gold, gangs, businesses, brothels}
	// are zero or less, the rival is eliminated
	int VictoryPoints = 0;
	if (rival->m_Gold <= 0)
		VictoryPoints++;
	if (rival->m_NumGangs <= 0)
		VictoryPoints++;
	if (rival->m_BusinessesExtort <= 0)
		VictoryPoints++;
	if (rival->m_NumBrothels <= 0)
		VictoryPoints++;

	if (VictoryPoints >= 2)
	{
	    stringstream ssVic;
		ssVic << gettext("You have dealt ") << rival->m_Name << gettext(" a fatal blow.  Their criminal organization crumbles to nothing before you.");
		m_Rivals->RemoveRival(rival);
		gang->m_Events.AddMessage(ssVic.str(), IMGTYPE_PROFILE, EVENT_WARNING);
	}

}

bool cGangManager::recapture_mission(sGang* gang)
{
	// check if any girls are run away
	stringstream ss;

	gang->m_Events.AddMessage(gettext("This gang is looking for escaped girls."), IMGTYPE_PROFILE, EVENT_GANG);


	// if no runnaway then the gang continues on as normal
	sGirl* runnaway = g_Brothels.GetFirstRunaway();
	if(runnaway == 0)
		return true;

	ss << gettext("Gang: ") << gang->m_Name << ": ";
/*
 *	if yes then combat the girl and attempt capture then
 *	return on success or keep hunting her
 *
 *	if have a net then just capture her easy
 *
 *	I'd do a deferred restock here, but since i've never actually
 *	run out of nets, it's probably ok to do it on the fly
 */
	if(m_NumNets > 0 || m_KeepNetsStocked)	// Case #1: Use a net
	{
		ss << gettext("Your goons find and net ") << runnaway->m_Realname << gettext(". She struggles but can't escape, and has been returned to your dungeon.");
		runnaway->m_RunAway = 0;
		g_Brothels.RemoveGirlFromRunaways(runnaway);
		m_Dungeon->AddGirl(runnaway, DUNGEON_GIRLRUNAWAY);
		gang->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_GANG);
		if(m_NumNets)
			m_NumNets--;
		else
			g_Gold.consumable_cost(5);
		return true;
	}

	if(!g_Brothels.FightsBack(runnaway))  // Case #2: She doesn't fight back
	{
		ss << gettext("Your goons find ") << runnaway->m_Realname << gettext(". She comes quietly and doesn't put up a fight.")
		;
		runnaway->m_RunAway = 0;
		g_Brothels.RemoveGirlFromRunaways(runnaway);
		m_Dungeon->AddGirl(runnaway, DUNGEON_GIRLRUNAWAY);
		gang->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_GANG);
		return true;
	}

	// Case #3: She fights back
	ss << gettext("Your goons find ")  << runnaway->m_Realname << gettext(" who fights back fiercely. ");
	int num = (int)gang->m_Num;
	// 3A: Gang wins
	if(!GangCombat(runnaway, gang))
	{
		gang->m_Num -= gang->m_Num-num;  // Is this a bug???
		ss << gettext("She is subdued and taken to your dungeon.");
		runnaway->m_RunAway = 0;
		g_Brothels.RemoveGirlFromRunaways(runnaway);
		m_Dungeon->AddGirl(runnaway, DUNGEON_GIRLRUNAWAY);
		gang->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_GANG);
		return true;
	}
/*
 *	if we get here, the gang lost 
 *
 *	Do we want this always to be fatal for the gang?
 *	I'm assuming that was a bug
 */
	// 3B: Escapee wins
	//gang->m_Num = 0;	
	if(gang->m_Num > 0) {
		ss << gettext("She thrashes your gang before disappearing again.");
		gang->m_Events.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_GANG);
		return true;
	}
	ss << gettext("She destroys your gang before disappearing again.");
	g_MessageQue.AddToQue(ss.str(), 0);
	return false;
}





