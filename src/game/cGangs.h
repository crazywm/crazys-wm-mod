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

#pragma once

#include <vector>
#include <memory>
#include <tinyxml2.h>
#include "Constants.h"
#include "cEvents.h"


class sGirl;
class IGangMission;

// A gang of street toughs
struct sGang {

public:

    sGang(std::string name="");
    ~sGang() = default;

    tinyxml2::XMLElement& SaveGangXML(tinyxml2::XMLElement& elRoot);
    bool LoadGangXML(const tinyxml2::XMLElement& hGang);

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

    // weapons
    int weapon_level() const { return m_WpnLevel; }
    void set_weapon_level(int lvl) { m_WpnLevel = std::min(lvl, 3); }

    int magic()   const  { return m_Skills[SKILL_MAGIC]; }
    int combat()  const  { return m_Skills[SKILL_COMBAT]; }
    int service() const  { return m_Skills[SKILL_SERVICE]; }        // `J` added for .06.02.41
    int intelligence() const { return m_Stats[STAT_INTELLIGENCE]; }
    int agility() const  { return m_Stats[STAT_AGILITY]; }
    int constitution() const    { return m_Stats[STAT_CONSTITUTION]; }
    int charisma() const { return m_Stats[STAT_CHARISMA]; }
    int strength() const { return m_Stats[STAT_STRENGTH]; }
    int happy() const    { return m_Stats[STAT_HAPPINESS]; }


    // `J` added for .06.02.41
    void AdjustGangSkill(int skill, int amount);
    void AdjustGangStat(int stat, int amount);

    void magic(int amount)            { AdjustGangSkill(SKILL_MAGIC, amount); }
    void combat(int amount)            { AdjustGangSkill(SKILL_COMBAT, amount); }
    void service(int amount)        { AdjustGangSkill(SKILL_SERVICE, amount); }
    void intelligence(int amount)    { AdjustGangStat(STAT_INTELLIGENCE, amount); }
    void agility(int amount)        { AdjustGangStat(STAT_AGILITY, amount); }
    void constitution(int amount)    { AdjustGangStat(STAT_CONSTITUTION, amount); }
    void charisma(int amount)        { AdjustGangStat(STAT_CHARISMA, amount); }
    void strength(int amount)        { AdjustGangStat(STAT_STRENGTH, amount); }
    void happy(int amount)            { AdjustGangStat(STAT_HAPPINESS, amount); }

    void BoostSkill(int* affect_skill, int count = 1);                    // increases a specific skill/stat the specified number of times
    void BoostStat(STATS stat, int count = 1);
    void BoostSkill(SKILLS skill, int count = 1);
    void BoostCombatSkills(int amount);
    void BoostRandomSkill(const std::vector<int*>& possible_skills, int count = 1, int boost_count = 1);  // chooses from the passed skills/stats and raises one or more of them

    cEvents& GetEvents() { return m_Events; }
    void AddMessage(std::string message, EventType event_type = EventType::EVENT_GANG);
public:
    static int max_members(); // number of memberts a gang can have
    int m_Num = 0;    // number in the gang
    int m_Skills[NUM_SKILLS];    // skills of the gang
    int m_Stats[NUM_STATS];    // stats of the gang

    int m_MissionID;    // the type of mission currently performing
    int m_LastMissID = -1;    // the last mission if auto changed to recruit mission
    bool m_AutoRecruit;    // true if auto recruiting
    bool m_Combat = false;    // is true when gang has seen combat in the last week

    int m_MedicalCost = 0;      // If the members of the gang have been hurt, this is what you need to pay the doctors to heal them
private:
    std::string m_Name;

    int m_Potions = 0;    // number of potions the gang has
    int m_Nets = 0;        // number of nets the gang has
    int m_WpnLevel = 0; // weapon level

    cEvents m_Events;
};
