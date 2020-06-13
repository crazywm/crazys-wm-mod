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
#include <utils/streaming_random_selection.hpp>
#include "cGirlGangFight.h"
#include "cGangs.h"
#include "Game.hpp"
#include "character/sGirl.hpp"
#include "character/cPlayer.h"
#include "combat/combat.h"

extern cRng g_Dice;

EGirlEscapeAttemptResult AttemptEscape(sGirl& girl) {
    // decide if she's going to fight or flee
    if (!girl.fights_back())        return EGirlEscapeAttemptResult::SUBMITS;

    // ok, she fights. Find all the gangs on guard duty
    vector<sGang*> v = g_Game->gang_manager().gangs_on_mission(MISS_GUARDING);
    if (!v.empty())
    {
        int index = g_Dice.in_range(0, v.size() - 1);
        sGang *gang = v[index];
        g_LogFile.log(ELogLevel::DEBUG, "cGirlGangFight: random gang index = ", index, " gang = ", gang->name());

        Combat combat(ECombatObjective::CAPTURE, ECombatObjective::ESCAPE);
        combat.add_combatants(ECombatSide::ATTACKER, *gang, 50);
        combat.add_combatant(ECombatSide::DEFENDER, girl);
        gang->m_Combat = true;      // TODO this is interactive, so maybe does not really change things...

        auto result = combat.run(10);
        for(auto& round : combat.round_summaries()) {
            g_Game->push_message(round, 0);
        }

        if (result == ECombatResult::VICTORY)
            return EGirlEscapeAttemptResult::STOPPED_BY_GOONS;

        g_Game->push_message(girl.FullName() + " managed to escape your goons. You try to catch her yourself.", 1);
    } else {
        g_Game->push_message("You dont have any goons to catch " + girl.FullName() + ". You have to catch her yourself.", 1);
    }

    Combat pc_combat(ECombatObjective::CAPTURE, ECombatObjective::ESCAPE);
    pc_combat.add_combatant(ECombatSide::ATTACKER, g_Game->player());
    pc_combat.add_combatant(ECombatSide::DEFENDER, girl);

    auto pc_result = pc_combat.run(10);
    for(auto& round : pc_combat.round_summaries()) {
        g_Game->push_message(round, 0);
    }

    if (pc_result == ECombatResult::VICTORY)
    {
        return EGirlEscapeAttemptResult::STOPPED_BY_PLAYER;
    }
    return EGirlEscapeAttemptResult::SUCCESS;
}


EAttemptCaptureResult AttemptCapture(sGang& gang, sGirl& girl) {
    // decide if she's going to fight or flee
    /// TODO need a way to influence this
    // if (!girl.fights_back())        return EAttemptCaptureResult::SUBMITS;

    Combat combat(ECombatObjective::CAPTURE, ECombatObjective::ESCAPE);
    combat.add_combatants(ECombatSide::ATTACKER, gang, 50);
    combat.add_combatant(ECombatSide::DEFENDER, girl);

    /// TODO ambush mechanics for first round -- net throwing
    auto result = combat.run(10);
    auto report = std::make_shared<CombatReport>();
    report->rounds = combat.round_summaries();
    gang.m_Events.AddMessage(combat.round_summaries().back(), EVENT_GANG, report);
    gang.m_Combat = true;

    if (result == ECombatResult::VICTORY)
        return EAttemptCaptureResult::CAPTURED;
    return EAttemptCaptureResult::ESCAPED;
}

EFightResult GangBrawl(sGang& a, sGang& b) {
    Combat combat(ECombatObjective::KILL, ECombatObjective::KILL);
    combat.add_combatants(ECombatSide::ATTACKER, a, 75);
    combat.add_combatants(ECombatSide::DEFENDER, b, 75);

    int start_num_a = a.m_Num;
    int start_num_b = b.m_Num;

    auto result = combat.run(10);
    auto report = std::make_shared<CombatReport>();
    report->rounds = combat.round_summaries();

    /// TODO return the report instead of pushing? So we can attach a custom summary
    std::string a_result = " \nThey lost " + std::to_string(start_num_a - a.m_Num) + " members.";
    std::string b_result = " \nThey lost " + std::to_string(start_num_b - b.m_Num) + " members.";
    a.m_Events.AddMessage(combat.round_summaries().back() + a_result, EVENT_GANG, report);
    b.m_Events.AddMessage(combat.round_summaries().back() + b_result, EVENT_GANG, report);

    a.m_Combat = true;
    b.m_Combat = true;

    switch(result) {
        case ECombatResult::VICTORY:
            return EFightResult::VICTORY;
        case ECombatResult::DRAW: {
            // repelled gangs count as defeated
            if(combat.get_attackers().is_repelled()) return EFightResult::DEFEAT;
            if(combat.get_defenders().is_repelled()) return EFightResult::VICTORY;
            return EFightResult::DRAW;
        }
        case ECombatResult::DEFEAT:
            return EFightResult::DEFEAT;
    }
}

EFightResult GirlFightsGirl(sGirl& a, sGirl& b) {
    Combat combat(ECombatObjective::CAPTURE, ECombatObjective::KILL);
    combat.add_combatant(ECombatSide::ATTACKER, a);
    combat.add_combatant(ECombatSide::DEFENDER, b);

    auto result = combat.run(10);
    auto report = std::make_shared<CombatReport>();
    report->rounds = combat.round_summaries();
    a.m_Events.AddMessage(combat.round_summaries().back(), EVENT_GANG, report);
    b.m_Events.AddMessage(combat.round_summaries().back(), EVENT_GANG, report);

    switch(result) {
        case ECombatResult::VICTORY:
            return EFightResult::VICTORY;
        case ECombatResult::DRAW:
            return EFightResult::DRAW;
        case ECombatResult::DEFEAT:
            return EFightResult::DEFEAT;
    }
}
