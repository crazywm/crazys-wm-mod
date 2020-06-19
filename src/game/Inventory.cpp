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

#include "Inventory.hpp"
#include "cInventory.h"
#include "Game.hpp"
#include <algorithm>
#include <tinyxml2.h>
#include "xml/util.h"
#include "xml/getattr.h"
#include "character/ICharacter.h"
#include "character/traits/ITraitsCollection.h"


int Inventory::add_item(const sInventoryItem *item, int count) {
    auto found = m_ItemCounts.find(item);
    if(found != m_ItemCounts.end()) {
        found->second.count += count;
    } else {
        m_ItemCounts[item].count = count;
    }

    if(m_ItemCounts[item].count > 999) {
        int cap = m_ItemCounts[item].count - 999;
        m_ItemCounts[item].count -= cap;
        return count - cap;
    }
    return count;
}

bool Inventory::has_item(const sInventoryItem* item, int count) const {
    auto found = get_entry(item);
    if(found == m_ItemCounts.end()) {
        return false;
    }

    return found->second.count >= count;
}

int Inventory::get_num_items(const sInventoryItem* item) const {
    auto found = get_entry(item);
    if(found != m_ItemCounts.end()) {
        return found->second.count;
    }

    return 0;
}

int Inventory::get_num_items() const {
    int sum = 0;
    for(auto& entry : m_ItemCounts) {
        sum += entry.second.count;
    }
    return sum;
}

Inventory::item_map_citer Inventory::get_entry(const sInventoryItem* item) const {
    return m_ItemCounts.find(const_cast<sInventoryItem*>(item));
}

int Inventory::get_num_of_type(int type) const
{
    int num = 0;
    for(auto& entry : m_ItemCounts) {
        if (entry.first->m_Type == type)
            num++;
    }
    return num;
}

bool Inventory::empty() const
{
    return m_ItemCounts.empty();
}

int Inventory::remove_item(const sInventoryItem * item, int amount)
{
    auto found = m_ItemCounts.find(const_cast<sInventoryItem*>(item));
    if(found == m_ItemCounts.end()) {
        return 0;
    }

    // if equipped, figure out how many
    int num_equipped = found->second.equipped;

    // if the number of unequipped items is lower than the number we want to remove
    if(found->second.count - num_equipped < amount) {
        auto remove = found->second.count - num_equipped;
        // if all were unequipped, we can remove the entry
        if(num_equipped == 0) {
            m_ItemCounts.erase(found);
        } else {
            found->second.count -= remove;
        }
        return remove;
    }

    found->second.count -= amount;
    if(found->second.count == 0) {
        m_ItemCounts.erase(found);
    }
    return amount;
}

const sInventoryItem* Inventory::has_item(const std::string& item, int count) const
{
    auto p_item = g_Game->inventory_manager().GetItem(item);
    if(!p_item)
        return nullptr;
    if(has_item(p_item, count)) {
        return p_item;
    }
    return nullptr;
}

void Inventory::save_to_xml(tinyxml2::XMLElement& root)
{
    for(auto& entry : m_ItemCounts)
    {
        auto& elItem = PushNewElement(root, "Item");
        elItem.SetAttribute("Name", entry.first->m_Name.c_str());
        elItem.SetAttribute("Amount", entry.second.count);
        if(entry.second.equipped)
            elItem.SetAttribute("Equip", entry.second.equipped);
    }
}

void Inventory::load_from_xml(const tinyxml2::XMLElement& root, ICharacter* character)
{
    for (const auto& elItem : IterateChildElements(root, "Item"))
    {
        if (elItem.Attribute("Name"))
        {
            sInventoryItem* item = g_Game->inventory_manager().GetItem(elItem.Attribute("Name"));
            if (!item) {
                // TODO log error
                continue;
            }

            int amount = 1;
            if (elItem.Attribute("Amount"))    elItem.QueryIntAttribute("Amount", &amount);
            m_ItemCounts[item].count = GetIntAttribute(elItem, "Amount");
            int num_eq = elItem.IntAttribute("Equip", 0);
            if(num_eq > 0) {
                assert(character);
                for (int num = 0; num < num_eq; ++num) {
                    add_to_equipment(item, *character);
                }
            }
        }
    }
}

int Inventory::get_num_items(const std::string &item) const {
    auto p_item = g_Game->inventory_manager().GetItem(item);
    if(!p_item)
        return 0;
    return get_num_items(p_item);
}

int Inventory::num_equipped(const sInventoryItem* item) const
{
    auto found = m_ItemCounts.find(item);
    if(found == m_ItemCounts.end()) {
        return 0;
    }
    return found->second.equipped;
}

int Inventory::num_equipped_of_type(int type) const
{
    return std::count_if(begin(m_EquippedItems), end(m_EquippedItems), [type](auto& item){ return item.item->m_Type == type; });
}

bool Inventory::add_to_equipment(const sInventoryItem* item, ICharacter& target)
{
    auto found = m_ItemCounts.find(item);
    if(found == m_ItemCounts.end()) {
        return false;
    }

    // if the items are already equipped
    if(found->second.count <= found->second.equipped) {
        return false;
    }

    sEquipRecord equip;
    equip.item = item;

    // add traits
    bool change = false;
    for (auto& effect : item->m_Effects)
    {
        if(effect.m_Affects == sEffect::Trait) {
            equip.traits.push_back(target.raw_traits().add_dynamic_trait(effect.m_Trait.c_str(), effect.m_Amount == 0));
            change = true;
        }
    }

    if(change) {
        target.raw_traits().update();
    }

    m_EquippedItems.push_back(std::move(equip));
    ++found->second.equipped;
    return true;
}

bool Inventory::remove_from_equipment(const sInventoryItem* item)
{
    auto found = std::find_if(begin(m_EquippedItems), end(m_EquippedItems), [item](auto& eq){
        return eq.item == item; });

    if(found == m_EquippedItems.end()) {
        return false;
    }

    auto found2 = m_ItemCounts.find(item);
    if(found2 == m_ItemCounts.end()) {
        // TODO this is a violation of an invariant!
        assert(false);
        return false;
    }

    assert(found2->second.equipped > 0);
    m_EquippedItems.erase(found);
    --found2->second.equipped;

    return true;
}

void Inventory::unequip_all_of_type(int type, ICharacter &target) {
    for(auto& it : m_EquippedItems) {
        if(it.item->m_Type == type) {
            target.unequip(it.item);
            break;
        }
    }
}
