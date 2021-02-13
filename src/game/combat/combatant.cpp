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

#include <iomanip>
#include <cmath>
#include <cGirls.h>
#include <cGangs.h>
#include "combatant.h"
#include "combat.h"
#include "character/ICharacter.h"
#include "IGame.h"
#include "character/sGirl.h" // TODO only needed to determine the pronoun
#include "traits.h"
#include "cInventory.h"
#include "Inventory.h"


int Combatant::get_initiative() const {
    int base = 200 * get_agility() - tiredness;
    return std::max(10, std::min(base, 100) + initiative);
}

void Combatant::log_status(std::ostream& target, bool brief) const {
    std::string status = "";
    if(is_dead())
        status = "[unc]";
    else if (is_captured())
        status = "[capt]";
    else if (is_escaped())
        status = "[esc]";
    else if (is_fleeing())
        status = "[flee]";
    target << std::setw(20) << std::left << name
            << std::setw(5) << status
           << " HP: " << std::setw(4) << hitpoints;
    if(!brief) {
        target  << " MP: " << std::setw(4) << mana
                << " AP: " << std::setw(4) << vitality;
    }
}

void Combatant::setup_parties(Party* a, Party* e) {
    allies = a;
    enemies = e;
}

double Combatant::get_agility() const {
    if(is_captured()) return 0.0;
    return agility / 100.0 * (1000 + vitality) / 2000.0;
}

double Combatant::get_combat() const {
    return combat / 100.0 * (1000 + vitality) / 2000.0;
}

double Combatant::get_strength() const {
    return strength / 100.0 * (500 + vitality) / 1500.0;
}

double Combatant::get_magic() const {
    return magic / 100.0 * (2000 + vitality) / 3000.0;
}


Combatant::Combatant(std::string n, int HP, int MP, int tiredness_, int constitution_, int combat_, int magic_,
                     int agility_, int strength_) :
                     name(std::move(n)), hitpoints(500 + (5 * HP * constitution_) / 100 ), mana(10*MP), max_mana(10*MP),
                     vitality(std::min(1000, 500 - 10 * tiredness_ + 10 * constitution_)),
                     constitution(constitution_), agility(agility_), combat(combat_),
                     strength(strength_), magic(magic_), tiredness(tiredness_),
                     initial_hp(hitpoints)
                     {

}

bool Combatant::hurt(int amount, const Combatant& source) {
    hitpoints = std::max(0, hitpoints - amount);
    // TODO use combat RNG here
    vitality = std::max(0, vitality - g_Dice % amount);
    vitality_used -= amount;

    if(hitpoints == 0) {
        status_is_dead = true;
        allies->notify_end_combat(this);
        return true;
    }
    return false;
}

void Combatant::end_round() {
    vitality = std::max(0, vitality + constitution / 2 + 10);
    hitpoints += constitution / 10;
    mana = std::min(mana + magic / 2, max_mana);
    crowding = 0;
}

int Combatant::get_smarts() const {
    return combat;      // TODO + INT
}

void Combatant::use_vitality(int amount) {
    vitality -= amount;
    vitality_used += amount;
    if(vitality < 0) vitality = 0;
}

void Combatant::use_mana(int amount) {
    mana -= amount;
    max_mana -= amount / 3;

    if(mana < 0) mana = 0;
    if(max_mana < 0) max_mana = 0;
}

void Combatant::mark_attack() {
    allies->has_attacked = true;
}

Combatant::Combatant(ICharacter& character) :
    Combatant(character.FullName(), character.health(), character.mana(), character.tiredness(),
              character.constitution(), character.combat(), character.magic(), character.agility(), character.strength())
{
    character_src = &character;
    if(dynamic_cast<sGirl*>(&character) != nullptr) {
        set_female();
    }

    for(auto& entry : character.inventory().all_items()) {
        auto item = entry.first;
        if (item->m_Type == sInventoryItem::Weapon && entry.second.equipped) {
            // found an equipped weapon -- damage is currently only based on weapon cost.
            double dmg = 1 + std::min(1.0, std::sqrt(item->m_Cost / 1000.0));
            set_weapon(item->m_Name, dmg);
        }
    }
}

void Combatant::finalize() {
    if(character_src) {
        // re-sync character_src stats

        // this is: 0, if hitpoints are unchanged, and health if hitpoints are 0
        int damage = (initial_hp - hitpoints) * character_src->health() / initial_hp;
        int new_health = character_src->health() - damage;
        // miraculous survival
        if(new_health <= 0 && g_Dice.percent(50)) {
            new_health = 1;
        }

        character_src->set_stat(STAT_HEALTH, new_health);

        // second: mana
        character_src->set_stat(STAT_MANA, max_mana / 10);

        // third: tiredness
        character_src->tiredness((10*vitality_used) / (constitution + 100));
    } else if (gang_src) {
        // cannot regain captured or escaped gang members
        if(hitpoints <= 0 || is_dead() || is_captured() || (is_escaped() && g_Dice.percent(50))) {
            gang_src->m_Num -= 1;
        } else {
            // only invest in those still with us
            // 250 per health potion == 10G
            int dmg_points = initial_hp - hitpoints;
            int heal_cost = (10 * dmg_points) / 250;
            gang_src->m_MedicalCost += heal_cost;
        }
    }
}

void Combatant::capture() {
    status_is_captured = true;
    vitality = std::max(0, vitality - 100);
    allies->notify_end_combat(this);
}

void Combatant::inc_crowding() {
    ++crowding;
}

double Combatant::crowd_protection() const {
    switch(crowding) {
        case 0:
            return 0.0;
        case 1:
            return 0.05;
        case 2:
            return 0.25;
        case 3:
            return 0.50;
        case 4:
            return 0.75;
        default:
            return 0.90;
    }
}

void Combatant::flee() {
    if(status_escaped == EEscapeStatus::FIGHTING) {
        status_escaped = EEscapeStatus::FLEEING;
    } else if (status_escaped == EEscapeStatus::FLEEING) {
        status_escaped = EEscapeStatus::ESCAPED;
    }
    crowding = 0;
}

void Combatant::prevent_flight() {
    status_escaped = EEscapeStatus::FIGHTING;
}

void Combatant::begin_round() {
    initiative = 0;
    if(get_party()->aim == ECombatObjective::ESCAPE)
        initiative += 10;
}

void Combatant::change_initiative(int amount) {
    initiative += amount;
}

bool Combatant::consume_health_potion() {
    if(gang_src) {
        if(gang_src->num_potions() > 0) {
            gang_src->use_potion();
            hitpoints += 250;
            vitality += 50;
            return true;
        }
        return false;
    } else if (character_src) {
        // TODO this is a temporary solution! Introduce real health potions.
        auto hs = g_Game->inventory_manager().GetItem("Healing Salve (S)");
        if(character_src->remove_item(hs) != 0) {
            hitpoints += 250;
            vitality += 50;
            return true;
        };
        return false;
    }
    return false;
}

void Combatant::set_gang(sGang* gang) {
    assert(gang_src == nullptr);
    assert(character_src == nullptr);
    gang_src = gang;
    set_male();
}

void Combatant::set_male() {
    he_she = "he";
    his_her = "his";
    him_her = "him";
}

void Combatant::set_female() {
    he_she = "she";
    his_her = "her";
    him_her = "her";
}

void Combatant::set_weapon(std::string name, double strength) {
    weapon = std::move(name);
    weapon_strength = strength;
}

bool Combatant::has_trait(const char* trait) const {
    if(character_src)
        return character_src->has_active_trait(trait);
    return false;
}

double Combatant::physical_hurt_multiplier() const {
    if(!character_src)  return 1.0;

    int trait_influence = character_src->get_trait_modifier(ct::PHYS_DMG_MOD);
    return std::pow(0.95, trait_influence);
}

double Combatant::magical_hurt_multiplier() const {
    if(!character_src)  return 1.0;

    int trait_influence = character_src->get_trait_modifier(ct::MAGIC_DMG_MOD);
    return std::pow(0.95, trait_influence);
}

int Combatant::get_trait_modifier(const char* trait) const {
    if(character_src)
        return character_src->get_trait_modifier(trait);
    return 0;
}
