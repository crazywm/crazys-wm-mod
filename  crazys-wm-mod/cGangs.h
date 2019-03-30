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

#include <vector>
#include <memory>
#include "Constants.h"
#include "cEvents.h"

class TiXmlElement;
class TiXmlHandle;
class sGirl;
class IGangMission;

// A gang of street toughs
struct sGang
{
public:
    sGang(std::string name="");
    ~sGang() = default;

    TiXmlElement* SaveGangXML(TiXmlElement* pRoot);
    bool LoadGangXML(TiXmlHandle hGang);

    const std::string& name() const { return m_Name; }

    // potions
    /// Give potions to the gang. At most, every member of the gang can have a potion. Returns the number of potions that
    /// were actually distributed.
    int give_potions(int n);
    /// returns the current number of potions.
    int num_potions() const { return m_Potions; }
    /// use a healing potion
    void use_potion();

    // nets
    /// Give nets to the gang. At most, every second member of the gang can have a net. Returns the number of nets that
    /// were actually distributed.
    int give_nets(int n);
    /// returns the current number of nets that the gang has
    int num_nets() const  { return m_Nets; }
    /// try to use the net to catch the target girl. Has a chance of destroying a net. Returns whether the girl was caught.
    bool use_net(sGirl& target);
    bool damage_net(double chance);

    int members() const;

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
    void AdjustGangSkill(int skill, int amount);
    void AdjustGangStat(int stat, int amount);

    void magic(int amount)			{ AdjustGangSkill(SKILL_MAGIC, amount); }
    void combat(int amount)			{ AdjustGangSkill(SKILL_COMBAT, amount); }
    void service(int amount)		{ AdjustGangSkill(SKILL_SERVICE, amount); }
    void intelligence(int amount)	{ AdjustGangStat(STAT_INTELLIGENCE, amount); }
    void agility(int amount)		{ AdjustGangStat(STAT_AGILITY, amount); }
    void constitution(int amount)	{ AdjustGangStat(STAT_CONSTITUTION, amount); }
    void charisma(int amount)		{ AdjustGangStat(STAT_CHARISMA, amount); }
    void strength(int amount)		{ AdjustGangStat(STAT_STRENGTH, amount); }
    void happy(int amount)			{ AdjustGangStat(STAT_HAPPINESS, amount); }

    void BoostSkill(int* affect_skill, int count = 1);					// increases a specific skill/stat the specified number of times
    void BoostStat(STATS stat, int count = 1);
    void BoostSkill(SKILLS skill, int count = 1);
    void BoostCombatSkills(int amount);
    void BoostRandomSkill(const std::vector<int*>& possible_skills, int count = 1, int boost_count = 1);  // chooses from the passed skills/stats and raises one or more of them
public:
    int m_Num = 0;	// number in the gang
    int m_Skills[NUM_SKILLS];	// skills of the gang
    int m_Stats[NUM_STATS];	// stats of the gang

    u_int m_MissionID;	// the type of mission currently performing
    int m_LastMissID = -1;	// the last mission if auto changed to recruit mission
    bool m_AutoRecruit;	// true if auto recruiting
    bool m_Combat = false;	// is true when gang has seen combat in the last week
    cEvents m_Events;
private:
    std::string m_Name;

    int m_Potions = 0;	// number of potions the gang has
    int m_Nets = 0;	// number of nets the gang has
};

/*
 * manages all the player gangs
 */
class cGangManager
{
public:
    cGangManager();
    ~cGangManager();

    void Free();

    void AddNewGang(bool boosted = false);	// Adds a new randomly generated gang to the recruitable list
    void HireGang(int gangID);	// hired a recruitable gang, so add it to your gangs
    void FireGang(int gangID);	// fired a gang, so send it back to recruitables (or just delete if full up)
    void AddGang(unique_ptr<sGang> newGang);

    // removed a controlled gang completely from service
    void RemoveGang(sGang* gang);
    // sends a gang on a mission
    sGang* GetGang(int gangID);											// gets a gang
    // gets a recruitable gang
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
    sGang GetTempGang();													// creates a new gang
    sGang GetTempGang(int mod);											    // creates a new gang with stat/skill mod

    sGang* GetTempWeakGang();

    TiXmlElement* SaveGangsXML(TiXmlElement* pRoot);
    bool LoadGangsXML(TiXmlHandle hGangManager);

    int GetNumBusinessExtorted()		{ return m_BusinessesExtort; }
    int NumBusinessExtorted(int n)		{ m_BusinessesExtort += n; return m_BusinessesExtort; }

    int* GetWeaponLevel() {return &m_SwordLevel;}						//

    int GetNets() {return m_NumNets;}									//
    int GetNetRestock() {return m_KeepNetsStocked;}						//
    void KeepNetStocked(int stocked){m_KeepNetsStocked = stocked;}		//
    int  BuyNets(int amount, bool autobuy);

    int GetHealingPotions() {return m_NumHealingPotions;}				//
    void KeepHealStocked(int stocked){m_KeepHealStocked = stocked;}		//
    int  GetHealingRestock() {return m_KeepHealStocked;}					//
    int  BuyHealingPotions(int amount, bool autobuy);

    bool losegang(sGang& gang);
    void check_gang_recruit(sGang& gang);
    void GangStartOfShift();
    void RestockNetsAndPots();

    int chance_to_catch(sGirl* girl);

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


    std::vector<std::unique_ptr<sGang>>& GetPlayerGangs() { return m_PlayersGangs; }
    std::vector<std::unique_ptr<sGang>>& GetHireableGangs() { return m_HireableGangs; }
private:
    int m_BusinessesExtort;	// number of businesses under your control

    bool m_Control_Gangs;
    int m_Gang_Gets_Girls;
    int m_Gang_Gets_Items;
    int m_Gang_Gets_Beast;

    int m_MaxNumGangs;
    std::vector<std::string> m_GangNames;
    std::vector<std::unique_ptr<sGang>> m_PlayersGangs;
    std::vector<std::unique_ptr<sGang>> m_HireableGangs;
    std::vector<std::unique_ptr<IGangMission>> m_Missions;

    // gang armory
    int m_KeepHealStocked = 0;
    int m_NumHealingPotions = 0;
    int m_SwordLevel;
    int m_KeepNetsStocked = 0;
    int m_NumNets = 0;
};

#endif  /* __CGANGS_H */
