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
#ifndef __CINVENTORY_H
#define __CINVENTORY_H

// includes
#include <ostream>
#include <string>
#include <vector>
#include <set>
#include <map>
#include <algorithm>

#include "Constants.h"
#include "utils/streaming_random_selection.hpp"

#include <memory>

struct sGirl;
namespace tinyxml2 {
    class XMLElement;
}

struct sEffect
{
    // MOD docclox
/*
 *    let's have an enum for possible values of m_Affects
 */
    enum What {
        Skill        = 0,
        Stat         = 1,
        Nothing      = 2,
        GirlStatus   = 3,
        Trait        = 4,
        Enjoy        = 5
    };
    What m_Affects;

/*
 *    and a function to go the other way
 *    we need this to turn the strings in the xml file
 *    into numbers
 */
    void set_what(std::string s);
/*
 *    can't make an enum for this since it can represent
 *    different quantites.
 *
 *    The OO approach would be to write some variant classes, I expect
 *    but really? Life's too short...
 */
    unsigned char m_EffectID;    // what stat, skill or status effect it affects

/*
 *    and we need to go the other way,
 *    setting m_EffectID from the strings in the XML file
 *
 *    WD:    Change to use definition and code in sGirl::
 *        remove duplicated code
 */
    bool set_skill(std::string s);
    bool set_girl_status(std::string s);
    bool set_stat(std::string s);
    bool set_Enjoyment(std::string s);


/*
 *    magnitude of the effect. 
 *    -10 will subtract 10 from the target stat while equiped
 *    and add 10 when unequiped.
 *
 *    With status effects and traits 1 means add,
 *    0 means take away and 2 means disable
 */
    int m_Amount;
    
    int m_Duration;    // `J` added for temporary trait duration
/*
 *    name of the trait it adds
 */
    std::string m_Trait;

    // chance that the effect happens (percent)
    // this only makes sense for consumable items, otherwise just re-equipping
    // can change this effect.
    int m_Chance = 100;

    // end mod
};

namespace tinyxml2
{
    class XMLElement;
}

class CraftingData {
public:
    /// checks whether the given girl can craft the item with the given job and craft points
    bool can_craft(sGirl& girl, JOBS job, int craft_points) const;

    /// checks whether the item is in principle craftable by the given job
    bool is_craftable_by(JOBS job) const;

    void from_xml(tinyxml2::XMLElement& element);

    int craft_cost() const;
    int mana_cost() const;
    int weight() const;
private:
    /// set of jobs that can craft this item
    std::set<JOBS> m_CraftableBy;

    /// creation cost
    int m_CraftPoints = 0;
    /// mana cost
    int m_ManaCost    = 0;
    /// relative weight for selecting this item when choosing what to craft
    int m_Weight      = 1;

    /// TODO skill, attribute, level conditions
    std::map<SKILLS, int> m_SkillRequirements;
    std::map<STATS, int> m_StatsRequirements;
};


struct sInventoryItem
{
    std::string m_Name;
    std::string m_Desc;
    /*
     *    item type: let's make an enum
     */
    enum Type {
        Ring = 1,            // worn on fingers (max 8)
        Dress = 2,            // Worn on body, (max 1)
        Shoes = 3,            // worn on feet, (max 1)
        Food = 4,            // Eaten, single use
        Necklace = 5,        // worn on neck, (max 1)
        Weapon = 6,            // equiped on body, (max 2)
        Makeup = 7,            // worn on face, single use
        Armor = 8,            // worn on body over dresses (max 1)
        Misc = 9,            // random stuff. may cause a constant effect without having to be equiped
        Armband = 10,        // (max 2), worn around arms
        SmWeapon = 11,        // (max 2), hidden on body
        Underwear = 12,        // (max 1) worn under dress
        Hat = 13,            // CRAZY added this - Noncombat worn on the head (max 1)
        Helmet = 14,        // CRAZY added this    - Combat worn on the head (max 1)
        Glasses = 15,        // CRAZY added this    - Glasses (max 1)
        Swimsuit = 16,        // CRAZY added this - Swimsuit (max 1 in use but can have as many as they want)
        Combatshoes = 17,    // `J`   added this - Combat Shoes (max 1) often unequipped outside of combat
        Shield = 18            // `J`   added this - Shields (max 1) often unequipped outside of combat
    };
    Type m_Type;
    /*
     *    and another for special values
     */
    enum Special {
        None = 0,
        AffectsAll = 1,
        Temporary = 2
    };
    Special m_Special;
    /*
     *    if 1 then this item doesn't run out if stocked in shop inventory
     */
    bool m_Infinite;
    /*
     *    the number of effects this item has
     */
    std::vector<sEffect> m_Effects;
    /*
     *    how much the item is worth?
     */
    long m_Cost;
    /*    0 is good, while badness > is evil.
     *    Girls may fight back if badness > 0,
     *    Girls won't normally buy items > 20 on their own
     *      default formula is -5% chance to buy on their own per Badness point (5 Badness = 75% chance)
     *    Girls with low obedience may take off something that is bad for them
     */
    int m_Badness;
    int m_GirlBuyChance;  // chance that a girl on break will buy this item if she's looking at it in the shop

    Item_Rarity m_Rarity;

    void set_rarity(const std::string& s);
    void set_special(const std::string& s);
    void set_type(const std::string& s);

    CraftingData m_Crafting;
    friend std::ostream& operator<<(std::ostream& os,const sInventoryItem::Type& typ);
};

class cInventory
{
public:
    cInventory() {
    }
    ~cInventory();

    bool LoadItemsXML(const std::string& filename);
    sInventoryItem* GetItem(std::string name);


    // Returns a random item; if none can be found (that is, if the
    // internal item list is empty) ,`nullptr` is returned instead.
    //
    // Note: Will not spuriously return `nullptr`.
    sInventoryItem* GetRandomItem() const
    {
       return GetRandomItem([](auto& /*item*/){return true;});;
    }

    // Returns a random item that satisfies the predicate `pred`; if
    // none can be found, `nullptr` is returned instead.
    //
    // Note: Will not spuriously return `nullptr`.
    template<typename Pred>
    sInventoryItem* GetRandomItem(Pred pred) const
    {
       RandomSelector<sInventoryItem> sel;

       std::for_each(begin(m_Items), end(m_Items),
                     [pred, &sel](const std::unique_ptr<sInventoryItem>& ptr) {
                       if(ptr && pred(*ptr))
                          sel.process(ptr.get(), 1.0);
                    });
       return sel.selection();
    }

    sInventoryItem* GetRandomCatacombItem();

    sInventoryItem* GetCraftableItem(sGirl& girl, JOBS job, int craft_points);
    /// return all items that can be crafted by a given job.
    std::vector<sInventoryItem*> GetCraftableItems(JOBS job);

    void Equip(sGirl& girl, const sInventoryItem* item, bool force);

    void AddItem(sInventoryItem* item);

    int HappinessFromItem(const sInventoryItem * item);  // determines how happy a girl will be to receive an item (or how unhappy to lose it)

    void GivePlayerAllItems();

    bool IsItemEquipable(const sInventoryItem *item)
    {
        return item->m_Type != sInventoryItem::Type::Misc;
    }

    static int NumItemSlots(const sInventoryItem* item);


private:
    mutable std::vector<std::unique_ptr<sInventoryItem>> m_Items;  // Master list of items?
};

#endif
