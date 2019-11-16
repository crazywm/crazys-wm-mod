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

#ifndef WM_COMBATANT_H
#define WM_COMBATANT_H

#include <string>

class Party;
class ICharacter;

enum class EEscapeStatus {
    FIGHTING,
    FLEEING,
    ESCAPED
};

struct Combatant {
public:
    explicit Combatant(ICharacter& character);
    Combatant(std::string name, int health, int mana, int tiredness_, int constitution_, int combat_, int magic_,
              int agility_, int strength_);

    const std::string& get_name() const { return name; }
    const std::string& get_weapon_name() const { return weapon; }
    double get_weapon_multiplier() const { return weapon_strength; }

    // dumb getters
    int get_vitality() const { return vitality; }
    int get_hitpoints() const { return hitpoints; }
    int get_max_hp() const { return initial_hp; }
    int get_mana() const { return mana; }
    int get_max_mana() const { return max_mana; }

    bool is_dead() const { return status_is_dead; }
    bool is_captured() const { return status_is_captured; }
    bool is_fleeing() const { return status_escaped != EEscapeStatus::FIGHTING; }
    bool is_escaped() const { return status_escaped == EEscapeStatus::ESCAPED; }

    const Party* get_party() const { return allies; }
    const Party* get_enemies() const { return enemies; }

    int get_initiative() const;

    bool can_act() const { return !is_dead() &&!is_captured() && !is_escaped(); }

    // there return in range [0, 1]
    double get_agility() const;
    double get_combat() const;
    double get_strength() const;
    double get_magic() const;
    double crowd_protection() const;

    bool has_trait(const char* trait) const;
    int get_trait_modifier(const char* trait) const;
    /// multiplier for the physical damage RECEIVED by this combatant
    double physical_hurt_multiplier() const;
    double magical_hurt_multiplier() const;

    int get_smarts() const;     // returns the chance of doing the 'correct' thing in combat

    // applies damage to this combatant. This decreases hitpoint and vitality. Returns true if the target is dead.
    bool hurt(int amount, const Combatant& source);
    void capture();
    void use_vitality(int amount);
    void use_mana(int amount);
    bool consume_health_potion();

    void change_initiative(int amount);
    /// this is called after the move order has been determined, but before any actions happen
    void begin_round();
    /// this is called after all actions and recovery
    void end_round();
    void mark_attack();
    void inc_crowding();
    void flee();
    void prevent_flight();

    void log_status(std::ostream& target, bool brief=false) const;

    void setup_parties(Party* allies, Party* enemies);
    void finalize();

    void set_gang(sGang* gang);
    void set_weapon(std::string name, double strength);

    // setup pronouns
    void set_male();
    void set_female();

    const char* heshe() const { return he_she; }
    const char* himher() const { return him_her; }
    const char* hisher() const { return his_her; }

private:
    std::string name;           // name used in messages
    std::string weapon  = "fists";
    double weapon_strength = 1.0;
    const char* he_she  = "he/she";
    const char* him_her = "him/her";
    const char* his_her = "his/her";

    Party* allies;
    Party* enemies;

    // dynamic values
    int hitpoints;              // health, range from [0, 1000]
    int mana;                   // mana, range from [0, 1000]
    int max_mana;               // the maximum amount of mana that can regenerate
    int vitality;               // action points, range from [0, 1000]

    int initiative = 0;
    int crowding = 0;

    // statues
    bool status_is_dead = false;
    bool status_is_captured = false;
    EEscapeStatus status_escaped = EEscapeStatus::FIGHTING;

    // static values inherited from pre-battle stats
    int constitution;
    int agility;
    int combat;
    int strength;
    int magic;
    int tiredness;

    // stats tracking
    int initial_hp;
    int vitality_used = 0;

    // source of this combatant
    ICharacter* character_src = nullptr;
    sGang* gang_src           = nullptr;
};

inline std::ostream& operator<<(std::ostream& stream, const Combatant& cbt) {
    cbt.log_status(stream);
    return stream;
}


#endif //WM_COMBATANT_H
