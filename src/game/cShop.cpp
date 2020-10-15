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

#include "cInventory.h"
#include "cShop.h"
#include "character/sGirl.h"
#include "cGirls.h"
#include "Game.hpp"

bool is_warrior(const sGirl& girl) {
    return girl.has_active_trait("Adventurer") || girl.has_active_trait("Assassin") ||
        girl.combat() >= 50 || girl.magic() >= 50;
}

bool cShop::GirlWantsItem(const sGirl& girl, const sInventoryItem& item) const {
    if (girl.has_item(&item)            // if she already has one
        || girl.m_Money - item.m_Cost < 0)             // or if she can't afford it
        return false;                                // skip it

    if (!g_Dice.percent(item.m_GirlBuyChance))
        return false;

    switch (item.m_Type) {
        default: return true;
        case sInventoryItem::Armband:
        case sInventoryItem::SmWeapon:
            if(is_warrior(girl)) {
                int magic = 0, combat = 0;
                for (const auto &curEffect : item.m_Effects)
                {
                    if (curEffect.m_EffectID == SKILL_COMBAT)    combat += curEffect.m_Amount;
                    if (curEffect.m_EffectID == SKILL_MAGIC)    magic += curEffect.m_Amount;
                }
                if ((girl.combat() >= girl.magic() && combat >= magic) ||    // if the girl favors combat and the item favors combat
                    (girl.magic() >= girl.combat() && magic >= combat))    // or if the girl favors magic and the item favors magic
                {
                    return true;        // buy it
                }
            }
            return false;
    }
    return true;
}

bool cShop::GirlBuyItem(sGirl& girl, const sInventoryItem& item) {
    int MaxItems = 1;
    bool AutoEquip = true;
    switch (item.m_Type) {
        default:
            MaxItems = 1;
            break;
        case sInventoryItem::Ring:
            MaxItems = 8;
            break;
        case sInventoryItem::Food:
        case sInventoryItem::Makeup:
        case sInventoryItem::Misc:
            MaxItems = 15; AutoEquip = false;
            break;
        case sInventoryItem::Armband:
        case sInventoryItem::SmWeapon:
            MaxItems = 2;
            break;
    }

    // cannot afford it
    if(girl.m_Money < item.m_Cost) return false;

    if (cGirls::GetNumItemType(girl, item.m_Type) >= MaxItems)
    {
        // if she has enough of this type, she won't buy more unless it's better than what she has
        auto nicerThan = cGirls::GetWorseItem(girl, (int)item.m_Type, item.m_Cost);
        if (!nicerThan)
        {
            return false;
        }
        // found a worse item of the same type in her inventory
        cGirls::SellInvItem(girl, nicerThan);
    }

    girl.m_Money -= item.m_Cost;
    girl.add_item(&item);
    if (AutoEquip) girl.equip(&item, false);    // MYR: Check temp value
    return true;

}

const sInventoryItem* cShop::GetRandomItem() {
    if(m_Inventory.empty())
        return nullptr;

    int index = g_Dice % m_Inventory.all_items().size();
    auto it = m_Inventory.all_items().begin();
    std::advance(it, index);
    return it->first;
}

cShop::cShop(int capacity) : m_Capacity(capacity) {

}

int cShop::CountItem(const std::string &name) const {
    return m_Inventory.get_num_items(name);
}

int cShop::BuyItem(const sInventoryItem *item, int amount) {
    return m_Inventory.remove_item(item, amount);
}

void cShop::RestockShop() {
    // remove 50% of old items
    for(auto it = m_Inventory.all_items().begin(); it != m_Inventory.all_items().end(); ) {
        if(g_Dice.percent(50)) {
            it = m_Inventory.all_items().erase(it);
        } else {
            it = ++it;
        }
    }

    // fill up again
    while(m_Inventory.all_items().size() < m_Capacity) {
        // find a random item
        sInventoryItem* item = g_Game->inventory_manager().GetRandomItem();
        if(!item) break;        // supplier is all out :(

        // check spawn chance
        int chance = g_Dice.d100();
        if (!(item->m_Rarity == sInventoryItem::Common ||
             (item->m_Rarity == sInventoryItem::Shop50 && chance <= 50) ||
             (item->m_Rarity == sInventoryItem::Shop25 && chance <= 25) ||
             (item->m_Rarity == sInventoryItem::Shop05 && chance <= 5))) {
            continue;
        }

        // check duplicates
        m_Inventory.add_item(item, item->m_Infinite ? 1000 : 1);
    }
}
