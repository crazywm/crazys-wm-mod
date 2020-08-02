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

#ifndef CRAZYS_WM_MOD_CSHOP_H
#define CRAZYS_WM_MOD_CSHOP_H

#include <vector>
#include <string>
#include <functional>
#include "Inventory.h"

class sInventoryItem;
class sGirl;

/*!
 * \brief The shop allows the player (and girls) to purchase some items.
 * \details Each week, the shop carries a certain selection of items. The
 * number of items is based on a (mostly) fixed shop capacity, and on the
 * number of items sold in the preceeding week.
 */
class cShop {
public:
    explicit cShop(int capacity);

    // iterate items
    template<class F>
    void IterateItems(F&& callback) const {
        for(auto& item : m_Inventory.all_items()) {
            callback(item.first, item.second.count);
        }
    }

    /// fill up the shop's inventory after the end of the week.
    void RestockShop();

    /// returns whether the shop's inventory is emtpy
    bool empty() const { return m_Inventory.empty(); }

    /// returns how many items `name` are in the shop.
    int CountItem(const std::string& name) const;

    /// Returns a randomly selected item.
    const sInventoryItem* GetRandomItem();

    // Buy an item -- remove `amount` many from the shop's inventory. Returns the amount of items removed from the
    // shop's inventory.
    int BuyItem(const sInventoryItem *item, int amount);

    // girl shopping
    bool GirlWantsItem(const sGirl& girl, const sInventoryItem& item) const;
    bool GirlBuyItem(sGirl& girl, const sInventoryItem& item);  // girl buys selected item if possible; returns true if bought
private:
    int m_Capacity;                             // basic amount of items the shop carries
    Inventory m_Inventory;
};


#endif //CRAZYS_WM_MOD_CSHOP_H
