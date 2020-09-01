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
#include <tinyxml2.h>
#include "cInventory.h"
#include <iostream>
#include "cGirls.h"
#include "character/pregnancy.h"
#include "character/sGirl.h"
#include "character/cCustomers.h"
#include "character/cPlayer.h"
#include "Game.hpp"
#include "utils/streaming_random_selection.hpp"
#include "CLog.h"
#include "xml/util.h"
#include "xml/getattr.h"
#include "Inventory.h"

using namespace std;

namespace settings {
    extern const char* PREG_MULTI_CHANCE;
}

extern cRng g_Dice;

extern string stringtolowerj(string name);

// ----- Misc

cInventory::~cInventory() = default;

bool sEffect::set_skill(string s)
{
    m_EffectID = get_skill_id(s);
    return true;
}
bool sEffect::set_girl_status(string s)
{
    m_EffectID = get_status_id(s);
    return true;
}
bool sEffect::set_stat(string s)
{
    try {
        m_EffectID = get_stat_id(s);
        return true;
    } catch (std::out_of_range& oor) {
        return false;
    }
}
bool sEffect::set_Enjoyment(string s)
{
    m_EffectID = get_action_id(s);
    return true;
}

void cInventory::GivePlayerAllItems()
{
    sInventoryItem* item;
    for (auto & m_Item : m_Items)
    {
        item = m_Item;
        g_Game->player().inventory().add_item(item, 999);
    }
}

/*
 * should make this part of the cInventoryItem
 */
static void do_effects(tinyxml2::XMLElement *parent, sInventoryItem *item)
{
    const char *pt;
    for (auto el = parent->FirstChildElement("Effect"); el; el = el->NextSiblingElement("Effect"))
    {
        sEffect eff;
        if (pt = el->Attribute("What")) eff.set_what(pt);
        if (pt = el->Attribute("Name"))
        {
            switch (eff.m_Affects)
            {
            case sEffect::Trait:
                eff.m_Trait = pt;
                break;
            case sEffect::Stat:
                if (eff.set_stat(pt) == false)
                {
                    g_LogFile.log(ELogLevel::ERROR, "effect type code == ", eff.m_Affects,
                            " stat lookup failed for ", item->m_Name);
                }
                break;
            case sEffect::GirlStatus:
                if (!eff.set_girl_status(pt))  g_LogFile.log(ELogLevel::ERROR, "Error in item ", item->m_Name);
                break;
            case sEffect::Skill:
                if (!eff.set_skill(pt)) g_LogFile.log(ELogLevel::ERROR, "Error in item ", item->m_Name);
                break;
            case sEffect::Enjoy:
                if (!eff.set_Enjoyment(pt)) g_LogFile.log(ELogLevel::ERROR, "Error in item ", item->m_Name);
                break;
            default:
                g_LogFile.log(ELogLevel::ERROR, "Can't handle effect type ", eff.m_Affects);
                break;
            }
        }
        eff.m_Amount = el->IntAttribute("Amount", eff.m_Amount);
        eff.m_Duration = el->IntAttribute("Duration", item->m_Special == 2 ? 20 : 0);

        item->m_Effects.push_back(std::move(eff));
    }
}

void cInventory::AddItem(sInventoryItem* item)
{
    m_Items.push_back(item);
}

int cInventory::HappinessFromItem(const sInventoryItem * item)
{
    // decrease value by 5% for each point of badness
    int Value = int((double)item->m_Cost * ((100 - ((double)item->m_Badness * 5)) / 100));
    // then, 1 happiness per 300 gold value
    return (Value > 300) ? (Value / 300) : 1;
}

ostream& operator << (ostream& os, sEffect::What &w)
{
    switch (w) {
    case sEffect::Skill:        return os << "Skill";
    case sEffect::Stat:            return os << "Stat";
    case sEffect::Nothing:        return os << "Nothing";
    case sEffect::GirlStatus:    return os << "GirlStatus";
    case sEffect::Trait:        return os << "Trait";
    case sEffect::Enjoy:        return os << "Enjoy";
    default:
        g_LogFile.log(ELogLevel::ERROR, "unexpected 'what' value: ", int(w));
        return os << "Error(" << int(w) << ")";
    }
}

void sEffect::set_what(string s) {
    if        (s == "Skill")        m_Affects = Skill;
    else if    (s == "Stat")        m_Affects = Stat;
    else if    (s == "Nothing")    m_Affects = Nothing;
    else if    (s == "GirlStatus")    m_Affects = GirlStatus;
    else if (s == "Trait")        m_Affects = Trait;
    else if (s == "Enjoy")        m_Affects = Enjoy;
    else {
        m_Affects = Nothing;
        g_LogFile.log(ELogLevel::ERROR, "Bad 'what' string for item effect: '", s, "'");
    }
}

ostream& operator<<(ostream& os, sEffect& eff) {
    os << "Effect: " << eff.m_Affects << " ";
    if (eff.m_Affects == sEffect::Stat) { os << get_stat_name((STATS)eff.m_EffectID); }
    if (eff.m_Affects == sEffect::Skill) { os << get_skill_name((SKILLS)eff.m_EffectID); }
    if (eff.m_Affects == sEffect::Trait) { os << "'" << eff.m_Trait << "'"; }
    if (eff.m_Affects == sEffect::GirlStatus) { os << get_status_name((STATUS)eff.m_EffectID); }
    if (eff.m_Affects == sEffect::Enjoy) { os << get_action_name((Action_Types)eff.m_EffectID); }
    os << (eff.m_Amount > 0 ? " +" : " ") << eff.m_Amount;
    return os << endl;
}

// ----- Get
sInventoryItem* cInventory::GetRandomItem()
{
    sInventoryItem *ipt;
    if (m_Items.empty())
    {
        return nullptr;
    }
    if (m_Items.size() == 1)
    {
        ipt = m_Items[0];
        return m_Items[0];
    }
    int index = g_Dice % (m_Items.size() - 1);    // fixed crash with going outside vector size - necro
    ipt = m_Items[index];
    return ipt;
}
sInventoryItem* cInventory::GetRandomCatacombItem()
{
    if (m_Items.empty())    return nullptr;
    sInventoryItem *temp = nullptr;
    int index = g_Dice % (m_Items.size() - 1);

    int tries = m_Items.size() / 3;    // try 1/3 of all items to get an item
    while (tries > 0)
    {
        if (tries % 10 == 0) index = g_Dice % (m_Items.size() - 1);
        if (index >= (int)m_Items.size()) index = 0;
        temp = m_Items[index];
        switch (temp->m_Rarity) {
        case RARITYSHOP25:                                    return temp;    break;
        case RARITYSHOP05:        if (g_Dice.percent(25))        return temp;    break;
        case RARITYCATACOMB15:    if (g_Dice.percent(15))        return temp;    break;
        case RARITYCATACOMB05:    if (g_Dice.percent(5))        return temp;    break;
        case RARITYCATACOMB01:    if (g_Dice.percent(1))        return temp;    break;
        case RARITYSCRIPTONLY:
        case RARITYSCRIPTORREWARD:
            temp = nullptr;
            break;    // if at the end it is a script item, no item is returned
        case RARITYCOMMON:
        case RARITYSHOP50:
        default:
            break;    // if at the end it is a common item, that item is returned
        }
        tries--;
        index++;
    }
    if (!temp) return nullptr;
    return temp;
}

sInventoryItem* cInventory::GetItem(string name)
{
    for (auto& item : m_Items)
    {
        if (item->m_Name == name) return item;
    }
    return nullptr;
}

// ----- Equip unequip

void cInventory::Equip(sGirl& girl, const sInventoryItem* item, bool force)
{
    if (item == nullptr) return;    // no item then ignore the call

    if (girl.is_dead() && (    // Allow certain Items to recover the dead
        stringtolowerj(item->m_Name) == stringtolowerj("Better Zed than Dead") ||
        stringtolowerj(item->m_Name) == stringtolowerj("Elixir of Ultimate Regeneration")
        )){}
    // A few items are hard coded
    else if (stringtolowerj(item->m_Name) == stringtolowerj("Reset Potion MK i"))
    {
        int age = girl.age();
        // reset all numbers to default
        for (int i = 0; i < NUM_SKILLS; i++)    girl.set_skill(i, 0);
        for (int i = 0; i < NUM_STATS; i++)     girl.set_stat(i, 0);
        girl.set_stat(STAT_HEALTH, 100);
        girl.set_stat(STAT_HAPPINESS, 100);
        girl.set_stat(STAT_AGE, (age == 100 ? 100 : 18)); // keep ageless girls ageless    // `J` Legal Note: 18 is the Legal Age of Majority for the USA where I live
        girl.set_default_house_percent();

        g_Game->push_message(girl.FullName() + ": " + item->m_Name +
                             ": The use of this item has reset all her stats and skills to default.", COLOR_BLUE);
        girl.remove_item(item);
        cGirls::CalculateGirlType(girl);
        return;
    }
    else if (stringtolowerj(item->m_Name) == stringtolowerj("Reset Potion MK ii"))
    {
        /// TODO (traits) RESET POTION MK ii
        /*
        // remove all traits
        girl.set_stat(STAT_HEALTH, 100);        // `J` revive dead girls
        for (int i = 0; i < MAXNUM_TRAITS; i++)
        {
            if (girl.m_Traits[i]) girl.remove_trait(girl.m_Traits[i]->name(), false, true, false);
        }
        cGirls::RemoveAllRememberedTraits(girl);

        g_Game->push_message(girl.FullName() + ": " + item->m_Name +
                             ": The use of this item has removed all her traits.", COLOR_BLUE);
        girl.remove_item(item);
        cGirls::ApplyTraits(girl);
        cGirls::CalculateGirlType(girl);
         */
        return;
    }
    else if (girl.is_dead() ||                    // dead girls shouldn't be able to equip or use anything
        girl.inventory().num_equipped(item) == girl.inventory().get_num_items(item) ||        // if all are already equipped do nothing
        item->m_Special == sInventoryItem::AffectsAll) // no "AffectsAll" item should be equipable
        return;

    // check if the item is equippable
    if(!girl.can_equip(item)) {
        if(!force) return;
        // if forced, unequip an item of the given type
        girl.inventory().unequip_all_of_type(item->m_Type, girl);
        if(!girl.can_equip(item)) {
            return;  // something went wrong. TODO log an error here!
        }
    }

    bool usenewpreg = false;
#if 1        // `J` Pregnancy item fix
    usenewpreg = true;
    stringstream pregmsg;
    bool pregbefore = girl.is_pregnant();
    int pregadd = 0;    int pregend = 0;
    int pregbyC = 0;    int pregrmC = 0;
    int pregbyP = 0;    int pregrmP = 0;
    int pregbyB = 0;    int pregrmB = 0;

    for (auto effect : item->m_Effects)
    {
        if(g_Dice % 100 >= effect.m_Chance) {
            continue;
        }
        int affects = effect.m_Affects;
        int eff_id = effect.m_EffectID;
        int amount = effect.m_Amount;

        if (affects == sEffect::GirlStatus && (eff_id == STATUS_PREGNANT || eff_id == STATUS_PREGNANT_BY_PLAYER || eff_id == STATUS_INSEMINATED))
        {
            if (amount >= 1)
            {
                pregadd++;
                if (eff_id == STATUS_PREGNANT)                pregbyC++;
                if (eff_id == STATUS_PREGNANT_BY_PLAYER)    pregbyP++;
                if (eff_id == STATUS_INSEMINATED)            pregbyB++;
            }
            if (amount == 0)
            {
                pregend++;
                if (eff_id == STATUS_PREGNANT)                pregrmC++;
                if (eff_id == STATUS_PREGNANT_BY_PLAYER)    pregrmP++;
                if (eff_id == STATUS_INSEMINATED)            pregrmB++;
            }
        }
    }
    if (pregadd + pregend > 0)    // if the item changes pregnancy
    {
        int badchance = 0;
        int goodchance = 0;
        bool endpreg = false;
        bool startpreg = false;
        int type = STATUS_PREGNANT;
        pregmsg << girl.FullName() << " has used her " << item->m_Name << ".\n";

        if (pregadd == 0)        // end only
        {
            if (pregbefore)        // can end normally
            {
                if (((girl.has_status(STATUS_PREGNANT) || girl.has_status(STATUS_PREGNANT_BY_PLAYER))    // abort human
                    && pregrmC + pregrmP > 0) || (girl.has_status(STATUS_INSEMINATED) && pregrmB > 0))        // abort beast
                {
                    endpreg = true;    // the item can do what is intended
                }
                else    // if made to abort human only but trying on beast or beast on human - can be bad
                {
                    badchance += pregend * 5;
                    pregmsg << "It was not intended to end "
                        << (girl.has_status(STATUS_INSEMINATED) ? "beast " : "human ") << "pregnancies ";
                    if (g_Dice.percent(min(80, pregend * 10)))    // may still abort
                    {
                        endpreg = true;
                        pregmsg << "but it still worked.";
                        badchance *= 4;
                    }
                    else pregmsg << "so it made her sick.";
                }
            }
            else                // not pregnant so possible bad effects
            {
                pregmsg << "She was not pregnant before she used it so it made her really sick.";
                badchance += pregend * 10;
            }
        }
        else if (pregend == 0)    // add only
        {
            if (!pregbefore)    // can add normally
            {
                startpreg = true;
                if (pregbyC == 0 && pregbyP == 0)                    // add beast
                {
                    type = STATUS_INSEMINATED;
                }
                else if (pregbyC == 0 && pregbyB == 0)                // add Player
                {
                    type = STATUS_PREGNANT_BY_PLAYER;
                }
                else if (pregbyP == 0 && pregbyB == 0)                // add Customer
                {
                    type = STATUS_PREGNANT;
                }
                else if (pregbyP > 0 && pregbyC > 0 && pregbyB > 0)    // tries to add all three
                {
                    if (pregbyP == pregbyC && pregbyP == pregbyB)    // all are equal
                    {
                        int roll = g_Dice % 100;
                        /* */if (roll < 33)    type = STATUS_PREGNANT;
                        else if (roll < 66)    type = STATUS_PREGNANT_BY_PLAYER;
                        else/*           */    type = STATUS_INSEMINATED;            // slightly higher chance for beast
                    }
                    else if (pregbyP > pregbyC && pregbyP > pregbyB)    type = STATUS_PREGNANT_BY_PLAYER;    // Player is highest
                    else if (pregbyC > pregbyP && pregbyC > pregbyB)    type = STATUS_PREGNANT;                // Cust is highest
                    else if (pregbyB > pregbyC && pregbyB > pregbyP)    type = STATUS_INSEMINATED;            // Beast is highest
                    else
                    {
                        int roll = g_Dice % (pregbyP + pregbyC + pregbyB);
                        /* */if (roll < pregbyC)/*      */    type = STATUS_PREGNANT;
                        else if (roll < pregbyP + pregbyC)    type = STATUS_PREGNANT_BY_PLAYER;
                        else/*                          */    type = STATUS_INSEMINATED;
                    }
                }

            }
            else                // already pregnant
            {
                if (girl.has_status(STATUS_PREGNANT) || girl.has_status(STATUS_PREGNANT_BY_PLAYER))
                {
                    pregmsg << "She is already pregnant";
                    if (girl.has_status(STATUS_PREGNANT_BY_PLAYER))
                        pregmsg << " with your child";
                    if (pregbyB == 0)    // human only
                    {
                        pregmsg << " so the new pregnancy did not take hold.";
                    }
                    else                // possible beast insemination
                    {
                        double chance = 10.0;
                        if (pregbyC + pregbyP > 0) chance /= pregbyC + pregbyP;
                        if (chance < 1.0) chance = 1.0;
                        if (g_Dice.percent(pregbyB * chance))
                        {
                            pregmsg << " but the insemination fluid consumed the human baby and took over the pregnancy.";
                            girl.clear_pregnancy();
                            type = STATUS_INSEMINATED;
                            startpreg = true;
                            badchance += pregbyB * 5;
                        }
                        else pregmsg << " so the new pregnancy did not take hold.";
                    }

                }
                else            // already inseminated
                {
                    pregmsg << "She is already carrying some type of beast in her";
                    if (girl.m_WeeksPreg > girl.get_preg_duration() / 2 && g_Dice.percent(50))    // more than halfway through pregnancy
                    {
                        pregmsg << " and it fed on what the item tried to implant.";
                        // zzzzzz - Add in the effects
                        pregmsg << " (not implemented yet).";

                    }
                    else if (girl.m_WeeksPreg < 3 && g_Dice.percent(100 * g_Game->settings().get_percent(settings::PREG_MULTI_CHANCE)*pregadd * 2))    // fairly new pregnancy
                    {
                        pregmsg << " and the new implantation" << (pregadd > 1 ? "s" : "") << " seemed to cause it to multiply instead of create a new pregnancy.";
                        // zzzzzz - Add in the effects
                        pregmsg << " (not implemented yet).";
                    }
                    else
                    {
                        pregmsg << " so the new pregnancy did not take hold.";
                    }

                }
            }
        }
        else                    // some combination of add and end
        {
#if 0
            if (pregbefore)        // end old pregnancy to start a new one?
            {
                if (girl.has_status(STATUS_PREGNANT))
                {

                }
                else if (girl.has_status(STATUS_PREGNANT_BY_PLAYER))
                {

                }
                else    // girl.has_status(STATUS_INSEMINATED)
                {

                }

//                pregbyC                pregbyP                pregbyB
//                pregrmC                pregrmP                pregrmB



            }
            else                // or just start a new one
            {

            }
#else
            pregmsg << "(Add AND Remove pregnancy. Details not implemented yet)\n";
            endpreg = true;
            startpreg = true;
            int roll = g_Dice % (pregbyP + pregbyC + pregbyB);
            /* */if (roll < pregbyC)/*      */    type = STATUS_PREGNANT;
            else if (roll < pregbyP + pregbyC)    type = STATUS_PREGNANT_BY_PLAYER;
            else/*                          */    type = STATUS_INSEMINATED;
#endif


        }

        if (endpreg)
        {
            girl.m_ChildrenCount[CHILD09_ABORTIONS]++;
            pregmsg << "\nShe is no longer ";
            if (girl.has_status(STATUS_PREGNANT_BY_PLAYER))
            {
                pregmsg << "pregnant with your child.";
            }
            else if (girl.has_status(STATUS_INSEMINATED))
            {
                pregmsg << "inseminated.";
            }
            else
            {
                pregmsg << "pregnant.";
            }
            girl.clear_pregnancy();
        }
        if (startpreg)
        {
            int numchildren = 1 + g_Dice % pregbyP;
            if (type == STATUS_PREGNANT_BY_PLAYER)
            {
                create_pregnancy(girl, numchildren, type, g_Game->player());
                pregmsg << "\nThe item has gotten her pregnant with your child.";
            }
            else
            {
                if (type == STATUS_INSEMINATED)
                {
                    numchildren = pregbyB;
                    pregmsg << "\nThe item has gotten her inseminated.";
                }
                else
                {
                    numchildren = 1 + g_Dice % pregbyC;
                    pregmsg << "\nThe item has gotten her pregnant.";
                }
                sCustomer Cust{};
                create_pregnancy(girl, numchildren, type, Cust);
            }
        }


        if (stringtolowerj(item->m_Name) == stringtolowerj("Better Zed than Dead"))
        {
            // `J` a few specific items don't need the rest of the code
        }
        else
        {
            if (badchance > 0 && goodchance > 0)
            {
                pregmsg << "\n(not implemented yet: badchance = " << badchance << " : goodchance = " << goodchance << ")";
            }
            else if (badchance > 0)
            {
                pregmsg << "\n(not implemented yet: badchance = " << badchance << ")";
            }
            else if (goodchance > 0)
            {
                pregmsg << "\n(not implemented yet: goodchance = " << goodchance << ")";
            }


            if (pregmsg.str().length() > 0)
            {
                g_Game->push_message(pregmsg.str(), COLOR_RED);
            }

        }
    }




#endif

    // apply the effects
    bool is_consumed = item->m_Type == sInventoryItem::Food || item->m_Type == sInventoryItem::Makeup;
    for (int i = 0; i < item->m_Effects.size(); i++)
    {
        int affects = item->m_Effects[i].m_Affects;
        int eff_id = item->m_Effects[i].m_EffectID;
        int amount = item->m_Effects[i].m_Amount;
        int duration = item->m_Effects[i].m_Duration;
        bool tempeff = item->m_Special == sInventoryItem::Temporary && affects != sEffect::Trait;

        if (tempeff)
        {
            switch (affects) {
                case sEffect::Nothing:
                    break;
                case sEffect::GirlStatus:

                    if (usenewpreg && (eff_id == STATUS_PREGNANT || eff_id == STATUS_PREGNANT_BY_PLAYER ||
                                       eff_id == STATUS_INSEMINATED)) {
                        break;    // `J` handled at the start
                    }

                    /*
                     *                I can understand ignoring "Nothing"
                     *                Ought "GirlStatus" to have an effect?
                     *                Seem to remember this cropping up on the forum
        "None",        "Poisoned",        "Badly Poisoned",
        "Slave",        "Controlled",
        "Catacombs",    "Arena",        "Your Daughter"
                     */
                    break;

                case sEffect::Skill:            // affects skill
                    girl.upd_temp_skill(eff_id, amount);
                    break;

                case sEffect::Stat:            // affects skill
                    girl.upd_temp_stat(eff_id, amount);
                    break;

                case sEffect::Enjoy:            // affects enjoyment
                    girl.upd_temp_Enjoyment((Action_Types)eff_id, amount);
                    break;

                case sEffect::Trait:            // affects skill
                    break;
            }
        }
        else    // m_Special == sInventoryItem::None
        {
            /* */if (affects == sEffect::Skill)
            {
                // `J` food and makeup are single use items, so if permanent, make them affect the base skill
                if (is_consumed)
                    girl.upd_skill(eff_id, amount);
                // `J` all other items can be removed so use skill mod
                else girl.upd_mod_skill(eff_id, amount);
            }
            else if (affects == sEffect::Stat)
            {
                // `J` food and makeup are single use items, so if permanent, make them affect the base skill
                if (is_consumed)
                                  girl.upd_base_stat(eff_id, amount);
                // `J` all other items can be removed so use skill mod
                else girl.upd_mod_stat(eff_id, amount);
            }
            else if (affects == sEffect::Enjoy)
            {
                // `J` food and makeup are single use items, so if permanent, make them affect the base skill
                if (is_consumed)
                    girl.upd_Enjoyment((Action_Types)eff_id, amount);
                // `J` all other items can be removed so use skill mod
                else cGirls::UpdateEnjoymentMod(girl, eff_id, amount);
            }
            else if (affects == sEffect::GirlStatus)    // adds/removes status
            {
                if (usenewpreg && (eff_id == STATUS_PREGNANT || eff_id == STATUS_PREGNANT_BY_PLAYER || eff_id == STATUS_INSEMINATED))
                {
                    continue;    // `J` handled at the start
                }
                else    // all other GirlStatus
                {
                    if (amount >= 1)    // add status
                        girl.set_status((STATUS)eff_id);
                    else if (amount == 0)    // remove status
                        girl.remove_status((STATUS)eff_id);
                }
            }
            else if (affects == sEffect::Trait)    // trait
            {
                /// for equipable items, traits will be handled in the inventory equipment.
                if(!is_consumed) continue;

                string trait = item->m_Effects[i].m_Trait; // avoids repeat calls

                if (amount == 0)                // remove trait from equiping an item
                {
                    //SIN: I know it's kinda player's fault anyway, but given cost of medicines and difficulty to
                    //source player should get some kindness rep for this. Prob cheaper and easier to let them die
                    //and replace, but if we get here, player is looking out for staff.
                    if (trait == "AIDS" || trait == "Syphilis" || trait == "Herpes" || trait == "Chlamydia")
                    {
                        if (girl.is_slave())
                        {        //SIN: just protecting investment in property
                            if (trait == "AIDS") g_Game->player().evil(-4);
                            if (trait == "Syphilis") g_Game->player().evil(-3);
                            if (trait == "Herpes") g_Game->player().evil(-2);
                            if (trait == "Chlamydia") g_Game->player().evil(-1);
                        }
                        else   //SIN: a genuinely kind act to support staff
                        {
                            if (trait == "AIDS") g_Game->player().evil(-8);
                            if (trait == "Syphilis") g_Game->player().evil(-6);
                            if (trait == "Herpes") g_Game->player().evil(-4);
                            if (trait == "Chlamydia") g_Game->player().evil(-2);
                        }
                    }
                    girl.lose_trait(trait.c_str());
                }
                else if (amount == 1)            // add normal trait    from equiping an item
                {
                    if(duration > 0) {
                        girl.add_temporary_trait(trait.c_str(), duration);
                    } else {
                        girl.gain_trait(trait.c_str());
                    }
                }
            }
        }
    }

    // if consumable then remove from inventory
    if (is_consumed)
    {
        girl.remove_item(item);
    }
    else    // set it as equiped
        girl.inventory().add_to_equipment(item, girl);

    cGirls::CalculateGirlType(girl);
}

// ----- Load

static sInventoryItem* handle_element(tinyxml2::XMLElement& el)
{
    const char *pt;
    sInventoryItem* item = new sInventoryItem();
    item->m_Name = GetStringAttribute(el, "Name");
    if (pt = el.Attribute("Desc"))                        item->m_Desc = pt; else cout << "no desc attribute found" << endl;
    if (pt = el.Attribute("Type"))                        item->set_type(pt);
    item->m_Badness = el.IntAttribute("Badness", 0);
    item->m_GirlBuyChance = el.IntAttribute("GirlBuyChance", (item->m_Badness < 20) ? (100 - (item->m_Badness * 5)) : 0);
    if (pt = el.Attribute("Special"))                    item->set_special(pt);
    item->m_Cost = el.IntAttribute("Cost", 0);
    if (pt = el.Attribute("Rarity"))                    item->set_rarity(pt);
    item->m_Infinite = el.BoolAttribute("Infinite", false);

    auto crafting = el.FirstChildElement("Crafting");
    if(crafting) {
        item->m_Crafting.from_xml(*crafting);
    }

    do_effects(&el, item);
    //    do_tests(el, item);        //    `J` will be added in the future (hopefully)
    return item;
}

bool cInventory::LoadItemsXML(const string& filename)
{
    auto doc = LoadXMLDocument(filename);
    tinyxml2::XMLElement *root_el = doc->RootElement();
    for (auto& el : IterateChildElements(*root_el))
    {
        try {
            sInventoryItem* item = handle_element(el);
            if (item->m_Name.empty()) {
                g_LogFile.error("items","Item in '", filename, "' has no name!");
                continue;
            }
            g_LogFile.debug("items", "Loading item", *item);
            m_Items.push_back(item);
        } catch (std::runtime_error& error) {
            g_LogFile.error("items", "Could not load item from '", filename, "' (", el.GetLineNum(), "): ", error.what());
        }
    }
    return true;
}

sInventoryItem* cInventory::GetCraftableItem(sGirl& girl, JOBS job, int craft_points) {
    RandomSelector<sInventoryItem> selector;
    for(auto& item : m_Items) {
        if(item->m_Crafting.can_craft(girl, job, craft_points)) {
            // only consider items that need at least one third of the crafting points
            if(item->m_Crafting.craft_cost() < craft_points / 3)
                continue;

            selector.process(item, item->m_Crafting.weight());
        }
    }
    return selector.selection();
}

std::vector<sInventoryItem*> cInventory::GetCraftableItems(JOBS job)
{
    std::vector<sInventoryItem*> result;
    for(auto& item : m_Items) {
        if(item->m_Crafting.is_craftable_by(job)) {
            result.push_back(item);
        }
    }
    return std::move(result);
}

int cInventory::NumItemSlots(const sInventoryItem* item)
{
    switch (item->m_Type)
    {
    case sInventoryItem::Ring:
        return 8;
    case sInventoryItem::Dress:
    case sInventoryItem::Shoes:
    case sInventoryItem::Necklace:
    case sInventoryItem::Armor:
    case sInventoryItem::Underwear:
    case sInventoryItem::Hat:
    case sInventoryItem::Helmet:
    case sInventoryItem::Glasses:
    case sInventoryItem::Swimsuit:
    case sInventoryItem::Combatshoes:
    case sInventoryItem::Shield:
        return 1;
    case sInventoryItem::SmWeapon:    // doc: added missing enum
    case sInventoryItem::Weapon:
    case sInventoryItem::Armband:
        return 2;
    case sInventoryItem::Food:
    case sInventoryItem::Makeup:
    case sInventoryItem::Misc:
    default:
        return 999999;
    }
}

ostream& operator<<(ostream& os, sInventoryItem& it) {
    os << "Item: " << it.m_Name << endl;
    os << "Desc: " << it.m_Desc << endl;
    os << "Type: " << it.m_Type << endl;
    os << "Badness: " << int(it.m_Badness) << endl;
    os << "Special: " << it.m_Special << endl;
    os << "Cost: " << it.m_Cost << endl;
    os << "Rarity: " << it.m_Rarity << endl;
    os << "Infinite: " << (it.m_Infinite ? "True" : "False") << endl;
    for(auto & eff : it.m_Effects) {
        os << eff;
    }
    return os;
}

ostream& operator<<(ostream& os, sInventoryItem::Type& typ) {
    switch(typ) {
        case sInventoryItem::Ring:            return os << "Ring";
        case sInventoryItem::Dress:            return os << "Dress";
        case sInventoryItem::Underwear:        return os << "Underwear";
        case sInventoryItem::Shoes:            return os << "Shoes";
        case sInventoryItem::Food:            return os << "Food";
        case sInventoryItem::Necklace:        return os << "Necklace";
        case sInventoryItem::Weapon:        return os << "Weapon";
        case sInventoryItem::SmWeapon:        return os << "Small Weapon";
        case sInventoryItem::Makeup:        return os << "Makeup";
        case sInventoryItem::Armor:            return os << "Armor";
        case sInventoryItem::Misc:            return os << "Misc";
        case sInventoryItem::Armband:        return os << "Armband";
        case sInventoryItem::Hat:            return os << "Hat";
        case sInventoryItem::Helmet:        return os << "Helmet";
        case sInventoryItem::Glasses:        return os << "Glasses";
        case sInventoryItem::Swimsuit:        return os << "Swimsuit";
        case sInventoryItem::Combatshoes:    return os << "Combat Shoes";
        case sInventoryItem::Shield:        return os << "Shield";
        default:
            cerr << "Unexpected type value: " << int(typ) << endl;
            return os << "Error";
    }
}

ostream& operator<<(ostream& os, sInventoryItem::Rarity& r) {
    switch(r) {
        case sInventoryItem::Common:            return os << "Common";
        case sInventoryItem::Shop50:            return os << "Shops, 50%";
        case sInventoryItem::Shop25:            return os << "Shops, 25%";
        case sInventoryItem::Shop05:            return os << "Shops, 05%";
        case sInventoryItem::Catacomb15:        return os << "Catacombs, 15%";
        case sInventoryItem::Catacomb05:        return os << "Catacombs, 05%";
        case sInventoryItem::Catacomb01:        return os << "Catacombs, 01%";
        case sInventoryItem::ScriptOnly:        return os << "Scripted Only";
        case sInventoryItem::ScriptOrReward:    return os << "Scripts or Reward";
        default:    cerr << "error: unexpected rarity value: " << int(r) << endl;
            return os << "Error(" << int(r) << ")";
    }
}

ostream& operator<<(ostream& os, sInventoryItem::Special& spec) {
    switch(spec) {
        case sInventoryItem::None:                return os << "None";
        case sInventoryItem::AffectsAll:        return os << "AffectsAll";
        case sInventoryItem::Temporary:            return os << "Temporary";
        default:    cerr << "error: unexpected special value: " << int(spec) << endl;
            return os << "Error(" << int(spec) << ")";
    }
}

void sInventoryItem::set_type(const string& s) {
    if      (s == "Ring")                            { m_Type = Ring; }
    else if (s == "Dress")                            { m_Type = Dress; }
    else if (s == "Under Wear" || s == "Underwear")    { m_Type = Underwear; }
    else if (s == "Shoes")                            { m_Type = Shoes; }
    else if (s == "Food")                            { m_Type = Food; }
    else if (s == "Necklace")                        { m_Type = Necklace; }
    else if (s == "Weapon")                            { m_Type = Weapon; }
    else if (s == "Small Weapon")                    { m_Type = SmWeapon; }
    else if (s == "Makeup")                            { m_Type = Makeup; }
    else if (s == "Armor")                            { m_Type = Armor; }
    else if (s == "Misc")                            { m_Type = Misc; }
    else if (s == "Armband")                        { m_Type = Armband; }
    else if (s == "Hat")                            { m_Type = Hat; }
    else if (s == "Glasses")                        { m_Type = Glasses; }
    else if (s == "Swimsuit")                        { m_Type = Swimsuit; }
    else if (s == "Helmet")                            { m_Type = Helmet; }
    else if (s == "Shield")                            { m_Type = Shield; }
    else if (s == "Combat Shoes")                    { m_Type = Combatshoes; }
    else if (s == "CombatShoes")                    { m_Type = Combatshoes; }
    else { m_Type = Misc; cerr << "Error: unexpected item type: " << s << endl; }
}

void sInventoryItem::set_special(const string& s) {
    if      (s == "None")            { m_Special = None; }
    else if (s == "AffectsAll")        { m_Special = AffectsAll; }
    else if (s == "Temporary")        { m_Special = Temporary; }
    else { m_Special = None; cerr << "unexpected special string: '" << s << "'" << endl; }
}

void sInventoryItem::set_rarity(const string& s) {
    if      (s == "Common")            { m_Rarity = Common; }
    else if (s == "Shop50")            { m_Rarity = Shop50; }
    else if (s == "Shop25")            { m_Rarity = Shop25; }
    else if (s == "Shop05")            { m_Rarity = Shop05; }
    else if (s == "Catacomb15")        { m_Rarity = Catacomb15; }
    else if (s == "Catacomb05")        { m_Rarity = Catacomb05; }
    else if (s == "Catacomb01")        { m_Rarity = Catacomb01; }
    else if (s == "ScriptOnly")        { m_Rarity = ScriptOnly; }
    else if (s == "ScriptOrReward") { m_Rarity = ScriptOrReward; }
    else { cerr << "Error in set_rarity: unexpected value '" << s << "'" << endl; m_Rarity = Shop05; }    // what to do?
}

void CraftingData::from_xml(tinyxml2::XMLElement& element) {
    element.QueryIntAttribute("CraftPoints", &m_CraftPoints);
    element.QueryIntAttribute("ManaCost", &m_ManaCost);
    element.QueryIntAttribute("Weight", &m_Weight);

    for(auto job = element.FirstChildElement("Job"); job; job = job->NextSiblingElement("Job")) {
        auto job_name = job->Attribute("Name");
        if(job_name) {
            m_CraftableBy.insert(get_job_id(job_name));
        } else {
            g_LogFile.error("items", "No name given for <Job> tag.");
        }
    }

    for(auto skill = element.FirstChildElement("Skill"); skill; skill = skill->NextSiblingElement("Skill")) {
        auto skill_name = skill->Attribute("Name");
        if(skill_name) {
            m_SkillRequirements[get_skill_id(skill_name)] = skill->IntAttribute("Minimum", 0);
        }
    }

    for(auto stat = element.FirstChildElement("Stat"); stat; stat = stat->NextSiblingElement("Stat")) {
        auto stat_name = stat->Attribute("Name");
        if(stat_name) {
            m_StatsRequirements[get_stat_id(stat_name)] = stat->IntAttribute("Minimum", 0);
        }
    }
}

int CraftingData::craft_cost() const {
    return m_CraftPoints;
}

int CraftingData::mana_cost() const {
    return m_ManaCost;
}

bool CraftingData::can_craft(sGirl& girl, JOBS job, int craft_points) const {
    if(!is_craftable_by(job))
        return false;

    if(craft_points < m_CraftPoints)
        return false;

    if(girl.mana() < m_ManaCost)
        return false;

    for(auto& skill : m_SkillRequirements) {
        if(girl.get_skill(skill.first) < skill.second)
            return false;
    }

    for(auto& stat : m_StatsRequirements) {
        if(girl.get_stat(stat.first) < stat.second)
            return false;
    }

    return true;
}

bool CraftingData::is_craftable_by(JOBS job) const
{
    return m_CraftableBy.count(job) != 0;
}


int CraftingData::weight() const {
    if(m_Weight > 0)
        return m_Weight;
    return 1;
}
