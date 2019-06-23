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
	int m_Skills[NUM_SKILLS];	// skills of the gang
	int m_Stats[NUM_STATS];	// stats of the gang
	int m_Heal_Limit;	// number of potions the gang has
	int m_Net_Limit;	// number of nets the gang has
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

	// `J` have the gangs only get a set amount each turn
	int	heal_limit()			{ return m_Heal_Limit; }		
	void heal_limit(int n)		{ m_Heal_Limit += n; }
	void set_heal_limit(int n)	{ m_Heal_Limit = n; }
	int	net_limit()				{ return m_Net_Limit; }
	void net_limit(int n)		{ m_Net_Limit += n; }
	void set_net_limit(int n)	{ m_Net_Limit = n; }

	int magic()			{ return m_Skills[SKILL_MAGIC]; }
	int combat()		{ return m_Skills[SKILL_COMBAT]; }
	int service()		{ return m_Skills[SKILL_SERVICE]; }		// `J` added for .06.02.41
	int intelligence()	{ return m_Stats[STAT_INTELLIGENCE]; }
	int agility()		{ return m_Stats[STAT_AGILITY]; }
	int constitution()	{ return m_Stats[STAT_CONSTITUTION]; }
	int charisma()		{ return m_Stats[STAT_CHARISMA]; }
	int strength()		{ return m_Stats[STAT_STRENGTH]; }
	int happy()			{ return m_Stats[STAT_HAPPINESS]; }


	// `J` added for .06.02.41
	void AdjustGangSkill(sGang* gang, int skill, int amount);
	void AdjustGangStat(sGang* gang, int stat, int amount);
	void AdjustGangSkill(int skill, int amount);
	void AdjustGangStat(int stat, int amount);

	void magic(int amount)			{ AdjustGangSkill(SKILL_MAGIC, amount); }
	void combat(int amount)			{ AdjustGangSkill(SKILL_COMBAT, amount); }
	void service(int amount)			{ AdjustGangSkill(SKILL_SERVICE, amount); }
	void intelligence(int amount)	{ AdjustGangStat(STAT_INTELLIGENCE, amount); }
	void agility(int amount)			{ AdjustGangStat(STAT_AGILITY, amount); }
	void constitution(int amount)	{ AdjustGangStat(STAT_CONSTITUTION, amount); }
	void charisma(int amount)		{ AdjustGangStat(STAT_CHARISMA, amount); }
	void strength(int amount)		{ AdjustGangStat(STAT_STRENGTH, amount); }
	void happy(int amount)			{ AdjustGangStat(STAT_HAPPINESS, amount); }



	sGang()
	{
		m_Name = "Unnamed";
		m_Num = 0;
		m_Prev = m_Next = 0;
		m_MissionID = MISS_GUARDING;
		m_Combat = false;
		m_LastMissID = -1;
		m_AutoRecruit = false;
		for (int i = 0; i < NUM_SKILLS; i++)	m_Skills[i] = 0;
		for (int i = 0; i < NUM_STATS; i++)		m_Stats[i] = 0;
		m_Stats[STAT_HEALTH] = 100;
		m_Stats[STAT_HAPPINESS] = 100;

	}

	~sGang()
	{
		if(m_Next) delete m_Next;
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
	void RemoveHireableGang(int gangID);								// removed a recruitable gang from the list
	void RemoveHireableGang(sGang* gang);
	void RemoveGang(int gangID);										// removed a controlled gang completely from service
	void RemoveGang(sGang* gang);
	void SendGang(int gangID, int missID);								// sends a gang on a mission
	sGang* GetGang(int gangID);											// gets a gang
	sGang* GetHireableGang(int gangID);									// gets a recruitable gang
	sGang* GetGangOnMission(u_int missID);								// gets a gang on the current mission
	sGang* GetRandomGangOnMission(u_int missID);						// gets a random gang on the current mission
	sGang* GetGangNotFull(int roomfor = 0, bool recruiting = true);		// gets a gang with room to spare
	sGang* GetGangRecruitingNotFull(int roomfor = 0);					// gets a gang recruiting with room to spare
	void UpdateGangs();

	int GetMaxNumGangs();
	int GetNumGangs();
	int GetNumHireableGangs();
	bool GangCombat(sGirl* girl, sGang* gang);								// returns true if the girl wins
	bool GangBrawl(sGang* gang1, sGang* gang2, bool rivalVrival = false);	// returns true if gang1 wins
	sGang* GetTempGang();													// creates a new gang
	sGang* GetTempGang(int mod);											// creates a new gang with stat/skill mod
	void BoostGangSkill(int* affect_skill, int count = 1);					// increases a specific skill/stat the specified number of times
	void BoostGangRandomSkill(vector<int*>* possible_skills, int count = 1, int boost_count = 1);  // chooses from the passed skills/stats and raises one or more of them
	void BoostGangCombatSkills(sGang* gang, int count = 1);					// simple function to increase a gang's combat skills a bit

	sGang* GetTempWeakGang();

	TiXmlElement* SaveGangsXML(TiXmlElement* pRoot);
	bool LoadGangsXML(TiXmlHandle hGangManager);

	int GetNumBusinessExtorted()		{ return m_BusinessesExtort; }
	int NumBusinessExtorted(int n)		{ m_BusinessesExtort += n; return m_BusinessesExtort; }

	int* GetWeaponLevel() {return &m_SwordLevel;}						// 

	int* GetNets() {return &m_NumNets;}									// 
	int GetNetRestock() {return m_KeepNetsStocked;}						// 
	void KeepNetStocked(int stocked){m_KeepNetsStocked = stocked;}		// 
	int net_limit();													// 

	int* GetHealingPotions() {return &m_NumHealingPotions;}				// 
	void KeepHealStocked(int stocked){m_KeepHealStocked = stocked;}		// 
	int GetHealingRestock() {return m_KeepHealStocked;}					// 
	int healing_limit();												// 

	bool sabotage_mission(sGang* gang);									// 
	bool recapture_mission(sGang* gang);								// 
	bool extortion_mission(sGang* gang);								// 
	bool petytheft_mission(sGang* gang);								// 
	bool grandtheft_mission(sGang* gang);								// 
	bool kidnapp_mission(sGang* gang);									// 
	bool catacombs_mission(sGang* gang);								// 
	bool gangtraining(sGang* gang);										// 
	bool gangrecruiting(sGang* gang);									// 
	bool service_mission(sGang* gang);									// 
	bool losegang(sGang* gang);
	void check_gang_recruit(sGang* gang);
	void GangStartOfShift();
	void RestockNetsAndPots();

	int chance_to_catch(sGirl* girl);

	// Used by the new brothel security code
	sGang*	random_gang(vector<sGang*>& v);
	bool GirlVsEnemyGang(sGirl* girl, sGang* enemy_gang);

	vector<sGang*> gangs_on_mission(u_int mission_id);
	vector<sGang*> gangs_watching_girls();

	bool Control_Gangs()	{ return m_Control_Gangs; }
	int Gang_Gets_Girls()	{ return m_Gang_Gets_Girls; }
	int Gang_Gets_Items()	{ return m_Gang_Gets_Items; }
	int Gang_Gets_Beast()	{ return m_Gang_Gets_Beast; }
	bool Control_Gangs(bool cg)	{ return m_Control_Gangs = cg; }
	int Gang_Gets_Girls(int g)	{ return m_Gang_Gets_Girls = g; }
	int Gang_Gets_Items(int g)	{ return m_Gang_Gets_Items = g; }
	int Gang_Gets_Beast(int g)	{ return m_Gang_Gets_Beast = g; }


private:
	int m_BusinessesExtort;	// number of businesses under your control

	bool m_Control_Gangs;
	int m_Gang_Gets_Girls;
	int m_Gang_Gets_Items;
	int m_Gang_Gets_Beast;

	int m_MaxNumGangs;
	int m_NumGangNames;
	int m_NumGangs;
	sGang* m_GangStart;	// the start and end of the list of gangs under the players employment
	sGang* m_GangEnd;
	int m_NumHireableGangs;
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
