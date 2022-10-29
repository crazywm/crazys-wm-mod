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

#include <CLog.h>
#include "action.h"
#include "combat.h"
#include "combatant.h"
#include "cRng.h"
#include "traits.h"
#include <cmath>

ICombatAction::ICombatAction(Combat* c, const char* n) : m_Combat(c), m_Name(n) {
}

cRng& ICombatAction::rng() const {
    return m_Combat->rng();
}

std::stringstream& ICombatAction::narration() const {
    return m_Combat->narration();
}

double ICombatAction::score(Combatant& self, Combatant& target) const {
    auto res = calc_score(self, target);
    double cost = res.vit_cost;
    if (self.get_vitality() - res.vit_cost < 100) {
        cost += res.vit_cost;
    }

    if (self.get_vitality() < res.vit_cost) {
        return 0;
    }

    // mana cost
    cost += res.mana_cost;
    if (self.get_mana() - res.mana_cost < 100) {
        cost += res.mana_cost;
    }

    if (self.get_mana() < res.mana_cost) {
        return 0;
    }
    double gain = res.goal;
    /// TODO should be combined with smarts!
    // there is no point in attacking a captured enemy if the objective is to capture or escape
    if (target.is_captured() && (self.get_party()->aim == ECombatObjective::CAPTURE || self.get_party()->aim == ECombatObjective::ESCAPE)) {
    } else {
        gain += res.opp_damage * res.chance;
    }

    if( res.opp_damage * 1.2 > target.get_hitpoints()) {
        if(self.get_party()->aim == ECombatObjective::KILL || self.get_party()->aim == ECombatObjective::ESCAPE) {
            // less gain because some damage goes unused, but bonus gain for killing enemy
            double surplus = std::max(res.opp_damage * res.chance - target.get_hitpoints(), 0.0);
            gain -= surplus;
        } else {
            // less gain, we want to capture. But self-preservation comes first
            if(self.get_hitpoints() > 250) {
                if(rng().percent(80 + target.get_smarts() / 5)) {
                    gain = 0;
                }
            } else if (self.get_hitpoints() > 100) {
                // panic sets in, good decisions are more difficult
                if(rng().percent(50 + target.get_smarts() / 2)) {
                    gain = 0;
                }
            }
            gain *= 0.5;

        }
    }

    // add a base cost for "using a turn"
    cost = cost + 20;
    // rescale gain, + some noise probability
    gain = gain * 5 + 5;

    double score = gain / cost + 0.01 * gain;
    g_LogFile.debug_table("combat", "Evalutate action ",
            "action", name(),
            "gain", int(gain),
            "cost", cost,
            "chance", res.chance,
            "dmg", res.opp_damage,
            "score", int(score * 1000)
            );

    return score;
}

void ICombatAction::act(Combatant& self, Combatant& target) const {
    // this marks the messages with the side
    if(self.get_party()->side == ECombatSide::ATTACKER) {
        narration() << "> ";
    } else {
        narration() << "< ";
    }

    do_act(self, target);
    narration() << "\n";
}

// ----------------------------------------------------------------------------

double PhysicalAttack::evade_chance(Combatant& self, Combatant& target) const {
    double attack = (self.get_combat() + self.get_agility()) / 2.0;
    double evade = (target.get_agility() + 0.5) / 2.5;

    // escaped targets have better chance at evading
    if(target.is_fleeing())
        evade = std::min(1.0, evade + 0.25);

    // feeble attack
    if(self.get_vitality() < m_VitalityCost) {
        attack *= 0.25;
    }

    double chance = 0.2*evade + 0.8 * evade * std::min(1.0, 0.9 + 0.1*evade - attack);

    if(target.get_vitality() < 50)
        chance *= 0.5;
    return chance;
}

double PhysicalAttack::parry_chance(Combatant& self, Combatant& target) const {
    double attack = (self.get_combat() + self.get_agility()) / 2.0;
    double parry = (target.get_combat() + 0.5) / 2.5;

    // feeble attack
    if(self.get_vitality() < m_VitalityCost) {
        attack *= 0.25;
    }

    double chance = 0.2*parry + 0.8 * parry * std::min(1.0, 0.9 + 0.1*parry - attack);
    if(target.get_vitality() < 20)
        chance *= 0.5;
    return chance;
}

double PhysicalAttack::expected_damage(Combatant& self, Combatant& target) const {
    double attack = (self.get_combat() + self.get_agility()) / 2.0;
    double base_damage = m_BaseDamageMultiplier - 5 * target.get_combat();
    base_damage *= self.get_weapon_multiplier();
    // make this less strength dependent with weapons
    return base_damage * (1 + attack) * (1 + 2 * self.get_strength()) * target.physical_hurt_multiplier();
}

ActionResult PhysicalAttack::calc_score(Combatant& self, Combatant& target) const {
    double evade = evade_chance(self, target);
    double parry = parry_chance(self, target);
    // two ways to miss: evade and parry. Assume opponent chooses better action.
    double miss_chance = std::max(evade, parry);

    double dmg = expected_damage(self, target);
    int vp = m_VitalityCost;
    // do we need to do the weak version?
    if(self.get_vitality() < 100) {
        vp = 50;
        dmg *= 0.5;
    }

    double crowd = 1.0 - target.crowd_protection();
    double goal = 0;

    // need to run after the target in order to attack
    if(target.is_fleeing()) {
        vp += 50;
        if(self.get_party()->aim == ECombatObjective::CAPTURE) {
            goal += 10;
        }
    }

    if(self.is_fleeing()) {
        goal -= 10;
    }

    return ActionResult{dmg, (1.0 - miss_chance) * crowd, goal, vp, 0};
}

void PhysicalAttack::do_act(Combatant& self, Combatant& target) const {
    double parry = parry_chance(self, target) + rng().in_range(-5, 5) / 100.0;
    double evade = evade_chance(self, target) + rng().in_range(-5, 5) / 100.0;
    bool feeble = false;
    int followed = 0;

    if(self.is_fleeing()) {
        self.prevent_flight();
    }

    g_LogFile.debug("combat", self.get_name(), " performs a ", name(), " with parry=", parry, " and evade=", evade);

    if(target.is_fleeing()) {
        if(self.get_vitality() < 50) return;
        self.use_vitality(50);
        followed = 50;

        // cause the pursuer to spend even more vitality points
        int run_away = rng().in_range( 10 + 25 * target.get_agility(), 20 + 75 * target.get_agility() );
        if(target.get_vitality() > run_away / 2) {
            target.use_vitality(run_away / 2);
            if (self.get_vitality() < run_away * 1.5) {
                narration() << self.get_name() << " wants to attack " << target.get_name()
                << " but " << target.heshe() << " manages to outrun " << self.himher() << " (" << run_away <<  "AP).";
                return;
            }
            self.use_vitality(run_away);
            followed += run_away;
        }
    }

    if(self.get_vitality() < m_VitalityCost) {
        if(self.get_vitality() < m_VitalityCost / 2) {
            return;
        }
        // do a weak attack
        feeble = true;
    }

    int crowd = 100*target.crowd_protection();
    if(rng().percent(crowd)) {
        narration() << self.get_name() << " wants to attack " << target.get_name()
                    << " but cannot get a clear shot through the crowd (" << crowd << "%).";
        return;
    }

    bool success = false;

    self.use_vitality( feeble ? m_VitalityCost / 2 : m_VitalityCost );
    self.mark_attack();
    // TODO include weapons + variety in text.
    if(feeble) {
        narration() << self.get_name() << " is exhausted, but " << (followed ? "follows and ": "") << "attacks "
                    << target.get_name() << " (" << target.get_hitpoints() << " HP)" << " with "
                    << self.get_weapon_name() << ". " << target.get_name();

    } else {
        if(followed) {
            narration() << self.get_name() << " runs (" << followed << "AP) after " << target.get_name() << " (" << target.get_hitpoints() << " HP)"
                        << " and attacks " << target.himher() << " with " << self.get_weapon_name() << ". " << target.get_name();

        } else {
            narration() << self.get_name() << " attacks " << target.get_name() << " (" << target.get_hitpoints() << " HP)"
                        << " with " << self.get_weapon_name() << ", who";
        }
    }

    target.inc_crowding();
    int dev = (100 - target.get_smarts()) / 10;
    bool should_parry = 100 * parry + rng().in_range(0, dev) > 100 * evade + rng().in_range(0, dev);

    // surrender?
    if(self.get_party()->aim == ECombatObjective::CAPTURE) {
        double surrender = 1 - (should_parry ? parry : evade);
        // TODO modulate this based on the targets `SPIRIT` stat
        if(rng().percent(100 * surrender ) && expected_damage(self, target) > target.get_hitpoints()) {
            narration() << " surrenders.";
            target.capture();
            return;
        }
    }

    if(should_parry) {
        // opponent tries to parry
        target.use_vitality(10);
        if(rng().percent(100 * parry)) {
            narration() <<  " managed to block (" << int(100 * parry) << "%) the attack.";
            if(followed && rng().percent(50) ){
                narration() << " It sent " << target.himher() << " staggering, though, and allowed the others to catch up.";
                target.prevent_flight();
            }
        } else {
            narration() << " tried to block (" << int(100 * parry) << "%) the attack.";
            success = true;
        }
    } else {
        // opponent tries to evade
        target.use_vitality(25);
        if(rng().percent(100 * evade)) {
            narration() <<  " managed to evade (" << int(100 * evade) << "%) the attack.";
        } else {
            narration() << " tried to evade (" << int(100 * evade) << "%) the attack.";
            success = true;
        }
    }

    if(success) {
        // damage target
        int damage = expected_damage(self, target);
        if(feeble)
            damage *= 0.5;

        // critical hit chance -- increases if target is sluggish
        double crit_chance = 5 + self.get_trait_modifier(traits::modifiers::COMBAT_MELEE_CRIT);
        if(target.get_vitality() < 100)
            crit_chance += (100 - target.get_vitality()) / 10.0;

        if(rng().percent(crit_chance)) {
            damage *= 2;
            narration() << " Critical Hit!";
        }

        damage = rng().in_range(0.5*damage, 1.5*damage);

        if(!target.hurt(damage, self)) {
            narration() << " " << target.get_name() << " takes " << damage << " damage.";
            if(followed) {
                narration() << " The others manage catch up.";
                target.prevent_flight();
            }
        } else {
            narration() << " " << target.get_name() << " takes " << damage << " damage and falls unconscious.";
        }
    }
}

PhysicalAttack::PhysicalAttack(Combat* c) : ICombatAction(c, "PhysicalAttack") {

    m_BaseDamageMultiplier = 20;
    m_VitalityCost = 100;
}

// ----------------------------------------------------------------------------

double MagicalAttack::evade_chance(Combatant& self, Combatant& target) const {
    if(target.get_vitality() < 25) return 0;

    double attack = (self.get_magic() + 0.5 * self.get_combat()) / 1.5;
    double evade = (target.get_agility() + 0.5) / 2.5;

    double chance = 0.2*evade + 0.8 * evade * std::min(1.0, 0.9 + 0.1*evade - attack);

    // for evaded targets, hit probability is halved
    if(target.is_fleeing()) {
        return 1.0 - (1.0 - chance) / 2;
    }

    return chance;
}

double MagicalAttack::deflect(Combatant& self, Combatant& target) const {
    if(target.get_mana() < 20) return 0;

    double attack = (self.get_magic() + self.get_combat() * 0.5) / 1.5;
    double parry = (target.get_magic() + 0.5) / 2.5;

    return 0.2*parry + 0.8 * parry * std::min(1.0, 0.9 + 0.1*parry - attack);
}

double MagicalAttack::expected_damage(Combatant& self, Combatant& target) const {
    double attack = (self.get_magic() + (1 - target.get_magic())) / 2.0;
    double base_dmg = 50;
    if(self.get_mana() > 300) {
        base_dmg = 70;
    }
    return base_dmg * (1 + 2 * attack) * (1 + 2*self.get_magic()) * target.magical_hurt_multiplier();
}

ActionResult MagicalAttack::calc_score(Combatant& self, Combatant& target) const {
    // two ways to miss: evade and parry. Assume opponent chooses better action.
    double miss_chance = std::max(evade_chance(self, target), deflect(self, target));

    double dmg = expected_damage(self, target);
    double chance = spell_chance(self) * (1.0 - miss_chance) * (1.0 - target.crowd_protection());
    return ActionResult{dmg, chance, 0, 20, 100};
}

void MagicalAttack::do_act(Combatant& self, Combatant& target) const {
    if(self.get_mana() < 100) return;

    int crowd = 100*target.crowd_protection();
    if(rng().percent(crowd)) {
        narration() << self.get_name() << " wants to attack " << target.get_name()
                    << " but cannot get a clear shot through the crowd (" << crowd << "%).";
        return;
    }

    double parry = deflect(self, target) + rng().in_range(-5, 5) / 100.0;
    double evade = std::max(0.0, evade_chance(self, target) + rng().in_range(-5, 5) / 100.0);
    bool smart = rng().percent(target.get_smarts());
    bool success = false;

    self.use_vitality(20);
    if(self.get_mana() > 300) {
        // use a big spell!
        self.use_mana(150);
    } else {
        self.use_mana(100);
    }
    self.mark_attack();
    target.inc_crowding();

    double spell = spell_chance(self);
    if(!rng().percent(100 * spell)) {
        narration() << self.get_name() << " tried to attack " << target.get_name() << " with magic, but the spell failed";
        return;
    }

    // TODO include weapons + variety in text.
    narration() << self.get_name() << " attacks " << target.get_name() << " (" << target.get_hitpoints() << " HP)" << " with magic, who";

    bool should_parry = parry > evade;
    if((should_parry && smart) || (!should_parry && !smart)) {
        // opponent tries to parry
        target.use_mana(20);
        if(rng().percent(100 * parry)) {
            narration() <<  " managed to deflect (" << int(100 * parry) << "%) the attack.";
        } else {
            narration() << " tried to deflect (" << int(100 * parry) << "%) the attack.";
            success = true;
        }
    } else {
        // opponent tries to evade
        target.use_vitality(25);
        if(rng().percent(100 * evade)) {
            narration() <<  " managed to evade (" << int(100 * evade) << "%) the attack.";
        } else {
            narration() << " tried to evade (" << int(100 * evade) << "%) the attack. ";
            success = true;
        }
    }

    if(success) {
        // damage target
        int damage = expected_damage(self, target);
        damage = rng().in_range(0.5*damage, 1.5*damage);
        if(!target.hurt(damage, self)) {
            narration() << " " << target.get_name() << " takes " << damage << " damage.";
        } else {
            narration() << " " << target.get_name() << " takes " << damage << " damage and is dead.";
        }
    }
}

MagicalAttack::MagicalAttack(Combat* c) : ICombatAction(c, "MagicalAttack") {

}

double MagicalAttack::spell_chance(const Combatant& self) const {
    // TODO use confidence; intelligence; magic combination
    double fail = std::min(self.get_magic(), 1.0);
    return std::max(0.25, 0.75 - fail * 0.5);
}

// ---------------------------------------------------------------------------------------------------------------------

Rest::Rest(Combat* c) : ICombatAction(c, "Rest") {

}

ActionResult Rest::calc_score(Combatant& self, Combatant& target) const {
    double goal = 5;
    if(self.get_vitality() < 100) {
        goal = 15;
    } else if(self.get_vitality() < 200) {
        goal = 10;
    }
    if(self.get_mana() + 10 < self.get_max_mana()) {
        goal += 5;
    }

    if(target.is_fleeing()) {
        goal -= 5;
    }

    goal += self.get_trait_modifier(traits::modifiers::COMBAT_REST);

    return ActionResult{0, 0.0, goal, 0,0};
}

void Rest::do_act(Combatant& self, Combatant& target) const {
    narration() << self.get_name() << " catches " << self.hisher() << " breath.";

    // a resting round restores like three fighting rounds.
    self.end_round();
    self.end_round();
    self.change_initiative(10);
}

// ---------------------------------------------------------------------------------------------------------------------

Capture::Capture(Combat* c) : ICombatAction(c, "Capture") {

}

ActionResult Capture::calc_score(Combatant& self, Combatant& target) const {
    if(target.is_captured())
        return ActionResult{0, 0, 0, 50, 0};

    double throw_chance = net_throw_chance(self);
    bool wants_capture = self.get_party()->aim == ECombatObjective::CAPTURE;

    double goal = wants_capture ? 100 : 5;
    double target_vita_cost = throw_chance * (wants_capture ? 10 : 5);
    double chance = net_catch_chance(self, target) * (1.0 - target.crowd_protection());
    return ActionResult{0, chance, goal * chance + target_vita_cost, 50, 0};
}

void Capture::do_act(Combatant& self, Combatant& target) const {
    if(self.get_vitality() < 50) return;
    if(target.is_captured()) return;

    int crowd = 100*target.crowd_protection();
    if(rng().percent(crowd)) {
        narration() << self.get_name() << " wants to catch " << target.get_name()
                    << " but cannot get a clear shot through the crowd (" << crowd << "%).";
        return;
    }

    double cap_chance = 100 * net_throw_chance(self);
    narration() << self.get_name() << " tried to capture (" << int(cap_chance) <<  "%) " << target.get_name() << ".";
    // bungle the throw?

    self.use_vitality(50);
    self.mark_attack();
    target.inc_crowding();

    if(!rng().percent(cap_chance)) {
        narration() << " However, the net flies wide, not catching " << target.get_name() << ".";
    } else {
        target.use_vitality(50);
        // TODO magic defence / cut net
        double escape = 100 * net_escape_chance(target);
        if(rng().percent(escape)) {
            narration() << " At the last moment, " << target.heshe() << " ducks (" << int(escape) << "%) under the incoming net.";
        } else {
            narration() << " " << target.heshe() << " tries to evade (" << int(escape) << "%) the net, but it entangles "
                        << target.hisher() << " limbs and immobilizing " << target.himher() << ".";
            target.capture();
        }
    }
}

double Capture::net_catch_chance(const Combatant& self, const Combatant& target) const
{
    double good_throw = net_throw_chance(self);
    double escape = net_escape_chance(target);
    return good_throw * (1.0 - escape);
}

double Capture::net_escape_chance(const Combatant& target) const {
    double lvf = 1.0;
    if(target.get_vitality() < 50) lvf = target.get_vitality() / 50.0;
    double vit = target.get_vitality() / 1000.0;

    return std::max(0.1, lvf * std::min(0.9, (0.25 + target.get_agility() * (1.0 + vit))));
}

double Capture::net_throw_chance(const Combatant& self) const {
    return 0.4 + 0.6 * self.get_combat();;
}

// ---------------------------------------------------------------------------------------------------------------------

Flee::Flee(Combat* c) : ICombatAction(c, "Flee") {

}

ActionResult Flee::calc_score(Combatant& self, Combatant& target) const {
    double goal = 0;
    if(self.get_party()->aim == ECombatObjective::ESCAPE) {
        goal = 50;
    }

    if(self.get_hitpoints() < 200)
        goal += 25;

    if(self.get_hitpoints() < 100)
        goal += 25;

    // run away if strongly outnumbers
    if(self.get_party()->active_members.size() * 2 + 1 < target.get_party()->active_members.size()) {
        goal += 33;
    }

    // don't stop now
    if(self.is_fleeing())
        goal *= 1.2;

    goal += self.get_trait_modifier(traits::modifiers::COMBAT_RETREAT);

    return ActionResult{0, 1.0, goal, 10,0};
}

void Flee::do_act(Combatant& self, Combatant& target) const {
    narration() << self.get_name() << " runs away.";
    self.use_vitality(10);
    self.flee();
    self.change_initiative(-25);
}
