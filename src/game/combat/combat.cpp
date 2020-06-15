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
#include "combat.h"
#include "action.h"
#include "combatant.h"

#include <algorithm>
#include <main.h>
#include "utils/streaming_random_selection.hpp"
#include "cGirlGangFight.h"
#include "cGangs.h"
#include "Game.hpp"
#include "character/sGirl.hpp"
#include "character/cPlayer.h"
#include <cmath>

extern cRng g_Dice;

Combatant& Party::draw_target() {
    return *active_members[ g_Dice % active_members.size() ];
}

Combatant& Party::add_member(std::unique_ptr<Combatant> cbt) {
    members.push_back(std::move(cbt));
    active_members.push_back(members.back().get());
    return *members.back();
}

void Party::notify_end_combat(Combatant* cbt) {
    auto found = std::find(begin(active_members), end(active_members), cbt);
    if(found != active_members.end()) {
        active_members.erase(found);
    }
}

void Party::log_status(std::ostream& os, bool brief) {
    for(auto& m : members) {
        m->log_status(os, brief);
        os << "\n";
    }
}

bool Party::is_defeated() const {
    for(auto& m : members) {
        if(!(m->is_dead() || m->is_captured())) return false;
    }
    return true;
}

bool Party::is_repelled() const {
    for(auto& m : members) {
        if(!(m->is_dead() || m->is_captured() || m->is_escaped())) return false;
    }
    return true;
}


Party::Party(ECombatSide s, ECombatObjective obj) : aim(obj), side(s) {}

void Party::finalize() {
    for(auto& m : members) {
        m->finalize();
    }
}

Party::~Party() = default;


cRng& Combat::rng() {
    return g_Dice;
}

Combat::~Combat() = default;

void Combat::get_initiatives(Party& party) {
    for(auto& member : party.members) {
        if(!member->can_act()) continue;

        int initiative = g_Dice % member->get_initiative();
        m_CombatOrder.push_back(CombatOrderEntry{member.get(), initiative});
    }
}

bool Combat::round(int num) {
    // logging
    narration() << "   ** ROUND " << num << " **\n";
    m_Attackers.log_status(narration());
    narration() << "    -- VS --   \n";
    m_Defenders.log_status(narration());
    narration() << "\n\n";

    m_Attackers.has_attacked = false;
    m_Defenders.has_attacked = false;
    m_CombatOrder.clear();

    g_LogFile.debug("combat", "Initiative phase");
    get_initiatives(m_Attackers);
    get_initiatives(m_Defenders);

    // sort combatants according to initiative

    // for some reason, std::sort occasionally caused crashes here,
    // this manual bubble sort might be less efficient, but we do
    // not expect many entries anyway, so it shouldn't matter too much
    while(true) {
        bool swapped = false;
        for(int i = 0; i < m_CombatOrder.size() - 1; ++i)
        {
            if(m_CombatOrder[i].initiative > m_CombatOrder[i+1].initiative) {
                std::swap(m_CombatOrder[i], m_CombatOrder[i+1]);
                swapped = true;
            }
        }
        if(!swapped)
            break;
    }
    //std::sort(begin(m_CombatOrder), end(m_CombatOrder));

    for(auto& cb : m_CombatOrder) {
        cb.actor->begin_round();
    }

    // now let everybody act
    g_LogFile.debug("combat", "Action phase");
    for(auto& cb : m_CombatOrder) {
        if(!cb.actor->is_dead() && !cb.actor->is_captured())
            act(*cb.actor);
    }

    g_LogFile.debug("combat", "Recovery phase");
    // now everybody recovers
    for(auto& cb : m_CombatOrder) {
        if(cb.actor->is_dead())
            continue;

        cb.actor->end_round();
        if(cb.actor->is_escaped()) {
            const_cast<Party*>(cb.actor->get_party())->notify_end_combat(cb.actor);
            narration() << cb.actor->get_name() << " has fled the fight for good!\n";
            continue;
        }

        if(cb.actor->get_hitpoints() < 150 && cb.actor->consume_health_potion()) {
            narration() << cb.actor->get_name() << " drinks a health potion. Now has " << cb.actor->get_hitpoints() << "HP.\n";
        }
    }

    if(!m_Attackers.has_attacked && !m_Defenders.has_attacked && rng().percent(50)) {
        // check that this is not only a temporary thing
        bool all_exhausted = true;
        for(auto& a : m_Attackers.active_members) {
            all_exhausted &= a->get_vitality() < 150;
        }

        for(auto& a : m_Defenders.active_members) {
            all_exhausted &= a->get_vitality() < 150;
        }

        if(all_exhausted) {
            narration() << "The combat has gone on for so long that everybody is too tired for further fighting. "
                           "It's a draw.";
            return false;
        }
    }

    return !(m_Attackers.active_members.empty() || m_Defenders.active_members.empty());
}

void Combat::act(Combatant& actor) {
    // first, select a target
    if(actor.get_enemies()->active_members.empty())
        return;

    auto& target = const_cast<Party*>(actor.get_enemies())->draw_target();

    g_LogFile.debug("combat", actor.get_name(), " picks target ", target.get_name());
    g_LogFile.debug("combat", actor);
    g_LogFile.debug("combat", target);

    RandomSelector<ICombatAction> select;
    m_ScoreCache.clear();
    double max_score = -1000;
    for(auto& action : m_Actions) {
        double score = action->score(actor, target);
        m_ScoreCache.push_back(score);
        max_score = std::max(max_score, score);
    }

    g_LogFile.debug("combat", "Maximum action score ", max_score);
    for(int i = 0; i < m_Actions.size(); ++i) {
        auto& action = m_Actions[i];
        auto score = m_ScoreCache[i];
        g_LogFile.verbose("combat", "action ", action->name() ," chance: ", std::exp(score - max_score + 10));
        select.process(action.get(), std::exp(score - max_score + 10));
    }

    // OK, now an action has been selected
    auto action = select.selection();

    // we can execute it
    if(action) {
        g_LogFile.debug("combat", actor.get_name(), " picks action ", action->name());
        action->act(actor, target);
    } else {
        // ERROR -- default to REST action
    }
}

Combat::Combat(ECombatObjective attacker_aim, ECombatObjective defender_aim) :
    m_Attackers(ECombatSide::ATTACKER, attacker_aim), m_Defenders(ECombatSide::DEFENDER, defender_aim)
{
    m_Actions.push_back(std::make_unique<PhysicalAttack>(this));
    m_Actions.push_back(std::make_unique<MagicalAttack>(this));
    m_Actions.push_back(std::make_unique<Rest>(this));
    m_Actions.push_back(std::make_unique<Flee>(this));
    m_Actions.push_back(std::make_unique<Capture>(this));
}

Combatant& Combat::add_combatant(ECombatSide side, unique_ptr<Combatant> combatant) {
    if(side == ECombatSide::ATTACKER) {
        combatant->setup_parties(&m_Attackers, &m_Defenders);
        return m_Attackers.add_member(std::move(combatant));
    } else {
        combatant->setup_parties(&m_Defenders, &m_Attackers);
        return m_Defenders.add_member(std::move(combatant));
    }
}

ECombatResult Combat::run(int max_duration) {
    g_LogFile.info("combat", "Starting Combat");
    for(int r = 0; r < max_duration; ++r) {
        m_Narration.str("");
        bool result = round(r);
        m_RoundSummaries.push_back( m_Narration.str() );
        g_LogFile.debug("combat", m_RoundSummaries.back());
        if(!result) break;
    }

    m_Attackers.finalize();
    m_Defenders.finalize();

    // one last summary
    narration().str("");
    narration() << "     ** RESULT **\n";
    m_Attackers.log_status(narration(), true);
    narration() << "    VS    \n";
    m_Defenders.log_status(narration(), true);
    m_RoundSummaries.push_back( m_Narration.str() );
    g_LogFile.debug("combat", m_RoundSummaries.back());

    g_LogFile.info("combat", "End Combat");

    if(m_Attackers.is_defeated() && !m_Defenders.is_defeated())
        return ECombatResult::DEFEAT;
    else if (m_Defenders.is_defeated() && !m_Attackers.is_defeated())
        return ECombatResult::VICTORY;
    return ECombatResult::DRAW;
}

void Combat::add_combatants(ECombatSide side, sGang& gang, int chance) {
    for(int i = 0; i < gang.members(); ++i) {
        // not all gang members are present for each fight. But the first two are always present.
        if(i > 1 && !rng().percent(chance))
            continue;

        // only some members of the group are mages
        bool mage = rng().percent( std::min(50, 25 + gang.magic() / 2 ) );

        // mages are good at magic, but not so good at combat
        int magic = mage ? gang.magic() : rng() % gang.magic();
        int combat = mage ? (gang.combat() + rng() % gang.combat()) / 2 : gang.combat();
        int constitution = mage ? (gang.constitution() + rng() % gang.constitution()) / 2 : gang.combat();
        int strength = mage ? rng() % gang.strength() : gang.strength();

        auto& cbt = add_combatant(side, std::make_unique<Combatant>( g_BoysNameList.random(),
                rng().in_range(50, 100),
                rng().in_range(magic / 2, magic),
                rng().in_range(5, 20), constitution, combat, magic, gang.agility(),
                strength
                ));
        cbt.set_gang(&gang);
        cbt.set_male();
        switch(gang.weapon_level()) {
        case 3:
            cbt.set_weapon("a sword", 2.0);
            break;
        case 2:
            cbt.set_weapon("an axe", 1.66);
            break;
        case 1:
            cbt.set_weapon("a club", 1.33);
            break;
        }
    }
}

void Combat::add_combatant(ECombatSide side, ICharacter& character) {
    add_combatant(side, std::make_unique<Combatant>(character));
}
