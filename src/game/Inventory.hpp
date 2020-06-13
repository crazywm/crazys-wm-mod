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


#ifndef CRAZYS_WM_MOD_INVENTORY_HPP
#define CRAZYS_WM_MOD_INVENTORY_HPP

#include <map>
#include <string>
#include <vector>
#include "character/traits/hDynamicTraitHandle.h"

namespace tinyxml2 {
    class XMLElement;
}

class sInventoryItem;
class ICharacter;

/// TODO maybe we should separate Inventory and Equipment
class Inventory {
private:
    struct ItemData {
        int count = 1;          // How many items in inventory
        int equipped = 0;       // How many of those are equipped
    };

    using item_map_t = std::map<const sInventoryItem*, ItemData>;
    using item_map_iter = typename item_map_t::iterator;
    using item_map_citer = typename item_map_t::const_iterator;
public:
    explicit Inventory() {};

    /// adds an item to the inventory. Returns the number of items that were added.
    int add_item(const sInventoryItem* item, int count = 1);

    /// returns whether the item is present in the inventory
    bool empty() const;
    bool has_item(const sInventoryItem* item, int count = 1) const;
    const sInventoryItem* has_item(const std::string& item, int count = 1) const;

    /// gets the total number of items in the inventory
    int get_num_items() const;
    /// get number of items in a given class
    int get_num_items(const sInventoryItem* item) const;
    int get_num_items(const std::string& item) const;
    /// get number of item classes of a given type
    int get_num_of_type(int type) const;

    /// remove item from inventory. Returns the number of elements that were removed. This function will not remove
    /// equipped items: These need to be unequipped first!
    int remove_item(const sInventoryItem* item, int amount = 1);

    // for presenting all items
    const item_map_t& all_items() const { return m_ItemCounts; }
    item_map_t& all_items() { return m_ItemCounts; }  // TODO delete this?

    // equipment
    int num_equipped(const sInventoryItem* item) const;
    int num_equipped_of_type(int type) const;

    bool add_to_equipment(const sInventoryItem* item, ICharacter& target);
    bool remove_from_equipment(const sInventoryItem* item);
    void unequip_all_of_type(int type, ICharacter& target);

    // io
    void load_from_xml(const tinyxml2::XMLElement& root, ICharacter* character);
    void save_to_xml(tinyxml2::XMLElement& root);
private:
    item_map_t m_ItemCounts;
    item_map_citer get_entry(const sInventoryItem* item) const;

    struct sEquipRecord {
        const sInventoryItem* item;
        std::vector<hDynamicTraitHandle> traits;
    };

    std::vector<sEquipRecord> m_EquippedItems;
};

#endif //CRAZYS_WM_MOD_INVENTORY_HPP
