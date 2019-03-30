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

class sInventoryItem;
class TiXmlElement;

class Inventory {
private:
    using item_map_t = std::map<sInventoryItem*, int>;
    using item_map_iter = typename item_map_t::iterator;
    using item_map_citer = typename item_map_t::const_iterator;
public:
    /// adds an item to the inventory. Returns the number of items that were added.
    int add_item(sInventoryItem* item, int count = 1);

    /// returns whether the item is present in the inventory
    bool empty() const;
    bool has_item(const sInventoryItem* item, int count = 1) const;
    bool has_item(const std::string& item, int count = 1) const;

    /// gets the total number of items in the inventory
    int get_num_items() const;
    /// get number of items in a given class
    int get_num_items(const sInventoryItem* item) const;
    /// get number of item classes of a given type
    int get_num_classes(int type) const;

    /// remove item from inventory. Returns the number of elements that were removed.
    int remove_item(const sInventoryItem* item, int amount = 1);

    // for presenting all items
    const item_map_t& all_items() const { return m_ItemCounts; }

    // io
    void load_from_xml(TiXmlElement& root);
    void save_to_xml(TiXmlElement& root);
private:
    std::map<sInventoryItem*, int> m_ItemCounts;

    item_map_citer get_entry(const sInventoryItem* item) const;
};

#endif //CRAZYS_WM_MOD_INVENTORY_HPP
