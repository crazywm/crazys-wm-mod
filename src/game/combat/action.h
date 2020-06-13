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

#ifndef WM_ACTION_H
#define WM_ACTION_H

#include <sstream>

class Combatant;
class Combat;
class cRng;

struct ActionResult {
    // damage dealt to opponent
    double opp_damage;
    double chance;
    double goal;

    // vitality used
    int vit_cost;
    // mana used
    int mana_cost;
};

class ICombatAction {
public:
    ICombatAction(Combat*, const char* name);
    virtual ~ICombatAction() = default;

    const char* name() const { return m_Name; }
    void act(Combatant& self, Combatant& target) const;
    double score(Combatant& self, Combatant& target) const;

protected:
    cRng& rng() const;
    std::stringstream& narration() const;


private:
    virtual void do_act(Combatant& self, Combatant& target) const = 0;
    virtual ActionResult calc_score(Combatant& self, Combatant& target) const = 0;

    Combat* m_Combat;
    const char* m_Name;
};

class PhysicalAttack : public ICombatAction {
public:
    explicit PhysicalAttack(Combat* c);
    ActionResult calc_score(Combatant& self, Combatant& target) const override;
    void do_act(Combatant& self, Combatant& target) const override;

    double evade_chance(Combatant& self, Combatant& target) const;
    double parry_chance(Combatant& self, Combatant& target) const;
    double expected_damage(Combatant& self, Combatant& target) const;

private:
    double m_BaseDamageMultiplier;
    int    m_VitalityCost;
};

class MagicalAttack : public ICombatAction {
public:
    explicit MagicalAttack(Combat* c);
    ActionResult calc_score(Combatant& self, Combatant& target) const override;
    void do_act(Combatant& self, Combatant& target) const override;

    double evade_chance(Combatant& self, Combatant& target) const;
    double deflect(Combatant& self, Combatant& target) const;
    double expected_damage(Combatant& self, Combatant& target) const;
    double spell_chance(const Combatant& self) const;
};

class Rest : public ICombatAction {
public:
    explicit Rest(Combat* c);
    ActionResult calc_score(Combatant& self, Combatant& target) const override;
    void do_act(Combatant& self, Combatant& target) const override;
};

class Capture : public ICombatAction {
public:
    explicit Capture(Combat* c);
    ActionResult calc_score(Combatant& self, Combatant& target) const override;
    void do_act(Combatant& self, Combatant& target) const override;

    double net_catch_chance(const Combatant& self, const Combatant& target) const;
    double net_escape_chance(const Combatant& target) const;
    double net_throw_chance(const Combatant& self) const;
};

class Flee : public ICombatAction {
public:
    explicit Flee(Combat* c);
    ActionResult calc_score(Combatant& self, Combatant& target) const override;
    void do_act(Combatant& self, Combatant& target) const override;
};

#endif //WM_ACTION_H
