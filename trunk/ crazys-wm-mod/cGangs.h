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

#ifndef __CGANGS_H
#define __CGANGS_H

#include "cGirls.h"
#include "cRival.h"
#include "cGold.h"
#include "cBrothel.h"
#include <vector>

extern CLog g_LogFile;
extern cBrothelManager g_Brothels;

// A gang of street toughs
typedef struct sGang
{
	int m_Num;	// number in the gang
	unsigned char m_Skills[NUM_SKILLS];	// skills of the gang
	unsigned char m_Stats[NUM_STATS];	// stats of the gang
	u_int m_MissionID;	// the type of mission currently performing
	int m_LastMissID;	// the last mission if auto changed to recruit mission
	bool m_AutoRecruit;	// true if auto recruiting
	string m_Name;
	bool m_Combat;	// is true when gang has seen combat in the last week
	cEvents m_Events;

	sGang* m_Next;
	sGang* m_Prev;

	TiXmlElement* SaveGangXML(TiXmlElement* pRoot);
	bool LoadGangXML(TiXmlHandle hGang);

	int magic()		{ return m_Skills[SKILL_MAGIC]; }
	int combat()		{ return m_Skills[SKILL_COMBAT]; }
	int intelligence()	{ return m_Stats[STAT_INTELLIGENCE]; }

	sGang()
	{
		m_Name = "Unnamed";
		m_Num = 0;
		m_Prev = m_Next = 0;
		m_MissionID = MISS_GUARDING;
		m_Combat = false;
		m_LastMissID = -1;
		m_AutoRecruit = false;
	}

	~sGang()
	{
		if(m_Next)
			delete m_Next;
		m_Prev = m_Next = 0;
	}
}sGang;

/*
 * manages all the player gangs
 */
class cGangManager
{
public:
	cGangManager();

	~cGangManager() {Free();}

	void Free();

	void AddNewGang(bool boosted = false);	// Adds a new randomly generated gang to the recruitable list
	void HireGang(int gangID);	// hired a recruitable gang, so add it to your gangs
	void FireGang(int gangID);	// fired a gang, so send it back to recruitables (or just delete if full up)
	void AddHireableGang(sGang* gang);
	void AddGang(sGang* gang);
	void RemoveHireableGang(int gangID);	// removed a recruitable gang from the list
	void RemoveHireableGang(sGang* gang);
	void RemoveGang(int gangID);	// removed a controlled gang completely from service
	void RemoveGang(sGang* gang);
	void SendGang(int gangID, int missID);	// sends a gang on a mission
	sGang* GetGang(int gangID);	// gets a gang
	sGang* GetHireableGang(int gangID);	// gets a recruitable gang
	sGang* GetGangOnMission(u_int missID);	// gets a gang on the current mission
	void UpdateGangs();
	int GetNumGangs();
	int GetNumHireableGangs();
	bool GangCombat(sGirl* girl, sGang* gang);	// returns true if the girl wins
	bool GangBrawl(sGang* gang1, sGang* gang2);	// returns true if gang1 wins
	sGang* GetTempGang();	// creates a new gang
	void BoostGangSkill(unsigned char* affect_skill, int count = 1);  // increases a specific skill/stat the specified number of times
	void BoostGangRandomSkill(vector<unsigned char*>* possible_skills, int count = 1, int boost_count = 1);  // chooses from the passed skills/stats and raises one or more of them
	void BoostGangCombatSkills(sGang* gang, int count = 1);  // simple function to increase a gang's combat skills a bit

	sGang* GetTempWeakGang();

	TiXmlElement* SaveGangsXML(TiXmlElement* pRoot);
	bool LoadGangsXML(TiXmlHandle hGangManager);
	void LoadGangsLegacy(ifstream& ifs);

	int GetNumBusinessExtorted() {return m_BusinessesExtort;}

	int* GetWeaponLevel() {return &m_SwordLevel;}
	int* GetNets() {return &m_NumNets;}
	int GetNetRestock() {return m_KeepNetsStocked;}
	void KeepNetStocked(int stocked){m_KeepNetsStocked = stocked;}
	int* GetHealingPotions() {return &m_NumHealingPotions;}
	void KeepHealStocked(int stocked){m_KeepHealStocked = stocked;}
	int GetHealingRestock() {return m_KeepHealStocked;}

	void sabotage_mission(sGang* gang);
	bool recapture_mission(sGang* gang);
	int chance_to_catch(sGirl* girl);
	int healing_limit();

	// Used by the new brothel security code
	sGang*	random_gang(vector<sGang*>& v);
	bool GirlVsEnemyGang(sGirl* girl, sGang* enemy_gang);

	vector<sGang*> gangs_on_mission(u_int mission_id);

private:
	int m_BusinessesExtort;	// number of businesses under your control

	int m_NumGangNames;
	unsigned char m_NumGangs;
	sGang* m_GangStart;	// the start and end of the list of gangs under the players employment
	sGang* m_GangEnd;
	unsigned char m_NumHireableGangs;
	sGang* m_HireableGangStart;	// the start and end of the list of gangs which are available for hire
	sGang* m_HireableGangEnd;

	// gang armory
	// mod - changing the keep stocked flag to an int
	// so we can record the level at which to maintain
	// the stock - then we can restock at turn end
	// to prevent squads becoming immortal by
	// burning money
	int m_KeepHealStocked;
	int m_NumHealingPotions;
	int m_SwordLevel;
	int m_KeepNetsStocked;
	int m_NumNets;
};

#endif  /* __CGANGS_H */
