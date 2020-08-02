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

#include "Game.hpp"
#include "CLog.h"
#include "cInventory.h"
#include "XmlMisc.h"
#include "character/cPlayer.h"
#include "ICharacter.h"
#include <tinyxml2.h>
#include <cmath>
#include <cRng.h>
#include "xml/util.h"
#include "xml/getattr.h"
#include "traits/ITraitsCollection.h"
#include "Inventory.h"

extern cRng g_Dice;

ICharacter::ICharacter(std::unique_ptr<ITraitsCollection> tc, bool unique) :
    m_Inventory(std::make_unique<Inventory>()),
    m_Traits( std::move(tc) ),
    m_IsUnique(unique)
{
    // default-initialize stats and skills
    m_Stats.fill(sAttributeValue{});
    m_Skills.fill(sAttributeValue{});
}

ICharacter::~ICharacter() = default;
ICharacter::ICharacter(ICharacter&&) noexcept = default;


int ICharacter::get_stat(int stat_id) const {
    int min = get_all_stats()[stat_id].min;
    int max = get_all_stats()[stat_id].max;
    // Generic calculation
    int value = m_Stats[stat_id].m_Value + m_Stats[stat_id].m_PermanentMods + m_Stats[stat_id].m_TempMods;
    value += m_Traits->stat_effects()[stat_id];

    if (value < min) value = min;
    else if (value > max) value = max;
    return value;
}

void ICharacter::set_stat(int stat_id, int amount)
{
    int min = get_all_stats()[stat_id].min;
    int max = get_all_stats()[stat_id].max;

    if (amount > max) amount = max;
    else if (amount < min) amount = min;
    m_Stats[stat_id].m_Value = amount;
}

int ICharacter::upd_base_stat(int stat, int amount, bool usetraits)
{
    set_stat(stat, get_base_stat(stat) + amount);
    return get_stat(stat);
}

void ICharacter::upd_mod_stat(int stat, int amount)
{
    m_Stats[stat].m_TempMods += amount;
}

void ICharacter::upd_temp_stat(int stat_id, int amount, bool usetraits)
{
    if(!get_all_stats()[stat_id].permanent) {
        // TODO ERROR?
        upd_base_stat(stat_id, amount);
        return;
    }
    m_Stats[stat_id].m_TempMods += amount;
}

int ICharacter::get_skill(int skill_id) const
{
    int min = get_all_skills()[skill_id].min;
    int max = get_all_skills()[skill_id].max;
    // Generic calculation
    auto& data = m_Skills[skill_id];
    int value = data.m_Value + data.m_PermanentMods + data.m_TempMods;
    value += m_Traits->skill_effects()[skill_id];

    if (value < min) value = min;
    else if (value > max) value = max;
    return value;
}

void ICharacter::set_skill(int skill_id, int amount)
{
    int min = get_all_skills()[skill_id].min;
    int max = get_all_skills()[skill_id].max;

    if (amount > max) amount = max;
    else if (amount < min) amount = min;
    m_Skills[skill_id].m_Value = amount;
}

int ICharacter::upd_skill(int skill_id, int amount, bool usetraits)
{
    set_skill(skill_id, get_base_skill(skill_id) + amount);
    return get_skill(skill_id);
}


void ICharacter::SaveXML(tinyxml2::XMLElement& elRoot) const
{
    elRoot.SetAttribute("Unique", m_IsUnique);

    // stats
    auto& stats_el = PushNewElement(elRoot, "Stats");
    for (int i = 0; i < NUM_STATS; i++)
    {
        auto& el = PushNewElement(stats_el, "Stat");
        el.SetAttribute("Name", get_stat_name(STATS(i)));
        el.SetAttribute("Value", m_Stats[i].m_Value);
        if (m_Stats[i].m_TempMods)    el.SetAttribute("Temp", m_Stats[i].m_TempMods);
    }

    // skills
    for (int i = 0; i < NUM_SKILLS; i++)
    {
        auto& el = PushNewElement(stats_el, "Skill");
        el.SetAttribute("Name", get_skill_name(SKILLS(i)));
        el.SetAttribute("Value", m_Skills[i].m_Value);
        if (m_Skills[i].m_TempMods)    el.SetAttribute("Temp", m_Skills[i].m_TempMods);
    }

    // other data
    auto& name_el = PushNewElement(elRoot, "Name");
    name_el.SetAttribute("Full", m_FullName.c_str());
    if(!m_FirstName.empty())  name_el.SetAttribute("First", m_FirstName.c_str());
    if(!m_MiddleName.empty())  name_el.SetAttribute("Middle", m_MiddleName.c_str());
    if(!m_Surname.empty())  name_el.SetAttribute("Last", m_Surname.c_str());

    auto& bd_el = PushNewElement(elRoot, "BirthDay");
    bd_el.SetAttribute("Year", m_BirthYear);
    bd_el.SetAttribute("Month", m_BirthMonth);
    bd_el.SetAttribute("Day", m_BirthDay);

    m_Traits->save_to_xml(PushNewElement(elRoot, "Traits"));
    m_Inventory->save_to_xml(PushNewElement(elRoot, "Inventory"));
}

void ICharacter::LoadXML(const tinyxml2::XMLElement& elRoot)
{
    elRoot.QueryAttribute("Unique", &m_IsUnique);

    // load name
    auto name_el = elRoot.FirstChildElement("Name");
    if(!name_el) {
        g_LogFile.error("game", "Character (l. ", elRoot.GetLineNum(), ") does not have a name attribute. ");
    } else {
        m_FullName = GetDefaultedStringAttribute(*name_el, "Full", "");
        m_FirstName = GetDefaultedStringAttribute(*name_el, "First", "");
        m_MiddleName = GetDefaultedStringAttribute(*name_el, "Middle", "");
        m_Surname = GetDefaultedStringAttribute(*name_el, "Last", "");
    }

    auto bd_el = elRoot.FirstChildElement("BirthDay");
    bd_el->QueryIntAttribute("Year", &m_BirthYear);
    bd_el->QueryIntAttribute("Month", &m_BirthMonth);
    bd_el->QueryIntAttribute("Day", &m_BirthDay);

    auto root = elRoot.FirstChildElement("Stats");
    for(const auto& el : IterateChildElements(*root, "Stat")) {
        const char* name = el.Attribute("Name");
        if(!name) {
            g_LogFile.error("game", "'Stat' tag does not contain 'Name' attribute. Character: ", m_FullName);
            continue;
        }
        auto stat_id = get_stat_id(name);
        m_Stats[stat_id].m_TempMods = el.IntAttribute("Temp", 0);
        set_stat(stat_id, GetIntAttribute(el, "Value"));
    }

    for(const auto& el : IterateChildElements(*root, "Skill")) {
        const char* name = el.Attribute("Name");
        if(!name) {
            g_LogFile.error("game", "'Skill' tag does not contain 'Name' attribute. Character: ", m_FullName);
            continue;
        }
        auto stat_id = get_skill_id(name);
        m_Skills[stat_id].m_TempMods = el.IntAttribute("Temp", 0);
        set_skill(stat_id, GetIntAttribute(el, "Value"));
    }

    auto trt = elRoot.FirstChildElement("Traits");
    if(trt) {
        m_Traits->load_from_xml(*trt);
        m_Traits->update();
    }

    auto inv = elRoot.FirstChildElement("Inventory");
    if(inv)
        m_Inventory->load_from_xml(*inv, this);
}

void ICharacter::DecayTemp()
{
    for(auto& stat : m_Stats) {
        if(stat.m_TempMods != 0) {
            int change = (int)(std::ceil(float(stat.m_TempMods) * 0.3f));
            stat.m_TempMods -= change;
        }
    }

    for(auto& stat : m_Skills) {
        if(stat.m_TempMods != 0) {
            int change = (int)(std::ceil(float(stat.m_TempMods) * 0.3f));
            stat.m_TempMods -= change;
        }
    }

    m_Traits->tick();
}

void ICharacter::upd_temp_skill(int stat, int amount)
{
    m_Skills[stat].m_TempMods += amount;
}

void ICharacter::upd_mod_skill(int stat, int amount)
{
    m_Skills[stat].m_PermanentMods += amount;
}

int ICharacter::SetBirthYear(int n)
{
    m_BirthYear = m_BirthYear + n;
    return m_BirthYear;
}

int ICharacter::SetBirthMonth(int n)
{
    m_BirthMonth = n;
    if (m_BirthMonth > 12)        m_BirthMonth = 12;
    if (m_BirthMonth < 1)        m_BirthMonth = 1;
    return m_BirthMonth;
}

int ICharacter::SetBirthDay(int n)
{
    m_BirthDay = n;
    if (m_BirthDay > 30)    m_BirthDay = 30;
    if (m_BirthDay < 1)        m_BirthDay = 1;
    return m_BirthDay;
}

void ICharacter::SetName(std::string first, std::string middle, std::string last)
{
    m_FirstName = std::move(first);
    m_MiddleName = std::move(middle);
    m_Surname = std::move(last);

    m_FullName.reserve(m_FirstName.size() + m_MiddleName.size() + m_Surname.size() + 2);
    m_FullName = m_FirstName;
    if(!m_MiddleName.empty()) {
        m_FullName.append(1, ' ');
        m_FullName.append(m_MiddleName);
    }
    m_FullName.append(1, ' ');
    m_FullName.append(m_Surname);
}

void ICharacter::SetSurname(std::string surname)
{
    SetName(std::move(m_FirstName), std::move(m_MiddleName), std::move(surname));
}

Inventory& ICharacter::inventory()
{
    return *m_Inventory;
}

const Inventory& ICharacter::inventory() const
{
    return *m_Inventory;
}

bool ICharacter::has_item(const sInventoryItem* item, int amount) const
{
    return inventory().has_item(item, amount);
}

int ICharacter::remove_item(const sInventoryItem* item, int amount)
{
    // can we remove without unequipping?
    auto num_removed = inventory().remove_item(item, amount);
    if(num_removed == amount) {
        return num_removed;
    }

    amount -= num_removed;
    // try to unequip as many as possible/necessary
    while(amount > 0) {
        if(!unequip(item))
            break;
        --amount;
    }
    num_removed += inventory().remove_item(item, amount);
    return num_removed;
}

bool ICharacter::unequip(const sInventoryItem* item)
{
    return false;
}

bool ICharacter::add_item(const sInventoryItem* item, int amount)
{
    int added = m_Inventory->add_item(item, amount);
    return added == amount;
}

const sInventoryItem* ICharacter::has_item(const std::string& name, int amount) const
{
    auto item = g_Game->inventory_manager().GetItem(name);
    if(m_Inventory->has_item(item, amount)) {
        return item;
    }
    return nullptr;
}

bool ICharacter::has_active_trait(const char *trait_name) const {
    return m_Traits->has_active_trait(trait_name);
}

bool ICharacter::gain_trait(const char *trait_name, float chance) {
    if(chance < 100) {
        if(!g_Dice.percent(chance)) {
            return false;
        }
    }
    if(m_Traits->has_inherent_trait(trait_name) == ITraitsCollection::TRAIT_ACTIVE) {
        m_Traits->add_permanent_trait(trait_name);
        m_Traits->update();
        return false;
    }
    bool result = m_Traits->add_permanent_trait(trait_name);
    m_Traits->update();
    return result;
}

bool ICharacter::lose_trait(const char *trait_name, bool hide, int chance) {
    if(chance < 100) {
        if(!g_Dice.percent(chance)) {
            return false;
        }
    }

    bool lost = false;
    lost |= m_Traits->disable_inherent_trait(trait_name);
    if(hide) {
        lost |= m_Traits->disable_permanent_trait(trait_name);
    } else {
        lost |= m_Traits->remove_permanent_trait(trait_name);
    }
    m_Traits->update();
    return lost;
}

void ICharacter::add_temporary_trait(const char *trait_name, int duration) {
    m_Traits->add_temporary_trait(trait_name, duration);
}

bool ICharacter::has_dormant_trait(const char *trait_name) const {
    auto inh = m_Traits->has_inherent_trait(trait_name);
    auto per = m_Traits->has_permanent_trait(trait_name);
    if(inh == ITraitsCollection::TRAIT_ACTIVE || per == ITraitsCollection::TRAIT_ACTIVE)
        return false;

    return inh == ITraitsCollection::TRAIT_INACTIVE || per == ITraitsCollection::TRAIT_INACTIVE;
}

std::vector<sTraitInfo> ICharacter::get_trait_info() const {
    return m_Traits->get_trait_info();
}

int ICharacter::get_trait_modifier(const char* type) const {
    return m_Traits->get_modifier(type);
}


namespace
{
    struct UpdateVisitor {
        ICharacter* target;
        int amount;
        int operator()(SKILLS skill) const {
            return target->upd_skill(skill, amount);
        }

        int operator()(STATS stat) const {
            return target->upd_base_stat(stat, amount);
        }
        using result_type = int;
    };

    struct GetVisitor {
        const ICharacter* target;
        int operator()(SKILLS skill) const {
            return target->get_skill(skill);
        }

        int operator()(STATS stat) const {
            return target->get_stat(stat);
        }
        using result_type = int;
    };
}


int ICharacter::get_attribute(StatSkill id) const {
    GetVisitor visitor{this};
    return id.apply_visitor(visitor);
}

int ICharacter::update_attribute(StatSkill id, int amount) {
    UpdateVisitor visitor{this, amount};
    return id.apply_visitor(visitor);
}

bool ICharacter::IsUnique() const {
    return m_IsUnique;
}
