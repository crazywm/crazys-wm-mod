/*
 * Copyright 2009, 2010, The Pink Petal Development Team.
 * The Pink Petal Devloment Team are defined as the game's coders
 * who meet on http://pinkpetal.org     // old site: http://pinkpetal .co.cc
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
#include "src/Game.hpp"
#include "tinyxml.h"


int Inventory::add_item(sInventoryItem* item, int count) {
    auto found = m_ItemCounts.find(item);
    if(found != m_ItemCounts.end()) {
        found->second += count;
    } else {
        m_ItemCounts[item] = count;
    }

    if(m_ItemCounts[item] > 999) {
        int cap = m_ItemCounts[item] - 999;
        m_ItemCounts[item] -= cap;
        return count - cap;
    }
    return count;
}

bool Inventory::has_item(const sInventoryItem* item, int count) const {
    auto found = get_entry(item);
    if(found == m_ItemCounts.end()) {
        return false;
    }

    return found->second >= count;
}

int Inventory::get_num_items(const sInventoryItem* item) const {
    auto found = get_entry(item);
    if(found != m_ItemCounts.end()) {
        return found->second;
    }

    return 0;
}

int Inventory::get_num_items() const {
    int sum = 0;
    for(auto& entry : m_ItemCounts) {
        sum += entry.second;
    }
    return sum;
}

Inventory::item_map_citer Inventory::get_entry(const sInventoryItem* item) const {
    return m_ItemCounts.find(const_cast<sInventoryItem*>(item));
}

int Inventory::get_num_classes(int type) const
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

    if(found->second < amount) {
        auto remove = found->second;
        m_ItemCounts.erase(found);
        return remove;
    }

    found->second -= amount;
    return amount;
}

bool Inventory::has_item(const std::string& item, int count) const
{
    auto p_item = g_Game.inventory_manager().GetItem(item);
    if(!p_item)
        return false;
    return has_item(p_item, count);
}

void Inventory::save_to_xml(TiXmlElement& root)
{
    for(auto& entry : m_ItemCounts)
    {
        auto pItem = new TiXmlElement("Item");
        root.LinkEndChild(pItem);
        pItem->SetAttribute("Name", entry.first->m_Name);
        pItem->SetAttribute("Amount", entry.second);
    }
}

void Inventory::load_from_xml(TiXmlElement& root)
{
    for (TiXmlElement* pItem = root.FirstChildElement("Item"); pItem != nullptr; pItem = pItem->NextSiblingElement("Item"))
    {
        if (pItem->Attribute("Name"))
        {
            sInventoryItem* item = g_Game.inventory_manager().GetItem(pItem->Attribute("Name"));
            if (!item) {
                // Todo log error
                continue;
            }

            int amount = 1;
            if (pItem->Attribute("Amount"))	pItem->QueryIntAttribute("Amount", &amount);
            m_ItemCounts[item] = amount;
        }
    }
}
