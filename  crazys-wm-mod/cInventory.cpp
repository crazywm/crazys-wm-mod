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
#include "tinyxml.h"
#include "cInventory.h"
#include <iostream>
#include "src/buildings/cBrothel.h"
#include "cCustomers.h"
#include "src/Game.hpp"
#include "src/utils/streaming_random_selection.hpp"
#include "cTraits.h"
#include "CLog.h"
using namespace std;

extern cTraits g_Traits;
extern cRng g_Dice;
extern cConfig      cfg;

extern string stringtolowerj(string name);

// ----- Misc

cInventory::~cInventory()
{
	Free();
}

void cInventory::Free()
{
	for(auto & m_ShopItem : m_ShopItems) m_ShopItem = nullptr;
}

const char *sEffect::girl_status_name(unsigned int id)
{
	if (id < sGirl::max_statuses) return sGirl::status_names[id];
	g_LogFile.os() << "[sEffect::girl_status_name] Error: girl status id " << id << " too large (max is " << sGirl::max_statuses << ")" << endl;
	return "";
}
const char *sEffect::skill_name(unsigned int id)
{
	if (id < sGirl::max_skills) return sGirl::skill_names[id];
	g_LogFile.os() << "[sEffect::skill_name] Error: skill id " << id << " too large (max is " << sGirl::max_skills << ")" << endl;
	return "";
}
const char *sEffect::stat_name(unsigned int id)
{
	if (id < sGirl::max_stats) return sGirl::stat_names[id];
	g_LogFile.os() << "[sEffect::stat_name] Error: stat id " << id << " too large (max is " << sGirl::max_stats << ")" << endl;
	return "";
}
const char *sEffect::enjoy_name(unsigned int id)
{
	if (id < sGirl::max_stats) return sGirl::enjoy_names[id];
	g_LogFile.os() << "[sEffect::enjoy_names] Error: enjoy id " << id << " too large (max is " << sGirl::max_enjoy << ")" << endl;
	return "";
}

bool sEffect::set_skill(string s)
{
	int nID = sGirl::lookup_skill_code(s);
	if (nID == -1)		// ERROR
	{
		g_LogFile.os() << "[sEffect::set_skill] Error: unknown Skill: " << s << ". Skill ID: " << nID << endl;
		return false;
	}
	m_EffectID = nID;
	return true;
}
bool sEffect::set_girl_status(string s)
{
	int nID = sGirl::lookup_status_code(s);
	if (nID == -1)		// ERROR
	{
		g_LogFile.os() << "[sEffect::lookup_status_code] Error: unknown Status: " << s << ". Status ID: " << nID << endl;
		return false;
	}
	m_EffectID = nID;
	return true;
}
bool sEffect::set_stat(string s)
{
	int nID = sGirl::lookup_stat_code(s);
	if (nID == -1)		// ERROR
	{
		g_LogFile.os() << "[sEffect::set_stat] Error: unknown Stat: " << s << ". Stat ID: " << nID << endl;
		return false;
	}
	m_EffectID = nID;
	return true;
}
bool sEffect::set_Enjoyment(string s)
{
	int nID = sGirl::lookup_enjoy_code(s);
	if (nID == -1)		// ERROR
	{
		g_LogFile.os() << "[sEffect::set_enjoy] Error: unknown Enjoy: " << s << ". Enjoy ID: " << nID << endl;
		return false;
	}
	m_EffectID = nID;
	return true;
}

void cInventory::GivePlayerAllItems()
{
	sInventoryItem* item;
	for (auto & m_Item : m_Items)
	{
		item = m_Item;
		g_Game.player().inventory().add_item(item, 999);
	}
}

/*
 * should make this part of the cInventoryItem
 */
static void do_effects(TiXmlElement *parent, sInventoryItem *item)
{
	int ival; const char *pt; TiXmlElement *el;
	for (el = parent->FirstChildElement(); el; el = el->NextSiblingElement())
	{
		sEffect *ept = new sEffect;
		if (pt = el->Attribute("What")) ept->set_what(pt);
		if (pt = el->Attribute("Name"))
		{
			switch (ept->m_Affects)
			{
			case sEffect::Trait:
				ept->m_Trait = pt;
				break;
			case sEffect::Stat:
				if (ept->set_stat(pt) == false)
				{
					g_LogFile.os() << "effect type code == " << ept->m_Affects;
					g_LogFile.os() << " stat lookup failed for " << item->m_Name << endl;
				}
				break;
			case sEffect::GirlStatus:
				if (!ept->set_girl_status(pt)) g_LogFile.os() << " Error in item " << item->m_Name << endl;
				break;
			case sEffect::Skill:
				if (!ept->set_skill(pt)) g_LogFile.os() << " Error in item " << item->m_Name << endl;
				break;
			case sEffect::Enjoy:
				if (!ept->set_Enjoyment(pt)) g_LogFile.os() << " Error in item " << item->m_Name << endl;
				break;
			default:
				g_LogFile.os() << " can't handle effect type " << ept->m_Affects << endl;
				break;
			}
		}
		if (pt = el->Attribute("Amount", &ival)) ept->m_Amount = ival;

		if (pt = el->Attribute("Duration", &ival)) ept->m_Duration = ival;
		else ept->m_Duration = (item->m_Special == 2 ? 20 : 0);

		item->m_Effects.push_back(*ept);
	}
}
static void do_tests(TiXmlElement *parent, sInventoryItem *item)	// `J` added
{// `J` This is to be for xml item tests for if the girl can auto use the item
	/*  `J` copied from do_effects as the base
	int ival;
	const char *pt;
	TiXmlElement *el;
	for (el = parent->FirstChildElement(); el; el = el->NextSiblingElement()) {
		sEffect *ept = new sEffect;

		if ((pt = el->Attribute("What")))
			ept->set_what(pt);

		if ((pt = el->Attribute("Name"))) {
			switch (ept->m_Affects) {
			case sEffect::Trait:
				ept->m_Trait = pt;
				break;
			case sEffect::Stat:
				if (ept->set_stat(pt) == false) {
					g_LogFile.os() << "effect type code == " << ept->m_Affects;
					g_LogFile.os() << " stat lookup failed for " << item->m_Name << endl;

				}
				break;
			case sEffect::GirlStatus:
				ept->set_girl_status(pt);
				break;
			case sEffect::Skill:
				ept->set_skill(pt);
				break;
			default:
				g_LogFile.os() << " can't handle effect type " << ept->m_Affects << endl;
			}
		}

		if ((pt = el->Attribute("Amount", &ival)))
			ept->m_Amount = ival;

		item->m_Effects.push_back(*ept);
	}
	*/
}

void cInventory::AddItem(sInventoryItem* item)
{
	m_Items.push_back(item);
}
void cInventory::remove_trait(sGirl* girl, int num, int index)
{
	u_int item_type = girl->m_Inventory[num]->m_Type;
	string trait_name = girl->m_Inventory[num]->m_Effects[index].m_Trait;

	/*
	 *	WD:	New logic for remembering traits
	 *		moved to AddTrait() RemoveTrait() fn's
	 *
	 *		Don't think this is called any more.
	 *
	 */
	if (trait_name == "Virgin") girl->m_Virgin = 0;

	girl->remove_trait(trait_name,					// Remove Traits
		item_type != INVFOOD && item_type != INVMAKEUP);	// Remember if not consumable
}
bool cInventory::GirlBuyItem(sGirl* girl, int ShopItem, int MaxItems, bool AutoEquip)
{
	// girl buys selected item if possible; returns true if bought
	sInventoryItem* item = GetShopItem(ShopItem);
	if (cGirls::GetNumItemType(girl, item->m_Type) >= MaxItems)
	{
		// if she has enough of this type, she won't buy more unless it's better than what she has
		int nicerThan = cGirls::GetWorseItem(girl, (int)item->m_Type, item->m_Cost);
		if (nicerThan != -1)
		{
			// found a worse item of the same type in her inventory
			cGirls::SellInvItem(girl, nicerThan);
			girl->m_Money -= item->m_Cost;
			int temp = girl->add_inv(item);
			if (temp != -1 && AutoEquip) Equip(girl, temp, false); // MYR: Check temp value
			return true;
		}
	}
	else
	{
		girl->m_Money -= item->m_Cost;
		int temp = girl->add_inv(item);
		if (temp != -1 && AutoEquip) Equip(girl, temp, false);	// MYR: Check temp value
		return true;
	}
	return false;
}
void cInventory::CalculateCost(sInventoryItem* newItem)
{
	for (u_int i = 0; i < newItem->m_Effects.size(); i++)
	{
		if (newItem->m_Effects[i].m_Affects == sEffect::Stat)	// stats
		{
			if (newItem->m_Effects[i].m_Amount >= 0)
			{
				// bad stats increases
				if (newItem->m_Effects[i].m_EffectID == STAT_TIREDNESS || newItem->m_Effects[i].m_EffectID == STAT_PCHATE)
					newItem->m_Cost += newItem->m_Effects[i].m_Amount * 2;
				else	// good stats increases
				{
					newItem->m_Cost += newItem->m_Effects[i].m_Amount * 10;
				}
				// make items that do more cost a little less (buy in bulk)
				newItem->m_Cost -= (newItem->m_Effects[i].m_Amount / 5) * 5;
			}
			else
			{
				// bad stats decreases
				if (newItem->m_Effects[i].m_EffectID == STAT_TIREDNESS || newItem->m_Effects[i].m_EffectID == STAT_PCHATE)
					newItem->m_Cost += newItem->m_Effects[i].m_Amount * 10;
				else	// good stats decreases
				{
					newItem->m_Cost += newItem->m_Effects[i].m_Amount * 2;
				}
				// make items that do heaps of stuff cost a little less
				newItem->m_Cost -= (newItem->m_Effects[i].m_Amount / 5) * 5;
			}
		}
		if (newItem->m_Effects[i].m_Affects == sEffect::Skill)	// skills
		{
			if (newItem->m_Effects[i].m_Amount >= 0)	// increases skill
				newItem->m_Cost += newItem->m_Effects[i].m_Amount * 15;
			else	// decreases skill
				newItem->m_Cost += newItem->m_Effects[i].m_Amount * 2;

			// make items that do heaps of stuff cost a little less
			newItem->m_Cost -= (newItem->m_Effects[i].m_Amount / 5) * 5;
		}
		if (newItem->m_Effects[i].m_Affects == sEffect::Enjoy)	// skills
		{
			if (newItem->m_Effects[i].m_Amount >= 0)	// increases enjoyment
				newItem->m_Cost += newItem->m_Effects[i].m_Amount * 15;
			else	// decreases enjoyment
				newItem->m_Cost += newItem->m_Effects[i].m_Amount * 2;

			// make items that do heaps of stuff cost a little less
			newItem->m_Cost -= (newItem->m_Effects[i].m_Amount / 5) * 5;
		}
		if (newItem->m_Effects[i].m_Affects == sEffect::GirlStatus)	// status
		{
			if (newItem->m_Effects[i].m_Amount == 1)	// adds status
			{
				/* */if (newItem->m_Effects[i].m_EffectID == STATUS_POISONED)				newItem->m_Cost -= 20;
				else if (newItem->m_Effects[i].m_EffectID == STATUS_BADLY_POISONED)			newItem->m_Cost -= 50;
				else if (newItem->m_Effects[i].m_EffectID == STATUS_PREGNANT)				newItem->m_Cost += 30;
				else if (newItem->m_Effects[i].m_EffectID == STATUS_PREGNANT_BY_PLAYER)		newItem->m_Cost += 30;
				else if (newItem->m_Effects[i].m_EffectID == STATUS_SLAVE)					newItem->m_Cost += 400;
				else if (newItem->m_Effects[i].m_EffectID == STATUS_HAS_DAUGHTER)			newItem->m_Cost += 200;
				else if (newItem->m_Effects[i].m_EffectID == STATUS_HAS_SON)				newItem->m_Cost += 200;
				else if (newItem->m_Effects[i].m_EffectID == STATUS_INSEMINATED)			newItem->m_Cost += 30;
				else if (newItem->m_Effects[i].m_EffectID == STATUS_CONTROLLED)				newItem->m_Cost += 900;
				else if (newItem->m_Effects[i].m_EffectID == STATUS_CATACOMBS)				newItem->m_Cost += 90;
				else if (newItem->m_Effects[i].m_EffectID == STATUS_ARENA)					newItem->m_Cost += 90;
				else if (newItem->m_Effects[i].m_EffectID == STATUS_YOURDAUGHTER)			newItem->m_Cost += 900;
			}
			else	// removes status
			{
				/* */if (newItem->m_Effects[i].m_EffectID == STATUS_POISONED)				newItem->m_Cost += 100;
				else if (newItem->m_Effects[i].m_EffectID == STATUS_BADLY_POISONED)			newItem->m_Cost += 200;
				else if (newItem->m_Effects[i].m_EffectID == STATUS_PREGNANT)				newItem->m_Cost += 10;
				else if (newItem->m_Effects[i].m_EffectID == STATUS_PREGNANT_BY_PLAYER)		newItem->m_Cost += 10;
				else if (newItem->m_Effects[i].m_EffectID == STATUS_SLAVE)					newItem->m_Cost += 70;
				else if (newItem->m_Effects[i].m_EffectID == STATUS_HAS_DAUGHTER)			newItem->m_Cost += 20;
				else if (newItem->m_Effects[i].m_EffectID == STATUS_HAS_SON)				newItem->m_Cost += 20;
				else if (newItem->m_Effects[i].m_EffectID == STATUS_INSEMINATED)			newItem->m_Cost += 10;
				else if (newItem->m_Effects[i].m_EffectID == STATUS_CONTROLLED)				newItem->m_Cost += 20;
				else if (newItem->m_Effects[i].m_EffectID == STATUS_CATACOMBS)				newItem->m_Cost += 20;
				else if (newItem->m_Effects[i].m_EffectID == STATUS_ARENA)					newItem->m_Cost += 20;
				else if (newItem->m_Effects[i].m_EffectID == STATUS_YOURDAUGHTER)			newItem->m_Cost += 200;
			}
		}
		if (newItem->m_Effects[i].m_Affects == sEffect::Trait)	newItem->m_Cost += 500;	// traits
	}
	if (!newItem->m_Effects.empty())		newItem->m_Cost += newItem->m_Effects.size() * 5;
	if (newItem->m_Rarity > 0)				newItem->m_Cost += newItem->m_Rarity * 5;
	if (newItem->m_Special == 1)			newItem->m_Cost += 2000;
	else if (newItem->m_Special == 2)		newItem->m_Cost += 100;
	if (newItem->m_Cost <= 10)				newItem->m_Cost = 10;
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
	case sEffect::Skill:		return os << "Skill";
	case sEffect::Stat:			return os << "Stat";
	case sEffect::Nothing:		return os << "Nothing";
	case sEffect::GirlStatus:	return os << "GirlStatus";
	case sEffect::Trait:		return os << "Trait";
	case sEffect::Enjoy:		return os << "Enjoy";
	default:
		g_LogFile.os() << "error: unexpected 'what' value: " << int(w) << endl;
		return os << "Error(" << int(w) << ")";
	}
}

void sEffect::set_what(string s) {
    if		(s == "Skill")		m_Affects = Skill;
    else if	(s == "Stat")		m_Affects = Stat;
    else if	(s == "Nothing")	m_Affects = Nothing;
    else if	(s == "GirlStatus")	m_Affects = GirlStatus;
    else if (s == "Trait")		m_Affects = Trait;
    else if (s == "Enjoy")		m_Affects = Enjoy;
    else {
        m_Affects = Nothing;
        cerr << "Error: Bad 'what' string for item effect: '" << s << "'" << endl;
    }
}

ostream& operator<<(ostream& os, sEffect& eff) {
    os << "Effect: " << eff.m_Affects << " ";
    if (eff.m_Affects == sEffect::Stat) { os << eff.stat_name(eff.m_EffectID); }
    if (eff.m_Affects == sEffect::Skill) { os << eff.skill_name(eff.m_EffectID); }
    if (eff.m_Affects == sEffect::Trait) { os << "'" << eff.m_Trait << "'"; }
    if (eff.m_Affects == sEffect::GirlStatus) { os << eff.girl_status_name(eff.m_EffectID); }
    if (eff.m_Affects == sEffect::Enjoy) { os << eff.enjoy_name(eff.m_EffectID); }
    os << (eff.m_Amount > 0 ? " +" : " ") << eff.m_Amount;
    return os << endl;
}

// ----- Shop
sInventoryItem* cInventory::BuyShopItem(int num)
{
	if (num >= NUM_SHOPITEMS) return nullptr;
	sInventoryItem* item = m_ShopItems[num];
	if (item->m_Infinite == 0)
	{
		m_ShopItems[num] = nullptr;
		m_NumShopItems--;
	}
	return item;
}

int cInventory::CheckShopItem(string name)
{
	int num = -1;
	for (int i = 0; i < NUM_SHOPITEMS; i++)
	{
		if (m_ShopItems[i])
		{
			if (m_ShopItems[i]->m_Name == name) num = i;
		}
	}
	return num;
}
void cInventory::UpdateShop()
{
	for (int i = 0; i < NUM_SHOPITEMS; i++)
	{
		sInventoryItem* item = GetRandomItem();
		while (item == nullptr) item = GetRandomItem();
		if (item == nullptr) break;	// should never happen but left in anyway

		int chance = g_Dice.d100();
		if ((item->m_Rarity == RARITYCOMMON ||
			(item->m_Rarity == RARITYSHOP50 && chance <= 50) ||
			(item->m_Rarity == RARITYSHOP25 && chance <= 25) ||
			(item->m_Rarity == RARITYSHOP05 && chance <= 5)) &&
			(item->m_Infinite == 0 || (item->m_Infinite == 1 && CheckShopItem(item->m_Name) == -1)))
		{
			m_ShopItems[i] = item;
			m_NumShopItems++;
			continue;
		}
		else
		{
			i--;
			continue;
		}
	}
}
sInventoryItem* cInventory::GetShopItem(int num)
{
	if (m_NumShopItems == 0) UpdateShop();
	if (num >= NUM_SHOPITEMS) return nullptr;
	return m_ShopItems[num];
}
int cInventory::GetRandomShopItem()
{
	if (m_NumShopItems == 0) UpdateShop();
	int num = g_Dice%NUM_SHOPITEMS;
	while (m_ShopItems[num] == nullptr) num = g_Dice%NUM_SHOPITEMS;
	if (num > NUM_SHOPITEMS - 1) num = NUM_SHOPITEMS - 1;   // shouldn't be necessary, but once I got 40 back causing OOB elsewhere
	return num;
}

// ----- Get
sInventoryItem* cInventory::GetRandomItem()
{
	sInventoryItem *ipt;
	int log = 0; if (cfg.debug.log_items()) log = (cfg.debug.log_extradetails()) ? 2 : 1;
	if (log > 0) g_LogFile.os() << "cInventory::GetRandomItem: " << "items.size == " << m_Items.size() << endl;
	if (m_Items.empty())
	{
		if (log > 1) g_LogFile.os() << "	returning null" << endl;
		return nullptr;
	}
	if (m_Items.size() == 1)
	{
		ipt = m_Items[0];
		if (log > 1) g_LogFile.os() << "	returning 0x" << hex << long(ipt) << endl;
		return m_Items[0];
	}
	int index = g_Dice % (m_Items.size() - 1);	// fixed crash with going outside vector size - necro
	if (log > 1) g_LogFile.os() << "	returning item at index " << index << endl;
	ipt = m_Items[index];
	if (log > 1) g_LogFile.os() << "	returning 0x" << hex << long(ipt) << dec << endl;
	return ipt;
}
sInventoryItem* cInventory::GetRandomCatacombItem()
{
	if (m_Items.empty())	return nullptr;
	sInventoryItem *temp = nullptr;
	int index = g_Dice % (m_Items.size() - 1);

	int tries = m_Items.size() / 3;	// try 1/3 of all items to get an item
	while (tries > 0)
	{
		if (tries % 10 == 0) index = g_Dice % (m_Items.size() - 1);
		if (index >= (int)m_Items.size()) index = 0;
		temp = m_Items[index];
		switch (temp->m_Rarity) {
		case RARITYSHOP25:									return temp;	break;
		case RARITYSHOP05:		if (g_Dice.percent(25))		return temp;	break;
		case RARITYCATACOMB15:	if (g_Dice.percent(15))		return temp;	break;
		case RARITYCATACOMB05:	if (g_Dice.percent(5))		return temp;	break;
		case RARITYCATACOMB01:	if (g_Dice.percent(1))		return temp;	break;
		case RARITYSCRIPTONLY:
		case RARITYSCRIPTORREWARD:
			temp = nullptr;
			break;	// if at the end it is a script item, no item is returned
		case RARITYCOMMON:
		case RARITYSHOP50:
		default:
			break;	// if at the end it is a common item, that item is returned
		}
		tries--;
		index++;
	}
	if (!temp) return nullptr;
	return temp;
}

sInventoryItem* cInventory::GetItem(string name)
{
	sInventoryItem* item;
	for (u_int i = 0; i < m_Items.size(); i++)
	{
		item = m_Items[i];
		if (item->m_Name == name) return item;
	}
	return nullptr;
}

// ----- Equip unequip

void cInventory::Equip(sGirl* girl, int num, bool force)
{
	if (num == -1) return;	// no item then ignore the call

	if (girl->is_dead() && (	// Allow certain Items to recover the dead
		stringtolowerj(girl->m_Inventory[num]->m_Name) == stringtolowerj("Better Zed than Dead") ||
		stringtolowerj(girl->m_Inventory[num]->m_Name) == stringtolowerj("Elixir of Ultimate Regeneration")
		)){}
	// A few items are hard coded
	else if (stringtolowerj(girl->m_Inventory[num]->m_Name) == stringtolowerj("Reset Potion MK i"))
	{
		int age = girl->age();
		// reset all numbers to default
		for (u_int i = 0; i < NUM_SKILLS; i++)	girl->m_Skills[i] = 0;
		for (int i = 0; i < NUM_STATS; i++)		girl->m_Stats[i] = 0;
		girl->m_Stats[STAT_HEALTH]		= 100;
		girl->m_Stats[STAT_HAPPINESS]	= 100;
		girl->m_Stats[STAT_AGE] = (age == 100 ? 100 : 18);	// keep ageless girls ageless	// `J` Legal Note: 18 is the Legal Age of Majority for the USA where I live
		girl->m_Stats[STAT_HOUSE] = girl->is_slave() ? cfg.initial.slave_house_perc() : cfg.initial.girls_house_perc();

		g_Game.push_message(girl->m_Realname + ": " + girl->m_Inventory[num]->m_Name +
			": The use of this item has reset all her stats and skills to default.", COLOR_BLUE);
		girl->m_Inventory[num] = nullptr;
		girl->m_EquipedItems[num] = 0;
		girl->m_NumInventory--;
		cGirls::CalculateGirlType(girl);
		return;
	}
	else if (stringtolowerj(girl->m_Inventory[num]->m_Name) == stringtolowerj("Reset Potion MK ii"))
	{
		// remove all traits
		girl->m_Stats[STAT_HEALTH] = 100;	// `J` revive dead girls
		for (int i = 0; i < MAXNUM_TRAITS; i++)
		{
			if (girl->m_Traits[i]) girl->remove_trait(girl->m_Traits[i]->name(), false, true, false);
		}
		cGirls::RemoveAllRememberedTraits(girl);

		g_Game.push_message(girl->m_Realname + ": " + girl->m_Inventory[num]->m_Name +
			": The use of this item has removed all her traits.", COLOR_BLUE);
		girl->m_Inventory[num] = nullptr;
		girl->m_EquipedItems[num] = 0;
		girl->m_NumInventory--;
		cGirls::ApplyTraits(girl);
		cGirls::CalculateGirlType(girl);
		return;
	}
	else if (girl->is_dead() ||					// dead girls shouldn't be able to equip or use anything
		girl->m_EquipedItems[num] == 1 ||		// if already equiped do nothing
		girl->m_Inventory[num]->m_Special == sInventoryItem::AffectsAll || // no "AffectsAll" item should be equipable
		ok_2_equip(girl, num, force) == false)	// of if it is not ok to equip it
		return;

	bool usenewpreg = false;
#if 1		// `J` Pregnancy item fix
	usenewpreg = true;
	stringstream pregmsg;
	bool pregbefore = girl->is_pregnant();
	bool pregafter	= girl->is_pregnant();
	int pregadd = 0;	int pregend = 0;
	int pregbyC = 0;	int pregrmC = 0;
	int pregbyP = 0;	int pregrmP = 0;
	int pregbyB = 0;	int pregrmB = 0;

	for (auto effect : girl->m_Inventory[num]->m_Effects)
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
				if (eff_id == STATUS_PREGNANT)				pregbyC++;
				if (eff_id == STATUS_PREGNANT_BY_PLAYER)	pregbyP++;
				if (eff_id == STATUS_INSEMINATED)			pregbyB++;
			}
			if (amount == 0)
			{
				pregend++;
				if (eff_id == STATUS_PREGNANT)				pregrmC++;
				if (eff_id == STATUS_PREGNANT_BY_PLAYER)	pregrmP++;
				if (eff_id == STATUS_INSEMINATED)			pregrmB++;
			}
		}
	}
	if (pregadd + pregend > 0)	// if the item changes pregnancy
	{
		int badchance = 0;
		int goodchance = 0;
		bool endpreg = false;
		bool startpreg = false;
		int type = STATUS_PREGNANT;
		pregmsg << girl->m_Realname << " has used her " << girl->m_Inventory[num]->m_Name << ".\n";

		if (pregadd == 0)		// end only
		{
			if (pregbefore)		// can end normally
			{
				if (((girl->m_States&(1 << STATUS_PREGNANT) || girl->m_States&(1 << STATUS_PREGNANT_BY_PLAYER))	// abort human
					&& pregrmC + pregrmP > 0) || (girl->m_States&(1 << STATUS_INSEMINATED) && pregrmB > 0))		// abort beast
				{
					endpreg = true;	// the item can do what is intended
				}
				else	// if made to abort human only but trying on beast or beast on human - can be bad
				{
					badchance += pregend * 5;
					pregmsg << "It was not intended to end "
						<< (girl->m_States&(1 << STATUS_INSEMINATED) ? "beast " : "human ") << "pregnancies ";
					if (g_Dice.percent(min(80, pregend * 10)))	// may still abort
					{
						endpreg = true;
						pregmsg << "but it still worked.";
						badchance *= 4;
					}
					else pregmsg << "so it made her sick.";
				}
			}
			else				// not pregnant so possible bad effects
			{
				pregmsg << "She was not pregnant before she used it so it made her really sick.";
				badchance += pregend * 10;
			}
		}
		else if (pregend == 0)	// add only
		{
			if (!pregbefore)	// can add normally
			{
				startpreg = true;
				if (pregbyC == 0 && pregbyP == 0)					// add beast
				{
					type = STATUS_INSEMINATED;
				}
				else if (pregbyC == 0 && pregbyB == 0)				// add Player
				{
					type = STATUS_PREGNANT_BY_PLAYER;
				}
				else if (pregbyP == 0 && pregbyB == 0)				// add Customer
				{
					type = STATUS_PREGNANT;
				}
				else if (pregbyP > 0 && pregbyC > 0 && pregbyB > 0)	// tries to add all three
				{
					if (pregbyP == pregbyC && pregbyP == pregbyB)	// all are equal
					{
						int roll = g_Dice % 100;
						/* */if (roll < 33)	type = STATUS_PREGNANT;
						else if (roll < 66)	type = STATUS_PREGNANT_BY_PLAYER;
						else/*           */	type = STATUS_INSEMINATED;			// slightly higher chance for beast
					}
					else if (pregbyP > pregbyC && pregbyP > pregbyB)	type = STATUS_PREGNANT_BY_PLAYER;	// Player is highest
					else if (pregbyC > pregbyP && pregbyC > pregbyB)	type = STATUS_PREGNANT;				// Cust is highest
					else if (pregbyB > pregbyC && pregbyB > pregbyP)	type = STATUS_INSEMINATED;			// Beast is highest
					else
					{
						int roll = g_Dice % (pregbyP + pregbyC + pregbyB);
						/* */if (roll < pregbyC)/*      */	type = STATUS_PREGNANT;
						else if (roll < pregbyP + pregbyC)	type = STATUS_PREGNANT_BY_PLAYER;
						else/*                          */	type = STATUS_INSEMINATED;
					}
				}

			}
			else				// already pregnant
			{
				if (girl->m_States&(1 << STATUS_PREGNANT) || girl->m_States&(1 << STATUS_PREGNANT_BY_PLAYER))
				{
					pregmsg << "She is already pregnant";
					if (girl->m_States&(1 << STATUS_PREGNANT_BY_PLAYER))
						pregmsg << " with your child";
					if (pregbyB == 0)	// human only
					{
						pregmsg << " so the new pregnancy did not take hold.";
					}
					else				// possible beast insemination
					{
						double chance = 10.0;
						if (pregbyC + pregbyP > 0) chance /= pregbyC + pregbyP;
						if (chance < 1.0) chance = 1.0;
						if (g_Dice.percent(pregbyB * chance))
						{
							pregmsg << " but the insemination fluid consumed the human baby and took over the pregnancy.";
							girl->clear_pregnancy();
							type = STATUS_INSEMINATED;
							startpreg = true;
							badchance += pregbyB * 5;
						}
						else pregmsg << " so the new pregnancy did not take hold.";
					}

				}
				else			// already inseminated
				{
					pregmsg << "She is already carrying some type of beast in her";
					if (girl->m_WeeksPreg > cfg.pregnancy.weeks_monster_p() / 2 && g_Dice.percent(50))	// more than halfway through pregnancy
					{
						pregmsg << " and it fed on what the item tried to implant.";
						// zzzzzz - Add in the effects
						pregmsg << " (not implemented yet).";

					}
					else if (girl->m_WeeksPreg < 3 && g_Dice.percent(cfg.pregnancy.multi_birth_chance()*pregadd * 2))	// fairly new pregnancy
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
		else					// some combination of add and end
		{
#if 0
			if (pregbefore)		// end old pregnancy to start a new one?
			{
				if (girl->m_States&(1 << STATUS_PREGNANT))
				{

				}
				else if (girl->m_States&(1 << STATUS_PREGNANT_BY_PLAYER))
				{

				}
				else	// girl->m_States&(1 << STATUS_INSEMINATED)
				{

				}

//				pregbyC				pregbyP				pregbyB
//				pregrmC				pregrmP				pregrmB



			}
			else				// or just start a new one
			{

			}
#else
			pregmsg << "(Add AND Remove pregnancy. Details not implemented yet)\n";
			endpreg = true;
			startpreg = true;
			int roll = g_Dice % (pregbyP + pregbyC + pregbyB);
			/* */if (roll < pregbyC)/*      */	type = STATUS_PREGNANT;
			else if (roll < pregbyP + pregbyC)	type = STATUS_PREGNANT_BY_PLAYER;
			else/*                          */	type = STATUS_INSEMINATED;
#endif


		}

		if (endpreg)
		{
			girl->m_ChildrenCount[CHILD09_ABORTIONS]++;
			pregmsg << "\nShe is no longer ";
			if (girl->m_States&(1 << STATUS_PREGNANT_BY_PLAYER))
			{
				pregmsg << "pregnant with your child.";
			}
			else if (girl->m_States&(1 << STATUS_INSEMINATED))
			{
				pregmsg << "inseminated.";
			}
			else
			{
				pregmsg << "pregnant.";
			}
			girl->clear_pregnancy();
		}
		if (startpreg)
		{
			int numchildren = 1 + g_Dice % pregbyP;
			if (type == STATUS_PREGNANT_BY_PLAYER)
			{
				cGirls::CreatePregnancy(girl, numchildren, type, g_Game.player().m_Stats, g_Game.player().m_Skills);
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
				cGirls::CreatePregnancy(girl, numchildren, type, Cust.m_Stats, Cust.m_Skills);
			}
		}


		if (stringtolowerj(girl->m_Inventory[num]->m_Name) == stringtolowerj("Better Zed than Dead"))
		{
			// `J` a few specific items don't need the rest of the code
		}
		else
		{
			pregafter = girl->is_pregnant();
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
				g_Game.push_message(pregmsg.str(), COLOR_RED);
			}

		}
	}




#endif

	// apply the effects
	for (u_int i = 0; i < girl->m_Inventory[num]->m_Effects.size(); i++)
	{
		int affects = girl->m_Inventory[num]->m_Effects[i].m_Affects;
		int eff_id = girl->m_Inventory[num]->m_Effects[i].m_EffectID;
		int amount = girl->m_Inventory[num]->m_Effects[i].m_Amount;
		int duration = girl->m_Inventory[num]->m_Effects[i].m_Duration;
		bool tempeff = girl->m_Inventory[num]->m_Special == sInventoryItem::Temporary;

		if (tempeff)
		{
			switch (affects)
			{
			case sEffect::Nothing:
				break;
			case sEffect::GirlStatus:

				if (usenewpreg && (eff_id == STATUS_PREGNANT || eff_id == STATUS_PREGNANT_BY_PLAYER || eff_id == STATUS_INSEMINATED))
				{
					break;	// `J` handled at the start
				}

				/*
				 *				I can understand ignoring "Nothing"
				 *				Ought "GirlStatus" to have an effect?
				 *				Seem to remember this cropping up on the forum
	"None",		"Poisoned",		"Badly Poisoned",
	"Slave",		"Has Daughter",	"Has Son", "Controlled",
	"Catacombs",	"Arena",		"Your Daughter"
				 */
				break;

			case sEffect::Skill:			// affects skill
				girl->upd_temp_skill(eff_id, amount);
				break;

			case sEffect::Stat:			// affects skill
				girl->upd_temp_stat(eff_id, amount);
				break;

			case sEffect::Enjoy:			// affects enjoyment
				girl->upd_temp_Enjoyment(eff_id, amount);
				break;

			case sEffect::Trait:			// affects skill
				/*
				 *	WD:	New logic for remembering traits
				 *		moved to AddTrait() RemoveTrait() fn's
				 *
				 *		EQUIP Temporary Item
				 */
				if (amount == 0)						// remove trait temporarily from equiping an item
					girl->remove_trait(girl->m_Inventory[num]->m_Effects[i].m_Trait, true);		// addrememberlist = true Temporary Item trait removal

				else if (amount == 1)		// add temporary trait
					girl->add_trait(girl->m_Inventory[num]->m_Effects[i].m_Trait, duration, true);	// Temp = true Temporary Item, removeitem = true for Temporary Item trait addition

				if (girl->m_Inventory[num]->m_Effects[i].m_Trait == "Virgin")
				{
					girl->m_Virgin = (amount == 1);
				}
			}
		}
		else	// m_Special == sInventoryItem::None
		{
			/* */if (affects == sEffect::Skill)
			{
				// `J` food and makeup are single use items, so if permanent, make them affect the base skill
				if (girl->m_Inventory[num]->m_Type == INVFOOD || girl->m_Inventory[num]->m_Type == INVMAKEUP)
					girl->upd_skill(eff_id, amount);
				// `J` all other items can be removed so use skill mod
				else cGirls::UpdateSkillMod(girl, eff_id, amount);
			}
			else if (affects == sEffect::Stat)
			{
				// `J` food and makeup are single use items, so if permanent, make them affect the base skill
				if (girl->m_Inventory[num]->m_Type == INVFOOD || girl->m_Inventory[num]->m_Type == INVMAKEUP)
					girl->upd_stat(eff_id, amount);
				// `J` all other items can be removed so use skill mod
				else cGirls::UpdateStatMod(girl, eff_id, amount);
			}
			else if (affects == sEffect::Enjoy)
			{
				// `J` food and makeup are single use items, so if permanent, make them affect the base skill
				if (girl->m_Inventory[num]->m_Type == INVFOOD || girl->m_Inventory[num]->m_Type == INVMAKEUP)
					girl->upd_Enjoyment(eff_id, amount);
				// `J` all other items can be removed so use skill mod
				else cGirls::UpdateEnjoymentMod(girl, eff_id, amount);
			}
			else if (affects == sEffect::GirlStatus)	// adds/removes status
			{
				if (usenewpreg && (eff_id == STATUS_PREGNANT || eff_id == STATUS_PREGNANT_BY_PLAYER || eff_id == STATUS_INSEMINATED))
				{
					continue;	// `J` handled at the start
				}
				else	// all other GirlStatus
				{
					if (amount >= 1)	// add status
						girl->m_States |= (1 << eff_id);
					else if (amount == 0)	// remove status
						girl->m_States &= ~(1 << eff_id);
				}
			}
			else if (affects == sEffect::Trait)	// trait
			{
				/*
				 *	WD:	New logic for remembering traits
				 *		moved to AddTrait() RemoveTrait() fn's
				 *
				 *		EQUIP Normal Item
				 */
				string trait = girl->m_Inventory[num]->m_Effects[i].m_Trait; // avoids repeat calls

				if (amount == 0)				// remove trait from equiping an item
				{
					//SIN: I know it's kinda player's fault anyway, but given cost of medicines and difficulty to
					//source player should get some kindness rep for this. Prob cheaper and easier to let them die
					//and replace, but if we get here, player is looking out for staff.
					if (trait == "AIDS" || trait == "Syphilis" || trait == "Herpes" || trait == "Chlamydia")
					{
						if (girl->is_slave())
						{		//SIN: just protecting investment in property
							if (trait == "AIDS") g_Game.player().evil(-4);
							if (trait == "Syphilis") g_Game.player().evil(-3);
							if (trait == "Herpes") g_Game.player().evil(-2);
							if (trait == "Chlamydia") g_Game.player().evil(-1);
						}
						else   //SIN: a genuinely kind act to support staff
						{
							if (trait == "AIDS") g_Game.player().evil(-8);
							if (trait == "Syphilis") g_Game.player().evil(-6);
							if (trait == "Herpes") g_Game.player().evil(-4);
							if (trait == "Chlamydia") g_Game.player().evil(-2);
						}
					}
					girl->remove_trait(trait, girl->m_Inventory[num]->m_Type != INVFOOD && girl->m_Inventory[num]->m_Type != INVMAKEUP);		// addrememberlist = true only if not consumable
				}
				else if (amount == 1)			// add normal trait	from equiping an item
				{
					int d =	(duration > 0 && (girl->m_Inventory[num]->m_Type == INVFOOD || girl->m_Inventory[num]->m_Type == INVMAKEUP)) ? duration : 0;
					girl->add_trait(trait, d, girl->m_Inventory[num]->m_Type != INVFOOD && girl->m_Inventory[num]->m_Type != INVMAKEUP);		// Temp = false Normal Item, removeitem = true only if not consumable
				}
				if (trait == "Virgin")
				{
					girl->m_Virgin = (amount == 1);
				}
			}
		}
	}

	// if consumable then remove from inventory
	if (girl->m_Inventory[num]->m_Type == INVFOOD || girl->m_Inventory[num]->m_Type == INVMAKEUP)
	{
		girl->m_Inventory[num] = nullptr;
		girl->m_EquipedItems[num] = 0;
		girl->m_NumInventory--;
	}
	else	// set it as equiped
		girl->m_EquipedItems[num] = 1;

	cGirls::CalculateGirlType(girl);
}

void cInventory::Unequip(sGirl* girl, int num)
{
	if (girl->m_EquipedItems[num] == 0) return;	// if already unequiped do nothing
	// unapply the effects
	for (u_int i = 0; i < girl->m_Inventory[num]->m_Effects.size(); i++)
	{
		int eff_id = girl->m_Inventory[num]->m_Effects[i].m_EffectID;
		int affects = girl->m_Inventory[num]->m_Effects[i].m_Affects;
		int amount = girl->m_Inventory[num]->m_Effects[i].m_Amount;

		/* */if (affects == sEffect::Skill)	cGirls::UpdateSkillMod(girl, eff_id, -amount);
		else if (affects == sEffect::Stat)	cGirls::UpdateStatMod(girl, eff_id, -amount);
		else if (affects == sEffect::Enjoy)	cGirls::UpdateEnjoymentMod(girl, eff_id, -amount);
		else if (affects == sEffect::GirlStatus)	// adds/removes status
		{
			if (amount == 1) girl->m_States &= ~(1 << eff_id);		// add status
			else if (amount == 0) girl->m_States |= (1 << eff_id);	// remove status
		}
		else if (affects == sEffect::Trait)	// trait
		{
			/*
			 *	WD:	New logic for remembering traits
			 *		moved to AddTrait() RemoveTrait() fn's
			 *
			 *		UNEQUIP
			 */
			if (amount == 0 && girl->m_Inventory[num]->m_Effects[i].m_Trait == "Virgin")
			{
				girl->m_Virgin = 0; // `J` unequiping an item will not make her a virgin again
			}
			else if (amount == 0)					// possibly add remembered trait from unequiping an item
				girl->add_trait(girl->m_Inventory[num]->m_Effects[i].m_Trait, false, false, true);	// inrememberlist = true Add trait only if it is in the rememebered list

			else if (amount == 1)				// remove item trait from unequiping an item
				girl->remove_trait(girl->m_Inventory[num]->m_Effects[i].m_Trait);
		}
	}
	// set it as unequiped
	girl->m_EquipedItems[num] = 0;

	cGirls::CalculateGirlType(girl);
}

void cInventory::Equip(sGirl* girl, sInventoryItem* item, bool force)
{
	// this function is only used for global effects sInventoryItem::AffectsAll = 1
	if (item->m_Special != sInventoryItem::AffectsAll) return;
	for (u_int i = 0; i < item->m_Effects.size(); i++)
	{
		int eff_id = item->m_Effects[i].m_EffectID;
		int affects = item->m_Effects[i].m_Affects;
		int amount = item->m_Effects[i].m_Amount;

		/* */if (affects == sEffect::Skill)	cGirls::UpdateSkillMod(girl, eff_id, amount);
		else if (affects == sEffect::Stat)	cGirls::UpdateStatMod(girl, eff_id, amount);
		else if (affects == sEffect::Enjoy)	cGirls::UpdateEnjoymentMod(girl, eff_id, amount);

		else if (affects == sEffect::Trait)	// trait
		{
			if (amount == 0)			// remove trait temporarily from equiping an item
				girl->remove_trait(item->m_Effects[i].m_Trait, true);	// addrememberlist = true AffectAll trait removal

			else if (amount == 1)		// add temporary trait
				girl->add_trait(item->m_Effects[i].m_Trait, 20, true); // Temp = true AffectAll Item, removeitem = true for AffectAll trait

			if (item->m_Effects[i].m_Trait == "Virgin")
			{
				girl->m_Virgin = (amount == 1);
			}
		}
	}
}

bool cInventory::equip_ring_ok(sGirl* girl, int num, bool force)
{
	return equip_limited_item_ok(girl, num, force, 8);
}

bool cInventory::equip_singleton_ok(sGirl* girl, int num, bool force)
{
	return equip_limited_item_ok(girl, num, force, 1);
}

bool cInventory::equip_pair_ok(sGirl* girl, int num, bool force)
{
	return equip_limited_item_ok(girl, num, force, 2);
}

bool cInventory::equip_limited_item_ok(sGirl* girl, int num, bool force, int limit)
{
	int count = 0;
	int target_type = girl->m_Inventory[num]->m_Type;
	/*
	 *	there's a limited number of items that can be equipped for any given type
	 *	so we're going to loop through the inventory counting the item in question
	 */
	for (int i = 0; i < 40 && count < limit; i++)
	{
		if (girl->m_Inventory[i] == nullptr ||					// if there's nothing in the slot or
			girl->m_Inventory[i]->m_Type != target_type ||	// if there's something in the slot but it's not the correct type or
			girl->m_EquipedItems[i] != 1)					// if it is the target type but she does not have it equipped,
			continue;										// Skip it

		count++;			// All right, all right. it's the right type, and it's equipped add it to the count.
		/*
		 *		now if we're not forcing her to wear this then at this point we can skip on to the next item,
		 *
		 *		for that matter, if we are forcing her, but the count is less than the limit, we might still find a free slot
		 *		so we skip on then, too
		 */
		if (!force || count < limit) continue;
		/*
		 *		If we get here, we have no free slots, and we're forcing her to wear it (Muh-ha-ha-ha-ha!)
		 *		so we need to remove this last ring, decrement the count and exit the loop
		 */
		count--;
		Unequip(girl, i);
		break;
	}
    return count != limit;
}

bool cInventory::ok_2_equip(sGirl *girl, int num, bool force)
{
	// first check if can equip it
	switch (girl->m_Inventory[num]->m_Type)
	{
	case sInventoryItem::Ring:
		if (equip_ring_ok(girl, num, force) == false) return false;
		break;
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
		if (equip_singleton_ok(girl, num, force) == false) return false;
		break;
	case sInventoryItem::SmWeapon:	// doc: added missing enum
	case sInventoryItem::Weapon:
	case sInventoryItem::Armband:
		if (equip_pair_ok(girl, num, force) == false) return false;
		break;
	case sInventoryItem::Food:
	case sInventoryItem::Makeup:
	case sInventoryItem::Misc:
	default:
		return true;
	}
	return true;
}

// ----- Load

static sInventoryItem* handle_element(TiXmlElement *el)
{
	int ival;
	const char *pt;
	sInventoryItem* item = new sInventoryItem();
	if (pt = el->Attribute("Name"))						item->m_Name = pt;
	if (pt = el->Attribute("Desc"))						item->m_Desc = pt; else cout << "no desc attribute found" << endl;
	if (pt = el->Attribute("Type"))						item->set_type(pt);
	if (pt = el->Attribute("Badness", &ival))			item->m_Badness = ival;
	else 												item->m_Badness = 0;
	if (pt = el->Attribute("GirlBuyChance", &ival))		item->m_GirlBuyChance = ival;
	else  // if not specified, set chance based on badness
		item->m_GirlBuyChance = (item->m_Badness < 20) ? (100 - (item->m_Badness * 5)) : 0;
	if (pt = el->Attribute("Special"))					item->set_special(pt);
	if (pt = el->Attribute("Cost", &ival))				item->m_Cost = ival;
	if (pt = el->Attribute("Rarity"))					item->set_rarity(pt);
	if (pt = el->Attribute("Infinite"))					item->m_Infinite = ((string(pt) == "true") || (string(pt) == "True"));
	else												item->m_Infinite = false;


	auto crafting = el->FirstChildElement("Crafting");
	if(crafting) {
        item->m_Crafting.from_xml(*crafting);
    }

	do_effects(el, item);
	//	do_tests(el, item);		//	`J` will be added in the future (hopefully)
	return item;
}

bool cInventory::LoadItemsXML(const string& filename)
{
	TiXmlDocument doc(filename);
	if (!doc.LoadFile())
	{
		g_LogFile.os() << "can't load item file " << filename << endl;
		g_LogFile.os() << "Error: line " << doc.ErrorRow() << ", col " << doc.ErrorCol() << ": " << doc.ErrorDesc() << endl;
		return false;
	}
	TiXmlElement *el;
	TiXmlElement *root_el = doc.RootElement();
	bool log_flag = cfg.debug.log_items();
	for (el = root_el->FirstChildElement(); el; el = el->NextSiblingElement())
	{
		sInventoryItem* item = handle_element(el);
		if (log_flag) g_LogFile.os() << *item << endl;
		m_Items.push_back(item);
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

int cInventory::BuyShopItem(sInventoryItem* item, int amount)
{
    auto found = std::find(begin(m_ShopItems), end(m_ShopItems), item);
    if(found == end(m_ShopItems))
        return 0;

    if ((*found)->m_Infinite == 0)
    {
        *found = nullptr;
        m_NumShopItems--;
        return 1;
    }
    return amount;
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
        case sInventoryItem::Ring:			return os << "Ring";
        case sInventoryItem::Dress:			return os << "Dress";
        case sInventoryItem::Underwear:		return os << "Underwear";
        case sInventoryItem::Shoes:			return os << "Shoes";
        case sInventoryItem::Food:			return os << "Food";
        case sInventoryItem::Necklace:		return os << "Necklace";
        case sInventoryItem::Weapon:		return os << "Weapon";
        case sInventoryItem::SmWeapon:		return os << "Small Weapon";
        case sInventoryItem::Makeup:		return os << "Makeup";
        case sInventoryItem::Armor:			return os << "Armor";
        case sInventoryItem::Misc:			return os << "Misc";
        case sInventoryItem::Armband:		return os << "Armband";
        case sInventoryItem::Hat:			return os << "Hat";
        case sInventoryItem::Helmet:		return os << "Helmet";
        case sInventoryItem::Glasses:		return os << "Glasses";
        case sInventoryItem::Swimsuit:		return os << "Swimsuit";
        case sInventoryItem::Combatshoes:	return os << "Combat Shoes";
        case sInventoryItem::Shield:		return os << "Shield";
        default:
            cerr << "Unexpected type value: " << int(typ) << endl;
            return os << "Error";
    }
}

ostream& operator<<(ostream& os, sInventoryItem::Rarity& r) {
    switch(r) {
        case sInventoryItem::Common:			return os << "Common";
        case sInventoryItem::Shop50:			return os << "Shops, 50%";
        case sInventoryItem::Shop25:			return os << "Shops, 25%";
        case sInventoryItem::Shop05:			return os << "Shops, 05%";
        case sInventoryItem::Catacomb15:		return os << "Catacombs, 15%";
        case sInventoryItem::Catacomb05:		return os << "Catacombs, 05%";
        case sInventoryItem::Catacomb01:		return os << "Catacombs, 01%";
        case sInventoryItem::ScriptOnly:		return os << "Scripted Only";
        case sInventoryItem::ScriptOrReward:	return os << "Scripts or Reward";
        default:	cerr << "error: unexpected rarity value: " << int(r) << endl;
            return os << "Error(" << int(r) << ")";
    }
}

ostream& operator<<(ostream& os, sInventoryItem::Special& spec) {
    switch(spec) {
        case sInventoryItem::None:				return os << "None";
        case sInventoryItem::AffectsAll:		return os << "AffectsAll";
        case sInventoryItem::Temporary:			return os << "Temporary";
        default:	cerr << "error: unexpected special value: " << int(spec) << endl;
            return os << "Error(" << int(spec) << ")";
    }
}

void sInventoryItem::set_type(const string& s) {
    if      (s == "Ring")							{ m_Type = Ring; }
    else if (s == "Dress")							{ m_Type = Dress; }
    else if (s == "Under Wear" || s == "Underwear")	{ m_Type = Underwear; }
    else if (s == "Shoes")							{ m_Type = Shoes; }
    else if (s == "Food")							{ m_Type = Food; }
    else if (s == "Necklace")						{ m_Type = Necklace; }
    else if (s == "Weapon")							{ m_Type = Weapon; }
    else if (s == "Small Weapon")					{ m_Type = SmWeapon; }
    else if (s == "Makeup")							{ m_Type = Makeup; }
    else if (s == "Armor")							{ m_Type = Armor; }
    else if (s == "Misc")							{ m_Type = Misc; }
    else if (s == "Armband")						{ m_Type = Armband; }
    else if (s == "Hat")							{ m_Type = Hat; }
    else if (s == "Glasses")						{ m_Type = Glasses; }
    else if (s == "Swimsuit")						{ m_Type = Swimsuit; }
    else if (s == "Helmet")							{ m_Type = Helmet; }
    else if (s == "Shield")							{ m_Type = Shield; }
    else if (s == "Combat Shoes")					{ m_Type = Combatshoes; }
    else if (s == "CombatShoes")					{ m_Type = Combatshoes; }
    else { m_Type = Misc; cerr << "Error: unexpected item type: " << s << endl; }
}

void sInventoryItem::set_special(const string& s) {
    if      (s == "None")			{ m_Special = None; }
    else if (s == "AffectsAll")		{ m_Special = AffectsAll; }
    else if (s == "Temporary")		{ m_Special = Temporary; }
    else { m_Special = None; cerr << "unexpected special string: '" << s << "'" << endl; }
}

void sInventoryItem::set_rarity(const string& s) {
    if      (s == "Common")			{ m_Rarity = Common; }
    else if (s == "Shop50")			{ m_Rarity = Shop50; }
    else if (s == "Shop25")			{ m_Rarity = Shop25; }
    else if (s == "Shop05")			{ m_Rarity = Shop05; }
    else if (s == "Catacomb15")		{ m_Rarity = Catacomb15; }
    else if (s == "Catacomb05")		{ m_Rarity = Catacomb05; }
    else if (s == "Catacomb01")		{ m_Rarity = Catacomb01; }
    else if (s == "ScriptOnly")		{ m_Rarity = ScriptOnly; }
    else if (s == "ScriptOrReward") { m_Rarity = ScriptOrReward; }
    else { cerr << "Error in set_rarity: unexpected value '" << s << "'" << endl; m_Rarity = Shop05; }	// what to do?
}

void CraftingData::from_xml(TiXmlElement& element) {
    element.QueryIntAttribute("CraftPoints", &m_CraftPoints);
    element.QueryIntAttribute("ManaCost", &m_ManaCost);
    element.QueryIntAttribute("Weight", &m_Weight);

    std::cout << m_CraftPoints << "\n";

    for(auto job = element.FirstChildElement("Job"); job; job = job->NextSiblingElement("Job")) {
        auto job_name = job->Attribute("Name");
        std::cout << "job: " << job_name << "\n";
        if(job_name) {
            auto job_id = sGirl::lookup_jobs_code(job_name);
            if(job_id != -1) {
                m_CraftableBy.insert((JOBS)job_id);
            }
        } else {
            std::cerr << "No name given for <Job> tag.\n";
        }
    }

    for(auto skill = element.FirstChildElement("Skill"); skill; skill = skill->NextSiblingElement("Skill")) {
        auto skill_name = skill->Attribute("Name");
        if(skill_name) {
            int skill_value = 0;
            auto skill_id = sGirl::lookup_skill_code(skill_name);
            skill->QueryIntAttribute("Minimum", &skill_value);
            if(skill_id != -1) {
                m_SkillRequirements[(SKILLS)skill_id] = skill_value;
            }
        }
    }

    for(auto stat = element.FirstChildElement("Stat"); stat; stat = stat->NextSiblingElement("Stat")) {
        auto stat_name = stat->Attribute("Name");
        if(stat_name) {
            int stat_value = 0;
            auto stat_id = sGirl::lookup_stat_code(stat_name);
            stat->QueryIntAttribute("Minimum", &stat_value);
            if(stat_id != -1) {
                m_StatsRequirements[(STATS)stat_id] = stat_value;
            }
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

    std::cout << " Possible Item. Checking Requirements\n";
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

int sCondition::sSkillSource::get(const sGirl& girl)
{
    return girl.get_skill(skill);
}

int sCondition::sStatSource::get(const sGirl& girl)
{
    return girl.get_stat(stat);
}

bool sCondition::check(const sGirl& girl) const
{
    if(m_Comparison == Cmp::LOWER) {
        return m_ValueSource->get(girl) <= m_ReferenceValue;
    } else {
        return m_ValueSource->get(girl) >= m_ReferenceValue;
    }
}
/*
void sCondition::from_xml(TiXmlElement& element)
{
    const char* pt = nullptr;
    if( element.Attribute("Minimum", &m_ReferenceValue)) {
        m_Comparison = Cmp::LOWER;
    } else if( (pt = element.Attribute("Maximum", &m_ReferenceValue))) {
        m_Comparison = Cmp::GREATER;
    } else {
        // Error. One of `Minimum`, `Maximum` needs to be specified
    }

    const char* type = element.Attribute("Type");
    if(!type) {
        // Error
    }

    std::string type_str = type;
    if( type_str == "Stat" ) {
        m_ValueSource = std::unique_ptr<sSource>( new );
    }
    element.QueryIntAttribute("CraftPoints", &m_ReferenceValue);
    element.QueryIntAttribute("ManaCost", &m_ManaCost);
    element.QueryIntAttribute("Weight", &m_Weight);
}
*/