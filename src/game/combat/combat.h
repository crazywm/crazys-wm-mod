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

#ifndef WM_COMBAT_H
#define WM_COMBAT_H

#include <vector>
#include <sstream>
#include <memory>
#include "cRng.h"

struct Party;
struct Combatant;
class ICombatAction;
class ICharacter;
class sGang;

enum class ECombatObjective {
    ESCAPE,
    CAPTURE,
    KILL
};

enum class ECombatSide {
    ATTACKER,
    DEFENDER
};

enum class ECombatResult {
    VICTORY,
    DEFEAT,
    DRAW
};

struct Party {
    Party(ECombatSide side, ECombatObjective obj);
    ~Party();
    Party(const Party&) = delete;

    ECombatObjective aim;
    ECombatSide side;

    std::vector<std::unique_ptr<Combatant>> members;
    std::vector<Combatant*> active_members;

    Combatant& add_member(std::unique_ptr<Combatant> cbt);
    void notify_end_combat(Combatant* cbt);
    void log_status(std::ostream& os, bool brief=false);

    bool is_defeated() const;
    bool is_repelled() const;

    // has any member of this party attacked in this round
    bool has_attacked = false;

    Combatant& draw_target();
    void finalize();
};

struct CombatOrderEntry {
    Combatant* actor;
    int initiative;
    bool operator<(const CombatOrderEntry& o) const {
        return initiative >= o.initiative;
    }
};

class Combat {
public:
    Combat(ECombatObjective attacker_aim, ECombatObjective defender_aim);
    ~Combat();

    Combatant& add_combatant(ECombatSide side, std::unique_ptr<Combatant> combatant);
    void add_combatants(ECombatSide side, sGang& gang, int chance=100);
    void add_combatant(ECombatSide side, ICharacter& character);
    ECombatResult run(int max_duration);

    const std::vector<std::string>& round_summaries() const { return m_RoundSummaries; }
    std::stringstream& narration() { return m_Narration; }
    cRng& rng();

    Party& get_attackers() { return m_Attackers; };
    Party& get_defenders() { return m_Defenders; };
private:
    void get_initiatives(Party& party);
    bool round(int num);
    void act(Combatant& actor);

    Party m_Attackers;
    Party m_Defenders;

    std::vector<CombatOrderEntry> m_CombatOrder;
    std::vector<std::unique_ptr<ICombatAction>> m_Actions;

    std::stringstream m_Narration;
    std::vector<std::string> m_RoundSummaries;

    std::vector<double> m_ScoreCache;
};

#endif //WM_COMBAT_H
