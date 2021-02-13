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
#include "buildings/cBuildingManager.h"
#include "cRng.h"
#include "cInventory.h"
#include <sstream>
#include "IGame.h"
#include "sStorage.h"
#include "CLog.h"
#include "cGirlGangFight.h"
#include "combat/combat.h"
#include "combat/combatant.h"
#include "character/predicates.h"
#include "character/cCustomers.h"
#include "cGirls.h"

// `J` Job Arena - Fighting
bool WorkFightBeast(sGirl& girl, bool Day0Night1, cRng& rng)
{
    auto brothel = girl.m_Building;

    Action_Types actiontype = ACTION_COMBAT;
    std::stringstream ss;

    if (g_Game->storage().beasts() < 1)
    {
        ss << "${name} had no beasts to fight.";
        girl.AddMessage(ss.str(), IMGTYPE_PROFILE, Day0Night1 ? EVENT_NIGHTSHIFT : EVENT_DAYSHIFT);
        return false;    // not refusing
    }
    int roll = rng.d100();
    if (roll <= 10 && girl.disobey_check(actiontype, JOB_FIGHTBEASTS))
    {
        ss << "${name} refused to fight beasts today.\n";
        girl.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_NOWORK);
        return true;
    }

    cGirls::EquipCombat(girl);
    bool has_armor = girl.get_num_item_equiped(sInventoryItem::Armor);
    bool has_wpn = girl.get_num_item_equiped(sInventoryItem::Weapon) + girl.get_num_item_equiped(sInventoryItem::SmWeapon);

    if (!has_armor)
    {
        ss << "The crowd can't believe you sent ${name} out to fight without armor";
        if (!has_wpn)
        {
            ss << " or a weapon.";
        }
        else
        {
            ss << ". But at least she had a weapon.";
        }


    }
    else
    {
        ss << "${name} came out in armor";
        if (has_wpn)
        {
            ss << " but didn't have a weapon.";
        }
        else
        {
            ss << " and with a weapon in hand. The crowd felt she was ready for battle.";
        }
    }
    ss << "\n \n";

    int wages = 175, tips = 0, enjoy = 0;
    double jobperformance = girl.job_performance(JOB_FIGHTBEASTS, false);
    int fightxp = 1;

    // TODO need better dialog
    Combat combat(ECombatObjective::KILL, ECombatObjective::KILL);
    combat.add_combatant(ECombatSide::ATTACKER, girl);
    auto beast = std::make_unique<Combatant>("Beast", 100, 0, 0,
            g_Dice.in_range(40, 80), g_Dice.in_range(40, 80), 0,
            g_Dice.in_range(40, 80), g_Dice.in_range(40, 80));
    combat.add_combatant(ECombatSide::DEFENDER, std::move(beast));

    auto result = combat.run(15);
    auto report = std::make_shared<CombatReport>();
    report->rounds = combat.round_summaries();
    girl.m_Events.AddMessage(combat.round_summaries().back(), EVENT_GANG, report);

    if (result == ECombatResult::VICTORY)    // she won
    {
        fightxp = 3;
        ss << " fights against a beast. She won the fight.";//was confusing
        enjoy += 3;
        girl.AddMessage(ss.str(), IMGTYPE_COMBAT, Day0Night1 ? EVENT_NIGHTSHIFT : EVENT_DAYSHIFT);
        int roll_max = girl.fame() + girl.charisma();
        roll_max /= 4;
        wages += 10 + rng%roll_max;
        girl.m_Tips = std::max(0, tips);
        girl.m_Pay = std::max(0, wages);
        girl.fame(2);
    }
    else  // she lost or it was a draw
    {
        ss << " was unable to win the fight.";
        enjoy -= 1;
        //Crazy i feel there needs be more of a bad outcome for losses added this... Maybe could use some more
        if (brothel->is_sex_type_allowed(SKILL_BEASTIALITY) && !is_virgin(girl))
        {
            ss << " So as punishment you allow the beast to have its way with her."; enjoy -= 1;
            girl.upd_temp_stat(STAT_LIBIDO, -50, true);
            girl.beastiality(2);
            girl.AddMessage(ss.str(), IMGTYPE_BEAST, Day0Night1 ? EVENT_NIGHTSHIFT : EVENT_DAYSHIFT);
            if (!girl.calc_insemination(cGirls::GetBeast(), 1.0))
            {
                g_Game->push_message(girl.FullName() + " has gotten inseminated", 0);
            }
        }
        else
        {
            ss << " So you send your men in to cage the beast before it can harm her.";
            girl.AddMessage(ss.str(), IMGTYPE_COMBAT, Day0Night1 ? EVENT_NIGHTSHIFT : EVENT_DAYSHIFT);
            girl.fame(-1);
        }
    }

    int kills = rng % 6 - 4;                 // `J` how many beasts she kills 0-2
    if (g_Game->storage().beasts() < kills)    // or however many there are
        kills = g_Game->storage().beasts();
    if (kills < 0) kills = 0;                // can't gain any
    g_Game->storage().add_to_beasts(-kills);

    if (girl.is_pregnant())
    {
        if (girl.strength() >= 60)
        {
            ss << "\n \nAll that fighting proved to be quite exhausting for a pregnant girl, even for one as strong as ${name} .\n";
        }
        else
        {
            ss << "\n \nAll that fighting proved to be quite exhausting for a pregnant girl like ${name} .\n";
        }
        girl.tiredness(10 - girl.strength() / 20 );
    }



    if (roll <= 15)
    {
        ss << " didn't like fighting beasts today.";
        enjoy -= 3;
    }
    else if (roll >= 90)
    {
        ss << " loved fighting beasts today.";
        enjoy += 3;
    }
    else
    {
        ss << " had a pleasant time fighting beasts today.";
        enjoy += 1;
    }
    ss << "\n \n";

    if (girl.is_unpaid())
    {
        wages = 0;
    }

    int earned = 0;
    for (int i = 0; i < jobperformance; i++)
    {
        earned += rng % 10 + 5; // 5-15 gold per customer  This may need tweaked to get it where it should be for the pay
    }
    brothel->m_Finance.arena_income(earned);
    ss.str("");
    ss << "${name} drew in " << jobperformance << " people to watch her and you earned " << earned << " from it.";
    girl.AddMessage(ss.str(), IMGTYPE_PROFILE, Day0Night1 ? EVENT_NIGHTSHIFT : EVENT_DAYSHIFT);

    girl.upd_Enjoyment(actiontype, enjoy);
    // Improve girl
    int xp = 3 * fightxp, skill = 1;

    if (girl.has_active_trait("Quick Learner"))        { skill += 1; xp += 3; }
    else if (girl.has_active_trait("Slow Learner"))    { skill -= 1; xp -= 3; }

    girl.exp(xp);
    girl.combat(rng%fightxp + skill);
    girl.magic(rng%fightxp + skill);
    girl.agility(rng%fightxp + skill);
    girl.constitution(rng%fightxp + skill);
    girl.beastiality(rng%fightxp * 2 + skill);

    cGirls::PossiblyGainNewTrait(girl, "Tough", 20, actiontype, "She has become pretty Tough from all of the fights she's been in.", Day0Night1);
    cGirls::PossiblyGainNewTrait(girl, "Aggressive", 60, actiontype, "She is getting rather Aggressive from her enjoyment of combat.", Day0Night1);
    cGirls::PossiblyGainNewTrait(girl, "Fleet of Foot", 30, actiontype, "She is getting rather fast from all the fighting.", Day0Night1);
    if (rng.percent(25) && girl.strength() >= 60 && girl.combat() > girl.magic())
    {
        cGirls::PossiblyGainNewTrait(girl, "Strong", 60, ACTION_COMBAT, "${name} has become pretty Strong from all of the fights she's been in.", Day0Night1);
    }

    //lose traits
    cGirls::PossiblyLoseExistingTrait(girl, "Fragile", 75, actiontype, "${name} has had to heal from so many injuries you can't say she is fragile anymore.", Day0Night1);

    return false;
}

double JP_FightBeast(const sGirl& girl, bool estimate)// not used
{
    double jobperformance = 0.0;

    if (estimate)// for third detail string
    {
        jobperformance +=
            (girl.fame() / 2) +
            (girl.charisma() / 2) +
            (girl.combat() / 2) +
            (girl.magic() / 2) +
            (girl.level());
    }
    else// for the actual check
    {
        jobperformance += (girl.fame() + girl.charisma()) / 2;
        if (!estimate)
        {
            int t = girl.tiredness() - 80;
            if (t > 0)
                jobperformance -= (t + 2) * (t / 2);
        }
    }

    jobperformance += girl.get_trait_modifier("work.fightarena");

    return jobperformance;
}
